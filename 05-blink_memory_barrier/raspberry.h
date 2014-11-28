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
#define RASPBERRY_H

#include "raspberry_cpu.h"
#include "common.h"

typedef unsigned int u32;

/* Base GPIO address can be found in the Broadcom SoC manual at
 * page 90.
 * The referred address, 0x7E200000, is a virtual BUS memory address.
 * Physical memory address is the following (refer to page 5 of the
 * manual)
 */
#define GPIO_BASE 0x20200000

/* Offset of the GPIO registers in relation to GPIO_BASE */
#define GPFSEL1 0x4			/* GPIO Function Select 1 */
#define GPLEV0 0x34			/* GPIO Pin Level 0 */
#define GPSET0 0x1C			/* GPIO Pin Output Set 0 */
#define GPCLR0 0x28			/* GPIO Pin Output Clear 0 */

#endif
