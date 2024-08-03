#include "hook.h"
#include <unistd.h>
#include <sys/mman.h>
#include <cstdint>

namespace customHook {
	int hook(void *target, func_t replace, func_t *backup) {
	    unsigned long pageSize = sysconf(_SC_PAGE_SIZE);
	    unsigned long size = pageSize * sizeof(uintptr_t);
	    void *p = (void *)((uintptr_t)target - ((uintptr_t)target %pageSize) - pageSize);
		// return DobbyHook(target, replace, backup);
		if (mprotect(p, (size_t)size, PROT_EXEC | PROT_READ | PROT_WRITE) == 0) { // error: expected expression
		    return DobbyHook(target, replace, backup);
		}
		return -1;
	}
}