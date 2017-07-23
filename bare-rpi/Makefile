include ../py/mkenv.mk

SRC=source/

# qstr definitions (must come before including py.mk)
QSTR_DEFS = $(SRC)qstrdefsport.h
BOARD?=qemu
PIOS_BOARD ?= boards/$(BOARD)

# include py core make definitions
include ../py/py.mk
include lib/PiOS/$(PIOS_BOARD)/board.mk

CROSS_COMPILE = arm-none-eabi-

OBJCPY=$(CROSS_COMPILE)objcopy

INC += -I.
INC += -I..
INC += -I$(BUILD)

SHELL := /bin/bash
## find the version number of gcc (I think this is a bit too complicated :/)
CCVERSION:=$(shell $(CC) --version | sed 's/[ ]/\n/g' | grep "\." | xargs | awk '{ print $$1 }')
ARMGCCLIBPATH=/usr/lib/gcc/arm-none-eabi/$(CCVERSION)#/hard

#CFLAGS_CORTEX_M4 = -marm -mtune=cortex-m4 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS_BUILD = -mcpu=$(CPU) -marm -mfpu=vfp -mfloat-abi=hard
CCPU = $(CFLAGS_BUILD)
CFLAGS += $(INC) -I$(LIB)PiOS -Wall -ansi -std=gnu99 -nostdlib $(CFLAGS_BUILD) $(COPT) -I$(LIB)PiOS/include/ -Os 

LIST=kernel.list

LDFLAGS = -nostdlib -Map=$@.map --no-undefined -T $(LDSCRIPT) #--cref -T kernel.ld
LIBS = -L. -lpios -L$(ARMGCCLIBPATH) -lgcc #-lc -lm

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
	$(SRC)irq.c \
	$(SRC)module_rawptr.c \
	$(SRC)module_cdebug.c \
	$(SRC)module_cio.c \
	
SRC_PY = \
    $(SRC)interactive.c 


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

LIBPIOS=./libpios.a
LIB=lib/

CLEAN_EXTRA = libpios.a
dump: $(BUILD)/firmware.elf
	$(CROSS_COMPILE)objdump -d $(BUILD)/firmware.elf > $(LIST)

pios: libpios.a
libpios.a:
	cd $(LIB)PiOS/ &&\
        cp $(PIOS_BOARD)/board.mk . &&\
        cp $(PIOS_BOARD)/pios_port_config.h . &&\
		$(MAKE) lib &&\
		cd ../../ &&\
		cp $(LIB)PiOS/lib/libpios.a .

%.c: %.py
	xxd -i $< > $@

$(BUILD):
	mkdir $(BUILD)

$(BUILD)/firmware.elf: libpios.a $(OBJ) 
	$(ECHO) "LINK $@"
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(OBJCPY) $(BUILD)/firmware.elf -O binary $(BUILD)/kernel.img
	$(Q)$(SIZE) $@
	
include ../py/mkrules.mk
