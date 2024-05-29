BOARD ?=

# Check if the ModusToolbox setup has been initialized
# If that is the case, get active board
MTB_LIB_DIR = mtb-libs
MTB_LIBS_APP_INFO = $(MTB_LIB_DIR)/build/get_app_info.txt
ifneq ($(wildcard $(MTB_LIBS_APP_INFO)),)
	ACTIVE_BOARD = $(shell egrep '^ *MTB_TARGET' $(MTB_LIBS_APP_INFO) | sed 's/^.*= *//g' | sed 's/APP_//')
endif

# Get active board from mtb-lib previous runs
# The board is set only after make mtb_init 
# has been run. 
ifeq ($(BOARD),)
    ifeq ($(ACTIVE_BOARD),)
        $(error ModusToolbox not initialized. Run "make mtb_init BOARD=<target-board>" to configure the environment. )
    else
        BOARD = $(ACTIVE_BOARD)
    endif
endif

BOARD_DIR  = boards/$(BOARD)

$(info PSoC6 board        : $(BOARD))

ifeq ($(wildcard $(BOARD_DIR)/.),)
  $(error Invalid BOARD specified)
endif

# Files that are generated and needed before the QSTR build.
QSTR_GENERATED_HEADERS = build/pins_qstr.h
# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(QSTR_GENERATED_HEADERS)
QSTR_GLOBAL_DEPENDENCIES += $(BOARD_DIR)/mpconfigboard.h $(QSTR_GENERATED_HEADERS)

MICROPY_FROZEN_MANIFEST      ?= $(BOARD_DIR)/manifest.py
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


INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

ifeq ($(MICROPY_PSOC6_LWIP),1)
	INC += -Ilwip_inc
endif

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
BOARD_PINS := pins.csv 
BOARD_AF_PINS := pins_af.csv

GENERATED_PINS = $(GEN_PINS_SRC) $(GEN_PINS_HDR) $(GEN_PINS_QSTR)

$(GENERATED_PINS): 
	@echo "Generating $@"
	$(PYTHON) boards/make-pins-csv.py --gen-pin-for $(PIN_PACKAGE_FILE) --save-pins-csv-at $(BOARD_PINS) --save-pins-af-csv-at $(BOARD_AF_PINS)
	$(MKDIR) -p $(BUILD)/genhdr
	$(PYTHON) boards/make-pins.py --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) > $(GEN_PINS_SRC)
	rm -rf $(BOARD_PINS) $(BOARD_AF_PINS)


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
  CFLAGS += -DMICROPY_PY_LWIP=1
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
	\
	modpsoc6.c \
	psoc6_fatfs.c \
	psoc6_flash.c 

ifeq ($(MICROPY_PY_EXT_FLASH),1)
	CFLAGS += -DMICROPY_ENABLE_EXT_QSPI_FLASH=1
	MOD_SRC_C += psoc6_qspi_flash.c 
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

$(BUILD)/firmware.elf: $(OBJ) $(MPY_MTB_LIBRARIES) $(LIBS)
	$(info )
	$(info Linking $@     $^  $(LIBS) ...)
	$(Q) $(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(info Linking $@ done.)
	$(Q) $(SIZE) $@ -A
	$(info )


$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(Q) $(OBJCOPY) -O ihex $^ $@


# include adapter makefile
include mtb-libs/makefile_mtb.mk

MPY_CROSS_FLAGS += -march=armv7m

build: mtb_get_build_flags $(GENERATED_PINS) $(BUILD)/firmware.hex

all: build

clean: mtb_clean

rebuild: clean mtb_clean all

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

help:
	@:
	$(info )
	$(info ----------)
	$(info Basic flow)
	$(info ----------)
	$(info )
	$(info Prior working on a specific board, it is required to initialise ONCE the)
	$(info ModusToolbox setup and retrieve all necessary assets for the specified)
	$(info board:)
	$(info )
	$(info $$ make mtb_init BOARD=<board>)
	$(info )
	$(info Then repeatedly build the firmware:)
	$(info )
	$(info  $$ make )
	$(info )
	$(info and flash the firmware on the device with:)
	$(info )
	$(info  $$ make program)
	$(info )
	$(info 'make program' will also build the .hex file if any changes occurred in the code.)
	$(info )
	$(info -------------)
	$(info Build targets)
	$(info -------------)
	$(info )
	$(info The default 'make' target is 'build' when called without arguments.)
	$(info - build    Compile the MicroPython program binary file. )
	$(info - clean    Remove the '/build' folders with all object files and intermediate build support files.)
	$(info - rebuild  Clean before build.)
	$(info )
	$(info ---------------)
	$(info Program targets)
	$(info ---------------)
	$(info )
	$(info - program        Flash the board device with the built micropython firmware. )					
	$(info - program_multi  Flash multiple board deviced. )
	$(info Options: )
	$(info - EXT_HEX_FILE   An external .hex file can be provided to the program targets, instead of building from the sources.) 
	$(info )
	$(info ------------)
	$(info Test targets)
	$(info ------------)
	$(info )
	$(info - test           Run the on-target test in tests/psoc6 folder. Uses /dev/ttyACM0.)
	$(info ..               Optionally, pass TESTS variable for change the tests set. )
	$(info - test_multi     Run multi-instance tests on-target test in tests/psoc6/multi folder.)
	$(info ..               Uses /dev/ttyACM0 and /dev/ttyACM1.)
	$(info ..               Optionally, pass MULTI_TESTS variable for change the tests set. )
	$(info )
	$(info --------------------)
	$(info ModusToolbox targets)
	$(info --------------------)
	$(info )
	$(info - mtb_init BOARD=<board>  Add the board support package required firmware assets, set the bsp  )
	$(info ..                        as active in the ModusToolbox project, and retrieves all required additional )
	$(info ..                        middleware assets for MicroPython. )
	$(info - mtb_deinit              Clean all ModusToolbox shared and board support package dependencies assets.)
	$(info - mtb_add_bsp             Add a board support package. Only the ones integrated in MicroPython are supported. )
	$(info - mtb_set_bsp             Set the board as active in the ModusToolbox project. ) 


.PHONY: build test test_multi program_multi help

# include py core make definitions
include $(TOP)/py/mkrules.mk