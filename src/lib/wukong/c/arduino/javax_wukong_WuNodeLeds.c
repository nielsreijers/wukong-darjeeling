#include <avr/io.h>
#include "execution.h"

#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

void javax_wukong_WuNodeLeds_void__setLed_int_boolean()
{
	unsigned char on = dj_exec_stackPopShort();
	unsigned int nr = dj_exec_stackPopInt();

	if (on)
		output_low(PORTK, nr);
	else
		output_high(PORTK, nr);
}

void javax_wukong_WuNodeLeds_void__init()
{
	set_output(DDRK, 0);
	set_output(DDRK, 1);
	set_output(DDRK, 2);
	set_output(DDRK, 3);
	output_high(PORTK, 0);
	output_high(PORTK, 1);
	output_high(PORTK, 2);
	output_high(PORTK, 3);
}