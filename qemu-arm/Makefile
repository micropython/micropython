include ../py/mkenv.mk
-include mpconfigport.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

INC += -I.
INC += -I..
INC += -I$(BUILD)
INC += -I../tools/tinytest/

CFLAGS_CORTEX_M3 = -mthumb -mcpu=cortex-m3
CFLAGS = $(INC) -Wall -Wpointer-arith -Werror -ansi -std=gnu99 $(CFLAGS_CORTEX_M3) $(COPT) \
	 -flto -ffunction-sections -fdata-sections

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g -DPENDSV_DEBUG
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

## With CoudeSourcery it's actually a little different, you just need `-T generic-m-hosted.ld`.
## Although for some reason `$(LD)` will not find that linker script, it works with `$(CC)`.
## It turns out that this is specific to CoudeSourcery, and ARM version of GCC ships something
## else instead and according to the following files, this is what we need to pass to `$(CC).
## - gcc-arm-none-eabi-4_8-2014q1/share/gcc-arm-none-eabi/samples/src/makefile.conf
## - gcc-arm-none-eabi-4_8-2014q1/share/gcc-arm-none-eabi/samples/src/qemu/Makefile
LDFLAGS= --specs=nano.specs --specs=rdimon.specs -Wl,--gc-sections -Wl,-Map=$(@:.elf=.map)

SRC_C = \
	main.c \

SRC_TEST_C = \
	test_main.c \

SRC_S = \

OBJ =
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

OBJ_TEST =
OBJ_TEST += $(PY_O)
OBJ_TEST += $(addprefix $(BUILD)/, $(SRC_TEST_C:.c=.o))
OBJ_TEST += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ_TEST += $(BUILD)/tinytest.o

# we can't use LTO for nlrthumb.c because it contains inline assembler
$(PY_BUILD)/nlrthumb.o: CFLAGS += -fno-lto

all: run

run: $(BUILD)/firmware.elf
	qemu-system-arm -machine integratorcp -cpu cortex-m3 -nographic -monitor null -serial null -semihosting -kernel $(BUILD)/firmware.elf

test: $(BUILD)/firmware-test.elf
	qemu-system-arm -machine integratorcp -cpu cortex-m3 -nographic -monitor null -serial null -semihosting -kernel $(BUILD)/firmware-test.elf > $(BUILD)/console.out
	$(Q)tail -n2 $(BUILD)/console.out
	$(Q)tail -n1 $(BUILD)/console.out | grep -q "status: 0"

.PHONY: $(BUILD)/genhdr/tests.h

$(BUILD)/test_main.o: $(BUILD)/genhdr/tests.h
$(BUILD)/genhdr/tests.h:
	$(Q)echo "Generating $@";(cd ../tests; ../tools/tinytest-codegen.py) > $@

$(BUILD)/tinytest.o:
	$(Q)$(CC) $(CFLAGS) -DNO_FORKING -o $@ -c ../tools/tinytest/tinytest.c

## `$(LD)` doesn't seem to like `--specs` for some reason, but we can just use `$(CC)` here.
$(BUILD)/firmware.elf: $(OBJ)
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware-test.elf: $(OBJ_TEST)
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

include ../py/mkrules.mk
