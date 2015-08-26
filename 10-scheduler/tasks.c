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

/* List of all installed tasks (can be active or not) */
struct task taskset[MAX_NUM_TASKS];
int active_tasks; /* How many active tasks are there */

void init_taskset(void)
{
	int i;
	
	active_tasks = 0;
	for (i=0; i<MAX_NUM_TASKS; i++)
		taskset[i].valid = 0;
}

/* Add a new task to the taskset
 * @job: the job to be released by this task
 * @arg: data of the function call
 * @period: time between two releases
 * @delay: initial delay before the firse release (task phase)
 * @priority: value in [0, MAXUINT] (0 := max priority)
 * @name: name description for this task
 */
int create_task(job_t job, void *arg, unsigned long period,
		unsigned long delay, unsigned long priority,
		const char *name)
{
	int i;
	struct task *t;
	
	/* Find a free slot or return -1 */
	for (i=0; i<MAX_NUM_TASKS; ++i)
		if (!taskset[i].valid)
			break;
	if (i == MAX_NUM_TASKS)
		return -1;
	
	/* Get the pointer to the structure */
	t = taskset + i;
	
	/* Fill the task structure */
	t->job = job;
	t->arg = arg;
	t->name = name;
	t->period = period;
	t->priority = priority;
	t->releasetime = SYSTEM_TICKS + delay;
	t->released = 0;
	++active_tasks;
	
	/* Scheduler is asynchronous in respect of the creation of this task.
	 * The validity bit must be the last field to be set in the structure.
	 * In order to prevent code reordering we must use a memory barrier. */
	__memory_barrier();
	
	t->valid = 1;

	puts("Task \"");
	puts(name);
	puts("\" with id ");
	putd(i);
	puts(" created.\n");
	
	return 0;
}
