#include "panic.h"
#include "debug.h"
#include "config.h"
#include "wkcomm.h"
#include "wkcomm_zwave.h"
#include "wkcomm_xbee.h"

// Initialise wkcomm and whatever protocols are enabled.
void wkcomm_init(void) {
	#ifdef RADIO_USE_ZWAVE
		wkcomm_zwave_init();
	#endif
	#ifdef RADIO_USE_XBEE
		wkcomm_xbee_init();
	#endif
}

// Get my own node id
address_t wkcomm_get_node_id() {
	// TODO: This doesn't work for xbee yet, but it didn't in nanovm either.
	#ifdef RADIO_USE_ZWAVE
		return wkcomm_zwave_get_node_id();
	#endif
	#ifdef RADIO_USE_XBEE
		return wkcomm_xbee_get_node_id();
	#endif
	return 1; // Just return 1 if we have no radios at all.
}

// Call this periodically to receive data
void wkcomm_poll(void) {
	#ifdef RADIO_USE_ZWAVE
		wkcomm_zwave_poll();
	#endif
	#ifdef RADIO_USE_XBEE
		wkcomm_xbee_poll();
	#endif
}

// Send length bytes to dest
int wkcomm_send(address_t dest, uint8_t command, uint8_t *payload, uint8_t length) {
	if (length > WKCOMM_MESSAGE_SIZE) {
		DEBUG_LOG(DBG_COMM, "message oversized\n");
		return -2; // Message too large
	}
	int retval = -1;
	DEBUG_LOG(DBG_COMM, "wkcomm_send\n");
	#ifdef RADIO_USE_ZWAVE
		retval = wkcomm_zwave_send(dest, command, payload, length);
		if (retval == 0)
			return retval;
	#endif
	#ifdef RADIO_USE_XBEE
		retval = wkcomm_xbee_send(dest, command, payload, length);
		if (retval == 0)
			return retval;
	#endif
	return retval;
}

// Wait for a message of a specific type, while still handling messages of other types
wkcomm_message *wkcomm_wait(uint16_t wait_msec, uint8_t *commands, uint8_t number_of_commands) {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
	return NULL; // To keep the compiler happy.
}


