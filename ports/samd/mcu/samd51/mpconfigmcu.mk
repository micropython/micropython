CFLAGS_MCU += -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS_MCU += -DCFG_TUSB_MCU=OPT_MCU_SAMD51

MPY_CROSS_MCU_ARCH = armv7m

MICROPY_HW_CODESIZE ?= 368K

MICROPY_VFS_LFS2 ?= 1
MICROPY_VFS_FAT ?= 1

ifeq ($(BOARD_VARIANT),WLAN)

MICROPY_PY_NETWORK ?= 1
MICROPY_PY_NETWORK_NINAW10 ?= 1
ifeq ($(MICROPY_HW_CODESIZE),496K)
MICROPY_PY_SSL ?= 1
MICROPY_SSL_MBEDTLS ?= 1
MICROPY_PY_BLUETOOTH ?= 1
MICROPY_BLUETOOTH_NIMBLE ?= 1
endif
ifeq ($(MICROPY_HW_CODESIZE),1008K)
MICROPY_PY_SSL ?= 1
MICROPY_SSL_MBEDTLS ?= 1
MICROPY_PY_BLUETOOTH ?= 1
MICROPY_BLUETOOTH_NIMBLE ?= 1
endif

endif

ifeq ($(MICROPY_PY_NETWORK),1)
FROZEN_MANIFEST ?= mcu/$(MCU_SERIES_LOWER)/manifest_net.py
else
FROZEN_MANIFEST ?= mcu/$(MCU_SERIES_LOWER)/manifest.py
endif

SRC_S += shared/runtime/gchelper_thumb2.s

SRC_C += \
	fatfs_port.c \
    mbedtls/mbedtls_port.c


ifeq ($(MICROPY_PY_NETWORK),1)
CFLAGS += -DMICROPY_PY_NETWORK=1

SHARED_SRC_C += \
    shared/netutils/dhcpserver.c \
    shared/netutils/netutils.c \
    shared/netutils/trace.c
endif

ifeq ($(MICROPY_PY_NETWORK_NINAW10),1)
CFLAGS += -DMICROPY_PY_NETWORK_NINAW10=1
INC += -I$(TOP)/drivers/ninaw10

DRIVERS_SRC_C += \
    drivers/ninaw10/nina_bt_hci.c \
    drivers/ninaw10/nina_wifi_drv.c \
    drivers/ninaw10/nina_wifi_bsp.c \
    drivers/ninaw10/machine_pin_nina.c

SRC_C += \
    nina_wifi_bsp.c

endif

ifeq ($(MICROPY_BLUETOOTH_NIMBLE), 1)
SRC_C += \
    mpbthciport.c \
    mpnimbleport.c

INC += \
	-I$(TOP)/extmod/nimble \
    -I$(TOP)/lib/mynewt-nimble/nimble/host/include \
    -I$(TOP)/lib/mynewt-nimble/nimble/include \
    -I$(TOP)/lib/mynewt-nimble/porting/nimble/include
endif
