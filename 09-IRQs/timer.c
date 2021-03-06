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

volatile unsigned long SYSTEM_TICKS = 0;

/* High-level interrupt handler function for ARM timer */
static void isr_tick(void)
{
	/* Send an ACK to the interrupt handler (every value should be ok) */
	iomem(TIMER_CLEAR) = 0xfffffffful;
	SYSTEM_TICKS++;
}

void init_ticks(void)
{
	irq_disable();
	
	/* Register isr_tick() function as IRQ handler for ARM timer */
	if (register_isr_irq_basic(TIMER_IRQ_LINE, isr_tick)) {
		puts("Cannot register timer interrupt\n");
		panic0();
	}
	
	/* Timer clock must be 1MHz as expected by SP804 ARM timer module. */
	iomem(TIMER_PRE_DIVIDER) = PRE_DIVIDER_VAL;
	
	/* Now we want the timer to generate an interrupt at frequency of 1000Hz */
	iomem(TIMER_LOAD) = TIMER_LOAD_VALUE;
	
	/* Enable timer and related interrupt line */
	//iomem(TIMER_CONTROL) = TIMER_CTLR_IRQ_EN | TIMER_CTLR_EN;
	iomem(TIMER_CONTROL) = TIMER_CTLR_32BIT_COUNTER
						| TIMER_CTLR_IRQ_EN
						| TIMER_CTLR_EN;
	
	irq_enable();
}
