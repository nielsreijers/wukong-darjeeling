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
 
#include <stdio.h>
#include <stdlib.h>

#include "execution.h"
#include "panic.h"
#include "avr.h"

void dj_panic(int32_t panictype)
{
    switch(panictype)
    {
        case DJ_PANIC_OUT_OF_MEMORY:
#ifdef DARJEELING_DEBUG
        	avr_serialPrint("PANIC: out of memory!\n");
#endif
            break;
        case DJ_PANIC_ILLEGAL_INTERNAL_STATE:
#ifdef DARJEELING_DEBUG
        	avr_serialPrint("PANIC: illegal internal state!\n");
#endif
            break;
        case DJ_PANIC_UNIMPLEMENTED_FEATURE:
#ifdef DARJEELING_DEBUG
        	avr_serialPrint("PANIC: unimplemented feature!\n");
#endif
            break;
        case DJ_PANIC_UNCAUGHT_EXCEPTION:
#ifdef DARJEELING_DEBUG
        	avr_serialPrint("PANIC: uncaught exception!\n");
#endif
        case DJ_PANIC_UNSATISFIED_LINK:
#ifdef DARJEELING_DEBUG
            avr_serialPrint("PANIC: unsatisfied link!\n");
#endif
            break;
        case DJ_PANIC_MALFORMED_INFUSION:
#ifdef DARJEELING_DEBUG
        	avr_serialPrint("PANIC: malformed infusion!\n");
#endif
            break;
        case DJ_PANIC_ASSERTION_FAILURE:
#ifdef DARJEELING_DEBUG
            avr_serialPrint("PANIC: assertion failed!\n");
#endif
            break;
        case DJ_PANIC_SAFE_POINTER_OVERFLOW:
#ifdef DARJEELING_DEBUG
            avr_serialPrint("PANIC: safe pointer overflow!\n");
#endif
            break;
        default:
#ifdef DARJEELING_DEBUG
            avr_serialPrint("PANIC: unknown panic type %d!\n", panictype);
#endif
            break;
    }
    exit(-1);
}
