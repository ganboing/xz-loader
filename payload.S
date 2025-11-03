	.section .rodata.payload, "a", %progbits
	.align 4
	.globl payload_start
	.globl payload_end
payload_start:
#if !defined(PAYLOAD_XZ)
	#error "PAYLOAD_XZ must be defined"
#endif
	.incbin	PAYLOAD_XZ
payload_end:
