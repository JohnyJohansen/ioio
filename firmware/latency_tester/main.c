/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

#include <stdint.h>

#include "Compiler.h"
#include "libconn/connection.h"
#include "common/logging.h"
#include "latency.h"

// define in non-const arrays to ensure data space
static char descManufacturer[] = "IOIO Open Source Project";
static char descModel[] = "IOIO";
static char descDesc[] = "IOIO Latency Tester";
static char descVersion[] = "IOIO0300";
static char descUri[] = "https://github.com/ytai/ioio/wiki/ADK";
static char descSerial[] = "N/A";

const char* accessoryDescs[6] = {
  descManufacturer,
  descModel,
  descDesc,
  descVersion,
  descUri,
  descSerial
};

typedef enum {
  STATE_INIT,
  STATE_OPEN_CHANNEL,
  STATE_WAIT_CHANNEL_OPEN,
  STATE_CONNECTED,
  STATE_ERROR
} STATE;

static STATE state = STATE_INIT;
static CHANNEL_HANDLE handle;

static void AppCallback(const void* data, UINT32 data_len, int_or_ptr_t arg);

static inline CHANNEL_HANDLE OpenAvailableChannel() {
  int_or_ptr_t arg = { .i = 0 };
  if (ConnectionTypeSupported(CHANNEL_TYPE_ADB)) {
    if (ConnectionCanOpenChannel(CHANNEL_TYPE_ADB)) {
      return ConnectionOpenChannelAdb("tcp:4545", &AppCallback, arg);
    }
  } else if (ConnectionTypeSupported(CHANNEL_TYPE_ACC)) {
    if (ConnectionCanOpenChannel(CHANNEL_TYPE_ACC)) {
      return ConnectionOpenChannelAccessory(&AppCallback, arg);
    }
  } else if (ConnectionTypeSupported(CHANNEL_TYPE_BT)) {
    if (ConnectionCanOpenChannel(CHANNEL_TYPE_BT)) {
      return ConnectionOpenChannelBtServer(&AppCallback, arg);
    }
  } else if (ConnectionTypeSupported(CHANNEL_TYPE_CDC_DEVICE)) {
    if (ConnectionCanOpenChannel(CHANNEL_TYPE_CDC_DEVICE)) {
      return ConnectionOpenChannelCdc(&AppCallback, arg);
    }
  }
  return INVALID_CHANNEL_HANDLE;
}

void AppCallback(const void* data, UINT32 data_len, int_or_ptr_t arg) {
  if (data) {
    if (!LatencyHandleIncoming(data, data_len)) {
      // got corrupt input. need to close the connection and soft reset.
      log_printf("Protocol error");
      state = STATE_ERROR;
    }
  } else {
    // connection closed, soft reset and re-establish
    if (state == STATE_CONNECTED) {
      log_printf("Channel closed");
    } else {
      log_printf("Channel failed to open");
    }
    state = STATE_OPEN_CHANNEL;
  }
}

int main() {
  log_init();
  log_printf("***** Hello from latency tester! *******");
  TRISFbits.TRISF3 = 1;

  ConnectionInit();
  while (1) {
    ConnectionTasks();
    switch (state) {
      case STATE_INIT:
        handle = INVALID_CHANNEL_HANDLE;
        state = STATE_OPEN_CHANNEL;
        break;

      case STATE_OPEN_CHANNEL:
        if ((handle = OpenAvailableChannel()) != INVALID_CHANNEL_HANDLE) {
          log_printf("Connected");
          state = STATE_WAIT_CHANNEL_OPEN;
        }
        break;

      case STATE_WAIT_CHANNEL_OPEN:
       if (ConnectionCanSend(handle)) {
          log_printf("Channel open");
          LatencyInit(handle);
          state = STATE_CONNECTED;
        }
        break;

      case STATE_CONNECTED:
        LatencyTasks(handle);
        break;

      case STATE_ERROR:
        ConnectionCloseChannel(handle);
        state = STATE_INIT;
        break;
    }
  }
  return 0;
}
