# QEMU tests for halow_sched.c on a Cortex-M55 (mps3-an547 == the N6's core).
#
#   make        build + run under QEMU
#
# Uses the OpenMV SDK toolchain, same as the firmware build: Ubuntu's
# arm-none-eabi-gcc 13.2 is rejected for Cortex-M55 by check_toolchain.mk.
SDK      ?= $(HOME)/openmv-sdk-1.6.0
CC       := $(SDK)/gcc/bin/arm-none-eabi-gcc
HALOW    := ../..
MPY      := ../../../..
LDSCRIPT := $(MPY)/ports/qemu/mcu/arm/mps3.ld

# Matches ports/qemu/boards/MPS3_AN547/mpconfigboard.mk, which in turn matches
# how the N6 is built: hard float, so the vpush {d8-d15} path is live.
ARCH   := -mthumb -mcpu=cortex-m55 -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS := $(ARCH) -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -Og -g \
          -ffreestanding -fno-common \
          -DMICROPY_PY_NETWORK_HALOW=1 \
          -Istub -I$(HALOW) -I$(MPY)/lib/mm-iot-sdk/framework/morselib/include
LDFLAGS := $(ARCH) -T$(LDSCRIPT) -nostartfiles -Wl,--gc-sections \
           -Wl,-Map=test_sched.map -specs=nosys.specs \
           -Wl,--no-warn-rwx-segments

SRC := startup.c test_sched.c $(HALOW)/halow_sched.c

all: run

test_sched.elf: $(SRC) $(LDSCRIPT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC)

run: test_sched.elf
	@qemu-system-arm -M mps3-an547 -cpu cortex-m55 -nographic \
	    -semihosting-config enable=on,target=native \
	    -kernel test_sched.elf

clean:
	rm -f test_sched.elf test_sched.map

.PHONY: all run clean
