#include "stddef.h"
#include "stdint.h"

void __attribute__((noreturn, format(printf, 1, 2))) panic(const char *str, ...);
void decompress(const uint8_t *in, size_t in_size, uint8_t *out, size_t out_size);

extern uint8_t payload_start[];
extern uint8_t payload_end[];

extern uint8_t _bss_start[];
extern uint8_t _bss_end[];

#if __STDC_HOSTED__ == 0
int uart_putchar(int c);
int uart_puts(const char *str);
#define putchar(x) uart_putchar(x)
#define puts(x) uart_puts(x)
#endif
