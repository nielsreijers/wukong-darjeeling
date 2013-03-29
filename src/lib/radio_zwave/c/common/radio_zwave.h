#ifndef RADIO_ZWAVEH
#define RADIO_ZWAVEH

#include "types.h"
#include "wkcomm.h"

typedef uint8_t radio_zwave_address_t;

extern void radio_zwave_init(void);
extern radio_zwave_address_t radio_zwave_get_node_id();
extern void radio_zwave_poll(void);
extern uint8_t radio_zwave_send(radio_zwave_address_t dest, uint8_t command, uint8_t *payload, uint8_t length, uint16_t seqnr);

#endif // RADIO_ZWAVEH