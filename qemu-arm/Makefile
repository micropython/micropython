include ../py/mkenv.mk
-include mpconfigport.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

INC =  -I.
INC += -I..
INC += -I$(PY_SRC)
INC += -I$(BUILD)

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

ifeq ($(RUN_TESTS), 1)
SRC_C = \
	test_main.c \

else
SRC_C = \
	main.c \

endif

SRC_S = \

OBJ =
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

ifeq ($(RUN_TESTS), 1)
INC += -I../tools/tinytest/
OBJ += $(BUILD)/tinytest.o
endif

all: run

run: $(BUILD)/flash.elf
	qemu-system-arm -cpu cortex-m3 -nographic -monitor null -serial null -semihosting -kernel $(BUILD)/flash.elf

.PHONY: $(BUILD)/genhdr/tests.h

$(BUILD)/test_main.o: $(BUILD)/genhdr/tests.h
$(BUILD)/genhdr/tests.h:
	$(Q)echo "Generating $@";(cd ../tests; ../tools/tinytest-codegen.py) > $@

$(BUILD)/tinytest.o:
	$(Q)$(CC) $(CFLAGS) -DNO_FORKING -o $@ -c ../tools/tinytest/tinytest.c

## `$(LD)` doesn't seem to like `--specs` for some reason, but we can just use `$(CC)` here.
$(BUILD)/flash.elf: $(OBJ)
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

include ../py/mkrules.mk
