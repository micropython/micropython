# Determine which board we are compiling for...
ifdef BOARD_DIR
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
BOARD ?= FRDM_MCXN947
BOARD_DIR ?= boards/$(BOARD)
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
	$(error Invalid BOARD specified: $(BOARD_DIR))
endif

BUILD ?= build-$(BOARD)
GIT_SUBMODULES += lib/nxp_driver

include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

include $(BOARD_DIR)/mpconfigboard.mk

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# File containing description of content to be frozen into firmware.
FROZEN_MANIFEST ?= boards/manifest.py

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

include mcx.mk

CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS += $(INC) -Wall -Werror -std=gnu99 -nostdlib $(TARGET_CFLAGS_CPU) $(COPT)

CFLAGS_BUILTIN ?= -ffreestanding -fno-builtin -fno-lto

ASFLAGS += $(TARGET_CFLAGS_CPU)

# Configure floating point support
ifeq ($(MICROPY_FLOAT_IMPL),single)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
CFLAGS += -fsingle-precision-constant
else ifeq ($(MICROPY_FLOAT_IMPL),none)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_NONE
endif

LDFLAGS += -nostdlib \
	$(addprefix -T,devices/$(TARGET_MCU_SERIES)/$(TARGET_MCU_VARIANT)_flash.ld) \
	-Wl,-Map=$(@:.elf=.map) -Wl,--cref -Wl,--gc-sections

ifneq ($(TARGET_STACK_SIZE),)
LDFLAGS += -Wl,--defsym=__stack_size__=$(TARGET_STACK_SIZE)
endif

ifneq ($(TARGET_HEAP_SIZE),)
LDFLAGS += -Wl,--defsym=__heap_size__=$(TARGET_HEAP_SIZE)
endif

CSUPEROPT = -Os # save some code space

# PIN Generator
AF_FILE = $(TARGET_MCU_AF_FILE)
MAKE_PINS = boards/make-pins.py
PREFIX_FILE = boards/mcx_pin_prefix.c
BOARD_PINS = $(BOARD_DIR)/pins.csv
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h

# SDK
SDK_DIR = lib/nxp_driver/sdk
MCUX_DIR = $(SDK_DIR)/devices/$(TARGET_MCU_SERIES)

TARGET_STARTUP_S = devices/$(TARGET_MCU_SERIES)/startup_$(TARGET_MCU_VARIANT)
TARGET_SYSTEM_C = devices/$(TARGET_MCU_SERIES)/system_$(TARGET_MCU_VARIANT)

# Tune for Debugging or Optimization
CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -DDEBUG
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Hardware & SDK C defines
CFLAGS += \
	-D__START=main \
	-D$(TARGET_MCU_DEF) \
	-DMCUXPRESSO_SDK \
	-D__STARTUP_CLEAR_BSS

# Hardware & SDK C includes
CFLAGS += \
	-I$(BOARD_DIR) \
	-I$(TOP)/$(SDK_DIR)/CMSIS/Include \
	-I$(TOP)/$(MCUX_DIR) \
	-I$(TOP)/$(MCUX_DIR)/drivers \
	-I$(TOP)/$(MCUX_DIR)/drivers/romapi/flash \


# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))

LIBS = \
	"$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)"

SRC_C = \
	drv_adc.c \
	drv_i2c.c \
	drv_iflash.c \
	drv_pin.c \
	drv_pwm.c \
	drv_spi.c \
	drv_uart.c \
	hal_pin.c \
	hal_stdio.c \
	hal_systick.c \
	machine_i2c.c \
	machine_pin.c \
	machine_spi.c \
	main.c \
	mcx_flash.c \
	modmcx.c \
	mphalport.c \
	pendsv.c \
	tusb_port.c \

SRC_BOARD_C = $(addprefix $(BOARD_DIR)/, \
	board.c \
	clock_config.c \
	interrupts.c \
	pin_mux.c \
	)

SRC_SDK_C = $(addprefix $(MCUX_DIR)/, \
	drivers/romapi/flash/src/fsl_flash.c \
	drivers/fsl_clock.c \
	drivers/fsl_common.c \
	drivers/fsl_common_arm.c \
	drivers/fsl_ctimer.c \
	drivers/fsl_gpio.c \
	drivers/fsl_lpadc.c \
	drivers/fsl_lpi2c.c \
	drivers/fsl_lpspi.c \
	drivers/fsl_lpuart.c \
	drivers/fsl_power.c \
	drivers/fsl_reset.c \
	drivers/fsl_spc.c \
	drivers/fsl_vref.c \
	)

SRC_DRIVERS_C = $(addprefix drivers/, \
	bus/softspi.c \
	bus/softqspi.c \
	memory/spiflash.c \
	dht/dht.c \
	)

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
	lib/tinyusb/src/tusb.c

CFLAGS += \
	-I$(TOP)/lib/tinyusb/hw \
	-I$(TOP)/lib/tinyusb/hw/bsp/mcx \
	-I$(TOP)/lib/tinyusb/src \

SRC_C += $(TARGET_SYSTEM_C).c
SRC_O += $(TARGET_STARTUP_S).o

ifeq ($(TARGET_MCU_HAS_LPFLEXCOMM), 1)
SRC_SDK_C += $(MCUX_DIR)/drivers/fsl_lpflexcomm.c
CFLAGS += -DMICROPY_HW_HAS_LPFLEXCOMM
endif

ifeq ($(TARGET_MCU_HAS_OSTIMER), 1)
SRC_SDK_C += $(MCUX_DIR)/drivers/fsl_ostimer.c
CFLAGS += -DMICROPY_HW_HAS_OSTIMER
endif

ifeq ($(TARGET_MCU_HAS_USB_HS), 1)
SRC_TINYUSB_C += lib/tinyusb/src/portable/chipidea/ci_hs/dcd_ci_hs.c
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_MCXN9
CFLAGS += -DMICROPY_HW_HAS_USB_HS
endif

ifeq ($(TARGET_MCU_HAS_FLEXSPI), 1)
SRC_SDK_C += $(MCUX_DIR)/drivers/fsl_flexspi.c
CFLAGS += -DMICROPY_HW_HAS_FLEXSPI
endif

SRC_SHARED_C = $(addprefix shared/,\
	libc/printf.c \
	readline/readline.c \
	runtime/interrupt_char.c \
	runtime/gchelper_native.c \
	runtime/mpirq.c \
	runtime/pyexec.c \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	)


$(BUILD)/shared/libc/string0.o: CFLAGS += $(CFLAGS_BUILTIN)

# LibC sources
SRC_LIBC_C = \
	shared/libc/string0.c

# LibM sources
SRC_LIBM_C = $(addprefix lib/libm/,\
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
SRC_LIBM_C += lib/libm/thumb_vfp_sqrtf.c
else
SRC_LIBM_C += lib/libm/ef_sqrt.c
endif

SRC_O += \
	shared/runtime/gchelper_thumb2.o

SRC_QSTR += $(SRC_C) $(SRC_SHARED_C) $(GEN_PINS_SRC)

# Making OBJ use an order-only dependency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(GEN_PINS_HDR)

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_BOARD_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SDK_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_DRIVERS_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_TINYUSB_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM_C:.c=.o))
OBJ += $(GEN_PINS_SRC:.c=.o)

all: $(BUILD)/firmware.hex $(BUILD)/firmware.bin

# Need correct CPU flags to assemble this file.
$(BUILD)/shared/runtime/gchelper_thumb2.o: $(TOP)/shared/runtime/gchelper_thumb2.s
	$(ECHO) "AS $<"
	$(Q)$(AS) $(ASFLAGS) -o $@ $<

# Startup file requires CPP support.
$(BUILD)/$(TARGET_STARTUP_S).o: $(TARGET_STARTUP_S).S
	$(ECHO) "AS $<"
	$(Q)$(CC) $(CFLAGS) -c -xassembler-with-cpp -o $@ $<

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(ECHO) "COPY $@"
	$(Q)$(OBJCOPY) -Oihex $^ $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "COPY $@"
	$(Q)$(OBJCOPY) -Obinary $^ $@

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_$(BOARD).c and pins.h
$(BUILD)/%_$(BOARD).c $(HEADER_BUILD)/%.h: $(BOARD_PINS) $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board-csv $(BOARD_PINS) --af-csv $(AF_FILE) --prefix $(PREFIX_FILE) \
		--output-source $(GEN_PINS_SRC) --output-header $(GEN_PINS_HDR)

include $(TOP)/py/mkrules.mk
