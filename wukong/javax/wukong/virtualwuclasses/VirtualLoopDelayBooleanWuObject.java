package javax.wukong.virtualwuclasses;

import javax.wukong.wkpf.WKPF;
import javax.wukong.wkpf.VirtualWuObject;


public class VirtualLoopDelayBooleanWuObject extends GENERATEDVirtualLoopDelayBooleanWuObject {
    private short delay_count_boolean;
    private boolean over;

    public VirtualLoopDelayBooleanWuObject(){
        delay_count_boolean=0;
	over=false;
    }

    public void update() {
        // TODONR: replace these calls with convenience methods in VirtualWuObject once we get the inheritance issue sorted out.
        boolean input = WKPF.getPropertyBoolean(this, INPUT);
        short delay = WKPF.getPropertyShort(this, DELAY);

        if (delay_count_boolean>=delay && over==false) {
	  over=true;
	  WKPF.setPropertyBoolean(this, OUTPUT, input);
          System.out.println("WKPFUPDATE(loop_delay): Native loop_delay: write " + (input?"true":"false") + " to output \n");
        }
        else if(over==false) {
	  delay_count_boolean++;
          System.out.println("WKPFUPDATE(loop_delay): Native loop_delay: delay" + delay + ", now count to " + delay_count_boolean +"\n");
        }

    }
}
