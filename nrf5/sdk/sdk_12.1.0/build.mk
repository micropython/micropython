# this file's folder
SDK_DIR := $(SDK_ROOT)

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
DEFINES += NRF_SD_BLE_API_VERSION=3
DEFINES += PEER_MANAGER_ENABLED=1
DEFINES += FDS_ENABLED=1

ifeq ($(SDK_COMPOENTS), 1)
# nordic SDK C sources (relative path)
SDK_SRC_C += \
	components/ble/ble_advertising/ble_advertising.c \
	components/ble/ble_services/ble_nus/ble_nus.c \
	components/ble/common/ble_advdata.c \
	components/ble/common/ble_conn_params.c \
	components/ble/common/ble_conn_state.c \
	components/ble/common/ble_srv_common.c \
	components/ble/peer_manager/gatt_cache_manager.c \
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
	components/libraries/fds/fds.c \
	components/libraries/fifo/app_fifo.c \
	components/libraries/fstorage/fstorage.c \
	components/libraries/timer/app_timer.c \
	components/libraries/util/app_util_platform.c \
	components/libraries/util/sdk_mapped_flags.c \
	components/softdevice/common/softdevice_handler/softdevice_handler.c \
	components/drivers_nrf/clock/nrf_drv_clock.c \
	components/libraries/util/app_error.c \
	components/libraries/util/app_error_weak.c \
	components/drivers_nrf/common/nrf_drv_common.c 
endif

# include source folders (sort removes duplicates)
SDK_INC_DIRS += $(sort $(dir $(SDK_SRC_C)))
#SDK_INC_DIRS += $(sort $(${SDK_ROOT} ${SDK_SRC_C}))
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
# nrf_log.h
SDK_INC_DIRS += components/libraries/log/
# nrf_log_internal.h
SDK_INC_DIRS += components/libraries/log/src 
# nrf_clock.h
SDK_INC_DIRS += components/drivers_nrf/hal
# nrf_drv_common.h
SDK_INC_DIRS += components/drivers_nrf/common
# nrf_delay.h
SDK_INC_DIRS += components/drivers_nrf/delay

CFLAGS += $(patsubst %,-D%,${DEFINES})

# include full path
INC += $(patsubst %,-I${SDK_DIR}/%, ${SDK_INC_DIRS})

# additional SDK source files
SRC_C += $(addprefix ${SDK_ROOT}/, $(SDK_SRC_C))

# Wrappers
SRC_C += \
	$(SDK_MODULES_PATH)nrf52_ble.c \

