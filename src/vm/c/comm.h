#ifndef COMMH
#define COMMH

#include "types.h"

// Temporary file until I decide how to implement the communication layer properly.

// WuKong address. For now it's just a byte, but this will probably change.
// When it does, we need to change the component-node map as well
typedef uint8_t address_t;

#define nvmcomm_get_node_id()		((address_t)1)


#endif // COMMH
