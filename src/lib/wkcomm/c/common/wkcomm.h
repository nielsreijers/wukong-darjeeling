#ifndef WKCOMMH
#define WKCOMMH

#include "types.h"

#define WKCOMM_MESSAGE_SIZE   0x20

#define WKCOMM_SEND_OK					 0
#define WKCOMM_SEND_ERR_NOT_HANDLED		 2 // None of the available protocols (XBee/ZWave) could send this message
#define WKCOMM_SEND_ERR_TOO_LONG		 3
#define WKCOMM_SEND_ERR_NO_REPLY		 4

// WuKong address. For now it's just a byte, but this will probably change.
// When it does, we need to change the component-node map as well
typedef uint8_t address_t;

typedef struct wkcomm_received_msg {
	address_t src;
	uint16_t seqnr;
	uint8_t command;
	uint8_t *payload;
	uint8_t length;
} wkcomm_received_msg;


// Message handling. This function is called from the radio code (wkcomm_zwave_poll or wkcomm_xbee_poll), checks for replies we may be waiting for, or passes on the handling to one of the other libs.
extern void wkcomm_handle_message(wkcomm_received_msg *message);

// Initialise wkcomm and whatever protocols are enabled. Called from javax_wukong_wkcomm_WKComm_void__init()
extern void wkcomm_init(void);

// Get my own node id
extern address_t wkcomm_get_node_id();

// Call this periodically to receive data
extern void wkcomm_poll(void);

// Send length bytes to dest
extern int wkcomm_send(address_t dest, uint8_t command, uint8_t *payload, uint8_t length);

// Send length bytes to dest
extern int wkcomm_send_reply(address_t dest, uint8_t command, uint8_t *payload, uint8_t length, wkcomm_received_msg *received_msg);

// Send length bytes to dest and wait for a specific reply (and matching sequence nr)
extern int wkcomm_send_and_wait_for_reply(address_t dest, uint8_t command, uint8_t *payload, uint8_t length, uint16_t wait_msec, uint8_t *commands, uint8_t number_of_commands, wkcomm_received_msg **reply);

#endif // WKCOMMH