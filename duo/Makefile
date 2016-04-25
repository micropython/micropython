ifeq ($(shell uname),Darwin)
MACOSX=1
CFLAGS+=-D__MACOSX__
STAT_FLAGS='-f ''%z'''
MAKE_OS=OSX
SUDO=sudo
else
STAT_FLAGS='-c ''%s'''
MAKE_OS=LINUX
SUDO=sudo
endif

ifeq ($(OS),Windows_NT)
MINGW=1
MAKE_OS=WINDOWS
SUDO=
endif

ifeq (WINDOWS,$(MAKE_OS))
    filesize=`stat --print %s $1`
else
  ifeq (LINUX, $(MAKE_OS))
    filesize=`stat -c %s $1`
  else
    filesize=`stat -f%z $1`
  endif
endif

ifndef SINGLETHREAD
MAKEFLAGS=-j5 # multicore
endif

CWD = .
ROOT = $(CWD)/..
TARGET_NAME = micropython_duo_firmware

SOURCES = 
CPPSOURCES =
INCLUDE_PATH=
LIBS=
DEFINES=
CFLAGS= 
CPPFLAGS=
LDFLAGS=

ifdef RELEASE
DEFINES += -DNO_ASSERT -DRELEASE
endif

ifdef DEBUG
DEFINES+=-DDEBUG
endif

INCLUDE_PATH += -I$(CWD)/libs/CMSIS/Include
INCLUDE_PATH += -I$(CWD)/libs/A_user/inc
INCLUDE_PATH += -I$(CWD)/libs/E_rt-dynalib/inc
INCLUDE_PATH += -I$(CWD)/libs/F_wiring/inc
INCLUDE_PATH += -I$(CWD)/libs/H_platform/shared/inc
INCLUDE_PATH += -I$(CWD)/libs/H_platform/MCU/shared/STM32/inc
INCLUDE_PATH += -I$(CWD)/libs/H_platform/MCU/STM32F2xx/STM32_StdPeriph_Driver/inc
INCLUDE_PATH += -I$(CWD)/libs/H_platform/MCU/STM32F2xx/SPARK_Firmware_Driver/inc
INCLUDE_PATH += -I$(CWD)/libs/H_platform/MCU/STM32F2xx/STM32_USB_Device_Driver/inc
INCLUDE_PATH += -I$(CWD)/libs/H_platform/MCU/STM32F2xx/STM32_USB_OTG_Driver/inc
INCLUDE_PATH += -I$(CWD)/libs/H_platform/MCU/STM32F2xx/CMSIS/Device/ST/Include
INCLUDE_PATH += -I$(CWD)/libs/I_modules/duo/user-part/inc
INCLUDE_PATH += -I$(CWD)/libs/I_modules/shared/stm32f2xx/inc
INCLUDE_PATH += -I$(CWD)/libs/X_define
INCLUDE_PATH += -I$(CWD)/libs/X_hal/inc
INCLUDE_PATH += -I$(CWD)/libs/X_hal/shared
INCLUDE_PATH += -I$(CWD)/libs/X_hal/src/stm32f2xx
INCLUDE_PATH += -I$(CWD)/libs/X_hal/src/stm32
INCLUDE_PATH += -I$(CWD)/libs/X_hal/src/duo
INCLUDE_PATH += -I$(CWD)/libs/X_hal/src/duo/include
INCLUDE_PATH += -I$(CWD)/libs/X_hal/src/duo/libraries/btstack/port
INCLUDE_PATH += -I$(CWD)/libs/X_hal/src/duo/libraries/btstack/src
INCLUDE_PATH += -I$(CWD)/libs/X_hal/src/duo/libraries/btstack/src/ble
INCLUDE_PATH += -I$(CWD)/libs/X_hal/src/duo/libraries/btstack/src/classic
INCLUDE_PATH += -I$(CWD)/libs/X_services/inc
INCLUDE_PATH += -I$(CWD)/libs/X_system/inc
INCLUDE_PATH += -I$(CWD)/libs/X_communication/src
INCLUDE_PATH += -I$(CWD)/libs/X_dynalib/inc
INCLUDE_PATH += -I$(CWD)/libs/X_lib
INCLUDE_PATH += -I$(CWD)/libs/wiring_api

CPPSOURCES += \
  $(CWD)/main.cpp \
  $(CWD)/libs/I_modules/duo/user-part/src/newlib_stubs.cpp \
  $(CWD)/libs/wiring_api/usartserial_api.cpp \
  $(CWD)/libs/wiring_api/usbserial_api.cpp \
  $(CWD)/libs/wiring_api/gpio_api.cpp \
  $(CWD)/libs/wiring_api/servo_api.cpp \
  $(CWD)/libs/wiring_api/ble_api.cpp \
  $(CWD)/libs/wiring_api/i2c_api.cpp \
  $(CWD)/libs/wiring_api/spi_api.cpp \
  $(CWD)/libs/wiring_api/tone_api.cpp \
  $(CWD)/libs/wiring_api/rgb_api.cpp \
  $(CWD)/libs/wiring_api/wifi_api.cpp \
  $(CWD)/libs/wiring_api/tcpserver_api.cpp \
  $(CWD)/libs/wiring_api/tcpclient_api.cpp \
  $(CWD)/libs/wiring_api/timer_api.cpp \
  $(CWD)/libs/wiring_api/interrupt_api.cpp

SOURCES += \
  $(CWD)/libs/I_modules/duo/user-part/src/module_info.c \
  $(CWD)/libs/I_modules/duo/user-part/src/user_export.c \
  $(CWD)/libs/I_modules/duo/user-part/src/user_module.c 

LINKER_FILE = $(CWD)/libs/linker_scripts/gcc/linker.ld

# warnning flags
WARNFLAGS += -Wall -Wextra -Wno-switch -Wno-error=deprecated-declarations -Werror=implicit-function-declaration -fno-strict-aliasing \
             -Wno-conversion -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-value

# optimize flags
OPTIMIZEFLAGS += -Os -ffunction-sections -fdata-sections -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc \
                 -fno-builtin -fmessage-length=0 -fno-strict-aliasing -fno-common -fno-exceptions -flto -fno-fat-lto-objects \
                 -Wl,--allow-multiple-definition --param inline-unit-growth=6

# achitecture flags
ARCHFLAGS += -mcpu=cortex-m3 -mthumb

# debug flags
DEBUGFLAGS += -g3 -gdwarf-2

# libs and search path
LIBS_DEPS += -lcommunication-dynalib -lhal-dynalib -lplatform -lrt-dynalib -lservices-dynalib \
             -lsystem-dynalib -lwiring_globals -lwiring 

LDFLAGS += -L$(CWD)/libs/linker_scripts/gcc
LDFLAGS += -L$(CWD)/libs/linker_scripts/linker
LDFLAGS += -L$(CWD)/libs/linker_scripts/linker/stm32f2xx
LDFLAGS += -L$(CWD)/libs/X_lib
LDFLAGS += -Wl,--whole-archive -lSTM32F2xx_Peripheral_Libraries -Wl,--no-whole-archive\
           -nostartfiles -Xlinker --gc-sections \
           -Wl,--whole-archive $(LIBS_DEPS) -Wl,--no-whole-archive\
           -lnosys -Winline\
           -Wl,--defsym,USER_FIRMWARE_IMAGE_SIZE=0x40000 \
           -Wl,--defsym,USER_FIRMWARE_IMAGE_LOCATION=0x80C0000 \
#           -lstdc++_nano -lm \
#           -Wl,--start-group -lgcc -lg_nano -lc_nano -Wl,--end-group \
#           -Wl,--start-group -lgcc -lc_nano -Wl,--end-group

DEFINES += -DSTM32_DEVICE -DSTM32F2XX -DPLATFORM_THREADING=1 -DSTM32 -DUSE_STDPERIPH_DRIVER=1\
           -DPLATFORM_ID=88 -DPLATFORM_NAME=duo -DREDBEARDUO\
           -DUSBD_VID_SPARK=0x2B04 -DUSBD_PID_DFU=0xD058 -DUSBD_PID_CDC=0xC058 \
           -DRELEASE_BUILD -DINCLUDE_PLATFORM=1 -DUSE_STDPERIPH_DRIVER -DDFU_BUILD_ENABLE \
           -DSYSTEM_VERSION_STRING=0.2.3 \
           -DUSER_FIRMWARE_IMAGE_SIZE=0x40000 -DUSER_FIRMWARE_IMAGE_LOCATION=0x80C0000 \
           -DMODULAR_FIRMWARE=1 -DMODULE_FUNCTION=5 -DMODULE_INDEX=1 -DMODULE_VERSION=6 -DMODULE_DEPENDENCY=4,2,6 \
           -DSPARK=1 

OBJS = $(SOURCES:.c=.o) $(CPPSOURCES:.cpp=.cpp.o)

CFLAGS += $(OPTIMIZEFLAGS) -c $(ARCHFLAGS) $(DEFINES) $(INCLUDE_PATH) $(WARNFLAGS) $(DEBUGFLAGS)
CPPFLAGS += $(CFLAGS) -std=gnu++11
LDFLAGS += $(OPTIMIZEFLAGS) $(ARCHFLAGS) -T$(LINKER_FILE)

export CC = arm-none-eabi-gcc
export CPP = arm-none-eabi-g++
export LD = arm-none-eabi-g++
export AR = arm-none-eabi-ar
export AS = arm-none-eabi-as
export OBJCOPY = arm-none-eabi-objcopy
export OBJDUMP = arm-none-eabi-objdump
export GDB = arm-none-eabi-gdb

compile = $(CC) $(CFLAGS) $< -o $@
cppcompile = $(CPP) $(CPPFLAGS)  $< -o $@
link = $(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
obj_dump = $(OBJDUMP) -x -S $(TARGET_NAME).elf > $(TARGET_NAME).lst
obj_to_bin = $(OBJCOPY) -O $1 $(TARGET_NAME).elf $(TARGET_NAME).$2


%.o: %.c 
	@echo $(CC) $<
	@$(call compile)

%.cpp.o: %.cpp 
	@echo $(CPP) $<
	@$(call cppcompile)

.s.o:
	@echo $(CC) $<
	@$(call compile)

$(TARGET_NAME).elf: $(OBJS) $(LINKER_FILE)
	@echo LD $@ ...
	@$(call link)

$(TARGET_NAME).lst : $(TARGET_NAME).elf
	@echo GEN $(TARGET_NAME).lst 
	@$(call obj_dump)

$(TARGET_NAME).hex: $(TARGET_NAME).elf
	@echo GEN $@
	@$(call obj_to_bin,ihex,hex)

$(TARGET_NAME).srec : $(TARGET_NAME).elf
	@echo GEN $@
	@$(call obj_to_bin,srec,srec)

$(TARGET_NAME).bin : $(TARGET_NAME).elf
	@echo GEN $@
	@$(call obj_to_bin,binary,bin)
	if [ -s $@ ]; then \
	head -c $$(($(call filesize,$@) - 38)) $@ > $@.no_crc && \
	tail -c 38 $@ > $@.crc_block && \
	test "0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20280078563412" = `xxd -p -c 500 $@.crc_block` && \
	shasum -a 256 $@.no_crc | cut -c 1-65 | xxd -r -p | dd bs=1 of=$@ seek=$$(($(call filesize,$@) - 38)) conv=notrunc  && \
	head -c $$(($(call filesize,$@) - 4)) $@ > $@.no_crc && \
	crc32 $@.no_crc | cut -c 1-10 | xxd -r -p | dd bs=1 of=$@ seek=$$(($(call filesize,$@) - 4)) conv=notrunc ;\
	fi  
	
	
.PHONY:  proj

all: 	 proj

proj: $(TARGET_NAME).lst $(TARGET_NAME).bin $(TARGET_NAME).hex  

flash: all
ifdef RBLINK
	@echo "Using the RBLink to flash"
	openocd -f $(CWD)/libs/openocd/redbearduo.cfg -c "program $(TARGET_NAME).bin verify reset exit 0x80c0000"
else
	$(SUDO) dfu-util -d 2b04:d058 -a 0 -s 0x080C0000:leave -D $(TARGET_NAME).bin
endif

clean:
	@echo Cleaning targets
	@find . -name \*.o | grep -v libmbed | grep -v arm-bcm2708 | xargs rm -f
	@rm -f $(TARGET_NAME).elf
	@rm -f $(TARGET_NAME).hex
	@rm -f $(TARGET_NAME).bin
	@rm -f $(TARGET_NAME).lst
	@rm -f $(TARGET_NAME).bin.crc_block
	@rm -f $(TARGET_NAME).bin.no_crc
	
	