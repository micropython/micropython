
PLAT_DFLAGS = $(addprefix -D,$(DFLAGS))
PLAT_CFLAGS = $(QUEC_CFLAGS)
PLAT_CFLAGS += -Wno-unused-parameter -Wformat=0 -Wno-unused-function

INC += -I./boards/$(BOARD)
INC += -I$(ROOT)/system/platform/$(strip $(PLAT))/include

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
INC += -I$(TOP)/ports/quectel/core
INC += -I$(TOP)/ports/quectel/core/source
INC += -I$(TOP)/ports/quectel/core/py
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

ifeq ($(CONFIG_MBEDTLS), 1)
INC += -I$(ROOT)/system/mbedtls
INC += -I$(ROOT)/system/mbedtls/include
INC += -I$(ROOT)/system/mbedtls/include/mbedtls
INC += -I$(ROOT)/system/mbedtls/library
INC += -I$(ROOT)/system/mbedtls/port/helios/inc
endif

INC += -I$(ROOT)/system/network
INC += -I$(ROOT)/system/os
INC += -I$(ROOT)/system/startup
INC += -I$(ROOT)/system/fota
INC += -I$(ROOT)/system/bt
INC += -I$(ROOT)/system/at

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
	modflashdev.c \
	modhelios.c

# py modules define
QUEC_PY_MOD = modules/modules.py
