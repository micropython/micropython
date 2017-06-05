# this file's folder
SDK_DIR := $(abspath $(dir $(lastword ${MAKEFILE_LIST})))

# -D<define> in CFLAGS
DEFINES += __HEAP_SIZE=0
DEFINES += BLE_STACK_SUPPORT_REQD
DEFINES += CONFIG_GPIO_AS_PINRESET
DEFINES += NRF52
DEFINES += NRF52_PAN_12
DEFINES += NRF52_PAN_15
DEFINES += NRF52_PAN_20
DEFINES += NRF52_PAN_30
DEFINES += NRF52_PAN_31
DEFINES += NRF52_PAN_36
DEFINES += NRF52_PAN_51
DEFINES += NRF52_PAN_53
DEFINES += NRF52_PAN_54
DEFINES += NRF52_PAN_55
DEFINES += NRF52_PAN_58
DEFINES += NRF52_PAN_62
DEFINES += NRF52_PAN_63
DEFINES += NRF52_PAN_64
DEFINES += S132
DEFINES += SOFTDEVICE_PRESENT
DEFINES += SWI_DISABLE0

# nordic SDK C sources (relative path)
SDK_SRC_C += \
	components/ble/ble_advertising/ble_advertising.c \
	components/ble/ble_services/ble_nus/ble_nus.c \
	components/ble/common/ble_advdata.c \
	components/ble/common/ble_conn_params.c \
	components/ble/common/ble_conn_state.c \
	components/ble/common/ble_srv_common.c \
	components/ble/peer_manager/gatt_cache_manager.c \
	components/ble/peer_manager/gattc_cache_manager.c \
	components/ble/peer_manager/gatts_cache_manager.c \
	components/ble/peer_manager/id_manager.c \
	components/ble/peer_manager/peer_data.c \
	components/ble/peer_manager/peer_data_storage.c \
	components/ble/peer_manager/peer_database.c \
	components/ble/peer_manager/peer_id.c \
	components/ble/peer_manager/peer_manager.c \
	components/ble/peer_manager/pm_buffer.c \
	components/ble/peer_manager/pm_mutex.c \
	components/ble/peer_manager/security_dispatcher.c \
	components/ble/peer_manager/security_manager.c \
	components/drivers_nrf/delay/nrf_delay.c \
	components/drivers_nrf/pstorage/pstorage.c \
	components/libraries/fds/fds.c \
	components/libraries/fifo/app_fifo.c \
	components/libraries/fstorage/fstorage.c \
	components/libraries/timer/app_timer.c \
	components/libraries/util/app_util_platform.c \
	components/libraries/util/sdk_mapped_flags.c \
	components/softdevice/common/softdevice_handler/softdevice_handler.c \
	components/toolchain/system_nrf52.c

# add segger RTT
ifeq (${BTYPE}, debug)
DEFINES += USE_RTT
SDK_SRC_C += \
	external/segger_rtt/RTT_Syscalls_GCC.c \
	external/segger_rtt/SEGGER_RTT.c \
	external/segger_rtt/SEGGER_RTT_printf.c
endif

# # nordic SDK ASM sources (relative path)
# SDK_SRC_ASM += \
# 	components/toolchain/gcc/gcc_startup_nrf52.s

# include source folders (sort removes duplicates)
SDK_INC_DIRS += $(sort $(dir ${SDK_SRC_C}))
# ble.h
SDK_INC_DIRS += components/softdevice/s132/headers
# nrf52.h compiler_abstraction.h
SDK_INC_DIRS += components/device
# core_cm4.h
SDK_INC_DIRS += components/toolchain/CMSIS/Include
# section_vars.h
SDK_INC_DIRS += components/libraries/experimental_section_vars
# fstorage_config.h
SDK_INC_DIRS += components/libraries/fstorage/config
# nrf_drv_config.h
SDK_INC_DIRS += components/drivers_nrf/config
# app_util.h
SDK_INC_DIRS += components/libraries/util
# fds_config.h
SDK_INC_DIRS += components/libraries/fds/config

# include full path
INC += $(patsubst %,-I${SDK_DIR}/%, ${SDK_INC_DIRS})

# object folder
NORDIC_BUILD = ${BUILD}/nordic

OBJ += $(addprefix ${NORDIC_BUILD}/, $(SDK_SRC_C:.c=.o))
# OBJ += $(addprefix ${NORDIC_BUILD}/, $(SDK_SRC_ASM:.s=.o))
OBJ += ${NORDIC_BUILD}/gcc_startup_nrf52.o

# linker script folder
LDFLAGS += -L${SDK_DIR}/components/toolchain/gcc

# softdevice .hex file
SOFTDEV_HEX ?= $(lastword $(wildcard nordic/components/softdevice/s132/hex/s132*softdevice.hex))
