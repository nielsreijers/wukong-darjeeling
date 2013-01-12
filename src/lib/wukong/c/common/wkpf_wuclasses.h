#ifndef WKPF_WUCLASSESH
#define WKPF_WUCLASSESH

#include "heap.h"
#include "types.h"

#define WKPF_IS_NATIVE_WUCLASS(x)               (x->update != NULL)
#define WKPF_IS_VIRTUAL_WUCLASS(x)              (x->update == NULL)

struct wkpf_local_wuobject;
typedef void (*update_function_t)(struct wkpf_local_wuobject *);

typedef struct wkpf_wuclass_definition {
    uint16_t wuclass_id;
    update_function_t update; // Set for native wuclasses, NULL for virtual wuclasses
    uint8_t number_of_properties;
    struct wkpf_wuclass_definition *next;
    uint8_t properties[];
} wkpf_wuclass_definition;

extern uint8_t wkpf_register_wuclass(uint16_t wuclass_id, update_function_t update, uint8_t number_of_properties, uint8_t properties[]);
extern uint8_t wkpf_get_wuclass_by_id(uint16_t wuclass_id, wkpf_wuclass_definition **wuclass);
extern uint8_t wkpf_get_wuclass_by_index(uint8_t index, wkpf_wuclass_definition **wuclass);
extern uint8_t wkpf_get_number_of_wuclasses();

#endif // WKPF_WUCLASSESH
