#ifndef WKPF_LINKSH
#define WKPF_LINKSH

#include "wkcomm.h"

extern uint8_t wkpf_load_component_to_wuobject_map(void *map);
extern uint8_t wkpf_load_links(void *links);
extern bool wkpf_does_property_need_initialisation_pull(uint8_t port_number, uint8_t property_number);
extern uint8_t wkpf_propagate_dirty_properties();
extern uint8_t wkpf_get_node_and_port_for_component(uint16_t component_id, address_t *node_id, uint8_t *port_number);

extern bool wkpf_node_is_leader(uint16_t component_id, address_t node_id);

#endif // WKPF_LINKSH
