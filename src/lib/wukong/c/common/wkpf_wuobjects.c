#include "types.h"
#include "debug.h"
#include "heap.h"
#include "djtimer.h"
#include "wkpf.h"
#include "wkpf_wuobjects.h"

wkpf_local_wuobject *wuobjects = NULL;
wkpf_local_wuobject *last_updated_wuobject = NULL;

uint8_t wkpf_create_wuobject(uint16_t wuclass_id, uint8_t port_number, dj_object *java_instance_reference /* TODO: find out what datatype to use */ ) {
	wkpf_local_wuobject *wuobject;
	if (wkpf_get_wuobject_by_port(port_number, &wuobject) != WKPF_ERR_WUOBJECT_NOT_FOUND) {
		DEBUG_LOG(DBG_WKPF, "WKPF: Port %x in use while creating wuobject for wuclass id %x: FAILED\n", port_number, wuclass_id);
		return WKPF_ERR_PORT_IN_USE;
	}

	// Find the wuclass definition for this wuclass_id
	wkpf_wuclass_definition *wuclass;
	uint8_t retval;
	retval = wkpf_get_wuclass_by_id(wuclass_id, &wuclass);
	if (retval != WKPF_OK)
		return retval;

	// Check if an instance of the wuclass is provided if it's a virtual wuclass
	if (WKPF_IS_VIRTUAL_WUCLASS(wuclass) && java_instance_reference==0)
		return WKPF_ERR_NEED_VIRTUAL_WUCLASS_INSTANCE;

	// Allocate memory for the new wuobject
	uint16_t size = sizeof(wkpf_local_wuobject); // TODO: add space for the properties;
	dj_mem_addSafePointer((void**)&java_instance_reference); // dj_mem_alloc may cause GC to run, so the address of the wuclass and the virtual wuclass instance may change. this tells the GC to update our pointer if it does.
	dj_mem_addSafePointer((void**)&wuclass); // dj_mem_alloc may cause GC to run, so the address of the wuclass and the virtual wuclass instance may change. this tells the GC to update our pointer if it does.
	wuobject = (wkpf_local_wuobject*)dj_mem_alloc(size, CHUNKID_WUCLASS);
	dj_mem_removeSafePointer((void**)&java_instance_reference);
	dj_mem_removeSafePointer((void**)&wuclass);
	if (wuobject == NULL) {
		DEBUG_LOG(DBG_WKPF, "WKPF: Out of memory while creating wuobject for wuclass %x at port %x: FAILED\n", wuclass_id, port_number);
		return WKPF_ERR_OUT_OF_MEMORY;
	}


	wuobject->wuclass = wuclass;
	wuobject->port_number = port_number;
	wuobject->java_instance_reference = java_instance_reference;
	wuobject->need_to_call_update = false;
	if (retval != WKPF_OK)
		return retval;
	// Run update function once to initialise properties.
	wkpf_set_need_to_call_update_for_wuobject(wuobject);

	wuobject->next = wuobjects;
	wuobjects = wuobject;
	DEBUG_LOG(DBG_WKPF, "WKPF: Created wuobject for wuclass id %x at port %x\n", wuclass_id, port_number);
	return WKPF_OK;
}

uint8_t wkpf_remove_wuobject(uint8_t port_number) {
	wkpf_local_wuobject *wuobject;

	wuobject = wuobjects;
	if (wuobject && wuobject->port_number == port_number) {
		// It's the first in the list
		wuobjects = wuobjects->next;
		dj_mem_free(wuobject);
		return WKPF_OK;
	}

	while (wuobject) {
		if (wuobject->next && wuobject->next->port_number == port_number) {
			wkpf_local_wuobject *nextnext = wuobject->next->next;
			dj_mem_free(wuobject->next);
			wuobject->next = nextnext;
			return WKPF_OK;
		}
	}

	DEBUG_LOG(DBG_WKPF, "WKPF: No wuobject at port %x found: FAILED\n", port_number);
	return WKPF_ERR_WUOBJECT_NOT_FOUND;  
}

uint8_t wkpf_get_wuobject_by_port(uint8_t port_number, wkpf_local_wuobject **wuobject) {
	*wuobject = wuobjects;
	while (*wuobject) {
		if ((*wuobject)->port_number == port_number) {
			return WKPF_OK;
		}
		*wuobject = (*wuobject)->next;
	}
	DEBUG_LOG(DBG_WKPF, "WKPF: No wuobject at port %x found: FAILED\n", port_number);
	return WKPF_ERR_WUOBJECT_NOT_FOUND;
}

uint8_t wkpf_get_wuobject_by_index(uint8_t index, wkpf_local_wuobject **wuobject) {
	*wuobject = wuobjects;
	while (index > 0 && wuobject != NULL) {
		index--;
		*wuobject = (*wuobject)->next;
	}
	if (*wuobject)
		return WKPF_OK;
	else
		return WKPF_ERR_WUOBJECT_NOT_FOUND;
}

uint8_t wkpf_get_wuobject_by_java_instance_reference(dj_object *java_instance_reference, wkpf_local_wuobject **wuobject) {
	*wuobject = wuobjects;
	while (*wuobject) {
		if ((*wuobject)->java_instance_reference == java_instance_reference) {
			return WKPF_OK;
		}
		*wuobject = (*wuobject)->next;
	}
	DEBUG_LOG(DBG_WKPF, "WKPF: no wuobject for java object at %x found: FAILED\n", java_instance_reference);
	return WKPF_ERR_WUOBJECT_NOT_FOUND;
}

uint8_t wkpf_get_number_of_wuobjects() {
	int number_of_wuobjects = 0;
	wkpf_local_wuobject *wuobject = wuobjects;
	while (wuobject) {
		number_of_wuobjects++;
		wuobject = wuobject->next;
	}
	return number_of_wuobjects;
}

void wkpf_set_need_to_call_update_for_wuobject(wkpf_local_wuobject *wuobject) {
	// TODONR: for now just call directly for native wuclasses
	// Java update should be handled by returning from the WKPF.select() function
	if (WKPF_IS_NATIVE_WUOBJECT(wuobject))
		wuobject->wuclass->update(wuobject);
	else
		wuobject->need_to_call_update = true;
}

bool wkpf_get_next_wuobject_to_update(wkpf_local_wuobject **virtual_wuobject) {
	if (wuobjects == NULL)
		return false;
	if (last_updated_wuobject == NULL)
		last_updated_wuobject = wuobjects;
	wkpf_local_wuobject *wuobject = last_updated_wuobject;

	// wuobject is now pointing to the last updated object
	do {
		// Find the first next object that needs to be updated, or the same if there's no other.
		// Find next object
		wuobject = wuobject->next;
		if (wuobject == NULL)
			wuobject = wuobjects; // Wrap around to the first object.

		// Does it need to be updated?
		if ((wuobject->next_scheduled_update > 0 && wuobjects->next_scheduled_update < dj_timer_getTimeMillis())
				|| wuobjects->need_to_call_update) {
			// Update this object
			// Clear the flag if it was set
			wuobject->need_to_call_update = false;
			// If update has to be called because it's scheduled, schedule the next call
		if (wuobject->next_scheduled_update > 0 && wuobject->next_scheduled_update < dj_timer_getTimeMillis())
			wkpf_schedule_next_update_for_wuobject(wuobject);
			// Call update() directly for native wuobjects, or return virtual wuobject so WKPF.select() can return it to Java
		if (WKPF_IS_NATIVE_WUOBJECT(wuobject)) {
				// Mark wuobject as safe just in case the wuclass does something to trigger GC
			dj_mem_addSafePointer((void**)&wuobject);
			DEBUG_LOG(DBG_WKPFUPDATE, "WKPFUPDATE: Update native wuobject at port %x\n", wuobject->port_number);
			wuobject->wuclass->update(wuobject);
			dj_mem_removeSafePointer((void**)&wuobject);
			} else { // 
				*virtual_wuobject = wuobject;
				last_updated_wuobject = wuobject;
				DEBUG_LOG(DBG_WKPFUPDATE, "WKPFUPDATE: Update virtual wuobject at port %x\n", wuobject->port_number);
				return true;
			}
		}
	} while(wuobject != last_updated_wuobject);
	return false; // No Java wuobjects need to be updated
}

void wkpf_schedule_next_update_for_wuobject(wkpf_local_wuobject *wuobject) {
	for (int i=0; i<wuobject->wuclass->number_of_properties; i++) {
		if (WKPF_GET_PROPERTY_DATATYPE(wuobject->wuclass->properties[i]) == WKPF_PROPERTY_TYPE_REFRESH_RATE) {
			wkpf_refresh_rate_t refresh_rate;
// TODONR			wkpf_internal_read_property_refresh_rate(wuobject, i, &refresh_rate);
      if (refresh_rate == 0) // 0 means turned off
      	wuobject->next_scheduled_update = 0;
      else
      	wuobject->next_scheduled_update = dj_timer_getTimeMillis() + refresh_rate;
      DEBUG_LOG(DBG_WKPFUPDATE, "WKPFUPDATE: Scheduled next update for object at port %x. Refresh rate:%x Current time:%08lx Next update at:%08lx\n", wuobject->port_number, refresh_rate, dj_timer_getTimeMillis(), wuobject->next_scheduled_update);
      return;
  }
}
}
