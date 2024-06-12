CFLAGS_MCU += -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS_MCU += -DCFG_TUSB_MCU=OPT_MCU_SAMD51

MPY_CROSS_MCU_ARCH = armv7m

MICROPY_HW_CODESIZE ?= 368K

MICROPY_VFS_LFS2 ?= 1
MICROPY_VFS_FAT ?= 1

SRC_S += shared/runtime/gchelper_thumb2.s
SRC_C += \
    fatfs_port.c

ifneq ($(BOARD_VARIANT),)
    MICROPY_PY_NETWORK ?= 1
    CFLAGS += -DMICROPY_PY_NETWORK=1
    MICROPY_PY_NETWORK_NINAW10 ?= 1
    CFLAGS += -DMICROPY_PY_NETWORK_NINAW10=1

    INC += -I$(TOP)/drivers/ninaw10
    SRC_C += \
        mbedtls/mbedtls_port.c\
        nina_wifi_bsp.c
    SHARED_SRC_C += \
        shared/netutils/dhcpserver.c \
        shared/netutils/netutils.c \
        shared/netutils/trace.c
    DRIVERS_SRC_C += \
        drivers/ninaw10/nina_bt_hci.c \
        drivers/ninaw10/nina_wifi_drv.c \
        drivers/ninaw10/nina_wifi_bsp.c \
        drivers/ninaw10/machine_pin_nina.c

    ifeq ($(MICROPY_HW_CODESIZE),$(filter $(MICROPY_HW_CODESIZE), 496K 1008K))
        MICROPY_PY_SSL ?= 1
        MICROPY_SSL_MBEDTLS ?= 1

        ifeq ($(BOARD_VARIANT),WLAN_BLE)
            MICROPY_PY_BLUETOOTH ?= 1
            MICROPY_BLUETOOTH_NIMBLE ?= 1
            SRC_C += \
                mpbthciport.c \
                mpnimbleport.c

            INC += \
                -I$(TOP)/extmod/nimble
        endif
    endif
    FROZEN_MANIFEST ?= mcu/$(MCU_SERIES_LOWER)/manifest_net.py
else
    FROZEN_MANIFEST ?= mcu/$(MCU_SERIES_LOWER)/manifest.py
endif
