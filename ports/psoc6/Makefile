BOARD     ?= CY8CPROTO-062-4343W
BOARD_DIR  = boards/$(BOARD)


ifeq ($(wildcard $(BOARD_DIR)/.),)
  $(error Invalid BOARD specified)
endif


# get path to this file
MPY_PATH_TO_MAIN_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
MPY_DIR_OF_MAIN_MAKEFILE  := $(dir $(MPY_PATH_TO_MAIN_MAKEFILE))


# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES += $(BOARD_DIR)/mpconfigboard.h

MICROPY_FROZEN_MANIFEST      ?= boards/manifest.py
MICROPY_USER_FROZEN_MANIFEST ?= $(MICROPY_FROZEN_MANIFEST)
FROZEN_MANIFEST              ?= $(MICROPY_FROZEN_MANIFEST)


ifneq ($(FROZEN_MANIFEST),)
  CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
  CFLAGS += -DMICROPY_MODULE_FROZEN_MPY=1
  CFLAGS += -DMICROPY_MODULE_FROZEN_STR=1
endif


CROSS_COMPILE ?= arm-none-eabi-
CONFIG        ?= Debug

# include py core make definitions
include ../../py/mkenv.mk
include mpconfigport.mk
include $(BOARD_DIR)/mpconfigboard.mk
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

# get path to this file
MPY_MAIN_BUILD_DIR := $(dir $(MPY_DIR_OF_MAIN_MAKEFILE))/$(BUILD)

$(info MPY_PATH_TO_MAIN_MAKEFILE        : $(MPY_PATH_TO_MAIN_MAKEFILE))
$(info MPY_DIR_OF_MAIN_MAKEFILE         : $(MPY_DIR_OF_MAIN_MAKEFILE))
$(info MPY_MAIN_BUILD_DIR               : $(MPY_MAIN_BUILD_DIR))
$(info CONFIG                           : $(CONFIG))


INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

ifeq ($(MICROPY_PSOC6_LWIP),1)
	INC += -Ilwip_inc
endif

### for testing purposes only
# USERMOD_DIR = $(TOP)/examples/usercmodule/cexample
# include $(TOP)/examples/usercmodule/cexample/micropython.mk

LD               = arm-none-eabi-gcc
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mcpu=cortex-m4 -msoft-float -fsingle-precision-constant -Wdouble-promotion -Wfloat-conversion -UMICROPY_USE_INTERNAL_PRINTF

# std=c11 instead of std=c99 : provides "static_assert" (not available in c99) 
# -D_XOPEN_SOURCE=700 : makes sure the setenv/unsetenv headers are included
CFLAGS  += $(INC) -Wall -Werror -std=c11 $(CFLAGS_CORTEX_M4) $(COPT) -D_XOPEN_SOURCE=700
CFLAGS  += -Wno-error=double-promotion -Wno-error=overflow -Wno-error=analyzer-null-dereference -Wno-error=unused-local-typedefs -Wno-error=unused-function -Wno-error=maybe-uninitialized

ifeq ($(MICROPY_PSOC6_SSL_MBEDTLS),1)
  INC += -I$(TOP)/extmod/mbedtls
  CFLAGS += -DMBEDTLS_CONFIG_FILE=\"mbedtls/mbedtls_config.h\" 
  CFLAGS += -DMICROPY_SSL_MBEDTLS=1
endif

LDFLAGS += -Wl,--cref -Wl,--gc-sections


# Tune for Debugging or Optimization
ifeq ($(CONFIG), Debug)
  CFLAGS         += -O0 -ggdb
  MPY_MTB_CONFIG  = Debug
  MICROPY_ROM_TEXT_COMPRESSION ?= 0
else
  CFLAGS         += -O3 -Os -DNDEBUG
  CFLAGS         += -fdata-sections -ffunction-sections
  MPY_MTB_CONFIG  = Release
  MICROPY_ROM_TEXT_COMPRESSION ?= 1
endif


$(info Compiling in $(CONFIG) mode !)


# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))
CXXFLAGS += $(CXXFLAGS_MOD)

LDFLAGS += $(LDFLAGS_MOD)

LIBS += 

SHARED_SRC_C += $(addprefix shared/,\
	readline/readline.c \
	\
	runtime/gchelper_native.c \
	runtime/interrupt_char.c \
	runtime/pyexec.c \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	timeutils/timeutils.c \
	)

ifeq ($(MICROPY_PSOC6_LWIP),1)
  SHARED_SRC_C += $(addprefix shared/,\
	netutils/dhcpserver.c \
	netutils/netutils.c \
	netutils/trace.c \
  )
  CFLAGS += -DMICROPY_PY_LWIP=1
endif

DRIVERS_SRC_C += $(addprefix drivers/,\
	machine/psoc6_gpio.c \
	machine/psoc6_i2c.c \
	machine/psoc6_system.c  \
	)

MOD_SRC_C += $(addprefix modules/,\
	gc/modgc.c \
	\
	machine/modmachine.c \
	machine/machine_i2c.c \
	machine/machine_pin.c \
	machine/machine_rtc.c \
	machine/pins.c \
	\
	psoc6/modpsoc6.c \
	psoc6/psoc6_fatfs.c \
	psoc6/psoc6_flash.c \
	psoc6/psoc6_qspi_flash.c \
	)

ifeq ($(MICROPY_PY_NETWORK_IFX_WCM),1)
CFLAGS += -DMICROPY_PY_NETWORK_IFX_WCM=1 -Wno-stringop-truncation
	MOD_SRC_C += $(addprefix modules/network/,\
		network_ifx_wcm.c \
		)
	INC += -Imodules/network
endif

SRC_C = help.c \
		main.c \
		mphalport.c \
		$(BUILD)/frozen_content.c 

SRC_ASM += shared/runtime/gchelper_thumb1.s

SRC_QSTR += $(SHARED_SRC_C) $(MOD_SRC_C)

OBJ += $(PY_O) 
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o)) 
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o)) 
OBJ += $(addprefix $(BUILD)/, $(MOD_SRC_C:.c=.o)) 
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) 
OBJ += $(addprefix $(BUILD)/, $(SRC_ASM:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))

# switch for debug mode, also added to mpconfigport.h
# TODO: keep the more suitable one, delete the other
MP_LOGGER_DEBUG ?= 0

ifeq ($(MP_LOGGER_DEBUG), 1)
   CFLAGS += -DMICROPY_LOGGER_DEBUG=1
endif


$(MPY_MAIN_BUILD_DIR)/firmware.elf: $(OBJ) $(MPY_MTB_LIBRARIES) $(LIBS)
	$(info )
	$(info Linking $@     $^  $(LIBS) ...)
	$(Q) $(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(info Linking $@ done.)
	$(Q) $(SIZE) $@ -A
	$(info )


$(MPY_MAIN_BUILD_DIR)/firmware.hex: $(MPY_MAIN_BUILD_DIR)/firmware.elf
	$(Q) $(OBJCOPY) -O ihex $^ $@


# include adapter makefile
include $(BOARD_DIR)/makefile_mtb.mk

# include py core make definitions
include $(TOP)/py/mkrules.mk

MPY_CROSS_FLAGS += -march=armv7m


build: mpy_define_mtb_vars $(MPY_PATH_TO_MAIN_MAKEFILE) $(MPY_MAIN_BUILD_DIR)/firmware.hex

all: build

rebuild: clean mpy_mtb_clean all


ifndef SINGLE_TEST
TESTS=-d psoc6
else
TESTS=$(SINGLE_TEST)
endif

test: 
	@:
	$(info )
	$(info Running PSoC6 tests)
	$(Q) cd ../../tests ; ./run-tests.py --target psoc6 --device /dev/ttyACM0 $(TESTS)

test_multi: 
	@:
	$(info )
	$(info Running PSoC6 multi tests)
	$(Q) cd ../../tests ; ./run-multitests.py -i pyb:/dev/ttyACM0 -i pyb:/dev/ttyACM1 $(MULTI_TEST)

program: mpy_program

program_multi: mpy_program_multi

.PHONY: build test test_multi program_multi 