#################################################################################
# CM33 secure project

BOARD ?= KIT_PSE84_AI
BOARD_DIR ?= ../boards/$(BOARD)
BUILD ?= ../build-$(BOARD)/secboot

# Include the core environment definitions; this will set $(TOP).
include ../../../py/mkenv.mk

CROSS_COMPILE ?= arm-none-eabi-

PSE_LIB_TOP_DIR ?= lib/psoc-edge
PSE_LIB_DIR ?= $(TOP)/$(PSE_LIB_TOP_DIR)
BOARD_TOP_DIR = ports/psoc-edge/boards/$(BOARD)

DEFINES = \
	-DCOMPONENT_CM33 \
	-DCORE_NAME_CM33_0=1 \
	-DCOMPONENT_MTB_HAL \
	-DCOMPONENT_MW_ASYNC_TRANSFER \
	-DCOMPONENT_MW_MTB_IPC \
	-DCOMPONENT_MW_MTB_SRF \
	-DCOMPONENT_SECURE_DEVICE \
	-DCYBSP_MCUBOOT_HEADER_SIZE=0x400 \

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
	\
	se-rt-services-utils \
	\
	TARGET_$(BOARD) \
	TARGET_$(BOARD)/bluetooth \
	TARGET_$(BOARD)/COMPONENT_CM33/COMPONENT_SECURE_DEVICE \
	)

INC += \
	-I$(BOARD_DIR) \
	-I$(BOARD_DIR)/bsp-cfg \

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
	-mcmse \
	$(INC) \
	$(DEFINES) \

ifeq ($(DEBUG), 1)
CFLAGS += -Og 
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# If the nsc_veneer.o is not present, it is 
# generated without reference.
NSC_VENEER = nsc_veneer.o
ifneq ($(wildcard $(NSC_VENEER)),)
IN_IMPLIB_FLAG = -Wl,--in-implib -Wl,$(NSC_VENEER)
endif

LDFLAGS = $(MACHINE_FLAGS) \
	-Wl,--gc-sections \
	-Wl,--cmse-implib $(IN_IMPLIB_FLAG) -Wl,--out-implib -Wl,$(NSC_VENEER).tmp \
	-Wl,--print-memory-usage \
	-Wl,--no-warn-rwx-segments \
	-Xlinker -L $(PSE_LIB_DIR)/TARGET_$(BOARD)/COMPONENT_CM33/TOOLCHAIN_GCC_ARM \
	-Xlinker -L $(BOARD_DIR)/bsp-cfg \
	-T$(PSE_LIB_DIR)/TARGET_$(BOARD)/COMPONENT_CM33/TOOLCHAIN_GCC_ARM/pse84_s_cm33.ld \
	-Wl,-Map,$(BUILD)/secboot.map

SRC_C = \
	main.c \

PSE_BSP_CFG_C += $(addprefix $(BOARD_TOP_DIR)/bsp-cfg/,\
	cycfg.c \
	cycfg_clocks.c \
	cycfg_peripheral_clocks.c \
	cycfg_peripherals.c \
	cycfg_pins.c \
	cycfg_protection.c \
	cycfg_routing.c \
	cycfg_system.c \
	)

PSE_LIB_SRC_C += $(addprefix $(PSE_LIB_TOP_DIR)/, \
	mtb-dsl-pse8xxgp/pdl/devices/source/cy_device.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_gpio.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_mpc.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_pdl_srf.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_ppc.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_smif.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_sysclk_v2.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syslib.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syspm_pdcm.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syspm_ppu.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_syspm_v4.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_trigmux.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_wdt.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/ppu_v1.c \
	\
	mtb-dsl-pse8xxgp/support/source/mtb_stdlib_stubs.c \
	\
	mtb-srf/source/mtb_srf.c \
	\
	se-rt-services-utils/ifx_se_crc32.c \
	se-rt-services-utils/ifx_se_fih.c \
	se-rt-services-utils/ifx_se_platform.c \
	se-rt-services-utils/ifx_se_syscall.c \
	se-rt-services-utils/ifx_se_syscall_builtin.c \
	\
	TARGET_$(BOARD)/cybsp.c \
	TARGET_$(BOARD)/system_edge.c \
	TARGET_$(BOARD)/COMPONENT_CM33/COMPONENT_SECURE_DEVICE/s_start_pse84.c \
	TARGET_$(BOARD)/COMPONENT_CM33/COMPONENT_SECURE_DEVICE/s_system_init.c \
	TARGET_$(BOARD)/COMPONENT_CM33/COMPONENT_SECURE_DEVICE/s_system_pse84.c \
	)

PSE_SRC_ASM = $(PSE_LIB_TOP_DIR)/mtb-dsl-pse8xxgp/pdl/drivers/source/TOOLCHAIN_GCC_ARM/cy_syslib_ext.S \

OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PSE_BSP_CFG_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PSE_LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PSE_SRC_ASM:.S=.o))

# Main targets

all: $(BUILD)/secboot.hex cmse_post_build_update

# Generate elf file
$(BUILD)/secboot.elf: $(OBJ)
	@echo "Linking... $@"
	@$(CC) $(OBJ) $(LDFLAGS) -o $@
	@$(SIZE) $@

# Compile C files
vpath %.c . $(TOP) | $(BUILD)
$(BUILD)/%.o: %.c | $(BUILD)
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c -MD -MF $(@:.o=.d) -o $@ $<

# Compile S files
vpath %.S . $(TOP) | $(BUILD)
$(BUILD)/%.o: %.S | $(BUILD)
	@mkdir -p $(dir $@)
	@echo "Assembling $<"
	@$(CC) $(CFLAGS) -c -MD -MF $(@:.o=.d) -o $@ $<

# Generate hex file
$(BUILD)/secboot.hex: $(BUILD)/secboot.elf
	@$(OBJCOPY) -O ihex $< $@

# Create build directory
$(BUILD):
	@mkdir -p $(BUILD)

# Replace the original veneer with the generated temp veneer after build.
cmse_post_build_update:
	@$(Q)[ ! -f $(NSC_VENEER).tmp ] || cmp -s $(NSC_VENEER).tmp $(NSC_VENEER) || mv -f $(NSC_VENEER).tmp $(NSC_VENEER);\
	rm -f $(NSC_VENEER).tmp

# Clean
clean:
	@rm -rf $(BUILD)

.PHONY: all clean cmse_post_build_update
