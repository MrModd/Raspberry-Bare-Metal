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
 
	.text					/* Write in the code segment */
	.code 32				/* Write for 32bit architectures */
	.global _reset			/* Export global symbol _reset. Make it global */

_reset:
	/* Init the stack pointer */
	ldr sp,=stack_top		/* Write in sp the address of stack_top, defined in .lds file.
							 * stack_top is not a variable, and does not contain the address we are looking
							 * for. stack_top address is the address we are looking for. */
	b _init					/* Jump to the initialization function */
