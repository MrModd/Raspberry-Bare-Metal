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

#define VECTOR_BASE 0x00000000

static void init_vectors(void)
{
	extern void _reset(void);
	extern void _irq_handler(void);
	
	volatile u32 *vectors = VECTOR_BASE;
	
#define LDR_PC_PC 0xe59ff018 /* Hex encoding of the instruction:
                              * 	ldr pc,[pc,#0x18]
                              * 
                              * Each instruction is 32bit, 4 byte long.
                              * 0x18 = 24 byte = 6 instructions.
                              * 
                              * This instruction is a jump at PC + 6,
                              * considering that PC points 2 instructions after
                              * the currently running one. */

	vectors[0] = LDR_PC_PC;		/* Reset */
	vectors[1] = LDR_PC_PC;		/* Undefined Instruction */
	vectors[2] = LDR_PC_PC;		/* Software Interrupt */
	vectors[3] = LDR_PC_PC;		/* Prefetch Abort */
	vectors[4] = LDR_PC_PC;		/* Data Abort */
	vectors[5] = LDR_PC_PC;		/* Reserved */
	vectors[6] = LDR_PC_PC;		/* IRQ */
	vectors[7] = LDR_PC_PC;		/* FIQ */
	
	vectors[8] =  (u32) _reset;	/* Reset */
	vectors[9] =  (u32) panic0;	/* Undefined Instruction */
	vectors[10] = (u32) panic1;	/* Software Interrupt */
	vectors[11] = (u32) panic2;	/* Prefetch Abort */
	vectors[12] = (u32) panic3;	/* Data Abort */
	vectors[13] = (u32) panic4;	/* Reserved */
	vectors[14] = (u32) _irq_handler;	/* IRQ */
	vectors[15] = (u32) panic4;	/* FIQ */
	
	/* Set base address of the exception vector (ARM manual p. 3-121) */
	__asm__ __volatile__ ("mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" (vectors));
	/* For a description of mcr instruction and extended assembly see
	 * raspberry_cpu.h header file.
	 * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/Babfbcae.html */

#undef LDR_PC_PC
}

/* Enable floating point units */
static void init_vfp(void)
{
	u32 acr = read_coprocessor_access_control_register();
	
	u32 vfp_single = COPROCESSOR_ACCESS_USER << VFP_SINGLE_OFFSET;
	u32 vfp_double = COPROCESSOR_ACCESS_USER << VFP_DOUBLE_OFFSET;
	
	acr |= vfp_single | vfp_double; /* Enable single / double precision coprocessor units */
	
	write_coprocessor_access_control_register(acr);
	__memory_barrier();
	
	enable_vfp();
}

/* Init to 0 section .bss, where static variables that
 * must be initialized to 0 are located */
static void init_bss(void)
{
	/* _bss_start and _bss_end symbols are defined during
	 * linking process in file sert.lds */
	extern u32 _bss_start, _bss_end;
	u32 *p;
	
	for (p = &_bss_start; p < &_bss_end; p++)
		*p = 0UL;
	
	/* Why we use &_bss_start and &_bss_end?
	 * We must set to 0 all memory cells between _bss_start and
	 * _bss_end, so we must iterate on the address of these
	 * variables:
	 *                  ___________
	 * _bss_end        |           |
	 *                 |           |
	 *                 |           |
	 *                 |           |
	 * _bss_start      |           |
	 *                 |___________|
	 * 
	 * these symbols doesn't point to the memory area to erase,
	 * they ARE the memory addresses to erase. So we put p equals
	 * to the address of _bss_start and we iterate until _bss_end */
}

static void init_gpio(void)
{
	/* Init GPIO 16 (wired to LED ACT) as Output */
	
	//iomem(GPFSEL1) |= (1<<18);
	
	/* Or */
	//gpio[GPFSEL1/4] &= ~((1<<20)|(1<<19)|(1<<18));
	//gpio[GPFSEL1/4] |= ((0<<20)|(0<<19)|(1<<18));
	
	/* Or */
	
	GPIO1_SET_OUTPUT(GPIO_LED_SEL_OFFSET)
}

/* Init memory peripherals and then jump to entry() */
void _init(void)
{
	init_bss();
	init_vectors();
	init_vfp();
	init_gpio();
	
#ifdef MINI_UART
	init_miniuart(); /* Defined in uart.c */
#else
	init_uart(); /* Defined in uart.c */
#endif
	
	init_irq(); /* Defined in irq.c */
	init_ticks(); /* Defined in timer.c */
	
	init_taskset(); /* Defined in tasks.c */
	
	/* Create a task for the CBS server.
	 * Maximum budget 25 unit time (ticks) per period
	 * Period of 250 ticks. */
	if (init_cbs(25, 250, &cbs0, "cbs0") == -1) /* Create a task for the CBS server */
		_panic(__FILE__, __LINE__, "Cannot create CBS server task.");
	
	/* Prevent code reordering (just in case) */
	__memory_barrier();
	
	/* Jump to the entry point function that run user defined code */
	entry();
}
