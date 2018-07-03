# Select the board to build for: if not given on the command line,
# then default to PYBV10.
BOARD ?= PYBV10
ifeq ($(wildcard ../boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

include ../../../py/mkenv.mk
include ../boards/$(BOARD)/mpconfigboard.mk

CMSIS_DIR=$(TOP)/lib/stm32lib/CMSIS/STM32$(MCU_SERIES_UPPER)xx/Include
MCU_SERIES_UPPER = $(shell echo $(MCU_SERIES) | tr '[:lower:]' '[:upper:]')
HAL_DIR=lib/stm32lib/STM32$(MCU_SERIES_UPPER)xx_HAL_Driver
USBDEV_DIR=usbdev
DFU=$(TOP)/tools/dfu.py
PYDFU ?= $(TOP)/tools/pydfu.py
DEVICE=0483:df11
STFLASH ?= st-flash
OPENOCD ?= openocd
OPENOCD_CONFIG ?= boards/openocd_stm32f4.cfg

CROSS_COMPILE = arm-none-eabi-

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
CFLAGS_MCU_l4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4

CFLAGS = $(INC) -Wall -Wpointer-arith -Werror -std=gnu99 -nostdlib $(CFLAGS_MOD) $(CFLAGS_EXTRA)
CFLAGS += -D$(CMSIS_MCU)
CFLAGS += $(CFLAGS_MCU_$(MCU_SERIES))
CFLAGS += $(COPT)
CFLAGS += -I../boards/$(BOARD)
CFLAGS += -DSTM32_HAL_H='<stm32$(MCU_SERIES)xx_hal.h>'
CFLAGS += -DBOARD_$(BOARD)
CFLAGS += -DAPPLICATION_ADDR=$(TEXT0_ADDR)

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

SRC_LIB = $(addprefix lib/,\
	libc/string0.c \
	)

SRC_C = \
	main.c \
	drivers/bus/softspi.c \
	drivers/bus/softqspi.c \
	drivers/memory/spiflash.c \
	ports/stm32/i2cslave.c \
	ports/stm32/qspi.c \
	ports/stm32/flashbdev.c \
	ports/stm32/spibdev.c \
	ports/stm32/usbd_conf.c \
	$(patsubst $(TOP)/%,%,$(wildcard $(TOP)/ports/stm32/boards/$(BOARD)/*.c))

SRC_O = \
	ports/stm32/boards/startup_stm32$(MCU_SERIES).o \
	ports/stm32/resethandler.o \

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
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(SRC_HAL:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_USBDEV:.c=.o))

all: $(TOP)/lib/stm32lib/README.md $(BUILD)/firmware.dfu $(BUILD)/firmware.hex

# For convenience, automatically fetch required submodules if they don't exist
$(TOP)/lib/stm32lib/README.md:
	$(ECHO) "stm32lib submodule not found, fetching it now..."
	(cd $(TOP) && git submodule update --init lib/stm32lib)

.PHONY: deploy

deploy: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board"
	$(Q)$(PYTHON) $(PYDFU) -u $<

FLASH_ADDR = 0x08000000

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

$(BUILD)/main.o: $(BUILD)/genhdr/qstrdefs.generated.h

$(BUILD)/genhdr/qstrdefs.generated.h:
	$(MKDIR) -p $(BUILD)/genhdr
	$(Q)echo "// empty" > $@

-include $(OBJ:.o=.P)
