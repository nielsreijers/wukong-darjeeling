#include "debug.h"
#include "../../common/native_wuclasses/native_wuclasses.h"
#include <stdio.h>
#include "GENERATEDwuclass_light_actuator.h"


#ifdef ENABLE_WUCLASS_LIGHT_ACTUATOR

void wuclass_light_actuator_update(wuobject_t *wuobject) {
	bool onOff;
	wkpf_internal_read_property_boolean(wuobject, WKPF_PROPERTY_LIGHT_ACTUATOR_ON_OFF, &onOff);


	printf("WKPFUPDATE(Light): Setting light to: %x\n", onOff);

	lightactuator_switch_count_t *switch_count = wuclass_light_actuator_getPrivateData(wuobject);
	(*switch_count)++;
	printf("WKPFUPDATE(Light): Switched %d times.\n", *switch_count);	
}

#endif // ENABLE_WUCLASS_LIGHT_ACTUATOR
