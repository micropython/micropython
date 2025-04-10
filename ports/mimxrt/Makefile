# =============================================================================
# Parameter Configuration
# =============================================================================

# Select the board to build for:
ifdef BOARD_DIR
# Custom board path - remove trailing slash and get the final component of
# the path as the board name.
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
# If not given on the command line, then default to TEENSY40.
BOARD ?= TEENSY40
BOARD_DIR ?= boards/$(BOARD)
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
    $(error Invalid BOARD specified: $(BOARD_DIR))
endif

BUILD ?= build-$(BOARD)
PORT ?= /dev/ttyACM0
CROSS_COMPILE ?= arm-none-eabi-
GIT_SUBMODULES += lib/tinyusb lib/nxp_driver
UF2CONV ?= $(TOP)/tools/uf2conv.py

# MicroPython feature configurations
MICROPY_VFS_LFS2 ?= 1
MICROPY_VFS_FAT ?= 1

# qstr definitions (must come before including py.mk)
QSTR_DEFS += qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES = $(BOARD_DIR)/mpconfigboard.h

# Generation scripts
MAKE_PINS = boards/make-pins.py
MAKE_FLEXRAM_LD = boards/make-flexram-config.py

# Include py make environment
include ../../py/mkenv.mk

# Include micropython configuration board makefile
include $(BOARD_DIR)/mpconfigboard.mk

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# File containing description of content to be frozen into firmware.
FROZEN_MANIFEST ?= boards/manifest.py

# Include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

# Set SDK directory based on MCU_SERIES
MCU_DIR = lib/nxp_driver/sdk/devices/$(MCU_SERIES)

# Select linker scripts based on MCU_SERIES
LD_FILES = boards/$(MCU_SERIES).ld boards/common.ld

# Parameter configurations for generation
AF_FILE = boards/$(MCU_SERIES)_af.csv
BOARD_PINS = $(BOARD_DIR)/pins.csv
PREFIX_FILE = boards/mimxrt_prefix.c
GEN_FLEXRAM_CONFIG_SRC = $(BUILD)/flexram_config.s
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_SRC = $(BUILD)/pins_gen.c

# =============================================================================
# Includes
# =============================================================================

INC += -I$(BOARD_DIR)
INC += -I$(BUILD)
INC += -I$(TOP)
INC += -I$(TOP)/$(MCU_DIR)
INC += -I$(TOP)/$(MCU_DIR)/drivers
INC += -I$(TOP)/lib/cmsis/inc
INC += -I$(TOP)/lib/oofatfs
INC += -I$(TOP)/lib/tinyusb/hw
INC += -I$(TOP)/lib/tinyusb/hw/bsp/teensy_40
INC += -I$(TOP)/lib/tinyusb/src
INC += -I$(TOP)/shared/tinyusb
INC += -I.
INC += -Ihal

# All settings for Ethernet support are controller by the value of MICROPY_PY_LWIP
ifeq ($(MICROPY_PY_LWIP),1)
INC += -Ilwip_inc
INC += -Ihal/phy
endif

# =============================================================================
# Sources
# =============================================================================

# TinyUSB Stack source
SRC_TINYUSB_C += \
	lib/tinyusb/src/class/cdc/cdc_device.c \
	lib/tinyusb/src/class/dfu/dfu_rt_device.c \
	lib/tinyusb/src/class/hid/hid_device.c \
	lib/tinyusb/src/class/midi/midi_device.c \
	lib/tinyusb/src/class/msc/msc_device.c \
	lib/tinyusb/src/class/usbtmc/usbtmc_device.c \
	lib/tinyusb/src/class/vendor/vendor_device.c \
	lib/tinyusb/src/common/tusb_fifo.c \
	lib/tinyusb/src/device/usbd.c \
	lib/tinyusb/src/device/usbd_control.c \
	lib/tinyusb/src/portable/chipidea/ci_hs/dcd_ci_hs.c \
	lib/tinyusb/src/tusb.c

# All settings for Ethernet support are controller by the value of MICROPY_PY_LWIP
ifeq ($(MICROPY_PY_LWIP),1)
SRC_ETH_C += \
	$(MCU_DIR)/drivers/fsl_enet.c \
	hal/phy/device/phydp83825/fsl_phydp83825.c \
	hal/phy/device/phydp83848/fsl_phydp83848.c \
	hal/phy/device/phyksz8081/fsl_phyksz8081.c \
	hal/phy/device/phylan8720/fsl_phylan8720.c \
	hal/phy/device/phyrtl8211f/fsl_phyrtl8211f.c \
	hal/phy/mdio/enet/fsl_enet_mdio.c
endif

# NXP SDK sources
SRC_HAL_IMX_C += \
	$(MCU_DIR)/drivers/fsl_clock.c \
	$(MCU_DIR)/drivers/fsl_common.c \
	$(MCU_DIR)/drivers/fsl_dmamux.c \
	$(MCU_DIR)/drivers/fsl_edma.c \
	$(MCU_DIR)/drivers/fsl_flexram.c \
	$(MCU_DIR)/drivers/fsl_flexspi.c \
	$(MCU_DIR)/drivers/fsl_gpc.c \
	$(MCU_DIR)/drivers/fsl_gpio.c \
	$(MCU_DIR)/drivers/fsl_gpt.c \
	$(MCU_DIR)/drivers/fsl_lpi2c.c \
	$(MCU_DIR)/drivers/fsl_lpspi.c \
	$(MCU_DIR)/drivers/fsl_lpspi_edma.c \
	$(MCU_DIR)/drivers/fsl_pit.c \
	$(MCU_DIR)/drivers/fsl_pwm.c \
	$(MCU_DIR)/drivers/fsl_sai.c \
	$(MCU_DIR)/drivers/fsl_snvs_hp.c \
	$(MCU_DIR)/drivers/fsl_snvs_lp.c \
	$(MCU_DIR)/drivers/fsl_wdog.c \
	$(MCU_DIR)/system_$(MCU_SERIES)$(MCU_CORE).c \

# Use a specific boot header for 1062 so the Teensy loader doesn't erase the filesystem.
ifeq ($(MCU_SERIES), MIMXRT1062)
SRC_HAL_IMX_C += hal/fsl_flexspi_nor_boot.c
else
SRC_HAL_IMX_C += $(MCU_DIR)/xip/fsl_flexspi_nor_boot.c
endif

ifeq ($(MICROPY_HW_SDRAM_AVAIL),1)
SRC_HAL_IMX_C += $(MCU_DIR)/drivers/fsl_semc.c
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES), MIMXRT1021 MIMXRT1052 MIMXRT1062 MIMXRT1064 MIMXRT1176))
SRC_HAL_IMX_C += $(MCU_DIR)/drivers/fsl_usdhc.c
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES), MIMXRT1015 MIMXRT1021 MIMXRT1052 MIMXRT1062 MIMXRT1064 MIMXRT1176))
SRC_HAL_IMX_C += \
	$(MCU_DIR)/drivers/fsl_qtmr.c \
	$(MCU_DIR)/drivers/fsl_romapi.c
endif

# If not empty, then it is 10xx.
ifneq ($(findstring MIMXRT10, $(MCU_SERIES)),)
APPLICATION_ADDR := 0x6000C000
else
APPLICATION_ADDR := 0x3000C000
endif

ifeq ($(MCU_SERIES), MIMXRT1176)
INC += -I$(TOP)/$(MCU_DIR)/drivers/cm7

SRC_HAL_IMX_C += \
	$(MCU_DIR)/drivers/cm7/fsl_cache.c \
	$(MCU_DIR)/drivers/fsl_dcdc.c \
	$(MCU_DIR)/drivers/fsl_pmu.c \
	$(MCU_DIR)/drivers/fsl_common_arm.c \
	$(MCU_DIR)/drivers/fsl_anatop_ai.c \
	$(MCU_DIR)/drivers/fsl_caam.c \
	$(MCU_DIR)/drivers/fsl_lpadc.c \
	$(MCU_DIR)/drivers/fsl_mu.c
else
SRC_HAL_IMX_C += \
	$(MCU_DIR)/drivers/fsl_adc.c \
	$(MCU_DIR)/drivers/fsl_cache.c \
	$(MCU_DIR)/drivers/fsl_trng.c
endif

# C source files
SRC_C += \
	board_init.c \
	boards/$(MCU_SERIES)_clock_config.c \
	dma_manager.c \
	drivers/bus/softspi.c \
	drivers/dht/dht.c \
	eth.c \
	fatfs_port.c \
	flash.c \
	hal/fsl_lpuart.c \
	hal/pwm_backport.c \
	help.c \
	led.c \
	machine_bitstream.c \
	machine_i2c.c \
	machine_led.c \
	machine_pin.c \
	machine_rtc.c \
	machine_sdcard.c \
	machine_spi.c \
	main.c \
	mbedtls/mbedtls_port.c \
	mimxrt_flash.c \
	mimxrt_sdram.c \
	modmimxrt.c \
	mphalport.c \
	mpnetworkport.c \
	msc_disk.c \
	network_lan.c \
	pendsv.c \
	pin.c \
	sdcard.c \
	sdio.c \
	systick.c \
	ticks.c \
	usbd.c \

SHARED_SRC_C += \
	shared/libc/printf.c \
	shared/libc/string0.c \
	shared/netutils/dhcpserver.c \
	shared/netutils/netutils.c \
	shared/netutils/trace.c \
	shared/readline/readline.c \
	shared/runtime/gchelper_native.c \
	shared/runtime/interrupt_char.c \
	shared/runtime/mpirq.c \
	shared/runtime/pyexec.c \
	shared/runtime/softtimer.c \
	shared/runtime/stdout_helpers.c \
	shared/runtime/sys_stdio_mphal.c \
	shared/timeutils/timeutils.c \
	shared/tinyusb/mp_usbd.c \
	shared/tinyusb/mp_usbd_cdc.c \
	shared/tinyusb/mp_usbd_descriptor.c \

# Set flash driver name, base address and internal flash flag, based on the flash type.
ifeq ($(MICROPY_HW_FLASH_TYPE),$(filter $(MICROPY_HW_FLASH_TYPE),qspi_nor_flash))
    MICROPY_HW_FLASH_BASE = 0x60000000
    FLEXSPI_FLASH_TYPE = $(MICROPY_HW_FLASH_TYPE)
else ifeq ($(MICROPY_HW_FLASH_TYPE),$(filter $(MICROPY_HW_FLASH_TYPE),qspi_hyper_flash))
    MICROPY_HW_FLASH_BASE = 0x60000000
    FLEXSPI_FLASH_TYPE = $(MICROPY_HW_FLASH_TYPE)
else ifeq ($(MICROPY_HW_FLASH_TYPE),$(filter $(MICROPY_HW_FLASH_TYPE),internal))
    # The internal flash is an SPI NOR Flash.
    MICROPY_HW_FLASH_BASE = 0x70000000
    FLEXSPI_FLASH_TYPE = qspi_nor_flash
    CFLAGS += -DMICROPY_HW_FLASH_INTERNAL
else
    $(error Error: Unknown board flash type $(MICROPY_HW_FLASH_TYPE))
endif

# Set a flag if the UF2 bootloader is used
ifeq ($(USE_UF2_BOOTLOADER),1)
	CFLAGS += -DMICROPY_MACHINE_UF2_BOOTLOADER=1
endif

# Add sources for respective board flash type
# Add hal/flexspi_nor_flash.c or hal/flashspi_hyper_flash.c respectively
SRC_HAL_C += hal/flexspi_$(subst qspi_,,$(FLEXSPI_FLASH_TYPE)).c
#
# Add custom (board specific) or default configuration
ifeq ($(MICROPY_HW_BOARD_FLASH_FILES),1)
    SRC_HAL_C += $(BOARD_DIR)/$(FLEXSPI_FLASH_TYPE)_config.c
else
    SRC_HAL_C += hal/$(FLEXSPI_FLASH_TYPE)_config.c
endif

ifeq ($(MICROPY_PY_BLUETOOTH),1)
SRC_C += mpbthciport.c
DRIVERS_SRC_C += drivers/cyw43/cywbt.c
endif # MICROPY_PY_BLUETOOTH

ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)
SRC_C += mpnimbleport.c
endif

# Add mimxrt-specific implementation of libmetal (and optionally OpenAMP's rproc).
# Note: libmetal code is generated via a pre-processor so ensure that runs first.
ifeq ($(MICROPY_PY_OPENAMP),1)
SRC_C += mpmetalport.c
$(BUILD)/mpmetalport.o: $(BUILD)/openamp/metal/config.h
ifeq ($(MICROPY_PY_OPENAMP_REMOTEPROC),1)
SRC_C += mpremoteprocport.c
$(BUILD)/mpremoteprocport.o: $(BUILD)/openamp/metal/config.h
endif
endif

# Math library source files
ifeq ($(MICROPY_FLOAT_IMPL),double)
    LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_C)
    #
    ifeq ($(SUPPORTS_HARDWARE_FP_DOUBLE),1)
        LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_SQRT_HW_C)
    else
        LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_SQRT_SW_C)
    endif
else
    LIBM_SRC_C += $(SRC_LIB_LIBM_C)
    ifeq ($(SUPPORTS_HARDWARE_FP_SINGLE),1)
        LIBM_SRC_C += $(SRC_LIB_LIBM_SQRT_HW_C)
    else
        LIBM_SRC_C += $(SRC_LIB_LIBM_SQRT_SW_C)
    endif
endif

# Reset variables
SUPPORTS_HARDWARE_FP_SINGLE = 0
SUPPORTS_HARDWARE_FP_DOUBLE = 0

# Assembly source files
SRC_SS = \
	$(MCU_DIR)/gcc/startup_$(MCU_SERIES)$(MCU_CORE).S \
	hal/resethandler_MIMXRT10xx.S

SRC_S += shared/runtime/gchelper_thumb2.s \

hal/resethandler_MIMXRT10xx.S: $(GEN_FLEXRAM_CONFIG_SRC)

# =============================================================================
# QSTR Sources
# =============================================================================

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SHARED_SRC_C) $(GEN_PINS_SRC)

# =============================================================================
# Compiler Flags
# =============================================================================

CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG),1)
CFLAGS += -Og
# Disable text compression in debug builds
MICROPY_ROM_TEXT_COMPRESSION = 0
else
CFLAGS += -Os -DNDEBUG
endif

# Set default values for optional variables
MICROPY_HW_SDRAM_AVAIL ?= 0
MICROPY_HW_SDRAM_SIZE ?= 0

# Configure default compiler flags
CFLAGS += \
	$(INC) \
	-D__START=main \
	-D__STARTUP_CLEAR_BSS \
	-D__STARTUP_INITIALIZE_RAMFUNCTION \
	-DBOARD_$(BOARD) \
	-DBOARD_FLASH_SIZE=$(MICROPY_HW_FLASH_SIZE) \
	-DCFG_TUSB_MCU=OPT_MCU_MIMXRT1XXX \
	-DCFG_TUD_MAX_SPEED=OPT_MODE_HIGH_SPEED \
	-DCLOCK_CONFIG_H='<boards/$(MCU_SERIES)_clock_config.h>' \
	-DCPU_$(MCU_SERIES)$(MCU_CORE) \
	-DCPU_$(MCU_VARIANT) \
	-DCPU_HEADER_H='<$(MCU_SERIES)$(MCU_CORE).h>' \
	-DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \
	-DI2C_RETRY_TIMES=1000000 \
	-DMICROPY_HW_FLASH_SIZE=$(MICROPY_HW_FLASH_SIZE) \
	-DMICROPY_HW_SDRAM_AVAIL=$(MICROPY_HW_SDRAM_AVAIL) \
	-DMICROPY_HW_SDRAM_SIZE=$(MICROPY_HW_SDRAM_SIZE) \
	-DSPI_RETRY_TIMES=1000000 \
	-DUART_RETRY_TIMES=1000000 \
	-DXIP_BOOT_HEADER_ENABLE=1 \
	-DXIP_EXTERNAL_FLASH=1 \
	-fdata-sections \
	-ffunction-sections \
	-mcpu=cortex-m7 \
	-mthumb \
	-mtune=cortex-m7 \
	-nostdlib \
	-std=c99 \
	-Wall \
	-Wdouble-promotion \
	-Werror \
	-Wfloat-conversion \
	-Wno-error=unused-parameter

# Configure respective board flash type
# Add hal/flexspi_nor_flash.h or hal/flexspi_hyper_flash.h respectively
CFLAGS += -DBOARD_FLASH_OPS_HEADER_H=\"hal/flexspi_$(subst qspi_,,$(FLEXSPI_FLASH_TYPE)).h\"
#
# Add custom (board specific) or default configuration
ifeq ($(MICROPY_HW_BOARD_FLASH_FILES),1)
    CFLAGS += -DBOARD_FLASH_CONFIG_HEADER_H=\"$(BOARD)_flexspi_flash_config.h\"
else
    CFLAGS += -DBOARD_FLASH_CONFIG_HEADER_H=\"hal/flexspi_flash_config.h\"
endif

# Configure floating point support
ifeq ($(MICROPY_FLOAT_IMPL),single)
    CFLAGS += \
        -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT \
        -fsingle-precision-constant \
        -mfloat-abi=softfp \
        -mfpu=fpv5-sp-d16
else ifeq ($(MICROPY_FLOAT_IMPL),double)
    CFLAGS += \
        -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_DOUBLE \
        -mfloat-abi=hard \
        -mfpu=fpv5-d16
else ifeq ($(MICROPY_FLOAT_IMPL),none)
    CFLAGS += \
        -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_NONE
else
    $(error Error: Unknown floating point implementation $(MICROPY_FLOAT_IMPL))
endif


# All settings for Ethernet support are controller by the value of MICROPY_PY_LWIP
ifeq ($(MICROPY_PY_LWIP),1)
CFLAGS += \
	-DFSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE=1
endif

ifdef MICROPY_HW_FLASH_CLK
	CFLAGS += -DMICROPY_HW_FLASH_CLK=$(MICROPY_HW_FLASH_CLK)
endif
ifdef MICROPY_HW_FLASH_QE_CMD
	CFLAGS += -DMICROPY_HW_FLASH_QE_CMD=$(MICROPY_HW_FLASH_QE_CMD)
endif
ifdef MICROPY_HW_FLASH_QE_ARG
	CFLAGS += -DMICROPY_HW_FLASH_QE_ARG=$(MICROPY_HW_FLASH_QE_ARG)
endif

CFLAGS += $(CFLAGS_EXTRA)

MPY_CROSS_FLAGS += -march=armv7m

# =============================================================================
# Linker Flags
# =============================================================================

LDFLAGS += \
	--cref \
	--gc-sections \
	--print-memory-usage \
	-Map=$@.map

# LDDEFINES are used for link time adaptation of linker scripts, utilizing
#	the C preprocessor. Therefore keep LDDEFINES separated from LDFLAGS!

LDDEFINES = \
	-DMICROPY_HW_FLASH_BASE=$(MICROPY_HW_FLASH_BASE) \
	-DMICROPY_HW_FLASH_SIZE=$(MICROPY_HW_FLASH_SIZE)

ifdef MICROPY_HW_FLASH_RESERVED
LDDEFINES += -DMICROPY_HW_FLASH_RESERVED=$(MICROPY_HW_FLASH_RESERVED)
endif

ifdef MICROPY_HW_SDRAM_AVAIL
LDDEFINES += \
	-DMICROPY_HW_SDRAM_AVAIL=$(MICROPY_HW_SDRAM_AVAIL) \
	-DMICROPY_HW_SDRAM_SIZE=$(MICROPY_HW_SDRAM_SIZE)
endif

# =============================================================================
# Library and Object files
# =============================================================================

LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SS:.S=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_TINYUSB_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_HAL_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_HAL_IMX_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_ETH_C:.c=.o))
OBJ += $(GEN_PINS_SRC:.c=.o)

# Workaround for bug in older gcc, warning on "static usbd_device_t _usbd_dev = { 0 };"
$(BUILD)/lib/tinyusb/src/device/usbd.o: CFLAGS += -Wno-missing-braces

# =============================================================================
# Build targets
# =============================================================================

ifeq ($(USE_UF2_BOOTLOADER),1)
all: $(BUILD)/firmware.hex $(BUILD)/firmware.bin $(BUILD)/firmware.uf2
else
all: $(BUILD)/firmware.hex $(BUILD)/firmware.bin
endif

# Process linker scripts with C preprocessor to exchange LDDEFINES and
#	aggregate output of preprocessor in a single linker script `link.ld`
$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "PREPROCESS LINK $@"
	$(Q)$(CC) -E -x c $(LDDEFINES) $(LD_FILES) | grep -v '^#' > $(BUILD)/link.ld
	$(ECHO) "LINK $@"
	$(Q)$(LD) -T$(BUILD)/link.ld $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary -R .stack -R .ivt -R .flash_config $^ $@-binpart
	$(Q)$(PYTHON) $(UF2CONV) -b $(APPLICATION_ADDR) -f MIMXRT10XX -c -o $@ $@-binpart

# Making OBJ use an order-only dependency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(GEN_PINS_HDR)

# With conditional pins, we may need to regenerate qstrdefs.h when config
# options change.
$(HEADER_BUILD)/qstrdefs.generated.h: $(BOARD_DIR)/mpconfigboard.h

$(GEN_FLEXRAM_CONFIG_SRC): $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_FLEXRAM_LD) -d $(TOP)/$(MCU_DIR)/$(MCU_SERIES)$(MCU_CORE).h \
		-f $(TOP)/$(MCU_DIR)/$(MCU_SERIES)$(MCU_CORE)_features.h -l boards/$(MCU_SERIES).ld -c $(MCU_SERIES) > $@

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_gen.c and pins.h
$(BUILD)/%_gen.c $(HEADER_BUILD)/%.h: $(BOARD_PINS) $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board-csv $(BOARD_PINS) --af-csv $(AF_FILE) \
	    --prefix $(PREFIX_FILE) --iomux "$(abspath $(TOP)/$(MCU_DIR)/drivers/fsl_iomuxc.h)" \
	    --output-source $(GEN_PINS_SRC) --output-header $(GEN_PINS_HDR)

include $(TOP)/py/mkrules.mk
