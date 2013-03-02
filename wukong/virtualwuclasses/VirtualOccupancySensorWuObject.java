package javax.wukong.wkpf.wuclasses;

import javax.wukong.wkpf.WKPF;
import javax.wukong.wkpf.VirtualWuObject;

public class VirtualOccupancySensorWuObject extends GENERATEDVirtualOccupancySensorWuObject {
    public void update() {
      System.out.println("WKPFUPDATE(OccupancySensor): NOP");
      return;
    }
}
