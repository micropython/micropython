################################################################################
# Initial setup of Makefile environment

BOARD ?= ALIF_ENSEMBLE
BOARD_DIR ?= boards/$(BOARD)

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

include ../../py/mkenv.mk
include mpconfigport.mk
include $(BOARD_DIR)/mpconfigboard.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS += qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

################################################################################
# Project specific settings and compiler/linker flags

MPY_CROSS_FLAGS += -march=armv7emdp

CROSS_COMPILE ?= arm-none-eabi-
ALIF_DFP_REL_TOP ?= lib/alif_ensemble-cmsis-dfp
ALIF_DFP_REL_HERE ?= $(TOP)/lib/alif_ensemble-cmsis-dfp
CMSIS_DIR ?= $(TOP)/lib/cmsis/inc

MCU_CORE ?= M55_HP
ALIF_CONFIG ?= mcu/$(MCU_CORE)_cfg.json
LD_FILE ?= mcu/ensemble.ld.S

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(BOARD_DIR)
INC += -I$(CMSIS_DIR)
INC += -I$(ALIF_DFP_REL_HERE)/drivers/include/
INC += -I$(ALIF_DFP_REL_HERE)/se_services/include
INC += -I$(ALIF_DFP_REL_HERE)/ospi_xip/source/ospi
INC += -I$(ALIF_DFP_REL_HERE)/Device/common/config/
INC += -I$(ALIF_DFP_REL_HERE)/Device/common/include/
INC += -I$(ALIF_DFP_REL_HERE)/Device/core/$(MCU_CORE)/config/
INC += -I$(ALIF_DFP_REL_HERE)/Device/core/$(MCU_CORE)/include/
INC += -I$(ALIF_DFP_REL_HERE)/Device/$(MCU_SERIES)/$(MCU_VARIANT)/
INC += -I$(TOP)/lib/tinyusb/src
INC += -Itinyusb_port
INC += -Ilwip_inc

GEN_PIN_MKPINS = mcu/make-pins.py
GEN_PIN_PREFIX = mcu/pins_prefix.c
GEN_PINS_BOARD_CSV = $(BOARD_DIR)/pins.csv
GEN_PINS_MCU_CSV = mcu/ensemble_pin_alt.csv
GEN_PINS_SRC = $(BUILD)/pins_board.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins_board.h

CFLAGS_FPU += -mfloat-abi=hard -mfpu=fpv5-d16

CFLAGS += $(INC) \
          -std=c99 \
          -Wall \
          -Werror \
          -Wdouble-promotion \
          -Wfloat-conversion \
          -mthumb \
          -mcpu=cortex-m55 \
          -mtune=cortex-m55 \
          $(CFLAGS_FPU) \
          -march=armv8.1-m.main+fp+mve.fp \
          -fdata-sections \
          -ffunction-sections \
          --specs=nosys.specs \
          -D$(MCU_CORE)=1 \
          -DCORE_$(MCU_CORE) \
          -DALIF_CMSIS_H="\"$(MCU_CORE).h\""

ifeq ($(MICROPY_FLOAT_IMPL),float)
CFLAGS += -fsingle-precision-constant
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
else
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_DOUBLE
endif

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -Og -ggdb3
# Disable text compression in debug builds
MICROPY_ROM_TEXT_COMPRESSION = 0
else
CFLAGS += -O2 -DNDEBUG
endif

CFLAGS += $(CFLAGS_EXTRA)

AFLAGS = -mthumb -march=armv8.1-m.main+fp+mve.fp $(CFLAGS_FPU)

CFLAGS += -Wl,-T$(BUILD)/ensemble.ld \
          -Wl,-Map=$@.map \
          -Wl,--cref \
          -Wl,--gc-sections \
          -Wl,--print-memory-usage \
          -Wl,--no-warn-rwx-segment

################################################################################
# Source files and libraries

SRC_O += \
	shared/runtime/gchelper_thumb2.o

SRC_C = \
	alif_flash.c \
	cyw43_port_spi.c \
	fatfs_port.c \
	machine_pin.c \
	machine_i2c.c \
	machine_spi.c \
	machine_rtc.c \
	main.c \
	modalif.c \
	mphalport.c \
	mpnetworkport.c \
	mpu.c \
	mpuart.c \
	msc_disk.c \
	nosys_stubs.c \
	ospi_ext.c \
	ospi_flash.c \
	pendsv.c \
	system_tick.c \
	se_services.c \
	usbd.c \
	vfs_rom_ioctl.c \
	$(wildcard $(BOARD_DIR)/*.c)

ifeq ($(MICROPY_SSL_MBEDTLS),1)
SRC_C += mbedtls/mbedtls_port.c
endif

ifeq ($(MICROPY_PY_BLUETOOTH),1)
SRC_C += mpbthciport.c mpnimbleport.c
endif

ifeq ($(MICROPY_FLOAT_IMPL),float)
LIBM_SRC_C += $(SRC_LIB_LIBM_C)
LIBM_SRC_C += $(SRC_LIB_LIBM_SQRT_HW_C)
$(BUILD)/lib/libm/%.o: CFLAGS += -Wno-maybe-uninitialized
else
LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_C)
LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_SQRT_HW_C)
$(BUILD)/lib/libm_dbl/%.o: CFLAGS += -Wno-maybe-uninitialized
endif

SHARED_SRC_C += $(addprefix shared/,\
	libc/string0.c \
	netutils/dhcpserver.c \
	netutils/trace.c \
	readline/readline.c \
	runtime/gchelper_native.c \
	runtime/interrupt_char.c \
	runtime/mpirq.c \
	runtime/pyexec.c \
	runtime/softtimer.c  \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	timeutils/timeutils.c \
	tinyusb/mp_usbd.c \
	tinyusb/mp_usbd_cdc.c \
	tinyusb/mp_usbd_descriptor.c \
	)

DRIVERS_SRC_C += $(addprefix drivers/,\
	bus/softspi.c \
	bus/softqspi.c \
	memory/spiflash.c \
	dht/dht.c \
	)

TINYUSB_SRC_C += \
	lib/tinyusb/src/tusb.c \
	lib/tinyusb/src/class/cdc/cdc_device.c \
	lib/tinyusb/src/class/msc/msc_device.c \
	lib/tinyusb/src/common/tusb_fifo.c \
	lib/tinyusb/src/device/usbd.c \
	lib/tinyusb/src/device/usbd_control.c \
	tinyusb_port/tusb_alif_dcd.c \

ALIF_SRC_C += $(addprefix $(ALIF_DFP_REL_TOP)/,\
	Device/common/source/clk.c \
	Device/common/source/mpu_M55.c \
	Device/common/source/system_M55.c \
	Device/common/source/system_utils.c \
	Device/common/source/tcm_partition.c \
	Device/common/source/tgu_M55.c \
	Device/common/source/pm.c \
	Device/core/$(MCU_CORE)/source/startup_$(MCU_CORE).c \
	drivers/source/adc.c \
	drivers/source/i2c.c \
	drivers/source/mhu_driver.c \
	drivers/source/mhu_receiver.c \
	drivers/source/mhu_sender.c \
	drivers/source/mram.c \
	drivers/source/pinconf.c \
	drivers/source/spi.c \
	drivers/source/uart.c \
	drivers/source/utimer.c \
	ospi_xip/source/ospi/ospi_drv.c \
	se_services/source/services_host_application.c \
	se_services/source/services_host_boot.c \
	se_services/source/services_host_clocks.c \
	se_services/source/services_host_cryptocell.c \
	se_services/source/services_host_handler.c \
	se_services/source/services_host_system.c \
	se_services/source/services_host_power.c \
	se_services/source/services_host_maintenance.c \
	)

$(BUILD)/tinyusb_port/tusb_alif_dcd.o: CFLAGS += -Wno-unused-variable -DTUSB_ALIF_NO_IRQ_CFG=1
$(BUILD)/$(ALIF_DFP_REL_TOP)/drivers/source/mram.o: CFLAGS += -Wno-strict-aliasing
$(BUILD)/$(ALIF_DFP_REL_TOP)/drivers/source/spi.o: CFLAGS += -Wno-maybe-uninitialized
$(BUILD)/$(ALIF_DFP_REL_TOP)/se_services/source/services_host_boot.o: CFLAGS += -Wno-stringop-truncation
$(BUILD)/$(ALIF_DFP_REL_TOP)/se_services/source/services_host_system.o: CFLAGS += -Wno-maybe-uninitialized

# Add Alif-specific implementation of libmetal (and optionally OpenAMP's rproc).
# Note: libmetal code is generated via a pre-processor so ensure that runs first.
ifeq ($(MICROPY_PY_OPENAMP),1)
SRC_C += mpmetalport.c
$(BUILD)/mpmetalport.o: $(BUILD)/openamp/metal/config.h
ifeq ($(MICROPY_PY_OPENAMP_REMOTEPROC),1)
SRC_C += mpremoteprocport.c
$(BUILD)/mpremoteprocport.o: $(BUILD)/openamp/metal/config.h
endif
endif

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SHARED_SRC_C) $(GEN_PINS_SRC)

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(TINYUSB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(ALIF_SRC_C:.c=.o))
OBJ += $(GEN_PINS_SRC:.c=.o)

################################################################################
# Main targets

.PHONY: all erase deploy gdb objdump release

.DELETE_ON_ERROR:

obj: $(OBJ)
all: $(BUILD)/firmware.bin

$(BUILD):
	$(MKDIR) -p $@

$(BUILD)/ensemble.ld: $(LD_FILE)
	$(ECHO) "Preprocess linker script $@"
	$(Q)$(CPP) -P -E $(CFLAGS) $^ > $@

$(BUILD)/firmware.elf: $(OBJ) $(BUILD)/ensemble.ld
	$(ECHO) "Link $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -Obinary $^ $(BUILD)/firmware.bin

################################################################################
# Remaining make rules

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_board.c and pins_board.h
$(BUILD)/%_board.c $(HEADER_BUILD)/%_board.h: $(BOARD_DIR)/%.csv $(GEN_PIN_MKPINS) $(GEN_PIN_PREFIX) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(GEN_PIN_MKPINS) \
	    --af-csv $(GEN_PINS_MCU_CSV) \
	    --board-csv $(GEN_PINS_BOARD_CSV) \
	    --prefix $(GEN_PIN_PREFIX) \
	    --output-source $(GEN_PINS_SRC) \
	    --output-header $(GEN_PINS_HDR)

include $(TOP)/py/mkrules.mk
