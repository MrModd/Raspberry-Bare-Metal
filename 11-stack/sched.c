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

volatile unsigned long globalreleases = 0; /* Total number of job served */
volatile unsigned long trigger_schedule = 0; /* Need to call schedule */

struct task *current; /* Current task on the CPU */

void check_periodic_tasks(void)
{
	unsigned long now = SYSTEM_TICKS;
	struct task *f;
	int i;
	
	for (i=0, f=taskset+1 /* Defined in tasks.c */; i<active_tasks; ++f) {
		
		/* If f exceded taskset[MAX_NUM_TASKS] means that active_tasks
		 * had a value greater than the real number of active tasks */
		if (f-taskset > MAX_NUM_TASKS)
			panic0();
		
		/* This job is not active */
		if (!f->valid)
			continue;
		
		if (time_after_eq(now, f->releasetime)) {
			++f->released; /* f->released += 1; */
			f->releasetime += f->period; /* Update next release time */
			trigger_schedule = 1; /* Reschedule in order to check if this is a higher priority job */
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
	best = &taskset[0]; /* If no periodic task can be scheduled run the idle task */
	for (i=0, f=taskset+1; i<active_tasks; ++f) {
		
		/* If f exceded taskset[MAX_NUM_TASKS] means that active_tasks
		 * had a value greater than the real number of active tasks */
		if (f - taskset > MAX_NUM_TASKS)
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

struct task * schedule(void)
{
	struct task *best;
	unsigned long state;
	static int do_not_enter = 0;
	
	irq_disable();
	if (do_not_enter != 0) {
		irq_enable();
		return NULL;
	}
	
	do_not_enter = 1;
	
	do {
		state = globalreleases;
		irq_enable();
		best = select_best_task(); /* Get the highest priority job to execute */
		irq_disable();
	} while (state != globalreleases);
	trigger_schedule = 0;
	best = (best != current ? best : NULL);
	
	do_not_enter = 0;
	irq_enable();
	
	/* Return NULL if the task to execute is that already on CPU */
	return best;
}

/* STMIA: Store Multiple Increment After.
 * Store in memory (at address regs) the block of registers r4-r11 one by one
 * and for each register stored, increment by 4 the address. */
#define save_regs(regs) \
		__asm__ __volatile__("stmia %0,{r4-r11}" \
		: : "r" (regs) : "memory")

/* LDMIA: Load Multiple Increment After.
 * Load from memory (at address regs) the block of registers r4-r11 one by one
 * and for each register loaded, increment by 4 the address. */
#define load_regs(regs) \
		__asm__ __volatile__("ldmia %0,{r4-r11}" \
		: : "r" (regs) : "r4","r5","r6","r7","r8","r9","r10","r11","memory")

/* Switch the stack pointer from the exiting task to the entering one */
#define switch_stacks(from, to) \
		__asm__ __volatile__("str sp, %0\n\t" \
		                     "ldr sp, %1\n\t" \
		: : "m" ((from)->sp), "m" ((to)->sp) : "sp", "memory")

/* Define an instruction for the return to the caller */
#define naked_return() __asm__ __volatile__("bx lr")

/* L'attributo "memory" dice che dopo questa istruzione il processore
 * deve considerare che il contenuto della memoria è cambiata. */

/* Do a context change from the current task in execution to the new one (to).
 * __attribute__((naked)) specifies the compiler to generate assembly only for what
 * it is written and nothing else (such as initialization instruction or return instructions) */
void _switch_to(struct task *to) __attribute__((naked));
void _switch_to(struct task *to)
{
	/* We know what task is on the CPU because is that pointed by current. */
	irq_disable();
	save_regs(current->regs);
	load_regs(to->regs);
	switch_stacks(current, to);
	current = to;
	irq_enable();
	
	/* __attribute__((naked) prevent the compiler to add instruction
	 * for the return jump. That's why we need naked_return() */
	naked_return();
}

/* It's important to check the assembly generated in order to determine if the
 * compiler used some registers _switch_to() is going to save. Registers r4 to
 * r11 should compare only within the stmia and ldmia instructions. */
