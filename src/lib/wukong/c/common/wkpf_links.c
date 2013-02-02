// #include "config.h"
// #include "types.h"
#include "debug.h"
// #include "nvmcomm.h"
// #include "heap.h"
#include "array.h"
#include "wkcomm.h"
#include "wkpf.h"
// #include "group.h"
#include "wkpf_wuobjects.h"
#include "wkpf_properties.h"
#include "wkpf_comm.h"
#include "wkpf_links.h"

dj_int_array *wkpf_links_store = NULL;
dj_ref_array *wkpf_component_map_store = NULL;

// For now, we'll just use the Java array objects to store the link and component tables.
// But through the rest of the code, after setting wkpf_links_store and wkpf_component_map_store,
// use these constants so we can easily change that implementation later if we want to.
typedef dj_int_array wkpf_component_t;

#define wkpf_get_link(i) 								(&(((wkpf_link_t *)(wkpf_links_store->data.bytes))[i]))
#define wkpf_get_component(i) 							((wkpf_component_t *)REF_TO_VOIDP((wkpf_component_map_store->refs)[i]))
//#define wkpf_get_endpoint_for_component(component, i)	(&(((wkpf_endpoint_t *)(component->data.bytes))[i]))

#define wkpf_number_of_links							(wkpf_links_store ? ((wkpf_links_store->array.length)/sizeof(wkpf_link_t)) : 0)
#define wkpf_number_of_components						(wkpf_component_map_store ? ((wkpf_component_map_store->array.length)) : 0)
#define wkpf_number_of_endpoints(component)				((component->array.length)/sizeof(wkpf_endpoint_t))

// Original this was a #define, but the compiler complains about strict aliasing on AVR.
static inline wkpf_endpoint_t* wkpf_get_endpoint_for_component(wkpf_component_t *component, uint16_t i) {
	wkpf_endpoint_t *endpoints = (wkpf_endpoint_t *)component->data.bytes;
	return &(endpoints[i]);
}

bool wkpf_get_component_id(uint8_t port_number, uint16_t *component_id) {
	for(int i=0; i<wkpf_number_of_components; i++) {
		wkpf_component_t *component = wkpf_get_component(i);
		uint16_t number_of_endpoints = wkpf_number_of_endpoints(component);
		for(int j=0; j<number_of_endpoints; j++) {
			wkpf_endpoint_t *endpoint = wkpf_get_endpoint_for_component(component, j);
			if(endpoint->node_id == nvmcomm_get_node_id()
					&& endpoint->port_number == port_number) {
				*component_id = i;
				return true; // Found
			}
		}
	}
	return false; // Not found. Could happen for wuobjects that aren't used in the application (unused sensors, actuators, etc).
}

// uint8_t wkpf_get_link_by_dest_property_and_dest_wuclass_id(uint8_t property_number, uint16_t wuclass_id, wkpf_link_t *entry) {
//   for (int i=0; i<number_of_links; i++) {
//     if (links[i].dest_property_number == property_number && links[i].dest_wuclass_id == wuclass_id) {
//       *entry = links[i];
//       return WKPF_OK;
//     }
//   }
//   return WKPF_ERR_LINK_NOT_FOUND;
// }

uint8_t wkpf_load_component_to_wuobject_map(dj_ref_array *map) {
	wkpf_component_map_store = map;
	// After storing the reference, only use the constants defined above to access it so that we may change the storage implementation later
	DEBUG_LOG(DBG_WKPF, "WKPF: Registering %x components\n", wkpf_number_of_components);
#ifdef DARJEELING_DEBUG
	for (int i=0; i<wkpf_number_of_components; i++) {
		DEBUG_LOG(DBG_WKPF, "WKPF: Component %d -> ", i);
		wkpf_component_t *component = wkpf_get_component(i);
		for (int j=0; j<wkpf_number_of_endpoints(component); j++) {
			wkpf_endpoint_t *endpoint = wkpf_get_endpoint_for_component(component, j);
			DEBUG_LOG(DBG_WKPF, "  (node %d, port %d)", endpoint->node_id, endpoint->port_number);
		}
		DEBUG_LOG(DBG_WKPF, "\n");
	}
#endif // DARJEELING_DEBUG

// TODONR: nieuwe constante bedenken en implementatie van group_add_node_to_watch en nvmcomm_get_node_id
#ifdef NVM_USE_GROUP
	for (int i=0; i<wkpf_number_of_components; i++) {
		wkpf_component_t *component = wkpf_get_component(i);
		for (int j=0; j<wkpf_number_of_endpoints(component); j++) {
			wkpf_endpoint_t *endpoint = wkpf_get_endpoint_for_component(component, j);
			if (endpoint->node_id == nvmcomm_get_node_id()) {
				if (j == 0) {
					// I'm the leader, so watch everyone
					for (int k=1; k<wkpf_number_of_endpoints(component); k++)
						group_add_node_to_watch(wkpf_get_endpoint_for_component(component, k)->node_id);
				} else {
					// Just watch the leader
					group_add_node_to_watch(wkpf_get_endpoint_for_component(component, 0)->node_id);
				}
			}
		}
	}
#endif // NVM_USE_GROUP
	return WKPF_OK;
}

uint8_t wkpf_load_links(dj_int_array *links) {
	// Taking a shortcut here by directly using the byte array we get from Java and using
	// it as an array of wkpf_link_t structs.
	// This works on AVR and x86 since they're both little endian. To port WKPF to a big endian
	// platform we would need to do some swapping.
	// Also, this relies on the gcc packed struct extension to make sure the compiler doesn't
	// pad the struct, which would make it misaligned with the byte array in Java.
	wkpf_links_store = links;
	// After storing the reference, only use the constants defined above to access it so that we may change the storage implementation later

	DEBUG_LOG(DBG_WKPF, "WKPF: Registering %d links\n", (int)wkpf_number_of_links); // Need a cast here because the type may differ depending on architecture.
#ifdef DARJEELING_DEBUG
	for (int i=0; i<wkpf_number_of_links; i++) {
		// wkpf_link_t *link = wkpf_get_link(i);
		wkpf_link_t *link =  wkpf_get_link(i);
		DEBUG_LOG(DBG_WKPF, "WKPF: Link from (%d, %d) to (%d, %d), wuclass %d\n", link->src_component_id, link->src_property_number, link->dest_component_id, link->dest_property_number, link->dest_wuclass_id);
	}
#endif // DARJEELING_DEBUG
	return WKPF_OK;
}

bool wkpf_does_property_need_initialisation_pull(uint8_t port_number, uint8_t property_number) {
	uint16_t component_id;
	wkpf_get_component_id(port_number, &component_id);

	for(int i=0; i<wkpf_number_of_links; i++) {
		wkpf_link_t *link = wkpf_get_link(i);
		if (link->dest_component_id == component_id
				&& link->dest_property_number == property_number) {
			// The property is the destination of this link. If the source is remote, we need to ask for an initial value
			if (wkpf_node_is_leader(link->src_component_id, nvmcomm_get_node_id())) {
				DEBUG_LOG(DBG_WKPF, "%x, %x doesn't need pull: source is a local property\n", port_number, property_number);
				return false; // Source link is local, so no need to pull initial value as it will come automatically.
			} else {
				DEBUG_LOG(DBG_WKPF, "%x, %x needs initialisation pull\n", port_number, property_number);
				return true; // There is a link to this property, coming from another node. We need to ask it for the initial value.
			}
		}
	}
	DEBUG_LOG(DBG_WKPF, "%x, %x doesn't need pull: not a destination property\n", port_number, property_number);
	return false; // This wuobject isn't used in the application.
}

uint8_t wkpf_pull_property(uint8_t port_number, uint8_t property_number) {
	uint16_t component_id;
	wkpf_get_component_id(port_number, &component_id);

	for(int i=0; i<wkpf_number_of_links; i++) {
		wkpf_link_t *link = wkpf_get_link(i);
		if(link->dest_component_id == component_id && link->dest_property_number == property_number) {
			uint16_t src_component_id = link->src_component_id;
			wkpf_endpoint_t src_endpoint = wkpf_leader_for_component(src_component_id);
			uint8_t src_property_number = link->src_property_number;
			if (src_endpoint.node_id != nvmcomm_get_node_id()) {
				// Properties with local sources will be initialised eventually, so we only need to send a message
				// to ask for initial values coming from remote nodes
				return wkpf_send_request_property_init(src_endpoint.node_id, src_endpoint.port_number, src_property_number);      
			}
		}
	}
	return WKPF_ERR_SHOULDNT_HAPPEN;
}

uint8_t wkpf_propagate_property(wuobject_t *wuobject, uint8_t property_number, void *value) {
	uint8_t port_number = wuobject->port_number;
	uint16_t component_id;
	if (!wkpf_get_component_id(port_number, &component_id))
		return WKPF_OK; // WuObject isn't used in the application.

	wuobject_t *src_wuobject;
	uint8_t wkpf_error_code;

	DEBUG_LOG(DBG_WKPF, "WKPF: propagate property number %x of component %x on port %x (value %x)\n", property_number, component_id, port_number, *((uint16_t *)value)); // TODONR: values other than 16 bit values

	wkpf_get_wuobject_by_port(port_number, &src_wuobject);
	for(int i=0; i<wkpf_number_of_links; i++) {
		wkpf_link_t *link = wkpf_get_link(i);
		if(link->src_component_id == component_id
				&& link->src_property_number == property_number) {
			uint16_t dest_component_id = link->dest_component_id;
			uint8_t dest_property_number = link->dest_property_number;
			uint8_t dest_port_number = wkpf_leader_for_component(dest_component_id).port_number;
			address_t dest_node_id = wkpf_leader_for_component(dest_component_id).node_id;
			if (dest_node_id == nvmcomm_get_node_id()) {
				// Local
				wuobject_t *dest_wuobject;
				wkpf_error_code = wkpf_get_wuobject_by_port(dest_port_number, &dest_wuobject);
				if (wkpf_error_code == WKPF_OK) {
					DEBUG_LOG(DBG_WKPF, "WKPF: propagate_property (local). (%x, %x)->(%x, %x), value %x\n", port_number, property_number, dest_port_number, dest_property_number, *((uint16_t *)value)); // TODONR: values other than 16 bit values
					if (WKPF_GET_PROPERTY_DATATYPE(src_wuobject->wuclass->properties[property_number]) == WKPF_PROPERTY_TYPE_BOOLEAN)
						wkpf_error_code = wkpf_external_write_property_boolean(dest_wuobject, dest_property_number, *((bool *)value));
					else if (WKPF_GET_PROPERTY_DATATYPE(src_wuobject->wuclass->properties[property_number]) == WKPF_PROPERTY_TYPE_SHORT)
						wkpf_error_code = wkpf_external_write_property_int16(dest_wuobject, dest_property_number, *((uint16_t *)value));
					else
						wkpf_error_code = wkpf_external_write_property_refresh_rate(dest_wuobject, dest_property_number, *((uint16_t *)value));
				}
			} else {
				// Remote
				DEBUG_LOG(DBG_WKPF, "WKPF: propagate_property (remote). (%x, %x)->(%x, %x, %x), value %x\n", port_number, property_number, dest_node_id, dest_port_number, dest_property_number, *((uint16_t *)value)); // TODONR: values other than 16 bit values
				if (WKPF_GET_PROPERTY_DATATYPE(src_wuobject->wuclass->properties[property_number]) == WKPF_PROPERTY_TYPE_BOOLEAN)
					wkpf_error_code = wkpf_send_set_property_boolean(dest_node_id, dest_port_number, dest_property_number, link->dest_wuclass_id, *((bool *)value));
				else if (WKPF_GET_PROPERTY_DATATYPE(src_wuobject->wuclass->properties[property_number]) == WKPF_PROPERTY_TYPE_SHORT)
					wkpf_error_code = wkpf_send_set_property_int16(dest_node_id, dest_port_number, dest_property_number, link->dest_wuclass_id, *((uint16_t *)value));
				else
					wkpf_error_code = wkpf_send_set_property_refresh_rate(dest_node_id, dest_port_number, dest_property_number, link->dest_wuclass_id, *((uint16_t *)value));
			}
			if (wkpf_error_code != WKPF_OK)
				return wkpf_error_code;
		}
	}
	return WKPF_OK;
}

uint8_t wkpf_propagate_dirty_properties() {
	uint8_t wkpf_error_code;
	wuobject_t *dirty_wuobject;
	uint8_t dirty_property_number;

	while (wkpf_get_next_dirty_property(&dirty_wuobject, &dirty_property_number)) {
		// TODONR: comm
		// nvmcomm_poll(); // Process incoming messages
		wuobject_property_t *dirty_property = wkpf_get_property(dirty_wuobject, dirty_property_number);
		if (dirty_property->status & PROPERTY_STATUS_NEEDS_PUSH) {
			wkpf_error_code = wkpf_propagate_property(dirty_wuobject, dirty_property_number, &(dirty_property->value));
		} else { // PROPERTY_STATUS_NEEDS_PULL
			DEBUG_LOG(DBG_WKPF, "WKPF: (pull) requesting initial value for property %x at port %x\n", dirty_property_number, dirty_wuobject->port_number);
			wkpf_error_code = wkpf_pull_property(dirty_wuobject->port_number, dirty_property_number);
		}
		if (wkpf_error_code == WKPF_OK) {
			wkpf_propagating_dirty_property_succeeded(dirty_property);
		} else { // TODONR: need better retry mechanism
			DEBUG_LOG(DBG_WKPF, "WKPF: ------!!!------ Propagating property failed: port %x property %x error %x\n", dirty_wuobject->port_number, dirty_property_number, wkpf_error_code);
			wkpf_propagating_dirty_property_failed(dirty_property);
			return wkpf_error_code;
		}
	}
	return WKPF_OK;
}

// TODONR: proper definition for this function.
uint8_t wkpf_get_node_and_port_for_component(uint16_t component_id, address_t *node_id, uint8_t *port_number) {
	if (component_id > wkpf_number_of_components)
		return WKPF_ERR_COMPONENT_NOT_FOUND;
	wkpf_component_t *component = wkpf_get_component(component_id);
	wkpf_endpoint_t *endpoint = wkpf_get_endpoint_for_component(component, 0); // Just using the first here, since I'm not sure what this function means when there's more than one node for a component...
	*node_id = endpoint->node_id;
	*port_number = endpoint->port_number;
	return WKPF_OK;
}

bool wkpf_node_is_leader(uint16_t component_id, address_t node_id) {
	return wkpf_get_endpoint_for_component(wkpf_get_component(component_id), 0)->node_id == node_id;
}

wkpf_endpoint_t wkpf_leader_for_component(uint16_t component_id) {
	wkpf_component_t *component = wkpf_get_component(component_id);
	return *wkpf_get_endpoint_for_component(component, 0);
}

// uint8_t wkpf_local_endpoint_for_component(uint16_t component_id, remote_endpoint* endpoint) {
//   for (int i=0; i<component_to_wuobject_map[component_id].number_of_endpoints; i++) {
//     if (component_to_wuobject_map[component_id].endpoints[i].node_id == nvmcomm_get_node_id())
//       *endpoint = component_to_wuobject_map[component_id].endpoints[i];
//       return WKPF_OK;
//   }

//   return WKPF_ERR_ENDPOINT_NOT_FOUND;
// }

