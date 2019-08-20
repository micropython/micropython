BOARD ?= evkmimxrt1064

BUILD ?= build-$(BOARD)

Toolchain ?= gcc

Series ?= imx

Device ?= MIMXRT1064

include board/$(BOARD)/mpconfigboard.mk

#FreeRTos variable
FreeRTOS_DIR = lib/nxplib/rtos

#NXP lib variable
NXP_DRIVER_DIR = lib/nxplib/drivers/$(Series)
NXP_MIDDLEWARE_DIR = lib/nxplib/middleware
NXP_DEVICE_DIR = lib/nxplib/devices/$(Device)
NXP_COMPONENTS_DIR = lib/nxplib/components

NXP_UTILITIES_DIR = $(NXP_DEVICE_DIR)/utilities
NXP_TOOLCHAIN_DIR = $(NXP_DEVICE_DIR)/$(Toolchain)
NXP_XIP_DIR = $(NXP_DEVICE_DIR)/xip

LVGL_SRC =
ifeq ($(LITTLEVGL_ENABLE), 1)
#littlevGL
LVGL_BINDING_DIR = $(TOP)/lib/lv_binding_micropython
LVGL_DIR += $(TOP)/$(NXP_MIDDLEWARE_DIR)/littlevgl
LVGL_INC += -I$(LVGL_DIR)
ALL_LVGL_SRC = $(shell find $(LVGL_DIR) -type f) $(TOP)/$(BOARD_DIR)/lv_conf.h
LVGL_MPY = ./modlvgl.c
LVGL_SRC_INC = $(shell find $(LVGL_DIR) -type f -name "*.h")
LVGL_SRC = $(subst $(TOP)/,,$(shell find $(LVGL_DIR) -type f -name "*.c") $(LVGL_MPY)) 	modSDL.c
endif

# qstr definitions (must come before including py.mk)
include ../../py/mkenv.mk
QSTR_DEFS = qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES += $(LVGL_MPY)

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE = arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(TOP)/$(NXP_DRIVER_DIR)
INC += -I$(TOP)/lib/libm_dbl
INC += -I$(TOP)/lib/cmsis/inc
INC += -I$(TOP)/py
INC += -I$(TOP)/lib
INC += -I$(TOP)/$(FreeRTOS_DIR)/amazon-freertos/lib/include 
INC += -I$(TOP)/$(FreeRTOS_DIR)/amazon-freertos/lib/include/private
INC += -I$(TOP)/$(FreeRTOS_DIR)/amazon-freertos/lib/FreeRTOS/portable/GCC/ARM_CM4F
INC += -I$(TOP)/$(NXP_DEVICE_DIR)
INC += -I$(TOP)/$(NXP_MIDDLEWARE_DIR)/sdmmc/inc
INC += -I$(TOP)/$(NXP_MIDDLEWARE_DIR)/sdmmc/port
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/list
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/serial_manager
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/uart
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/ft5406_rt
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/i2c
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)
INC += -I$(TOP)/$(NXP_XIP_DIR)
INC += -I$(TOP)/$(NXP_UTILITIES_DIR)
INC += -I$(TOP)/$(NXP_UTILITIES_DIR)/debug_console
INC += -I$(TOP)/$(NXP_UTILITIES_DIR)/str
INC += -I$(TOP)/extmod
INC += -I$(TOP)/lib/oofatfs
INC += -I$(TOP)/$(BOARD_DIR)

ifeq ($(LITTLEVGL_ENABLE), 1)
INC += -I$(LVGL_DIR)
endif

LD_DIR = $(TOP)/$(NXP_TOOLCHAIN_DIR)

FATFS_DIR=lib/oofatfs

STARTUP_FILE = $(NXP_TOOLCHAIN_DIR)/startup_$(Device).o

CFLAGS = $(INC) $(CFLAGS_CORE) $(CFLAGS_BOARD) -Wall -Werror -std=gnu99 \
		-fno-tree-scev-cprop -Wpointer-arith -fdata-sections -ffunction-sections \
		-DLV_CONF_INCLUDE_SIMPLE=1 \
		-DFSL_RTOS_FREE_RTOS \
		-D__STARTUP_CLEAR_BSS \
		-D__STARTUP_INITIALIZE_NONCACHEDATA \
		-DFFCONF_H=\"ports/nxp/ffconf.h\" \
		--specs=nosys.specs \
		 --specs=nano.specs \
		 -D__START=main \


# ignore unused function
CFLAGS += -Wno-unused-function

LDFLAGS = --defsym=__heap_size__=0x8000 -T $(LD_DIR)/$(LINK_FILE) -Map=$(@:.elf=.map) --cref --gc-sections 

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

# Options for mpy-cross
MPY_CROSS_FLAGS += -march=armv7m

SRC_LIB = $(addprefix lib/,\
	oofatfs/ff.c \
	oofatfs/ffunicode.c \
	)

SRC_FreeRTOS = $(addprefix $(FreeRTOS_DIR)/, \
	amazon-freertos/lib/FreeRTOS/event_groups.c \
	amazon-freertos/lib/FreeRTOS/list.c \
	amazon-freertos/lib/FreeRTOS/queue.c \
	amazon-freertos/lib/FreeRTOS/stream_buffer.c \
	amazon-freertos/lib/FreeRTOS/tasks.c \
	amazon-freertos/lib/FreeRTOS/timers.c \
	amazon-freertos/lib/FreeRTOS/portable/MemMang/heap_4.c \
	amazon-freertos/lib/FreeRTOS/portable/GCC/ARM_CM4F/port.c \
)

SRC_LIBM = $(addprefix lib/libm/,\
	math.c \
	acoshf.c \
	asinfacosf.c \
	asinhf.c \
	atan2f.c \
	atanf.c \
	atanhf.c \
	ef_rem_pio2.c \
	erf_lgamma.c \
	ef_sqrt.c \
	fmodf.c \
	kf_cos.c \
	kf_rem_pio2.c \
	kf_sin.c \
	kf_tan.c \
	log1pf.c \
	nearbyintf.c \
	sf_cos.c \
	sf_erf.c \
	sf_frexp.c \
	sf_ldexp.c \
	sf_modf.c \
	sf_sin.c \
	sf_tan.c \
	wf_lgamma.c \
	wf_tgamma.c \
)

EXTMOD_SRC_C = $(addprefix extmod/, \
	modubinascii.c \
	modujson.c \
	modure.c \
)

SRC_FSL_DRIVERS = $(addprefix $(NXP_DRIVER_DIR)/, \
	fsl_common.c \
	fsl_gpio.c \
	fsl_lpuart.c \
	fsl_usdhc.c \
	fsl_cache.c \
	fsl_lpi2c.c \
	fsl_gpt.c \
	$(BOARD_FSL_DRIVERS) \
)

SRC_NXP_MIDDLEWARE = $(addprefix $(NXP_MIDDLEWARE_DIR)/, \
	sdmmc/port/usdhc/freertos/fsl_sdmmc_host.c \
	sdmmc/port/usdhc/freertos/fsl_sdmmc_event.c \
	sdmmc/src/fsl_mmc.c \
	sdmmc/src/fsl_sd.c \
	sdmmc/src/fsl_sdio.c \
	sdmmc/src/fsl_sdmmc_common.c \
)

SRC_C = \
	$(SRC_FSL_DRIVERS) \
	$(SRC_BOARD) \
	$(NXP_COMPONENTS_DIR)/lists/generic_list.c \
	$(NXP_COMPONENTS_DIR)/serial_manager/serial_manager.c \
	$(NXP_COMPONENTS_DIR)/serial_manager/serial_port_uart.c \
	$(NXP_COMPONENTS_DIR)/uart/lpuart_adapter.c \
	$(NXP_COMPONENTS_DIR)/i2c/lpi2c_adapter.c \
	$(NXP_COMPONENTS_DIR)/ft5406_rt/fsl_ft5406_rt.c \
	$(NXP_XIP_DIR)/fsl_flexspi_nor_boot.c \
	$(NXP_UTILITIES_DIR)/fsl_assert.c \
	$(NXP_UTILITIES_DIR)/debug_console/fsl_debug_console.c \
	$(NXP_UTILITIES_DIR)/str/fsl_str.c \
	$(SRC_FreeRTOS) \
	$(SRC_NXP_MIDDLEWARE) \
	$(EXTMOD_SRC_C) \
	$(LVGL_SRC) \
	$(SRC_SPECIFIC) \
	main.c \
	gccollect.c \
	uart_core.c \
	pin.c \
	led.c \
	sdcard.c \
	mphalport.c \
	pybi2c.c \
	pybpinmux.c \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	lib/timeutils/timeutils.c \
	moduos.c \
	modpyb.c \
	extmod/vfs.c \
	extmod/vfs_fat.c \
	extmod/vfs_fat_file.c \
	extmod/vfs_posix.c \
	extmod/vfs_reader.c \
	extmod/vfs_fat_diskio.c

SRC_O = \
	$(STARTUP_FILE) \
	$(NXP_DEVICE_DIR)/system_$(Device).o \
	lib/utils/gchelper_m3.o \


OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LVGL_C:.c=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os



# List of source for qstr extraction
SRC_QSTR += $(LVGL_SRC)
SRC_QSTR += $(SRC_C) 

all: $(BUILD)/firmware.bin

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $(BUILD)/firmware.bin

include $(TOP)/py/mkrules.mk
