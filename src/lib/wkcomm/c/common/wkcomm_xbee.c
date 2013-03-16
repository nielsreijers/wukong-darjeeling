#include "config.h" // To get RADIO_USE_XBEE

#ifdef RADIO_USE_XBEE

#include "types.h"
#include "panic.h"
#include "wkcomm.h"

void wkcomm_xbee_init(void) {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
}

address_t wkcomm_xbee_get_node_id() {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
	return 1; // To keep the compiler happy.
}

void wkcomm_xbee_poll(void) {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
}

uint8_t wkcomm_xbee_send(address_t dest, uint8_t command, uint8_t *payload, uint8_t length, uint16_t seqnr) {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
	return 1; // To keep the compiler happy.
}


#endif
