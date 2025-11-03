.PHONY: clean

all: test.elf boot-rv32.bin

clean:
	rm -f boot-rv32.elf boot-rv32.bin test.elf

ifdef PAYLOAD_XZ
CPPFLAGS += -DPAYLOAD_XZ=\"$(PAYLOAD_XZ)\"
CPPFLAGS += -DPAYLOAD_SIZE=$(shell xzcat $(PAYLOAD_XZ) | wc -c)
endif

CPPFLAGS += -Ixz-embedded/linux/include/linux -Iconfig

CFLAGS += -std=gnu11 -fno-strict-aliasing -ffunction-sections -fdata-sections
CFLAGS_CROSS := -fPIE -Wall -O2 -nostdlib -ffreestanding -fno-stack-protector -Wl,--exclude-libs,ALL
CFLAGS_RV32 := -mabi=ilp32 -march=rv32im_zifencei

# For riscv, the payload can be compressed with:
#   xz --riscv --lzma2=preset=0e,lc=2,lp=2,pb=2,mf=bt3 -C crc32 -vv < payload.bin > payload.xz

xz-objs := xz_crc32.c xz_dec_stream.c xz_dec_bcj.c xz_dec_lzma2.c
xz-objs := $(addprefix xz-embedded/linux/lib/xz/,$(xz-objs))

# RV32 needs libquad for implementations such as __umoddi3
libquad-objs := umoddi3.c udivdi3.c qdivrem.c
libquad-objs := $(addprefix libquad/,$(libquad-objs))

# Also needs libgcc for implementations such as __ashldi3
libgcc-objs := ashldi3.c
libgcc-objs := $(addprefix libgcc/,$(libgcc-objs))

boot-rv32-objs := boot.c string.c printf.c uart.c decompress.c payload.S entry-riscv.S
boot-rv32.elf: CFLAGS += $(CFLAGS_CROSS) $(CFLAGS_RV32)
boot-rv32.elf: LDFLAGS += -T ld-riscv.lds -Wl,--gc-sections -Wl,--no-dynamic-linker -Wl,-pie
boot-rv32.elf: $(boot-rv32-objs) ld-riscv.lds
	$(CROSS_COMPILE)$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ \
		$(boot-rv32-objs) $(libquad-objs) $(libgcc-objs) $(xz-objs)

boot-rv32.bin: boot-rv32.elf
	$(CROSS_COMPILE)objcopy -O binary $< $@

test.elf: CFLAGS += -fsanitize=address -fPIE -Wall -Os
test.elf: LDFLAGS += -Wl,--gc-sections
test.elf: test.c string.c printf.c decompress.c payload.S
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(xz-objs)
