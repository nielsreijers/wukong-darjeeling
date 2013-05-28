#ifndef WKREPROG_H
#define WKREPROG_H

#include "types.h"

extern bool wkreprog_open(uint8_t filenumber, uint16_t start_write_position);
extern void wkreprog_write(uint8_t size, uint8_t* data);
extern void wkreprog_close();

#endif // WKREPROG_H
