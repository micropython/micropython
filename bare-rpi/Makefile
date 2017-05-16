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

#CFLAGS_CORTEX_M4 = -marm -mtune=cortex-m4 -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS_RPI = -mcpu=arm1176jzf-s -marm -mfpu=vfp -mfloat-abi=hard
CCPU = $(CFLAGS_RPI)
CFLAGS = $(INC) -Wall -ansi -std=gnu99 -nostdlib $(CFLAGS_RPI) $(COPT)
CFLAGS += -Os 

#Debugging/Optimization
#ifeq ($(DEBUG), 1)
#
#else
#CFLAGS += -Os -DNDEBUG
#endif

LDFLAGS = -nostdlib -Map=$@.map --no-undefined -T kernel.ld #--cref -T kernel.ld
LIBS = -L. -lc -lm -L/usr/lib/gcc/arm-none-eabi/6.3.1/hard -lgcc

SRC_C = \
	main.c \
	irq.c \
	uart.c \
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
NEWLIB_PATH=newlib-cygwin
LIB=./

newlib: $(BUILD) $(LIBC)
build/arm-none-eabi/newlib/libc.a: 
	cd $(LIB)build && \
		CFLAGS_FOR_TARGET="$(CPUINFO) $(CCPU)"\
		../$(NEWLIB_PATH)/configure --target=arm-none-eabi --enable-newlib-hw-fp --with-float=hard --with-cpu=arm1176jzf-s --with-fpu=vfp --disable-multilib --disable-shared --enable-target-optspace  --disable-newlib-supplied-syscalls && \
		$(MAKE) 

$(LIBC): $(LIB)build/arm-none-eabi/newlib/libc.a
	cp $(LIB)build/arm-none-eabi/newlib/libc.a $(LIBC)
	cp $(LIB)build/arm-none-eabi/newlib/libm.a $(LIBM)

%.c: %.py
	xxd -i $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(OBJCPY) $(BUILD)/firmware.elf -O binary $(BUILD)/kernel.img
	$(Q)$(SIZE) $@

include ../py/mkrules.mk
