/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * include/lib/libgcc.h
 */

#ifndef __LIB_LIBGCC_H
#define __LIB_LIBGCC_H

#include <stdlib.h>

typedef int word_type __attribute__ ((mode (__word__)));

#if _BYTE_ORDER == _BIG_ENDIAN
struct DWstruct {
	int high, low;
};
#elif _BYTE_ORDER == _LITTLE_ENDIAN
struct DWstruct {
	int low, high;
};
#else
#error I feel sick.
#endif

typedef union {
	struct DWstruct s;
	long long ll;
} DWunion;

long long __ashldi3(long long u, word_type b);
long long __ashrdi3(long long u, word_type b);
word_type __cmpdi2(long long a, long long b);
long long __lshrdi3(long long u, word_type b);
long long __muldi3(long long u, long long v);
word_type __ucmpdi2(unsigned long long a, unsigned long long b);

#endif /* __ASM_LIBGCC_H */
