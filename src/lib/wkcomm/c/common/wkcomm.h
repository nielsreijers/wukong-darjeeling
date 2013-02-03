#ifndef WKCOMMH
#define WKCOMMH

#include "types.h"

#define WKCOMM_MESSAGE_SIZE   0x20

// WuKong address. For now it's just a byte, but this will probably change.
// When it does, we need to change the component-node map as well
typedef uint8_t address_t;

typedef struct wkcomm_message {
  address_t from;
  address_t to;
  uint8_t command;
  uint8_t *payload;
  uint8_t payload_length;
} wkcomm_message;

// Initialise wkcomm and whatever protocols are enabled.
extern void wkcomm_init(void);

// Get my own node id
extern address_t wkcomm_get_node_id();

// Call this periodically to receive data
extern void wkcomm_poll(void);

// Send length bytes to dest
extern int wkcomm_send(address_t dest, uint8_t command, uint8_t *payload, uint8_t length);

// Wait for a message of a specific type, while still handling messages of other types
extern wkcomm_message *wkcomm_wait(uint16_t wait_msec, uint8_t *commands, uint8_t number_of_commands);

#endif // WKCOMMH