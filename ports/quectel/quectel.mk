include plat.mk

PLAT_DFLAGS = $(addprefix -D,$(DFLAGS))
PLAT_CFLAGS = $(QUEC_CFLAGS)
PLAT_CFLAGS += -Wno-unused-parameter -Wformat=0 -Wno-unused-function

INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/config
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/include

ifeq ($(strip $(PLAT)),Qualcomm)
INC += -I$(COMPILER_PATH)/armv7m-none-eabi/libc/include
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/c-utility
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/c-utility/inc
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/c-utility/inc/azure_c_shared_utility
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/c-utility/pal
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/c-utility/pal/generic
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/iothub_client
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/iothub_client/inc
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/iothub_client/inc/internal
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/serializer
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/serializer/inc
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/umqtt
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/umqtt/inc
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/azure_api/umqtt/inc/azure_umqtt_c
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/qapi
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include/threadx_api
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
INC += -I$(ROOT)/system/at
INC += -I$(ROOT)/system/esim
endif

INC += -I.
INC += -I$(HEADER_BUILD)
INC += -I$(TOP)
INC += -I$(TOP)/py
INC += -I$(TOP)/extmod
INC += -I$(TOP)/lib/utils
INC += -I$(TOP)/lib/mp-readline
INC += -I$(TOP)/lib/littlefs
INC += -I$(TOP)/lib/netutils
INC += -I$(TOP)/lib/timeutils
INC += -I$(ROOT)/peripheral
INC += -I$(ROOT)/system/include
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/include
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/boards/$(strip $(BOARD))/include
INC += -I$(ROOT)/system/debug
INC += -I$(ROOT)/system/dev
INC += -I$(ROOT)/system/fs
INC += -I$(ROOT)/system/hal
INC += -I$(ROOT)/system/gnss
INC += -I$(ROOT)/system/network
INC += -I$(ROOT)/system/at
INC += -I$(ROOT)/system/aliiot

ifeq ($(strip $(PLAT)),$(filter $(PLAT),ASR_1803s ASR_1803sc))
INC += -I$(ROOT)/system/csd
INC += -I$(ROOT)/system/esim
endif
ifeq ($(strip $(PLAT)),ASR_1606)
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
endif
ifeq ($(strip $(PLAT)),ASR_1602)
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
endif
ifeq ($(strip $(PLAT)),ASR_1609)
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
endif

ifeq ($(strip $(PLAT)),Unisoc)
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/apps
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/priv
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/prot
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/netif
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix/arpa
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix/net
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix/netinet
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix/sys
INC += -I$(ROOT)/system/at
INC += -I$(ROOT)/system/esim
endif

ifeq ($(strip $(PLAT)),$(filter $(PLAT),Unisoc_8910_R05 Unisoc_8910_R06))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/apps
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/priv
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/prot
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/netif
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/netif/ppp
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/netif/ppp/polarssl
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix/arpa
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix/net
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix/netinet
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix/sys
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/stdc
INC += -I$(ROOT)/system/esim
endif

ifeq ($(strip $(PLAT)),$(filter $(PLAT),EIGEN EIGEN_718))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/lwip
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/lwip/apps
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/lwip/priv
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/lwip/prot
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/arch
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/netif
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/posix
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/posix/sys
endif

ifeq ($(strip $(PLAT)),$(filter $(PLAT),SONY_ALT1350))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/arch
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/lwip
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/lwip/priv
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/lwip/prot
endif

ifeq ($(strip $(PLAT)),$(filter $(PLAT),Unisoc_8850 Unisoc_8850_R02))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/apps
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/priv
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/prot
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/netif
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/netif/ppp
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/netif/ppp/polarssl
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix/arpa
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix/net
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix/netinet
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/posix/sys
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/compat/stdc
endif

ifeq ($(strip $(PLAT)),RDA)
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include/arch
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/apps
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/priv
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/lwip/prot
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/netif
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix/arpa
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix/net
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix/netinet
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/src/include/posix/sys
endif

ifeq ($(strip $(PLAT)), $(filter $(PLAT),ASR ASR_1803s ASR_1803sc))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
INC += -I$(ROOT)/system/at
endif

ifeq ($(strip $(PLAT)),ECR6600)
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/include
endif

ifeq ($(strip $(PLAT)),aic8800m40)
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/lwip-STABLE-2_0_2_RELEASE_VER/ports/rtos/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/lwip-STABLE-2_0_2_RELEASE_VER/ports/rtos/include/arch
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/lwip-STABLE-2_0_2_RELEASE_VER/src/include
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/lwip-STABLE-2_0_2_RELEASE_VER/src/include/lwip
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/lwip-STABLE-2_0_2_RELEASE_VER/src/include/lwip/priv
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/lwip-STABLE-2_0_2_RELEASE_VER/src/include/lwip/prot
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/lwip-STABLE-2_0_2_RELEASE_VER/src/include/netif
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/lwip-STABLE-2_0_2_RELEASE_VER/src/include/posix
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/lwip-STABLE-2_0_2_RELEASE_VER/src/include/posix/sys
INC += -I$(ROOT)/system/lwip/$(strip $(PLAT))/net_al
endif

#ifeq ($(CONFIG_MBEDTLS), 1)
INC += -I$(ROOT)/system/mbedtls
INC += -I$(ROOT)/system/mbedtls/include
INC += -I$(ROOT)/system/mbedtls/include/mbedtls
INC += -I$(ROOT)/system/mbedtls/library
INC += -I$(ROOT)/system/mbedtls/port/helios/inc
#endif

INC += -I$(ROOT)/system/network
INC += -I$(ROOT)/system/os
INC += -I$(ROOT)/system/startup
INC += -I$(ROOT)/system/fota
INC += -I$(ROOT)/system/bt
INC += -I$(ROOT)/system/at
ifeq (y, $(CONFIG_LVGL))
INC += -I$(TOP)/lib/lvgl
INC += -I$(TOP)/lib/lvgl/src
INC += -I$(TOP)/lib/lvgl/src/core
INC += -I$(TOP)/lib/lvgl/src/draw
INC += -I$(TOP)/lib/lvgl/src/extra
INC += -I$(TOP)/lib/lvgl/src/font
INC += -I$(TOP)/lib/lvgl/src/gpu
INC += -I$(TOP)/lib/lvgl/src/hal
INC += -I$(TOP)/lib/lvgl/src/misc
INC += -I$(TOP)/lib/lvgl/src/widgts
endif
INC += -I$(ROOT)/driver
INC += -I$(ROOT)/driver/lcd
ifeq (y, $(CONFIG_QRCODE))
INC += -I$(ROOT)/components/qrcode
endif
ifeq ($(CONFIG_JRTC), y)
INC += -I$(ROOT)/components/jrtc/inc
endif

ifeq ($(CONFIG_SENSOR_VC9202), y)
INC += -I$(ROOT)/components/sensor/VC9202/inc
INC += -I$(ROOT)/components/sensor/VC9202/algo/inc
endif

ifeq (y, $(CONFIG_QUECTHING))
INC += -I$(ROOT)/components/quecsdk/cloud
INC += -I$(ROOT)/components/quecsdk/driverLayer
INC += -I$(ROOT)/components/quecsdk/thirdLib/mqtt
INC += -I$(ROOT)/components/quecsdk/thirdLib/cJSON
INC += -I$(ROOT)/components/quecsdk/kernel
endif
ifeq ($(CONFIG_SPINAND), y)
INC += -I$(ROOT)/components/fs/include
INC += -I$(ROOT)/components/fs/yaffs/src/direct
INC += -I$(ROOT)/components/fs/yaffs/src/port
INC += -I$(ROOT)/components/fs/yaffs/src
INC += -I$(ROOT)/components/fs/yaffs
endif
ifeq ($(CONFIG_RTMP), y)
INC += -I$(ROOT)/components/rtmpdump/librtmp/inc
INC += -I$(ROOT)/components/rtmpdump/libz/inc
endif
INC += -I$(ROOT)/utilities
ifeq ($(CONFIG_JPEG), y)
INC += -I$(ROOT)/components/libjpeg-turbo/include
INC += -I$(ROOT)/components/libjpeg-turbo
INC += -I$(ROOT)/components/libjpeg-turbo/src
INC += -I$(ROOT)/components/libjpeg-turbo/src/simd
endif
ifeq ($(CONFIG_POC_BND), y)
INC += -I$(ROOT)/components/poc
INC += -I$(ROOT)/components/poc/bnd/inc
endif
ifeq ($(CONFIG_POC_BND_XIN), y)
INC += -I$(ROOT)/components/poc
INC += -I$(ROOT)/components/poc/bnd_xin/inc
endif
ifeq ($(CONFIG_POC_SL), y)
INC += -I$(ROOT)/components/poc
INC += -I$(ROOT)/components/poc/sl/inc
endif
ifeq ($(CONFIG_POC_QS), y)
INC += -I$(ROOT)/components/poc
INC += -I$(ROOT)/components/poc/qs/inc
endif
#Carlos.Meng add 2024/02/22
ifeq ($(CONFIG_POC_QS_R07), y)
INC += -I$(ROOT)/components/poc
INC += -I$(ROOT)/components/poc/qs/inc
endif
#Stephen.Gao add 2022/11/23
ifeq ($(CONFIG_POC_ZZD), y)
INC += -I$(ROOT)/components/poc
INC += -I$(ROOT)/components/poc/zzd/inc
endif
#Stephen.Gao add 2023/01/06
ifeq ($(CONFIG_POC_YDWL), y)
INC += -I$(ROOT)/components/poc
INC += -I$(ROOT)/components/poc/ydwl/inc
endif

ifeq ($(CONFIG_POC_SLPOC), y)
INC += -I$(ROOT)/components/poc  -I$(ROOT)/components/poc/slpoc/src
INC += -I$(ROOT)/components/poc/slpoc/inc
SRC_QSTR += $(ROOT)/components/poc/slpoc/src/modpoc_slpoc.c
endif

ifeq ($(CONFIG_POC_RDA_BND), y)
INC += -I$(ROOT)/components/poc  -I$(ROOT)/components/poc/rda_bnd/src
INC += -I$(ROOT)/components/poc/rda_bnd/inc
SRC_QSTR += $(ROOT)/components/poc/rda_bnd/src/modpocrda.c
endif

ifeq ($(CONFIG_POC_RDA_BND_XIN), y)
INC += -I$(ROOT)/components/poc  -I$(ROOT)/components/poc/rda_bnd_xin/src
INC += -I$(ROOT)/components/poc/rda_bnd_xin/inc
SRC_QSTR += $(ROOT)/components/poc/rda_bnd_xin/src/modpocrda.c
endif

ifeq ($(CONFIG_POC_TID), y)
INC += -I$(ROOT)/components/poc  -I$(ROOT)/components/poc/tid/src
INC += -I$(ROOT)/components/poc/tid/inc
SRC_QSTR += $(ROOT)/components/poc/tid/src/modpoc_tid.c
endif

ifeq (y,$(CONFIG_DECODE_ZBAR))
INC += -I$(ROOT)/components/decode/ZBar/zbar
INC += -I$(ROOT)/components/decode/ZBar/include
INC += -I$(ROOT)/components/decode/ZBar/zbar/decoder
INC += -I$(ROOT)/components/decode/ZBar/zbar/qrcode
endif
ifeq (y,$(CONFIG_DECODE_QUECTEL))
INC += -I$(ROOT)/components/decode/quectel/inc
endif

ifeq (y,$(CONFIG_DECODE_QINGYA))
ifneq ($(strip $(PLAT)),ASR)
ifneq ($(strip $(PLAT)),ASR_1606)
INC += -I$(ROOT)/components/decode/qingya
endif
endif
endif

ifeq ($(CONFIG_SPI_SDCARD), y)
INC += -I$(ROOT)/components/fs/fatfs/include
endif

ifeq ($(CONFIG_AUDIO_G711_COMPRESS), y)
INC += -I$(ROOT)/components/audio/G711
endif
ifeq ($(strip $(PLAT)),EIGEN)
INC += -I$(ROOT)/components/audio
endif

ifeq ($(CONFIG_GMSSL), y)
INC += -I$(ROOT)/components/GMSSL/include
INC += -I$(ROOT)/components/GMSSL/include/gmssl
endif

ifeq ($(CONFIG_GB2312), y)
INC += -I$(ROOT)/components/decode/gb2312
endif

ifeq ($(CONFIG_ESIMSDK), y)
INC += -I$(ROOT)/components/esim/inc
endif

ifeq ($(CONFIG_ESIM_IPA), y)
INC += -I$(ROOT)/system/esim
endif

ifeq ($(CONFIG_AUDIO_G729_COMPRESS), y)
INC += -I$(ROOT)/components/audio/G729
INC += -I$(ROOT)/components/audio/G729/src
INC += -I$(ROOT)/components/audio/G729/include
endif

ifeq ($(CONFIG_NFC_SL6320), y)
INC += -I$(ROOT)/components/NFC/SL6320
endif

ifeq ($(CONFIG_VOIP), y)
INC += -I$(ROOT)/components/voip/libexosip2/include
INC += -I$(ROOT)/components/voip/libosip2/include
INC += -I$(ROOT)/components/voip/rtp
endif

ifeq ($(CONFIG_ETHERNET_W5500), y)
INC += -I$(ROOT)/components/ethernet/wiznet
endif

ifeq ($(CONFIG_SLIP), y)
INC += -I$(ROOT)/components/slip
endif

ifeq ($(MICROPY_VFS_QUECFS),1)
QUEC_MOD_CFLAGS += -DMICROPY_VFS_QUECFS=1
endif

QUEC_SRC += \
	main.c \
	mphalport.c \
	mpthreadport.c \
	help.c

# modules source file will used by gen
QUEC_SRC_MOD +=  \
	gccollect.c \
	moduos.c \
	modutime.c \
	modsocket.c \
	modostimer.c \
	modfota.c \
	callbackdeal.c \
	modmachine.c \
	machine_pin.c \
	machine_wdt.c \
	machine_rtc.c \
	machine_uart.c \
	machine_timer.c \
	moddev.c \
	misc_power.c \
	modmisc.c \
	misc_usbnet.c \
	modsim.c \
	modnet.c \
	moddatacall.c \
	machine_extint.c \
	misc_adc.c \
	misc_pwm.c \
	modexample.c \
	machine_iic.c \
	machine_hw_spi.c \
	utime_mphal.c \
	modflashdev.c \
	modussl_mbedtls.c \
	modhelios.c
