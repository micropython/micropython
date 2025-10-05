# Manifest processing for freeze(), require(), include(), and c_module().
# This file handles both C module extraction and frozen Python content generation.

ifneq ($(FROZEN_MANIFEST),)

# Only process once, even if included multiple times.
ifndef MANIFEST_MK_INCLUDED
MANIFEST_MK_INCLUDED := 1

# Set default path variables to be passed to makemanifest.py. These will be
# available in path substitutions. Additional variables can be set per-board
# in mpconfigboard.mk or on the make command line.
MICROPY_MANIFEST_PORT_DIR ?= $(shell pwd)
MICROPY_MANIFEST_BOARD_DIR ?= $(BOARD_DIR)
MICROPY_MANIFEST_MPY_DIR ?= $(TOP)
MICROPY_MANIFEST_MPY_LIB_DIR ?= $(MPY_LIB_DIR)

# Find all MICROPY_MANIFEST_* variables and turn them into command line arguments.
MANIFEST_VARIABLES = $(foreach var,$(filter MICROPY_MANIFEST_%, $(.VARIABLES)),-v "$(subst MICROPY_MANIFEST_,,$(var))=$($(var))")

# Extract C module paths from manifest for early build phase.
# Only extract if the manifest file exists.
ifeq ($(wildcard $(FROZEN_MANIFEST)),$(FROZEN_MANIFEST))
MANIFEST_C_MODULES := $(shell $(MAKE_MANIFEST) --list-c-modules $(MANIFEST_VARIABLES) $(FROZEN_MANIFEST))
else
MANIFEST_C_MODULES :=
endif

# Combine USER_C_MODULES (from command line/environment) with manifest C modules.
USER_C_MODULES_ALL := $(USER_C_MODULES) $(MANIFEST_C_MODULES)

# Frozen content generation rule (needs $(BUILD) and other variables from port).
# Only define once, and only when BUILD is available.
ifdef BUILD
$(BUILD)/frozen_content.c: FORCE $(BUILD)/genhdr/qstrdefs.generated.h $(BUILD)/genhdr/root_pointers.h | $(MICROPY_MPYCROSS_DEPENDENCY)
	$(Q)test -e "$(MPY_LIB_DIR)/README.md" || (echo -e $(HELP_MPY_LIB_SUBMODULE); false)
	$(Q)$(MAKE_MANIFEST) -o $@ $(MANIFEST_VARIABLES) -b "$(BUILD)" $(if $(MPY_CROSS_FLAGS),-f"$(MPY_CROSS_FLAGS)",) --mpy-tool-flags="$(MPY_TOOL_FLAGS)" $(FROZEN_MANIFEST)
endif

endif # MANIFEST_MK_INCLUDED

endif # FROZEN_MANIFEST
