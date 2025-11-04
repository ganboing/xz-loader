#include <stdint.h>
#include "common.h"

#if !defined(UART_BASE)
	#error "UART_BASE must be defined"
#endif

typedef struct {
	volatile uint32_t thr;
	uint32_t ignored[4];
	volatile uint32_t lsr;
} uart8250;

static uart8250 * const uart = (uart8250*)UART_BASE;

#define UART_LSR_THRE 0x20

int uart_putchar(int c)
{
	while ((uart->lsr & UART_LSR_THRE) == 0)
		;
	uart->thr = c;
	return c;
}

int uart_puts(const char *str)
{
	int len;

	for (len = 0; *str; ++len)
		putchar(*str++);

	putchar('\n');
	return len + 1;
}

