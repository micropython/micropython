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
MAKEFLAGS=-j1 # multicore
endif


# Path
ROOT = ..
BUILD = ./build
RELEASES_PATH = $(BUILD)/releases

CROSS_COMPILE = arm-none-eabi-

TARGET_NAME = $(RELEASES_PATH)/micropython_duo_firmware

include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(BUILD)/pins_qstr.h $(BUILD)/modstm_qstr.h

# include MicroPython make definitions
include ../py/py.mk

HAL_INCLUDE_PATH += \
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
    
INC += $(addprefix -I,$(HAL_INCLUDE_PATH))
INC += -I.
INC += -I..
INC += -Iboards
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr
INC += -I../lib/mp-readline
INC += -I../lib/netutils
INC += -I../lib/timeutils

LINKER_FILE = libs/linker_scripts/gcc/linker.ld

# warnning flags
WARNFLAGS += -Wall -Wno-switch -Wno-error=deprecated-declarations -Werror=implicit-function-declaration -fno-strict-aliasing \
             -Wno-conversion -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-value -Wdouble-promotion

# optimize flags
OPTIMIZEFLAGS += -Os -ffunction-sections -fdata-sections -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc \
                 -fno-builtin -fmessage-length=0 -fno-strict-aliasing -fno-common -fno-exceptions -flto -fno-fat-lto-objects \
                 -Wl,--allow-multiple-definition --param inline-unit-growth=6 -fsingle-precision-constant

# achitecture flags
ARCHFLAGS += -mtune=cortex-m3 -mcpu=cortex-m3 -mthumb

# debug flags
ifdef DEBUG
DEBUGFLAGS += -g3 -gdwarf-2
endif 

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


HAL_SOURCE_PATH += \
    libs/I_modules/duo/user-part/src
    
HAL_CPPSOURCE_PATH += \
    . \
    libs/I_modules/duo/user-part/src \
    libs/wiring_api
    
# Sources and objects
HAL_SOURCES = $(foreach PATH,$(HAL_SOURCE_PATH),$(wildcard $(PATH)/*.c))
HAL_CPPSOURCES = $(foreach PATH,$(HAL_CPPSOURCE_PATH),$(wildcard $(PATH)/*.cpp))

LIB_SOURCES = $(addprefix lib/,\
	libc/string0.c \
	libm/math.c \
	libm/asinfacosf.c \
	libm/atanf.c \
	libm/atan2f.c \
	libm/fmodf.c \
	libm/roundf.c \
	libm/log1pf.c \
	libm/acoshf.c \
	libm/asinhf.c \
	libm/atanhf.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/erf_lgamma.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/sf_erf.c \
	libm/wf_lgamma.c \
	libm/wf_tgamma.c \
	fatfs/ff.c \
	mp-readline/readline.c \
	netutils/netutils.c \
	timeutils/timeutils.c \
	utils/pyexec.c \
	utils/pyhelp.c \
	utils/printf.c \
	)

MP_SOURCE = \
    mphalport.c \
    pin.c \
    pin_defs_stmhal.c \
    wiring.c \
	stmhal/pybstdio.c \
	modpyb.c \
	adc.c \

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/,$(HAL_SOURCES:.c=.o))
OBJ += $(addprefix $(BUILD)/,$(HAL_CPPSOURCES:.cpp=.cpp.o))
OBJ += $(addprefix $(BUILD)/,$(LIB_SOURCES:.c=.o))
OBJ += $(addprefix $(BUILD)/,$(MP_SOURCE:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))
OBJ += $(BUILD)/pins.o

# Dependencies
DEPS_MAKEFILES = ./Makefile


CFLAGS += $(OPTIMIZEFLAGS) $(ARCHFLAGS) $(WARNFLAGS) $(DEBUGFLAGS) $(INC) $(addprefix -D,$(GLOBAL_DEFINES))
CFLAGS += -std=gnu99 -nostdlib
CPPFLAGS += $(filter-out -std=gnu99, $(CFLAGS)) -std=gnu++11
LDFLAGS += $(OPTIMIZEFLAGS) $(ARCHFLAGS) -T$(LINKER_FILE) -Wl,-Map=$(@:.elf=.map),--cref


export LD = arm-none-eabi-g++

all: $(TARGET_NAME).bin $(TARGET_NAME).hex

ifneq ($(FROZEN_DIR),)
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
OBJ += $(BUILD)/frozen-files.o
MAKE_FROZEN = ../tools/make-frozen.py

$(BUILD)/frozen-files.o: $(BUILD)/frozen-files.c
	$(call compile_c)

$(BUILD)/frozen-files.c: $(shell find $(FROZEN_DIR) -type f)
	@$(ECHO) "Creating $@"
	$(Q)$(PYTHON) $(MAKE_FROZEN) $(FROZEN_DIR) > $@
endif

ifneq ($(FROZEN_MPY_DIR),)
# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen (be sure to build from scratch).
FROZEN_MPY_PY_FILES := $(wildcard $(FROZEN_MPY_DIR)/*.py)
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
OBJ += $(BUILD)/$(FROZEN_MPY_DIR)/frozen_mpy.o
MPY_CROSS = ../mpy-cross/mpy-cross
MPY_TOOL = ../tools/mpy-tool.py

$(BUILD)/$(FROZEN_MPY_DIR)/%.mpy: $(FROZEN_MPY_DIR)/%.py
	@$(ECHO) "MPY $<"
	$(Q)$(MPY_CROSS) -o $@ $^

$(BUILD)/$(FROZEN_MPY_DIR)/frozen_mpy.c: $(FROZEN_MPY_MPY_FILES) $(BUILD)/genhdr/qstrdefs.generated.h
	@$(ECHO) "Creating $@"
	$(Q)$(PYTHON) $(MPY_TOOL) -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h $(FROZEN_MPY_MPY_FILES) > $@

$(BUILD)/$(FROZEN_MPY_DIR)/frozen_mpy.o: $(BUILD)/$(FROZEN_MPY_DIR)/frozen_mpy.c
	$(call compile_c)
endif

.PHONY: flash

$(BUILD)/%.cpp.o: %.cpp
	$(Q)echo $(CXX) $<
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(CXX) $(CPPFLAGS) -c $< -o $@

$(TARGET_NAME).elf: $(OBJ) $(LINKER_FILE) $(DEPS_MAKEFILES)
	$(Q)echo LD $@ ...
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

$(TARGET_NAME).hex: $(TARGET_NAME).elf
	$(SILENT)echo GEN $@
	$(SILENT)$(MKDIR) -p $(dir $@)
	$(SILENT)$(OBJCOPY) -O ihex $(TARGET_NAME).elf $(TARGET_NAME).hex

$(TARGET_NAME).bin: $(TARGET_NAME).elf
	$(SILENT)echo GEN $@
	$(SILENT)$(MKDIR) -p $(dir $@)
	$(SILENT)$(OBJCOPY) -O binary $(TARGET_NAME).elf $(TARGET_NAME).bin
	if [ -s $@ ]; then \
	head -c $$(($(call filesize,$@) - 38)) $@ > $@.no_crc && \
	tail -c 38 $@ > $@.crc_block && \
	test "0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20280078563412" = `xxd -p -c 500 $@.crc_block` && \
	shasum -a 256 $@.no_crc | cut -c 1-65 | xxd -r -p | dd bs=1 of=$@ seek=$$(($(call filesize,$@) - 38)) conv=notrunc  && \
	head -c $$(($(call filesize,$@) - 4)) $@ > $@.no_crc && \
	crc32 $@.no_crc | cut -c 1-10 | xxd -r -p | dd bs=1 of=$@ seek=$$(($(call filesize,$@) - 4)) conv=notrunc ;\
	fi  

flash: all
ifdef RBLINK
	$(SILENT)echo "Using the RBLink to flash"
	openocd -f libs/openocd/redbearduo.cfg -c "program $(TARGET_NAME).bin verify reset exit 0x80c0000"
else
	$(SUDO) dfu-util -d 2b04:d058 -a 0 -s 0x080C0000:leave -D $(TARGET_NAME).bin
endif


MAKE_PINS = boards/make-pins.py
BOARD_PINS = boards/pins.csv
AF_FILE = boards/stm32f205_af.csv
PREFIX_FILE = boards/stm32f2xx_prefix.c
GEN_PINS_SRC = $(BUILD)/pins.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(BUILD)/pins_qstr.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_PY = $(BUILD)/pins_af.py

# List of sources for qstr extraction
SRC_QSTR += $(LIB_SOURCES) $(MP_SOURCE) $(SRC_MOD)

# Making OBJ use an order-only depenedency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(HEADER_BUILD)/pins.h

# With conditional pins, we may need to regenerate qstrdefs.h when config
# options change.
$(HEADER_BUILD)/qstrdefs.generated.h: boards/mpconfigboard.h

# Call make-pins.py to generate both pins_gen.c and pins.h
$(GEN_PINS_SRC) $(GEN_PINS_HDR) $(GEN_PINS_QSTR) $(GEN_PINS_AF_CONST): $(BOARD_PINS) $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) --af-const $(GEN_PINS_AF_CONST) --af-py $(GEN_PINS_AF_PY) > $(GEN_PINS_SRC)

$(BUILD)/pins.o: $(BUILD)/pins.c
	$(call compile_c)


GEN_STMCONST_HDR = $(HEADER_BUILD)/modstm_const.h
GEN_STMCONST_QSTR = $(BUILD)/modstm_qstr.h
GEN_STMCONST_MPZ = $(HEADER_BUILD)/modstm_mpz.h
CMSIS_MCU_HDR = libs/H_platform/MCU/STM32F2xx/CMSIS/Device/ST/Include/stm32f2xx.h

$(BUILD)/modstm.o: $(GEN_STMCONST_HDR)
# Use a pattern rule here so that make will only call make-stmconst.py once to
# make both modstm_const.h and modstm_qstr.h
$(HEADER_BUILD)/%_const.h $(BUILD)/%_qstr.h: $(CMSIS_MCU_HDR) make-stmconst.py | $(HEADER_BUILD)
	$(ECHO) "Create stmconst $@"
	$(Q)$(PYTHON) make-stmconst.py --qstr $(GEN_STMCONST_QSTR) --mpz $(GEN_STMCONST_MPZ) $(CMSIS_MCU_HDR) > $(GEN_STMCONST_HDR)


include ../py/mkrules.mk

