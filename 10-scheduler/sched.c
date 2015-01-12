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

/* Total number of job served */
volatile unsigned long globalreleases = 0;

void check_periodic_tasks(void)
{
	unsigned long now = SYSTEM_TICKS;
	struct task *f;
	int i;
	
	for (i=0, f=taskset; i<active_tasks; ++f) {
		
		/* This job is not active */
		if (!f->valid)
			continue;
		
		if (time_after_eq(now, f->releasetime)) {
			++f->released; /* f->released += 1; */
			f->releasetime += f->period; /* Update next release time */
			++globalreleases; /* Update the number of all releases */
		}
		
		++i;
	}
}

static inline struct task *select_best_task(void)
{
	unsigned long maxprio;
	int i;
	struct task *f, *best;
	
	maxprio = MAXUINT; /* Init to the least priority */
	best = NULL;
	for (i=0, f=taskset; i<active_tasks; ++f) {
		
		/* If f exceded taskset[MAX_NUM_TASKS] means that active_tasks
		 * had a value greater than the real number of active tasks */
		if (f - taskset >= MAX_NUM_TASKS)
			panic0();
		
		if (!f->valid)
			continue;
		
		++i; /* This task is active */
		
		/* There are no job released at this time for this task */
		if (f->released == 0)
			continue;
		
		/* Check the priority */
		if (f->priority < maxprio) {
			maxprio = f->priority;
			best = f;
		}
	}
	return best;
}

void run_periodic_tasks(void)
{
	struct task *best;
	unsigned long state;
	
	puts("\n--------------Starting tasks--------------\n\n");
	
	for (;;) {
		state = globalreleases;
		best = select_best_task(); /* Get the highest priority job to execute */
		
		/* If globalreleases differs, a new task was released in the meantime.
		 * We cannot execute the selected job because it is possible that the
		 * new job has an higher priority */
		if (best != NULL && state == globalreleases) {
			best->job(best->arg); /* Execute the job */
			best->released--;
		}
		else if (best == NULL)
			/* If nothing to do put the CPU into
			 * low power state until next timer interrupt */
			__wfi();
	}
}
