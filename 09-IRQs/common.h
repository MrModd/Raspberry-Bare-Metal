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

#ifndef RASPBERRY_H
#error You should not include sub-header files
#endif

/* Define NULL type */
#define NULL ((void *)0)

/* Define a data type that represent an high-level
 * interrupt handler function */
typedef void (*isr_t)(void);

/* Define the entry point function symbol that may be used by some functions
 * that include raspberry.h header file (such as init.c) */
extern void entry(void);

/* Declaration of other functions that may be used somewhere from the program */
extern void panic0(void);
extern void panic1(void);
extern void panic2(void);
extern void panic3(void);
extern void panic4(void);
extern void init_uart(void);
extern void init_miniuart(void);
extern int putc(int);
extern int puts(const char*);
extern int puth(unsigned long);
extern int putu(unsigned long);
extern int putd(long);
extern int putf(double, int);
extern void init_irq(void);
extern int register_isr_irq1(int, isr_t);
extern int register_isr_irq2(int, isr_t);
extern int register_isr_irq_basic(int, isr_t);
extern void init_ticks(void);
extern volatile unsigned long SYSTEM_TICKS;

/* inline tells to the compiler to optimize, when possible, this function
 * replacing the function call with the function itself */ 
static inline void loop_delay(unsigned long d)
{
	//volatile unsigned long c;
	/* Making the variable volatile we force the compiler to read
	 * its content from main memory each time, discarding eventual
	 * cache copies */
	while (d-- > 0)
		//c += d;
		/* This instruction is just to prevent to leave an empty while.
		 * Compiler optimization could remove the loop */
		
		/* We can optimize this instruction using a memory barrier, that is
		 * faster than a read in memory */
		__memory_barrier();
}



/* MORE PRECISE DELAY FUNCTIONS (based on system ticks) */

/* Define some macros for unsigned comparison that keep track
 * of register overflow */
#define time_after(a,b) ((long)((b)-(a))<0)
#define time_before(a,b) time_after(b,a)
#define time_after_eq(a,b) ((long)((a)-(b))>=0)
#define time_before_eq(a,b) time_after_eq(b,a)

static inline void delay_ticks(unsigned long d)
{
	unsigned long expire = d + SYSTEM_TICKS;
	/* time_before() avoid problems when SYSTEM_TICKS is near his
	 * register capacity. There's a period of time when expire
	 * variable is forward in time in respect of SYSTEM_TICKS,
	 * but his unsigned value is <.
	 * In order to consider always the correct difference of time,
	 * we must use a signed value comparison. */
	while (time_before(SYSTEM_TICKS, expire))
		; /* do nothing */
}

static inline void delay_ms(unsigned long ms)
{
	/* We don't simply call delay_ticks(ms * HZ / 1000) because
	 * we want this code to be as short as possible. A function call
	 * would cause saving and restoring registers on the stack */
	unsigned long expire = (ms * HZ / 1000) + SYSTEM_TICKS;
	while (time_before(SYSTEM_TICKS, expire))
		; /* do nothing */
}

#define delay_s(seconds) delay_ms((seconds) * 1000)
