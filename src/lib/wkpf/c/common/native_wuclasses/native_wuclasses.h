#ifndef NATIVE_WUCLASSESH
#define NATIVE_WUCLASSESH

#include "types.h"
#include "wkpf.h"
#include "wkpf_wuclasses.h"
#include "wkpf_wuobjects.h"
#include "wkpf_properties.h"
#include "GENERATEDwkpf_wuclass_library.h"

#define ENABLE_WUCLASS_GENERIC
#define ENABLE_WUCLASS_THRESHOLD
#define ENABLE_WUCLASS_NUMERIC_CONTROLLER
#define ENABLE_WUCLASS_LIGHT_ACTUATOR
#define ENABLE_WUCLASS_LIGHT_SENSOR
#define ENABLE_WUCLASS_LOGICAL
#define ENABLE_WUCLASS_MATH_OP
#define ENABLE_WUCLASS_LOOP_DELAY_BOOLEAN
#define ENABLE_WUCLASS_LOOP_DELAY_SHORT
#define ENABLE_WUCLASS_CONDITION_SELECTOR_BOOLEAN
#define ENABLE_WUCLASS_CONDITION_SELECTOR_SHORT

uint8_t wkpf_native_wuclasses_init();

#endif // NATIVE_WUCLASSESH

