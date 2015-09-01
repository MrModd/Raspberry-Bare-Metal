/*
 * Raspberry Bare Metal
 * Copyright (C) 2014  Federico "MrModd" Cosentino (http://mrmodd.it/)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "raspberry.h"

/* System panic function.
 * Print an error message and halt the system.
 * @file: the file where the panic happened (use __FILE__ macro)
 * @line: the line number in the file (use __LINE__ macro)
 * @msg: a message that will be printed on the serial before panic
 */
inline void _panic(const char *file, int line, const char *msg)
{
	irq_disable();
	puts("\n\nPANIC!\n");
	puts(file);
	puts(":");
	putd(line);
	puts(": ");
	puts(msg);
	for(;;) {
		LED_ON
		loop_delay(5000000u);
		LED_OFF
		loop_delay(10000000u);
	}
}

/* Next panic functions should be called just by the hardware.
 * See the exception vector in init.c */

void panic0(void)
{
	for(;;) {
		LED_ON
		loop_delay(10000000u);
		LED_OFF
		loop_delay(10000000u);
	}
}

void panic1(void)
{
	for(;;) {
		LED_ON
		loop_delay(50000000u);
		LED_OFF
		loop_delay(10000000u);
	}
}

void panic2(void)
{
	for(;;) {
		LED_ON
		loop_delay(10000000u);
		LED_OFF
		loop_delay(50000000u);
	}
}

void panic3(void)
{
	for(;;) {
		LED_ON
		loop_delay(10000000u);
		LED_OFF
		loop_delay(10000000u);
		LED_ON
		loop_delay(10000000u);
		LED_OFF
		loop_delay(10000000u);
		LED_ON
		loop_delay(10000000u);
		LED_OFF
		loop_delay(50000000u);
	}
}

/* SOS */
void panic4(void)
{
	for(;;) {
		LED_ON
		loop_delay(5000000u);
		LED_OFF
		loop_delay(5000000u);
		LED_ON
		loop_delay(5000000u);
		LED_OFF
		loop_delay(5000000u);
		LED_ON
		loop_delay(5000000u);
		LED_OFF
		
		loop_delay(10000000u);
		
		LED_ON
		loop_delay(20000000u);
		LED_OFF
		loop_delay(5000000u);
		LED_ON
		loop_delay(20000000u);
		LED_OFF
		loop_delay(5000000u);
		LED_ON
		loop_delay(20000000u);
		LED_OFF
		
		loop_delay(10000000u);
		
		LED_ON
		loop_delay(5000000u);
		LED_OFF
		loop_delay(5000000u);
		LED_ON
		loop_delay(5000000u);
		LED_OFF
		loop_delay(5000000u);
		LED_ON
		loop_delay(5000000u);
		LED_OFF
		loop_delay(50000000u);
	}
}
