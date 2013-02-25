#include "types.h"
#include "wkreprog_impl.h"
#include <avr/boot.h>

uint16_t wkreprog_impl_get_page_size() {
	return SPM_PAGESIZE;
}

void wkreprog_impl_open() {

}

void wkreprog_impl_write(uint8_t size, uint8_t* data) {

}

void wkreprog_impl_close() {

}

