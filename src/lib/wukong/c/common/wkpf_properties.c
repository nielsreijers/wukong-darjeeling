#include "types.h"
#include "debug.h"
#include "wkpf.h"
#include "wkpf_properties.h"

void wkpf_update_status_after_property_write(wuobject_t *wuobject, wuobject_property_t *property, bool external_access) {
	// Propagate this property:
	property->property_status |= PROPERTY_STATUS_NEEDS_PUSH;
	// And remove any flags that indicate we were waiting for an initial value:
	property->property_status &= ~PROPERTY_STATUS_NEEDS_PULL;
	property->property_status &= ~PROPERTY_STATUS_NEEDS_PULL_WAITING;
	if (external_access) // Only call update() when someone else writes to the property, not for internal writes (==writes that are already coming from update())
		wkpf_set_need_to_call_update_for_wuobject(wuobject);
}

// Verifies the property exists for this wuobject, and the data and r/w access match
uint8_t wkpf_verify_property_access(wuobject_t *wuobject, uint8_t property_number, uint8_t access, bool external_access, uint8_t type) {
	if (wuobject->wuclass->number_of_properties <= property_number)
		return WKPF_ERR_PROPERTY_NOT_FOUND;
	uint8_t property = wuobject->wuclass->properties[property_number];
	if (external_access) {
		if (access == WKPF_PROPERTY_ACCESS_READONLY && WKPF_IS_WRITEONLY_PROPERTY(property))
			return WKPF_ERR_WRITE_ONLY;
		if (access == WKPF_PROPERTY_ACCESS_WRITEONLY && WKPF_IS_READONLY_PROPERTY(property))
			return WKPF_ERR_READ_ONLY;
	}
	if (type != WKPF_GET_PROPERTY_DATATYPE(property))
		return WKPF_ERR_WRONG_DATATYPE;
	return WKPF_OK;
}


uint8_t wkpf_read_property_int16(wuobject_t *wuobject, uint8_t property_number, bool external_access, int16_t *value) {
	uint8_t retval = wkpf_verify_property_access(wuobject, property_number, WKPF_PROPERTY_ACCESS_READONLY, external_access, WKPF_PROPERTY_TYPE_SHORT);
	if (retval != WKPF_OK)
		return retval;
	*value = *((int16_t *)wkpf_get_property(wuobject, property_number)->value);
	return WKPF_OK;
}
uint8_t wkpf_write_property_int16(wuobject_t *wuobject, uint8_t property_number, bool external_access, int16_t value) {
	uint8_t retval = wkpf_verify_property_access(wuobject, property_number, WKPF_PROPERTY_ACCESS_WRITEONLY, external_access, WKPF_PROPERTY_TYPE_SHORT);
	if (retval != WKPF_OK)
		return retval;
	wuobject_property_t *property = wkpf_get_property(wuobject, property_number);
	*((int16_t *)wkpf_get_property(wuobject, property_number)->value) = value;
	wkpf_update_status_after_property_write(wuobject, property, external_access);
	return WKPF_OK;
}


uint8_t wkpf_read_property_boolean(wuobject_t *wuobject, uint8_t property_number, bool external_access, bool *value) {
	uint8_t retval = wkpf_verify_property_access(wuobject, property_number, WKPF_PROPERTY_ACCESS_READONLY, external_access, WKPF_PROPERTY_TYPE_BOOLEAN);
	if (retval != WKPF_OK)
		return retval;
	*value = *((bool *)wkpf_get_property(wuobject, property_number)->value);
	return WKPF_OK;
}
uint8_t wkpf_write_property_boolean(wuobject_t *wuobject, uint8_t property_number, bool external_access, bool value) {
	uint8_t retval = wkpf_verify_property_access(wuobject, property_number, WKPF_PROPERTY_ACCESS_WRITEONLY, external_access, WKPF_PROPERTY_TYPE_BOOLEAN);
	if (retval != WKPF_OK)
		return retval;
	wuobject_property_t *property = wkpf_get_property(wuobject, property_number);
	*((bool *)wkpf_get_property(wuobject, property_number)->value) = value;
	wkpf_update_status_after_property_write(wuobject, property, external_access);
	return WKPF_OK;
}


uint8_t wkpf_read_property_refresh_rate(wuobject_t *wuobject, uint8_t property_number, bool external_access, wkpf_refresh_rate_t *value) {
	uint8_t retval = wkpf_verify_property_access(wuobject, property_number, WKPF_PROPERTY_ACCESS_READONLY, external_access, WKPF_PROPERTY_TYPE_REFRESH_RATE);
	if (retval != WKPF_OK)
		return retval;
	*value = *((wkpf_refresh_rate_t *)wkpf_get_property(wuobject, property_number)->value);
	return WKPF_OK;
}
uint8_t wkpf_write_property_refresh_rate(wuobject_t *wuobject, uint8_t property_number, bool external_access, wkpf_refresh_rate_t value) {
	uint8_t retval = wkpf_verify_property_access(wuobject, property_number, WKPF_PROPERTY_ACCESS_WRITEONLY, external_access, WKPF_PROPERTY_TYPE_REFRESH_RATE);
	if (retval != WKPF_OK)
		return retval;
	wuobject_property_t *property = wkpf_get_property(wuobject, property_number);
	*((wkpf_refresh_rate_t *)wkpf_get_property(wuobject, property_number)->value) = value;
	wkpf_update_status_after_property_write(wuobject, property, external_access);
	return WKPF_OK;
}


// uint8_t wkpf_get_property_status(wuobject_t *wuobject, uint8_t property_number, uint8_t *status) {
// 	for (int i=0; i<number_of_properties; i++) {
// 		if (properties[i].wuobject_port_number == wuobject->port_number && properties[i].property_number == property_number) {
// 			DEBUGF_WKPF("WKPF: wkpf_get_property_status: (index %x port %x, property %x): %x\n", i, wuobject->port_number, property_number, properties[i].property_status);
// 			*status = properties[i].property_status;
// 			return WKPF_OK;
// 		}
// 	}
// 	return WKPF_ERR_PROPERTY_NOT_FOUND;
// }

// uint8_t wkpf_property_needs_initialisation_push(wuobject_t *wuobject, uint8_t property_number) {
// 	if (wuobject->wuclass->number_of_properties <= property_number)
// 		return WKPF_ERR_PROPERTY_NOT_FOUND;
// 	for (int i=0; i<number_of_properties; i++) {
// 		if (properties[i].wuobject_port_number == wuobject->port_number && properties[i].property_number == property_number) {
// 			if (properties[i].property_status & PROPERTY_STATUS_NEEDS_PULL || properties[i].property_status & PROPERTY_STATUS_NEEDS_PULL_WAITING) {
//         // A property in another WuObject needs this property as it's initial value,
//         // but this property is also waiting for its initial value itself (a chain of unitialised properties)
//         // So we only mark that the next push needs to be forced, and wait for the arrival of this
//         // property's initial value before propagating.
// 				properties[i].property_status |= PROPERTY_STATUS_FORCE_NEXT_PUSH;
// 			} else {
//         // Otherwise (the property's value is already available), immediately schedule it to be propagated
// 				properties[i].property_status = (PROPERTY_STATUS_NEEDS_PUSH | PROPERTY_STATUS_FORCE_NEXT_PUSH);
// 			}
// 			DEBUGF_WKPF("WKPF: wkpf_property_needs_initialisation_push: (index %x port %x, property %x): value %x, status %x\n", i, wuobject->port_number, property_number, properties[i].value, properties[i].property_status);
// 			return WKPF_OK;
// 		}
// 	}
// 	return WKPF_ERR_SHOULDNT_HAPPEN;
// }

// bool inline wkpf_property_status_is_dirty(uint8_t status) {
// 	if (!((status & PROPERTY_STATUS_NEEDS_PUSH) || (status & PROPERTY_STATUS_NEEDS_PULL)))
//     return false; // Doesn't need push or pull so skip.
// if ((status & 0x0C) == 0)
//     return true; // Didn't fail, or only once (because bits 2,3 are 0): send message
//   uint8_t timer_bit = (nvm_current_time >> (status & PROPERTY_STATUS_FAILURE_COUNT_TIMES2_MASK /* failurecount*2*/)) & 1;
// return (status & 1) == timer_bit;
// }

// static uint8_t last_returned_dirty_property_index = 0;
// bool wkpf_get_next_dirty_property(uint8_t *port_number, uint8_t *property_number, int16_t *value, uint8_t *status) {
// 	if (last_returned_dirty_property_index >= number_of_properties)
//     last_returned_dirty_property_index = number_of_properties-1; // Could happen if wuobjects were removed
// int i = last_returned_dirty_property_index;
// uint16_t component_id;

// do {
// 	i = (i+1) % number_of_properties;
// 	wkpf_get_component_id(properties[i].wuobject_port_number, &component_id);
// 	if (wkpf_property_status_is_dirty(properties[i].property_status)
// 			&& wkpf_node_is_leader(component_id, nvmcomm_get_node_id())) {
// 		DEBUGF_WKPF("WKPF: wkpf_get_next_dirty_property DIRTY[%x]: port %x property %x status %x\n", i, properties[i].wuobject_port_number, properties[i].property_number, properties[i].property_status);
// 		last_returned_dirty_property_index = i;
// 		*port_number = properties[i].wuobject_port_number;
// 		*property_number = properties[i].property_number;
// 		*value = properties[i].value;
// 		*status = properties[i].property_status;
// 	      // Optimistically set this property to not dirty to avoid having to go through the whole list again after the message has been sent.
// 	      // Hopefully succesful propagations will be the most common case.
// 	      // The original status will be passed back through wkpf_propagating_dirty_property_failed in case the propagation fails
// 		if (properties[i].property_status & PROPERTY_STATUS_NEEDS_PUSH) {
// 			properties[i].property_status &= (~PROPERTY_STATUS_NEEDS_PUSH & ~PROPERTY_STATUS_FORCE_NEXT_PUSH & 0xF0);
// 	      } else { // PROPERTY_STATUS_NEEDS_PULL: after sending the request, wait for the source node to send the value
// 	      	properties[i].property_status &= (~PROPERTY_STATUS_NEEDS_PULL & 0xF0);
// 	      	properties[i].property_status |= PROPERTY_STATUS_NEEDS_PULL_WAITING;
// 	      }
// 	      return TRUE;
// 	  }
// 	//    DEBUGF_WKPF("WKPF: wkpf_get_next_dirty_property NOT DIRTY[%x]: port %x property %x status %x\n", i, properties[i].wuobject_port_number, properties[i].property_number, properties[i].property_status);
// 	} while(i != last_returned_dirty_property_index);
// return FALSE;
// }

// void wkpf_propagating_dirty_property_failed(uint8_t port_number, uint8_t property_number, uint8_t original_status) {
// 	uint8_t failure_count = (original_status & PROPERTY_STATUS_FAILURE_COUNT_TIMES2_MASK) / 2;  
//   if (failure_count < 7) // increase if failure count < 7
//   	failure_count++;
//   uint8_t timer_bit_number = failure_count*2; 
//   uint8_t timer_bit_value = ((nvm_current_time >> timer_bit_number) + 1) & 1; // Add one to flip bit 
//   uint8_t new_status = (original_status & 0xF0) | (failure_count << 1) | timer_bit_value;
//   for (int i=0; i<number_of_properties; i++) {
//   	if (properties[i].wuobject_port_number == port_number
//   		&& properties[i].property_number == property_number)
//   		properties[i].property_status = new_status;
//   }
//   DEBUGF_WKPF("WKPF: wkpf_propagating_dirty_property_failed!!!!! property %x at port %x. previous status %x new status %x\n", property_number, port_number, original_status, new_status);
// }

