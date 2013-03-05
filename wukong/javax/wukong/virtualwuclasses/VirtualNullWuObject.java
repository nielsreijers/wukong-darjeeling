package javax.wukong.virtualwuclasses;

import javax.wukong.wkpf.WKPF;
import javax.wukong.wkpf.VirtualWuObject;

public class VirtualNullWuObject extends GENERATEDVirtualNullWuObject {
    public void update() {
        // Force property to be sent
        short value = WKPF.getPropertyShort(this, NULL);
        value += 1;
        WKPF.setPropertyShort(this, NULL, (short)value);
    }
}
