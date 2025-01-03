BOARD ?= ALIF_ENSEMBLE
BOARD_DIR ?= boards/$(BOARD)
BUILD ?= build-$(BOARD)
MCU_CORE ?= M55_HP
GIT_SUBMODULES += lib/tinyusb lib/alif_ensemble-cmsis-dfp

ALIF_TOOLS ?= tools

JLINK_CMD_PREFIX = \
ExitOnError 1\n\
Device $(JLINK_DEV)\n\
SelectInterface SWD\n\
Speed auto\n\
Connect\n\
Reset\n\
ShowHWStatus\n

JLINK_CMD_SUFFIX = \
Reset\n\
Exit

ifeq ($(MCU_CORE),M55_HP)

ALIF_TOC_CONFIG = mcu/M55_HP_cfg.json
ALIF_TOC_APPS = $(BUILD)/M55_HP/firmware.bin
JLINK_CMD = '\
$(JLINK_CMD_PREFIX)\
LoadFile "$(BUILD)/M55_HP/firmware.bin",0x80020000\n\
$(JLINK_CMD_SUFFIX)'

else ifeq ($(MCU_CORE),M55_HE)

ALIF_TOC_CONFIG = mcu/M55_HE_cfg.json
ALIF_TOC_APPS = $(BUILD)/M55_HE/firmware.bin
JLINK_CMD = '\
$(JLINK_CMD_PREFIX)\
LoadFile "$(BUILD)/M55_HE/firmware.bin",0x8037e000\n\
$(JLINK_CMD_SUFFIX)'

else ifeq ($(MCU_CORE),M55_DUAL)

ALIF_TOC_CONFIG = mcu/M55_DUAL_cfg.json
ALIF_TOC_APPS = $(BUILD)/M55_HP/firmware.bin $(BUILD)/M55_HE/firmware.bin
JLINK_CMD = '\
$(JLINK_CMD_PREFIX)\
LoadFile "$(BUILD)/M55_HP/firmware.bin",0x80020000\n\
LoadFile "$(BUILD)/M55_HE/firmware.bin",0x8037e000\n\
$(JLINK_CMD_SUFFIX)'

else
$(error Invalid MCU core specified))
endif

include ../../py/mkenv.mk
include mpconfigport.mk
include $(BOARD_DIR)/mpconfigboard.mk

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

################################################################################
# Main targets

all: $(BUILD)/firmware.toc.bin

# Force make commands to run the targets every time
# regardless of whether firmware.toc.bin already exists
# to detect changes in the source files and rebuild.
.PHONY: $(BUILD)/M55_HE/firmware.bin
.PHONY: $(BUILD)/M55_HP/firmware.bin

$(BUILD):
	$(MKDIR) -p $@

$(BUILD)/M55_HP/firmware.bin:
	make -f alif.mk MCU_CORE=M55_HP MICROPY_PY_OPENAMP_MODE=0
	$(Q)cp $(BUILD)/M55_HP/firmware.bin $(ALIF_TOOLS)/build/images/firmware_M55_HP.bin

$(BUILD)/M55_HE/firmware.bin:
	make -f alif.mk MCU_CORE=M55_HE MICROPY_PY_OPENAMP_MODE=1
	$(Q)cp $(BUILD)/M55_HE/firmware.bin $(ALIF_TOOLS)/build/images/firmware_M55_HE.bin

$(BUILD)/firmware.toc.bin: $(ALIF_TOC_APPS)
	$(Q)$(MKDIR) -p $(ALIF_TOOLS)/build/images/
	$(Q)$(MKDIR) -p $(ALIF_TOOLS)/build/logs/
	$(Q)cd $(ALIF_TOOLS) && python app-gen-toc.py -f $(abspath $(ALIF_TOC_CONFIG))
	$(Q)cp $(ALIF_TOOLS)/build/AppTocPackage.bin $(BUILD)/firmware.toc.bin

deploy: $(BUILD)/firmware.toc.bin
	$(ECHO) "Writing $< to the board"
	$(Q)cd $(ALIF_TOOLS) && python app-write-mram.py -p

deploy-jlink: $(ALIF_TOC_APPS)
	$(Q)echo -e $(JLINK_CMD) | $(JLINK_EXE)

include $(TOP)/py/mkrules.mk
