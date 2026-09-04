# The prebuilt morselib and transceiver blobs; extmod.mk links no libraries.  Needs BUILD set.

HALOW_DIR ?= drivers/halow
HALOW_TOP ?= $(TOP)
HALOW_MMIOT_DIR = $(HALOW_TOP)/lib/mm-iot-sdk/framework
HALOW_MORSELIB_DIR = $(HALOW_MMIOT_DIR)/morselib

# morselib is distributed as a prebuilt library under the Morse Micro Binary
# Distribution Licence.  Building it from source instead is useful for
# debugging, but those sources are GPL-3.0, which is not compatible with the
# rest of this firmware, so the prebuilt library is the default.
HALOW_MORSELIB_CORE ?= arm-cortex-m33f
ifeq ($(HALOW_MORSELIB_SOURCE),1)
INC += $(addprefix -I$(HALOW_MORSELIB_DIR)/,src src/internal src/emmet src/umac/rc/mmrc_osal mmrc/src/core)
SRC_THIRDPARTY_C += $(patsubst $(HALOW_TOP)/%,%,\
	$(shell find $(HALOW_MORSELIB_DIR)/src $(HALOW_MORSELIB_DIR)/mmrc/src -name '*.c'))
CFLAGS_THIRDPARTY += -DLOOKAROUND_FAIL_MAX=50 -Wno-c++-compat
else
# morselib is prebuilt against newlib, but the ports link with -nostdlib, so the
# C library functions it calls (sscanf, qsort, setjmp, _ctype_, ...) are not
# otherwise pulled in.  Resolve libc/libm for the target multilib the same way
# the ports resolve libgcc, and group them with the archive so the linker settles
# the references between morselib and libc regardless of order.  These are lazily
# expanded: CFLAGS only carries the -mcpu flags that select the multilib once the
# including port has finished adding them, after this file is included.
HALOW_LIBC = $(shell $(CC) $(CFLAGS) -print-file-name=libc.a)
HALOW_LIBM = $(shell $(CC) $(CFLAGS) -print-file-name=libm.a)
LIBS += -Wl,--start-group $(HALOW_MORSELIB_DIR)/lib/$(HALOW_MORSELIB_CORE)/libmorse.a $(HALOW_LIBC) $(HALOW_LIBM) -Wl,--end-group
endif

# objcopy derives a blob's symbol names from its path, mangling everything that
# is not alphanumeric into an underscore.
halow_blob_sym = _binary_$(subst .,_,$(subst -,_,$(subst /,_,$(1))))_$(2)

# Output format for the blobs.  Overridable, as the only thing tying the driver
# to a particular architecture is the prebuilt morselib.
HALOW_BFDNAME ?= elf32-littlearm
HALOW_BFDARCH ?= arm

# The transceiver firmware, and optionally a board configuration file holding
# calibration data, are linked in as binary blobs.  A board picks its BCF by
# name with HALOW_BCF; the SDK keeps them per chip under morsefirmware.
HALOW_CHIP ?= mm8108
HALOW_BCF ?= mf15457
HALOW_FW_MBIN ?= $(HALOW_MMIOT_DIR)/morsefirmware/mm8108b2-rl.mbin
ifneq ($(HALOW_BCF),)
HALOW_BCF_MBIN ?= $(HALOW_MMIOT_DIR)/morsefirmware/$(HALOW_CHIP)/bcfs/bcf_$(HALOW_BCF).mbin
endif
HALOW_FW_OBJ = $(BUILD)/$(HALOW_DIR)/halow_firmware.o

$(HALOW_FW_OBJ): $(HALOW_FW_MBIN)
	$(ECHO) "GEN $@"
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(OBJCOPY) -I binary -O $(HALOW_BFDNAME) -B $(HALOW_BFDARCH) $< $@ \
		--redefine-sym $(call halow_blob_sym,$<,start)=halow_firmware_start \
		--redefine-sym $(call halow_blob_sym,$<,end)=halow_firmware_end \
		--rename-section .data=.rodata.halow_firmware,contents,alloc,load,readonly,data \
		--set-section-alignment .data=4

ifneq ($(HALOW_BCF_MBIN),)
CFLAGS += -DMICROPY_HW_HALOW_BCF=1
HALOW_BCF_OBJ = $(BUILD)/$(HALOW_DIR)/halow_bcf.o

$(HALOW_BCF_OBJ): $(HALOW_BCF_MBIN)
	$(ECHO) "GEN $@"
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(OBJCOPY) -I binary -O $(HALOW_BFDNAME) -B $(HALOW_BFDARCH) $< $@ \
		--redefine-sym $(call halow_blob_sym,$<,start)=halow_bcf_start \
		--redefine-sym $(call halow_blob_sym,$<,end)=halow_bcf_end \
		--rename-section .data=.rodata.halow_bcf,contents,alloc,load,readonly,data \
		--set-section-alignment .data=4
endif

OBJ += $(HALOW_FW_OBJ) $(HALOW_BCF_OBJ)
