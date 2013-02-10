#include "wkcomm.h"
#include "panic.h"
#include "wkpf.h"
#include "wkpf_comm.h"

uint8_t send_message(address_t dest_node_id, uint8_t command, uint8_t *payload, uint8_t length) {
	// Print some debug info
#ifdef DEBUG
	DEBUG_LOG(DBG_WKPF, "WKPF: sending property set command to %d:", dest_node_id);
	for(int i=0; i<length; i++) {
		DEBUG_LOG(DBG_WKPF, "[%x] ", message_buffer[i]);
	}
	DEBUG_LOG(DBG_WKPF, "\n");
#endif

	// Send
	wkcomm_received_msg *reply;
	uint8_t retval = wkcomm_send_and_wait_for_reply(dest_node_id, command, payload, length,
													100 /* 100ms timeout */,  (uint8_t[]){command+1 /* the reply to this command */, WKPF_COMM_CMD_ERROR_R}, 2, &reply);
	if (retval == WKCOMM_SEND_OK) {
		if (reply->command != WKPF_COMM_CMD_ERROR_R)
			return WKPF_OK;
		else
			return reply->payload[0]; // Contains a WKPF_ERR code.
	} else if (retval == WKCOMM_SEND_ERR_NO_REPLY) {
		return WKPF_ERR_NVMCOMM_NO_REPLY;
	} else {
		return WKPF_ERR_NVMCOMM_SEND_ERROR;
	}
}

uint8_t wkpf_send_set_property_int16(address_t dest_node_id, uint8_t port_number, uint8_t property_number, uint16_t wuclass_id, int16_t value) {
	uint8_t message_buffer[7];
	message_buffer[0] = port_number;
	message_buffer[1] = (uint8_t)(wuclass_id >> 8);
	message_buffer[2] = (uint8_t)(wuclass_id);
	message_buffer[3] = property_number;
	message_buffer[4] = WKPF_PROPERTY_TYPE_SHORT;
	message_buffer[5] = (uint8_t)(value >> 8);
	message_buffer[6] = (uint8_t)(value);
	return send_message(dest_node_id, WKPF_COMM_CMD_WRITE_PROPERTY, message_buffer, 7);
}

uint8_t wkpf_send_set_property_boolean(address_t dest_node_id, uint8_t port_number, uint8_t property_number, uint16_t wuclass_id, bool value) {
	uint8_t message_buffer[6];
	message_buffer[0] = port_number;
	message_buffer[1] = (uint8_t)(wuclass_id >> 8);
	message_buffer[2] = (uint8_t)(wuclass_id);
	message_buffer[3] = property_number;
	message_buffer[4] = WKPF_PROPERTY_TYPE_BOOLEAN;
	message_buffer[5] = (uint8_t)(value);
	return send_message(dest_node_id, WKPF_COMM_CMD_WRITE_PROPERTY, message_buffer, 6);
}

uint8_t wkpf_send_set_property_refresh_rate(address_t dest_node_id, uint8_t port_number, uint8_t property_number, uint16_t wuclass_id, wkpf_refresh_rate_t value) {
	uint8_t message_buffer[7];
	message_buffer[0] = port_number;
	message_buffer[1] = (uint8_t)(wuclass_id >> 8);
	message_buffer[2] = (uint8_t)(wuclass_id);
	message_buffer[3] = property_number;
	message_buffer[4] = WKPF_PROPERTY_TYPE_REFRESH_RATE;
	message_buffer[5] = (uint8_t)(value >> 8);
	message_buffer[6] = (uint8_t)(value);
	return send_message(dest_node_id, WKPF_COMM_CMD_WRITE_PROPERTY, message_buffer, 7);
}

uint8_t wkpf_send_request_property_init(address_t dest_node_id, uint8_t port_number, uint8_t property_number) {
	uint8_t message_buffer[2];
	message_buffer[0] = port_number;
	message_buffer[1] = property_number;
	return send_message(dest_node_id, WKPF_COMM_CMD_REQUEST_PROPERTY_INIT, message_buffer, 2);
}

// void wkpf_comm_handle_message(address_t src, u08_t nvmcomm_command, u08_t *payload, u08_t *response_size, u08_t *response_cmd) {
//   uint8_t number_of_wuclasses;
//   uint8_t number_of_wuobjects;
//   // TODONR: uint16_t wuclass_id;
//   uint8_t port_number;
//   uint8_t property_number;
//   uint8_t retval;
//   wuobject_t *wuobject;

//   if (nvm_runlevel != NVM_RUNLVL_VM)
//     return;

//   switch (nvmcomm_command) {
//     case WKPF_COMM_CMD_GET_LOCATION:
//       {
//         char* location_in_message_payload = (char*)payload+3;
//         uint8_t* length_in_message_payload = (uint8_t *)&payload[2];
//         wkpf_config_get_location_string(location_in_message_payload, length_in_message_payload);
//         *response_cmd = WKPF_COMM_CMD_GET_LOCATION_R;
//         *response_size = 3 + *length_in_message_payload;
//       }
//     break;
//     case WKPF_COMM_CMD_SET_LOCATION:
//       retval = wkpf_config_set_location_string((char*) payload+3, payload[2]);
//       if (retval == WKPF_OK) {
//         *response_cmd = WKPF_COMM_CMD_SET_LOCATION_R;
//         *response_size = 2;//payload size
//       } else {
//         payload[2] = retval;       
//         *response_cmd = WKPF_COMM_CMD_ERROR_R;
//         *response_size = 3;//payload size
//       }
//     break;
//     case WKPF_COMM_CMD_GET_FEATURES:
//       {
//         int count = 0;
//         for (int i=0; i<=WKPF_MAX_FEATURE_NUMBER; i++) { // Needs to be changed if we have more features than fits in a single message, but for now it will work fine.
//           if (wkpf_config_get_feature_enabled(i)) {
//             payload[3+count++] = i;
//           }
//         }
//         payload[2] = count;
//         *response_cmd = WKPF_COMM_CMD_GET_FEATURES_R;
//         *response_size = 3+count;//payload size
//       }
//     break;
//     case WKPF_COMM_CMD_SET_FEATURE:
//       retval = wkpf_config_set_feature_enabled(payload[2], payload[3]);
//       if (retval == WKPF_OK) {
//           *response_cmd = WKPF_COMM_CMD_SET_FEATURE_R;
//           *response_size = 2;//payload size
//         } else {
//           payload[2] = retval;       
//           *response_cmd = WKPF_COMM_CMD_ERROR_R;
//           *response_size = 3;//payload size
//         }
//     break;
//     case WKPF_COMM_CMD_GET_WUCLASS_LIST:
//       number_of_wuclasses = wkpf_get_number_of_wuclasses();
//       payload[2] = number_of_wuclasses;
//       for (int i=0; i<number_of_wuclasses && i<9; i++) { // TODONR: i<9 is temporary to keep the length within MESSAGE_SIZE, but we should have a protocol that sends multiple messages
//         wuclass_t *wuclass;
//         wkpf_get_wuclass_by_index(i, &wuclass);
//         payload[3*i + 3] = (uint8_t)(wuclass->wuclass_id >> 8);
//         payload[3*i + 4] = (uint8_t)(wuclass->wuclass_id);
//         payload[3*i + 5] = WKPF_IS_VIRTUAL_WUCLASS(wuclass) ? 1 : 0;
//       }
//       *response_size = 3*number_of_wuclasses + 3;//payload size 2*wuclasses + 2 bytes seqnr + 1 byte number of wuclasses
//       *response_cmd = WKPF_COMM_CMD_GET_WUCLASS_LIST_R;
//     break;
//     case WKPF_COMM_CMD_GET_WUOBJECT_LIST:
//       number_of_wuobjects = wkpf_get_number_of_wuobjects();
//       payload[2] = number_of_wuobjects;
//       for (int i=0; i<number_of_wuobjects && i<9; i++) { // TODONR: i<9 is temporary to keep the length within MESSAGE_SIZE, but we should have a protocol that sends multiple messages
//         wuobject_t *wuobject;
//         wkpf_get_wuobject_by_index(i, &wuobject);
//         payload[3*i + 3] = (uint8_t)(wuobject->port_number);
//         payload[3*i + 4] = (uint8_t)(wuobject->wuclass->wuclass_id >> 8);
//         payload[3*i + 5] = (uint8_t)(wuobject->wuclass->wuclass_id);
//       }
//       *response_size = 3*number_of_wuobjects + 3;//payload size 3*wuobjects + 2 bytes seqnr + 1 byte number of wuclasses
//       *response_cmd = WKPF_COMM_CMD_GET_WUOBJECT_LIST_R;
//     break;
//     case WKPF_COMM_CMD_READ_PROPERTY: // TODONR: check wuclassid
//       port_number = payload[2];
//       // TODONR: wuclass_id = (uint16_t)(payload[3]<<8)+(uint16_t)(payload[4]);
//       property_number = payload[5];
//       retval = wkpf_get_wuobject_by_port(port_number, &wuobject);
//       if (retval != WKPF_OK) {
//         payload [2] = retval;
//         *response_cmd = WKPF_COMM_CMD_ERROR_R;
//         *response_size = 3;//payload size
//         break;
//       }
//       uint8_t property_status;
//       wkpf_get_property_status(wuobject, property_number, &property_status);
//       if (WKPF_GET_PROPERTY_DATATYPE(wuobject->wuclass->properties[property_number]) == WKPF_PROPERTY_TYPE_SHORT) {
//         int16_t value;
//         retval = wkpf_external_read_property_int16(wuobject, property_number, &value);
//         payload[6] = WKPF_GET_PROPERTY_DATATYPE(wuobject->wuclass->properties[property_number]);
//         payload[7] = property_status;
//         payload[8] = (uint8_t)(value>>8);
//         payload[9] = (uint8_t)(value);
//         *response_size = 10;//payload size
//         *response_cmd = WKPF_COMM_CMD_READ_PROPERTY_R;        
//       } else if (WKPF_GET_PROPERTY_DATATYPE(wuobject->wuclass->properties[property_number]) == WKPF_PROPERTY_TYPE_BOOLEAN) {
//         bool value;
//         retval = wkpf_external_read_property_boolean(wuobject, property_number, &value);
//         payload[6] = WKPF_GET_PROPERTY_DATATYPE(wuobject->wuclass->properties[property_number]);
//         payload[7] = property_status;
//         payload[8] = (uint8_t)(value);
//         *response_size = 9;//payload size
//         *response_cmd = WKPF_COMM_CMD_READ_PROPERTY_R;                
//       } else if (WKPF_GET_PROPERTY_DATATYPE(wuobject->wuclass->properties[property_number]) == WKPF_PROPERTY_TYPE_REFRESH_RATE) {
//         wkpf_refresh_rate_t value;
//         retval = wkpf_external_read_property_refresh_rate(wuobject, property_number, &value);
//         payload[6] = WKPF_GET_PROPERTY_DATATYPE(wuobject->wuclass->properties[property_number]);
//         payload[7] = property_status;
//         payload[8] = (uint8_t)(value>>8);
//         payload[9] = (uint8_t)(value);
//         *response_size = 10;//payload size
//         *response_cmd = WKPF_COMM_CMD_READ_PROPERTY_R;        
//       } else
//         retval = WKPF_ERR_SHOULDNT_HAPPEN;
//       if (retval != WKPF_OK) {
//         payload [2] = retval;
//         *response_cmd = WKPF_COMM_CMD_ERROR_R;
//         *response_size = 3;//payload size
//       }
//     break;
//     case WKPF_COMM_CMD_WRITE_PROPERTY:
//       port_number = payload[2];
//       // TODONR: wuclass_id = (uint16_t)(payload[3]<<8)+(uint16_t)(payload[4]);
//       property_number = payload[5];
//       uint16_t wuclass_id;
//       link_entry link;
//       wuclass_id = (int16_t)(payload[3]);
//       wuclass_id = (int16_t)(wuclass_id<<8) + (int16_t)(payload[4]);
//       wkpf_get_link_by_dest_property_and_dest_wuclass_id(property_number, wuclass_id, &link);

//       // TODO: should we do that now?
//       // If the sender is not a leader
//       if (!wkpf_node_is_leader(link.src_component_id, src)) {
//         /**response_cmd = WKPF_COMM_CMD_ERROR_R;*/
//         /**response_size = 3;//payload size*/
//         /*break;*/
//       }

//       retval = wkpf_get_wuobject_by_port(port_number, &wuobject);
//       if (retval != WKPF_OK) {
//         payload [2] = retval;
//         *response_cmd = WKPF_COMM_CMD_ERROR_R;
//         *response_size = 3;//payload size
//         break;
//       }
//       if (payload[6] == WKPF_PROPERTY_TYPE_SHORT) {
//         int16_t value;
//         value = (int16_t)(payload[7]);
//         value = (int16_t)(value<<8) + (int16_t)(payload[8]);
//         retval = wkpf_external_write_property_int16(wuobject, property_number, value);
//         *response_size = 6;//payload size
//         *response_cmd = WKPF_COMM_CMD_WRITE_PROPERTY_R;        
//       } else if (payload[6] == WKPF_PROPERTY_TYPE_BOOLEAN) {
//         bool value;
//         value = (bool)(payload[7]);
//         retval = wkpf_external_write_property_boolean(wuobject, property_number, value);
//         *response_size = 6;//payload size
//         *response_cmd = WKPF_COMM_CMD_WRITE_PROPERTY_R;                
//       } else if (payload[6] == WKPF_PROPERTY_TYPE_REFRESH_RATE) {
//         int16_t value;
//         value = (int16_t)(payload[7]);
//         value = (int16_t)(value<<8) + (int16_t)(payload[8]);
//         retval = wkpf_external_write_property_refresh_rate(wuobject, property_number, value);
//         *response_size = 6;//payload size
//         *response_cmd = WKPF_COMM_CMD_WRITE_PROPERTY_R;
//       } else
//         retval = WKPF_ERR_SHOULDNT_HAPPEN;
//       if (retval != WKPF_OK) {
//         payload [2] = retval;
//         *response_cmd = WKPF_COMM_CMD_ERROR_R;
//         *response_size = 3;//payload size
//       }
//     break;
//     case WKPF_COMM_CMD_REQUEST_PROPERTY_INIT:
//       port_number = payload[2];
//       property_number = payload[3];
//       retval = wkpf_get_wuobject_by_port(port_number, &wuobject);
//       if (retval == WKPF_OK) {
//         retval = wkpf_property_needs_initialisation_push(wuobject, property_number);
//       }
//       if (retval != WKPF_OK) {
//         payload [2] = retval;
//         *response_cmd = WKPF_COMM_CMD_ERROR_R;
//         *response_size = 3;//payload size
//       } else {
//         *response_size = 6;//payload size
//         *response_cmd = WKPF_COMM_CMD_REQUEST_PROPERTY_INIT_R;                
//       }
//     break;
//   }
// }
