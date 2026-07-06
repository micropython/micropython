# ---------------------------------------------------------------------------
# Self-contained CM55 IPC echo firmware build.
#
# Builds the CM55 ping-pong echo image (main.c) required by the board test
# tests/ports/psoc-edge/board_only_hw/single/ipc.py and flashes it to the
# m55_nvm region (0x60580000). Uses ONLY sources already present in this
# repository (no ModusToolbox getlibs, no downloads).
#
# Usage (from this directory):
#     make                # build build/cm55_ipc_echo.hex
#     make deploy         # build + flash to the board
#     make clean
#
# Optional overrides:
#     make deploy DEV_SERIAL_NUMBER=<kitprog3-serial>
#     make OPENOCD=/opt/openocd/bin/openocd deploy
# ---------------------------------------------------------------------------

BOARD ?= KIT_PSE84_AI

# Repo-relative locations (this Makefile lives in tests/ports/psoc-edge/cm55_ipc_echo).
TOP      := ../../../..
PORT_DIR := $(TOP)/ports/psoc-edge
PSE_DIR  := $(TOP)/lib/psoc-edge
BOARD_DIR := $(PORT_DIR)/boards/$(BOARD)
BSP_CFG  := $(BOARD_DIR)/bsp-cfg

BUILD ?= build

# Toolchain.
CROSS_COMPILE ?= arm-none-eabi-
CC      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size
OPENOCD ?= openocd

APP := cm55_ipc_echo

# ---------------------------------------------------------------------------
# Compiler / linker flags (Cortex-M55, non-secure, matches the CM33 port setup).
# ---------------------------------------------------------------------------
MACHINE_FLAGS = \
	-mcpu=cortex-m55 \
	--specs=nano.specs \
	--specs=nosys.specs \
	-mfloat-abi=hard \
	-mthumb

DEFINES = \
	-DCOMPONENT_CM55 \
	-DCORE_NAME_CM55_0=1 \
	-DCOMPONENT_NON_SECURE_DEVICE \
	-DCYBSP_MCUBOOT_HEADER_SIZE=0x400

INC = \
	-I. \
	-I$(TOP)/lib/CMSIS_6/CMSIS/Core/Include \
	-I$(TOP)/lib/CMSIS_6/CMSIS/Core/Include/m-profile \
	-I$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/devices/include \
	-I$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/devices/include/ip \
	-I$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/include \
	-I$(PSE_DIR)/mtb-dsl-pse8xxgp/hal/include \
	-I$(PSE_DIR)/mtb-dsl-pse8xxgp/device-utils/syspm/include \
	-I$(PSE_DIR)/mtb-ipc/include \
	-I$(PSE_DIR)/mtb-srf/include \
	-I$(PSE_DIR)/mtb-srf/include/COMPONENT_NON_SECURE_DEVICE \
	-I$(PSE_DIR)/mtb-srf/include/COMPONENT_NON_SECURE_DEVICE/COMPONENT_MW_MTB_IPC \
	-I$(PSE_DIR)/core-lib/include \
	-I$(PSE_DIR)/TARGET_$(BOARD) \
	-I$(BOARD_DIR) \
	-I$(BSP_CFG) \
	-I$(PORT_DIR)/dualcore/include

CFLAGS = -c $(MACHINE_FLAGS) -g -Os -Wall -pipe $(DEFINES) $(INC)

LDSCRIPT := $(PSE_DIR)/TARGET_$(BOARD)/COMPONENT_CM55/TOOLCHAIN_GCC_ARM/pse84_ns_cm55.ld

LDFLAGS = $(MACHINE_FLAGS) \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage \
	-Xlinker -L $(PSE_DIR)/TARGET_$(BOARD)/COMPONENT_CM55/TOOLCHAIN_GCC_ARM \
	-Xlinker -L $(BSP_CFG) \
	-T$(LDSCRIPT) \
	-Wl,-Map,$(BUILD)/$(APP).map

# ---------------------------------------------------------------------------
# Sources (minimal set: CM55 startup/system, IPC PDL drivers, echo app).
# ---------------------------------------------------------------------------
SRC_C = \
	main.c \
	$(PORT_DIR)/dualcore/source/COMPONENT_CM55/cm55_ipc_communication.c \
	$(PSE_DIR)/TARGET_$(BOARD)/COMPONENT_CM55/COMPONENT_NON_SECURE_DEVICE/ns_start_pse84.c \
	$(PSE_DIR)/TARGET_$(BOARD)/COMPONENT_CM55/COMPONENT_NON_SECURE_DEVICE/ns_system_pse84.c \
	$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/devices/source/cy_device.c \
	$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syslib.c \
	$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/cy_gpio.c \
	$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/cy_sysint_v2.c \
	$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/cy_sysclk_v2.c \
	$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/cy_ipc_drv.c \
	$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/cy_ipc_pipe.c \
	$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/cy_ipc_sema.c

# Toolchain-specific assembly (Cy_SysLib_DelayCycles, critical sections).
SRC_S = \
	$(PSE_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/TOOLCHAIN_GCC_ARM/cy_syslib_ext.S

OBJ = $(addprefix $(BUILD)/obj/,$(notdir $(SRC_C:.c=.o)))
OBJ += $(addprefix $(BUILD)/obj/,$(notdir $(SRC_S:.S=.o)))
VPATH = $(sort $(dir $(SRC_C)) $(dir $(SRC_S)))

# ---------------------------------------------------------------------------
# Build rules
# ---------------------------------------------------------------------------
ZIP ?= zip

.PHONY: all clean deploy package

all: $(BUILD)/$(APP).hex

$(BUILD)/obj/%.o: %.c | $(BUILD)/obj
	@echo "CC $<"
	@$(CC) $(CFLAGS) -o $@ $<

$(BUILD)/obj/%.o: %.S | $(BUILD)/obj
	@echo "AS $<"
	@$(CC) $(CFLAGS) -x assembler-with-cpp -o $@ $<

$(BUILD)/$(APP).elf: $(OBJ)
	@echo "LINK $@"
	@$(CC) $(LDFLAGS) -o $@ $(OBJ)
	@$(SIZE) $@

$(BUILD)/$(APP).hex: $(BUILD)/$(APP).elf
	@echo "HEX $@"
	@$(OBJCOPY) -O ihex $< $@

$(BUILD)/obj:
	@mkdir -p $@

# ---------------------------------------------------------------------------
# Flash (OpenOCD + in-repo QSPI flashloader). Mirrors ports/psoc-edge deploy.
# ---------------------------------------------------------------------------
ifndef DEV_SERIAL_NUMBER
SERIAL_ADAPTER_CMD =
else
SERIAL_ADAPTER_CMD = -c "adapter serial $(DEV_SERIAL_NUMBER)"
endif

FLM_FILE      := $(BSP_CFG)/PSE84_SMIF.FLM
QSPI_CFG_FILE := $(BSP_CFG)/qspi_config.cfg

# Bundle hex + QSPI flashloader + qspi config so it can be flashed with
# ports/psoc-edge/tools/mp-ifx-flash.py `from-package` (used by CI).
package: $(BUILD)/$(APP).zip
$(BUILD)/$(APP).zip: $(BUILD)/$(APP).hex $(FLM_FILE) $(QSPI_CFG_FILE)
	@echo "ZIP $@"
	@$(ZIP) -q -j $@ $^

deploy: $(BUILD)/$(APP).hex
	$(OPENOCD) \
		-s $(BSP_CFG) \
		-c "set QSPI_FLASHLOADER $(FLM_FILE)" \
		-c "source [find interface/kitprog3.cfg]" \
		$(SERIAL_ADAPTER_CMD) \
		-c "transport select swd" \
		-c "source [find target/infineon/pse84xgxs2.cfg]" \
		-c "init" \
		-c "reset init" \
		-c "adapter speed 12000" \
		-c "flash write_image erase $<" \
		-c "verify_image $<" \
		-c "reset run; shutdown"

clean:
	@rm -rf $(BUILD)
