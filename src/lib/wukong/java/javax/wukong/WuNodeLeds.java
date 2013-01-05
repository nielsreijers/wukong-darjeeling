package javax.wukong;

public class WuNodeLeds {
	public static final byte LED1 = 0;
	public static final byte LED2 = 0;
	public static final byte LED3 = 0;
	public static final byte LED4 = 0;

	static {
		init();
	}

    public static native void setLed(int led, boolean state);

    private static native void init();
}
