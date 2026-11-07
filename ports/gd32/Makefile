# Location of top-level MicroPython directory
TOP := $(abspath $(CURDIR)/../..)

# ---- board selection (needed before mkenv/py.mk so BUILD can be derived) ----
BOARD ?= $(error BOARD not specified. Use BOARD=GD32F407 or BOARD=GD32F470)
BOARD_DIR = boards/$(BOARD)
ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error BOARD=$(BOARD) not found in boards/)
endif

# Keep build output separate per board, otherwise `build.sh all` reuses the
# previous board's object files and silently produces a wrong firmware.
BUILD ?= build-$(BOARD)

include $(TOP)/py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# Frozen bytecode: a manifest lists the Python modules compiled into Flash.
# This must be set before py.mk so that it appends frozen_content.o to PY_O.
# Pass FROZEN_MANIFEST= on the command line to disable freezing.
FROZEN_MANIFEST ?= manifest.py

include $(TOP)/py/py.mk

CROSS_COMPILE ?= arm-none-eabi-

include $(BOARD_DIR)/mpconfigboard.mk

# Feature switches consumed by extmod.mk (make variables, not C macros).
-include mpconfigport.mk

# ---- pin generation ----
GEN_PINS_PY  = gen_pins.py
BOARD_PINS   = $(BOARD_DIR)/pins.csv
GEN_PINS_DIR = $(BUILD)/genhdr
GEN_PINS_HDR = $(GEN_PINS_DIR)/pins.h
GEN_PINS_TBL = $(GEN_PINS_DIR)/pin_table.h


# ---- MCU / core flags ----
MCU_ARCH = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS += $(MCU_ARCH)

# lwIP configuration (consumed by extmod.mk)
MICROPY_PY_LWIP ?= 1
MICROPY_PY_SSL ?= 1
MICROPY_SSL_MBEDTLS ?= 1
MICROPY_PY_LWIP_REPEAT_ARP ?= 1
CFLAGS += -D$(CMSIS_MCU) -DUSE_STDPERIPH_DRIVER

# Flash wait states for the target core frequency (GD32F4xx datasheet).
# Must be visible to system_gd32f4xx.c, which does not include the board header.
ifeq ($(BOARD),GD32F407)
CFLAGS += -DMICROPY_HW_FLASH_WS=5
else ifeq ($(BOARD),GD32F470)
CFLAGS += -DMICROPY_HW_FLASH_WS=7
else
CFLAGS += -DMICROPY_HW_FLASH_WS=7
endif
CFLAGS += -Os -g -std=gnu99 -Wall -Werror
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-common -fno-strict-aliasing
CFLAGS += -DNDEBUG
CFLAGS += -fsingle-precision-constant -Wdouble-promotion

# ---- includes ----
INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(BOARD_DIR)
INC += -Igd32lib/CMSIS_include
INC += -Igd32lib/peripheral_include
INC += -I$(TOP)/lib/oofatfs
INC += -I$(TOP)/lib/tinyusb/src
INC += -I$(TOP)/shared/tinyusb
INC += -I$(TOP)/extmod/lwip-include
INC += -Ilwip_inc

# extmod.mk contributes lwIP sources plus its include paths, so it must run
# before CFLAGS picks up $(INC).  This port still lists its own extmod module
# sources explicitly in SRC_C.
include $(TOP)/extmod/extmod.mk

CFLAGS += $(INC)
CFLAGS += -DLFS2_NO_MALLOC=0

# ---- sources ----
SRC_C = \
	main.c \
	lib/littlefs/lfs2.c \
	lib/littlefs/lfs2_util.c \
	flashbdev.c \
	fatfs_port.c \
	sdcard.c \
	eth.c \
	network_lan.c \
	usbd.c \
	machine_sdcard_port.c \
	drivers/bus/softspi.c \
	drivers/dht/dht.c \
	shared/runtime/softtimer.c \
	shared/runtime/mpirq.c \
	shared/tinyusb/mp_usbd.c \
	shared/tinyusb/mp_usbd_cdc.c \
	shared/tinyusb/mp_usbd_descriptor.c \
	shared/tinyusb/mp_usbd_runtime.c \
	machine_pin.c \
	machine_bitstream_port.c \
	machine_spi_port.c \
	machine_i2c_port.c \
	machine_dac_port.c \
	machine_rtc_port.c \
	mphalport.c \
	mbedtls_port.c \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	shared/timeutils/timeutils.c \
	gd32lib/system_gd32f4xx.c \
	gd32lib/peripheral_source/gd32f4xx_gpio.c \
	gd32lib/peripheral_source/gd32f4xx_rcu.c \
	gd32lib/peripheral_source/gd32f4xx_usart.c \
	gd32lib/peripheral_source/gd32f4xx_spi.c \
	gd32lib/peripheral_source/gd32f4xx_i2c.c \
	gd32lib/peripheral_source/gd32f4xx_adc.c \
	gd32lib/peripheral_source/gd32f4xx_dac.c \
	gd32lib/peripheral_source/gd32f4xx_rtc.c \
	gd32lib/peripheral_source/gd32f4xx_can.c \
	gd32lib/peripheral_source/gd32f4xx_timer.c \
	gd32lib/peripheral_source/gd32f4xx_sdio.c \
	gd32lib/peripheral_source/gd32f4xx_dma.c \
	gd32lib/peripheral_source/gd32f4xx_enet.c \
	gd32lib/peripheral_source/gd32f4xx_syscfg.c \
	gd32lib/peripheral_source/gd32f4xx_misc.c \
	gd32lib/peripheral_source/gd32f4xx_fmc.c \
	gd32lib/peripheral_source/gd32f4xx_pmu.c \
	gd32lib/peripheral_source/gd32f4xx_fwdgt.c \
	gd32lib/peripheral_source/gd32f4xx_trng.c \

SRC_S = gd32lib/GCC/$(STARTUP_FILE)

# VFS sources must be scanned so MP_REGISTER_ROOT_POINTER entries
# (vfs_cur, vfs_mount_table) land in genhdr/root_pointers.h.
SRC_QSTR += extmod/vfs.c extmod/vfs_blockdev.c extmod/vfs_lfs.c extmod/vfs_reader.c flashbdev.c
SRC_QSTR += shared/readline/readline.c shared/runtime/pyexec.c machine_pin.c \
	extmod/modmachine.c extmod/machine_timer.c extmod/machine_wdt.c \
	extmod/machine_uart.c extmod/machine_spi.c extmod/machine_i2c.c machine_i2c_target_port.c machine_i2s_port.c extmod/machine_i2c_target.c extmod/machine_i2s.c machine_uart_port.c machine_spi_port.c machine_i2c_port.c machine_adc_port.c machine_dac_port.c machine_rtc_port.c machine_can_port.c machine_pwm_port.c extmod/machine_adc.c extmod/machine_can.c extmod/machine_pwm.c extmod/machine_signal.c extmod/machine_pinbase.c flashbdev.c sdcard.c machine_sdcard_port.c machine_wdt_port.c shared/runtime/mpirq.c

# TinyUSB (device stack for USBDevice / CDC) on the GD32F4 USBFS DWC2 core.
# tinyusb.mk contributes paths relative to lib/tinyusb/ (e.g. src/tusb.c).
# Reset first so a second inclusion cannot stack the prefix.
TINYUSB_SRC_C_REL :=
-include $(TOP)/lib/tinyusb/src/tinyusb.mk
TINYUSB_SRC_C_REL := $(TINYUSB_SRC_C)
TINYUSB_SRC_C := $(sort $(addprefix lib/tinyusb/, \
	$(TINYUSB_SRC_C_REL) \
	src/portable/synopsys/dwc2/dcd_dwc2.c \
	src/portable/synopsys/dwc2/dwc2_common.c \
	))
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_GD32F4
CFLAGS += -DCFG_TUSB_OS=OPT_OS_NONE
CFLAGS += -DCFG_TUD_ENABLED=1
CFLAGS += -I$(TOP)/lib/tinyusb/src
# TinyUSB is third-party code; keep its warnings from breaking -Werror.
$(BUILD)/lib/tinyusb/%.o: CFLAGS += -Wno-error

# TinyUSB objects must be added here (after TINYUSB_SRC_C is defined).
OBJ += $(addprefix $(BUILD)/, $(TINYUSB_SRC_C:.c=.o))

# PY_O covers the MicroPython core, the extmod.mk sources (lwIP, network,
# netutils, ...) and frozen_content.o when FROZEN_MANIFEST is set.
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.S=.o))

# ---- linker ----
LD_FILES = ld/$(LD_SCRIPT)
LDFLAGS += $(MCU_ARCH)
LDFLAGS += -T $(LD_FILES)
LDFLAGS += -Wl,-Map=$@.map,--cref -Wl,--gc-sections
LDFLAGS += -Wl,--print-memory-usage
LDFLAGS += -Wl,--defsym=__stack_size=0x2000
LIBS += --specs=nano.specs --specs=nosys.specs -lm -lc -lgcc

# ---- targets ----
all: $(BUILD)/firmware.elf $(BUILD)/firmware.bin $(BUILD)/firmware-isr.bin $(BUILD)/firmware.hex

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

# The vector table lives in Sector 0 and the code starts after the filesystem
# partition, so a plain `objcopy -O binary` would pad the 48 KB filesystem hole
# with 0xFF and produce an image that must not be flashed as one piece.
# Emit the two halves separately and keep a HEX for anything that can handle
# gaps (OpenOCD/st-flash both honour the addresses in it).
$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "BIN $@ (code, flash at 0x08010000)"
	$(Q)$(OBJCOPY) -O binary -j .text -j .rodata -j .ARM.extab -j .ARM -j .ARM.attributes -j .preinit_array -j .init_array -j .fini_array -j .data $< $@

$(BUILD)/firmware-isr.bin: $(BUILD)/firmware.elf
	$(ECHO) "BIN $@ (vectors, flash at 0x08000000)"
	$(Q)$(OBJCOPY) -O binary -j .vectors $< $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(ECHO) "HEX $@"
	$(Q)$(OBJCOPY) -O ihex $< $@


# ---- generated pin tables ----
$(GEN_PINS_HDR) $(GEN_PINS_TBL): $(BOARD_PINS) $(GEN_PINS_PY)
	$(ECHO) "GEN pins for $(BOARD)"
	$(Q)mkdir -p $(GEN_PINS_DIR)
	$(Q)$(PYTHON) $(GEN_PINS_PY) $(BOARD_PINS) $(GEN_PINS_DIR) $(BOARD)

# ensure objects wait for generated headers
$(OBJ): $(GEN_PINS_HDR) $(GEN_PINS_TBL)

clean:
	$(RM) -rf $(BUILD)

.PHONY: all clean


# ---- export artifacts per board ----
DIST_DIR = dist/$(BOARD)

dist: all
	$(ECHO) "Export $@ artifacts"
	$(Q)mkdir -p $(DIST_DIR)
	$(Q)cp $(BUILD)/firmware.elf $(BUILD)/firmware.bin $(BUILD)/firmware-isr.bin $(BUILD)/firmware.hex $(DIST_DIR)/ 2>/dev/null || true
	$(Q)ls -lh $(DIST_DIR)

.PHONY: dist


# List of sources for qstr extraction.  This port lists its sources by hand
# instead of including extmod/extmod.mk, so the automatic
# SRC_QSTR += $(SRC_EXTMOD_C) never runs and must be replaced here.
SRC_QSTR += $(SRC_C)

# littlefs is third-party code: relax warnings for it
$(BUILD)/lib/littlefs/%.o: CFLAGS += -Wno-error


include $(TOP)/py/mkrules.mk
