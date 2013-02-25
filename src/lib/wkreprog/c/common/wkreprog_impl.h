#ifndef WKREPROG_IMPL_H
#define WKREPROG_IMPL_H

#include "types.h"

extern uint16_t wkreprog_impl_get_page_size();
extern void wkreprog_impl_open();
extern void wkreprog_impl_write(uint8_t size, uint8_t* data);
extern void wkreprog_impl_close();

#endif // WKREPROG_IMPL_H
