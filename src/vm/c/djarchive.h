#ifndef DJARCHIVEH
#define DJARCHIVEH

#include "program_mem.h"
#include "types.h"

// Archive format:
// Repeated for each file:
//   4 bytes file size
//   1 byte file type
//   X bytes file data
// 4 bytes: 00 00 00 00

#define FILETYPE_LIB_INFUSION 0
#define FILETYPE_APP_INFUSION 1
#define FILETYPE_WKPF_TABLE   2

#define dj_archive_filesize(file) (dj_di_getU32(file-3))
#define dj_archive_filetype(file) (dj_di_getU8(file-1))

uint8_t dj_archive_number_of_files(dj_di_pointer archive);
dj_di_pointer dj_archive_get_file(dj_di_pointer archive, uint8_t filenumber);

#endif // DJARCHIVEH
