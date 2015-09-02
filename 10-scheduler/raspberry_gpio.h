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


/* Base GPIO address can be found in the Broadcom SoC manual at
 * page 90.
 * The referred address, 0x7E200000, is a virtual BUS memory address.
 * Physical memory address is the following (refer to page 5 of the
 * manual)
 */
#define GPIO_BASE 0x20200000   

/* Bit mask definitions for alternative functions of a GPIO (3 bits) */
#define GPIO_FUNCTION_MASK 0b111
#define GPIO_INPUT_MASK 0b000
#define GPIO_OUTPUT_MASK 0b001
#define GPIO_ALT1_MASK 0b100
#define GPIO_ALT2_MASK 0b101
#define GPIO_ALT3_MASK 0b111
#define GPIO_ALT4_MASK 0b011
#define GPIO_ALT5_MASK 0b010

/* Offset of the GPIO registers in relation to GPIO_BASE */
iomemdef(GPIO_GPFSEL1, GPIO_BASE + 0x4);			/* GPIO Function Select 1 */
iomemdef(GPIO_GPLEV0, GPIO_BASE + 0x34);			/* GPIO Pin Level 0 */
iomemdef(GPIO_GPSET0, GPIO_BASE + 0x1C);			/* GPIO Pin Output Set 0 */
iomemdef(GPIO_GPCLR0, GPIO_BASE + 0x28);			/* GPIO Pin Output Clear 0 */
iomemdef(GPIO_GPPUD, GPIO_BASE + 0x94);				/* GPIO Pin Pull-up/down Enable */
iomemdef(GPIO_GPPUDCLK0, GPIO_BASE + 0x98);			/* GPIO Pin Pull-up/down Enable Clock 0 */

#define GPIO_SET_FUNC(REG,N,FUNC)	do { \
										iomem_low(REG, GPIO_FUNCTION_MASK << N); \
										iomem_high(REG, FUNC << N); \
									} while (0);

#define GPIO0_SET_LOW(N)			do { \
										iomem_high(GPIO_GPCLR0, 1<<N); \
									} while (0);

#define GPIO0_SET_HIGH(N)			do { \
										iomem_high(GPIO_GPSET0, 1<<N); \
									} while (0);

#define GPIO1_SET_OUTPUT(N) GPIO_SET_FUNC(GPIO_GPFSEL1,N,GPIO_OUTPUT_MASK)

static const u32 const LEV0[] = {GPIO_GPSET0, GPIO_GPCLR0};

#define GPIO0_TOGGLE_BIT(N)	do { \
									iomem_high( LEV0[( (iomem(GPIO_GPLEV0) & (1<<N) ) >> N)], 1<<N ); \
							} while (0);
/* Meaning of the macro:
 * from register GPIO_GPLEV0 read the N-th bit, clearing all other bits and shifting the N-th bit
 * to 0 position;
 * if the bit is 0, then it must be set to 1 using the GPIO_GPSET0 register, vice versa it must
 * be cleared (se to 0) using the GPIO_GPCLR0 register.
 * Location 0 of array LEV0 is the address of GPIO_GPSET0 register, so LEV0[0] returns the address of
 * the register required to set the bit to 1, vice versa LEV0[1] returns the address of the register
 * required to clear the bit.
 * iomem_high set to 1 the requested bit in the register pointed by LEV0: GPIO_GPSET0 if bit was 0
 * or GPIO_GPCLR0 if bit was 1. */
