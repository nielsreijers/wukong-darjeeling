#ifndef WKCOMM_XBEEH
#define WKCOMM_XBEEH

#include "config.h" // To get RADIO_USE_XBEE

#ifdef RADIO_USE_XBEE

#include "types.h"
#include "wkcomm.h"

extern void wkcomm_xbee_init(void);
extern address_t wkcomm_xbee_get_node_id();
extern void wkcomm_xbee_poll(void);
extern uint8_t wkcomm_xbee_send(address_t dest, uint8_t command, uint8_t *payload, uint8_t length);

#endif

#endif // WKCOMM_XBEEH