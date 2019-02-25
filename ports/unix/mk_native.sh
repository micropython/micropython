#!/bin/sh

# x64
CFLAGS=-fno-stack-protector

# thumb
#CROSS=arm-none-eabi-
#CFLAGS=-mthumb

# xtensa
#CROSS=xtensa-lx106-elf-
#CFLAGS=-mforce-l32

${CROSS}gcc -fPIC -fPIE $CFLAGS -std=c99 -Os -c native_ex.c -o native_ex.o
${CROSS}ld -T native_ex.ld native_ex.o -o native_ex.elf
${CROSS}objcopy -O binary -j .header -j .text -j .trailer native_ex.elf native_ex.mpy
