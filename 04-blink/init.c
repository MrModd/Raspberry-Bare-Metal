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

/* Init to 0 section .bss, where static variables that
 * must be initialized to 0 are located */
static void init_bss(void)
{
	/* _bss_start and _bss_end symbols are defined during
	 * linking process in file sert.lds */
	extern u32 _bss_start, _bss_end;
	u32 *p;
	
	for (p = &_bss_start; p < &_bss_end; p++)
		*p = 0UL;
	
	/* Why we use &_bss_start and &_bss_end?
	 * We must set to 0 all memory cells between _bss_start and
	 * _bss_end, so we must iterate on the address of these
	 * variables:
	 *                  ___________
	 * _bss_end        |           |
	 *                 |           |
	 *                 |           |
	 *                 |           |
	 * _bss_start      |           |
	 *                 |___________|
	 * 
	 * these symbols doesn't point to the memory area to erase,
	 * they ARE the memory addresses to erase. So we put p equals
	 * to the address of _bss_start and we iterate until _bss_end */
}

/* Init memory and then jump to entry() */
void _init(void)
{
	init_bss();
	
	/* Jump to the entry point function that run user defined code */
	entry();
}
