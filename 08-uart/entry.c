/*
 * Raspberry Bare Metal
 * Copyright (C) 2014-2015 Federico "MrModd" Cosentino (http://mrmodd.it/)
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

void welcome(void)
{
	puts("\n=======================================================================\n");
	puts("       ____      _    ____  ____  ____  _____ ____  ______   __\n");
	puts("      |  _ \\    / \\  / ___||  _ \\| __ )| ____|  _ \\|  _ \\ \\ / /\n");
	puts("      | |_) |  / _ \\ \\___ \\| |_) |  _ \\|  _| | |_) | |_) \\ V / \n");
	puts("      |  _ <  / ___ \\ ___) |  __/| |_) | |___|  _ <|  _ < | |  \n");
	puts("      |_| \\_\\/_/   \\_\\____/|_|   |____/|_____|_| \\_\\_| \\_\\|_|  \n");
	puts("                                                    Bare Metal \n\n");
	puts("Copyright (C) 2014-2015 Federico \"MrModd\" Cosentino (http://mrmodd.it/)\n");
	puts("=======================================================================\n\n");
}

void entry(void)
{
	/* Generate a software interrupt to
	 * test the functionality of exception
	 * vector (set in init.c) */
	//asm volatile("swi 0");
	
	welcome();
	
	for(;;) {
		LED_ON
		loop_delay(5000000u);
		LED_OFF
		loop_delay(100000000u);
		puts("Tick!\n");
		
		/* Or */
		/*
		GPIO0_TOGGLE_BIT(16)
		loop_delay(10000000);
		*/
		
		/* Or */
		/*
		GPIO0_SET_LOW(16)
		loop_delay(10000000);
		GPIO0_SET_HIGH(16)
		loop_delay(10000000);
		*/
		
		/* Or */
		/*
		iomem(GPCLR0) |= (1<<16);
		loop_delay(10000000);
		iomem(GPSET0) |= (1<<16);
		loop_delay(10000000);
		*/
	}
}
