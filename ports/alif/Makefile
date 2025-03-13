BOARD ?= ALIF_ENSEMBLE
BOARD_DIR ?= boards/$(BOARD)
BUILD ?= build-$(BOARD)
MCU_CORE ?= M55_HP
GIT_SUBMODULES += lib/tinyusb lib/alif_ensemble-cmsis-dfp lib/alif-security-toolkit
PORT ?= /dev/ttyACM0

ALIF_TOOLS ?= ../../lib/alif-security-toolkit/toolkit

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

ALIF_TOC_CONFIG = alif_cfg.json
ALIF_TOC_APPS = $(BUILD)/$(ALIF_TOC_CONFIG)
ALIF_TOC_CFLAGS += -DTOC_CFG_FILE=$(ALIF_TOOLKIT_CFG_FILE)

ifeq ($(MCU_CORE),M55_HP)

ALIF_TOC_CFLAGS += -DTOC_CORE_M55_HP_APP=1
ALIF_TOC_APPS += $(BUILD)/M55_HP/firmware.bin
JLINK_CMD = '\
$(JLINK_CMD_PREFIX)\
LoadFile "$(BUILD)/M55_HP/firmware.bin",0x80020000\n\
$(JLINK_CMD_SUFFIX)'

else ifeq ($(MCU_CORE),M55_HE)

ALIF_TOC_CFLAGS += -DTOC_CORE_M55_HE_APP=1
ALIF_TOC_APPS += $(BUILD)/M55_HE/firmware.bin
JLINK_CMD = '\
$(JLINK_CMD_PREFIX)\
LoadFile "$(BUILD)/M55_HE/firmware.bin",0x80320000\n\
$(JLINK_CMD_SUFFIX)'

else ifeq ($(MCU_CORE),M55_DUAL)

ALIF_TOC_CFLAGS += -DTOC_CORE_M55_HP_APP=1
ALIF_TOC_CFLAGS += -DTOC_CORE_M55_HE_APP=1
ALIF_TOC_APPS += $(BUILD)/M55_HP/firmware.bin $(BUILD)/M55_HE/firmware.bin
JLINK_CMD = '\
$(JLINK_CMD_PREFIX)\
LoadFile "$(BUILD)/M55_HP/firmware.bin",0x80020000\n\
LoadFile "$(BUILD)/M55_HE/firmware.bin",0x80320000\n\
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

.PHONY: all
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

$(BUILD)/M55_HE/firmware.bin:
	make -f alif.mk MCU_CORE=M55_HE MICROPY_PY_OPENAMP_MODE=1

$(BUILD)/$(ALIF_TOC_CONFIG): mcu/$(ALIF_TOC_CONFIG).in | $(BUILD)
	$(ECHO) "Preprocess toc config $@"
	$(Q)$(CPP) -P -E $(ALIF_TOC_CFLAGS) - < mcu/$(ALIF_TOC_CONFIG).in > $@

$(BUILD)/firmware.toc.bin: $(ALIF_TOC_APPS)
	$(Q)python $(ALIF_TOOLS)/app-gen-toc.py \
		--filename $(abspath $(BUILD)/$(ALIF_TOC_CONFIG)) \
		--output-dir $(BUILD) \
		--firmware-dir $(BUILD) \
		--output $@

.PHONY: deploy
deploy: $(BUILD)/firmware.toc.bin
	$(ECHO) "Writing $< to the board"
	$(Q)python $(ALIF_TOOLS)/app-write-mram.py \
		--cfg-part $(ALIF_TOOLKIT_CFG_PART) \
		--port $(PORT) \
		--pad \
		--images file:$(BUILD)/application_package.ds

.PHONY: deploy-jlink
deploy-jlink: $(ALIF_TOC_APPS)
	$(Q)echo -e $(JLINK_CMD) | $(JLINK_EXE)

.PHONY: maintenance
maintenance:
	$(Q)python $(ALIF_TOOLS)/maintenance.py \
		--cfg-part $(ALIF_TOOLKIT_CFG_PART) \
		--port $(PORT)

.PHONY: update-system-package
update-system-package:
	$(Q)python $(ALIF_TOOLS)/updateSystemPackage.py \
		--cfg-part $(ALIF_TOOLKIT_CFG_PART) \
		--port $(PORT)

include $(TOP)/py/mkrules.mk
