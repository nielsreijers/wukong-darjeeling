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
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "jlib_base.h"
#include "jlib_darjeeling3.h"
// #include "jlib_uart.h"
// #include "jlib_wkcomm.h"
// #include "jlib_wkpf.h"
// #include "jlib_wkreprog.h"

#include "core.h"
#include "types.h"
#include "vm.h"
#include "heap.h"
#include "execution.h"
#include "config.h"
#include "hooks.h"
#include "djarchive.h"

#include "pointerwidth.h"
char * ref_t_base_address;

// TODONR: this is necessary for the functions in ProgramFlash in java
//         Doesn't seem to work now, butjust leave it here until
//         we decide to remove or finish it.
FILE * progflashFile;
char** posix_argv;
char* posix_uart_filenames[4];

void parse_uart_arg(char *arg) {
	int uart = arg[0];
	uart -= '0';
	if (uart < 0 || uart > 3 || arg[1]!='=') {
		printf("option -u/--uart format: <uart>=<file>, where <uart> is 0, 1, 2, or 3 and <file> is the device to connect to thise uart.\n");
		abort();
	}
	posix_uart_filenames[uart] = arg+2;
	printf("Uart %d at %s\n", uart, posix_uart_filenames[uart]);
}

void parse_command_line(int argc,char* argv[]) {
	int c;
	while (1) {
		static struct option long_options[] = {
			{"uart",    required_argument, 0, 'u'},
			{0, 0, 0, 0}
		};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "u:",
		    long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
			case 'u':
				parse_uart_arg(optarg);
			break;

			default:
				abort ();
		}
	}
}


char* load_infusion_archive(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (!fp) {
		printf("Unable to open the program flash file %s.\n", filename);
		exit(1);
	}

	// Determine the size of the archive
	fseek(fp, 0, SEEK_END);
	size_t length = ftell(fp);

	// Allocate memory for the archive
	char* di_archive_data = malloc(length+4);
	if (!di_archive_data) {
		printf("Unable to allocate memory to load the program flash file.\n");
		exit(1);
	}

	// Read the file into memory
	rewind(fp);
	fread(di_archive_data, sizeof(char), length, fp); // Skip 4 bytes that contain the archive length
	fclose(fp);

	return di_archive_data;
}

int main(int argc,char* argv[])
{
	parse_command_line(argc, argv);
	posix_argv = argv;

	// Read the lib and app infusion archives from file
	char* di_lib_infusions_archive_data = load_infusion_archive("lib_infusions.dja");
	char* di_app_infusion_archive_data = load_infusion_archive("app_infusion.dja");

	// initialise memory manager
	void *mem = malloc(HEAPSIZE);
	ref_t_base_address = (char*)mem - 42;

	// Initialise the simulated program flash
	// TODONR: Refactor native config later to load infusion from a file instead of linked in
	// init_progflash();

	dj_named_native_handler handlers[] = {
			{ "base", &base_native_handler },
			{ "darjeeling3", &darjeeling3_native_handler },
			// { "uart", &uart_native_handler },
			// { "wkcomm", &wkcomm_native_handler },
			// { "wkpf", &wkpf_native_handler },
			// { "wkreprog", &wkreprog_native_handler },
		};
	int length = sizeof(handlers)/ sizeof(handlers[0]);

	dj_vm_main(mem, HEAPSIZE, (dj_di_pointer)di_lib_infusions_archive_data, (dj_di_pointer)di_app_infusion_archive_data, handlers, length);

	// Listen to the radio
	while(true)
		dj_hook_call(dj_vm_pollingHook, NULL);

	return 0;
}
