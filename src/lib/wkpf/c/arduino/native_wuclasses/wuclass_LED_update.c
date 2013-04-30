#include "debug.h"
#include "../../common/native_wuclasses/native_wuclasses.h"
#include <avr/io.h>
#include "../../../../vm/c/common/execution.h"


#define output_low(port, pin) port &= ~(1<<pin)
#define output_high(port, pin) port |= (1<<pin)
#define set_input(portdir, pin) portdir &= ~(1<<pin)
#define set_output(portdir, pin) portdir |= (1<<pin)

#ifdef ENABLE_WUCLASS_LED

void wuclass_led_setup(wuobject_t *wuobject) {

}

void wuclass_led_update(wuobject_t *wuobject) {
    bool p1, p2, p3, p4;
    DEBUG_LOG(DBG_WKCOMM, "WKCOMMUPDATE: led\n");

    set_output(DDRK, 0);
    set_output(DDRK, 1);
    set_output(DDRK, 2);
    set_output(DDRK, 3);

    //output_low(PORTK, 0);
    //output_low(PORTK, 1);
    //output_low(PORTK, 2);
    //output_low(PORTK, 3);

    wkpf_internal_read_property_boolean(wuobject, WKPF_PROPERTY_LED_PORT1, &p1);
    wkpf_internal_read_property_boolean(wuobject, WKPF_PROPERTY_LED_PORT2, &p2);
    wkpf_internal_read_property_boolean(wuobject, WKPF_PROPERTY_LED_PORT3, &p3);
    wkpf_internal_read_property_boolean(wuobject, WKPF_PROPERTY_LED_PORT4, &p4);

    if(p1==true)
        output_low(PORTK, 0);
    else
        output_high(PORTK, 0);
    if(p2==true)
        output_low(PORTK, 1);
    else
        output_high(PORTK, 1);
    if(p3==true)
        output_low(PORTK, 2);
    else
        output_high(PORTK, 2);
    if(p4==true)
        output_low(PORTK, 3);
    else
        output_high(PORTK, 3);
}

#endif // ENABLE_WUCLASS_LED
