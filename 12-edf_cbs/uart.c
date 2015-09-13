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

static void remove_pull_resistors_uart(void)
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

void init_uart(void)
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

void init_miniuart(void)
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
	iomem(AUX_MU_LCR_REG) = AUX_MU_LCR_DATA_SIZE_MASK; /* Reset data format register (SEE THE DATASHEET ERRATA) */
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

int putc_uart0(int ch)
{
	/* UART0 */
	//while(!(iomem(UART_FR) & UART_FR_TXFE)); /* Wait until FIFO becomes empty */
	while(iomem(UART_FR) & UART_FR_TXFF); /* Wait until FIFO becomes not full */
	iomem(UART_DR) = ch & 0xff;
	
	return 1;
}

int putc_uart1(int ch)
{
	/* UART1 */
	while(!(iomem(AUX_MU_LSR_REG) & AUX_MU_LSR_TX_EMPTY)); /* Wait until FIFO can accept at least 1 byte */
	iomem(AUX_MU_IO_REG) |= ch & 0xff;
	
	return 1;
}

#ifdef MINI_UART
#define putc(ch)	putc_uart1(ch)
#else
#define putc(ch)	putc_uart0(ch)
#endif

int puts(const char *st)
{
	int v = 0;
	while (*st) { /* Until st doesn't point to a \0 character */
		v+= putc(*st);
		if(*st++ == '\n')
			v += putc('\r');
		/* OPERATOR PRECEDENCE:
		 * ++ precedes *, but it is a post increment, so it returns
		 * old values of the variable and * reads correctly this value. */
	}
	
	return v;
}

int puth(unsigned long v) /* Write a number in hexadecimal notation */
{
	/* Consider groups of 4bits */
	int i, d, w = 0;
	u32 mask;
	
	puts("0x");
	
	/* We must write from the most significant value, because we reads in this way */
	mask = 0xf0000000;
	for (i = 0; mask != 0; i += 4, mask >>= 4) {
		d = (v & mask) >> (28-i); /* This is a 32bit number, but we are interested in just 4 bits and we need to shift them to the least significant bits */
		w += putc(d + (d > 9 ? 'a' - 10 : '0' ));
	}
	
	return w;
}

int putu(unsigned long v)
{
	/* Dividing every time by 10 we can read digit per digit, but we need a buffer in order to accumulate discovered
	 * digits that will be printed in inverse order */
	
	/* A 32bit number can contains up to 10 digits, considering the trailing line we need an 11 positions length buffer */
	char buf[11];
	int i, r;
	if (v < 10) {
		return putc(v + '0');
	}
	i = 10;
	buf[i] = '\0';
	while (v != 0) {
		unsigned long w = v / 10;
		r = v - w * 10; /* Reminder of the division (modulus is more expensive because it requires more divisions) */
		buf[--i] = (char) (r + '0');
		v = w;
	}
	
	return puts(buf + i);
	/* Start printing from the last written position, we don't know if the buffer is entirely full */
}

/* Signed numbers */
int putd(long v)
{
	int w = 0;
	if (v < 0) {
		w += putc('-');
		v = -v;
	}
	w += putu(v);
	return w;
}

/* Floating point numbers */
int putf(double v, int prec /* How many decimal digits to print */)
{
	int i, w = 0;
	if (v < 0.0) {
		w += putc('-');
		v = -v;
	}
	w += putu(v); /* Print the integer part, automatic cast from double to int removes the fractional part */
	w += putc('.');
	for (i=0; i<prec; ++i) {
		v = v - (long) v; /* Exclude integer part (already printed) */
		v *= 10; /* First digit after the point */
		w += putc('0' + (long) v);
	}
	return w;
}
