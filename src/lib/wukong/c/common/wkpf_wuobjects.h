#ifndef WKPF_WUOBJECTSH
#define WKPF_WUOBJECTSH

#include "types.h"
#include "wkpf_wuclasses.h"

// TODONR: only works if heap id 0 isn't used.
#define WKPF_IS_NATIVE_WUOBJECT(x)               (x->java_instance_reference == NULL)
#define WKPF_IS_VIRTUAL_WUOBJECT(x)              (x->java_instance_reference != NULL)

typedef struct wkpf_local_wuobject {
    wkpf_wuclass_definition *wuclass;
    uint8_t port_number;
    dj_object* java_instance_reference; // Set for virtual wuclasses, NULL for native wuclasses
    dj_time_t next_scheduled_update; // TODONR: include this in the refresh rate property when I have a better implementation of the property store
    bool need_to_call_update;
    struct wkpf_local_wuobject *next;
    uint8_t properties[];
} wkpf_local_wuobject;

extern uint8_t wkpf_create_wuobject(uint16_t wuclass_id, uint8_t port_number, dj_object *java_instance_reference /* TODO: find out what datatype to use */ );
extern uint8_t wkpf_remove_wuobject(uint8_t port_number);
extern uint8_t wkpf_get_wuobject_by_port(uint8_t port_number, wkpf_local_wuobject **wuobject);
extern uint8_t wkpf_get_wuobject_by_index(uint8_t index, wkpf_local_wuobject **wuobject);
extern uint8_t wkpf_get_wuobject_by_java_instance_reference(dj_object *java_instance_reference, wkpf_local_wuobject **wuobject);
extern uint8_t wkpf_get_number_of_wuobjects();
extern void wkpf_set_need_to_call_update_for_wuobject(wkpf_local_wuobject *wuobject);
extern bool wkpf_get_next_wuobject_to_update(wkpf_local_wuobject **wuobject);
extern void wkpf_schedule_next_update_for_wuobject(wkpf_local_wuobject *wuobject);

#endif // WKPF_WUOBJECTSH
