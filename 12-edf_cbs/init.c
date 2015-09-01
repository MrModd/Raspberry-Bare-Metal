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

#include "raspberry.h"

#define VECTOR_BASE 0x00000000

void init_vectors(void)
{
	extern void _reset(void);
	extern void _irq_handler(void);
	
	volatile u32 *vectors = VECTOR_BASE;
	
#define LDR_PC_PC 0xe59ff018 /* Hex encoding of the instruction:
                              * 	ldr pc,[pc,#0x18]
                              * 
                              * Each instruction is 32bit, 4 byte long.
                              * 0x18 = 24 byte = 6 instructions.
                              * 
                              * This instruction is a jump at PC + 6,
                              * considering that PC points 2 instructions after
                              * the currently running one. */

	vectors[0] = LDR_PC_PC;		/* Reset */
	vectors[1] = LDR_PC_PC;		/* Undefined Instruction */
	vectors[2] = LDR_PC_PC;		/* Software Interrupt */
	vectors[3] = LDR_PC_PC;		/* Prefetch Abort */
	vectors[4] = LDR_PC_PC;		/* Data Abort */
	vectors[5] = LDR_PC_PC;		/* Reserved */
	vectors[6] = LDR_PC_PC;		/* IRQ */
	vectors[7] = LDR_PC_PC;		/* FIQ */
	
	vectors[8] =  (u32) _reset;	/* Reset */
	vectors[9] =  (u32) panic0;	/* Undefined Instruction */
	vectors[10] = (u32) panic1;	/* Software Interrupt */
	vectors[11] = (u32) panic2;	/* Prefetch Abort */
	vectors[12] = (u32) panic3;	/* Data Abort */
	vectors[13] = (u32) panic4;	/* Reserved */
	vectors[14] = (u32) _irq_handler;	/* IRQ */
	vectors[15] = (u32) panic4;	/* FIQ */
	
	/* Set base address of the exception vector (ARM manual p. 3-121) */
	__asm__ __volatile__ ("mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" (vectors));
	/* For a description of mcr instruction and extended assembly see
	 * raspberry_cpu.h header file.
	 * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/Babfbcae.html */

#undef LDR_PC_PC
}

/* Enable floating point units */
void init_vfp()
{
	u32 acr = read_coprocessor_access_control_register();
	
	u32 vfp_single = COPROCESSOR_ACCESS_USER << VFP_SINGLE_OFFSET;
	u32 vfp_double = COPROCESSOR_ACCESS_USER << VFP_DOUBLE_OFFSET;
	
	acr |= vfp_single | vfp_double; /* Enable single / double precision coprocessor units */
	
	write_coprocessor_access_control_register(acr);
	__memory_barrier();
	
	enable_vfp();
}

/* Initialize all interrupts */
void init_irq()
{
	/* Mask all interrupts */
	iomem(IRQ_DISABLE1) = 0xfffffffful;
	iomem(IRQ_DISABLE2) = 0xfffffffful;
	iomem(IRQ_BASIC_DISABLE) = 0xfffffffful;
	
	/* Enable interrupts globally */
	irq_enable();
}

/* Init to 0 section .bss, where static variables that
 * must be initialized to 0 are located */
void init_bss()
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

void init_gpio()
{
	/* Init GPIO 16 (wired to LED ACT) as Output */
	
	//iomem(GPFSEL1) |= (1<<18);
	
	/* Or */
	//gpio[GPFSEL1/4] &= ~((1<<20)|(1<<19)|(1<<18));
	//gpio[GPFSEL1/4] |= ((0<<20)|(0<<19)|(1<<18));
	
	/* Or */
	
	GPIO1_SET_OUTPUT(GPIO_LED_SEL_OFFSET)
}

void remove_pull_resistors_uart()
{
	/* Remove pull resistors from UART pins (SoC manual at page 100-101) */
	
	iomem(GPIO_GPPUD) &= 0u; /* 0b00 in order to remove pull resistors */
	loop_delay(80); /* Wait 150 clock cycles (loop_delay(1) takes more than 1 tick) */
	iomem_high(GPIO_GPPUDCLK0, ((1<<GPIO_UART_TX) | (1<<GPIO_UART_RX)));
	/* Bits set to 1 in GPIO_GPPUDCLK0/1 assume the pull-up/down state
	 * defined in register GPPUD. Having set to 0 that register we want
	 * to disable pull resistors from GPIO 14 and 15 */
	loop_delay(80); /* Wait 150 clock cycles (loop_delay(1) takes more than 1 tick) */
	/* We should reset GPIO_GPPUD register, but it is already 0 */
	iomem(GPIO_GPPUDCLK0) &= 0u; /* Now GPIO14 and 15 has taken requested state. Reset the register */
}

void init_uart()
{
	/* Examples:
	 * http://wiki.osdev.org/ARM_RaspberryPi_Tutorial_C#uart.c
	 * https://github.com/dwelch67/raspberrypi/tree/master/uartx01 */
	
	/* Wait until previous transmissions end (uboot) */
	while(!(iomem(UART_FR) & UART_FR_TXFE));
	
	/* Disable UART0 and clear Control Register */
	iomem(UART_CR) = 0u;

	/* Set UART0 mode for GPIO14-15 */
	GPIO_SET_FUNC(GPIO_GPFSEL1,GPIO_UART_TX_SEL_OFFSET,GPIO_ALT1_MASK)
	GPIO_SET_FUNC(GPIO_GPFSEL1,GPIO_UART_RX_SEL_OFFSET,GPIO_ALT1_MASK)
	
	remove_pull_resistors_uart();
	
	/* Initialize UART0 */
	iomem(UART_ICR) = 0x7FF; /* Clear pendant interrupts */
	iomem(UART_FR) = 0x80; /* Reset flag register */
	
	// Set integer & fractional part of baud rate.
	// Divider = UART_CLOCK/(16 * Baud)
	// Fraction part register = (Fractional part * 64) + 0.5
	// UART_CLOCK = 3MHz; Baud = 115200.
	// Divider = 3000000/(16 * 115200) = 1.627 = ~1.
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	iomem(UART_IBRD) = 1; /* Integer part of the Baud Rate divisor */
	iomem(UART_FBRD) = 40; /* Fractional part of the Baud Rate divisor */
	
	iomem(UART_LCRH) = UART_LCRH_WLEN | UART_LCRH_FEN; /* Set UART characteristics (8bit, FIFO enabled) */
	iomem(UART_IMSC) = 0u; /* Clear all interrupts */
	iomem_high(UART_CR, UART_CR_TXE | UART_CR_RXE); /* Enable sending and receiving channel */

	/* Enable UART0 */
	iomem_high(UART_CR, UART_CR_UARTEN);
}

void init_miniuart()
{
	/* Examples:
	 * https://github.com/dwelch67/raspberrypi/blob/master/uart01/uart01.c */
	
	/* Wait until previous transmissions end (Uboot) */
	while(!(iomem(UART_FR) & UART_FR_TXFE)); /* (Uboot uses UART0) */
	 
	/* Disable UART0 */
	iomem_low(UART_CR, UART_CR_UARTEN);
	
	/* Set UART1 (mini UART) mode for GPIO14-15 */
	GPIO_SET_FUNC(GPIO_GPFSEL1,GPIO_UART_TX_SEL_OFFSET,GPIO_ALT5_MASK)
	GPIO_SET_FUNC(GPIO_GPFSEL1,GPIO_UART_RX_SEL_OFFSET,GPIO_ALT5_MASK)
	
	remove_pull_resistors_uart();
	
	iomem(AUX_ENABLES) = AUX_EN_UART;
	iomem(AUX_MU_IER_REG) = 0; /* Disable interrupt */
	iomem(AUX_MU_IIR_REG) = 0; /* Disable interrupt */
	iomem(AUX_MU_CNTL_REG) = 0; /* Reset UART configuration register */
	iomem(AUX_MU_LCR_REG) = AUX_MU_LCR_DATA_SIZE_MASK; /* Reset data format register (SEE THE DATASHEET ERRATA)*/
	/* baudrate = system_clock_freq / (8 * (baudrate_reg + 1))
	 * baudrate_reg = (system_clock_freq / (8 * baudrate)) - 1
	 * 
	 * Where system_clock_freq is the GPU frequency (default at 250MHz)
	 * 
	 * 250000000/8/1152000 - 1 ~= 270 */
	iomem(AUX_MU_BAUD_REG) = 270;
	
	iomem_high(AUX_MU_CNTL_REG, AUX_MU_CNTL_TX | AUX_MU_CNTL_RX); /* Enable sending and receiving channel */
	
	/* Wait a bit */
	loop_delay(100u);
}

/* Init memory peripherals and then jump to entry() */
void _init()
{
	init_bss();
	init_vectors();
	init_vfp();
	init_gpio();
	
#ifdef MINI_UART
	init_miniuart();
#else
	init_uart();
#endif
	
	init_irq();
	init_ticks();
	
	init_taskset();
	
	/* Create a task for the CBS server.
	 * Maximum budget 25 unit time (ticks) per period
	 * Period of 250 ticks. */
	if (init_cbs(25, 250, &cbs0, "cbs0") == -1) /* Create a task for the CBS server */
		_panic(__FILE__, __LINE__, "Cannot create CBS server task.");
	
	/* Prevent code reordering (just in case) */
	__memory_barrier();
	
	/* Jump to the entry point function that run user defined code */
	entry();
}
