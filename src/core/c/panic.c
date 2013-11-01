/*
 * panic.c
 * 
 * Copyright (c) 2008-2010 CSIRO, Delft University of Technology.
 * 
 * This file is part of Darjeeling.
 * 
 * Darjeeling is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Darjeeling is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with Darjeeling.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdlib.h>

#include "panic.h"
#include "debug.h"
#include "hooks.h"
#include "core.h"

void dj_panic(int32_t panictype)
{
    switch(panictype)
    {
        case DJ_PANIC_OUT_OF_MEMORY:
        	DEBUG_LOG(true, "PANIC: out of memory!\n");
            break;
        case DJ_PANIC_ILLEGAL_INTERNAL_STATE:
        	DEBUG_LOG(true, "PANIC: illegal internal state!\n");
            break;
        case DJ_PANIC_UNIMPLEMENTED_FEATURE:
        	DEBUG_LOG(true, "PANIC: unimplemented feature!\n");
            break;
        case DJ_PANIC_UNCAUGHT_EXCEPTION:
        	DEBUG_LOG(true, "PANIC: uncaught exception!\n");
            break;
        case DJ_PANIC_UNSATISFIED_LINK:
            DEBUG_LOG(true, "PANIC: unsatisfied link!\n");
            break;
        case DJ_PANIC_MALFORMED_INFUSION:
        	DEBUG_LOG(true, "PANIC: malformed infusion!\n");
            break;
        case DJ_PANIC_ASSERTION_FAILURE:
            DEBUG_LOG(true, "PANIC: assertion failed!\n");
            break;
        case DJ_PANIC_SAFE_POINTER_OVERFLOW:
            DEBUG_LOG(true, "PANIC: safe pointer overflow!\n");
            break;
        default:
            DEBUG_LOG(true, "PANIC: unknown panic type %d!\n", panictype);
            break;
    }
    if (dj_exec_getRunlevel() < RUNLEVEL_PANIC) {
        dj_exec_setRunlevel(panictype);
        while (true) // Still allow remote access through wkcomm when in panic state.
            dj_hook_call(dj_core_pollingHook, NULL);
    } else {
        exit(panictype); // To avoid getting into a recursive panic.
    }
}
