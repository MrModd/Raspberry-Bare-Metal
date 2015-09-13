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

/* These arrays are used to contain pointers to the functions that should be
 * called when a certain interrupt line become asserted. These functions
 * represent the high-level interrupt handler functions. */
static isr_t ISR_IRQ1[IRQ_1_LINES];
static isr_t ISR_IRQ2[IRQ_2_LINES];
static isr_t ISR_BASIC_IRQ[IRQ_BASIC_LINES];

/* This is a mid-level interrupt handler function */
void _bsp_irq(void)
{
	isr_t handler;
	int v, i;
	
	/* This Broadcom SoC does not support vectored interrupt,
	 * so we must do all the work by hand */
	
	/* While there's at least one IRQ line asserted (pending interrupt) */
	while(iomem(IRQ_BASIC_PENDING) != 0 ||
	      iomem(IRQ_PENDING1) != 0 ||
	      iomem(IRQ_PENDING2) != 0) {
		
		/* Check basic IRQ register */
		v = iomem(IRQ_BASIC_PENDING);
		i = 0;
		/* Shift until the asserted bit goes to the least
		 * significant bit of the register */
		while (v != 0) {
			if (v & 1u) {
				/* Call the high-level interrupt handler function related
				 * to this interrupt (if set) */
				handler = ISR_BASIC_IRQ[i];
				if (!handler)
					panic0();
				handler();
				__synchronization_barrier();
			}
			v = v >> 1;
			i++;
		}
		
		/* Check GPU IRQ register 1 */
		v = iomem(IRQ_PENDING1);
		i = 0;
		/* Shift until the asserted bit goes to the least
		 * significant bit of the register */
		while (v != 0) {
			if (v & 1u) {
				/* Call the high-level interrupt handler function related
				 * to this interrupt (if set) */
				handler = ISR_IRQ1[i];
				if (!handler)
					panic0();
				handler();
				__synchronization_barrier();
			}
			v = v >> 1;
			i++;
		}
		
		/* Check GPU IRQ register 2 */
		v = iomem(IRQ_PENDING2);
		i = 0;
		/* Shift until the asserted bit goes to the least
		 * significant bit of the register */
		while (v != 0) {
			if (v & 1u) {
				/* Call the high-level interrupt handler function related
				 * to this interrupt (if set) */
				handler = ISR_IRQ2[i];
				if (!handler)
					panic0();
				handler();
				__synchronization_barrier();
			}
			v = v >> 1;
			i++;
		}
		
	}
}

/* Set a function as interrupt handler for the GPU IRQ 1 line n */
int register_isr_irq1(int n, isr_t func)
{
	if (n > IRQ_1_LINES || ISR_IRQ1[n] != NULL) {
		return 1;
	}
	
	ISR_IRQ1[n] = func;
	
	/* Enable line interrupt in GPU IRQ 1 register */
	iomem_high(IRQ_ENABLE1, 1u << n);
	
	return 0;
}

/* Set a function as interrupt handler for the GPU IRQ 2 line n */
int register_isr_irq2(int n, isr_t func)
{
	if (n > IRQ_2_LINES || ISR_IRQ2[n] != NULL) {
		return 1;
	}
	
	ISR_IRQ2[n] = func;
	
	/* Enable line interrupt in GPU IRQ 2 register */
	iomem_high(IRQ_ENABLE2, 1u << n);
	
	return 0;
}

/* Set a function as interrupt handler for the BASIC IRQ line n */
int register_isr_irq_basic(int n, isr_t func)
{
	if (n > IRQ_BASIC_LINES || ISR_BASIC_IRQ[n] != NULL) {
		return 1;
	}
	
	ISR_BASIC_IRQ[n] = func;
	
	/* Enable line interrupt in IRQ BASIC register */
	iomem_high(IRQ_BASIC_ENABLE, 1u << n);
	
	return 0;
}
