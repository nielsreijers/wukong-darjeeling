import javax.wukong.wkpf.*;
import javax.wukong.virtualwuclasses.*;

public class WKDeploy {
    /* Component names to indexes:
    {%- for component in changesets.components %}
    {{ component.type }} => {{ component.index }}
    {%- endfor %}
    */

    public static void main (String[] args) {
        // Application: {{ name }}
        System.out.println("My node id: " + WKPF.getMyNodeId());
        WKPF.appInitLinkTableAndComponentMap();
        initialiseVirtualWuObjects();
        WKPF.appInitLocalObjectAndInitValues();

        while(true){
            VirtualWuObject wuobject = WKPF.select();
            if (wuobject != null) {
                wuobject.update();
            }
        }
    }

    private static void initialiseVirtualWuObjects() {
        //all WuClasses from the same group has the same instanceIndex and wuclass
        {%- for component in changesets.components %}
            {% for wuobject in component.instances %}
                {% if not wuobject.hasLocalWuClass %}
                    if (WKPF.getMyNodeId() == (short){{ wuobject.node_id }}) {
                        // Virtual WuClasses (Java)
                        VirtualWuObject wuclassInstance{{ wuobject.wuclass|wuclassname }} = new {{ wuobject.wuclass|wuclassvirtualclassname }}();
                        WKPF.registerWuClass(GENERATEDWKPF.{{ wuobject.wuclass|wuclassconstname }}, {{ wuobject.wuclass|wuclassgenclassname }}.properties);
                        WKPF.createWuObject((short)GENERATEDWKPF.{{ wuobject.wuclass|wuclassconstname }}, WKPF.getPortNumberForComponent((short){{ component.index }}), wuclassInstance{{ wuobject.wuclass|wuclassname }});
                    }
                {% endif %}
            {% endfor %}
        {%- endfor %}
    }
}
