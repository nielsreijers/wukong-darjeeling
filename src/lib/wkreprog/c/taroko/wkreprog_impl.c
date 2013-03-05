#include "panic.h"
#include "wkreprog_impl.h"

extern char** posix_argv;

uint8_t *wkreprog_impl_buffer;
int wkreprog_impl_pos;

uint16_t wkreprog_impl_get_page_size() {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
}

bool wkreprog_impl_open(uint16_t size_to_upload) {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
}

void wkreprog_impl_write(uint8_t size, uint8_t* data) {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
}

void wkreprog_impl_close() {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
}

void wkreprog_impl_reboot() {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
}
