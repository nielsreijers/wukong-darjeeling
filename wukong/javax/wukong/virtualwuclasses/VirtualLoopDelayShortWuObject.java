package javax.wukong.virtualwuclasses;

import javax.wukong.wkpf.WKPF;
import javax.wukong.wkpf.VirtualWuObject;


public class VirtualLoopDelayShortWuObject extends GENERATEDVirtualLoopDelayShortWuObject {
    private short delay_count_short;
    private boolean over;

    public VirtualLoopDelayShortWuObject(){
        delay_count_short=0;
	over=false;
    }

    public void update() {
        // TODONR: replace these calls with convenience methods in VirtualWuObject once we get the inheritance issue sorted out.
        short input = WKPF.getPropertyShort(this, INPUT);
        short delay = WKPF.getPropertyShort(this, DELAY);

        if (delay_count_short>=delay && over==false) {
	  over=true;
	  WKPF.setPropertyShort(this, OUTPUT, input);
          System.out.println("WKPFUPDATE(loop_delay): Native loop_delay: write" + input + "to output \n");
        }
        else if(over==false) {
	  delay_count_short++;
          System.out.println("WKPFUPDATE(loop_delay): Native loop_delay: delay" + delay + ", now count to " + delay_count_short +"\n");
        }

    }
}
