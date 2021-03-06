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

#define GPIO_UART_TX 14
#define GPIO_UART_RX 15

#define GPIO_UART_TX_SEL_OFFSET 12
#define GPIO_UART_RX_SEL_OFFSET 15

/* UART (PL011) DEFINITIONS */

/* Chapter 13 of the Broadcom SoC manual (pag. 175) */
#define UART_BASE 0x20201000 /* SHAME ON YOU BROADCOM!!! HOW THE FUCK CAN YOU MISPELL AN ADDRESS??? */

iomemdef(UART_DR, UART_BASE + 0x0); /* Data Register (controls the Transmit Holding Register) */
iomemdef(UART_FR, UART_BASE + 0x18); /* Flag Register */
iomemdef(UART_LCRH, UART_BASE + 0x2c); /* Line Control Register */
iomemdef(UART_CR, UART_BASE + 0x30); /* Control Register */
iomemdef(UART_ICR, UART_BASE + 0x44); /* Interupt Clear Register */
iomemdef(UART_IBRD, UART_BASE + 0x24); /* Integer Baud rate divisor */
iomemdef(UART_FBRD, UART_BASE + 0x28); /* Fractional Baud rate divisor */
iomemdef(UART_IMSC, UART_BASE + 0x38); /* Interupt Mask Set Clear Register */

/* FR register (page 181) */
#define UART_FR_TXFE (1u<<7) /* 1 if Transmit Holding Register is empty */
#define UART_FR_TXFF (1u<<5) /* 1 if transmission FIFO (if enabled) or the THR (if FIFO disabled) are full */
#define UART_FR_BUSY (1u<<3) /* 1 if currently sending or receiving data */

/* LCRH register (page 184) */
#define UART_LCRH_WLEN (3u<<5) /* Bits that define word length */
#define UART_LCRH_FEN (1u<<4) /* Set to 1 to enable FIFO queues */
#define UART_LCRH_PEN (1u<<1) /* Set to 1 to enable parity bit */

/* CR register (page 185) */
#define UART_CR_TXE (1u<<8) /* Enable transmission channel */
#define UART_CR_RXE (1u<<9) /* Enable reception channel */
#define UART_CR_RTS (1u<<11) /* Request to send */
#define UART_CR_UARTEN (1u) /* Enable UART globally */



/* MINI UART DEFINITIONS */

/* Chapter 2 of Broadcom SoC manual (page 8) */
#define AUX_BASE 0x20215000

iomemdef(AUX_ENABLES, AUX_BASE + 0x4); /* Enable auxiliary peripherals globally (SPI1, SPI2, UART1) */
iomemdef(AUX_MU_IO_REG, AUX_BASE + 0x40); /* Mini Uart I/O Data */
iomemdef(AUX_MU_IER_REG, AUX_BASE + 0x44); /* Mini Uart Interrupt Enable */
iomemdef(AUX_MU_IIR_REG, AUX_BASE + 0x48); /* Mini Uart Interrupt Identify */
iomemdef(AUX_MU_LCR_REG, AUX_BASE + 0x4C); /* Mini Uart Line Control */
iomemdef(AUX_MU_LSR_REG, AUX_BASE + 0x54); /* Mini Uart Line Status */
iomemdef(AUX_MU_CNTL_REG, AUX_BASE + 0x60); /* Mini Uart Extra Control */
iomemdef(AUX_MU_STAT_REG, AUX_BASE + 0x64); /* Mini Uart Extra Status */
iomemdef(AUX_MU_BAUD_REG, AUX_BASE + 0x68); /* Mini Uart Baudrate */

/* AUX_ENABLES register (page 9) */
#define AUX_EN_UART 1u /* bit 0: Mini UART enable */

/* AUX_MU_LCR_REG (page 14)
 * Check http://elinux.org/BCM2835_datasheet_errata#p14 for datasheet errata */
#define AUX_MU_LCR_DATA_SIZE_MASK (3u) /* 00 for 7bit/symbol, 11 for 8bit/symbol */

/* AUX_MU_LSR_REG register (page 15) */
#define AUX_MU_LSR_TX_EMPTY (1u<<5) /* 1 if transmission FIFO can accept at least 1 byte */

/* AUX_MU_CNTL register (page 16) */
#define AUX_MU_CNTL_RX 1u /* bit 0: Receiver enable */
#define AUX_MU_CNTL_TX (1u<<1) /* bit 1: Transmitter enable */

/* AUX_MU_STAT_REG register (page 18) */
#define AUX_MU_STAT_TX_EMPTY (1u<<8) /* 1 if transmission FIFO is empty */
#define AUX_MU_STAT_TX_FULL (1u<<5) /* 1 if transmission FIFO is full */
