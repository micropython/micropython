#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := micropython

include $(IDF_PATH)/make/project.mk

include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = main/qstrdefsport.h

MICROPY_PY_USSL = 0
MICROPY_SSL_AXTLS = 0
MICROPY_FATFS = 1
MICROPY_PY_BTREE = 1

#FROZEN_DIR = scripts
FROZEN_MPY_DIR = modules

# include py core make definitions
include $(TOP)/py/py.mk

PORT ?= /dev/ttyUSB0
BAUD ?= 460800
FLASH_MODE ?= dio
FLASH_FREQ ?= 40m
FLASH_SIZE ?= 4MB
CROSS_COMPILE ?= xtensa-esp32-elf-

ESPIDF_SUPHASH := 1c7a8b3b712c4020562551f692d0ced8f7470d2c

# paths to ESP IDF and its components
ifeq ($(ESPIDF),)
    ifneq ($(IDF_PATH),)
        ESPIDF = $(IDF_PATH)
    else
        $(info The ESPIDF variable has not been set, please set it to the root of the esp-idf repository.)
        $(info See README.md for installation instructions.)
        $(info Supported git hash: $(ESPIDF_SUPHASH))
        $(error ESPIDF not set)
    endif
endif
# ESPCOMP = $(ESPIDF)/components
# ESPTOOL ?= $(ESPCOMP)/esptool_py/esptool/esptool.py

# verify the ESP IDF version
ESPIDF_CURHASH := $(shell git -C $(ESPIDF) show -s --pretty=format:'%H')
ifneq ($(ESPIDF_CURHASH),$(ESPIDF_SUPHASH))
    $(info ** WARNING **)
    $(info The git hash of ESP IDF does not match the supported version)
    $(info The build may complete and the firmware may work but it is not guaranteed)
    $(info ESP IDF path:       $(ESPIDF))
    $(info Current git hash:   $(ESPIDF_CURHASH))
    $(info Supported git hash: $(ESPIDF_SUPHASH))
endif

# pretty format of ESP IDF version, used internally by the IDF
# IDF_VER := $(shell git -C $(ESPIDF) describe)

$(info 'Top:')
$(info $(TOP))
$(info '................')
$(info $(PWD))
$(info '................')

# This is horrible, but I can't figure out how to make
# the sdkconfig.h from the project be included first.
INC += -I${CURDIR}/build/include/.

INC += -I${CURDIR}/main/.
INC += -I${CURDIR}/../..
INC += -I${CURDIR}/../../lib/mp-readline
INC += -I${CURDIR}/../../lib/netutils
INC += -I${CURDIR}/../../lib/timeutils
INC += -I$(BUILD)

CFLAGS = $(INC)


# Debugging/Optimization
ifeq ($(DEBUG), 1)
    CFLAGS += -g
    COPT = -O0
else
    #CFLAGS += -fdata-sections -ffunction-sections
    COPT += -Os -DNDEBUG
    #LDFLAGS += --gc-sections
endif



################################################################################
# List of MicroPython source and object files

SRC_C = \
	main.c \
	uart.c \
	gccollect.c \
	mphalport.c \
	fatfs_port.c \
	help.c \
	modutime.c \
	moduos.c \
	machine_timer.c \
	machine_pin.c \
	machine_touchpad.c \
	machine_adc.c \
	machine_dac.c \
	machine_pwm.c \
	machine_uart.c \
	modmachine.c \
	modnetwork.c \
	network_lan.c \
	modsocket.c \
	modesp.c \
	esp32_ulp.c \
	modesp32.c \
	espneopixel.c \
	machine_hw_spi.c \
	machine_wdt.c \
	mpthreadport.c \
	machine_rtc.c \
	$(SRC_MOD)

EXTMOD_SRC_C = $(addprefix extmod/,\
	modonewire.c \
	)

LIB_SRC_C = $(addprefix lib/,\
	libm/math.c \
	libm/fmodf.c \
	libm/roundf.c \
	libm/ef_sqrt.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/asinfacosf.c \
	libm/atanf.c \
	libm/atan2f.c \
	mp-readline/readline.c \
	netutils/netutils.c \
	timeutils/timeutils.c \
	utils/pyexec.c \
	utils/interrupt_char.c \
	utils/sys_stdio_mphal.c \
	)

ifeq ($(MICROPY_FATFS), 1)
    LIB_SRC_C += \
    	lib/oofatfs/ff.c \
    	lib/oofatfs/option/unicode.c
endif

DRIVERS_SRC_C = $(addprefix drivers/,\
	bus/softspi.c \
	dht/dht.c \
	)


$(info '................')


MAKE_HEADERS: $(HEADER_BUILD)/qstrdefs.generated.h

COMPONENT_OBJS += $(PY_O)
COMPONENT_OBJS += $(MAKE_HEADERS)
COMPONENT_OBJS += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
COMPONENT_OBJS += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
COMPONENT_OBJS += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
COMPONENT_OBJS += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(EXTMOD_SRC_C) $(LIB_SRC_C) $(DRIVERS_SRC_C)
# Append any auto-generated sources that are needed by sources listed in SRC_QSTR
SRC_QSTR_AUTO_DEPS +=


include $(TOP)/py/mkrules.mk
