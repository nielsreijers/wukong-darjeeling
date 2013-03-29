#ifndef ROUTING_NONEH
#define ROUTING_NONEH

#include "types.h"

// This will be called from wkcomm when it needs to send a message
extern uint8_t routing_send(wkcomm_address_t dest, uint8_t command, uint8_t *payload, uint8_t length, uint16_t seqnr);

// This will be called by the radios when it receives a message
extern void routing_handle_message(wkcomm_received_msg *message);

#endif // ROUTING_NONEH

