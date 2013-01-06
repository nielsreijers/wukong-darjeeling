#include <stdio.h>
#include "jlib_base.h"

void javax_wukong_WuNodeLeds_void__setLed_int_boolean()
{
	unsigned char on = dj_exec_stackPopShort();
	unsigned int nr = dj_exec_stackPopInt();

	printf("Turning LED %d %s\n", nr, on ? "on" : "off");
}

void javax_wukong_WuNodeLeds_void__init()
{
}