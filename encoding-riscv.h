/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Bo Gan <ganboing@gmail.com>
 */

#ifdef __ASSEMBLER__
#define __ASM_STR(x)    x
#else
#define __ASM_STR(x)    #x
#endif

#if __riscv_xlen == 64
#define __REG_SEL(a, b) __ASM_STR(a)
#define __REG_FMT__ "%016lx"
#elif __riscv_xlen == 32
#define __REG_SEL(a, b) __ASM_STR(b)
#define __REG_FMT__ "%08lx"
#else
#error "Unexpected __riscv_xlen"
#endif

#define REG_L           __REG_SEL(ld, lw)
#define REG_S           __REG_SEL(sd, sw)

#define R_RISCV_RELATIVE	3
