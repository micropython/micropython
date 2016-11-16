# Select the board to build for: if not given on the command line,
# then default to pca10040.
BOARD ?= pca10040
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif

# If SoftDevice is selected, try to use that one.
SD ?= none
SD_LOWER = $(shell echo $(SD) | tr '[:upper:]' '[:lower:]')

# TODO: Verify that it is a valid target.


ifeq ($(SD), none)
	# If the build directory is not given, make it reflect the board name.
	BUILD ?= build-$(BOARD)
	include ../py/mkenv.mk
	include boards/$(BOARD)/mpconfigboard.mk
else
	# If the build directory is not given, make it reflect the board name.
	BUILD ?= build-$(BOARD)-$(SD_LOWER)
	include ../py/mkenv.mk
	include boards/$(BOARD)/mpconfigboard_$(SD_LOWER).mk
endif

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

MCU_VARIANT_LOWER = $(shell echo $(MCU_VARIANT) | tr '[:upper:]' '[:lower:]')

INC =  -I.
INC += -I..
INC += -I$(BUILD)
INC += -I./device
INC += -I./../lib/cmsis/inc
INC += -I./device
INC += -I./device/$(MCU_VARIANT_LOWER)
INC += -I./hal
INC += -I./hal/$(MCU_VARIANT_LOWER)
INC += -I./drivers
INC += -I../lib/mp-readline

NRF_DEFINES = -D$(MCU_VARIANT)
NRF_DEFINES += -DCONFIG_GPIO_AS_PINRESET

CFLAGS_CORTEX_M = -mthumb -mabi=aapcs -fsingle-precision-constant -Wdouble-promotion

CFLAGS_MCU_m4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS_MCU_m0 = $(CFLAGS_CORTEX_M) --short-enums -mtune=cortex-m0 -mcpu=cortex-m0 -mfloat-abi=soft -fno-builtin

CFLAGS += $(CFLAGS_MCU_$(MCU_SERIES)) 
CFLAGS += $(INC) -Wall -Werror -ansi -std=gnu99 -nostdlib $(COPT) $(NRF_DEFINES)
CFLAGS += -Iboards/$(BOARD)

LDFLAGS = $(CFLAGS)
LDFLAGS += -Xlinker -Map=$(@:.elf=.map)
LDFLAGS += -mthumb -mabi=aapcs -T $(LD_FILE)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
LDFLAGS += -O0
else
CFLAGS += -Os -DNDEBUG
LDFLAGS += -Os
endif

LIBS =  

SRC_LIB = $(addprefix lib/,\
	libc/string0.c \
	mp-readline/readline.c \
	utils/pyexec.c \
        utils/pyhelp.c \
	)

SRC_C += \
	main.c \
	device/$(MCU_VARIANT_LOWER)/system_$(MCU_VARIANT_LOWER).c \
	modpyb.c \
	led.c \
	mphalport.c \
	uart.c \
	help.c \
	gccollect.c \

SRC_S = \
	device/$(MCU_VARIANT_LOWER)/startup_$(MCU_VARIANT_LOWER).s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))


.phony: all flash

all: $(BUILD)/firmware.elf binary hex

flash: $(BUILD)/firmware.elf
	nrfjprog --program $(BUILD)/firmware.hex --sectorerase -f $(MCU_VARIANT_LOWER)
	nrfjprog --pinreset -f $(MCU_VARIANT_LOWER)

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

SRC_QSTR += $(SRC_C) $(SRC_MOD) $(SRC_LIB)

include ../py/mkrules.mk
include mkrules.mk
