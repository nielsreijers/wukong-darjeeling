#include "types.h"
#include "panic.h"
#include "debug.h"
#include "array.h"
#include "hooks.h"
#include "execution.h"
#include "wkcomm.h"
#include "heap.h"
#include "jlib_base.h"

#include "wkpf.h"
#include "wkpf_gc.h"
#include "wkpf_wuclasses.h"
#include "wkpf_wuobjects.h"
#include "wkpf_properties.h"
#include "wkpf_links.h"
#include "wkpf_comm.h"

uint8_t wkpf_error_code = WKPF_OK;

dj_hook wkpf_markRootSetHook;
dj_hook wkpf_updatePointersHook;
dj_hook wkpf_comm_handleMessageHook;

void javax_wukong_wkpf_WKPF_void__init() {
	wkpf_markRootSetHook.function = wkpf_markRootSet;
	dj_hook_add(&dj_vm_markRootSetHook, &wkpf_markRootSetHook);

	wkpf_updatePointersHook.function = wkpf_updatePointers;
	dj_hook_add(&dj_mem_updateReferenceHook, &wkpf_updatePointersHook);

	wkpf_comm_handleMessageHook.function = wkpf_comm_handle_message;
	dj_hook_add(&wkcomm_handle_message_hook, &wkpf_comm_handleMessageHook);
}

void javax_wukong_wkpf_WKPF_byte_getErrorCode()
{
	dj_exec_stackPushShort(wkpf_error_code);
}

void javax_wukong_wkpf_WKPF_void_registerWuClass_short_byte__()
{
	dj_int_array *byteArrayProperties = REF_TO_VOIDP(dj_exec_stackPopRef());
	// check null
	if (byteArrayProperties==nullref){
		dj_exec_createAndThrow(BASE_CDEF_java_lang_NullPointerException);
	}
	uint16_t wuclass_id = (uint16_t)dj_exec_stackPopShort();
	DEBUG_LOG(DBG_WKPF, "WKPF: Registering virtual wuclass with id %x\n", wuclass_id);
	wkpf_error_code = wkpf_register_virtual_wuclass(wuclass_id, NULL, byteArrayProperties->array.length, (uint8_t *)byteArrayProperties->data.bytes);
}

void javax_wukong_wkpf_WKPF_void_createWuObject_short_byte_javax_wukong_wkpf_VirtualWuObject()
{
	dj_object *java_instance_reference = REF_TO_VOIDP(dj_exec_stackPopRef());
	uint8_t port_number = (uint8_t)dj_exec_stackPopShort();
	uint16_t wuclass_id = (uint16_t)dj_exec_stackPopShort();
	DEBUG_LOG(DBG_WKPF, "WKPF: Creating wuobject for virtual wuclass with id %x at port %x (ref: %p)\n", wuclass_id, port_number, java_instance_reference);
	wkpf_error_code = wkpf_create_wuobject(wuclass_id, port_number, java_instance_reference);
}

void javax_wukong_wkpf_WKPF_void_destroyWuObject_byte()
{
	uint8_t port_number = (uint8_t)dj_exec_stackPopShort();
	DEBUG_LOG(DBG_WKPF, "WKPF: Removing wuobject at port %x\n", port_number);
	wkpf_error_code = wkpf_remove_wuobject(port_number);
}

void javax_wukong_wkpf_WKPF_short_getPropertyShort_javax_wukong_wkpf_VirtualWuObject_byte()
{
	uint8_t property_number = (uint8_t)dj_exec_stackPopShort();
	dj_object *java_instance_reference = REF_TO_VOIDP(dj_exec_stackPopRef());
	wuobject_t *wuobject;
	wkpf_error_code = wkpf_get_wuobject_by_java_instance_reference(java_instance_reference, &wuobject);
	if (wkpf_error_code == WKPF_OK) {
		int16_t value;
		wkpf_error_code = wkpf_internal_read_property_int16(wuobject, property_number, &value);
		dj_exec_stackPushShort(value);
	}
}

void javax_wukong_wkpf_WKPF_void_setPropertyShort_javax_wukong_wkpf_VirtualWuObject_byte_short() {
	int16_t value = (int16_t)dj_exec_stackPopShort();
	uint8_t property_number = (uint8_t)dj_exec_stackPopShort();
	dj_object *java_instance_reference = REF_TO_VOIDP(dj_exec_stackPopRef());
	wuobject_t *wuobject;
	wkpf_error_code = wkpf_get_wuobject_by_java_instance_reference(java_instance_reference, &wuobject);
	if (wkpf_error_code == WKPF_OK) {
		wkpf_error_code = wkpf_internal_write_property_int16(wuobject, property_number, value);
	}
}

void javax_wukong_wkpf_WKPF_boolean_getPropertyBoolean_javax_wukong_wkpf_VirtualWuObject_byte() {
	uint8_t property_number = (uint8_t)dj_exec_stackPopShort();
	dj_object *java_instance_reference = REF_TO_VOIDP(dj_exec_stackPopRef());
	wuobject_t *wuobject;
	wkpf_error_code = wkpf_get_wuobject_by_java_instance_reference(java_instance_reference, &wuobject);
	if (wkpf_error_code == WKPF_OK) {
		bool value;
		wkpf_error_code = wkpf_internal_read_property_boolean(wuobject, property_number, &value);
		dj_exec_stackPushShort(value);
	}
}
void javax_wukong_wkpf_WKPF_void_setPropertyBoolean_javax_wukong_wkpf_VirtualWuObject_byte_boolean() {
	bool value = (int16_t)dj_exec_stackPopShort();
	uint8_t property_number = (uint8_t)dj_exec_stackPopShort();
	dj_object *java_instance_reference = REF_TO_VOIDP(dj_exec_stackPopRef());
	wuobject_t *wuobject;
	wkpf_error_code = wkpf_get_wuobject_by_java_instance_reference(java_instance_reference, &wuobject);
	if (wkpf_error_code == WKPF_OK) {
		wkpf_error_code = wkpf_internal_write_property_boolean(wuobject, property_number, value);
	}
}

void javax_wukong_wkpf_WKPF_void_setPropertyShort_short_byte_short() {
	int16_t value = (int16_t)dj_exec_stackPopShort();
	uint8_t property_number = (uint8_t)dj_exec_stackPopShort();
	uint16_t component_id = (uint16_t)dj_exec_stackPopShort();
	address_t node_id;
	uint8_t port_number;
	wkpf_error_code = wkpf_get_node_and_port_for_component(component_id, &node_id, &port_number);
	if (wkpf_error_code == WKPF_OK) {
		if (node_id != wkcomm_get_node_id())
			wkpf_error_code = WKPF_ERR_REMOTE_PROPERTY_FROM_JAVASET_NOT_SUPPORTED;
		else {
			wuobject_t *wuobject;
			wkpf_error_code = wkpf_get_wuobject_by_port(port_number, &wuobject);
			if (wkpf_error_code == WKPF_OK) {
				DEBUG_LOG(DBG_WKPF, "WKPF: setPropertyShort (local). Port %x, property %x, value %x\n", port_number, property_number, value);
				wkpf_error_code = wkpf_external_write_property_int16(wuobject, property_number, value);
			}
		}
	}
}

void javax_wukong_wkpf_WKPF_void_setPropertyBoolean_short_byte_boolean() {
	bool value = (bool)dj_exec_stackPopShort();
	uint8_t property_number = (uint8_t)dj_exec_stackPopShort();
	uint16_t component_id = (uint16_t)dj_exec_stackPopShort();
	address_t node_id;
	uint8_t port_number;
	wkpf_error_code = wkpf_get_node_and_port_for_component(component_id, &node_id, &port_number);
	if (wkpf_error_code == WKPF_OK) {
		if (node_id != wkcomm_get_node_id())
			wkpf_error_code = WKPF_ERR_REMOTE_PROPERTY_FROM_JAVASET_NOT_SUPPORTED;
		else {
			wuobject_t *wuobject;
			wkpf_error_code = wkpf_get_wuobject_by_port(port_number, &wuobject);
			if (wkpf_error_code == WKPF_OK) {
				DEBUG_LOG(DBG_WKPF, "WKPF: setPropertyBoolean (local). Port %x, property %x, value %x\n", port_number, property_number, value);
				wkpf_error_code = wkpf_external_write_property_boolean(wuobject, property_number, value);
			}
		}
	}
}

void javax_wukong_wkpf_WKPF_void_setPropertyRefreshRate_short_byte_short() {
	int16_t value = (int16_t)dj_exec_stackPopShort();
	uint8_t property_number = (uint8_t)dj_exec_stackPopShort();
	uint16_t component_id = (uint16_t)dj_exec_stackPopShort();
	address_t node_id;
	uint8_t port_number;
	wkpf_error_code = wkpf_get_node_and_port_for_component(component_id, &node_id, &port_number);
	if (wkpf_error_code == WKPF_OK) {
		if (node_id != wkcomm_get_node_id())
			wkpf_error_code = WKPF_ERR_REMOTE_PROPERTY_FROM_JAVASET_NOT_SUPPORTED;
		else {
			wuobject_t *wuobject;
			wkpf_error_code = wkpf_get_wuobject_by_port(port_number, &wuobject);
			if (wkpf_error_code == WKPF_OK) {
				DEBUG_LOG(DBG_WKPF, "WKPF: setPropertyRefreshRate (local). Port %x, property %x, value %x\n", port_number, property_number, value);
				wkpf_error_code = wkpf_external_write_property_refresh_rate(wuobject, property_number, value);
			}
		}
	}
}

void javax_wukong_wkpf_WKPF_javax_wukong_wkpf_VirtualWuObject_select() {
	wuobject_t *wuobject;
	while(true) {

// // TODONR: implement communication
// 	// Process any incoming messages
// 	nvmcomm_poll();
// // TODONR: implement group stuff
// #ifdef NVM_USE_GROUP
// 	// Send out a heartbeat message if it's due, and check for failed nodes.
// 	group_heartbeat();
// #endif // NVM_USE_GROUP
// // TODONR: implement runlevels (needed when we start to do the bytecode upgrade)
//		if (nvm_runlevel == NVM_RUNLVL_VM) {
			// Propagate any dirty properties
			wkpf_propagate_dirty_properties();
			// Check if any wuobjects need updates
			if(wkpf_get_next_wuobject_to_update(&wuobject)) { // Will call update() for native profiles directly, and return true for virtual profiles requiring an update.
				dj_exec_stackPushRef(VOIDP_TO_REF(wuobject->java_instance_reference));
				DEBUG_LOG(DBG_WKPF, "WKPF: WKPF.select returning wuclass at port %x.\n", wuobject->port_number);
				return;
			}
		// }
	}
}

void javax_wukong_wkpf_WKPF_void_loadComponentToWuObjectAddrMap_java_lang_Object__() {
	dj_ref_array *map = REF_TO_VOIDP(dj_exec_stackPopRef());
	wkpf_error_code = wkpf_load_component_to_wuobject_map(map);
}

void javax_wukong_wkpf_WKPF_void_loadLinkDefinitions_byte__() {
	dj_int_array *links = REF_TO_VOIDP(dj_exec_stackPopRef());
	wkpf_error_code = wkpf_load_links(links);    
}

void javax_wukong_wkpf_WKPF_byte_getPortNumberForComponent_short() {
	uint16_t component_id = (uint16_t)dj_exec_stackPopShort();
	address_t node_id;
	uint8_t port_number;
	wkpf_error_code = wkpf_get_node_and_port_for_component(component_id, &node_id, &port_number);
	dj_exec_stackPushShort(port_number);
}

void javax_wukong_wkpf_WKPF_boolean_isLocalComponent_short() {
	uint16_t component_id = (int16_t)dj_exec_stackPopShort();
	address_t node_id;
	uint8_t port_number;
	wkpf_error_code = wkpf_get_node_and_port_for_component(component_id, &node_id, &port_number);
	dj_exec_stackPushShort(wkpf_error_code == WKPF_OK && node_id == wkcomm_get_node_id());
}

void javax_wukong_wkpf_WKPF_short_getMyNodeId() {
	dj_exec_stackPushShort(wkcomm_get_node_id());
}
