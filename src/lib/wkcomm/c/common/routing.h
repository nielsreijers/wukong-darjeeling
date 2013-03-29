#ifndef ROUTING_NONEH
#define ROUTING_NONEH

// This file in the wkcomm directory instead of a specific routing library 
// because the interface for all routing protocols should be the same.

#include "types.h"
#include "config.h"

extern void routing_init();

// This will be called from wkcomm when it needs to send a message
extern uint8_t routing_send(wkcomm_address_t dest, uint8_t *payload, uint8_t length);

// These will be called by the radios when it receives a message
#ifdef RADIO_USE_ZWAVE
#include "radio_zwave.h"
extern void routing_handle_zwave_message(radio_zwave_address_t zwave_addr, uint8_t *payload, uint8_t length);
#endif // RADIO_USE_ZWAVE

#ifdef RADIO_USE_XBEE
#include "radio_zwave.h"
extern void routing_handle_xbee_message(radio_xbee_address_t xbee_addr, uint8_t *payload, uint8_t length);
#endif // RADIO_USE_XBEE


#endif // ROUTING_NONEH

