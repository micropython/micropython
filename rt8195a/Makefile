include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-
CHIP = ameba
SRC = src

INC += -I.
INC += -I..
INC += -I$(BUILD)

INC += -I$(SRC)/include
INC += -I$(SRC)/targets/cmsis
INC += -I$(SRC)/targets/cmsis/target_rtk/target_8195a
INC += -I$(SRC)/targets/hal/target_rtk/target_8195a
INC += -I$(SRC)/targets/hal/target_rtk/target_8195a/rtl8195a
INC += -I$(SRC)/sw/lib/sw_lib/mbed/hal
INC += -I$(SRC)/sw/lib/sw_lib/mbed/hal_ext
INC += -I$(SRC)/sw/lib/sw_lib/mbed/targets/cmsis/rtl8195a
INC += -I$(SRC)/sw/lib/sw_lib/mbed/targets/hal/rtl8195a
INC += -I$(SRC)/sw/os
INC += -I$(SRC)/sw/os/os_dep/include


#-------------------------------------------------------------------------------
# Linkers
#-------------------------------------------------------------------------------
CFLAGS_CORTEX_M3 += -mthumb
CFLAGS_CORTEX_M3 += -mtune=cortex-m3
CFLAGS_CORTEX_M3 += -mcpu=cortex-m3
#CFLAGS_CORTEX_M3 += -mabi=aapcs-linux
#CFLAGS_CORTEX_M3 += -mfpu=fpv4-sp-d16
#CFLAGS_CORTEX_M3 += -mfloat-abi=hard
CFLAGS_CORTEX_M3 += -fsingle-precision-constant
CFLAGS_CORTEX_M3 += -Wdouble-promotion

CFLAGS_RT8195 += -c
CFLAGS_RT8195 += -nostartfiles 
#CFLAGS_RT8195 += -MMD -MP
#CFLAGS_RT8195 += --save-temps

COPT += -g
COPT += -O2
COPT += -ansi
COPT += -std=gnu99
COPT += -nostdlib
COPT += --specs=nano.specs
COPT += --specs=nosys.specs

COPT += -Wall
#COPT += -Werror
#COPT += -Wundef
COPT += -Wpointer-arith
COPT += -Wstrict-prototypes
COPT += -Wno-write-strings

COPT += -fno-short-enums
COPT += -fno-common
COPT += -fmessage-length=0
COPT += -fno-exceptions
COPT += -ffunction-sections
COPT += -fdata-sections
COPT += -fomit-frame-pointer
COPT += -fno-short-enums

EXTRA_DEFINE += -D$(CHIP) 

CFLAGS = $(INC) $(CFLAGS_CORTEX_M3) $(COPT) $(CFLAGS_RT8195) $(EXTRA_DEFINE)

ASFLAGS = $(INC) -mcpu=cortex-m3 -mthumb -Wall -a -g

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

LDFLAGS += -nostdlib -L ./scripts -T ./scripts/rlx8195a.ld -Map=$@.map --cref
LDFLAGS += --warn-common --gc-sections --entry=Reset_Handler

ELF_LDLIBS += startup.o lib_platform.a #-lstdc++ -lsupc++ -lm -lc -lgcc -lnosys

LIBS = $(ELF_LDLIBS)

#-------------------------------------------------------------------------------
# C source files and objects
#-------------------------------------------------------------------------------

SRC_C +=$(wildcard $(SRC)/targets/hal/target_rtk/target_8195a/src/*.c)
SRC_C +=$(wildcard $(SRC)/targets/cmsis/target_rtk/target_8195a/*.c)
SRC_C +=$(wildcard $(SRC)/sw/lib/sw_lib/mbed/targets/hal/rtl8195a/*.c)
SRC_C +=$(wildcard $(SRC)/sw/os/os_dep/*.c)

SRC_C += \
	main.c \
	lib/libc/string0.c \
	#printf.c \
	string0.c \
	malloc0.c \
	gccollect.c \

SRC_S += $(wildcard $(SRC)/source/*.s)
SRC_S += \
#	startup_stm32f40xx.s \
	gchelper.s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------
all: $(BUILD)/firmware.elf

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ --start-group $^ --end-group $(LIBS)
	$(Q)$(SIZE) $@

deploy:
	@./makebin/makebin.sh

include ../py/mkrules.mk
