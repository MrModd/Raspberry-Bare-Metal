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

/* Definition of inline assembly:
 * 
 * asm ("instruction1" "instruction2" "instruction3");
 * or
 * __asm__ ("instruction1" "instruction2" "instruction3");
 * 
 * __asm__ is preferable because is ISO C
 * 
 * 
 * 
 * Definition of extended assembly:
 * it uses operands too:
 * 
 * __asm__ ("instruction1"
 * 				: output operands
 * 				: input operands
 * 				: list of cobbler registers
 * 		);
 * 
 * 
 * 
 * For further details see http://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
 * 
 */

/* Data Synchronization Barrier (ARM manual p. 3-83) */
/* Wait for the pipeline to be empty before execute next instruction */
#define __synchronization_barrier() __asm__ __volatile__ ("mcr p15, 0, %[dummy], c7, c10, 4" : : [dummy] "r" (0) : "memory")

/* Data Memory Barrier (ARM manual p. 3-84) */
/* Finish all reads ad writes in memory before execute next instruction */
#define __memory_barrier() __asm__ __volatile__ ("mcr p15, 0, %[dummy], c7, c10, 5" : : [dummy] "r" (0) : "memory")

/* Explanation of MCR instruction:
 * it is used to send commands to a coprocessor (the MMU in this case, that correspond
 * to cp15 coprocessor) together to some data.
 * 
 * Vice versa, MRC instruction is used to send commands to a coprocessor receiving data.
 * 
 * 
 * MCR coproc, #opcode1, Rt, CRn, CRm{, #opcode2}
 * 
 * where:
 * 			coproc		is the coprocessor at which we want to communicate, p15 is the MMU
 * 			#opcode1	is an opcode specific for the current coprocessor (they are 3 bits)
 * 			Rt			is the source register from where the processor takes input data
 * 			CRn			is the coprocessor register
 * 			CRm			is another coprocessor register
 * 			#opcode2	is another opcode specific for the current coprocessor (it is optional and are 3 bits)
 * 
 * As input register we use [dummy] because we don't actually need that. Using dummy we are telling to the
 * compiler to use a register currently unused, avoiding to accidentally picking a register the compiler
 * used for other reasons.
 * 
 * In the ARM11 specific, if opcode1 is set to 0, then we are operating to the level 0 of the cache.
 * c7 is used for barrier operations and cache operations. Arguments c10 and 5 specify that we want to do a
 * Data Memory Barrier.
 * See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0301h/Babhejba.html or look for
 * "c7, Cache Operations Register" for ARM 11.
 */



/* This instruction would work as memory barrirer as well, but just at compiler level.
 * In order to make the memory barrier actually work at processor register level, we need
 * to use the mcr instruction as seen above.
 */
/*
#define __memory_barrier() __asm__ __volatile__ ("":::"memory")
*/
