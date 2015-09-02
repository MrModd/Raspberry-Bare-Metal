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
	
	/* Begin of preemptibility routine */
	
	and r0, r0, #SYS_MODE               /* r0 contains the spsr register restored from
	                                     * the stack. Get only the bits related to the
	                                     * mode of execution... */
	teq r0, #SYS_MODE                   /* ... and check if it was in SYSTEM mode */
	bne .Lnosched                       /* If not, complete the interrupt handler routine.
	                                     * Before entering this interrupt handler the CPU
	                                     * already was in IRQ mode (probably another IRQ
	                                     * handler). Let the nested IRQs end before executing
	                                     * the scheduler. */
	
	ldr r0, =trigger_schedule           /* Load the address of the variable */
	ldr r0, [r0]                        /* Load the content of the variable */
	tst r0, r0                          /* Check if it is zero */
	beq .Lnosched                       /* Yes, there's no need to call the scheduler, exit */
	
	ldr lr, =_irq_schedule              /* Load in lr the address of the _irq_schedule routine */
	movs pc, lr                         /* Jump to _irq_schedule in SYSTEM mode:
	                                     * movs restore the SPSR register of the current (IRQ)
	                                     * mode to the CPSR register. Since SPSR indicates
	                                     * a SYSTEM mode as stated at the beginning of the
	                                     * preemptibility routine, next instruction will be
	                                     * executed in SYSTEM mode as well. */
	
	/* End of preemptibility routine */
	
.Lnosched:
	
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

/* End of _irq_handler */



_irq_schedule:
	
	sub sp, sp, #32                     /* What need to be saved is already on the stack */
.Lnosub32:
	ldr r12, =schedule                  /* Put the address of schedule() function in r12 */
	mov lr, pc                          /* Save the pc for when the function returns */
	bx r12                              /* Jump to schedule(). Return value will be put on r0 */
	tst r0, r0                          /* Check if schedule() has returned NULL */
	beq .Lnoswitch                      /* If so, don't do the context switch */
	
	ldr r12, =_switch_to                /* Put in r12 the address of _switch_to() function */
	mov lr, pc                          /* Save the pc for when the function returns */
	bx r12                              /* Jump to _switch_to(). The function wants the new task
	                                     * to schedule as argument. There already is such task in
	                                     * r0, returned by previous call of schedule() */
	
	/* This task will not return from the jump until the scheduler will put it again on the
	 * CPU. After the jump function the register content changed, including the stack pointer
	 * that now points to the stack of the new task in execution. */
	
.Lnoswitch:
	
	/* Now it's time to restore those registers at the top of the stack that are saved every time
	 * the task is removed from the execution. These are also the registers initialized during the
	 * task creation. See init_task_context() in task.c. */
	
	/* First of all restore the status register spsr. This is (almost) at the bottom of the stack,
	 * so doing some math the stack pointer is increased and the value is copied in r0.
	 * Then it is restored in cpsr. */
	 
	ldr r0,[sp,#(7*4)]                  /* Copy the content of the stack at position sp+28 in r0,
	                                     * but do not update sp for now */
	msr cpsr_f,r0                       /* Write r0 (that contains spsr) in cpsr */
	
	/* Then restore other registers except pc. */
	
	ldmfd sp!,{r0-r3,r12,lr}            /* Restore those registers and then increase sp */
	
	/* Last two registers: sp and spsr. The second one was already restored, so restore
	 * pc, increase the value of sp and then continue the execution from last instruction executed
	 * by this task. */
	
	ldr pc,[sp],#(4*2)                  /* Load in pc the content of sp, then update sp with sp+8 */

/* End of _irq_schedule */



	.globl _sys_schedule

_sys_schedule:

	str lr,[sp, #-(4*2)]!               /* push the return address skipping cpsr's slot */
	mrs lr,cpsr                         /* read special register cpsr */
	str lr,[sp, #4]                     /* store the value of cpsr in its slot */
	ldr lr,[sp]                         /* load in r14/lr the return address */
	stmfd sp !, {r0-r3, r12, lr}        /* save the AACS-clobbered registers */
	b .Lnosub32                         /* jump into the _irq_schedule() function, 
	                                     * bypassing the adjustment of the stack top */

/* End of _sys_schedule */

/* Whats the difference between _irq_schedule and _sys_schedule?
 * Both handle the change of task in execution, but _irq_schedule do this because
 * of preemptibility and _sys_schedule do this because the task ended its execution.
 * In this last case, if the scheduler won't find another task to run, will execute the
 * idle task (taskset[0]).
 * So the difference is that _sys_schedule is called from a task and all the context
 * saving done by the interrupt handler _irq_handler is skipped. _irq_schedule expect
 * to find some registers in the stack and that's exaclty what _sys_schedule is for: it
 * "simulate" the execution of the IRQ handler saving these registers before calling
 * _irq_schedule. */
