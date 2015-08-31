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

/*   (We will not use FIQs)  */

#define IRQ_BASE 0x2000B000

/* Number of IRQ lines for every register */
#define IRQ_1_LINES 32 /* GPU interrupt lines register 1*/
#define IRQ_2_LINES 32 /* GPU interrupt lines register 2 */
#define IRQ_BASIC_LINES 20 /* ARM interrupt lines (other bits are for some GPU interrupt lines
                            * already present in the other two registers) */

/* IRQ registers as offset of IRQ_BASE (Broadcom manual p. 112) */
iomemdef(IRQ_BASIC_PENDING, IRQ_BASE + 0x200);
iomemdef(IRQ_PENDING1, IRQ_BASE + 0x204); 
iomemdef(IRQ_PENDING2, IRQ_BASE + 0x208);
iomemdef(IRQ_ENABLE1, IRQ_BASE + 0x210);
iomemdef(IRQ_ENABLE2, IRQ_BASE + 0x214);
iomemdef(IRQ_BASIC_ENABLE, IRQ_BASE + 0x218);
iomemdef(IRQ_DISABLE1, IRQ_BASE + 0x21C);
iomemdef(IRQ_DISABLE2, IRQ_BASE + 0x220);
iomemdef(IRQ_BASIC_DISABLE, IRQ_BASE + 0x224);

/* Enable IRQs globally */
#define irq_enable() do { \
	unsigned long temp; \
	__asm__ __volatile__ ("mrs %0, cpsr\n\t" \
	                      "bic %0, %0, #0x80\n\t" \
	                      "msr cpsr_c, %0\n\t" \
	                      : "=r" (temp) \
	                      : : "memory"); \
} while (0)

/* Disable IRQs globally */
#define irq_disable() do { \
	unsigned long temp; \
	__asm__ __volatile__ ("mrs %0, cpsr\n\t" \
	                      "orr %0, %0, #0x80\n\t" \
	                      "msr cpsr_c, %0\n\t" \
	                      : "=r" (temp) \
	                      : : "memory"); \
} while (0)

/* We used CPSR_C register instead of CPSR because we are interested
 * in just the "control" part of CPSR, that is the 8 less significant
 * bits of the register.
 * As seen in startup.S, 0x80 is the bit related to IRQ disabling.
 * Check page 2-24 of the ARM manual for further informations. */
