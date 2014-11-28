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

volatile unsigned int *gpio = (unsigned int *) GPIO_BASE;

void main(void)
{
	/* Init GPIO 16 (wired to LED ACT) as Output */
	gpio[GPFSEL1/4] |= (1<<18);
	
	/* Alternativelly: */
	//gpio[GPFSEL1/4] &= ~((1<<20)|(1<<19)|(1<<18));
	//gpio[GPFSEL1/4] |= ((0<<20)|(0<<19)|(1<<18));
	
	/* Set bit related to GPIO 16 low */
	gpio[GPCLR0/4] |= (1<<16);
	
	for(;;)
		;
}
