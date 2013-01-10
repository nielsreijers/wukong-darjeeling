#include "types.h"
#include "debug.h"
#include "heap.h"
#include "wkpf.h"
#include "wkpf_wuclasses.h"

wkpf_wuclass_definition *wuclasses = NULL;

uint8_t wkpf_register_wuclass(uint16_t wuclass_id, update_function_t update, uint8_t number_of_properties, uint8_t properties[]) {
  wkpf_wuclass_definition *wuclass;
  if (wkpf_get_wuclass_by_id(wuclass_id, &wuclass) != WKPF_ERR_WUCLASS_NOT_FOUND) {
      DEBUG_LOG(DBG_WKPF, "WKPF: WuClass id in use while registering wuclass id %x: FAILED\n", wuclass.wuclass_id);
      return WKPF_ERR_WUCLASS_ID_IN_USE;
  }

  // Allocate memory for the new wuclass
  uint16_t size = sizeof(wkpf_wuclass_definition) + number_of_properties;
  dj_mem_addSafePointer((void**)&properties); // dj_mem_alloc may cause GC to run, so the address of the properties may change. this tells the GC to update our pointer if it does.
  wuclass = (wkpf_wuclass_definition*)dj_mem_alloc(size, CHUNKID_WUCLASS);
  dj_mem_removeSafePointer((void**)&properties);
  if (wuclass == NULL) {
    DEBUG_LOG(DBG_WKPF, "WKPF: Out of memory while registering wuclass id %x: FAILED\n", wuclass.wuclass_id);
    return WKPF_ERR_OUT_OF_MEMORY;
  }

  DEBUG_LOG(DBG_WKPF, "WKPF: Registering wuclass id %x at index %x\n", wuclass.wuclass_id, number_of_wuclasses);
  wuclass->wuclass_id = wuclass_id;
  wuclass->update = update;
  wuclass->number_of_properties = number_of_properties;
  for (int i=0; i<number_of_properties; i++)
    wuclass->properties[i] = properties[i];
  wuclass->next = wuclasses;
  wuclasses = wuclass;

  return WKPF_OK;
}

uint8_t wkpf_get_wuclass_by_id(uint16_t wuclass_id, wkpf_wuclass_definition **wuclass) {
  *wuclass = wuclasses;
  while (*wuclass) {
    if ((*wuclass)->wuclass_id == wuclass_id)
      return WKPF_OK;
    *wuclass = (*wuclass)->next;
  }
  DEBUG_LOG(DBG_WKPF, "WKPF: No wuclass with id %x found: FAILED\n", wuclass_id);
  return WKPF_ERR_WUCLASS_NOT_FOUND;
}

uint8_t wkpf_get_wuclass_by_index(uint8_t index, wkpf_wuclass_definition **wuclass) {
  *wuclass = wuclasses;
  while (index > 0 && wuclass != NULL) {
    index--;
    *wuclass = (*wuclass)->next;
  }
  if (*wuclass)
    return WKPF_OK;
  else
    return WKPF_ERR_WUCLASS_NOT_FOUND;
}

uint8_t wkpf_get_number_of_wuclasses() {
  int number_of_wuclasses = 0;
  wkpf_wuclass_definition *wuclass = wuclasses;
  while (wuclass) {
    number_of_wuclasses++;
    wuclass = wuclass->next;
  }
  return number_of_wuclasses;
}
