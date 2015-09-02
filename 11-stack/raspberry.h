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
#define RASPBERRY_H

typedef unsigned int u32;

/* _iomem is a address that will be used as base address of the memory.
 * It is important to set as 0 because compiler can make optimizations when
 * accessing to an offset of this, because the sum of a number with
 * zero is always the number itself. This prevents to load the constant
 * value from memory. See also the iomem(N) definition below. */
static volatile u32 * const _iomem = (u32*) 0;

/* iomemdef generates new constants called as parameter N, the first argument
 * of the macro call, and value as parameter V, the second argument.
 * This macro is useful in order to define memory address at runtime as constants
 * instead of variables. Using constants avoids the necessity to fetch the variable
 * value from the RAM. */
#define iomemdef(N, V) enum { N = (V) / sizeof(u32) }

/* iomem(N) is used to access address N as if it was a function, but it has the benefit
 * of avoiding RAM accesses to fetch addresses. In fact N is a constant defined by
 * iomemdef and does not require a memory access to get its value, moreover the
 * _iomem + N sum is optimized by the compiler in N because _iomem is 0. */
#define iomem(N) _iomem[N]

/* Set to 1 all bits of register reg that are marked as 1 in the mask */
static inline void iomem_high(unsigned int reg, u32 mask)
{
	iomem(reg) |= mask;
}

/* Set to 0 all bits of register reg that are marked as 1 in the mask */
static inline void iomem_low(unsigned int reg, u32 mask)
{
	iomem(reg) &= ~mask;
}

#include "raspberry_cpu.h"
#include "raspberry_gpio.h"
#include "raspberry_led.h"
#include "raspberry_uart.h"
#include "raspberry_irq.h"
#include "raspberry_timer.h"
#include "common.h"

#endif
