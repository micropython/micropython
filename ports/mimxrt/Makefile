BOARD ?= TEENSY40
BOARD_DIR ?= boards/$(BOARD)
BUILD ?= build-$(BOARD)

PORT ?= /dev/ttyACM0

CROSS_COMPILE ?= arm-none-eabi-

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

include ../../py/mkenv.mk
include $(BOARD_DIR)/mpconfigboard.mk

# Set optional flash configuration variables
BOARD_FLASH_RESERVED ?=

LD_MEMORY_CONFIG_DEFINES += \
	BOARD_FLASH_TYPE=$(BOARD_FLASH_TYPE) \
	BOARD_FLASH_SIZE=$(BOARD_FLASH_SIZE)

ifdef $(BOARD_FLASH_RESERVED)
LD_MEMORY_CONFIG_DEFINES += BOARD_FLASH_RESERVED=$(BOARD_FLASH_RESERVED)
endif

# Qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES = $(BOARD_DIR)/mpconfigboard.h

# MicroPython feature configurations
FROZEN_MANIFEST ?= boards/manifest.py
MICROPY_VFS_LFS2 ?= 1
MICROPY_VFS_FAT ?= 1

# Include py core make definitions
include $(TOP)/py/py.mk

GIT_SUBMODULES = lib/tinyusb lib/nxp_driver

MCU_DIR = lib/nxp_driver/sdk/devices/$(MCU_SERIES)
LD_FILES = boards/$(MCU_SERIES).ld boards/common.ld

MAKE_PINS = boards/make-pins.py
MAKE_FLEXRAM_LD = boards/make-flexram-config.py
BOARD_PINS = $(BOARD_DIR)/pins.csv
AF_FILE = boards/$(MCU_SERIES)_af.csv
PREFIX_FILE = boards/mimxrt_prefix.c
GEN_PINS_SRC = $(BUILD)/pins_gen.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(BUILD)/pins_qstr.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_PY = $(BUILD)/pins_af.py
GEN_FLEXRAM_CONFIG_SRC = $(BUILD)/flexram_config.s

# mcu driver cause following warnings
#CFLAGS += -Wno-error=float-equal -Wno-error=nested-externs
CFLAGS += -Wno-error=unused-parameter

INC += -I.
INC += -Ihal
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

CFLAGS_MCU = -mtune=cortex-m7 -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS += $(INC) -Wall -Werror -Wdouble-promotion -Wfloat-conversion -std=c99 -nostdlib -mthumb $(CFLAGS_MCU)
CFLAGS += -DCPU_$(MCU_SERIES) -DCPU_$(MCU_VARIANT)
CFLAGS += -DXIP_EXTERNAL_FLASH=1 \
	-DXIP_BOOT_HEADER_ENABLE=1 \
	-DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \
	-DCFG_TUSB_MCU=OPT_MCU_MIMXRT10XX \
	-D__STARTUP_CLEAR_BSS \
	-D__STARTUP_INITIALIZE_RAMFUNCTION \
	-D__START=main \
	-DCPU_HEADER_H='<$(MCU_SERIES).h>' \
	-DBOARD_FLASH_SIZE=$(BOARD_FLASH_SIZE) \
	-DBOARD_FLASH_CONFIG_HEADER_H=\"$(BOARD)_flexspi_nor_config.h\"

ifeq ($(BOARD_FLASH_TYPE), qspi_nor)
CFLAGS += -DBOARD_FLASH_OPS_HEADER_H=\"hal/flexspi_nor_flash.h\"
else ifeq ($(BOARD_FLASH_TYPE), hyperflash)
CFLAGS += -DBOARD_FLASH_OPS_HEADER_H=\"hal/flexspi_hyper_flash.h\"
endif

ifeq ($(MICROPY_PY_MACHINE_SDCARD),1)
CFLAGS += -DMICROPY_PY_MACHINE_SDCARD=1
endif

CFLAGS += $(CFLAGS_MOD) $(CFLAGS_EXTRA)

# Configure floating point support
ifeq ($(MICROPY_FLOAT_IMPL),double)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_DOUBLE
else
ifeq ($(MICROPY_FLOAT_IMPL),none)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_NONE
else
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
CFLAGS += -fsingle-precision-constant
endif
endif

SUPPORTS_HARDWARE_FP_SINGLE = 0
SUPPORTS_HARDWARE_FP_DOUBLE = 0

LDFLAGS = -Map=$@.map --cref --print-memory-usage
LDDEFINES = $(addprefix -D, $(LD_MEMORY_CONFIG_DEFINES))
LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

# Tune for Debugging or Optimization
ifeq ($(DEBUG),1)
CFLAGS += -O0 -ggdb
LDFLAGS += --gc-sections
CFLAGS += -fdata-sections -ffunction-sections
else
CFLAGS += -Os -DNDEBUG
LDFLAGS += --gc-sections
CFLAGS += -fdata-sections -ffunction-sections
endif

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
	lib/tinyusb/src/portable/nxp/transdimension/dcd_transdimension.c \
	lib/tinyusb/src/tusb.c

SRC_HAL_IMX_C += \
	$(MCU_DIR)/drivers/fsl_adc.c \
	$(MCU_DIR)/drivers/fsl_cache.c \
	$(MCU_DIR)/drivers/fsl_clock.c \
	$(MCU_DIR)/drivers/fsl_common.c \
	$(MCU_DIR)/drivers/fsl_dmamux.c \
	$(MCU_DIR)/drivers/fsl_edma.c \
	$(MCU_DIR)/drivers/fsl_flexram.c \
	$(MCU_DIR)/drivers/fsl_flexspi.c \
	$(MCU_DIR)/drivers/fsl_gpio.c \
	$(MCU_DIR)/drivers/fsl_gpt.c \
	$(MCU_DIR)/drivers/fsl_lpi2c.c \
	$(MCU_DIR)/drivers/fsl_lpspi.c \
	$(MCU_DIR)/drivers/fsl_lpspi_edma.c \
	$(MCU_DIR)/drivers/fsl_lpuart.c \
	$(MCU_DIR)/drivers/fsl_pit.c \
	$(MCU_DIR)/drivers/fsl_snvs_lp.c \
	$(MCU_DIR)/drivers/fsl_trng.c \
	$(MCU_DIR)/project_template/clock_config.c \
	$(MCU_DIR)/system_$(MCU_SERIES).c \
	$(MCU_DIR)/xip/fsl_flexspi_nor_boot.c \

ifeq ($(MICROPY_PY_MACHINE_SDCARD),1)
SRC_HAL_IMX_C += $(MCU_DIR)/drivers/fsl_usdhc.c
endif

SRC_C += \
	board_init.c \
	dma_channel.c \
	drivers/bus/softspi.c \
	extmod/modonewire.c \
	fatfs_port.c \
	led.c \
	machine_adc.c \
	machine_bitstream.c \
	machine_i2c.c \
	machine_led.c \
	machine_pin.c \
	machine_rtc.c \
	machine_sdcard.c \
	machine_spi.c \
	machine_timer.c \
	machine_uart.c \
	main.c \
	mimxrt_flash.c \
	modmachine.c \
	modmimxrt.c \
	moduos.c \
	modutime.c \
	mphalport.c \
	pin.c \
	sdcard.c \
	shared/libc/printf.c \
	shared/libc/string0.c \
	shared/readline/readline.c \
	shared/runtime/gchelper_native.c \
	shared/runtime/mpirq.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	shared/runtime/sys_stdio_mphal.c \
	shared/timeutils/timeutils.c \
	ticks.c \
	tusb_port.c \
	$(SRC_TINYUSB_C) \
	$(SRC_HAL_IMX_C) \

ifeq ($(BOARD_FLASH_TYPE), qspi_nor)
SRC_C += \
	hal/flexspi_nor_flash.c \
	$(BOARD_DIR)/qspi_nor_flash_config.c
else ifeq ($(BOARD_FLASH_TYPE), hyperflash)
SRC_C += \
	hal/flexspi_hyper_flash.c \
	$(BOARD_DIR)/qspi_hyper_flash_config.c
else
$(error Error: Unknown board flash type $(BOARD_FLASH_TYPE))
endif

ifeq ($(MICROPY_FLOAT_IMPL),double)
LIBM_SRC_C += $(addprefix lib/libm_dbl/,\
	__cos.c \
	__expo2.c \
	__fpclassify.c \
	__rem_pio2.c \
	__rem_pio2_large.c \
	__signbit.c \
	__sin.c \
	__tan.c \
	acos.c \
	acosh.c \
	asin.c \
	asinh.c \
	atan.c \
	atan2.c \
	atanh.c \
	ceil.c \
	copysign.c \
	cos.c \
	cosh.c \
	erf.c \
	exp.c \
	expm1.c \
	floor.c \
	fmod.c \
	frexp.c \
	ldexp.c \
	lgamma.c \
	log.c \
	log10.c \
	log1p.c \
	modf.c \
	nearbyint.c \
	pow.c \
	rint.c \
	round.c \
	scalbn.c \
	sin.c \
	sinh.c \
	tan.c \
	tanh.c \
	tgamma.c \
	trunc.c \
	)
ifeq ($(SUPPORTS_HARDWARE_FP_DOUBLE),1)
LIBM_SRC_C += lib/libm_dbl/thumb_vfp_sqrt.c
else
LIBM_SRC_C += lib/libm_dbl/sqrt.c
endif
else
LIBM_SRC_C += $(addprefix lib/libm/,\
	acoshf.c \
	asinfacosf.c \
	asinhf.c \
	atan2f.c \
	atanf.c \
	atanhf.c \
	ef_rem_pio2.c \
	erf_lgamma.c \
	fmodf.c \
	kf_cos.c \
	kf_rem_pio2.c \
	kf_sin.c \
	kf_tan.c \
	log1pf.c \
	math.c \
	nearbyintf.c \
	roundf.c \
	sf_cos.c \
	sf_erf.c \
	sf_frexp.c \
	sf_ldexp.c \
	sf_modf.c \
	sf_sin.c \
	sf_tan.c \
	wf_lgamma.c \
	wf_tgamma.c \
	)
ifeq ($(SUPPORTS_HARDWARE_FP_SINGLE),1)
LIBM_SRC_C += lib/libm/thumb_vfp_sqrtf.c
else
LIBM_SRC_C += lib/libm/ef_sqrt.c
endif
endif

LIBM_O = $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))

# Too many warnings in libm_dbl, disable for now.
ifeq ($(MICROPY_FLOAT_IMPL),double)
$(LIBM_O): CFLAGS := $(filter-out -Wdouble-promotion -Wfloat-conversion, $(CFLAGS))
endif

SRC_SS = \
	$(MCU_DIR)/gcc/startup_$(MCU_SERIES).S \
	hal/resethandler_MIMXRT10xx.S

SRC_S += shared/runtime/gchelper_m3.s \

# List of sources for qstr extraction
SRC_QSTR += \
	extmod/modonewire.c \
	machine_adc.c \
	machine_led.c \
	machine_pin.c \
	machine_rtc.c \
	machine_sdcard.c \
	machine_spi.c \
	machine_timer.c \
	machine_uart.c \
	mimxrt_flash.c \
	modmachine.c \
	modmimxrt.c \
	moduos.c \
	modutime.c \
	pin.c \
	shared/runtime/mpirq.c \
	shared/runtime/sys_stdio_mphal.c \
	$(GEN_PINS_SRC) \

OBJ += $(PY_O)
OBJ += $(LIBM_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SS:.S=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))
OBJ += $(BUILD)/pins_gen.o

# Workaround for bug in older gcc, warning on "static usbd_device_t _usbd_dev = { 0 };"
$(BUILD)/lib/tinyusb/src/device/usbd.o: CFLAGS += -Wno-missing-braces

all: $(BUILD)/firmware.hex $(BUILD)/firmware.bin

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

# Making OBJ use an order-only dependency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(GEN_PINS_HDR) $(GEN_FLEXRAM_CONFIG_SRC)

# With conditional pins, we may need to regenerate qstrdefs.h when config
# options change.
$(HEADER_BUILD)/qstrdefs.generated.h: $(BOARD_DIR)/mpconfigboard.h

$(GEN_FLEXRAM_CONFIG_SRC):
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_FLEXRAM_LD) -d $(TOP)/$(MCU_DIR)/$(MCU_SERIES).h \
		-f $(TOP)/$(MCU_DIR)/$(MCU_SERIES)_features.h -l boards/$(MCU_SERIES).ld -c $(MCU_SERIES) > $(GEN_FLEXRAM_CONFIG_SRC)


# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_gen.c and pins.h
$(BUILD)/%_gen.c $(HEADER_BUILD)/%.h: $(BOARD_PINS) $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE)\
		--iomux $(abspath $(TOP)/$(MCU_DIR)/drivers/fsl_iomuxc.h) \
		--prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) > $(GEN_PINS_SRC)

$(BUILD)/pins_gen.o: $(BUILD)/pins_gen.c
	$(call compile_c)

include $(TOP)/py/mkrules.mk
