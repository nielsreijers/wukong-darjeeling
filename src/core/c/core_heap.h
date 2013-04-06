#ifndef CORE_HEAPH
#define CORE_HEAPH

#include "types.h"

// The most basic memory management functions that need to be supported.
// For now, they're implemented in the vm lib, so this makes any config that doesn't include vm broken
// vm's memory management also supports freeing memory and garbage collection.
// In the future, the planned picokong library will provide a simpler allocate-only model

void dj_mem_init(void *mem_pointer, uint16_t mem_size);
void * dj_mem_alloc(uint16_t size, runtime_id_t id);
uint16_t dj_mem_getFree();
uint16_t dj_mem_getSize();


#endif // CORE_HEAPH