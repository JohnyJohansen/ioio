package ioio.lib;

/**
 * Interface to the IOIO board.
 * 
 * @author arshan
 */
public interface IOIOApi {
	// Magic bytes for the IOIO, spells 'IOIO'
	public final byte[] IOIO_MAGIC = { 0x49,  0x4F, 0x49, 0x4F };

	// Outgoing messages
	public static final int HARD_RESET 	= 0x00;
	public static final int SOFT_RESET 	= 0x01;
	public static final int SET_OUTPUT 	= 0x02;
	public static final int SET_VALUE 		= 0x03;
	public static final int SET_INPUT 		= 0x04;
	public static final int SET_CHANGE_NOTIFY = 0x05;
	public static final int SET_PERIODIC_SAMPLE = 0x06;
	public static final int RESERVED1 = 0x07;
	public static final int SET_PWM = 0x08;
	public static final int SET_DUTYCYCLE = 0x09;
	public static final int SET_PERIOD = 0x0A;
	public static final int SET_ANALOG_INPUT = 0x0B;
	public static final int UART_TX = 0x0C;
	public static final int UART_CONFIGURE = 0x0D;
	public static final int UART_SET_RX = 0x0E;
	public static final int UART_SET_TX = 0x0F;
	
	// Incoming messages (where different)
	public static final int ESTABLISH_CONNECTION = 0x00;
	public static final int REPORT_DIGITAL_STATUS = 0x03;
	public static final int REPORT_PERIODIC_DIGITAL = 0x07;
	public static final int REPORT_ANALOG_FORMAT = 0x08;
	public static final int REPORT_ANALOG_STATUS = 0x09;
	public static final int UART_TX_STATUS = 0x0A;
	public static final int UART_RX = 0x0C;
	
	public void connect();
	
	public void disconnect();

	public void softReset();
	
	public void hardReset();

	public DigitalInput openDigitalInput(int pin);
	
	public DigitalOutput openDigitalOutput(int pin);
	
	public AnalogInput openAnalogInput(int pin);

	// ytai: nit: camel case convention recommends getPwmOutput (i.e. don't capitalize the entire acronym).
	// same for UART. but I don't really care that much...
	public PwmOutput openPwmOutput(int pin);
	
	public Uart openUart(int rx, int tx, int baud, int parity, int stopbits);
	
}
