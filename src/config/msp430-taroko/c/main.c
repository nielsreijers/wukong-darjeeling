/*
 * main.c
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

#include "debug.h"
#include "vm.h"
#include "heap.h"
#include "infusion.h"
#include "types.h"
#include "vmthread.h"
#include "djtimer.h"
#include "execution.h"
#include "hooks.h"

#include "jlib_base.h"
#include "jlib_darjeeling3.h"
#include "jlib_uart.h"
#include "jlib_wkcomm.h"
#include "jlib_wkpf.h"
#include "jlib_wkreprog.h"

#include "pointerwidth.h"

extern unsigned char di_lib_infusions_archive_data[];
extern unsigned char di_app_infusion_data[];

unsigned char mem[HEAPSIZE];

int main()
{
	// TODONR How does this work on Taroko?
	// initialise serial port
	// avr_serialInit(115200);

	dj_named_native_handler handlers[] = {
			{ "base", &base_native_handler },
			{ "darjeeling3", &darjeeling3_native_handler },
			{ "uart", &uart_native_handler },
			{ "wkcomm", &wkcomm_native_handler },
			{ "wkpf", &wkpf_native_handler },
			{ "wkreprog", &wkreprog_native_handler },
		};
	uint16_t length = sizeof(handlers)/ sizeof(handlers[0]);

	dj_vm_main(mem, HEAPSIZE, (dj_di_pointer)di_lib_infusions_archive_data, (dj_di_pointer)di_app_infusion_data, handlers, length);

	// Listen to the radio
	while(true)
		dj_hook_call(dj_vm_pollingHook, NULL);

	return 0;
}
