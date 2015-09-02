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

/* Shamelessy "inspired" by "Building bare-metal ARM Systems with GNU",
   by Miro Samek of Quantum Leaps, LLC */

	.equ NO_IRQ, 0x80
	.equ NO_FIQ, 0x40
	.equ NO_INT, (NO_IRQ | NO_FIQ)
	.equ FIQ_MODE, 0x11
	.equ IRQ_MODE, 0x12
	.equ SYS_MODE, 0x1f

	.section .text
	.code 32
	.globl _irq_handler

/* Description of registers:
 * 
 * +--+--+--+--+--+--+--+--+--+--+---+---+---+---+---+---+----+----+
 * |r0|r1|r2|r3|r4|r5|r6|r7|r8|r9|r10|r11|r12|r13|r14|r15|cpsr|spsr|
 * |  |  |  |  |  |  |  |  |  |  |   |   |   |sp |lr |pc |    |    |
 * +--+--+--+--+--+--+--+--+--+--+---+---+---+---+---+---+----+----+
 *                                             ^   ^   ^        ^
 * 
 * Those with a ^ underneath are private for the execution mode, that
 * means that they are different when executing in IRQ mode or SYSTEM
 * mode. In example sp register can assume two different values accessible
 * alternately when in IRQ or in SYSTEM mode.
 * 
 * AAPCS-clobbered registers are those that "by convention" should not be
 * preserved during a C function invocation. If this function want to
 * overwrite these registers it can do that. It is the assembly code
 * (this IRQ handler in this case) that must save their values on the
 * stack before jumping to the C function.
 * On the other hand the non AAPCS-clobbered registers are those that
 * during the execution of a C function should not be overwritten.
 * The C code must save their values before use them and it must
 * restore their original value before returning to the assembly
 * procedure.
 * With ARM AAPCS the clobbered registers are {r0-r3,r12,r14/lr}; instead
 * those registers that the C function must preserve are {r4-r11}.
 * 
 * _irq_handler below saves those AAPCS-clobbered registers on the stack,
 * jumps to the high level ISR routine and then restore them. */

/* Low-level interrupt handler function */
_irq_handler:

	/* IRQ mode */                      /* When an IRQ occurs, the CPU jumps
	                                     * automatically in IRQ mode */
	mov r13, r0                         /* save r0 in r13_IRQ */
	sub r0, lr, #4                      /* put return address in r0_SYS */
	mov lr, r1                          /* save r1 in r14_IRQ (lr) */
	mrs r1, spsr                        /* put the spsr in r1_SYS */
	msr cpsr_c, #(SYS_MODE|NO_IRQ)      /* SYSTEM mode, IRQ disabled */
	
	/* SYSTEM mode */
	
	stmfd sp !, {r0, r1}                /* save spsr and pc on SYS stack */
	stmfd sp !, {r2-r3, r12, lr}        /* save AAPCS-clobbered regs on SYS stack */
	mov r0, sp                          /* make sp_SYS visible to IRQ mode */
	sub sp, sp, #(2*4)                  /* make room for the stacking (r0_SYS,r1_SYS) */
	msr cpsr_c, #(IRQ_MODE|NO_IRQ)      /* IRQ mode, IRQ disabled */
	
	/* IRQ mode */
	
	stmfd r0 !, {r13, r14}              /* finish saving the context (r0_SYS,r1_SYS) */
	msr cpsr_c, #(SYS_MODE|NO_IRQ)      /* SYSTEM mode, IRQ disabled */
	
	/* SYSTEM mode */
	
	ldr r12, = _bsp_irq                 /* address of middle level C IRQ-handler */
	mov lr, pc                          /* copy return address to link register */
	bx r12                              /* call the C IRQ-handler */
	msr cpsr_c, #(SYS_MODE|NO_INT)      /* SYSTEM mode, IRQ/FIQ disabled */
	mov r0, sp                          /* make sp_SYS visible to IRQ mode */
	add sp, sp, #(8*4)                  /* fake unstacking 8 registers from the stack */
	msr cpsr_c, #(IRQ_MODE|NO_INT)      /* IRQ mode, IRQ/FIQ disabled */
	
	/* IRQ mode */
	
	mov sp, r0                          /* copy sp_SYS to sp_IRQ */
	ldr r0,[sp, #(7*4)]                 /* load the saved spsr from the stack */
	msr spsr_cxsf, r0                   /* copy the saved spsr to spsr_IRQ */
	ldmfd sp, {r0-r3, r12, lr}^         /* unstack all saved SYSTEM registers
	                                     * "^" here means: use SYSTEM/USER registers.
	                                     * We are in IRQ mode, but the lr register will
	                                     * be saved in the SYSTEM mode equivalent
	                                     * register. */
	nop                                 /* can't access banked registers immediately */
	ldr lr,[sp, #(6*4)]                 /* load return address from the stack */
	movs pc, lr                         /* jump back to what was running before entering
	                                     * the interrupt handler, but before that restore
	                                     * SPSR register (of the IRQ mode) to the CPSR
	                                     * position. Since SPSR indicates
	                                     * a SYSTEM mode, because that's the execution
	                                     * mode of the CPU before the IRQ, next
	                                     * instruction will be executed in SYSTEM mode
	                                     * as well. */
