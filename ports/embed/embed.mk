# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2022-2023 Damien P. George
#
# This file is intended to be included by a Makefile in a custom project.

# Set the build output directory for the generated files.
BUILD = build-embed

# Include the core environment definitions; this will set $(TOP).
include $(MICROPYTHON_TOP)/py/mkenv.mk

# Include py core make definitions.
include $(TOP)/py/py.mk
ifeq ($(filter extmod,$(EMBED_EXTRA)),extmod)
include $(TOP)/extmod/extmod.mk
endif

# The parts of EMBED_EXTRA that name literal files and don't need special handling.
EMBED_EXTRA_FILES = $(filter-out extmod,$(EMBED_EXTRA))

# Extra files need to be searched for QSTRs.
SRC_QSTR += $(addprefix $(TOP)/,$(EMBED_EXTRA_FILES))

# Set the location of the MicroPython embed port.
MICROPYTHON_EMBED_PORT = $(MICROPYTHON_TOP)/ports/embed

# Set default makefile-level MicroPython feature configurations.
MICROPY_ROM_TEXT_COMPRESSION ?= 0

# Set CFLAGS for the MicroPython build.
CFLAGS += -I. -I$(TOP) -I$(BUILD) -I$(MICROPYTHON_EMBED_PORT)
CFLAGS += -Wall -Werror -std=c99

# Define the required generated header files.
GENHDR_OUTPUT = $(addprefix $(BUILD)/genhdr/, \
	moduledefs.h \
	mpversion.h \
	qstrdefs.generated.h \
	root_pointers.h \
	)

# Define the module freezing output.
ifneq ($(FROZEN_MANIFEST),)
FROZEN_OUTPUT = $(BUILD)/frozen_content.c
endif

# Define the top-level target, the generated output files.
.PHONY: all
all: micropython-embed-package

clean: clean-micropython-embed-package

.PHONY: clean-micropython-embed-package
clean-micropython-embed-package:
	$(RM) -rf $(PACKAGE_DIR)

PACKAGE_DIR ?= micropython_embed
PACKAGE_DIR_LIST = $(addprefix $(PACKAGE_DIR)/,py extmod shared/runtime genhdr port)
ifeq ($(filter extmod,$(EMBED_EXTRA)),extmod)
PACKAGE_DIR_LIST += $(addprefix $(PACKAGE_DIR)/,lib/uzlib lib/crypto-algorithms lib/re1.5)
endif
ifneq ($(FROZEN_MANIFEST),)
PACKAGE_DIR_LIST += $(addprefix $(PACKAGE_DIR)/,frozen)
endif
ifneq ($(EMBED_EXTRA_FILES),)
PACKAGE_DIR_LIST += $(addprefix $(PACKAGE_DIR)/,$(filter-out ./,$(sort $(dir $(EMBED_EXTRA_FILES)))))
endif

.PHONY: micropython-embed-package
micropython-embed-package: $(GENHDR_OUTPUT) $(FROZEN_OUTPUT)
	$(ECHO) "Generate micropython_embed output:"
	$(Q)$(RM) -rf $(PACKAGE_DIR_LIST)
	$(Q)$(MKDIR) -p $(PACKAGE_DIR_LIST)
	$(ECHO) "- py"
	$(Q)$(CP) $(TOP)/py/*.[ch] $(PACKAGE_DIR)/py
	$(ECHO) "- extmod"
ifeq ($(filter extmod,$(EMBED_EXTRA)),extmod)
	$(Q)$(CP) $(TOP)/extmod/*.[ch] $(PACKAGE_DIR)/extmod
	$(ECHO) "- lib"
	$(Q)$(CP) $(TOP)/lib/uzlib/*.[ch] $(PACKAGE_DIR)/lib/uzlib
	$(Q)$(CP) $(TOP)/lib/crypto-algorithms/*.[ch] $(PACKAGE_DIR)/lib/crypto-algorithms
	$(Q)$(CP) $(TOP)/lib/re1.5/*.[ch] $(PACKAGE_DIR)/lib/re1.5
else
	$(Q)$(CP) $(TOP)/extmod/modplatform.h $(PACKAGE_DIR)/extmod
endif
	$(ECHO) "- shared"
	$(Q)$(CP) $(TOP)/shared/runtime/gchelper.h $(PACKAGE_DIR)/shared/runtime
	$(Q)$(CP) $(TOP)/shared/runtime/gchelper_generic.c $(PACKAGE_DIR)/shared/runtime
	$(ECHO) "- genhdr"
	$(Q)$(CP) $(GENHDR_OUTPUT) $(PACKAGE_DIR)/genhdr
ifneq ($(FROZEN_MANIFEST),)
	$(ECHO) "- frozen"
	$(Q)$(CP) $(FROZEN_OUTPUT) $(PACKAGE_DIR)/frozen
endif
	$(ECHO) "- port"
	$(Q)$(CP) $(MICROPYTHON_EMBED_PORT)/port/*.[ch] $(PACKAGE_DIR)/port
ifneq ($(EMBED_EXTRA_FILES),)
	$(ECHO) "- extra"
	$(Q)$(foreach FILE,$(EMBED_EXTRA_FILES),$(CP) $(TOP)/$(FILE) $(dir $(PACKAGE_DIR)/$(FILE)) &&) true
endif

# Include remaining core make rules.
include $(TOP)/py/mkrules.mk
