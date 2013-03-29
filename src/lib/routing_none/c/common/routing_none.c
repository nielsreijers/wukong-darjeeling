#include "types.h"

// Here we have a circular dependency between routing_X and wkcomm.
// Bit of a code smell, but since the two are supposed to be used together I'm leaving it like this for now.
// (wkcomm requires exactly 1 routing_ library to be linked in)
#include "wkcomm.h"

#ifdef RADIO_USE_ZWAVE
#include "radio_zwave.h"

bool addr_wkcomm_to_zwave(wkcomm_address_t wkcomm_addr, radio_zwave_address_t *zwave_addr) {
    // Temporary: addresses <128 are ZWave, addresses >=128 are XBee
    if (wkcomm_addr>=128)
        return false;
    *zwave_addr = wkcomm_addr;
    return true;
}

bool addr_zwave_to_wkcomm(wkcomm_address_t *wkcomm_addr, radio_zwave_address_t zwave_addr) {
    if (zwave_addr>=128)
        return false;
    *wkcomm_addr = zwave_addr;
    return true;
}
#endif // RADIO_USE_ZWAVE

#ifdef RADIO_USE_XBEE
#include "radio_xbee.h"
#endif // RADIO_USE_XBEE

uint8_t routing_send(wkcomm_address_t dest, uint8_t command, uint8_t *payload, uint8_t length, uint16_t seqnr) {
	DEBUG_LOG(DBG_WKCOMM, "routing_send\n");
	#ifdef RADIO_USE_ZWAVE
		retval = wkcomm_zwave_send(dest, command, payload, length, seqnr);
		if (retval == 0)
			return retval;
	#endif
	#ifdef RADIO_USE_XBEE
		retval = wkcomm_xbee_send(dest, command, payload, length, seqnr);
		if (retval == 0)
			return retval;
	#endif

	return retval;
}

void routing_handle_message(wkcomm_received_msg *message) {
	// Since this library doesn't contain any routing, we just always pass the message up to wkcomm.
	// In a real routing library there will probably be some messages to maintain the routing protocol
	// state that could be handled here, while messages meant for higher layers like wkpf and wkreprog
	// should be sent up to wkcomm.
	routing_wkcomm_handle_message(message);
}

