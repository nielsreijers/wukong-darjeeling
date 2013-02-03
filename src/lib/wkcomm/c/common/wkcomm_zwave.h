#ifndef WKCOMM_ZWAVEH
#define WKCOMM_ZWAVEH

#include "config.h" // To get RADIO_USE_ZWAVE

#ifdef RADIO_USE_ZWAVE

#include "types.h"
#include "wkcomm.h"

extern void wkcomm_zwave_init(void);
extern address_t wkcomm_zwave_get_node_id();
extern void wkcomm_zwave_poll(void);
extern uint8_t wkcomm_zwave_send(address_t dest, uint8_t command, uint8_t *payload, uint8_t length);

#endif

#endif // WKCOMM_ZWAVEH