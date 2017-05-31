include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

OBJCPY=$(CROSS_COMPILE)objcopy

INC += -I.
INC += -I..
INC += -I$(BUILD)

SHELL := /bin/bash
## find the version number of gcc (I think this is a bit too complicated :/)
CCVERSION:=$(shell $(CC) --version | sed 's/[ ]/\n/g' | grep "\." | xargs | awk '{ print $$1 }')
ARMGCCLIBPATH=/usr/lib/gcc/arm-none-eabi/$(CCVERSION)/hard

RPI = rpi

#CFLAGS_CORTEX_M4 = -marm -mtune=cortex-m4 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS_RPI = -mcpu=arm1176jzf-s -marm -mfpu=vfp -mfloat-abi=hard
CCPU = $(CFLAGS_RPI)
CFLAGS = $(INC) -Wall -ansi -std=gnu99 -nostdlib $(CFLAGS_RPI) $(COPT) -I$(LIB)PiOS/source/
CFLAGS += -Os 

LIST=kernel.list

LDFLAGS = -nostdlib -Map=$@.map --no-undefined -T kernel.ld #--cref -T kernel.ld
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
	startup_rpi.s 
#	gchelper.s

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

all: $(BUILD)/firmware.elf

LIBC=./libc.a
LIBM=./libm.a
LIBPIOS=./libpios.a
NEWLIB_PATH=newlib-cygwin
LIB=lib/
PLAT=PLATFORM_RPI

# RPI
rpi: PLAT=PLATFORM_RPI 
rpi: $(BUILD) $(BUILD)/firmware.elf

# RPI2
rpi2: PLAT=PLATFORM_RPI2 
rpi2: $(BUILD) $(BUILD)/firmware.elf

# RPI B+
rpibp: PLAT=PLATFORM_RPIBP 
rpibp: $(BUILD) $(BUILD)/firmware.elf

CFLAGS+= -D$(PLAT)

dump: $(BUILD)/firmware.elf
	$(CROSS_COMPILE)objdump -d $(BUILD)/firmware.elf > $(LIST)

newlib: $(BUILD) $(LIBC)
build/arm-none-eabi/newlib/libc.a: 
	cd build && \
		CFLAGS_FOR_TARGET="$(CPUINFO) $(CCPU)"\
		../$(LIB)$(NEWLIB_PATH)/configure --target=arm-none-eabi --enable-newlib-hw-fp --with-float=hard --with-cpu=arm1176jzf-s --with-fpu=vfp --disable-multilib --disable-shared --enable-target-optspace  --disable-newlib-supplied-syscalls && \
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
		$(MAKE) lib$(RPI) &&\
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
