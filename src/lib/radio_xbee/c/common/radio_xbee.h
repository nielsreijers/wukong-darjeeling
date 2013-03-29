#ifndef WKCOMM_XBEEH
#define WKCOMM_XBEEH

#include "types.h"

typedef uint8_t radio_xbee_address_t;

extern void wkcomm_xbee_init(void);
extern radio_xbee_address_t wkcomm_xbee_get_node_id();
extern void wkcomm_xbee_poll(void);
extern uint8_t radio_xbee_send(radio_xbee_address_t dest, uint8_t *payload, uint8_t length);

#endif // WKCOMM_XBEEH
