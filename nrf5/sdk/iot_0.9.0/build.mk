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
DEFINES += s1xx
DEFINES += SOFTDEVICE_PRESENT
DEFINES += SWI_DISABLE0
DEFINES += NRF_SD_BLE_API_VERSION=3
DEFINES += PEER_MANAGER_ENABLED=1
DEFINES += FDS_ENABLED=1
DEFINES += LWIP_DEBUG=0

# nordic SDK C sources (relative path)
SDK_SRC_C += \
	components/ble/common/ble_advdata.c \
	components/ble/common/ble_conn_params.c \
	components/ble/common/ble_srv_common.c \
	components/libraries/fifo/app_fifo.c \
	components/libraries/timer/app_timer.c \
	components/libraries/util/app_util_platform.c \
	components/softdevice/common/softdevice_handler/softdevice_handler.c \
	components/drivers_nrf/clock/nrf_drv_clock.c \
	components/libraries/util/app_error.c \
	components/drivers_nrf/common/nrf_drv_common.c \
	components/libraries/mem_manager/mem_manager.c \
	components/libraries/trace/app_trace.c \
	components/iot/context_manager/iot_context_manager.c \
	components/iot/iot_timer/iot_timer.c \
	external/lwip/src/core/def.c \
	external/lwip/src/core/dhcp.c \
	external/lwip/src/core/ipv6/dhcp6.c \
	external/lwip/src/core/dns.c \
	external/lwip/src/core/ipv4/icmp.c \
	external/lwip/src/core/ipv6/icmp6.c \
	external/lwip/src/core/ipv6/inet6.c \
	external/lwip/src/core/inet_chksum.c \
	external/lwip/src/core/init.c \
	external/lwip/src/core/ipv4/ip4.c \
	external/lwip/src/core/ipv4/ip4_addr.c \
	external/lwip/src/core/ipv6/ip6.c \
	external/lwip/src/core/ipv6/ip6_addr.c \
	external/lwip/src/core/memp.c \
	external/lwip/src/core/ipv6/mld6.c \
	external/lwip/src/core/ipv6/nd6.c \
	external/lwip/src/core/netif.c \
	external/lwip/src/port/nrf_platform_port.c \
	external/lwip/src/core/pbuf.c \
	external/lwip/src/core/raw.c \
	external/lwip/src/core/sys.c \
	external/lwip/src/core/tcp.c \
	external/lwip/src/core/tcp_in.c \
	external/lwip/src/core/tcp_out.c \
	external/lwip/src/core/timers.c \
	external/lwip/src/core/udp.c \

# include source folders (sort removes duplicates)
SDK_INC_DIRS += $(sort $(dir $(SDK_SRC_C)))
# nrf_drv_config.h
SDK_INC_DIRS += components/drivers_nrf/config
# app_util.h
SDK_INC_DIRS += components/libraries/util
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
# ble_6lowpan.h
SDK_INC_DIRS += components/iot/ble_6lowpan
# ble_ipsp.h
SDK_INC_DIRS += components/iot/ble_ipsp
# iot_defines.h
SDK_INC_DIRS += components/iot/common
# SDK lwip includes
SDK_INC_DIRS += external/lwip/src/port/arch
SDK_INC_DIRS += external/lwip/src/include
SDK_INC_DIRS += external/lwip/src/include/netif
SDK_INC_DIRS += external/lwip/src/port
SDK_INC_DIRS += external/lwip/src/include/lwip

LIBS += $(SDK_ROOT)/components/iot/ble_6lowpan/lib/ble_6lowpan.a

CFLAGS += $(patsubst %,-D%,${DEFINES})

# include full path
INC += $(patsubst %,-I${SDK_DIR}/%, ${SDK_INC_DIRS})

# additional SDK source files
SRC_C += $(addprefix ${SDK_ROOT}/, $(SDK_SRC_C))

# Wrappers
SRC_C += \
	$(SDK_MODULES_PATH)sdkhelp.c \
	$(SDK_MODULES_PATH)modnwble6lowpan.c \


