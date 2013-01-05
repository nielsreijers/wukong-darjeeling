#include "jlib_base.h"
#include <avr/io.h>

#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

void javax_wukong_WuNodeLeds_void_setLed_int_boolean()
{
	unsigned char on = dj_exec_stackPopShort();
	unsigned int nr = dj_exec_stackPopInt();

	set_output(DDRK, 0);
	set_output(DDRK, 1);
	set_output(DDRK, 2);
	set_output(DDRK, 3);

	if (on)
		output_low(PORTK, nr);
	else
		output_high(PORTK, nr);
}
