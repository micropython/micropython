################################################################################
# Initial setup of Makefile environment

BOARD ?= KIT_PSE84_AI
BOARD_DIR ?= boards/$(BOARD)
BUILD ?= build-$(BOARD)

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

# qstr definitions (must come before including py.mk).
QSTR_DEFS += qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES += $(BOARD_DIR)/mpconfigboard.h 

# Frozen manifest for VFS support
MICROPY_FROZEN_MANIFEST = $(BOARD_DIR)/manifest.py
FROZEN_MANIFEST         = $(MICROPY_FROZEN_MANIFEST)

ifneq ($(FROZEN_MANIFEST),)
  CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
  CFLAGS += -DMICROPY_MODULE_FROZEN_MPY=1
  CFLAGS += -DMICROPY_MODULE_FROZEN_STR=1
endif

# Include the core environment definitions; this will set $(TOP).
include ../../py/mkenv.mk

# Port and board specific make settings.
include mpconfigport.mk
include $(BOARD_DIR)/mpconfigboard.mk

# Include py core make definitions.
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

################################################################################
# Project specific settings and compiler/linker flags

# Required git submodules.
GIT_SUBMODULES += $(addprefix lib/psoc-edge/,\
    async-transfer \
	cmsis \
	core-lib \
	mtb-dsl-pse8xxgp \
	mtb-ipc \
	mtb-srf \
	retarget-io \
	se-rt-services-utils \
	serial-memory \
	TARGET_KIT_PSE84_AI \
)

# Define toolchain and other tools.
CROSS_COMPILE ?= arm-none-eabi-
EDGEPROTECTTOOLS ?= edgeprotecttools
OPENOCD ?= openocd

# SDK locations.
PSE_LIB_TOP_DIR ?= lib/psoc-edge
PSE_LIB_DIR ?= $(TOP)/$(PSE_LIB_TOP_DIR)

################################################################################
# Pin generation

HEADER_BUILD := $(BUILD)/genhdr
MAKE_PINS = boards/make-pins.py
BOARD_PINS = $(BOARD_DIR)/pins.csv
AF_FILE = boards/pse8x_af.csv
PREFIX_FILE = boards/pse84_prefix.c
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h

# Making OBJ use an order-only dependency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(GEN_PINS_HDR)

# With conditional pins, we may need to regenerate qstrdefs.h when config
# options change.
$(HEADER_BUILD)/qstrdefs.generated.h: $(BOARD_DIR)/mpconfigboard.h

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_$(BOARD).c and pins.h
$(BUILD)/%_$(BOARD).c $(HEADER_BUILD)/%.h : $(BOARD_DIR)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board-csv $(BOARD_PINS) --af-csv $(AF_FILE) --prefix $(PREFIX_FILE) \
	    --output-source $(GEN_PINS_SRC) --output-header $(GEN_PINS_HDR)

#################################################################################
# PSOC Edge source files and libraries

DEFINES = \
	-DCOMPONENT_CM33 \
	-DCORE_NAME_CM33_0=1 \
	-DCOMPONENT_MTB_HAL \
	-DCOMPONENT_MW_ASYNC_TRANSFER \
	-DCOMPONENT_MW_MTB_IPC \
	-DCOMPONENT_MW_MTB_SRF \
	-DCOMPONENT_NON_SECURE_DEVICE \
	-DCYBSP_MCUBOOT_HEADER_SIZE=0x400 \
	-DCY_RETARGET_IO_CONVERT_LF_TO_CRLF \

INC += $(addprefix -I$(PSE_LIB_DIR)/,\
	async-transfer/include \
	\
	cmsis/Core/Include \
	cmsis/Core/Include/a-profile \
	cmsis/Core/Include/m-profile \
	cmsis/Core/Include/r-profile \
	\
	core-lib/include \
	\
	mtb-dsl-pse8xxgp/device-utils/syspm/hobto/pdm_pcm/audioss_pdm \
	mtb-dsl-pse8xxgp/device-utils/syspm/hobto/pdm_pcm/mxpdm \
	mtb-dsl-pse8xxgp/device-utils/syspm/include \
	\
	mtb-dsl-pse8xxgp/hal/include \
	\
	mtb-dsl-pse8xxgp/nnkernel/COMPONENT_CM33/include \
	\
	mtb-dsl-pse8xxgp/pdl/devices/include \
	mtb-dsl-pse8xxgp/pdl/devices/include/ip \
	mtb-dsl-pse8xxgp/pdl/drivers/include \
	mtb-dsl-pse8xxgp/pdl/drivers/third_party/ethernet/include \
	\
	mtb-ipc/include \
	\
	mtb-srf/include \
	mtb-srf/include/COMPONENT_NON_SECURE_DEVICE \
	mtb-srf/include/COMPONENT_NON_SECURE_DEVICE/COMPONENT_MW_MTB_IPC \
	\
	retarget-io/include \
	\
	serial-memory/include \
	\
	se-rt-services-utils \
	\
	TARGET_$(BOARD) \
	TARGET_$(BOARD)/bluetooth \
	)

INC += \
    -I$(BOARD_DIR) \
    -I$(BOARD_DIR)/bsp-cfg \
	-I$(BOARD_DIR)/mtb-prj \
	-Idualcore/include

MACHINE_FLAGS = \
	-mcpu=cortex-m33 \
	--specs=nano.specs \
	-mfloat-abi=softfp \
	-mfpu=fpv5-sp-d16 \
	-mthumb

CFLAGS += -c $(MACHINE_FLAGS) \
	-ffat-lto-objects \
	-g \
	-Wall \
	-pipe \
	$(DEFINES) \

LDFLAGS += $(MACHINE_FLAGS) \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage \
	-Xlinker -L $(PSE_LIB_DIR)/TARGET_$(BOARD)/COMPONENT_CM33/TOOLCHAIN_GCC_ARM \
	-Xlinker -L $(BOARD_DIR)/bsp-cfg \
	-T$(PSE_LIB_DIR)/TARGET_$(BOARD)/COMPONENT_CM33/TOOLCHAIN_GCC_ARM/pse84_ns_cm33.ld \
	-Wl,-Map,$(BUILD)/firmware.map \

PSE_BSP_CFG_C += $(addprefix $(BOARD_DIR)/bsp-cfg/,\
	cycfg.c \
	cycfg_clocks.c \
	cycfg_dmas.c \
	cycfg_peripheral_clocks.c \
	cycfg_peripherals.c \
	cycfg_pins.c \
	cycfg_protection.c \
	cycfg_qspi_memslot.c \
	cycfg_routing.c \
	cycfg_system.c \
	)

# TODO: Move this retarget init to the uart driver.
PSE_BSP_SRC_C += $(addprefix $(BOARD_DIR)/,\
	mtb-prj/retarget_io_init.c \
	)

PSE_LIB_SRC_C += $(addprefix $(PSE_LIB_TOP_DIR)/, \
	async-transfer/source/mtb_async_transfer.c \
	\
	mtb-dsl-pse8xxgp/device-utils/syspm/source/mtb_syspm_callbacks_scb_uart.c \
	\
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_clock.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_gpio.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_system.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_timer.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_uart.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_utils_impl.c \
	\
	mtb-dsl-pse8xxgp/pdl/devices/source/cy_device.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_gpio.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_ipc_drv.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_ipc_pipe.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_ipc_sema.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_pdm_pcm_v2.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_rtc.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_scb_common.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_scb_i2c.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_scb_uart.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_smif.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_smif_hb_flash.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_smif_memnum.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_smif_memslot.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_smif_sfdp.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_sysclk_v2.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_sysint_v2.c \
    mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syslib.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syspm_pdcm.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syspm_ppu.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syspm_v4.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_tcpwm_counter.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/ppu_v1.c \
	\
	mtb-dsl-pse8xxgp/support/source/mtb_stdlib_stubs.c \
	\
	mtb-ipc/source/mtb_ipc.c \
	\
	mtb-srf/source/COMPONENT_NON_SECURE_DEVICE/mtb_srf_pool.c \
	mtb-srf/source/COMPONENT_NON_SECURE_DEVICE/COMPONENT_MW_MTB_IPC/mtb_srf_ipc.c \
	\
	retarget-io/source/cy_retarget_io.c \
	\
	serial-memory/source/mtb_serial_memory.c \
	\
    TARGET_$(BOARD)/cybsp.c \
	TARGET_$(BOARD)/system_edge.c \
    TARGET_$(BOARD)/COMPONENT_CM33/COMPONENT_NON_SECURE_DEVICE/ns_start_pse84.c \
    TARGET_$(BOARD)/COMPONENT_CM33/COMPONENT_NON_SECURE_DEVICE/ns_system_pse84.c \
	)

PSE_SRC_ASM = $(PSE_LIB_TOP_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/TOOLCHAIN_GCC_ARM/cy_syslib_ext.S \

PSE_SRC_O = secboot/nsc_veneer.o

OBJ += $(PSE_SRC_O)
OBJ += $(addprefix $(BUILD)/, $(PSE_BSP_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PSE_BSP_CFG_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PSE_LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PSE_SRC_ASM:.S=.o))

################################################################################
# MicroPython source files and libraries

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS += $(INC)

# Select debugging or optimisation build.
ifeq ($(DEBUG), 1)
CFLAGS += -Og 
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

AFLAGS = -mthumb -march=armv8.1-m.main+fp+mve.fp

LDFLAGS +=

LIBS += -lm

# PSOC Edge uses Cortex-M33 and Cortex-M55 (ARMv8-M architecture)
# However, since ARMv8-M is not directly supported by mpy-cross,
# we use armv7emsp as the closest compatible architecture
MPY_CROSS_FLAGS += -march=armv7emsp 

SRC_O += \
	shared/runtime/gchelper_thumb2.o \

SHARED_SRC_C += $(addprefix shared/,\
	readline/readline.c \
	\
	runtime/gchelper_native.c \
	runtime/interrupt_char.c \
	runtime/mpirq.c \
	runtime/pyexec.c \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	\
	timeutils/timeutils.c \
	)

MOD_SRC_C += \
	machine_pin.c \
	machine_pin_af.c \
	machine_pin_irq.c \
	machine_rtc.c \
	modpsocedge.c \

ifeq ($(MICROPY_PY_EXT_FLASH),1)
	CFLAGS += -DMICROPY_ENABLE_EXT_QSPI_FLASH=1
	MOD_SRC_C += psoc_edge_qspi_flash.c 
endif

SRC_C += \
	help.c \
	main.c \
	mphalport.c \

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SHARED_SRC_C) $(MOD_SRC_C) $(GEN_PINS_SRC)

# Define the required object files.
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(MOD_SRC_C:.c=.o)) 
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(GEN_PINS_SRC:.c=.o)

################################################################################
# Secondary targets

## Secure targets
##
## Build, sign and flash separately 
## the secure boot hex.

build_s: $(BUILD)/secboot/secboot.hex

$(BUILD)/secboot/secboot.hex:
	$(Q)$(MAKE) -C secboot BOARD=$(BOARD)

deploy_s: $(BUILD)/secboot/secboot.signed.hex
	$(call flash_target,$<)

sign_s: $(BUILD)/secboot/secboot.signed.hex
$(BUILD)/secboot/secboot.signed.hex: $(BUILD)/secboot/secboot.hex
	@echo "Reallocating hex file..."
	@$(EDGEPROTECTTOOLS) run-config -i config/sign_s.json \
	--set s_hex $^ \
	--set s_signed_hex $@ \
	--symbol-search $(BOARD_DIR)/bsp-cfg

clean_s:
	$(Q)$(MAKE) -C secboot  BOARD=$(BOARD) clean

.PHONY: build_s sign_s deploy_s clean_s

## Non-secure targets 
##
## Build, shift and deploy separately
## the non-secure hex file when the secure hex 
## is already build, signed and flashed in the
## device 

build_ns: $(BUILD)/firmware.hex

shift_ns: $(BUILD)/firmware.shifted.hex
$(BUILD)/firmware.shifted.hex: $(BUILD)/firmware.hex
	@echo "Reallocating hex file..."
	@$(EDGEPROTECTTOOLS) run-config -i config/shift_ns.json \
	--set ns_hex $^ \
	--set ns_shifted_hex $@ \
	--symbol-search $(BOARD_DIR)/bsp-cfg

deploy_ns: $(BUILD)/firmware.shifted.hex
	$(call flash_target,$<)

.PHONY: build_ns shift_ns deploy_ns

## Openocd reusable flashing target

# Deploy on an specific serial device if passed
ifndef DEV_SERIAL_NUMBER
SERIAL_ADAPTER_CMD =
else
SERIAL_ADAPTER_CMD = -c "adapter serial $(DEV_SERIAL_NUMBER)"
endif

OPENOCD_OPEN_CMDS = \
	-s $(BOARD_DIR)/bsp-cfg \
	-c "set QSPI_FLASHLOADER $(BOARD_DIR)/bsp-cfg/PSE84_SMIF.FLM" \
	-c "source [find interface/kitprog3.cfg]" \
	$(SERIAL_ADAPTER_CMD) \
	-c "transport select swd" \
 	-c "source [find target/infineon/pse84xgxs2.cfg]" \
	-c "init" \
	-c "reset init" \
	-c "adapter speed 12000" \

OPENOCD_CLOSE_CMDS = \
	-c "reset run; shutdown" \


define flash_target
    $(Q)$(OPENOCD) \
        $(OPENOCD_OPEN_CMDS) \
        -c "flash write_image erase $(1)" \
        -c "verify_image $(1)" \
        $(OPENOCD_CLOSE_CMDS)
endef

## Multiple devices deploy targets
##
## Supports test and CI HIL operations.

ifndef EXT_HEX_FILE
HEX_FILE = $(BUILD)/firmware.signed.merged.hex
else
HEX_FILE = $(EXT_HEX_FILE)
endif

qdeploy:
	$(call flash_target,$(HEX_FILE))

# When multiple types of boards are connected, a devs file needs to be provided.
# When working locally, if a "local-devs.yml" file is placed in "tools/psoc-edge"
# it will be used
ifneq ($(DEVS_FILE),)
MULTI_BOARD_DEVS_OPTS = -f name=$(BOARD) --devs-yml $(DEVS_FILE)
else 
DFLT_LOCAL_DEVS_FILE_NAME = local-devs.yml
LOCAL_DEVS_FILE=$(TOP)/tools/psoc-edge/$(DFLT_LOCAL_DEVS_FILE_NAME)
ifneq (,$(wildcard $(LOCAL_DEVS_FILE)))
MULTI_BOARD_DEVS_OPTS = -f name=$(BOARD) --devs-yml $(LOCAL_DEVS_FILE)
endif
endif

attached_devs:
	@:
	$(eval ATTACHED_TARGET_LIST = $(shell etdevs-query serial_number $(MULTI_BOARD_DEVS_OPTS)))
	$(eval ATTACHED_TARGETS_NUMBER = $(words $(ATTACHED_TARGET_LIST)))
	$(info Number of attached targets : $(ATTACHED_TARGETS_NUMBER))
	$(info List of attached targets : $(ATTACHED_TARGET_LIST))

qdeploy_multi: attached_devs
	$(foreach ATTACHED_TARGET, $(ATTACHED_TARGET_LIST), $(MAKE) qdeploy DEV_SERIAL_NUMBER=$(ATTACHED_TARGET);)

deploy_multi: all qdeploy_multi

.PHONY: qdeploy qdeploy_multi deploy_multi attached_devs

################################################################################
# Main targets

all: $(BUILD)/firmware.signed.merged.hex

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "Link $@"
	@$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	@$(Q)$(SIZE) $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	@$(Q)$(OBJCOPY) -O ihex $^ $@

sign_merge: $(BUILD)/firmware.signed.merged.hex
$(BUILD)/firmware.signed.merged.hex: $(BUILD)/secboot/secboot.hex $(BUILD)/firmware.hex 
	@echo "Signing and combining hex files..."
	@$(EDGEPROTECTTOOLS) run-config -i config/sign_shift_merge.json \
	--set s_hex $(word 1,$^) \
	--set s_signed_hex $(BUILD)/secboot/secboot.signed.hex \
	--set ns_hex $(word 2,$^) \
	--set ns_shifted_hex $(BUILD)/firmware.shifted.hex \
	--set signed_merged_hex $@ \
	--symbol-search $(BOARD_DIR)/bsp-cfg

deploy: $(BUILD)/firmware.signed.merged.hex
	$(call flash_target,$<)

.PHONY: all sign_merge deploy 

###############################################################################
# Help target

help:
	@:
	$(info )
	$(info Main targets:)
	$(info )
	$(info all                      Build the MicroPython firmware)
	$(info submodules               Initialize port required port submodules)
	$(info deploy                   Deploy the MicroPython firmware on the board)
	$(info clean                    Remove the board build directory)
	$(info help                     Show this help message)	
	$(info )
	$(info Secondary targets:)
	$(info )
	$(info build_s                  Build separately the secure boot signed firmware)
	$(info deploy_s                 Deploy separately the secure boot signed firmware)		
	$(info clean_s                  Remove the secure boot build directory)
	$(info build_ns                 Build separately the non secure firmware.)
	$(info deploy_ns                Deploy separately the non secure shifted firmware)
	$(info sign_s                   Sign the secure boot firmware)
	$(info shift_ns                 Reallocate the non secure firmware )		
	$(info sign_merge               Sign the secure boot firmware, reallocate the non)
	$(info ..                       secure firmware, and merge them into a single file) 
	$(info )
	$(info Test and CI support targets:)
	$(info )
	$(info qdeploy                  Deploy the MicroPython firmware without rebuilding)
	$(info qdeploy_multi            Deploy the MicroPython firmware on multiple devices)
	$(info ..                       without rebuilding)
	$(info deploy_multi             Deploy the MicroPython firmware on multiple devices)
	$(info )
	$(info Options for build targets:)
	$(info - BOARD                  Target PSOC Edge MicroPython board)
	$(info )
	$(info Options for deploy targets:)
	$(info - DEV_SERIAL_NUMBER      Serial number of the target to program) 
	$(info ..                       when multiple devices are connected)
	$(info - EXT_HEX_FILE           An external .hex file can be provided to the deploy)
	$(info ..                       targets, instead of building from the sources) 
	$(info )

.PHONY: help

################################################################################
# Remaining make rules

# Include remaining core make rules.
include $(TOP)/py/mkrules.mk
