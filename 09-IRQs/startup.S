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
 
	.text					/* Write in the code segment */
	.code 32				/* Write for 32bit architectures */
	
	/* CPSR register description can be found at page 2-24 of the ARM manual,
	 * figure 2-10, under chapter "The program status registers */
	
	.equ	MODE_BITS,	0x1F	/* 0b11111 */
	.equ	FIQ_BIT,	0x40	/* FIQ disable bit */
	.equ	IRQ_BIT,	0x80	/* IRQ disable bit */
	
	/* Bit configurations can be found in table 2-7 of page 2-28 */
	
	.equ	USER_MODE,	0x10	/* User mode: 0b10000 */
	.equ	FIQ_MODE,	0x11	/* FIQ mode: 0b10001 */
	.equ	IRQ_MODE,	0x12	/* IRQ mode: 0b10010 */
	.equ	SUPER_MODE,	0x13	/* Supervisor mode: 0b10011 */
	.equ	ABRT_MODE,	0x17	/* Abort mode: 0b10111 */
	.equ	UNDEF_MODE,	0x1B	/* Undefined mode: 0b11011 */
	.equ	SYS_MODE,	0x1F	/* System mode: 0b11111 */
	.equ	MON_MODE,	0x16	/* Monitor mode: 0b10110 */
	
	.global _reset			/* Export global symbol _reset. Make it global */

_reset:
	/* Initialize processor state */
	mrs		r0, cpsr			/* Move special register CPSR to r0 */
	bic		r0, r0, #MODE_BITS	/* Bit clear: clear all bits set in MODE_BITS doing a logic AND between r0 and the opposite of MODE_BITS */
	orr		r0, r0, #(SYS_MODE|FIQ_BIT|IRQ_BIT)	/* OR between r0 and the immediate value */
	msr		cpsr, r0			/* Move again special register CPSR */
	
	/* Init the stack pointer */
	ldr sp,=stack_top		/* Write in sp the address of stack_top, defined in .lds file.
							 * stack_top is not a variable, and does not contain the address we are looking
							 * for. stack_top address is the address we are looking for. */
	b _init					/* Jump to the initialization function */
