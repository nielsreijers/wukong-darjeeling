#ifndef WKPF_WUOBJECTSH
#define WKPF_WUOBJECTSH

#include "types.h"
#include "wkpf_wuclasses.h"

// TODONR: only works if heap id 0 isn't used.
#define WKPF_IS_NATIVE_WUOBJECT(x)               (x->java_instance_reference == NULL)
#define WKPF_IS_VIRTUAL_WUOBJECT(x)              (x->java_instance_reference != NULL)

typedef struct wuobject_t {
    wuclass_t *wuclass;
    uint8_t port_number;
    dj_object* java_instance_reference; // Set for virtual wuclasses, NULL for native wuclasses
    dj_time_t next_scheduled_update; // TODONR: include this in the refresh rate property when I have a better implementation of the property store
    bool need_to_call_update;
    struct wuobject_t *next;
    uint8_t properties_store[];
} wuobject_t;

typedef struct wuobject_property_t {
	uint8_t property_status;
	uint8_t value[];
} wuobject_property_t;


extern uint8_t wkpf_create_wuobject(uint16_t wuclass_id, uint8_t port_number, dj_object *java_instance_reference /* TODO: find out what datatype to use */ );
extern uint8_t wkpf_remove_wuobject(uint8_t port_number);
extern uint8_t wkpf_get_wuobject_by_port(uint8_t port_number, wuobject_t **wuobject);
extern uint8_t wkpf_get_wuobject_by_index(uint8_t index, wuobject_t **wuobject);
extern uint8_t wkpf_get_wuobject_by_java_instance_reference(dj_object *java_instance_reference, wuobject_t **wuobject);
extern uint8_t wkpf_get_number_of_wuobjects();
extern void wkpf_set_need_to_call_update_for_wuobject(wuobject_t *wuobject);
extern bool wkpf_get_next_wuobject_to_update(wuobject_t **wuobject);
extern void wkpf_schedule_next_update_for_wuobject(wuobject_t *wuobject);

// Access to the properties
extern wuobject_property_t* wkpf_get_property(wuobject_t *wuobject, uint8_t property_number);


#endif // WKPF_WUOBJECTSH
