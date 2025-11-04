## Self-Extracting (XZ) Firmware/Bootloader Compressor

Usage for building a risv32 firmware blob:
```shell
xz --riscv --lzma2=preset=0e,lc=2,lp=2,pb=2,mf=bt3 -C crc32 < loader.bin > loader.xz
make CROSS_COMPILE=riscv32-unknown-linux-gnu- CC=gcc PAYLOAD_XZ=payload.xz boot-rv32.bin
```

The `boot-rv32.bin` can be used in place of `loader.bin`
