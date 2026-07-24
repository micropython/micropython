# Manifest processing for freeze(), require(), include(), and c_module().
# This file handles both C module extraction and frozen Python content generation.

ifneq ($(FROZEN_MANIFEST),)

# Set default path variables to be passed to makemanifest.py. These will be
# available in path substitutions. Additional variables can be set per-board
# in mpconfigboard.mk or on the make command line.
MICROPY_MANIFEST_PORT_DIR ?= $(CURDIR)
MICROPY_MANIFEST_BOARD_DIR ?= $(BOARD_DIR)
MICROPY_MANIFEST_MPY_DIR ?= $(TOP)
MICROPY_MANIFEST_MPY_LIB_DIR ?= $(MPY_LIB_DIR)

# Find all MICROPY_MANIFEST_* variables and turn them into command line
# arguments. Values must not contain whitespace (GNU make word-splits).
MANIFEST_VARIABLES = $(foreach var,$(filter MICROPY_MANIFEST_%, $(.VARIABLES)),-v "$(subst MICROPY_MANIFEST_,,$(var))=$($(var))")

# Skip c_module extraction for targets that don't need a build.
MANIFEST_NON_BUILD_GOALS := clean clean-prog submodules help print-cfg print-def
MANIFEST_GOALS := $(filter-out $(MANIFEST_NON_BUILD_GOALS),$(or $(MAKECMDGOALS),build))

# Extract C module paths up front so downstream rules see them. Skipped when
# micropython-lib isn't initialised (require() would fail) or for non-build
# targets. $(shell) collapses newlines, so c_module() paths cannot contain
# whitespace on the make side (the cmake side handles it correctly).
MANIFEST_C_MODULES :=
ifneq ($(MANIFEST_GOALS),)
ifeq ($(wildcard $(FROZEN_MANIFEST)),$(FROZEN_MANIFEST))
ifeq ($(wildcard $(MPY_LIB_DIR)/README.md),$(MPY_LIB_DIR)/README.md)
MANIFEST_C_MODULES := $(shell $(MAKE_MANIFEST) --list-c-modules $(MANIFEST_VARIABLES) $(FROZEN_MANIFEST))
# Treat a populated, non-zero .SHELLSTATUS as failure. GNU make < 4.2 (eg
# macOS 3.81) leaves .SHELLSTATUS empty, in which case skip the check; an
# empty filter result means "succeed (status was 0, or unavailable)".
ifneq ($(filter-out 0,$(.SHELLSTATUS)),)
$(error makemanifest.py --list-c-modules failed (exit $(.SHELLSTATUS)) for $(FROZEN_MANIFEST))
endif
else
$(warning c_module() extraction skipped: micropython-lib not initialised, run 'make submodules')
endif
endif
endif

# Merge manifest c_modules into USER_C_MODULES. $(sort) also de-duplicates;
# this changes USER_C_MODULES iteration order vs cmake's list(REMOVE_DUPLICATES)
# which preserves first occurrence. Module include order should not be
# behaviour-sensitive so this is acceptable.
USER_C_MODULES := $(sort $(USER_C_MODULES) $(MANIFEST_C_MODULES))

# Frozen content rule, requires $(BUILD) and other variables from the port.
ifdef BUILD
$(BUILD)/frozen_content.c: FORCE $(BUILD)/genhdr/qstrdefs.generated.h $(BUILD)/genhdr/root_pointers.h | $(MICROPY_MPYCROSS_DEPENDENCY)
	$(Q)test -e "$(MPY_LIB_DIR)/README.md" || (echo -e $(HELP_MPY_LIB_SUBMODULE); false)
	$(Q)$(MAKE_MANIFEST) -o $@ $(MANIFEST_VARIABLES) -b "$(BUILD)" $(if $(MPY_CROSS_FLAGS),-f"$(MPY_CROSS_FLAGS)",) --mpy-tool-flags="$(MPY_TOOL_FLAGS)" $(FROZEN_MANIFEST)
endif

endif # FROZEN_MANIFEST
