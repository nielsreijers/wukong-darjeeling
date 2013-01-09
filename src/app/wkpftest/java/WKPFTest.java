import javax.wukong.WKPF;
import javax.wukong.VirtualWuObject;

public class WKPFTest
{
	private static class VirtualTestWuClass extends VirtualWuObject
	{
		public static final byte[] properties = new byte[]
		{
			42, 43, 44, 45, 46
		};

		public void update() {}
	}

	public static void main(String args[])
	{
		System.out.println("Hello, world!\n\r");
		System.out.println("WKPF " + WKPF.getErrorCode());
		WKPF.registerWuClass((short)0x42, VirtualTestWuClass.properties);
		System.out.println("WKPF " + WKPF.getErrorCode());
	}
}
