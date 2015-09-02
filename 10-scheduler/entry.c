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

static void led_cycle(void *arg __attribute__((unused)))
{
	LED_ON
	delay_ms(100);
	LED_OFF
}

static void show_ticks(void *arg __attribute__((unused)))
{
	puts("Tick! ");putu(SYSTEM_TICKS);puts("\r");
}

void entry(void)
{
	/* Generate a software interrupt to
	 * test the functionality of exception
	 * vector (set in init.c) */
	//asm volatile("swi 0");
	
	welcome();
	
	if (create_task(led_cycle,
			NULL,
			get_ticks_in_sec(2) /* Every 2 seconds */,
			5 /* Initial phase */,
			get_ticks_in_sec(2) /* Priority (Rate Monotonic: proportional to his frequency) */,
			"led_cycle") == -1) {
		puts("ERROR: cannot create task led_cycle\n");
		panic0();
	}

	if (create_task(show_ticks,
			NULL,
			get_ticks_in_sec(1) /* Every second */,
			5 /* Initial phase */,
			get_ticks_in_sec(1) /* Priority (Rate Monotonic: proportional to his frequency) */,
			"show_ticks") == -1) {
		puts("ERROR: cannot create task show_ticks\n");
		panic0();
	}
	
	run_periodic_tasks();
}
