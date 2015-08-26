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

#ifndef RASPBERRY_H
#error You should not include sub-header files
#endif

/* =================================================== */
/* CHANGE THIS IF YOU WANT DIFFERENT TIMER FREQUENCIES */
#define HZ 1000 /* Frequency of the timer interrupt    */
/*                 Must not be greater than TIMER_FREQ */
/* =================================================== */

#define TIMER_BASE 0x2000B000

/* Timer registers as offset of IRQ_BASE (Broadcom manual p. 196) */
iomemdef(TIMER_LOAD, TIMER_BASE + 0x400); /* Load */
iomemdef(TIMER_VALUE, TIMER_BASE + 0x404); /* Value (Read Only) */
iomemdef(TIMER_CONTROL, TIMER_BASE + 0x408); /* Control */
iomemdef(TIMER_CLEAR, TIMER_BASE + 0x40C); /* IRQ Clear/Ack (Write only) */
iomemdef(TIMER_RAW_IRQ, TIMER_BASE + 0x410); /* RAW IRQ (Read Only) */
iomemdef(TIMER_MASKED_IRQ, TIMER_BASE + 0x414); /* Masked IRQ (Read Only) */
iomemdef(TIMER_RELOAD, TIMER_BASE + 0x418); /* Reload */
iomemdef(TIMER_PRE_DIVIDER, TIMER_BASE + 0x41C); /* Pre-divider (Not in real AP804!) */
iomemdef(TIMER_COUNTER, TIMER_BASE + 0x420); /* Free running counter (Not in real AP804!) */

#define TIMER_IRQ_LINE 0 /* This timer is wired to the IRQ BASIC line 0 */

/* This board uses a timer based on an ARM AP804 timer module (see Broadcom SoC page 196).
 * This module expects a clock source of 1MHz which it isn't present on the SoC. We use
 * a pre-divider in order to take the main peripheral clock source, the APB (Advanced
 * Peripheral BUS), rated at 250MHz by default and slow down at 1MHz.
 * APB_CLOCK is the frequency of the core clock and TIMER_FREQ is the desired 1MHz frequency.
 * TIMER_PRE_DIVIDER is the value to be set in the homonym register following this formula:
 * 
 *     timer_clock = apb_clock/(pre_divider+1)
 * 
 * as said in the manual (Broadcom SoC page 199), which means:
 * 
 *     pre_divider = (apb_clock / timer_clock) - 1
 * 
 */
#define APB_CLOCK 250000000 /* Hz */
#define TIMER_FREQ 1000000 /* Hz */
#define TIMER_PRE_DIVIDER ((unsigned long)(APB_CLOCK / TIMER_FREQ) - 1) /* To be set in TIMER_PRE_DIVIDER register */

/* We want an interrupt assertion every 1/Hz seconds.
 * There's a register that get decremented every 1/TIMER_FREQ seconds
 * and when it reach 0 it asserts the timer IRQ line and initial
 * value is reloaded into this register.
 * In order to trigger the IRQ assertion every 1/Hz seconds we must
 * reload the value TIMER_FREQ/HZ into the register. This value is
 * set in the TIMER_LOAD register */
#define TIMER_LOAD_VALUE ((unsigned long)(TIMER_FREQ / HZ)) /* Value to reload in the timer register */

/* Control register (Broadcom SoC manual page 197) */
#define TIMER_CTLR_32BIT_COUNTER (1u<<1) /* Broadcom manual says this bit is for 23bit counter... That's not true! */
#define TIMER_CTLR_PRESCALE_16 (1u<<2)
#define TIMER_CTLR_PRESCALE_256 (2u<<2)
#define TIMER_CTLR_IRQ_EN (1u<<5)
#define TIMER_CTLR_EN (1u<<7)
