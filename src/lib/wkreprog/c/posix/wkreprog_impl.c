#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "types.h"
#include "wkreprog_impl.h"

#include "posix_utils.h"

FILE *fp;

uint16_t wkreprog_impl_get_page_size() {
	return 256;
}

bool wkreprog_impl_open(uint16_t start_write_position) {
	fp = fopen("app_infusion.dja", "rb+");
	if (fp== NULL) {
		printf("Error in opening file to write infusion to...\n");
	}
	fseek(fp, start_write_position, SEEK_SET);
	return true;
}

void wkreprog_impl_write(uint8_t size, uint8_t* data) {
	assert(fp != NULL);
	fwrite(data, size, 1, fp);
}

void wkreprog_impl_close() {
	assert(fp != NULL);
	fclose(fp);
}

void wkreprog_impl_reboot() {
	execvp(posix_argv[0], posix_argv);
}
