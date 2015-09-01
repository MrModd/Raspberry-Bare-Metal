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

struct cbs_queue cbs0; /* This is the CBS server present in the system.
                        * It is bonded to a periodic task of type CBS. */

/* Function that runs an aperiodic job
 * @arg: the pointer to the CBS server structure */
static void cbs_server(void *arg)
{
	struct cbs_queue *q = (struct cbs_queue *)arg;
	int i;
	
	for (i=0; i<q->num_workers; ++i) {
		if (q->pending[i] > 0)
			break;
	}
		
	if (i == q->num_workers) {
		/* No aperiodic job can be executed */
		puts("WARNING: useless activation of CBS server.\n");
		return;
	}
	
	/* Run the job */
	q->workers[i](q->args[i]);
	/* Decrement number of releases synchronously */
	irq_disable();
	--q->pending[i];
	irq_enable();
	
	/* Running a job per execution because this simplifies how to update 
	 * the value of released jobs of the task */
}

/* Release a new aperiodic job.
 * @q: the queue from where to take the job
 * @wid: the type of job, must be in [0, MAX_NUM_WORKERS] interval
 */
void activate_cbs_worker(struct cbs_queue *q, int wid)
{
	struct task *t = q->task;
	
	if (wid >= q->num_workers)
		_panic(__FILE__, __LINE__, "Invalid worker ID.");
	
	irq_disable();
	q->pending[wid]++;
	t->released++;
	if (t->released == 1) { /* CBS server was empty */
		/* If:
		 *     - A new aperiodic job is released
		 *     - CBS server was empty
		 *     - Cs >= (d - t) * u
		 *       where Cs is current budget, d is absolute deadline
		 *       u is the utilization of the server and t is the
		 *       current instant
		 * Then: d <- t + p
		 *       where p is the period of the server
		 * 
		 * Remember that:
		 *   - t->abs_deadline is the absolute deadline of the EDF task (hosting the CBS server)
		 *   - t->max_budget is the max budget for the EDF server
		 *   - t->period is the period of the task and reload period of the CBS server
		 *   - t->budget is the current budget
		 */
		u32 pd = t->abs_deadline * t->max_budget;
		u32 tdbp = SYSTEM_TICKS * t->max_budget + t->budget * t->period;
		if (tdbp >= pd) {
			t->abs_deadline = SYSTEM_TICKS + t->period;
			t->budget = t->max_budget;
			trigger_schedule = 1; /* Need to reschedule because priority changed */
			++globalreleases;
		}
	}
	irq_enable();
}

/* This function must be invoked at each tick as
 * it decreases the current budget of the CBS server
 * when an aperiodic job is running.
 * @t: the task associated with a CBS server
 */
void decrease_cbs_budget(struct task *t)
{
	t->budget--;
	if (t->budget > 0)
		return;
	/* Budget reached 0, reload and postpone the deadline */
	t->budget = t->max_budget;
	t->abs_deadline += t->period;
	
	trigger_schedule = 1; /* Need to reschedule because priority changed */
}

/* Initialize the CBS server.
 * @max_cap: max execution time for the server (a.k.a. maximum budget)
 * @period: period of the server
 * @cbs_q: the structure of the CBS server
 * @name: a canonical name for the server
 */
int init_cbs(unsigned long max_cap, unsigned long period, struct cbs_queue *cbs_q,
		const char *name)
{
	int tid;
	irq_disable();
	
	/* Initialize cbs_q struct */
	cbs_q->num_workers = 0;
	tid = create_task(cbs_server, cbs_q, period, 1, max_cap, CBS, name);
	if (tid == -1)
		return -1;
	cbs_q->task = taskset + tid; /* Link the task structure allocated by create_task() */
	
	irq_enable();
	return 0;
}

/* Add a worker (a type of jobs) in a CBS server.
 * @cbs_q: the CBS server structure
 * @worker_fn: the function to be executed when the job is released
 * @worker_arg: argument for the function call
 */
int add_cbs_worker(struct cbs_queue *cbs_q, job_t worker_fn, void *worker_arg)
{
	int i;
	irq_disable();
	
	i = cbs_q->num_workers;
	if (i >= MAX_NUM_WORKERS) {
		/* Server is already full */
		irq_enable();
		return -1;
	}
	/* Init data struct */
	cbs_q->workers[i] = worker_fn;
	cbs_q->args[i] = worker_arg;
	cbs_q->pending[i] = 0;
	cbs_q->num_workers++;
	
	irq_enable();
	return 0;
}
