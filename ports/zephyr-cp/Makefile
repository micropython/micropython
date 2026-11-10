ALL_BOARDS_IN_PORT := $(patsubst boards/%,%,$(wildcard boards/*/*))

# Needed to print the board list one board per line below.
empty :=
space := $(empty) $(empty)
define newline


endef
# An incorrect BOARD might have been specified, so check against the list.
# Boards are stored as vendor/name but BOARD is the flattened vendor_name, so
# flatten the list the same way (vendors contain no underscores). There is
# deliberately no space after the :=
VALID_BOARD :=$(filter $(BOARD),$(subst /,_,$(ALL_BOARDS_IN_PORT)))

# Fail fast on a mistyped BOARD instead of letting west build a board the port
# isn't configured for (which dies later with confusing Kconfig errors). Targets
# like `clean` don't take a BOARD, so only check when one is given.
ifneq ($(strip $(BOARD)),)
ifeq ($(strip $(VALID_BOARD)),)
$(error Invalid BOARD '$(BOARD)'. Valid boards:$(newline)$(subst $(space),$(newline),$(subst /,_,$(ALL_BOARDS_IN_PORT))))
endif
endif

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

TRANSLATION ?= en_US

# Compute shield args once. Command-line SHIELD/SHIELDS values override board defaults from circuitpython.toml.
ifneq ($(strip $(BOARD)),)
WEST_SHIELD_ARGS := $(shell SHIELD_ORIGIN="$(origin SHIELD)" SHIELDS_ORIGIN="$(origin SHIELDS)" SHIELD="$(SHIELD)" SHIELDS="$(SHIELDS)" python cptools/get_west_shield_args.py $(BOARD))
endif

WEST_CMAKE_ARGS := -DZEPHYR_BOARD_ALIASES=$(CURDIR)/boards/board_aliases.cmake

# Board files live in the board's own folder as board.overlay / board.conf. Zephyr only auto-applies
# boards/<name>.overlay and boards/<name>.conf named after the RESOLVED Zephyr board, so it finds
# neither those (wrong directory) nor files named after a board declared through cp_board_alias()
# (wrong name) - the build silently produces a different firmware instead of warning. Pass them
# explicitly when they exist.
CP_BOARD_DIR := $(CURDIR)/boards/$(firstword $(subst _, ,$(BOARD)))/$(patsubst $(firstword $(subst _, ,$(BOARD)))_%,%,$(BOARD))
# DTC_OVERLAY_FILE, not EXTRA_: a board overlay REPLACES app.overlay (Zephyr only falls back to
# app.overlay when no board overlay is found), which is why the board overlays here include it
# themselves - and why one board that has no USB deliberately does not. Adding the overlay instead
# would force app.overlay on every board. socs/ ships .conf only, so nothing else is displaced.
CP_BOARD_OVERLAY := $(wildcard $(CP_BOARD_DIR)/board.overlay)
ifneq ($(CP_BOARD_OVERLAY),)
WEST_CMAKE_ARGS += -Dzephyr-cp_DTC_OVERLAY_FILE=$(CP_BOARD_OVERLAY)
endif
CP_BOARD_CONF := $(wildcard $(CP_BOARD_DIR)/board.conf)

# bsim boards link against the BabbleSim simulator checked out under tools/bsim.
# Zephyr's bsim_hw_models module requires these env vars at configure time, so
# export them here instead of relying on the caller's shell having them set.
ifneq ($(findstring bsim,$(BOARD)),)
export BSIM_COMPONENTS_PATH := $(CURDIR)/tools/bsim/components
export BSIM_OUT_PATH := $(CURDIR)/tools/bsim
endif

# When DEBUG=1, apply additional Kconfig fragments for debug-friendly settings.
DEBUG_CONF_FILE ?= $(CURDIR)/debug.conf
ifeq ($(DEBUG),1)
CP_BOARD_CONF := $(strip $(CP_BOARD_CONF) $(DEBUG_CONF_FILE))
endif
# When CONSOLE=1 (implied by DEBUG=1), apply the board's console.overlay, if it has one, and the
# port's console.conf. Boards whose console pins are also user pins enable the Zephyr UART console
# only then.
CONSOLE ?= $(DEBUG)
CP_BOARD_CONSOLE_OVERLAY :=
ifeq ($(CONSOLE),1)
CP_BOARD_CONSOLE_OVERLAY := $(wildcard $(CP_BOARD_DIR)/console.overlay)
ifneq ($(CP_BOARD_CONSOLE_OVERLAY),)
CP_BOARD_CONF := $(strip $(CP_BOARD_CONF) $(CURDIR)/console.conf)
endif
endif
CP_BOARD_CONF := $(subst $(eval) ,;,$(CP_BOARD_CONF))
# Always pass the conf and console overlay files, even if empty. This undoes DEBUG=1 or CONSOLE=1
# when it's omitted on a later run. Quoted because the conf value can be a semicolon-separated
# list, and an unquoted semicolon would end the shell command.
WEST_CMAKE_ARGS += -Dzephyr-cp_EXTRA_CONF_FILE="$(CP_BOARD_CONF)"
WEST_CMAKE_ARGS += -Dzephyr-cp_EXTRA_DTC_OVERLAY_FILE="$(CP_BOARD_CONSOLE_OVERLAY)"

# Boards can opt into building the MCUboot bootloader (and thus BLE OTA
# updates) by shipping a sysbuild.conf fragment. It sets sysbuild-level
# Kconfig (SB_CONFIG_BOOTLOADER_MCUBOOT, bootloader mode, signing keys, ...).
CP_BOARD_SYSBUILD_CONF := $(wildcard $(CP_BOARD_DIR)/sysbuild.conf)
ifneq ($(CP_BOARD_SYSBUILD_CONF),)
WEST_CMAKE_ARGS += -DSB_EXTRA_CONF_FILE=$(CP_BOARD_SYSBUILD_CONF)
endif

# Boards that enable the bootloader can also tune the MCUboot image itself
# (Kconfig for the bootloader build) via a mcuboot.conf fragment.
CP_BOARD_MCUBOOT_CONF := $(wildcard $(CP_BOARD_DIR)/mcuboot.conf)
ifneq ($(CP_BOARD_MCUBOOT_CONF),)
WEST_CMAKE_ARGS += -Dmcuboot_EXTRA_CONF_FILE=$(CP_BOARD_MCUBOOT_CONF)
endif

.PHONY: $(BUILD)/zephyr-cp/zephyr/zephyr.elf flash recover debug debug-jlink debugserver attach run run-sim clean menuconfig all clean-all sim clean-sim test fetch-port-submodules adaboot

# In the environment, not a cmake argument, so a new language does not reconfigure.
export TRANSLATION

# west reruns the whole sysbuild configure whenever it gets a cmake argument, so pass
# them only when the build directory is new or they changed since the last configure.
WEST_CONFIGURE_ARGS := -b $(BOARD) $(WEST_SHIELD_ARGS) --sysbuild -- $(WEST_CMAKE_ARGS)
WEST_ARGS_STAMP := $(BUILD)/west_configure_args.txt

$(BUILD)/zephyr-cp/zephyr/zephyr.elf:
	python cptools/pre_zephyr_build_prep.py $(BOARD)
	@mkdir -p $(BUILD)
	@if [ ! -f $(BUILD)/build.ninja ] || [ "$$(cat $(WEST_ARGS_STAMP) 2>/dev/null)" != '$(WEST_CONFIGURE_ARGS)' ]; then \
		echo west build -d $(BUILD) $(WEST_CONFIGURE_ARGS); \
		echo '$(WEST_CONFIGURE_ARGS)' > $(WEST_ARGS_STAMP); \
		west build -d $(BUILD) $(WEST_CONFIGURE_ARGS) || { rm -f $(WEST_ARGS_STAMP); exit 1; }; \
	else \
		echo west build -d $(BUILD); \
		west build -d $(BUILD); \
	fi
	python ../../tools/build_memory_info.py $(BUILD)/zephyr-cp/zephyr/zephyr.map $(BUILD) --image $(BUILD)/zephyr-cp/zephyr/zephyr.bin

$(BUILD)/firmware.elf: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	cp $^ $@

# Boards the Adaboot fork owns the memory map for (BOARD id minus its vendor
# prefix is the fork's boards.toml key): pull in its generated make fragment,
# which carries MCUBOOT_UF2_APP_BASE -- the app slot's offset within its
# flash device, i.e. the base address .uf2 files must use (Adaboot's UF2
# writer routes blocks by flash-area offset, see boot/zephyr/uf2_disk.c).
# Like the fork's dts/ tree, the per-board conf/mk fragments live in vendor
# subdirectories; the vendor is the BOARD id's first underscore-separated
# token (the fork's boards.toml keys are board ids minus that prefix).
ADABOOT_VENDOR := $(word 1,$(subst _, ,$(BOARD)))
ADABOOT_KEY := $(subst $(ADABOOT_VENDOR)_,,$(BOARD))
-include bootloader/mcuboot/conf/$(ADABOOT_VENDOR)/$(ADABOOT_KEY)-autogen.mk

# MCUBOOT_UF2_APP_BASE is only set for boards that boot via the Adaboot
# MCUboot fork: the app links into the slot0 partition, so the flashable app
# image is the signed one (MCUboot header + hash TLV), and firmware.hex
# carries both the bootloader and signed app images so it can be flashed
# directly. Standalone boards
# (adafruit/raspberrypi UF2-native, native sim, nrf54l15tag) have no
# bootloader domain and ship Zephyr's own unsigned outputs.
ifeq ($(MCUBOOT_UF2_APP_BASE),)

$(BUILD)/firmware.hex: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	cp $(BUILD)/zephyr-cp/zephyr/zephyr.hex $@

$(BUILD)/firmware.uf2: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	# Zephyr generates the UF2 itself, with the board's family ID and
	# flash base (CONFIG_BUILD_OUTPUT_UF2*).
	cp $(BUILD)/zephyr-cp/zephyr/zephyr.uf2 $@

else

# west flash programs every sysbuild domain, so ship the same in one hex:
# bootloader, signed app, and the network-core image (if this board builds
# one) so it can be flashed directly with a programmer. The unsigned app
# zephyr.hex also links into slot0 but carries no MCUboot header, so the
# bootloader rejects it.
$(BUILD)/firmware.hex: $(BUILD)/zephyr-cp/zephyr/zephyr.elf $(wildcard $(BUILD)/hci_ipc/zephyr/zephyr.hex)
	# hexmerge.py merges the bootloader and app images; it drops the ELF
	# start-address records, as a programmer expects.
	python zephyr/scripts/build/mergehex.py -o $@ $(BUILD)/mcuboot/zephyr/zephyr.hex $(BUILD)/zephyr-cp/zephyr/zephyr.signed.hex $(wildcard $(BUILD)/hci_ipc/zephyr/zephyr.hex)

# The signed slot0 image: what an SMP device-manager uploads into slot1 for
# a BLE update (and the payload firmware.uf2 wraps).
$(BUILD)/firmware.signed.bin: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	cp $(BUILD)/zephyr-cp/zephyr/zephyr.signed.bin $@

$(BUILD)/firmware.uf2: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	# Base for uf2conv: MCUBOOT_UF2_APP_BASE from the fork's autogen .mk,
	# the app slot's offset within its flash device (see the ADABOOT_KEY
	# comment above). Family 0xf4b47471 is Adaboot's own; it routes blocks
	# by base address, so any consistent non-zero ID is fine but keep it
	# stable for bootloaders that check it.
	python zephyr/scripts/build/uf2conv.py -c -f 0xf4b47471 -b $(MCUBOOT_UF2_APP_BASE) -o $@ $(BUILD)/zephyr-cp/zephyr/zephyr.signed.bin

endif

$(BUILD)/firmware.exe: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	cp $(BUILD)/zephyr-cp/zephyr/zephyr.exe $@

$(BUILD)/firmware.rps: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	cp $(BUILD)/zephyr-cp/zephyr/zephyr.rps $@

flash: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	west flash -d $(BUILD)

recover: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
		west flash --recover -d $(BUILD)

debug: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	west debug -d $(BUILD)

debug-jlink: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	west debug --runner jlink -d $(BUILD)

debugserver: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	west debugserver -d $(BUILD)

attach: $(BUILD)/zephyr-cp/zephyr/zephyr.elf
	west attach -d $(BUILD)

run: $(BUILD)/firmware.exe
	$^

run-sim:
	$(MAKE) BOARD=native_native_sim BUILD=build-native_native_sim build-native_native_sim/firmware.exe
	truncate -s 2M build-native_native_sim/flash.bin
	mformat -i build-native_native_sim/flash.bin ::
	@if [ -d CIRCUITPY ] && [ -n "$$(find CIRCUITPY -mindepth 1 -print -quit)" ]; then \
		echo "Populating build-native_native_sim/flash.bin from ./CIRCUITPY"; \
		mcopy -s -i build-native_native_sim/flash.bin CIRCUITPY/* ::; \
	fi
	build-native_native_sim/firmware.exe --flash=build-native_native_sim/flash.bin --flash_rm -wait_uart -rt --i2s_capture=build-native_native_sim/i2s_capture.wav

menuconfig:
	west build $(WEST_SHIELD_ARGS) --sysbuild -d $(BUILD) -t menuconfig -- $(WEST_CMAKE_ARGS)

adaboot:
	west build -d $(BUILD) --domain mcuboot

clean:
	rm -rf $(BUILD)

fetch-port-submodules:
		python ../../tools/ci_fetch_deps.py zephyr-cp

# Build all boards. The + prefix allows jobserver file descriptors to be passed through.
# This enables parallel builds across all boards when invoked with `make -jN all`.
all:
	+python cptools/build_all_boards.py --continue-on-error

clean-all:
	rm -rf build build-*

# Build all sim boards concurrently using the same jobserver as `make all`.
sim:
	+python cptools/build_all_boards.py --vendor native --continue-on-error

clean-sim:
	rm -rf $(wildcard build-native_*)

# Every board the test suite uses: native sim (non-asan + asan) and the bsim
# boards (tests/bsim/conftest.py parametrizes over both). Zephyr samples for
# bsim tests are built on demand by the zephyr_sample fixture.
TEST_BOARDS := native_native_sim native_native_sim_asan native_native_sim_lfs native_nrf5340bsim native_nrf54lm20bsim

# Delegate to a sub-make with BOARD set so the per-board build rule (and its
# shield args + bsim prep) applies. The targets are phony: the west builds are
# incremental, so this just keeps the test binaries fresh.
define test_board_rule
.PHONY: test-build-$(1)
test-build-$(1):
	+$$(MAKE) BOARD=$(1) BUILD=build-$(1) build-$(1)/zephyr-cp/zephyr/zephyr.elf
endef
$(foreach board,$(TEST_BOARDS),$(eval $(call test_board_rule,$(board))))

# CI splits this into several jobs, one board each, narrowing TEST_BOARDS and
# PYTEST_ARGS; see .github/workflows/run-zephyr-tests.yml.
PYTEST_ARGS ?= tests/

test: $(TEST_BOARDS:%=test-build-%)
	pytest cptools/tests
	pytest -v $(PYTEST_ARGS)

# The core test suite (tests/run-tests.py) on native_sim. Pass test directories
# or files and the number of simulators in RUN_TESTS_ARGS, for example
# RUN_TESTS_ARGS="-j 8 -d basics extmod".
RUN_TESTS_ARGS ?=

.PHONY: test-run-tests
test-run-tests: test-build-native_native_sim
	python cptools/run_tests_native_sim.py $(RUN_TESTS_ARGS)
