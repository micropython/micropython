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

# Set MBOOT_ENABLE_PACKING to 1 to enable DFU packing with encryption and signing.
# Ensure the MBOOT_PACK_xxx values match stm32/Makefile, to build matching application firmware.
MBOOT_ENABLE_PACKING ?= 0
MBOOT_PACK_CHUNKSIZE ?= 16384
MBOOT_PACK_KEYS_FILE ?= $(BOARD_DIR)/mboot_keys.h

# Sanity check that the board configuration directory exists
ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

# Enable BUILDING_MBOOT so boards can configure their .mk file accordingly.
BUILDING_MBOOT = 1

include ../../../py/mkenv.mk
include $(BOARD_DIR)/mpconfigboard.mk

# A board can set MBOOT_TEXT0_ADDR to a custom location where mboot should reside.
MBOOT_TEXT0_ADDR ?= 0x08000000

USBDEV_DIR=usbdev
DFU=$(TOP)/tools/dfu.py
PYDFU ?= $(TOP)/tools/pydfu.py
BOOTLOADER_DFU_USB_VID ?= 0x0483
BOOTLOADER_DFU_USB_PID ?= 0xDF11
STFLASH ?= st-flash
OPENOCD ?= openocd
OPENOCD_CONFIG ?= boards/openocd_stm32f4.cfg

include ../stm32.mk

CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I..
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(TOP)/lib/cmsis/inc
INC += -I$(STM32LIB_CMSIS_ABS)/Include
INC += -I$(STM32LIB_HAL_ABS)/Inc
INC += -I../$(USBDEV_DIR)/core/inc -I../$(USBDEV_DIR)/class/inc

# Standard C functions like memset need to be compiled with special flags so
# the compiler does not optimise these functions in terms of themselves.
CFLAGS_BUILTIN ?= -ffreestanding -fno-builtin -fno-lto

CFLAGS += $(INC) -Wall -Wpointer-arith -Wdouble-promotion -Wfloat-conversion -Werror -std=gnu99 -nostdlib $(CFLAGS_EXTRA)
CFLAGS += -D$(CMSIS_MCU)
CFLAGS += $(CFLAGS_MCU_$(MCU_SERIES))
CFLAGS += $(COPT)
CFLAGS += -I$(BOARD_DIR)
CFLAGS += -DSTM32_HAL_H='<stm32$(MCU_SERIES)xx_hal.h>'
CFLAGS += -DBOARD_$(BOARD)
CFLAGS += -DMBOOT_VTOR=$(MBOOT_TEXT0_ADDR)
CFLAGS += -DAPPLICATION_ADDR=$(TEXT0_ADDR)
CFLAGS += -DFFCONF_H=\"ports/stm32/mboot/ffconf.h\"
CFLAGS += -DLFS1_NO_MALLOC -DLFS1_NO_DEBUG -DLFS1_NO_WARN -DLFS1_NO_ERROR -DLFS1_NO_ASSERT
CFLAGS += -DLFS2_NO_MALLOC -DLFS2_NO_DEBUG -DLFS2_NO_WARN -DLFS2_NO_ERROR -DLFS2_NO_ASSERT -DLFS2_READONLY
CFLAGS += -DBUILDING_MBOOT=$(BUILDING_MBOOT)
CFLAGS += -DMICROPY_HW_STM32WB_FLASH_SYNCRONISATION=0
CFLAGS += -DBOOTLOADER_DFU_USB_VID=$(BOOTLOADER_DFU_USB_VID) -DBOOTLOADER_DFU_USB_PID=$(BOOTLOADER_DFU_USB_PID)

MBOOT_LD_FILES ?= stm32_memory.ld stm32_sections.ld
LDFLAGS += -nostdlib -L . $(addprefix -T,$(MBOOT_LD_FILES)) -Map=$(@:.elf=.map) --cref
LIBS += $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

# Remove uncalled code from the final image.
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += --gc-sections

# Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g -DPENDSV_DEBUG
COPT = -Og
else
COPT += -Os -DNDEBUG
endif

$(BUILD)/shared/libc/string0.o: CFLAGS += $(CFLAGS_BUILTIN)
LIB_SRC_C += \
	shared/libc/string0.c \
	lib/littlefs/lfs1.c \
	lib/littlefs/lfs1_util.c \
	lib/littlefs/lfs2.c \
	lib/littlefs/lfs2_util.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/ffunicode.c \
	lib/uzlib/adler32.c \
	lib/uzlib/crc32.c \
	lib/uzlib/tinfgzip.c \
	lib/uzlib/tinflate.c

SRC_C += \
	main.c \
	elem.c \
	fsload.c \
	gzstream.c \
	pack.c \
	sdcard.c \
	ui.c \
	vfs_fat.c \
	vfs_lfs.c \
	drivers/bus/softspi.c \
	drivers/bus/softqspi.c \
	drivers/memory/spiflash.c \
	ports/stm32/flash.c \
	ports/stm32/flashbdev.c \
	ports/stm32/i2cslave.c \
	ports/stm32/machine_adc.c \
	ports/stm32/powerctrlboot.c \
	ports/stm32/qspi.c \
	ports/stm32/spibdev.c \
	ports/stm32/usbd_conf.c \
	$(wildcard $(BOARD_DIR)/*.c)

SRC_O += \
	$(STARTUP_FILE) \
	$(SYSTEM_FILE) \
	ports/stm32/resethandler.o \

ifeq ($(MBOOT_ENABLE_PACKING), 1)

SRC_C += lib/libhydrogen/hydrogen.c

CFLAGS += -DMBOOT_ENABLE_PACKING=1 -DPARTICLE -DPLATFORM_ID=3
CFLAGS += -DMBOOT_PACK_CHUNKSIZE=$(MBOOT_PACK_CHUNKSIZE)
CFLAGS += -DMBOOT_PACK_KEYS_FILE=\"$(MBOOT_PACK_KEYS_FILE)\"
endif

$(BUILD)/$(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_ll_usb.o: CFLAGS += -Wno-attributes
SRC_HAL += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal_cortex.c \
	hal_dma.c \
	hal_flash.c \
	hal_flash_ex.c \
	hal_pcd.c \
	hal_pcd_ex.c \
	hal_rcc.c \
	hal_rcc_ex.c \
	ll_usb.c \
	)

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),f4 f7 h7))
SRC_HAL += $(addprefix $(STM32LIB_HAL_BASE)/Src/stm32$(MCU_SERIES)xx_,\
	hal_mmc.c \
	hal_sd.c \
	ll_sdmmc.c \
	)
endif

SRC_USBDEV += $(addprefix ports/stm32/$(USBDEV_DIR)/,\
	core/src/usbd_core.c \
	core/src/usbd_ctlreq.c \
	core/src/usbd_ioreq.c \
	)

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

deploy: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board"
	$(Q)$(PYTHON) $(PYDFU) -u $<

deploy-stlink: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board via ST-LINK"
	$(Q)$(STFLASH) write $(BUILD)/firmware.bin $(MBOOT_TEXT0_ADDR)

$(BUILD)/firmware.dfu: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin
	$(Q)$(PYTHON) $(DFU) -b $(MBOOT_TEXT0_ADDR):$(BUILD)/firmware.bin $@

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
GEN_ROOT_POINTERS = $(HEADER_BUILD)/root_pointers.h
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(HEADER_BUILD)/pins_qstr.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_DEFS = $(HEADER_BUILD)/pins_af_defs.h
GEN_PINS_AF_PY = $(BUILD)/pins_af.py

$(OBJ): $(GEN_QSTRDEFS_GENERATED) $(GEN_ROOT_POINTERS) $(GEN_PINS_AF_DEFS)

$(HEADER_BUILD):
	$(MKDIR) -p $(BUILD)/genhdr

$(GEN_QSTRDEFS_GENERATED): | $(HEADER_BUILD)
	$(Q)echo "// empty" > $@

$(GEN_ROOT_POINTERS): | $(HEADER_BUILD)
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
