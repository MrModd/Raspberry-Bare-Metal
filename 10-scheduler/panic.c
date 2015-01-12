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
