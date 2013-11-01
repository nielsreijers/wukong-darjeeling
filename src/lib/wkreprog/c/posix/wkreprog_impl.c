#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "types.h"
#include "djarchive.h"
#include "wkreprog_impl.h"

#include "posix_utils.h"

// The posix implementation writes to two places
// simultaneously.
//  - fp will be a file handle to app_infusion.dja,
// positioned at the right location in the file.
// Writing to fp persists the changes across reboots.
//  - in_memory_pointer will point to the copy of
// app_infusion.dja loaded in main memory. Writing to
// this makes the changes immediately available to the
// application.
FILE *fp;
void *in_memory_pointer;

uint16_t wkreprog_impl_get_page_size() {
	return 256;
}

bool wkreprog_impl_open(uint16_t start_write_position) {
	fp = fopen("app_infusion.dja", "rb+");
	if (fp== NULL) {
		printf("Error in opening file to write infusion to...\n");
	}
	fseek(fp, start_write_position, SEEK_SET);
	in_memory_pointer = (void *)di_app_archive + start_write_position;
	return true;
}

void wkreprog_impl_write(uint8_t size, uint8_t* data) {
	assert(fp != NULL);
	// Write to file
	fwrite(data, size, 1, fp);
	// Write to memory
	memcpy(in_memory_pointer, data, size);
	in_memory_pointer += size;
}

void wkreprog_impl_close() {
	assert(fp != NULL);
	fclose(fp);
}

void wkreprog_impl_reboot() {
	execvp(posix_argv[0], posix_argv);
}
