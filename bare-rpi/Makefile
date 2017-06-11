include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h
PIOS_BOARD ?= boards/qemu

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
ARMGCCLIBPATH=/usr/lib/gcc/arm-none-eabi/$(CCVERSION)/fpu

#CFLAGS_CORTEX_M4 = -marm -mtune=cortex-m4 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS_BUILD = -mcpu=$(CPU) -marm -mfpu=vfp -mfloat-abi=hard
CCPU = $(CFLAGS_BUILD)
CFLAGS = $(INC) -I$(LIB)PiOS -Wall -ansi -std=gnu99 -nostdlib $(CFLAGS_BUILD) $(COPT) -I$(LIB)PiOS/include/
CFLAGS += -Os 

LIST=kernel.list

LDFLAGS = -nostdlib -Map=$@.map --no-undefined -T $(LDSCRIPT) #--cref -T kernel.ld
LIBS = -L. -lc -lm -lpios -L$(ARMGCCLIBPATH) -lgcc

SRC_C = \
	main.c \
	irq.c \
	syscalls.c 	\
	module_rawptr.c \
	module_cdebug.c \
	module_cio.c \
	interactive.c 


SRC_S = \
	qemu_start.s 
#	gchelper.s

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

all: $(BUILD)/firmware.elf

LIBC=./libc.a
LIBM=./libm.a
LIBPIOS=./libpios.a
NEWLIB_PATH=newlib-cygwin
LIB=lib/


NEWLIB_CFLAGS?=$(CPUINFO) $(CCPU)
NEWLIB_OPTS?=--target=arm-none-eabi --enable-newlib-hw-fp --with-float=hard --with-cpu=arm1176jzf-s --with-fpu=vfp --disable-multilib --disable-shared --enable-target-optspace  --disable-newlib-supplied-syscalls

dump: $(BUILD)/firmware.elf
	$(CROSS_COMPILE)objdump -d $(BUILD)/firmware.elf > $(LIST)

newlib: $(BUILD) $(LIBC)
build/arm-none-eabi/newlib/libc.a: 
	cd build && \
		CFLAGS_FOR_TARGET="$(NEWLIB_CFLAGS)"\
		../$(LIB)$(NEWLIB_PATH)/configure $(NEWLIB_OPTS) && \
		$(MAKE) 
		
newlib_clean:
	rm -r build
	mkdir build
	rm $(LIBC) $(LIBM)
	
$(LIBC): build/arm-none-eabi/newlib/libc.a
	cp build/arm-none-eabi/newlib/libc.a $(LIBC)
	cp build/arm-none-eabi/newlib/libm.a $(LIBM)

pios: 
	cd $(LIB)PiOS/ &&\
        cp $(PIOS_BOARD)/board.mk . &&\
        cp $(PIOS_BOARD)/pios_port_config.h . &&\
		$(MAKE) lib &&\
		cd ../../ &&\
		cp $(LIB)PiOS/lib/libpios.a .

%.c: %.py
	xxd -i $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(OBJCPY) $(BUILD)/firmware.elf -O binary $(BUILD)/kernel.img
	$(Q)$(SIZE) $@

include ../py/mkrules.mk
