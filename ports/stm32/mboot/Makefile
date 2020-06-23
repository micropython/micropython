# Select the board to build for: if not given on the command line,
# then default to PYBV10.
BOARD ?= PYBV10

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

# Allow the directory containing the board configuration to be specified
BOARD_DIR ?= $(abspath ../boards/$(BOARD))

# Set USE_MBOOT to 1 so that TEXT0_ADDR gets set properly for those boards
# that can be built with or without mboot.
USE_MBOOT ?= 1

# Sanity check that the board configuration directory exists
ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

include ../../../py/mkenv.mk
include $(BOARD_DIR)/mpconfigboard.mk

MCU_SERIES_UPPER = $(shell echo $(MCU_SERIES) | tr '[:lower:]' '[:upper:]')
CMSIS_MCU_LOWER = $(shell echo $(CMSIS_MCU) | tr '[:upper:]' '[:lower:]')

CMSIS_DIR=$(TOP)/lib/stm32lib/CMSIS/STM32$(MCU_SERIES_UPPER)xx/Include
HAL_DIR=lib/stm32lib/STM32$(MCU_SERIES_UPPER)xx_HAL_Driver
USBDEV_DIR=usbdev
DFU=$(TOP)/tools/dfu.py
PYDFU ?= $(TOP)/tools/pydfu.py
BOOTLOADER_DFU_USB_VID ?= 0x0483
BOOTLOADER_DFU_USB_PID ?= 0xDF11
STFLASH ?= st-flash
OPENOCD ?= openocd
OPENOCD_CONFIG ?= boards/openocd_stm32f4.cfg
STARTUP_FILE ?= lib/stm32lib/CMSIS/STM32$(MCU_SERIES_UPPER)xx/Source/Templates/gcc/startup_$(CMSIS_MCU_LOWER).o
SYSTEM_FILE ?= lib/stm32lib/CMSIS/STM32$(MCU_SERIES_UPPER)xx/Source/Templates/system_stm32$(MCU_SERIES)xx.o

CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I..
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(TOP)/lib/cmsis/inc
INC += -I$(CMSIS_DIR)/
INC += -I$(TOP)/$(HAL_DIR)/Inc
INC += -I../$(USBDEV_DIR)/core/inc -I../$(USBDEV_DIR)/class/inc

# Basic Cortex-M flags
CFLAGS_CORTEX_M = -mthumb

# Options for particular MCU series
CFLAGS_MCU_f4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_f7 = $(CFLAGS_CORTEX_M) -mtune=cortex-m7 -mcpu=cortex-m7
CFLAGS_MCU_h7 = $(CFLAGS_CORTEX_M) -mtune=cortex-m7 -mcpu=cortex-m7
CFLAGS_MCU_l4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_wb = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4

# Standard C functions like memset need to be compiled with special flags so
# the compiler does not optimise these functions in terms of themselves.
CFLAGS_BUILTIN ?= -ffreestanding -fno-builtin -fno-lto

CFLAGS = $(INC) -Wall -Wpointer-arith -Wdouble-promotion -Wfloat-conversion -Werror -std=gnu99 -nostdlib $(CFLAGS_MOD) $(CFLAGS_EXTRA)
CFLAGS += -D$(CMSIS_MCU)
CFLAGS += $(CFLAGS_MCU_$(MCU_SERIES))
CFLAGS += $(COPT)
CFLAGS += -I$(BOARD_DIR)
CFLAGS += -DSTM32_HAL_H='<stm32$(MCU_SERIES)xx_hal.h>'
CFLAGS += -DBOARD_$(BOARD)
CFLAGS += -DAPPLICATION_ADDR=$(TEXT0_ADDR)
CFLAGS += -DFFCONF_H=\"ports/stm32/mboot/ffconf.h\"
CFLAGS += -DBUILDING_MBOOT=1
CFLAGS += -DBOOTLOADER_DFU_USB_VID=$(BOOTLOADER_DFU_USB_VID) -DBOOTLOADER_DFU_USB_PID=$(BOOTLOADER_DFU_USB_PID)

LDFLAGS = -nostdlib -L . -T stm32_generic.ld -Map=$(@:.elf=.map) --cref
LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

# Remove uncalled code from the final image.
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += --gc-sections

# Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g -DPENDSV_DEBUG
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

$(BUILD)/lib/libc/string0.o: CFLAGS += $(CFLAGS_BUILTIN)
LIB_SRC_C = \
	lib/libc/string0.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/ffunicode.c \
	extmod/uzlib/crc32.c \
	extmod/uzlib/adler32.c \
	extmod/uzlib/tinflate.c \
	extmod/uzlib/tinfgzip.c

SRC_C = \
	main.c \
	elem.c \
	fsload.c \
	diskio.c \
	drivers/bus/softspi.c \
	drivers/bus/softqspi.c \
	drivers/memory/spiflash.c \
	ports/stm32/flash.c \
	ports/stm32/flashbdev.c \
	ports/stm32/i2cslave.c \
	ports/stm32/powerctrlboot.c \
	ports/stm32/qspi.c \
	ports/stm32/spibdev.c \
	ports/stm32/usbd_conf.c \
	$(wildcard $(BOARD_DIR)/*.c)

SRC_O = \
	$(STARTUP_FILE) \
	$(SYSTEM_FILE) \
	ports/stm32/resethandler.o \

$(BUILD)/$(HAL_DIR)/Src/stm32$(MCU_SERIES)xx_ll_usb.o: CFLAGS += -Wno-attributes
SRC_HAL = $(addprefix $(HAL_DIR)/Src/stm32$(MCU_SERIES)xx_,\
	hal_cortex.c \
	hal_flash.c \
	hal_flash_ex.c \
	hal_pcd.c \
	hal_pcd_ex.c \
	ll_usb.c \
	)

SRC_USBDEV = $(addprefix ports/stm32/$(USBDEV_DIR)/,\
	core/src/usbd_core.c \
	core/src/usbd_ctlreq.c \
	core/src/usbd_ioreq.c \
	)

OBJ =
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(SRC_HAL:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_USBDEV:.c=.o))

all: $(TOP)/lib/stm32lib/README.md $(BUILD)/firmware.dfu $(BUILD)/firmware.hex

# For convenience, automatically fetch required submodules if they don't exist
$(TOP)/lib/stm32lib/README.md:
	$(ECHO) "stm32lib submodule not found, fetching it now..."
	(cd $(TOP) && git submodule update --init lib/stm32lib)

.PHONY: deploy deploy-stlink

FLASH_ADDR = 0x08000000

deploy: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board"
	$(Q)$(PYTHON) $(PYDFU) -u $<

deploy-stlink: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board via ST-LINK"
	$(Q)$(STFLASH) write $(BUILD)/firmware.bin $(FLASH_ADDR)

$(BUILD)/firmware.dfu: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin
	$(Q)$(PYTHON) $(DFU) -b $(FLASH_ADDR):$(BUILD)/firmware.bin $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O ihex $< $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

#########################################
# Rules to generate header files

MAKE_PINS = ../boards/make-pins.py
PREFIX_FILE = ../boards/stm32f4xx_prefix.c
BOARD_PINS = $(BOARD_DIR)/pins.csv
HEADER_BUILD = $(BUILD)/genhdr
GEN_QSTRDEFS_GENERATED = $(HEADER_BUILD)/qstrdefs.generated.h
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(HEADER_BUILD)/pins_qstr.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_DEFS = $(HEADER_BUILD)/pins_af_defs.h
GEN_PINS_AF_PY = $(BUILD)/pins_af.py

$(OBJ): $(GEN_QSTRDEFS_GENERATED) $(GEN_PINS_AF_DEFS)

$(HEADER_BUILD):
	$(MKDIR) -p $(BUILD)/genhdr

$(GEN_QSTRDEFS_GENERATED): | $(HEADER_BUILD)
	$(Q)echo "// empty" > $@

$(GEN_PINS_AF_DEFS): $(BOARD_PINS) $(MAKE_PINS) ../$(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af ../$(AF_FILE) \
		--prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) \
		--af-const $(GEN_PINS_AF_CONST) --af-defs $(GEN_PINS_AF_DEFS) \
		--af-py $(GEN_PINS_AF_PY) > $(GEN_PINS_SRC)

#########################################

vpath %.S . $(TOP)
$(BUILD)/%.o: %.S
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

vpath %.s . $(TOP)
$(BUILD)/%.o: %.s
	$(ECHO) "AS $<"
	$(Q)$(AS) -o $@ $<

define compile_c
$(ECHO) "CC $<"
$(Q)$(CC) $(CFLAGS) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  $(RM) -f $(@:.o=.d)
endef

vpath %.c . $(TOP)
$(BUILD)/%.o: %.c
	$(call compile_c)

# $(sort $(var)) removes duplicates
#
# The net effect of this, is it causes the objects to depend on the
# object directories (but only for existence), and the object directories
# will be created if they don't exist.
OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	$(MKDIR) -p $@

clean:
	$(RM) -rf $(BUILD) $(CLEAN_EXTRA)
.PHONY: clean

###########################################

-include $(OBJ:.o=.P)
