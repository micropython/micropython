include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS += qstrdefsport.h $(HEADER_BUILD)/pins_qstr.h

-include mpconfigport.mk

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

SDK = libameba/sdk
LIB = linklibs
TOOLS = tools
HELPER = mphelper
PROJECT = realtek_ameba1_va0_example
BOARD ?= ameba

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------

INC += -I.
INC += -I..
INC += -I$(BUILD)

INC += -I$(HEADER_BUILD)

INC += -I../lib/utils
INC += -I../lib/timeutils
INC += -I../lib/mp-readline

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

INC += -I$(HELPER)
INC += -I$(HELPER)/gccollect
INC += -I$(HELPER)/hal
INC += -I$(HELPER)/mods

INC += -I../stmhal

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
CFLAGS += -nostdlib
CFLAGS += -Werror
CFLAGS += -Wundef
#CFLAGS += -Wpointer-arith
CFLAGS += -Wstrict-prototypes
#CFLAGS += -Wno-write-strings
#CFLAGS += -fsingle-precision-constant
#CFLAGS += -Wdouble-promotion
CFLAGS += -nostartfiles
CFLAGS += -DCONFIG_PLATFORM_8195A $(INC)
CFLAGS += $(CFLAGS_MOD)

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

ELF_ARLIST += ./$(LIB)/lib_wlan.a
ELF_ARLIST += ./$(LIB)/lib_wlan_mp.a 
ELF_ARLIST += ./$(LIB)/lib_wps.a
ELF_ARLIST += ./$(LIB)/lib_mdns.a
ELF_ARLIST += ./$(LIB)/lib_ameba.a
ELF_ARLIST += ./$(LIB)/lib_rtlstd.a
ELF_ARLIST += ./$(LIB)/lib_platform.a

#-------------------------------------------------------------------------------
# C source files and objects
#-------------------------------------------------------------------------------
SRC_C += $(wildcard $(HELPER)/*.c)
SRC_C += $(wildcard $(HELPER)/gccollect/*.c)
SRC_C += $(wildcard $(HELPER)/hal/*.c)
SRC_C += $(wildcard $(HELPER)/mods/*.c)
SRC_C += $(wildcard $(HELPER)/mods/hardware/*.c)
SRC_C += $(wildcard $(HELPER)/mods/wireless/*.c)

#-------------------------------------------------------------------------------
# External C source files and objects
#-------------------------------------------------------------------------------
STM_SRC_C = $(addprefix stmhal/,\
	bufhelper.c \
	)

SRC_C += \
	main.c \
	lib/libc/string0.c \
	lib/utils/pyexec.c \
	lib/mp-readline/readline.c \
	lib/netutils/netutils.c \
	$(SRC_MOD) \
	$(STM_SRC_C)

SRC_S += $(wildcard $(HELPER)/gccollect/*.s)
SRC_S += 

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------
all: $(BUILD)/firmware.elf image

deploy:
	cp $(BUILD)/ram_all.bin /Volumes/MBED/
	sync

$(addprefix $(BUILD)/,$(OBJ)): $(BUILD)/%.o: %.c %.s
	$(Q)$(CC) $(CFLAGS) $< -o $@

# Making OBJ use an order-only dependency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(BUILD)/firmware.elf:| $(BUILD)/pins.o

$(BUILD)/firmware.elf: $(OBJ) $(BUILD)/pins.o
	$(Q)$(ECHO) "BUILDING $@"
	$(Q)$(CC) $(ELF_CFLAGS) $(ELF_LDFLAGS) -o $@ -Wl,--start-group $^ -Wl,--start-group $(ELF_ARLIST) -Wl,--end-group -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys
	$(Q)$(SIZE) $@

MAKE_PINS = boards/make-pins.py
BOARD_PINS = boards/$(BOARD)/pins.csv
AF_FILE = boards/rtl8195a_af.csv
PREFIX_FILE = boards/rtl8195a_prefix.c
GEN_PINS_SRC = $(BUILD)/pins.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(HEADER_BUILD)/pins_qstr.h

$(GEN_PINS_SRC) $(GEN_PINS_HDR) $(GEN_PINS_QSTR): $(BOARD_PINS) $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "CREATING PIN FILES"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) > $(GEN_PINS_SRC)

$(BUILD)/pins.o: $(BUILD)/pins.c
	$(call compile_c)

image: 
	$(Q)$(ECHO) "CREATING IMAGE"
	$(Q)$(RM) -f $(BUILD)/firmware.asm 
	$(Q)$(RM) -f $(BUILD)/firmware.map
	$(Q)$(RM) -f $(BUILD)/*.bin
	$(Q)$(CROSS_COMPILE)nm $(BUILD)/firmware.elf | sort > $(BUILD)/firmware.map
	$(Q)$(CROSS_COMPILE)objdump -d $(BUILD)/firmware.elf >  $(BUILD)/firmware.asm
	$(Q)$(eval RAM2_START_ADDR :=0x$(shell grep __ram_image2_text $(BUILD)/firmware.map | grep _start__ | awk '{print $$1}'))
	$(Q)$(eval RAM2_END_ADDR :=0x$(shell grep __ram_image2_text $(BUILD)/firmware.map | grep _end__ | awk '{print $$1}'))
	$(Q)$(eval RAM3_START_ADDR :=0x$(shell grep __sdram_data_ $(BUILD)/firmware.map | grep _start__ | awk '{print $$1}'))
	$(Q)$(eval RAM3_END_ADDR :=0x$(shell grep __sdram_data_ $(BUILD)/firmware.map | grep _end__ | awk '{print $$1}'))
	$(Q)$(OBJCOPY) -j .image2.start.table -j .ram_image2.text -j .ram.data -Obinary $(BUILD)/firmware.elf $(BUILD)/ram_2.bin
	$(Q)$(OBJCOPY) -j .image3 -j .ARM.exidx -j .sdr_data -Obinary $(BUILD)/firmware.elf $(BUILD)/sdram.bin
	$(Q)$(TOOLS)/mac/pick $(RAM2_START_ADDR) $(RAM2_END_ADDR) $(BUILD)/ram_2.bin $(BUILD)/ram_2.p.bin body+reset_offset+sig
	$(Q)$(TOOLS)/mac/pick $(RAM2_START_ADDR) $(RAM2_END_ADDR) $(BUILD)/ram_2.bin $(BUILD)/ram_2.ns.bin body+reset_offset
	$(Q)$(TOOLS)/mac/pick $(RAM3_START_ADDR) $(RAM3_END_ADDR) $(BUILD)/sdram.bin $(BUILD)/ram_3.p.bin body+reset_offset
	$(Q)$(CP) $(TOOLS)/ram_1.p.bin $(BUILD)/ram_1.p.bin
	$(Q)$(TOOLS)/mac/padding 44k 0xFF $(BUILD)/ram_1.p.bin
	$(Q)cat $(BUILD)/ram_1.p.bin $(BUILD)/ram_2.p.bin $(BUILD)/ram_3.p.bin > $(BUILD)/ram_all.bin
	$(Q)cat $(BUILD)/ram_2.ns.bin $(BUILD)/ram_3.p.bin > $(BUILD)/ota.bin
	$(Q)$(TOOLS)/mac/checksum $(BUILD)/ota.bin

include ../py/mkrules.mk
