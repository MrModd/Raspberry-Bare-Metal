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

#define STACK_SIZE 4096 /* Each stack is a page long */
char stacks[MAX_NUM_TASKS * STACK_SIZE] /* Each task has its own stack */
		__attribute__((aligned(STACK_SIZE))) /* Align this array in memory */
		__attribute__((section(".bss.stack"))); /* Put this variable in a separate part of bss section */
/* Memory alignment is not strictly necessary, but it is useful.
 * .bss.stack will be in .bss section, but it must not be zeroed like other variables because
 * it will be loaded with the stack of the first program to be executed (process 0).
 * See linker script sert.lds for a better understanding of the positioning. */
const char *stack0_top = stacks + MAX_NUM_TASKS * STACK_SIZE;
/* stack0_top is the beginning of the stack (growing for decreasing addresses).
 * In particular it is the beginning of the stack of the task 0.
 * Initialization of stack pointer register is done in startup.S. */

void init_taskset(void)
{
	int i;
	
	active_tasks = 0;
	for (i=0; i<MAX_NUM_TASKS; i++)
		taskset[i].valid = 0;
	
	current = &taskset[0];
	/* Task 0 is considered the idle task or the kernel task and it is
	 * executed every time no other task can run. */
}

void task_entry_point(struct task *t) __attribute__((naked));
/* Handler for a periodic task
 * @t: the task to run
 */
void task_entry_point(struct task *t)
{
	for (;;) {
		if (t->valid == 0 || t->released == 0)
			/* If scheduler selected this task for execution, but
			 * ther're no job released, then select_best_task() did
			 * something wrong. */
			panic0();
		
		irq_enable();
		t->job(t->arg); /* Run the job for this task */
		irq_disable();
		--t->released;
		
		/* This job ended its execution, so no other work can be done
		 * by this task until its next release. Calling _sys_schedule() will
		 * invoke the scheduler, that will see that this task cannot run
		 * anymore and will change the task on the CPU. */
		_sys_schedule();
		/* When this function call returns, another job has been released
		 * and this task has been put again on the CPU for the execution. */
	}
}

/* Initialize the stack for the specific task
 * @t: the pointer to the task for which the stack is going to be initialized
 * @ntask: the number of the task (between 0 and MAX_NUM_TASK - 1)
 */
void init_task_context(struct task *t, int ntask)
{
	unsigned long *sp;
	int i;
	
	sp = (unsigned long *)(stack0_top - ntask * STACK_SIZE); /* Get the top of the stack for this task */
	
	/* Initialize the stack
	 * See save_regs() and load_regs() macros in sched.c.
	 * This is the order of registers in the stack:
	 * 
	 *         +---------------+ <-- mem_end (defined in sert.lds)
	 *         |               |
	 *         |               | Addresses:
	 *         |               |           <-- i+1
	 *         |     FREE      |           <-- i
	 *         |               |           <-- i-1
	 *         |               |
	 *         +---------------+  <-- Stack top for task 0 (taskset[0]), also stack0_top points here
	 *    ^    |      spsr     |        
	 *    |    |      ret      |
	 *    s    |      lr       |
	 *    t    |      r12      |
	 *    a    |      r3       |
	 *    c    |      r2       |
	 *    k    |      r1       |
	 *    s    |      r0       |
	 *         |      ...      |
	 *    v    +---------------+ <-- Stack top for task 1 (taskset[1])
	 *    a    |      spsr     |        
	 *    r    |      ret      |
	 *    i    |      lr       |
	 *    a    |      ...      |
	 *    b    +---------------+ <-- Stack top for task MAX_NUM_TASKS - 1 (taskset[MAX_NUM_TASK-1])
	 *    l    |               |
	 *    e    |      ...      |
	 *    |    |               |
	 *   -+-   +---------------+ <-- End of stack for task MAX_NUM_TASK - 1, also base address of stacks variable
	 *         |               |
	 *         |      ...      |
	 *         |               |
	 *         +---------------+ <-- mem_start (defined in sert.lds)
	 * 
	 */
	*(--sp) = 0ul;                              /* spsr */
	*(--sp) = (unsigned long) task_entry_point; /* Function to call when this task starts */
	*(--sp) = 0ul;                              /* r14/lr */
	*(--sp) = 0ul;                              /* r12 */
	*(--sp) = 0ul;                              /* r3 */
	*(--sp) = 0ul;                              /* r2 */
	*(--sp) = 0ul;                              /* r1 */
	*(--sp) = (unsigned long) t;                /* r0: task_entry_point() wants the pointer to the task structure as argument.
	                                             * We put it here in r0 register. */
	t->sp = (unsigned long) sp;                 /* Now sp points 8*4 positions below the beginning of the stack for this task */
	
	/* At this point the stack contains those registers that during a context switch the IRQ handler expect to find in
	 * order to schedule this task. What here is a pointer to function task_entry_point(),
	 * will become the pointer of the next instruction that the task would have executed if it wasn't interrupted (ndr. the pc).
	 * See irqhandler.S for a better understanding on how these registers are reloaded after a context switch. */
	
	/* The order of the registers is dictated by the interrupt handler routine (_irq_handler) and it's way to save
	 * registers on the stack when an IRC occurs. */
	
	for (i = 0; i<8; ++i)
		t->regs[i] = 0ul; /* Clear registers {r4-r11} */
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
	for (i=1; i<MAX_NUM_TASKS; ++i) /* Task 0 is the idle task */
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
	init_task_context(t, i);
	
	/* Scheduler is asynchronous in respect of the creation of this task.
	 * The validity bit must be the last field to be set in the structure.
	 * In order to prevent code reordering we must use a memory barrier. */
	__memory_barrier();
	
	t->valid = 1;

	irq_disable();
	puts("Task \"");
	puts(name);
	puts("\" with id ");
	putd(i);
	puts(" created.\n");
	irq_enable();
	
	return 0;
}
