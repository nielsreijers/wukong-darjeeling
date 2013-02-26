#include "config.h" // To get RADIO_USE_ZWAVE

#ifdef RADIO_USE_ZWAVE

#include "types.h"
#include "panic.h"
#include "wkcomm.h"

void wkcomm_zwave_init(void) {
	// NOOP
}

address_t wkcomm_zwave_get_node_id() {
	return 1; // Temporary
}

void wkcomm_zwave_poll(void) {
	// dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
}

uint8_t wkcomm_zwave_send(address_t dest, uint8_t command, uint8_t *payload, uint8_t length, uint16_t seqnr) {
	// dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
	return 1; // To keep the compiler happy.
}


#endif
