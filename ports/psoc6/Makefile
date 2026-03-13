MTB_LIBS_DIR = ../../lib/mtb-psoc6-libs
include mpconfigport.mk
-include $(MTB_LIBS_DIR)/mtb-bsp-setup.mk

# The only target allowed to run without BOARD defined is 'submodules' or 'help'
ifeq ($(BOARD),)
	ifneq ($(filter $(MAKECMDGOALS),submodules help),$(MAKECMDGOALS))
    	$(error No active board is set. Run "make BOARD=<target-board>" to initialize the ModusToolbox libraries and set the active board. )
	endif
endif

BUILD ?= build-$(BOARD)

# Files that are generated and needed before the QSTR build.
QSTR_GENERATED_HEADERS =$(BUILD)/pins_qstr.h
# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(QSTR_GENERATED_HEADERS)
QSTR_GLOBAL_DEPENDENCIES += $(BOARD_DIR)/mpconfigboard.h $(QSTR_GENERATED_HEADERS)

MICROPY_FROZEN_MANIFEST = $(BOARD_DIR)/manifest.py
FROZEN_MANIFEST         = $(MICROPY_FROZEN_MANIFEST)

ifneq ($(FROZEN_MANIFEST),)
  CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
  CFLAGS += -DMICROPY_MODULE_FROZEN_MPY=1
  CFLAGS += -DMICROPY_MODULE_FROZEN_STR=1
endif


CROSS_COMPILE ?= arm-none-eabi-
CONFIG        ?= Debug

# include py core make definitions
include ../../py/mkenv.mk
-include $(BOARD_DIR)/mpconfigboard.mk
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

GIT_SUBMODULES += lib/mtb-psoc6-libs lib/mpy-test-ext

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

ifeq ($(MICROPY_PSOC6_LWIP),1)
	INC += -Ilwip_inc
endif

LD               = arm-none-eabi-gcc
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mcpu=cortex-m4 -msoft-float -fsingle-precision-constant -Wdouble-promotion -Wfloat-conversion -UMICROPY_USE_INTERNAL_PRINTF -Wno-error=float-conversion

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
LDFLAGS += -Wl,-Map,$(BUILD)/firmware.map  -Wl,--start-group -Wl,--end-group -Wl,--print-memory-usage


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

#ToDo: Post adding af functionality, refactor to minimize dependent variables in py script if possible
GEN_PINS_SRC := $(BUILD)/pins_$(BOARD).c
HEADER_BUILD := $(BUILD)/genhdr
GEN_PINS_HDR := $(BUILD)/genhdr/pins.h
GEN_PINS_QSTR := $(BUILD)/pins_qstr.h

GENERATED_PINS = $(GEN_PINS_SRC) $(GEN_PINS_HDR) $(GEN_PINS_QSTR)

$(GENERATED_PINS): 
	@echo "Generating $@"
	$(MKDIR) -p $(BUILD)/genhdr
	$(PYTHON) boards/make-pins.py --gen-pin-for $(PIN_PACKAGE_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) > $(GEN_PINS_SRC)

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
	runtime/mpirq.c\
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
  MOD_SRC_C += modsocket.c 
endif

DRIVERS_SRC_C += $(addprefix drivers/,\
	bus/softspi.c \
	)

MOD_SRC_C += \
	modgc.c \
	\
	machine_i2c.c \
	machine_pin_phy.c \
	machine_pin.c \
	machine_rtc.c \
	machine_spi.c \
	machine_timer.c \
	machine_adc.c \
	machine_adcblock.c \
	machine_bitstream.c\
	machine_pdm_pcm.c\
	\
	modpsoc6.c \
	psoc6_fatfs.c \
	psoc6_flash.c 

ifeq ($(MICROPY_PY_EXT_FLASH),1)
	CFLAGS += -DMICROPY_ENABLE_EXT_QSPI_FLASH=1
	MOD_SRC_C += psoc6_qspi_flash.c 
endif

ifeq ($(MICROPY_PY_SD_CARD),1)
	CFLAGS += -DMICROPY_ENABLE_SD_CARD=1
	MOD_SRC_C += machine_sdcard.c 
endif

ifeq ($(MICROPY_PY_NETWORK_IFX_WCM),1)
CFLAGS += -DMICROPY_PY_NETWORK=1 -DMICROPY_PY_NETWORK_IFX_WCM=1 -Wno-stringop-truncation
	MOD_SRC_C += network_ifx_wcm.c 
endif

SRC_C = help.c \
		main.c \
		mphalport.c \
		frozen_content.c \
		pins_$(BOARD).c

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

-include $(TOP)/lib/mtb-psoc6-libs/mtb-makefile.mk

$(BUILD)/firmware.elf: $(OBJ) $(LIBS) 
	$(info Linking $@     $^  $(LIBS) ...)
	$(Q) $(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(info Linking $@ done.)
	$(Q) $(SIZE) $@ -A
	$(info )

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(Q) $(OBJCOPY) -O ihex $^ $@

mpy_build: $(BUILD)/firmware.hex

MPY_CROSS_FLAGS += -march=armv7m

all: mtb_init mtb_get_build_flags mpy_build

clean: mtb_clean

rebuild: clean all

qdeploy: mtb_program
deploy: all mtb_program

# When multiple types of boards are connected, a devs file needs to be provided.
# When working locally, if a "local-devs.yml" file is placed in "tools/psoc6"
# it will be used
ifneq ($(DEVS_FILE),)
MULTI_BOARD_DEVS_OPTS = -f name=$(BOARD) --devs-yml $(DEVS_FILE)
else 
DFLT_LOCAL_DEVS_FILE_NAME = local-devs.yml
LOCAL_DEVS_FILE=$(TOP)/tools/psoc6/$(DFLT_LOCAL_DEVS_FILE_NAME)
ifneq (,$(wildcard $(LOCAL_DEVS_FILE)))
MULTI_BOARD_DEVS_OPTS = -f name=$(BOARD) --devs-yml $(LOCAL_DEVS_FILE)
endif
endif

attached_devs:
	@:
	$(eval ATTACHED_TARGET_LIST = $(shell etdevs-query serial_number $(MULTI_BOARD_DEVS_OPTS)))
	$(eval ATTACHED_TARGETS_NUMBER = $(words $(ATTACHED_TARGET_LIST)))
	$(info Number of attached targets : $(ATTACHED_TARGETS_NUMBER))
	$(info List of attached targets : $(ATTACHED_TARGET_LIST))

qdeploy_multi: attached_devs
	$(foreach ATTACHED_TARGET, $(ATTACHED_TARGET_LIST), $(MAKE) qdeploy DEV_SERIAL_NUMBER=$(ATTACHED_TARGET);)

deploy_multi: all qdeploy_multi

TESTS ?=-d psoc6
DEV0 ?= /dev/ttyACM0
DEV1 ?= /dev/ttyACM1

test: 
	@:
	$(info )
	$(info Running PSoC6 tests)
	$(Q) cd ../../tests ; ./run-tests.py --target psoc6 --device $(DEV0)  $(TESTS)

MULTI_TESTS ?= $(shell cd ../../tests; find ./psoc6/multi/ -type f -name "*.py")

test_multi: 
	@:
	$(info )
	$(info Running PSoC6 multi tests)

	$(Q) cd ../../tests ; ./run-multitests.py -i pyb:$(DEV0) -i pyb:$(DEV1) $(MULTI_TESTS)

port_help:
	@:
	$(info )
	$(info Available commands:)
	$(info ) 
	$(info   make submodules                    Initialize port required submodules.)
	$(info   make BOARD=<board_name>            Build the project for the specified board.)
	$(info   ..                                 The board name needs to be specified only the first time.)
	$(info   ..                                 Then simply run "make" to build for the same board.)
	$(info   make rebuild                       Build the project after cleaning previous build.)
	$(info   make deploy                        Build and deploy the project to the specified board.)
	$(info   make qdeploy                       Deploy the project to the specified board without rebuilding.)
	$(info   make deploy_multi                  Deploy the project to multiple boards.)
	$(info   make qdeploy_multi                 Deploy the project to multiple boards without rebuilding.)
	$(info   make clean                         Clean the build files.)
	$(info   make test                          Run the on-target test in tests/psoc6 folder. Uses /dev/ttyACM0.)
	$(info   ..                                 Optionally, pass TESTS variable for change the tests set. )
	$(info   make test_multi                    Run multi-instance tests on-target test in tests/psoc6/multi folder.)
	$(info   ..                                 Uses /dev/ttyACM0 and /dev/ttyACM1.)
	$(info   ..                                 Optionally, pass MULTI_TESTS variable for change the tests set. )
	$(info   make help                          Show this help.)
	$(info )
	$(info Options: )
	$(info   EXT_HEX_FILE                       An external .hex file can be provided to the deploy targets, instead of building from the sources.) 
	$(info )

help: port_help mtb_bsp_help mtb_build_help
.DEFAULT_GOAL := all

.PHONY: all clean rebuild deploy qdeploy deploy_multi qdeploy_multi test test_multi help

# include py core make definitions
include $(TOP)/py/mkrules.mk