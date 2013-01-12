#ifndef HOOKS_H
#define HOOKS_H

typedef struct _dj_hook {
	void (*function) (void *);
    struct _dj_hook *next;
} dj_hook;

extern dj_hook *dj_vm_markRootSetHook;
extern dj_hook *dj_mem_updateReferenceHook;

extern void dj_hook_add(dj_hook **list, dj_hook *callback);
extern void dj_hook_call(dj_hook *list, void *data);

#endif // HOOKS_H
