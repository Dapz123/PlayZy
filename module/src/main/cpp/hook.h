#ifndef HOOK_H
#define HOOK_H

#include "include/dobby.h"
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <string>

typedef dobby_dummy_func_t func_t;

namespace customHook {
	int hook(void *target, func_t replace, func_t *backup);
}

#endif