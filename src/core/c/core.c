#include "core.h"
#include "types.h"
#include "hooks.h"

// Runlevel. Used to pause the VM when reprogramming and reset it afterwards.
uint8_t dj_exec_runlevel;

// For libraries that need frequent polling. Currently just for radios, but maybe there are other uses. Should be fast.
dj_hook *dj_vm_pollingHook = NULL;
