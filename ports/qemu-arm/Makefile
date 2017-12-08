include ../../py/mkenv.mk
-include mpconfigport.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE = arm-none-eabi-

TINYTEST = $(TOP)/lib/tinytest

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(TINYTEST)

CFLAGS_CORTEX_M3 = -mthumb -mcpu=cortex-m3 -mfloat-abi=soft
CFLAGS = $(INC) -Wall -Wpointer-arith -Werror -std=gnu99 $(CFLAGS_CORTEX_M3) $(COPT) \
	 -ffunction-sections -fdata-sections

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

SRC_COMMON_C = \
	moduos.c \
	modmachine.c \

SRC_RUN_C = \
	main.c \

SRC_TEST_C = \
	test_main.c \

LIB_SRC_C = $(addprefix lib/,\
	libm/math.c \
	libm/fmodf.c \
	libm/nearbyintf.c \
	libm/ef_sqrt.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/asinfacosf.c \
	libm/atanf.c \
	libm/atan2f.c \
	utils/sys_stdio_mphal.c \
	)

OBJ_COMMON =
OBJ_COMMON += $(PY_O)
OBJ_COMMON += $(addprefix $(BUILD)/, $(SRC_COMMON_C:.c=.o))
OBJ_COMMON += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))

OBJ_RUN =
OBJ_RUN += $(addprefix $(BUILD)/, $(SRC_RUN_C:.c=.o))

OBJ_TEST =
OBJ_TEST += $(addprefix $(BUILD)/, $(SRC_TEST_C:.c=.o))
OBJ_TEST += $(BUILD)/tinytest.o

# All object files, needed to get dependencies correct
OBJ = $(OBJ_COMMON) $(OBJ_RUN) $(OBJ_TEST)

# List of sources for qstr extraction
SRC_QSTR += $(SRC_COMMON_C) $(SRC_RUN_C) $(LIB_SRC_C)

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
	$(Q)echo "Generating $@";(cd $(TOP)/tests; ../tools/tinytest-codegen.py) > $@

$(BUILD)/tinytest.o:
	$(Q)$(CC) $(CFLAGS) -DNO_FORKING -o $@ -c $(TINYTEST)/tinytest.c

## `$(LD)` doesn't seem to like `--specs` for some reason, but we can just use `$(CC)` here.
$(BUILD)/firmware.elf: $(OBJ_COMMON) $(OBJ_RUN)
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware-test.elf: $(OBJ_COMMON) $(OBJ_TEST)
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

include $(TOP)/py/mkrules.mk
