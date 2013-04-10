#include "hooks.h"
#include "core.h"
#include "wkcomm.h"

dj_hook wkcomm_pollingHook;

void wkcomm_init() {
	wkcomm_pollingHook.function = wkcomm_poll;
	dj_hook_add(&dj_vm_pollingHook, &wkcomm_pollingHook);
	wkcomm_radio_init();
}
