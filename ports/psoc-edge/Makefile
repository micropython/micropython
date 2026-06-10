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
FROZEN_MANIFEST ?= boards/manifest.py

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

# extmod/extmod.mk enables mbedTLS sources based on MICROPY_SSL_MBEDTLS.
ifeq ($(MICROPY_SSL_MBEDTLS),1)
MBEDTLS_CONFIG_FILE = \"mbedtls/mbedtls_config_port.h\"
endif

# Include py core make definitions.
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

################################################################################
# Project specific settings and compiler/linker flags

# Required git submodules.
GIT_SUBMODULES += lib/CMSIS_6
GIT_SUBMODULES += $(addprefix lib/psoc-edge/,\
	abstraction-rtos \
    async-transfer \
	clib-support \
	connectivity-utilities \
	core-lib \
	cy-mbedtls-acceleration \
	freertos \
	lwip \
	lwip-freertos-integration \
	lwip-network-interface-integration \
	mtb-dsl-pse8xxgp \
	mtb-ipc \
	mtb-srf \
	retarget-io \
	secure-sockets \
	se-rt-services-utils \
	serial-memory \
	TARGET_KIT_PSE84_AI \
	whd-bsp-integration \
	wifi-connection-manager \
	wifi-host-driver \
	wifi-resources \
	wpa3-external-supplicant \
)

GIT_SUBMODULES += lib/mbedtls
GIT_SUBMODULES += lib/mpy-test-ext

# Define toolchain and other tools.
CROSS_COMPILE ?= arm-none-eabi-
EDGEPROTECTTOOLS ?= edgeprotecttools
OPENOCD ?= openocd

# SDK locations.
PSE_LIB_TOP_DIR ?= lib/psoc-edge
PSE_LIB_DIR ?= $(TOP)/$(PSE_LIB_TOP_DIR)

# WHD resource configuration for standalone MicroPython build.
WHD_FW_FILE ?= $(TOP)/$(PSE_LIB_TOP_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/resources/firmware/COMPONENT_55500/COMPONENT_SM/55500A1.trxcse
WHD_NVRAM_FILE ?= $(TOP)/$(PSE_LIB_TOP_DIR)/wifi-resources/nvram/COMPONENT_WIFI6/COMPONENT_55500/COMPONENT_MURATA-2FY/cyw55500-sdio-mur-2FY.txt
WHD_CLM_FILE ?= $(TOP)/$(PSE_LIB_TOP_DIR)/wifi-resources/clm/COMPONENT_WIFI6/COMPONENT_55500/COMPONENT_CYW55513_MOD_PSE84_SOM/55500A1.clm_blob

################################################################################
# Pin generation

HEADER_BUILD := $(BUILD)/genhdr
MAKE_PINS = boards/make-pins.py
BOARD_PINS = $(BOARD_DIR)/pins.csv
AF_FILE = boards/pse8x_af.csv
PREFIX_FILE = boards/pse84_prefix.c
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_AF_HDR= $(HEADER_BUILD)/pins_af.h

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
	    --output-source $(GEN_PINS_SRC) --output-header $(GEN_PINS_HDR) --output-af-header $(GEN_PINS_AF_HDR)

#################################################################################
# libs source files and libraries

INC += $(addprefix -I$(TOP)/lib/,\
	CMSIS_6/CMSIS/Core/Include \
	CMSIS_6/CMSIS/Core/Include/a-profile \
	CMSIS_6/CMSIS/Core/Include/m-profile \
	CMSIS_6/CMSIS/Core/Include/r-profile \
)

#################################################################################
# PSOC Edge source files and libraries

DEFINES = \
	-DCOMPONENT_CM33 \
	-DCOMPONENT_FREERTOS \
	-DCOMPONENT_PSE84 \
	-DCYBSP_WIFI_CAPABLE \
	-DCOMPONENT_WIFI_INTERFACE_SDIO \
	-DCORE_NAME_CM33_0=1 \
	-DCOMPONENT_MTB_HAL \
	-DCOMPONENT_MW_ASYNC_TRANSFER \
	-DCOMPONENT_MW_MTB_IPC \
	-DCOMPONENT_MW_MTB_SRF \
	-DCOMPONENT_NON_SECURE_DEVICE \
	-DCY_RTOS_AWARE \
	-DBLHS_SUPPORT \
	-DCY_WIFI_COUNTRY=WHD_COUNTRY_UNITED_STATES \
	-DCYBSP_WIFI_WL_HOSTWAKE_DRIVE_MODE=MTB_HAL_GPIO_DRIVE_OPENDRAINDRIVESLOW \
	-DCYBSP_WIFI_WL_HOSTWAKE_INIT_STATE=WHD_TRUE \
	-DCYBSP_MCUBOOT_HEADER_SIZE=0x400 \
	-DCY_RETARGET_IO_CONVERT_LF_TO_CRLF \

INC += $(addprefix -I$(PSE_LIB_DIR)/,\
	async-transfer/include \
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

ifeq ($(MICROPY_PY_NETWORK_IFX_WCM),1)
WHD_FW_SIZE := $(shell stat -c%s $(WHD_FW_FILE))
WHD_NVRAM_SIZE := $(shell stat -c%s $(WHD_NVRAM_FILE))
WHD_CLM_SIZE := $(shell stat -c%s $(WHD_CLM_FILE))

INC += \
	-I$(PSE_LIB_DIR)/wifi-host-driver \
	-I$(PSE_LIB_DIR)/abstraction-rtos/include \
	-I$(PSE_LIB_DIR)/abstraction-rtos/include/COMPONENT_FREERTOS \
	-I$(PSE_LIB_DIR)/clib-support/include \
	-I$(PSE_LIB_DIR)/clib-support/source/TOOLCHAIN_GCC_ARM \
	-I$(PSE_LIB_DIR)/connectivity-utilities \
	-I$(PSE_LIB_DIR)/connectivity-utilities/cy_log \
	-I$(PSE_LIB_DIR)/connectivity-utilities/network \
	-I$(PSE_LIB_DIR)/freertos/Source/include \
	-I$(PSE_LIB_DIR)/freertos/Source/portable/COMPONENT_CM33 \
	-I$(PSE_LIB_DIR)/freertos/Source/portable/COMPONENT_CM33/TOOLCHAIN_GCC_ARM/COMPONENT_FREERTOS_NTZ \
	-I$(PSE_LIB_DIR)/lwip-network-interface-integration/include \
	-I$(PSE_LIB_DIR)/lwip-network-interface-integration/source \
	-I$(PSE_LIB_DIR)/lwip-freertos-integration/arch \
	-I$(PSE_LIB_DIR)/lwip/src/include \
	-I$(PSE_LIB_DIR)/whd-bsp-integration \
	-I$(PSE_LIB_DIR)/wifi-connection-manager/include \
	-I$(PSE_LIB_DIR)/wifi-connection-manager/source/COMPONENT_55500 \
	-I$(PSE_LIB_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/inc \
	-I$(PSE_LIB_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/resources \
	-I$(PSE_LIB_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/resources/firmware \
	-I$(PSE_LIB_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/resources/resource_imp \
	-I$(PSE_LIB_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/src \
	-I$(PSE_LIB_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/src/include

DEFINES += \
	-DFW_IMAGE_SIZE=$(WHD_FW_SIZE) -DFW_IMAGE_NAME=\"$(WHD_FW_FILE)\" \
	-DNVRAM_IMAGE_SIZE=$(WHD_NVRAM_SIZE) -DNVRAM_IMAGE_NAME=\"$(WHD_NVRAM_FILE)\" \
	-DCLM_IMAGE_SIZE=$(WHD_CLM_SIZE) -DCLM_IMAGE_NAME=\"$(WHD_CLM_FILE)\" \
	-UWHD_PRINT_VERSION_ENABLE \
	-DWHD_PRINT_DISABLE
endif

MACHINE_FLAGS = \
	-mcpu=cortex-m33 \
	--specs=nano.specs \
	--specs=nosys.specs \
	-mfloat-abi=hard \
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
	-T$(TOP)/ports/psoc-edge/mpy_gc_assert.ld \
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

PSE_BSP_SRC_C += \
	dualcore/source/COMPONENT_CM33/cm33_ipc_communication.c \


PSE_LIB_SRC_C += $(addprefix $(PSE_LIB_TOP_DIR)/, \
	async-transfer/source/mtb_async_transfer.c \
	\
	mtb-dsl-pse8xxgp/device-utils/syspm/source/mtb_syspm_callbacks_scb_uart.c \
	\
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_clock.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_gpio.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_lptimer.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_lptimer_common.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_sdhc.c \
	mtb-dsl-pse8xxgp/hal/source/mtb_hal_syspm.c \
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
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_crypto_core_hw.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_crypto_core_trng.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_rtc.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_scb_common.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_scb_i2c.c \
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_sd_host.c \
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
	mtb-dsl-pse8xxgp/pdl/drivers/source/cy_tcpwm_pwm.c \
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

ifeq ($(MICROPY_PY_NETWORK_IFX_WCM),1)
PSE_LIB_SRC_C += \
	$(PSE_LIB_TOP_DIR)/abstraction-rtos/source/cy_worker_thread.c \
	$(PSE_LIB_TOP_DIR)/abstraction-rtos/source/COMPONENT_FREERTOS/cyabs_freertos_common.c \
	$(PSE_LIB_TOP_DIR)/abstraction-rtos/source/COMPONENT_FREERTOS/cyabs_freertos_helpers.c \
	$(PSE_LIB_TOP_DIR)/abstraction-rtos/source/COMPONENT_FREERTOS/cyabs_rtos_freertos.c \
	$(PSE_LIB_TOP_DIR)/clib-support/source/cy_time.c \
	$(PSE_LIB_TOP_DIR)/clib-support/source/COMPONENT_FREERTOS/cy_mutex_pool.c \
	$(PSE_LIB_TOP_DIR)/clib-support/source/TOOLCHAIN_GCC_ARM/cy_clib_support_newlib.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/croutine.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/event_groups.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/list.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/queue.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/stream_buffer.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/tasks.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/timers.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/portable/COMPONENT_CM33/TOOLCHAIN_GCC_ARM/COMPONENT_FREERTOS_NTZ/port.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/portable/COMPONENT_CM33/TOOLCHAIN_GCC_ARM/COMPONENT_FREERTOS_NTZ/portasm.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/portable/COMPONENT_CM33/TOOLCHAIN_GCC_ARM/COMPONENT_FREERTOS_NTZ/portdsram.c \
	$(PSE_LIB_TOP_DIR)/freertos/Source/portable/MemMang/heap_3.c \
	$(PSE_LIB_TOP_DIR)/connectivity-utilities/network/cy_nw_helper_common.c \
	$(PSE_LIB_TOP_DIR)/wifi-connection-manager/source/cy_wcm.c \
	$(PSE_LIB_TOP_DIR)/whd-bsp-integration/COMPONENT_LWIP/cy_network_buffer_lwip.c \
	$(PSE_LIB_TOP_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/resources/resource_imp/whd_resources.c \
	$(PSE_LIB_TOP_DIR)/lwip-network-interface-integration/source/cy_ethernetif.c \
	$(PSE_LIB_TOP_DIR)/lwip-network-interface-integration/source/cy_lwip_dhcp_server.c \
	$(PSE_LIB_TOP_DIR)/lwip-network-interface-integration/source/cy_network_mw_core.c \
	$(PSE_LIB_TOP_DIR)/lwip-freertos-integration/arch/sys_arch.c \
	$(PSE_LIB_TOP_DIR)/whd-bsp-integration/cybsp_wifi.c \
	$(patsubst $(TOP)/%,%,$(wildcard $(TOP)/$(PSE_LIB_TOP_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/src/*.c)) \
	$(patsubst $(TOP)/%,%,$(wildcard $(TOP)/$(PSE_LIB_TOP_DIR)/wifi-host-driver/WHD/COMPONENT_WIFI6/src/bus_protocols/*.c)) \
	$(patsubst $(TOP)/%,%,$(wildcard $(TOP)/$(PSE_LIB_TOP_DIR)/lwip/src/api/*.c)) \
	$(patsubst $(TOP)/%,%,$(wildcard $(TOP)/$(PSE_LIB_TOP_DIR)/lwip/src/core/*.c)) \
	$(patsubst $(TOP)/%,%,$(wildcard $(TOP)/$(PSE_LIB_TOP_DIR)/lwip/src/core/ipv4/*.c)) \
	$(patsubst $(TOP)/%,%,$(wildcard $(TOP)/$(PSE_LIB_TOP_DIR)/lwip/src/core/ipv6/*.c)) \
	$(patsubst $(TOP)/%,%,$(wildcard $(TOP)/$(PSE_LIB_TOP_DIR)/lwip/src/netif/*.c))
endif

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

ifeq ($(MICROPY_PSOC_EDGE_LWIP),1)
	INC += -Ilwip_inc
endif

ifeq ($(MICROPY_SSL_MBEDTLS),1)
  INC += -I$(TOP)/extmod/mbedtls
	INC += -I$(TOP)/ports/psoc-edge/mbedtls
	CFLAGS += -DMICROPY_SSL_MBEDTLS=1 -DMICROPY_MBEDTLS_USE_MTB_ENTROPY=1
endif

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

ifeq ($(MICROPY_PSOC_EDGE_LWIP),1)
  SHARED_SRC_C += $(addprefix shared/,\
	netutils/dhcpserver.c \
	netutils/netutils.c \
	netutils/trace.c \
  )
endif

MOD_SRC_C += \
	machine_pin.c \
	machine_pin_af.c \
	machine_pin_irq.c \
	machine_pdm_pcm_extmod.c\
	machine_i2c.c\
	machine_rtc.c \
	modpsocedge.c \
	machine_ipc.c \
	machine_scb.c \

ifeq ($(MICROPY_PY_EXT_FLASH),1)
	CFLAGS += -DMICROPY_ENABLE_EXT_QSPI_FLASH=1
	MOD_SRC_C += psoc_edge_qspi_flash.c 
endif

ifeq ($(MICROPY_PY_NETWORK_IFX_WCM),1)
CFLAGS += -DMICROPY_PY_NETWORK=1 -DMICROPY_PY_NETWORK_IFX_WCM=1 -Wno-stringop-truncation
	MOD_SRC_C += network_ifx_wcm.c 
	MOD_SRC_C += modsocket.c
endif

# Size of the C-malloc heap carved from the linker .heap zone (sbrk/malloc).
# FreeRTOS uses its own static heap (configTOTAL_HEAP_SIZE) and does NOT draw
# from this zone.  WiFi (WHD/WCM/LwIP) does, so it needs a larger reservation.
# The GC heap receives the remainder: [__HeapBase+MICROPY_C_HEAP_SIZE .. __HeapLimit].
# Defaults can be overridden on the make command line, e.g. make MICROPY_C_HEAP_SIZE=0x20000.
ifeq ($(MICROPY_PY_NETWORK_IFX_WCM),1)
MICROPY_C_HEAP_SIZE ?= 0x16000
else
MICROPY_C_HEAP_SIZE ?= 0x8000
endif

# GC heap split: carve [__HeapBase .. __HeapBase+MICROPY_C_HEAP_SIZE) for C malloc
# and give the remainder to the MicroPython GC heap.
LDFLAGS += -Wl,--defsym=__GcHeapStart=__HeapBase+$(MICROPY_C_HEAP_SIZE)
LDFLAGS += -Wl,--defsym=__GcHeapEnd=__HeapLimit

SRC_C += \
	help.c \
	main.c \
	mphalport.c \
	sys_int.c \

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
	$(Q)$(MAKE) -C secboot BOARD=$(BOARD) BUILD=$(abspath $(BUILD)/secboot)

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
	$(Q)$(MAKE) -C secboot  BOARD=$(BOARD) BUILD=$(abspath $(BUILD)/secboot) clean

.PHONY: build_s sign_s deploy_s clean_s

## Non-secure targets 
##
## Build, shift and deploy separately
## the non-secure hex file when the secure hex 
## is already build, signed and flashed in the
## device 

build_ns: $(BUILD)/firmware.unshifted.hex

shift_ns: $(BUILD)/firmware.shifted.hex
$(BUILD)/firmware.shifted.hex: $(BUILD)/firmware.unshifted.hex
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

FLM_FILE = $(BOARD_DIR)/bsp-cfg/PSE84_SMIF.FLM
QSPI_CFG_FILE = $(BOARD_DIR)/bsp-cfg/qspi_config.cfg

OPENOCD_OPEN_CMDS = \
	-s $(BOARD_DIR)/bsp-cfg \
	-c "set QSPI_FLASHLOADER $(FLM_FILE)" \
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

all: $(BUILD)/firmware.hex $(BUILD)/firmware.zip

$(BUILD)/firmware.unshifted.elf: $(OBJ)
	$(ECHO) "Link $@"
	@$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	@$(Q)$(SIZE) $@

$(BUILD)/firmware.unshifted.hex: $(BUILD)/firmware.unshifted.elf
	$(ECHO) "Create $@"
	@$(Q)$(OBJCOPY) -O ihex $^ $@

sign_merge: $(BUILD)/firmware.hex
$(BUILD)/firmware.hex: $(BUILD)/secboot/secboot.hex $(BUILD)/firmware.unshifted.hex 
	@echo "Signing and combining hex files..."
	@$(EDGEPROTECTTOOLS) run-config -i config/sign_shift_merge.json \
	--set s_hex $(word 1,$^) \
	--set s_signed_hex $(BUILD)/secboot/secboot.signed.hex \
	--set ns_hex $(word 2,$^) \
	--set ns_shifted_hex $(BUILD)/firmware.shifted.hex \
	--set signed_merged_hex $@ \
	--symbol-search $(BOARD_DIR)/bsp-cfg

deploy: $(BUILD)/firmware.hex
	$(call flash_target,$<)

zip: $(BUILD)/firmware.zip
$(BUILD)/firmware.zip: $(BUILD)/firmware.hex $(FLM_FILE) $(QSPI_CFG_FILE)
	$(ECHO) "Create $@"
	$(Q)$(ZIP) -q -j $@ $^

.PHONY: all sign_merge deploy zip

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
