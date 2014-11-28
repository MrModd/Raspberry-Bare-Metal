#include "raspberry.h"

int putc_uart0(int ch)
{
	/* UART0 */
	//while(!(iomem(UART_FR) & UART_FR_TXFE));
	while(iomem(UART_FR) & UART_FR_TXFF);
	iomem(UART_DR) |= ch & 0xff;
	
	return 1;
}

int putc_uart1(int ch)
{
	/* UART1 */
	while(!(iomem(AUX_MU_LSR_REG) & AUX_MU_LSR_TX_EMPTY));
	iomem(AUX_MU_IO_REG) |= ch & 0xff;
	
	return 1;
}

int putc(int ch)
{
	return putc_uart1(ch);
}

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
	 * ciphers that will be printed in inverse order */
	
	/* A 32bit number can contains up to 10 ciphers, considering the trailing line we need an 11 positions length buffer */
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
int putf(double v, int prec /* How many decimal ciphers to print */)
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
