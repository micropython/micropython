
CONFIG ?= .config
export KCONFIG_CONFIG = $(abspath $(CONFIG))

-include $(KCONFIG_CONFIG)

ifeq ($(CONFIG_PORT),)
# if KCONFIG_CONFIG does not exist fall back to default
PORT ?= stm32
else
# we allow overriding PORT for now, e.g. so we can build mboot with PORT=stm32/mboot
PORT ?= $(CONFIG_PORT:"%"=%)
endif

# If using non-default configuration file and BUILD was not already specified,
# then use build directory based on the config file name
ifeq ($(BUILD),)
ifneq ($(CONFIG),.config)
export BUILD = build-$(notdir $(CONFIG))
endif
endif

.DEFAULT_GOAL := all

help:
	@echo 'Build targets:'
	@echo '  make [all]                   Builds binary using .config file'
	@echo '  make clean                   Cleans output files'
	@echo '  make submodules              Ensures that git submodules are up to date'
	@echo ''
	@echo 'Configuration targets:'
	@echo '  make configs/<name>_config   Creates .config file using one of the included default configuration files'
	@echo '  make defconfig <file>        Creates .config from an arbitrary default configuration file'
	@echo '  make savedefconfig [<file>]  Saves the .config file in condensed format (default <file> `defconfig`)'
	@echo '  make guiconfig               Edits the .config file using a graphical interface'
	@echo '  make menuconfig              Edits the .config file using a text interface'
	@echo '  make olddefconfig            Updates the .config file using default values'
	@echo '  make oldconfig               Updates the .config file prompting for each new value'
	@echo ''
	@echo 'Other targets:'
	@echo '  make mpy-cross               Builds mpy-cross using default configuration'
	@echo '  make help                    Prints this help message'
	@echo ''
	@echo 'Variables:'
	@echo '  BUILD                        Build output directory (default `build` or `build-<CONFIG>`)'
	@echo '  CONFIG                       Configuration file to use (default `.config`)'
	@echo '  PORT                         Directory relative to ports/ (default determined by configuration file)'
	@echo ''
	@echo 'Usage:'
	@echo '  Most MicroPython ports require mpy-cross as part of the build process and'
	@echo '  also use git submodules for some 3rd-party libraries, so the first step is to'
	@echo '  run:'
	@echo ''
	@echo '      make mpy-cross'
	@echo '      make submodules PORT=<port>'
	@echo ''
	@echo '  where <port> is one of the directory names in `ports/`. These commands only'
	@echo '  need to be run once (unless the source code is changed).'
	@echo ''
	@echo '  Then to build the firmware or executuable for a board using the default'
	@echo '  configuration for that board, run:'
	@echo ''
	@echo '      make CONFIG=configs/<board>_config'
	@echo ''
	@echo '  where `<board>_config` is a file in `configs/`.'
	@echo ''
	@echo '  To make a custom configuration based on a default configuration, run:'
	@echo ''
	@echo '      make configs/<board>_config'
	@echo ''
	@echo '  to copy the default configuration to `.config`. Then run:'
	@echo ''
	@echo '      make menuconfig # text-based'
	@echo '      # OR'
	@echo '      make guiconfig # graphical'
	@echo ''
	@echo '  to make changes to the configuration. Once the configuration has been saved,'
	@echo '  simply run:'
	@echo ''
	@echo '      make'
	@echo ''
	@echo '  to build the firmware or executuable based on the custom configuration.'
	@echo ''

.PHONY: help

# general build targets

BUILD_TARGETS := all clean submodules

$(BUILD_TARGETS):
	$(MAKE) -C ports/$(PORT) $@

.PHONY: $(BUILD_TARGETS)


# default configuration targets

CONFIGS := $(wildcard configs/*_config)

$(CONFIGS): tools/Kconfiglib/kconfiglib.py
	./tools/Kconfiglib/defconfig.py $@

.PHONY: $(CONFIGS)


# Kconfig targets

# If first command line argument is `defconfig`, then second command line option
# is treated as the defconfig file.
ifeq (defconfig,$(firstword $(MAKECMDGOALS)))
DEFCONFIG := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
$(eval $(DEFCONFIG):;@:)
endif

defconfig: tools/Kconfiglib/kconfiglib.py
	./tools/Kconfiglib/defconfig.py $(DEFCONFIG)

.PHONY: defconfig

# If first command line argument is `savedefconfig`, then second command line
# option is treated as the defconfig file.
ifeq (savedefconfig,$(firstword $(MAKECMDGOALS)))
DEFCONFIG := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
$(eval $(DEFCONFIG):;@:)
ifneq ($(DEFCONFIG),)
DEFCONFIG := --out $(DEFCONFIG)
endif
endif

savedefconfig: tools/Kconfiglib/kconfiglib.py
	./tools/Kconfiglib/savedefconfig.py $(DEFCONFIG)

.PHONY: savedefconfig

KCONFIG_COMMANDS := guiconfig menuconfig oldconfig olddefconfig

$(KCONFIG_COMMANDS): tools/Kconfiglib/kconfiglib.py
	./tools/Kconfiglib/$@.py

.PHONY: $(KCONFIG_COMMANDS)


# mpy-cross targets

.mpy-cross-config: tools/Kconfiglib/kconfiglib.py
	KCONFIG_CONFIG=$(abspath $@) ./tools/Kconfiglib/defconfig.py configs/mpy_cross_config

mpy-cross: .mpy-cross-config
	KCONFIG_CONFIG=$(abspath $<) $(MAKE) -C mpy-cross CROSS_COMPILE=

.PHONY: mpy-cross

# Ensure that submodule is cloned
tools/Kconfiglib/kconfiglib.py:
	@git submodule update --init tools/Kconfiglib
