#include "debug.h"
#include "../../common/native_wuclasses/native_wuclasses.h"

#ifdef ENABLE_WUCLASS_LIGHT_SENSOR

void wuclass_light_sensor_setup(wuobject_t *wuobject) {}

void wuclass_light_sensor_update(wuobject_t *wuobject) {
  // Just return a value
  wkpf_internal_write_property_int16(wuobject, WKPF_PROPERTY_LIGHT_SENSOR_CURRENT_VALUE, 42);
}

#endif // ENABLE_WUCLASS_LIGHT_SENSOR
