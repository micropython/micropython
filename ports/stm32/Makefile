# Select the board to build for:
ifdef BOARD_DIR
# Custom board path - remove trailing slash and get the final component of
# the path as the board name.
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
# If not given on the command line, then default to PYBV10.
BOARD ?= PYBV10
BOARD_DIR ?= boards/$(BOARD)
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

ifneq ($(BOARD_VARIANT),)
ifeq ($(wildcard $(BOARD_DIR)/mpconfigvariant_$(BOARD_VARIANT).mk),)
$(error Invalid BOARD_VARIANT specified: $(BOARD_VARIANT))
endif
endif

# If the build directory is not given, make it reflect the board name (and
# optionally the board variant).
ifneq ($(BOARD_VARIANT),)
BUILD ?= build-$(BOARD)-$(BOARD_VARIANT)
else
BUILD ?= build-$(BOARD)
endif

include ../../py/mkenv.mk
-include mpconfigport.mk

# Include board specific .mk file, and optional board variant .mk file.
include $(BOARD_DIR)/mpconfigboard.mk
ifeq ($(BOARD_VARIANT),)
-include $(BOARD_DIR)/mpconfigvariant.mk
else
include $(BOARD_DIR)/mpconfigvariant_$(BOARD_VARIANT).mk
endif

# qstr definitions (must come before including py.mk)
QSTR_DEFS += qstrdefsport.h

# Files that are generated and needed before the QSTR build.
QSTR_GLOBAL_DEPENDENCIES += mpconfigboard_common.h $(BOARD_DIR)/mpconfigboard.h $(GEN_STMCONST_HDR)

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# File containing description of content to be frozen into firmware.
FROZEN_MANIFEST ?= boards/manifest.py

# Location of mboot (or other bootloader) if the device has one.
# Used by machine.bootloader().
MBOOT_TEXT0_ADDR ?= 0x08000000

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

GIT_SUBMODULES += lib/libhydrogen lib/stm32lib

LD_DIR=boards
USBDEV_DIR=usbdev
#USBHOST_DIR=usbhost
DFU=$(TOP)/tools/dfu.py
MBOOT_PACK_DFU = mboot/mboot_pack_dfu.py
# may need to prefix dfu-util with sudo
USE_PYDFU ?= 1
PYDFU ?= $(TOP)/tools/pydfu.py
DFU_UTIL ?= dfu-util
BOOTLOADER_DFU_USB_VID ?= 0x0483
BOOTLOADER_DFU_USB_PID ?= 0xDF11
STFLASH ?= st-flash
OPENOCD ?= openocd
OPENOCD_CONFIG ?= boards/openocd_stm32f4.cfg

include stm32.mk

PLLVALUES = boards/pllvalues.py
PLLI2SVALUES = boards/plli2svalues.py
MAKE_PINS = boards/make-pins.py
BOARD_PINS = $(BOARD_DIR)/pins.csv
PREFIX_FILE = boards/stm32f4xx_prefix.c
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_DEFS = $(HEADER_BUILD)/pins_af_defs.h

INSERT_USB_IDS = $(TOP)/tools/insert-usb-ids.py
FILE2H = $(TOP)/tools/file2h.py

USB_IDS_FILE = mpconfigboard_common.h
CDCINF_TEMPLATE = pybcdc.inf_template
GEN_CDCINF_FILE = $(HEADER_BUILD)/pybcdc.inf
GEN_CDCINF_HEADER = $(HEADER_BUILD)/pybcdc_inf.h

GEN_PLLFREQTABLE_HDR = $(HEADER_BUILD)/pllfreqtable.h
GEN_PLLI2STABLE_HDR = $(HEADER_BUILD)/plli2stable.h
GEN_STMCONST_HDR = $(HEADER_BUILD)/modstm_const.h
GEN_STMCONST_MPZ = $(HEADER_BUILD)/modstm_mpz.h
CMSIS_MCU_HDR = $(STM32LIB_CMSIS_ABS)/Include/$(CMSIS_MCU_LOWER).h

# Select the cross compile prefix
CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(TOP)/lib/cmsis/inc
INC += -I$(STM32LIB_CMSIS_ABS)/Include
INC += -I$(STM32LIB_HAL_ABS)/Inc
INC += -I$(USBDEV_DIR)/core/inc -I$(USBDEV_DIR)/class/inc
#INC += -I$(USBHOST_DIR)
INC += -Ilwip_inc

CFLAGS += $(INC) -Wall -Wpointer-arith -Werror -Wdouble-promotion -Wfloat-conversion -std=gnu99 -nostdlib $(CFLAGS_EXTRA)
CFLAGS += -D$(CMSIS_MCU) -DUSE_FULL_LL_DRIVER
CFLAGS += $(CFLAGS_MCU_$(MCU_SERIES))
CFLAGS += $(COPT)
CFLAGS += -I$(BOARD_DIR)
CFLAGS += -DSTM32_HAL_H='<stm32$(MCU_SERIES)xx_hal.h>'
CFLAGS += -DMBOOT_VTOR=$(MBOOT_TEXT0_ADDR)
CFLAGS += -DMICROPY_HW_VTOR=$(TEXT0_ADDR)

AFLAGS += $(filter -mcpu=%,$(CFLAGS_MCU_$(MCU_SERIES)))

# Configure for nan-boxing object model if requested
ifeq ($(NANBOX),1)
CFLAGS += -DMP_CONFIGFILE='"mpconfigport_nanbox.h"'
ifneq ($(MICROPY_FLOAT_IMPL),none)
MICROPY_FLOAT_IMPL = double
endif
endif

# Configure floating point support
ifeq ($(MICROPY_FLOAT_IMPL),double)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_DOUBLE
else ifeq ($(MICROPY_FLOAT_IMPL),none)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_NONE
else
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
CFLAGS += -fsingle-precision-constant
endif

# Configure linker include dir for ram/rom isr support
ifeq ($(MICROPY_HW_ENABLE_ISR_UART_FLASH_FUNCS_IN_RAM),1)
CFLAGS += -DMICROPY_HW_ENABLE_ISR_UART_FLASH_FUNCS_IN_RAM=1
LD_ISR_DIR = boards/common_isr_ram
else
LD_ISR_DIR = boards/common_isr_rom
endif

LDFLAGS += -nostdlib -L $(LD_DIR) -L $(LD_ISR_DIR) $(addprefix -T,$(LD_FILES)) -Wl,-Map=$(@:.elf=.map) -Wl,--cref
LDFLAGS += -Wl,--defsym=_estack_reserve=8
LIBS += "$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)"

# Remove uncalled code from the final image.
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,--gc-sections

ifeq ($(LTO),1)
CFLAGS += -flto=auto
# LTO requires passing compiler flags to the linker as it will run the assembler.
# To avoid risk of missing something relevant, pass all flags except for preprocessor args
LDFLAGS += $(filter-out -I%,$(filter-out -D%,$(CFLAGS)))

$(BUILD)/stm32_it.o $(BUILD)/pendsv.o: CFLAGS += -fno-lto
endif

# Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g -DPENDSV_DEBUG
COPT ?= -Og
# Disable text compression in debug builds
MICROPY_ROM_TEXT_COMPRESSION = 0
else
ifneq ($(LTO), 1)
CFLAGS += -g  # always include debug info in the ELF, unless LTO is on
endif
COPT ?= -Os -DNDEBUG
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -Wmissing-prototypes -Wold-style-definition -std=gnu99,$(CFLAGS))

# TODO make this common -- shouldn't be using these "private" vars from py.mk
ifneq ($(SRC_CXX)$(SRC_USERMOD_CXX)$(SRC_USERMOD_LIB_CXX),)
LIBSTDCPP_FILE_NAME = "$(shell $(CXX) $(CXXFLAGS) -print-file-name=libstdc++.a)"
LDFLAGS += -L"$(shell dirname $(LIBSTDCPP_FILE_NAME))"
endif

# Options for mpy-cross
MPY_CROSS_FLAGS += -march=$(MPY_CROSS_MCU_ARCH_$(MCU_SERIES))

SHARED_SRC_C += $(addprefix shared/,\
	libc/string0.c \
	netutils/dhcpserver.c \
	netutils/netutils.c \
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
	)

ifeq ($(MICROPY_FLOAT_IMPL),double)
LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_C)
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

DRIVERS_SRC_C += $(addprefix drivers/,\
	bus/softspi.c \
	bus/softqspi.c \
	memory/spiflash.c \
	dht/dht.c \
	)

SRC_C += \
	boardctrl.c \
	main.c \
	stm32_it.c \
	usbd_conf.c \
	usbd_desc.c \
	usbd_cdc_interface.c \
	usbd_hid_interface.c \
	usbd_msc_interface.c \
	mphalport.c \
	mpnetworkport.c \
	mpthreadport.c \
	irq.c \
	pendsv.c \
	systick.c  \
	powerctrl.c \
	powerctrlboot.c \
	rfcore.c \
	pybthread.c \
	factoryreset.c \
	timer.c \
	led.c \
	pin.c \
	pin_defs_stm32.c \
	pin_named_pins.c \
	bufhelper.c \
	dma.c \
	i2c.c \
	pyb_i2c.c \
	spi.c \
	pyb_spi.c \
	qspi.c \
	octospi.c \
	uart.c \
	ulpi.c \
	can.c \
	fdcan.c \
	pyb_can.c \
	usb.c \
	eth.c \
	eth_phy.c \
	gccollect.c \
	help.c \
	machine_bitstream.c \
	machine_i2c.c \
	machine_spi.c \
	modpyb.c \
	modstm.c \
	network_lan.c \
	extint.c \
	usrsw.c \
	rng.c \
	rtc.c \
	flash.c \
	flashbdev.c \
	spibdev.c \
	storage.c \
	sdcard.c \
	sdram.c \
	vfs_rom_ioctl.c \
	fatfs_port.c \
	lcd.c \
	accel.c \
	servo.c \
	dac.c \
	adc.c \
	sdio.c \
	subghz.c \
	$(wildcard $(BOARD_DIR)/*.c)

SRC_O += \
	$(STARTUP_FILE) \
	$(SYSTEM_FILE)

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f0 g0 l0))
CSUPEROPT = -Os # save some code space
SRC_O += \
	resethandler_m0.o \
	shared/runtime/gchelper_thumb1.o
else ifeq ($(MCU_SERIES),l1)
CFLAGS += -DUSE_HAL_DRIVER
SRC_O += \
	resethandler_m3.o \
	shared/runtime/gchelper_thumb2.o
else
SRC_O += \
	system_stm32.o \
	resethandler.o \
	shared/runtime/gchelper_thumb2.o
endif

HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal.c \
	hal_adc.c \
	hal_adc_ex.c \
	hal_cortex.c \
	hal_dma.c \
	hal_flash.c \
	hal_flash_ex.c \
	hal_gpio.c \
	hal_i2c.c \
	hal_pwr.c \
	hal_pwr_ex.c \
	hal_rcc.c \
	hal_rcc_ex.c \
	hal_rtc.c \
	hal_rtc_ex.c \
	hal_spi.c \
	hal_tim.c \
	hal_tim_ex.c \
	hal_uart.c \
	ll_rcc.c \
	ll_utils.c \
	)

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f4 f7 g0 g4 h5 h7 l0 l1 l4 wb))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal_pcd.c \
	hal_pcd_ex.c \
	ll_usb.c \
	)
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f4 f7 h5 h7 l4))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal_sd.c \
	ll_sdmmc.c \
	ll_fmc.c \
	)
ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),h5))
# HAL H5-1.0.0 has a bug with uninitialised variable in HAL_SD_ConfigWideBusOperation.
$(BUILD)/$(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_hal_sd.o: CFLAGS += -Wno-error=maybe-uninitialized
endif
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f4 f7 h7))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal_mmc.c \
	hal_sdram.c \
	hal_dcmi.c \
	)
ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f4))
# HAL F4-1.16.0 has a bug with missing parentheses in HAL_MMC_Erase.
# This function is unused so let the error go by as a warning.
$(BUILD)/$(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_hal_mmc.o: CFLAGS += -Wno-error=parentheses
endif
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f4 f7 g0 g4 h5 h7))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal_dma_ex.c \
	)
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),h5))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal_icache.c \
	)
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f0 f4 f7))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_, hal_can.c)
else ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),g0 g4 h7))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_, hal_fdcan.c)
else ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),l4))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/Legacy/stm32$(MCU_SERIES)xx_, hal_can.c)
$(BUILD)/$(STM32LIB_HAL_BASE)/Src/Legacy/stm32$(MCU_SERIES)xx_hal_can.o: CFLAGS += -Wno-error=cpp
endif

ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),STM32H747xx))
    HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_, hal_hsem.c)
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f0 f4 f7 g0 l0))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal_i2s.c \
	)
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f4))
HAL_SRC_C += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal_i2s_ex.c \
	)
endif

USBDEV_SRC_C += $(addprefix $(USBDEV_DIR)/,\
	core/src/usbd_core.c \
	core/src/usbd_ctlreq.c \
	core/src/usbd_ioreq.c \
	class/src/usbd_cdc_msc_hid.c \
	class/src/usbd_msc_bot.c \
	class/src/usbd_msc_scsi.c \
	)

ifeq ($(MICROPY_SSL_MBEDTLS),1)
LIB_SRC_C += mbedtls/mbedtls_port.c
endif

ifeq ($(MICROPY_PY_BLUETOOTH),1)
SRC_C += mpbthciport.c

ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)
SRC_C += mpnimbleport.c
endif

ifeq ($(MICROPY_BLUETOOTH_BTSTACK),1)
SRC_C += mpbtstackport.c
MICROPY_BLUETOOTH_BTSTACK_H4 ?= 1
endif

endif # MICROPY_PY_BLUETOOTH

# Add stm32-specific implementation of libmetal (and optionally OpenAMP's rproc).
# Note: libmetal code is generated via a pre-processor so ensure that runs first.
ifeq ($(MICROPY_PY_OPENAMP),1)
SRC_C += mpmetalport.c
$(BUILD)/mpmetalport.o: $(BUILD)/openamp/metal/config.h
ifeq ($(MICROPY_PY_OPENAMP_REMOTEPROC),1)
SRC_C += mpremoteprocport.c
$(BUILD)/mpremoteprocport.o: $(BUILD)/openamp/metal/config.h
endif
endif

# SRC_O should be placed first to work around this LTO bug with binutils <2.35:
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83967
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(HAL_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(USBDEV_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(GEN_PINS_SRC:.c=.o)

# This file contains performance critical functions so turn up the optimisation
# level.  It doesn't add much to the code size and improves performance a bit.
# Don't use -O3 with this file because gcc tries to optimise memset in terms of itself.
$(BUILD)/shared/libc/string0.o: COPT += -O2

ifneq ($(MICROPY_HW_ENABLE_ISR_UART_FLASH_FUNCS_IN_RAM),1)
# If MBOOT or RAM_IFS is not used we put several files into the first 16K section with the ISRs.
# If we compile these using -O0 then it won't fit. So if you really want these
# to be compiled with -O0, then edit boards/common.ld (in the .isr_vector section)
# and comment out the following lines.
$(BUILD)/$(OOFATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os
$(PY_BUILD)/formatfloat.o: COPT += -Os
$(PY_BUILD)/parsenum.o: COPT += -Os
$(PY_BUILD)/mpprint.o: COPT += -Os
endif

all: $(TOP)/lib/stm32lib/README.md all_main $(BUILD)/firmware.hex

ifeq ($(MBOOT_ENABLE_PACKING),1)
all_main: $(BUILD)/firmware.pack.dfu
else
all_main: $(BUILD)/firmware.dfu
endif

# For convenience, automatically fetch required submodules if they don't exist
$(TOP)/lib/stm32lib/README.md:
	$(ECHO) "stm32lib submodule not found, fetching it now..."
	(cd $(TOP) && git submodule update --init lib/stm32lib)

define RUN_DFU
	$(ECHO) "Writing $(1) to the board"
	$(if $(filter $(USE_PYDFU),1),\
	$(Q)$(PYTHON) $(PYDFU) --vid $(BOOTLOADER_DFU_USB_VID) --pid $(BOOTLOADER_DFU_USB_PID) -u $(1),
	$(Q)$(DFU_UTIL) -a 0 -d $(BOOTLOADER_DFU_USB_VID):$(BOOTLOADER_DFU_USB_PID) -D $(1) -s :leave)
endef

define RUN_STLINK
	$(ECHO) "Writing $(1) to the board via ST-LINK"
	$(Q)$(STFLASH) write $(1) $(2)
endef

define RUN_OPENOCD
	$(ECHO) "Writing $(1) to the board via ST-LINK using OpenOCD"
	$(Q)$(OPENOCD) -f $(OPENOCD_CONFIG) -c "stm_flash $(1) $(2) $(3) $(4)"
endef

define GENERATE_ELF
	$(ECHO) "LINK $(1)"
	$(Q)$(CC) $(LDFLAGS) -o $(1) $(2) $(LDFLAGS_MOD) $(LIBS)
	$(Q)$(SIZE) $(1)
	$(if $(filter-out $(TEXT0_ADDR),0x08000000), \
		$(ECHO) "INFO: this build requires mboot to be installed first")
	$(if $(filter $(TEXT1_ADDR),0x90000000), \
		$(ECHO) "INFO: this build places firmware in external QSPI flash")
endef

define GENERATE_BIN
	$(ECHO) "GEN $(1)"
	$(Q)$(OBJCOPY) -O binary $(addprefix -j ,$(3)) $(2) $(1)
endef

define GENERATE_DFU
	$(ECHO) "GEN $(1)"
	$(Q)$(PYTHON) $(DFU) \
	-D $(BOOTLOADER_DFU_USB_VID):$(BOOTLOADER_DFU_USB_PID) \
	$(if $(2),$(addprefix -b ,$(3):$(2))) \
	$(if $(4),$(addprefix -b ,$(5):$(4))) \
	$(1)
endef

define GENERATE_PACK_DFU
	$(ECHO) "GEN $(1)"
	$(Q)$(PYTHON) $(MBOOT_PACK_DFU) --keys $(MBOOT_PACK_KEYS_FILE) pack-dfu --gzip $(MBOOT_PACK_CHUNKSIZE) $(2) $(1)
endef

define GENERATE_HEX
	$(ECHO) "GEN $(1)"
	$(Q)$(OBJCOPY) -O ihex $(2) $(1)
endef

.PHONY: deploy deploy-stlink deploy-openocd

ifeq ($(MBOOT_ENABLE_PACKING),1)
deploy: $(BUILD)/firmware.pack.dfu
	$(call RUN_DFU,$^)
else
deploy: $(BUILD)/firmware.dfu
	$(call RUN_DFU,$^)
endif

# A board should specify TEXT0_ADDR if to use a different location than the
# default for the firmware memory location.  A board can also optionally define
# TEXT1_ADDR to split the firmware into two sections; see below for details.
TEXT0_ADDR ?= 0x08000000

ifeq ($(TEXT1_ADDR),)
# No TEXT1_ADDR given so put all firmware at TEXT0_ADDR location

TEXT0_SECTIONS ?= .isr_vector .isr_extratext .text .data .ARM

deploy-stlink: $(BUILD)/firmware.bin
	$(call RUN_STLINK,$^,$(TEXT0_ADDR))

deploy-openocd: $(BUILD)/firmware.bin
	$(call RUN_OPENOCD,$^,$(TEXT0_ADDR))

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(call GENERATE_BIN,$@,$^,$(TEXT0_SECTIONS))

$(BUILD)/firmware.dfu: $(BUILD)/firmware.bin
	$(call GENERATE_DFU,$@,$^,$(TEXT0_ADDR))

else
# TEXT0_ADDR and TEXT1_ADDR are specified so split firmware between these locations

TEXT0_SECTIONS ?= .isr_vector .isr_extratext
TEXT1_SECTIONS ?= .text .data .ARM

deploy-stlink: $(BUILD)/firmware0.bin $(BUILD)/firmware1.bin
	$(call RUN_STLINK,$(word 1,$^),$(TEXT0_ADDR))
	$(call RUN_STLINK,$(word 2,$^),$(TEXT1_ADDR))

deploy-openocd: $(BUILD)/firmware0.bin $(BUILD)/firmware1.bin
	$(call RUN_OPENOCD,$(word 1,$^),$(TEXT0_ADDR),$(word 2,$^),$(TEXT1_ADDR))

$(BUILD)/firmware0.bin: $(BUILD)/firmware.elf
	$(call GENERATE_BIN,$@,$^,$(TEXT0_SECTIONS))

$(BUILD)/firmware1.bin: $(BUILD)/firmware.elf
	$(call GENERATE_BIN,$@,$^,$(TEXT1_SECTIONS))

$(BUILD)/firmware.dfu: $(BUILD)/firmware0.bin $(BUILD)/firmware1.bin
	$(call GENERATE_DFU,$@,$(word 1,$^),$(TEXT0_ADDR),$(word 2,$^),$(TEXT1_ADDR))
endif

$(BUILD)/firmware.pack.dfu: $(BUILD)/firmware.dfu $(MBOOT_PACK_KEYS_FILE)
	$(call GENERATE_PACK_DFU,$@,$<)

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(call GENERATE_HEX,$@,$^)

$(BUILD)/firmware.elf: $(OBJ)
	$(call GENERATE_ELF,$@,$^)

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_CXX) $(SHARED_SRC_C) $(GEN_PINS_SRC)

# Making OBJ use an order-only dependency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(GEN_PINS_HDR)

# With conditional pins, we may need to regenerate qstrdefs.h when config
# options change.
$(HEADER_BUILD)/qstrdefs.generated.h: $(BOARD_DIR)/mpconfigboard.h

# main.c can't be even preprocessed without $(GEN_CDCINF_HEADER)
# As main.c is in SRC_QSTR this will also ensure that GEN_CDCINF_HEADER
# will be run before QSTR extraction.
main.c: $(GEN_CDCINF_HEADER)

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_$(BOARD).c and pins.h
$(BUILD)/%_$(BOARD).c $(HEADER_BUILD)/%.h $(HEADER_BUILD)/%_af_const.h $(HEADER_BUILD)/%_af_defs.h: $(BOARD_DIR)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board-csv $(BOARD_PINS) --af-csv $(AF_FILE) --prefix $(PREFIX_FILE) \
	    --output-source $(GEN_PINS_SRC) --output-header $(GEN_PINS_HDR) \
		--output-af-const $(GEN_PINS_AF_CONST) --output-af-defs $(GEN_PINS_AF_DEFS)

$(BUILD)/powerctrl.o: $(GEN_PLLFREQTABLE_HDR)
$(GEN_PLLFREQTABLE_HDR): $(PLLVALUES) | $(HEADER_BUILD)/qstr.i.last
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PLLVALUES) -c -m $(CMSIS_MCU_LOWER) file:$(HEADER_BUILD)/qstr.i.last > $@

$(BUILD)/extmod/machine_i2s.o: $(GEN_PLLI2STABLE_HDR)
$(GEN_PLLI2STABLE_HDR): $(PLLI2SVALUES) | $(HEADER_BUILD)/qstr.i.last
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(PLLI2SVALUES) -c -m $(CMSIS_MCU_LOWER) file:$(HEADER_BUILD)/qstr.i.last > $@

$(BUILD)/modstm.o: $(GEN_STMCONST_HDR)
$(HEADER_BUILD)/modstm_const.h: $(CMSIS_MCU_HDR) make-stmconst.py | $(HEADER_BUILD)
	$(ECHO) "GEN stmconst $@"
	$(Q)$(PYTHON) make-stmconst.py --mpz $(GEN_STMCONST_MPZ) $(CMSIS_MCU_HDR) > $(GEN_STMCONST_HDR)

$(GEN_CDCINF_HEADER): $(GEN_CDCINF_FILE) $(FILE2H) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(FILE2H) $< > $@

$(GEN_CDCINF_FILE): $(CDCINF_TEMPLATE) $(INSERT_USB_IDS) $(USB_IDS_FILE) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(INSERT_USB_IDS) $(USB_IDS_FILE) $< > $@

include $(TOP)/py/mkrules.mk
