# paths to WM_SDK
ifneq ($(WMSDK_PATH),)
TOPDIR = $(WMSDK_PATH)
else
$(info The WMSDK_PATH variable has not been set, please set it to the root of the WM_SDK.)
$(info See README.md for installation instructions.)
$(error WMSDK_PATH not set)
endif

include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

MICROPY_PY_USSL = 0
MICROPY_SSL_AXTLS = 0
MICROPY_SSL_MBEDTLS = 0
#AXTLS_DEFS_EXTRA = -Dabort=abort_ -DRT_MAX_PLAIN_LENGTH=1024 -DRT_EXTRA=4096 -Wno-implicit-function-declaration
MICROPY_FATFS = 1

FROZEN_MPY_DIR = modules

# include py core make definitions
include $(TOP)/py/py.mk

INC += -I.
INC += -I./include
INC += -I$(TOP)
INC += -I$(TOP)/lib/mp-readline
INC += -I$(TOP)/lib/netutils
INC += -I$(TOP)/lib/timeutils
INC += -I$(TOP)/extmod/axtls-include
INC += -I$(BUILD)

#TOPDIR = $(WMSDK_PATH)
#include $(TOPDIR)/Tools/rules.mk
#include $(TOPDIR)/Tools/toplevel.mk
export TOPDIR
include $(TOPDIR)/Tools/subdir.mk
COMPILER_OS=$(shell uname -o)
$(warning COMPILER_OS: $(COMPILER_OS))
export COMPILER_OS
ifeq ($(COMPILER_OS),Cygwin)
COMPILER_OS_CYGWIN=1
else
COMPILER_OS_CYGWIN=0
endif
export COMPILER_OS_CYGWIN
include $(TOPDIR)/Tools/verbose.mk

ifeq ($(COMPILER_OS_CYGWIN),1)
$(shell mkdir $(BUILD))
$(shell ./tools/cygwin.sh $(PWD) $(WMSDK_PATH))
include $(BUILD)/cygwin.mk
endif

include $(TOPDIR)/Tools/toolchain.def

OUTPUT_PATH = $(BUILD)

#---------------------------------------------------------------------------
# Target definition (User)
#---------------------------------------------------------------------------
GOAL = W600

#---------------------------------------------------------------------------
# Source section (User)
#---------------------------------------------------------------------------
ASM_SRC := $(TOPDIR)/Platform/Boot/gcc/startup_ARMCM3.s
C_SRC := $(TOPDIR)/Platform/Boot/gcc/retarget_gcc.c \
	 $(TOPDIR)/Platform/Boot/gcc/misc.c \

#---------------------------------------------------------------------------
# Linker options
#---------------------------------------------------------------------------
LDFLAGS	   += -static
LDFLAGS    += -nostartfiles
LDFLAGS    += -T./tools/link_w600.ld
LDFLAGS    += -mthumb
LDFLAGS    += -mcpu=cortex-m3
#---------------------------------------------------------------------------
# Link Library Section
#---------------------------------------------------------------------------
#LIB_SRC := $(LIB_DIR)/wmboot.a
LIB_SRC += $(LIB_DIR)/oneshot.a \
	   $(LIB_DIR)/wmcJSON.a \
	   $(LIB_DIR)/wmcmd.a \
	   $(LIB_DIR)/wmcommon.a \
	   $(LIB_DIR)/wmdemo.a \
	   $(LIB_DIR)/wmdhcpserver.a \
	   $(LIB_DIR)/wmdnsserver.a \
	   $(LIB_DIR)/wmdriver.a \
	   $(LIB_DIR)/wmhttpclient.a \
	   $(LIB_DIR)/wmiperf.a \
	   $(LIB_DIR)/wmlwip.a \
	   $(LIB_DIR)/wmmain.a \
	   $(LIB_DIR)/wmota.a \
	   $(LIB_DIR)/wmntp.a \
	   $(LIB_DIR)/wmping.a \
	   $(LIB_DIR)/wmrtos.a \
	   $(LIB_DIR)/wmssl.a \
	   $(LIB_DIR)/wmweb.a \
	   $(LIB_DIR)/wmwebsocket.a \
	   $(LIB_DIR)/wmsslserver.a \
	   $(LIB_DIR)/libairkiss_log.a

ifeq ($(COST_DOWN),1)
LIB_SRC += $(LIB_DIR)/wlan_costdown.a
else
LIB_SRC += $(LIB_DIR)/wlan.a
endif


#---------------------------------------------------------------------------
# mpy Source section
#---------------------------------------------------------------------------
MPY_SRC = main.c \
        hal/help.c \
        hal/mpthreadport.c \
        hal/mphalport.c \
        hal/gccollect.c \
        hal/uart.c \
        hal/fatfs_port.c \
        hal/math_port.c \
        hal/modflash.c \
        machine/machine_timer.c \
        machine/machine_wdt.c \
        machine/machine_pin.c \
        machine/machine_rtc.c \
        machine/machine_uart.c \
        machine/machine_pwm.c \
        machine/machine_i2c.c \
        mods/modnetwork.c \
        mods/modmachine.c \
        mods/modutime.c \
        mods/moduos.c \
        mods/modusocket.c \
        mods/modw600.c \
        mods/moduhashlib.c \
        tools/ftpserver.c \
        $(SRC_MOD)

EXTMOD_SRC_C = $(addprefix extmod/,\
	modonewire.c \
	)

LIB_SRC_C = $(addprefix lib/,\
	libm/math.c \
    libm/nearbyintf.c \
    libm/wf_lgamma.c \
    libm/erf_lgamma.c \
    libm/sf_erf.c \
    libm/atanhf.c \
    libm/log1pf.c \
    libm/wf_tgamma.c \
    libm/asinhf.c \
    libm/acoshf.c \
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
	lib/oofatfs/ffunicode.c
endif

DRIVERS_SRC_C = $(addprefix drivers/,\
	bus/softspi.c \
	dht/dht.c \
	)

OBJ_MP =
OBJ_MP += $(PY_O)
#OBJ_MP += $(PY_CORE_O)
OBJ_MP += $(addprefix $(BUILD)/, $(MPY_SRC:.c=.o))
OBJ_MP += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
OBJ_MP += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ_MP += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))

OBJ_WM =
OBJ_WM += $(C_SRC:.c=.o)
OBJ_WM += $(ASM_SRC:.s=.o)

OBJ_FILES	= $(OBJ_MP) $(OBJ_WM)
OBJ = $(OBJ_FILES)

# List of sources for qstr extraction
SRC_QSTR += $(MPY_SRC) $(EXTMOD_SRC_C) $(LIB_SRC_C) $(DRIVERS_SRC_C)
# Append any auto-generated sources that are needed by sources listed in SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

CFLAGS += $(INCLUDES) $(INC) $(CFLAGS_MOD)

_ENV_OS=$(subst /,-,$(COMPILER_OS))
ENV_OS=$(shell echo $(_ENV_OS) | tr '[A-Z]' '[a-z]')

MODULES := library

all: $(MODULES) $(GOAL)
	@./tools/createimg.sh
#	@./createimg.sh -e $(ENV_OS) -r ${_SVN_Revision}

$(MODULES):
	@for i in ${Src_App_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i || exit $?; done
	@for i in ${Src_Net_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@for i in ${Src_Wlan_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@for i in ${Src_RTOS_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@for i in ${WM_Demo_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@for i in ${Platform_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w || exit $?; done
	@echo "finish MODULES"

$(GOAL): $(GOAL).elf 

$(GOAL).elf: $(OBJ_FILES) $(LIB_SRC)
	$(CC) -o $(OUTPUT_PATH)/$(GOAL).elf $(OBJ_FILES) $(LDFLAGS) \
		-Wl,--gc-sections \
		-Wl,--start-group $(LIB_SRC) -Wl,--end-group \
		-Wl,-Map,$(OUTPUT_PATH)/$(GOAL).map -Wl,--warn-common
	$(ELF) \
		--output-target=binary \
		-S -g -x -X -R .sbss -R .bss -R .reginfo -R .stack \
		$(OUTPUT_PATH)/$(GOAL).elf $(OUTPUT_PATH)/$(GOAL).bin	

include $(TOP)/py/mkrules.mk

clean:
	@for i in ${Src_App_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i clean || exit $?; done
	@for i in ${Src_Net_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	@for i in ${Src_Wlan_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	@for i in ${Src_RTOS_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	@for i in ${WM_Demo_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	@for i in ${Platform_dirs} ; do [ ! -d $$i ] || $(MAKE) -C $$i -w clean || exit $?; done
	$(RM) -f $(OBJ_FILES:.o=.d) \
		$(OBJ_FILES) $(GOAL) \
		$(OUTPUT_PATH)/$(GOAL).map \
		$(GOAL).elf \
		$(GOAL).text \
		$(OUTPUT_PATH)/$(GOAL).bin \
		$(GOAL).nm \
		$(OUTPUT_PATH)/*.bin \
		$(OUTPUT_PATH)/*.hex