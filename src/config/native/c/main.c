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

#include "jlib_base.h"
#include "jlib_darjeeling2.h"
#include "jlib_uart.h"
#include "jlib_wkcomm.h"
#include "jlib_wkpf.h"
#include "jlib_wkreprog.h"

#include "types.h"
#include "vm.h"
#include "heap.h"
#include "execution.h"
#include "config.h"

#include "pointerwidth.h"
char * ref_t_base_address;

// TODONR: this is necessary for the functions in ProgramFlash in java
//         Doesn't seem to work now, butjust leave it here until
//         we decide to remove or finish it.
FILE * progflashFile;
char** posix_argv;


char* load_infusion_archive(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (!fp) {
		printf("Unable to open the program flash file.\n");
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
	di_archive_data[0] = (length >> 0) % 256;
	di_archive_data[1] = (length >> 8) % 256;
	di_archive_data[2] = (length >> 16) % 256;
	di_archive_data[3] = (length >> 24) % 256;

	// Read the file into memory
	rewind(fp);
	fread(di_archive_data+4, sizeof(char), length, fp); // Skip 4 bytes that contain the archive length
	fclose(fp);

	return di_archive_data;
}

int main(int argc,char* argv[])
{
	posix_argv = argv;

	// Read the lib and app infusion archives from file
	char* di_lib_archive_data = load_infusion_archive("lib_infusions");
	char* di_app_archive_data = load_infusion_archive("app_infusions");

	// initialise memory manager
	void *mem = malloc(MEMSIZE);
	ref_t_base_address = (char*)mem - 42;

	// Initialise the simulated program flash
	// TODONR: Refactor native config later to load infusion from a file instead of linked in
	// init_progflash();

	dj_named_native_handler handlers[] = {
			{ "base", &base_native_handler },
			{ "darjeeling2", &darjeeling2_native_handler },
			{ "uart", &uart_native_handler },
			{ "wkcomm", &wkcomm_native_handler },
			{ "wkpf", &wkpf_native_handler },
			{ "wkreprog", &wkreprog_native_handler },
		};
	int length = sizeof(handlers)/ sizeof(handlers[0]);

	dj_vm_main(mem, MEMSIZE, (dj_di_pointer)di_lib_archive_data, (dj_di_pointer)di_app_archive_data, handlers, length);

	return 0;
}
