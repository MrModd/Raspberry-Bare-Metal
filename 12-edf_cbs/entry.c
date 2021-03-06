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

static void cbs_worker(void *arg)
{
	struct cbs_queue *q = (struct cbs_queue *) arg;
	struct task *t = q->task;
	static unsigned int count = 0;

	puts("CBS server: job ");
	putu(++count);
	puts(" prio=");
	putu(t->priority);
	puts(" nextrel=");
	putu(t->releasetime);
	puts(" pending=");
	putu(q->pending[0]);
	puts(" budget=");
	putu(t->budget);
	puts("\n");
	
	/* Wasting time... */
	delay_ms(2000);
}

static void led_cycle(void *arg)
{
	LED_ON
	delay_ms(100);
	LED_OFF
	
	/* Use the worker ID pointed by arg to
	 * release an aperiodic job of that worker */
	activate_cbs_worker(&cbs0, *((int *)arg));
}

static void show_ticks(void *arg __attribute__((unused)))
{
	puts("Tick! ");putu(SYSTEM_TICKS);puts("\n");
}

static void idle_task(void)
{
	for(;;)
		__wfi();
}

void entry(void)
{
	static int wid;
	
	/* Generate a software interrupt to
	 * test the functionality of exception
	 * vector (set in init.c) */
	//asm volatile("swi 0");
	
	welcome();
	
	/* cbs0 is the CBS server defined in this project (cbs.c)
	 * and initialized in _init() function in init.c */
	wid = add_cbs_worker(&cbs0, cbs_worker, &cbs0);
	if (wid == -1) {
		_panic(__FILE__, __LINE__, "Cannot create a worker for the CBS server.");
	}
	
	if (create_task(led_cycle,
			&wid,                   /* ID of the CBS worker as argument for this task */
			get_ticks_in_sec(2),    /* Every 2 seconds */
			5,                      /* Initial phase */
			get_ticks_in_sec(2),    /* Relative deadline: 2 seconds apart from release time */
			EDF,                    /* Earliest Deadline First */
			"led_cycle") == -1) {
		_panic(__FILE__, __LINE__, "Cannot create task led_cycle.");
	}
	
	if (create_task(show_ticks,
			NULL,
			get_ticks_in_sec(1),    /* Every second */
			5,                      /* Initial phase */
			get_ticks_in_sec(1),    /* Relative deadline: 1 second apart from release time */
			EDF,                    /* Earliest Deadline First */
			"show_ticks") == -1) {
		_panic(__FILE__, __LINE__, "Cannot create task show_ticks.");
	}
	
	/* This is the task 0, those that the scheduler runs when no other tasks are eligible.
	 * Let put the CPU in a low power state until next interrupt */
	idle_task();
}
