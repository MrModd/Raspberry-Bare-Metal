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

/* Define the entry point function symbol that may be used by some functions
 * that include raspberry.h header file (such as init.c) */
extern void entry(void);

/* Declaration of other functions that may be used somewhere from the program */
extern void panic0(void);
extern void panic1(void);
extern void panic2(void);
extern void panic3(void);
extern void panic4(void);

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
