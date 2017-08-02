include ../py/mkenv.mk

SRC=source/

# qstr definitions (must come before including py.mk)
QSTR_DEFS = $(SRC)qstrdefsport.h
BOARD?=rpi
PIOS_BOARD ?= boards/$(BOARD)

# include py core make definitions
include ../py/py.mk
include $(PIOS_BOARD)/board.mk

LDSCRIPT=$(PIOS_BOARD)/kernel.ld
CROSS_COMPILE = $(PLATFORM)-

OBJCPY=$(CROSS_COMPILE)objcopy

INC += -I.
INC += -I..
INC += -I$(BUILD)
INC += -I$(PIOS_BOARD)

SHELL := /bin/bash
CCVERSION:=$(shell $(CC) --version | sed 's/[ ]/\n/g' | grep "\." | xargs | awk '{ print $$1 }')
ARMGCCLIBPATH=/usr/lib/gcc/$(PLATFORM)/$(CCVERSION)#/hard

CFLAGS_BUILD = -mcpu=$(CPU) -marm -mfpu=vfp -mfloat-abi=hard
CCPU = $(CFLAGS_BUILD)
CFLAGS += $(INC) -Wall -ansi -std=gnu99 -nostdlib $(CFLAGS_BUILD) $(COPT) -Os 

LIST=kernel.list

LDFLAGS = -nostdlib -Map=$@.map --no-undefined -T $(LDSCRIPT) #--cref -T kernel.ld
LIBS = -L. -L$(ARMGCCLIBPATH) -lgcc #-lc -lm

SRC_LIB = $(addprefix lib/,\
	libc/string0.c \
	libm/math.c \
	libm/thumb_vfp_sqrtf.c \
	libm/asinfacosf.c \
	libm/atanf.c \
	libm/atan2f.c \
	libm/fmodf.c \
	libm/roundf.c \
	libm/log1pf.c \
	libm/acoshf.c \
	libm/asinhf.c \
	libm/atanhf.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/erf_lgamma.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/sf_erf.c \
	libm/wf_lgamma.c \
	libm/wf_tgamma.c)

SRC_C = \
	$(SRC)main.c \
    $(SRC)qemu_uart.c \
	$(SRC)uart.c \
	$(SRC)gpio.c
	
SRC_PY = 

SRC_S = \
	$(SRC)$(BOARD)_start.s 
#	gchelper.s

OBJ = $(PY_O) 
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o) $(SRC_LIB:.c=.o) $(SRC_PY:.c=.o))

all: 
#	$(MAKE) debug
#	$(MAKE) clean
	$(MAKE) release

release: $(BUILD)/firmware.elf

debug: CFLAGS += -ggdb
debug: release
	mv $(BUILD)/firmware.elf firmware_debug.elf

dump: $(BUILD)/firmware.elf
	$(CROSS_COMPILE)objdump -d $(BUILD)/firmware.elf > $(LIST)

%.c: %.py
	xxd -i $< > $@

$(BUILD):
	mkdir $(BUILD)

$(BUILD)/firmware.elf: $(OBJ) 
	$(ECHO) "LINK $@"
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(OBJCPY) $(BUILD)/firmware.elf -O binary $(BUILD)/kernel.img
	$(Q)$(SIZE) $@
	
include ../py/mkrules.mk
