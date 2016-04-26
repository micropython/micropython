# OS Environment
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


# Path
ROOT = ..
BUILD = ./build
RELEASES_PATH = $(BUILD)/releases

CROSS_COMPILE = arm-none-eabi-

include ../py/mkenv.mk

INCLUDE_PATH += \
    . \
    libs/CMSIS/Include \
    libs/A_user/inc \
    libs/E_rt-dynalib/inc \
    libs/F_wiring/inc \
    libs/H_platform/shared/inc \
    libs/H_platform/MCU/shared/STM32/inc \
    libs/H_platform/MCU/STM32F2xx/STM32_StdPeriph_Driver/inc \
    libs/H_platform/MCU/STM32F2xx/SPARK_Firmware_Driver/inc \
    libs/H_platform/MCU/STM32F2xx/STM32_USB_Device_Driver/inc \
    libs/H_platform/MCU/STM32F2xx/STM32_USB_OTG_Driver/inc \
    libs/H_platform/MCU/STM32F2xx/CMSIS/Device/ST/Include \
    libs/I_modules/duo/user-part/inc \
    libs/I_modules/shared/stm32f2xx/inc \
    libs/X_define \
    libs/X_hal/inc \
    libs/X_hal/shared \
    libs/X_hal/src/stm32f2xx \
    libs/X_hal/src/stm32 \
    libs/X_hal/src/duo \
    libs/X_hal/src/duo/include \
    libs/X_hal/src/duo/libraries/btstack/port \
    libs/X_hal/src/duo/libraries/btstack/src \
    libs/X_hal/src/duo/libraries/btstack/src/ble \
    libs/X_hal/src/duo/libraries/btstack/src/classic \
    libs/X_services/inc \
    libs/X_system/inc \
    libs/X_communication/src \
    libs/X_dynalib/inc \
    libs/X_lib \
    libs/wiring_api
    
SOURCE_PATH += \
    libs/I_modules/duo/user-part/src
    
CPPSOURCE_PATH += \
    . \
    libs/I_modules/duo/user-part/src \
    libs/wiring_api


# Platform configurations
TARGET_NAME = $(RELEASES_PATH)/micropython_duo_firmware

LINKER_FILE = libs/linker_scripts/gcc/linker.ld

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

LDFLAGS += -Llibs/linker_scripts/gcc
LDFLAGS += -Llibs/linker_scripts/linker
LDFLAGS += -Llibs/linker_scripts/linker/stm32f2xx
LDFLAGS += -Llibs/X_lib
LDFLAGS += -Wl,--whole-archive -lSTM32F2xx_Peripheral_Libraries -Wl,--no-whole-archive\
           -nostartfiles -Xlinker --gc-sections \
           -Wl,--whole-archive $(LIBS_DEPS) -Wl,--no-whole-archive\
           -lnosys -Winline\
           -Wl,--defsym,USER_FIRMWARE_IMAGE_SIZE=0x40000 \
           -Wl,--defsym,USER_FIRMWARE_IMAGE_LOCATION=0x80C0000 \
#           -lstdc++_nano -lm \
#           -Wl,--start-group -lgcc -lg_nano -lc_nano -Wl,--end-group \
#           -Wl,--start-group -lgcc -lc_nano -Wl,--end-group

GLOBAL_DEFINES += STM32_DEVICE STM32F2XX PLATFORM_THREADING=1 STM32 USE_STDPERIPH_DRIVER=1 \
                  PLATFORM_ID=88 PLATFORM_NAME=duo REDBEARDUO \
                  SPARK=1  USBD_VID_SPARK=0x2B04 USBD_PID_DFU=0xD058 USBD_PID_CDC=0xC058 \
                  RELEASE_BUILD INCLUDE_PLATFORM=1 USE_STDPERIPH_DRIVER DFU_BUILD_ENABLE \
                  SYSTEM_VERSION_STRING=0.2.3 MODULE_FUNCTION=5 MODULE_INDEX=1 MODULE_VERSION=6 MODULE_DEPENDENCY=4,2,6 \
                  MODULAR_FIRMWARE=1 USER_FIRMWARE_IMAGE_SIZE=0x40000 USER_FIRMWARE_IMAGE_LOCATION=0x80C0000 

ifdef RELEASE
GLOBAL_DEFINES += NO_ASSERT -DRELEASE
endif

ifdef DEBUG
GLOBAL_DEFINES += DEBUG
endif                


# Sources and objects
SOURCES = $(foreach PATH,$(SOURCE_PATH),$(wildcard $(PATH)/*.c))
CPPSOURCES = $(foreach PATH,$(CPPSOURCE_PATH),$(wildcard $(PATH)/*.cpp))
OBJS = $(addprefix $(BUILD)/,$(SOURCES:.c=.o))  $(addprefix $(BUILD)/,$(CPPSOURCES:.cpp=.cpp.o))

# Dependencies
DEPS = $(OBJS:.o=.o.d)
DEPS_MAKEFILES = ./makefile


CFLAGS += $(OPTIMIZEFLAGS) -c $(ARCHFLAGS) $(WARNFLAGS) $(DEBUGFLAGS) $(addprefix -I,$(INCLUDE_PATH)) $(addprefix -D,$(GLOBAL_DEFINES))
CFLAGS += -MD -MP -MF $@.d
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
export MKDIR = mkdir -p
export RMDIR = rm -f -r
export RM = rm -f
ifneq ("1","$(verbose)")
export SILENT = @
endif


$(BUILD)/%.o: %.c $(DEPS_MAKEFILES)
	$(SILENT)echo $(CC) $<
	$(SILENT)$(MKDIR) $(dir $@)
	$(SILENT)$(CC) $(CFLAGS) $< -o $@

$(BUILD)/%.cpp.o: %.cpp $(DEPS_MAKEFILES)
	$(SILENT)echo $(CPP) $<
	$(SILENT)$(MKDIR) $(dir $@)  
	$(SILENT)$(CPP) $(CPPFLAGS)  $< -o $@

$(BUILD)/.s.o: %.s $(DEPS_MAKEFILES)
	$(SILENT)echo $(CC) $<
	$(SILENT)$(MKDIR) $(dir $@)
	$(SILENT)$(CC) $(CFLAGS) $< -o $@
	

$(TARGET_NAME).elf: $(OBJS) $(LINKER_FILE) 
	$(SILENT)echo LD $@ ...
	$(SILENT)$(MKDIR) $(dir $@)
	$(SILENT)$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
 
$(TARGET_NAME).lst: $(TARGET_NAME).elf
	$(SILENT)echo GEN $(TARGET_NAME).lst 
	$(SILENT)$(MKDIR) $(dir $@)
	$(SILENT)$(OBJDUMP) -x -S $(TARGET_NAME).elf > $(TARGET_NAME).lst

$(TARGET_NAME).hex: $(TARGET_NAME).elf
	$(SILENT)echo GEN $@
	$(SILENT)$(MKDIR) $(dir $@)
	$(SILENT)$(OBJCOPY) -O ihex $(TARGET_NAME).elf $(TARGET_NAME).hex

$(TARGET_NAME).bin: $(TARGET_NAME).elf
	$(SILENT)echo GEN $@
	$(SILENT)$(MKDIR) $(dir $@)
	$(SILENT)$(OBJCOPY) -O binary $(TARGET_NAME).elf $(TARGET_NAME).bin
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
	$(SILENT)echo "Using the RBLink to flash"
	openocd -f libs/openocd/redbearduo.cfg -c "program $(TARGET_NAME).bin verify reset exit 0x80c0000"
else
	$(SUDO) dfu-util -d 2b04:d058 -a 0 -s 0x080C0000:leave -D $(TARGET_NAME).bin
endif

clean:
	$(SILENT)echo Cleaning targets ...
	$(SILENT)$(RMDIR) $(BUILD)
	
ifneq ("MAKECMDGOALS","clean")
-include $(DEPS)
endif	
	
