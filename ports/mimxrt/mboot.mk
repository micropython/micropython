BUILD_BL ?= build-$(BOARD)/mboot
LD_FILES_BL = $(BOOTLOADER_DIR)/boards/common.ld

INC_BL += \
	-I$(BOARD_DIR) \
	-I$(BOOTLOADER_DIR) \
	-I$(BOOTLOADER_DIR)/boards \
	-I$(BOOTLOADER_DIR)/hal \
	-I$(BOOTLOADER_DIR)/shared \
	-I$(TOP)/$(MCU_DIR) \
	-I$(TOP)/$(MCU_DIR)/drivers \
	-I$(TOP)/lib/cmsis/inc \
	-I$(TOP)/lib/tinyusb/hw \
	-I$(TOP)/lib/tinyusb/src \
	-I. \
	-Ihal

SRC_HAL_IMX_C_BL += \
	$(MCU_DIR)/drivers/fsl_cache.c \
	$(MCU_DIR)/drivers/fsl_clock.c \
	$(MCU_DIR)/drivers/fsl_common.c \
	$(MCU_DIR)/drivers/fsl_flexram.c \
	$(MCU_DIR)/drivers/fsl_flexspi.c \
	$(MCU_DIR)/drivers/fsl_gpt.c \
	$(MCU_DIR)/system_$(MCU_SERIES).c

SRC_TINYUSB_C_BL += \
	lib/tinyusb/src/class/dfu/dfu_device.c \
	lib/tinyusb/src/common/tusb_fifo.c \
	lib/tinyusb/src/device/usbd.c \
	lib/tinyusb/src/device/usbd_control.c \
	lib/tinyusb/src/portable/nxp/transdimension/dcd_transdimension.c \
	lib/tinyusb/src/tusb.c

SRC_C_BL += \
	$(BOOTLOADER_DIR)/boards/flexspi_nor_mboot.c \
	$(BOOTLOADER_DIR)/main.c \
	$(BOOTLOADER_DIR)/mboot_buffer.c \
	$(BOOTLOADER_DIR)/mboot_dfu.c \
	$(BOOTLOADER_DIR)/mboot_upgrade.c \
	$(BOOTLOADER_DIR)/mboot_utils.c \
	$(BOOTLOADER_DIR)/mboot_validate.c \
	$(BOOTLOADER_DIR)/tusb_port.c \
	$(SRC_HAL_IMX_C_BL) \
	$(SRC_TINYUSB_C_BL) \
	boards/$(MCU_SERIES)_clock_config.c \
	flash.c

# Set default values if optional variables not defined
ifndef MICROPY_HW_BOARD_FLASH_FILES
    MICROPY_HW_BOARD_FLASH_FILES = 0
endif

# Add sources for respective board flash type
ifeq ($(MICROPY_HW_FLASH_TYPE),$(filter $(MICROPY_HW_FLASH_TYPE),qspi_nor_flash qspi_hyper_flash))
    # Add hal/flexspi_nor_flash.c or hal/flashspi_hyper_flash.c respectively
    SRC_C_BL += hal/flexspi_$(subst qspi_,,$(MICROPY_HW_FLASH_TYPE)).c
    #
    # Add custom (board specific) or default configuration
    ifeq ($(MICROPY_HW_BOARD_FLASH_FILES),1)
        SRC_C_BL += $(BOARD_DIR)/$(MICROPY_HW_FLASH_TYPE)_config.c
    else
        SRC_C_BL += hal/$(MICROPY_HW_FLASH_TYPE)_config.c
    endif
else
    $(error Error: Unknown board flash type $(MICROPY_HW_FLASH_TYPE))
endif

SRC_SS_BL = \
	$(MCU_DIR)/gcc/startup_$(MCU_SERIES).S \
	$(BOOTLOADER_DIR)/hal/resethandler_mboot_MIMXRT10xx_RAM.S

# Flags
CFLAGS_DEFINES_BL += \
	-DMICROPY_BOOT_BUFFER_SIZE='$(MICROPY_BOOT_BUFFER_SIZE)'

# Tune for Debugging or Optimization
ifeq ($(DEBUG),1)
CFLAGS_DEBUG_BL += -Og -ggdb -DNDEBUG
else
CFLAGS_DEBUG_BL += -Os -DNDEBUG 
endif

CFLAGS_BL += \
	$(CFLAGS_DEBUG_BL) \
	$(CFLAGS_DEFINES_BL) \
	$(INC_BL) \
	-D__START=main \
	-D__STARTUP_CLEAR_BSS \
	-D__STARTUP_INITIALIZE_RAMFUNCTION \
	-DBOARD_$(BOARD) \
	-DCFG_TUSB_MCU=OPT_MCU_MIMXRT10XX \
	-DCLOCK_CONFIG_H='<boards/$(MCU_SERIES)_clock_config.h>' \
	-DCPU_$(MCU_SERIES) \
	-DCPU_$(MCU_VARIANT) \
	-DCPU_HEADER_H='<$(MCU_SERIES).h>' \
	-DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \
	-DMICROPY_HW_FLASH_SIZE=$(MICROPY_HW_FLASH_SIZE) \
	-DXIP_BOOT_HEADER_ENABLE=1 \
	-DXIP_EXTERNAL_FLASH=1 \
	-fdata-sections \
	-ffunction-sections \
	-mcpu=cortex-m7 \
	-mtune=cortex-m7 \
	-nostdlib \
	-std=c99 \
	-Wall \
	-Wdouble-promotion \
	-Werror \
	-Wfloat-conversion \
	-Wno-error=unused-parameter

# Configure respective board flash type
ifeq ($(MICROPY_HW_FLASH_TYPE),$(filter $(MICROPY_HW_FLASH_TYPE),qspi_nor_flash qspi_hyper_flash))
    # Add hal/flexspi_nor_flash.h or hal/flexspi_hyper_flash.h respectively
    CFLAGS_BL += -DBOARD_FLASH_OPS_HEADER_H=\"hal/flexspi_$(subst qspi_,,$(MICROPY_HW_FLASH_TYPE)).h\"
    #
    # Add custom (board specific) or default configuration
    ifeq ($(MICROPY_HW_BOARD_FLASH_FILES),1)
        CFLAGS_BL += -DBOARD_FLASH_CONFIG_HEADER_H=\"$(BOARD)_flexspi_flash_config.h\"
    else
        CFLAGS_BL += -DBOARD_FLASH_CONFIG_HEADER_H=\"hal/flexspi_flash_config.h\"
    endif
else
    $(error Error: Unknown board flash type $(MICROPY_HW_FLASH_TYPE))
endif	

LDFLAGS_BL += \
	-mcpu=cortex-m7 \
	-mtune=cortex-m7 \
	-specs=nano.specs \
	-specs=nosys.specs \
	-Wl,--cref \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage \
	-Wl,-Map=$@.map \
	-Wl,-N

# LDDEFINES_BL are used for link time adaptation of linker scripts, utilizing
#	the C preprocessor. Therefore keep LDDEFINES_BL separated from LDFLAGS!

LDDEFINES_BL = \
	-DMICROPY_HW_FLASH_TYPE=$(MICROPY_HW_FLASH_TYPE) \
	-DMICROPY_HW_FLASH_SIZE=$(MICROPY_HW_FLASH_SIZE) \
	-DBOARD_LINKER_SCRIPT_H=\"$(MCU_SERIES).ld\"

# Start of firmware is dependent on board flash type. Bootloader requires at least 32kB.
#	Firmware start has to be aligned to sector size to allow erase of firmware without affecting the bootloader.
#	Sector size is different between QPSI flash (4kB) and HyperFlash (256kB) devices!
ifeq ($(MICROPY_HW_FLASH_TYPE),qspi_nor_flash)
	LDDEFINES_BL += -DMICROPY_HW_FLASH_FIRMWARE_START_ADDR=0x00008000
else ifeq ($(MICROPY_HW_FLASH_TYPE),qspi_hyper_flash)
	LDDEFINES_BL += -DMICROPY_HW_FLASH_FIRMWARE_START_ADDR=0x00040000
else
	$(error Error: Unknown board flash type $(MICROPY_HW_FLASH_TYPE))
endif

LIBS_BL = $(shell $(CC) $(CFLAGS_BL) -print-libgcc-file-name)
OBJ_BL += $(addprefix $(BUILD_BL)/, $(SRC_C_BL:.c=.o))
OBJ_BL += $(addprefix $(BUILD_BL)/, $(SRC_SS_BL:.S=.o))

define compile_c_bl
$(ECHO) "CC $<"
$(Q)$(CC) $(CFLAGS_BL) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  $(RM) -f $(@:.o=.d)
endef

# Targets
mboot_all: $(BUILD_BL)/mboot.bin $(BUILD_BL)/mboot.hex $(BUILD_BL)/mboot.hex mboot_info 
.PHONY: mboot_all

mboot_clean:
	$(RM) -rf $(BUILD_BL)
.PHONY: mboot_clean

OBJ_DIRS_BL = $(sort $(dir $(OBJ_BL)))
$(OBJ_BL): | $(OBJ_DIRS_BL)
$(OBJ_DIRS_BL):
	$(MKDIR) -p $@

vpath %.S . $(TOP)
$(BUILD_BL)/%.o: %.S
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS_BL) -c -o $@ $<

vpath %.s . $(TOP)
$(BUILD_BL)/%.o: %.s
	$(ECHO) "AS $<"
	$(Q)$(AS) -o $@ $<

vpath %.c . $(TOP)
$(BUILD_BL)/%.o: %.c
	$(call compile_c_bl)

$(BUILD_BL)/%.pp: %.c
	$(ECHO) "PreProcess $<"
	$(Q)$(CPP) $(CFLAGS_BL) -Wp,-C,-dD,-dI -o $@ $<

$(BUILD_BL)/mboot.elf: $(OBJ_BL)
	$(ECHO) $(LINK_MSG)
	$(ECHO) "PREPROCESS LINK $@"
	$(Q)$(CC) -E -x c $(LDDEFINES_BL) $(LD_FILES_BL) | grep -v '^#' > $(BUILD_BL)/link.ld
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(addprefix -T, $(BUILD_BL)/link.ld)  $(LDFLAGS_BL) -o $@ $^ $(LIBS_BL)

$(BUILD_BL)/mboot.bin: $(BUILD_BL)/mboot.elf
	$(Q)$(OBJCOPY) -O binary $^ $@

$(BUILD_BL)/mboot.hex: $(BUILD_BL)/mboot.elf
	$(Q)$(OBJCOPY) -O ihex $^ $@

mboot_info: $(BUILD_BL)/mboot.elf $(BUILD_BL)/mboot.bin
	$(ECHO) "Bootloader Info"
	$(ECHO) "Download buffer $(MICROPY_BOOT_BUFFER_SIZE)"
	$(Q)$(SIZE) $(BUILD_BL)/mboot.elf
	$(Q)$(SIZE) $(BUILD_BL)/mboot.bin --target binary

BL_JLINK_COMMANDER_SCRIPT = $(BUILD_BL)/script_mboot.jlink

ifdef JLINK_IP
BL_JLINK_CONNECTION_SETTINGS = -IP $(JLINK_IP)
else
BL_JLINK_CONNECTION_SETTINGS = -USB
endif

mboot_deploy_jlink: $(BUILD_BL)/mboot.hex
	$(Q)$(TOUCH) $(BL_JLINK_COMMANDER_SCRIPT)
	$(ECHO) "ExitOnError 1" > $(BL_JLINK_COMMANDER_SCRIPT)
	$(ECHO) "speed auto" >> $(BL_JLINK_COMMANDER_SCRIPT)
	$(ECHO) "r" >> $(BL_JLINK_COMMANDER_SCRIPT)
	$(ECHO) "st" >> $(BL_JLINK_COMMANDER_SCRIPT)
	$(ECHO) "loadfile \"$(realpath $(BUILD_BL)/mboot.hex)\"" >> $(BL_JLINK_COMMANDER_SCRIPT)
	$(ECHO) "qc" >> $(BL_JLINK_COMMANDER_SCRIPT)
	$(JLINK_PATH)JLinkExe -device $(MCU_VARIANT) -if SWD $(BL_JLINK_CONNECTION_SETTINGS) -CommanderScript $(BL_JLINK_COMMANDER_SCRIPT)



.PHONY: mboot_info
.PHONY: mboot_deploy_jlink
