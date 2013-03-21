#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "wkreprog_impl.h"

extern char** posix_argv;

uint8_t *wkreprog_impl_buffer;
int wkreprog_impl_pos;

uint16_t wkreprog_impl_get_page_size() {
	return 256;
}

bool wkreprog_impl_open(uint16_t size_to_upload) {
	wkreprog_impl_buffer = malloc(size_to_upload);
	memset(wkreprog_impl_buffer, 0, size_to_upload);
	wkreprog_impl_pos = 0;
	return (wkreprog_impl_buffer != NULL);
}

void wkreprog_impl_write(uint8_t size, uint8_t* data) {
	memcpy(wkreprog_impl_buffer+wkreprog_impl_pos, data, size);
	wkreprog_impl_pos += size;
}

void wkreprog_impl_close() {
  FILE *fp = fopen("app_infusion.dja", "w");
  if (fp== NULL) {
	  printf("Error in opening file to write infusion to...\n");
  }
  // Skip first 4 bytes we get, which contain the length of the archive (needed for targets like AVR, but not for native since we'll just read the file from disk)
  fwrite(wkreprog_impl_buffer+4, wkreprog_impl_pos-4, 1, fp);
  fclose(fp);
}

void wkreprog_impl_reboot() {
	execvp(posix_argv[0], posix_argv);
}
