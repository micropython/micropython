include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

SDK = libameba/sdk
LIB = lib

PROJECT = realtek_ameba1_va0_example

INC += -I.
INC += -I..
INC += -I$(BUILD)
INC += -I$(SDK)/project/$(PROJECT)/inc
INC += -I$(SDK)/component/soc/realtek/common/bsp
INC += -I$(SDK)/component/os/freertos
INC += -I$(SDK)/component/os/freertos/freertos_v8.1.2/Source/include
INC += -I$(SDK)/component/os/freertos/freertos_v8.1.2/Source/portable/GCC/ARM_CM3
INC += -I$(SDK)/component/os/os_dep/include
INC += -I$(SDK)/component/soc/realtek/8195a/misc/driver
INC += -I$(SDK)/component/common/api/network/include
INC += -I$(SDK)/component/common/api
INC += -I$(SDK)/component/common/api/platform
INC += -I$(SDK)/component/common/api/wifi
INC += -I$(SDK)/component/common/api/wifi/rtw_wpa_supplicant/src
INC += -I$(SDK)/component/common/application
INC += -I$(SDK)/component/common/example
INC += -I$(SDK)/component/common/example/wlan_fast_connect
INC += -I$(SDK)/component/common/mbed/api
INC += -I$(SDK)/component/common/mbed/hal
INC += -I$(SDK)/component/common/mbed/hal_ext
INC += -I$(SDK)/component/common/mbed/targets/hal/rtl8195a
INC += -I$(SDK)/component/common/network
INC += -I$(SDK)/component/common/network/lwip/lwip_v1.4.1/port/realtek/freertos
INC += -I$(SDK)/component/common/network/lwip/lwip_v1.4.1/src/include
INC += -I$(SDK)/component/common/network/lwip/lwip_v1.4.1/src/include/lwip
INC += -I$(SDK)/component/common/network/lwip/lwip_v1.4.1/src/include/ipv4
INC += -I$(SDK)/component/common/network/lwip/lwip_v1.4.1/port/realtek
INC += -I$(SDK)/component/common/test
INC += -I$(SDK)/component/soc/realtek/8195a/cmsis
INC += -I$(SDK)/component/soc/realtek/8195a/cmsis/device
INC += -I$(SDK)/component/soc/realtek/8195a/fwlib
INC += -I$(SDK)/component/soc/realtek/8195a/fwlib/rtl8195a
INC += -I$(SDK)/component/common/drivers/wlan/realtek/include
INC += -I$(SDK)/component/common/drivers/wlan/realtek/src/osdep
INC += -I$(SDK)/component/common/drivers/wlan/realtek/src/hci
INC += -I$(SDK)/component/common/drivers/wlan/realtek/src/hal
INC += -I$(SDK)/component/common/drivers/wlan/realtek/src/hal/OUTSRC
INC += -I$(SDK)/component/soc/realtek/8195a/fwlib/ram_lib/wlan/realtek/wlan_ram_map/rom
INC += -I$(SDK)/component/common/network/ssl/polarssl-1.3.8/include
INC += -I$(SDK)/component/common/network/ssl/ssl_ram_map/rom
INC += -I$(SDK)/component/common/utilities
INC += -I$(SDK)/component/soc/realtek/8195a/misc/rtl_std_lib/include
INC += -I$(SDK)/component/soc/realtek/8195a/fwlib/ram_lib/usb_otg/include
INC += -I$(SDK)/component/common/video/v4l2/inc
INC += -I$(SDK)/component/common/media/codec
INC += -I$(SDK)/component/common/drivers/usb_class/host/uvc/inc

#-------------------------------------------------------------------------------
# Flags
#-------------------------------------------------------------------------------
CFLAGS += -c
CFLAGS += -g
CFLAGS += -w
CFLAGS += -mcpu=cortex-m3
CFLAGS += -mtune=cortex-m3
CFLAGS += -mthumb
CFLAGS += -O2
CFLAGS += -ansi
CFLAGS += -std=gnu99
CFLAGS += -fno-short-enums
CFLAGS += -fno-common
CFLAGS += -fmessage-length=0
CFLAGS += -Wall
CFLAGS += -fno-exceptions
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -fomit-frame-pointer
CFLAGS += -fno-short-enums
#CFLAGS += -nostdlib
#CFLAGS += -Werror
#CFLAGS += -Wundef
#CFLAGS += -Wpointer-arith
#CFLAGS += -Wstrict-prototypes
#CFLAGS += -Wno-write-strings
#CFLAGS += -fsingle-precision-constant
#CFLAGS += -Wdouble-promotion
#CFLAGS += -nostartfiles
CFLAGS += -DCONFIG_PLATFORM_8195A $(INC)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

ELF_CFLAGS += -O2
ELF_CFLAGS += -Wl,--gc-sections
ELF_CFLAGS += -mcpu=cortex-m3
ELF_CFLAGS += -mthumb
ELF_CFLAGS += --specs=nano.specs

ELF_LDFLAGS += -L./scripts -L. -T./scripts/rlx8195A-symbol-v03-img2_arduino_arduino.ld
ELF_LDFLAGS += -Wl,-Map=$@.map
ELF_LDFLAGS += -Wl,--cref
ELF_LDFLAGS += -Wl,--warn-common
ELF_LDFLAGS += -Wl,--gc-sections
ELF_LDFLAGS += -Wl,--no-enum-size-warning
ELF_LDFLAGS += -Wl,--no-wchar-size-warning
ELF_LDFLAGS += -Wl,--entry=InfraStart
ELF_LDFLAGS += -Wl,-nostdlib

ELF_ARLIST += ./lib/lib_wlan.a
ELF_ARLIST += ./lib/lib_wlan_mp.a 
ELF_ARLIST += ./lib/lib_wps.a
ELF_ARLIST += ./lib/lib_mdns.a
ELF_ARLIST += ./lib/lib_ameba.a
ELF_ARLIST += ./lib/lib_rtlstd.a
ELF_ARLIST += ./lib/lib_platform.a

#-------------------------------------------------------------------------------
# C source files and objects
#-------------------------------------------------------------------------------

# SRC_C += $(wildcard $(SRC)/targets/hal/target_rtk/target_8195a/src/*.c)

SRC_C += \
	main.c \
	lib/libc/string0.c \
	#printf.c \
	string0.c \
	malloc0.c \
	gccollect.c \

#SRC_S += $(wildcard $(SRC)/source/*.s)
SRC_S += \
#	startup_stm32f40xx.s \
	gchelper.s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------
all: $(BUILD)/firmware.elf

$(addprefix $(BUILD)/,$(OBJ)): $(BUILD)/%.o: %.c
	$(Q)$(CC) $(CFLAGS) $< -o $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "BUILDING $@"
	$(Q)$(CC) $(ELF_CFLAGS) $(ELF_LDFLAGS) -o $@ -Wl,--start-group $^ -Wl,--start-group $(ELF_ARLIST) -Wl,--end-group -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys
	$(Q)$(SIZE) $@

deploy:
	@./makebin/makebin.sh

include ../py/mkrules.mk
