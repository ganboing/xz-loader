#include "encoding-riscv.h"

#define PAYLOAD_ALIGNED (PAYLOAD_SIZE + 7) / 8 * 8

	.section .entry, "ax", %progbits
	.globl _start
_start:
	addi	sp, sp, - __SIZEOF_POINTER__ * 16
	REG_S	t6, __SIZEOF_POINTER__ * 15(sp)
	REG_S	t5, __SIZEOF_POINTER__ * 14(sp)
	REG_S	t4, __SIZEOF_POINTER__ * 13(sp)
	REG_S	t3, __SIZEOF_POINTER__ * 12(sp)
	REG_S	t2, __SIZEOF_POINTER__ * 11(sp)
	REG_S	t1, __SIZEOF_POINTER__ * 10(sp)
	REG_S	t0, __SIZEOF_POINTER__ * 9(sp)
	REG_S	a7, __SIZEOF_POINTER__ * 8(sp)
	REG_S	a6, __SIZEOF_POINTER__ * 7(sp)
	REG_S	a5, __SIZEOF_POINTER__ * 6(sp)
	REG_S	a4, __SIZEOF_POINTER__ * 5(sp)
	REG_S	a3, __SIZEOF_POINTER__ * 4(sp)
	REG_S	a2, __SIZEOF_POINTER__ * 3(sp)
	REG_S	a1, __SIZEOF_POINTER__ * 2(sp)
	REG_S	a0, __SIZEOF_POINTER__ * 1(sp)
	REG_S	ra, __SIZEOF_POINTER__ * 0(sp)

	lla	t2, _loader_start	/* load addr, assuming link base is 0 */
	li	t3, PAYLOAD_ALIGNED
	add	t2, t2, t3		/* t2, t4 <- new base */
	mv	t4, t2
	lla	t0, _loader_start
	lla	t1, _bss_start
.Lcopy_loop:
	REG_L	t3, 0(t0)
	REG_S	t3, 0(t2)
	addi	t0, t0, __SIZEOF_POINTER__
	addi	t2, t2, __SIZEOF_POINTER__
	blt	t0, t1, .Lcopy_loop
	fence.i
	j	.Lcopy_done + PAYLOAD_ALIGNED
.Lcopy_done:
	/* relocate the global table content */
	lla	t0, __rel_dyn_start
	lla	t1, __rel_dyn_end
	beq	t0, t1, .Lrelocate_done
.Lrelocate_loop:
	REG_L	t5, __SIZEOF_POINTER__(t0)	/* t5 <-- relocation info:type */
	li	t3, R_RISCV_RELATIVE		/* reloc type R_RISCV_RELATIVE */
	bne	t5, t3, .Lrelocate_skip
	REG_L	t3, 0(t0)			/* t3 <-- offset */
	REG_L	t5, (__SIZEOF_POINTER__ * 2)(t0)/* t5 <-- addend */
	add	t5, t5, t4
	add	t3, t3, t4
	REG_S	t5, 0(t3)		/* store runtime address to the GOT entry */
.Lrelocate_skip:
	addi	t0, t0, (__SIZEOF_POINTER__ * 3)
	blt	t0, t1, .Lrelocate_loop
.Lrelocate_done:

	lla	t0, _bss_start
	lla	t1, _bss_end
.Lbss_zero_loop:
	bgeu	t0, t1, .Lbss_zero_done
	REG_S	zero, (t0)
	add	t0, t0, __SIZEOF_POINTER__
	j	.Lbss_zero_loop
.Lbss_zero_done:

	li	a0, PAYLOAD_ALIGNED
	sub	a0, t4, a0
	mv	a1, sp
	jal	boot

	REG_L	ra, __SIZEOF_POINTER__ * 0(sp)
	REG_L	a0, __SIZEOF_POINTER__ * 1(sp)
	REG_L	a1, __SIZEOF_POINTER__ * 2(sp)
	REG_L	a2, __SIZEOF_POINTER__ * 3(sp)
	REG_L	a3, __SIZEOF_POINTER__ * 4(sp)
	REG_L	a4, __SIZEOF_POINTER__ * 5(sp)
	REG_L	a5, __SIZEOF_POINTER__ * 6(sp)
	REG_L	a6, __SIZEOF_POINTER__ * 7(sp)
	REG_L	a7, __SIZEOF_POINTER__ * 8(sp)
	REG_L	t0, __SIZEOF_POINTER__ * 9(sp)
	REG_L	t1, __SIZEOF_POINTER__ * 10(sp)
	REG_L	t2, __SIZEOF_POINTER__ * 11(sp)
	REG_L	t3, __SIZEOF_POINTER__ * 12(sp)
	REG_L	t4, __SIZEOF_POINTER__ * 13(sp)
	REG_L	t5, __SIZEOF_POINTER__ * 14(sp)
	REG_L	t6, __SIZEOF_POINTER__ * 15(sp)
	addi	sp, sp, __SIZEOF_POINTER__ * 16
	fence.i
	j	_loader_start - PAYLOAD_ALIGNED
