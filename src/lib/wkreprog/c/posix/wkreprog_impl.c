#include <unistd.h>
#include "types.h"
#include "wkreprog_impl.h"

extern char** posix_argv;

uint16_t wkreprog_impl_get_page_size() {
	return 256;
}

void wkreprog_impl_open() {

}

void wkreprog_impl_write(uint8_t size, uint8_t* data) {

}

void wkreprog_impl_close() {

}

void wkreprog_impl_reboot() {
	execvp(posix_argv[0], posix_argv);
}
