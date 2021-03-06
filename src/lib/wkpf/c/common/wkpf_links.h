#ifndef WKPF_LINKSH
#define WKPF_LINKSH

#include "wkcomm.h"
#include "array.h"

typedef struct
__attribute__ ((__packed__)) // This isn't optional through the PACK_STRUCT option like other struct since the current wkpf_load_links won't work with an unpacked struct.
wkpf_endpoint_t {
  address_t node_id;
  uint8_t port_number;  
} wkpf_endpoint_t;

typedef struct
__attribute__ ((__packed__)) // This isn't optional through the PACK_STRUCT option like other struct since the current wkpf_load_links won't work with an unpacked struct.
wkpf_link_t {
	uint16_t src_component_id;
	uint8_t src_property_number;
	uint16_t dest_component_id;  
	uint8_t dest_property_number;
	uint16_t dest_wuclass_id; // This is only here because there is an extra check on wuclass_id when remotely setting properties, but actually that's not strictly necessary. Not sure if it's worth the extra memory, but if we store this in flash it might be ok.
} wkpf_link_t;

extern uint8_t wkpf_load_component_to_wuobject_map(dj_ref_array *map);
extern uint8_t wkpf_load_links(dj_int_array *links);
extern bool wkpf_does_property_need_initialisation_pull(uint8_t port_number, uint8_t property_number);
extern uint8_t wkpf_propagate_dirty_properties();
extern uint8_t wkpf_get_node_and_port_for_component(uint16_t component_id, address_t *node_id, uint8_t *port_number);
// extern bool wkpf_get_component_id(uint8_t port_number, uint16_t *component_id);
// extern uint8_t wkpf_get_link_by_dest_property_and_dest_wuclass_id(uint8_t property_number, uint16_t wuclass_id, link_entry *entry);

extern bool wkpf_node_is_leader(uint16_t component_id, address_t node_id);
extern wkpf_endpoint_t wkpf_leader_for_component(uint16_t component_id);
// extern uint8_t wkpf_local_endpoint_for_component(uint16_t component_id, remote_endpoint* endpoint);

#endif // WKPF_LINKSH
