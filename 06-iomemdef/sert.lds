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
 
/*
 * This file defines how compiler must write ELF sections into the
 * binary file
 */

/* Entry point function of the program */
ENTRY(_reset)

/* Memory addresses (refer to the Broadcom SoC manual) */
mem_start = 0x00008000;
mem_end	  = (512 - 64) * 1024 * 1024; /* 512MB of RAM excluding GPU memory (64MB by default) */

/* In which order sections should be written */
SECTIONS
{
    . = mem_start;			/* Set current address to mem_start */
    .text : {
		startup.o(.text)	/* Force the linker to put the entry point function code at the begining.
							 * In this way the processor find the right instruction when it starts executing the program */   
		. = ALIGN(4);
        *(.text)			/* Write code sections of all source files */
        . = ALIGN(4);		/* At the end of all .text sections, align current address to the next word */
    }
    .rodata : {
        *(.rodata)			/* Write all Read-Only data (eg. constant strings) */
        . = ALIGN(4);		/* Align current address to the next word */
    }
    .data : {
        *(.data)			/* Write all global variables */
        . = ALIGN(4);		/* Align current address to the next word */
    }
    .bss : {
        _bss_start = .;		/* Save current address in _bss_start variable */
        *(.bss)				/* Write all static variables that should be initialized to 0 */
        *(COMMON)
        . = ALIGN(4);		/* Align current address to the next word */
        _bss_end = .;		/* Save current address in _bss_end variable */
    }
    stack_top = mem_end;
}
