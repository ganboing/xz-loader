#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <xz.h>
#include "encoding-riscv.h"
#include "common.h"

typedef struct {
	unsigned long ra;
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
	unsigned long a4;
	unsigned long a5;
	unsigned long a6;
	unsigned long a7;
	unsigned long t0;
	unsigned long t1;
	unsigned long t2;
	unsigned long t3;
	unsigned long t4;
	unsigned long t5;
	unsigned long t6;
} boot_regs;

_Static_assert(sizeof(boot_regs) == sizeof(long) * 16, "Check boot_args size");

void boot(void *image_base, boot_regs *regs)
{
	uint8_t *pstart = payload_start, *pend = payload_end;
	uint8_t *bstart = _bss_start, *bend = _bss_end;

	for (; bstart != bend; ++bstart)
		if (*bstart)
			panic("bss section is not properly zeroed");

	printf("Image base = %p, Boot regs:\n"
		"\tra: " __REG_FMT__ " a0: " __REG_FMT__ " a1: " __REG_FMT__ " a2: " __REG_FMT__ "\n"
		"\ta3: " __REG_FMT__ " a4: " __REG_FMT__ " a5: " __REG_FMT__ " a6: " __REG_FMT__ "\n"
		"\ta7: " __REG_FMT__ " t0: " __REG_FMT__ " t1: " __REG_FMT__ " t2: " __REG_FMT__ "\n"
		"\tt3: " __REG_FMT__ " t4: " __REG_FMT__ " t5: " __REG_FMT__ " t6: " __REG_FMT__ "\n",
		image_base,
		regs->ra, regs->a0, regs->a1, regs->a2, regs->a3, regs->a4, regs->a5, regs->a6,
		regs->a7, regs->t0, regs->t1, regs->t2, regs->t3, regs->t4, regs->t5, regs->t6);

	xz_crc32_init();
	decompress(pstart, pend - pstart, image_base, PAYLOAD_SIZE);
	printf("Starting...\n");
	return;
}

void *malloc(size_t size)
{
	static uint8_t *marker = _bss_end;
	uint8_t *ret = marker;

	size = (size + 7) / 8 * 8;
	marker += size;
	printf("malloc returning %p for size %lu\n", ret, (unsigned long)size);
	return ret;
}

void free(void *ptr)
{
}
