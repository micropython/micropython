BOARD ?= evkmimxrt1064

BUILD ?= build-$(BOARD)

Toolchain ?= gcc

#littlevGL
LVGL_BINDING_DIR = $(TOP)/lib/lv_binding_micropython
LVGL_DIR = $(LVGL_BINDING_DIR)/lvgl
LVGL_INC += -I$(LVGL_DIR)
ALL_LVGL_SRC = $(shell find $(LVGL_DIR) -type f) $(TOP)/lib/lv_conf.h
LVGL_MPY = ./modlvgl.c
LVGL_SRC_INC = $(shell find $(LVGL_DIR) -type f -name "*.h")
LVGL_SRC = $(subst $(TOP)/,,$(shell find $(LVGL_DIR) -type f -name "*.c") $(LVGL_MPY))

#$(LVGL_MPY): $(ALL_LVGL_SRC)
#	$(ECHO) "LVGL-GEN $@"
#	$(Q)mkdir -p $(dir $@)
#	$(Q)$(PYTHON) $(LVGL_BINDING_DIR)/gen/gen_mpy.py -X anim -X group -X task $(LVGL_INC) -I $(LVGL_BINDING_DIR)/pycparser/utils/fake_libc_include $(LVGL_DIR)/lvgl.h > $@

# qstr definitions (must come before including py.mk)
include ../../py/mkenv.mk
QSTR_DEFS = qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES += $(LVGL_MPY)

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE = arm-none-eabi-

#board variable
BOARD_XIP_DIR = ports/nxp/board/$(BOARD)/xip
BOARD_TEMPLATE_DIR = ports/nxp/board/$(BOARD)/template
BOARD_PROJECT_TEMPLATE_DIR = ports/nxp/board/$(BOARD)/project_template
BOARD_TOOLCHAIN_DIR = ports/nxp/board/$(BOARD)/$(Toolchain)
BOARD_DRIVERS_DIR = ports/nxp/board/$(BOARD)/drivers
BOARD_INC_DIR = ports/nxp/board/$(BOARD)

#FreeRTos variable
FreeRTOS_DIR = ports/nxp/FreeRTOS

#NXP lib variable
NXP_DRIVER_DIR = lib/nxplib/platform/drivers
NXP_MIDDLEWARE_DIR = lib/nxplib/middleware
NXP_UTILITIES_DIR = lib/nxplib/platform/utilities
NXP_COMPONENTS_DIR = lib/nxplib/components

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(TOP)/$(NXP_DRIVER_DIR)
INC += -I$(TOP)/lib/libm_dbl
INC += -I$(TOP)/lib/cmsis/inc
INC += -I$(TOP)/py
INC += -I$(TOP)/lib
INC += -I$(TOP)/lib/lv_binding_micropython/include 
INC += -I$(TOP)/lib/lv_binding_micropython
INC += -I$(TOP)/lib/lv_binding_micropython/lvgl
INC += -I$(TOP)/$(FreeRTOS_DIR)/amazon-freertos/lib/include 
INC += -I$(TOP)/$(FreeRTOS_DIR)/amazon-freertos/lib/include/private
INC += -I$(TOP)/$(FreeRTOS_DIR)/amazon-freertos/lib/FreeRTOS/portable/GCC/ARM_CM4F
INC += -I$(TOP)/$(NXP_MIDDLEWARE_DIR)/sdmmc/inc
INC += -I$(TOP)/$(NXP_MIDDLEWARE_DIR)/sdmmc/port
INC += -I$(TOP)/extmod
INC += -I$(TOP)/lib/oofatfs
INC += -I$(TOP)/$(BOARD_DRIVERS_DIR)
INC += -I$(TOP)/$(BOARD_PROJECT_TEMPLATE_DIR)
INC += -I$(TOP)/$(BOARD_INC_DIR)
INC += -I$(TOP)/$(BOARD_XIP_DIR)
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/list
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/serial_manager
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/uart
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/ft5406_rt
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/i2c
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)
INC += -I$(TOP)/$(NXP_UTILITIES_DIR)
INC += -I$(TOP)/$(NXP_UTILITIES_DIR)/debug_console
INC += -I$(TOP)/$(NXP_UTILITIES_DIR)/str


LD_DIR = $(TOP)/$(BOARD_TOOLCHAIN_DIR)

FATFS_DIR=lib/oofatfs

STARTUP_FILE = $(BOARD_TOOLCHAIN_DIR)/startup_MIMXRT1064.o

CFLAGS_CORTEX_M7 = -mthumb -mtune=cortex-m7 \
				   -mcpu=cortex-m7 \
				   -mfpu=fpv5-d16 -mfloat-abi=hard \
				   -fsingle-precision-constant -Wdouble-promotion

CFLAGS = $(INC) -Wall -Werror -std=gnu99 \
		 $(CFLAGS_CORTEX_M7) -DCPU_MIMXRT1064DVL6A \
		 -D__MPU_PRESENT=1 -fno-tree-scev-cprop -Wpointer-arith \
		 -fdata-sections -ffunction-sections \
		 -DXIP_EXTERNAL_FLASH=1 \
		 -DXIP_BOOT_HEADER_ENABLE=1 \
		 -DXIP_BOOT_HEADER_DCD_ENABLE=1 \
		 -DSKIP_SYSCLK_INIT \
		 -DLV_CONF_INCLUDE_SIMPLE=1 \
		 -DFSL_RTOS_FREE_RTOS \
		 -D__STARTUP_CLEAR_BSS \
		 -D__STARTUP_INITIALIZE_NONCACHEDATA \
		 -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1 \
		 -DFFCONF_H=\"ports/nxp/ffconf.h\" \
		 --specs=nosys.specs \
		 --specs=nano.specs \
		 -D__START=main \
		 $(CFLAGS_EXTRA)

# ignore unused function
CFLAGS += -Wno-unused-function

LDFLAGS = --defsym=__heap_size__=0x8000 -T $(LD_DIR)/MIMXRT1064xxxxx_flexspi_nor_sdram.ld -Map=$(@:.elf=.map) --cref --gc-sections 

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
	amazon-freertos/lib/FreeRTOS/portable/MemMang/heap_3.c \
	amazon-freertos/lib/FreeRTOS/portable/GCC/ARM_CM4F/port.c \
	amazon-freertos/lib/FreeRTOS/portable/GCC/ARM_CM4F/fsl_tickless_systick.c \
)

SRC_sdmmc = $(addprefix $(NXP_MIDDLEWARE_DIR)/sdmmc/src/, \
	fsl_mmc.c \
	fsl_sd.c \
	fsl_sdio.c \
	fsl_sdmmc_common.c \
	fsl_sdspi.c \
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
	fsl_elcdif.c \
	fsl_usdhc.c \
	fsl_cache.c \
	fsl_lpi2c.c \
	fsl_gpt.c \
)

SRC_NXP_MIDDLEWARE = $(addprefix $(NXP_MIDDLEWARE_DIR)/, \
	sdmmc/port/usdhc/freertos/fsl_sdmmc_host.c \
	sdmmc/port/usdhc/freertos/fsl_sdmmc_event.c \
	sdmmc/src/fsl_mmc.c \
	sdmmc/src/fsl_sd.c \
	sdmmc/src/fsl_sdio.c \
	sdmmc/src/fsl_sdmmc_common.c \
	sdmmc/src/fsl_sdspi.c \
)

SRC_C = \
	$(SRC_FSL_DRIVERS) \
	$(BOARD_INC_DIR)/port-evkmimxrt1064.c \
	$(BOARD_PROJECT_TEMPLATE_DIR)/board.c \
	$(BOARD_PROJECT_TEMPLATE_DIR)/clock_config.c \
	$(BOARD_INC_DIR)/hardware_init.c \
	$(BOARD_PROJECT_TEMPLATE_DIR)/pin_mux.c \
	$(BOARD_XIP_DIR)/evkmimxrt1064_flexspi_nor_config.c \
	$(BOARD_XIP_DIR)/evkmimxrt1064_sdram_ini_dcd.c \
	$(NXP_COMPONENTS_DIR)/lists/generic_list.c \
	$(NXP_COMPONENTS_DIR)/serial_manager/serial_manager.c \
	$(NXP_COMPONENTS_DIR)/serial_manager/serial_port_uart.c \
	$(NXP_COMPONENTS_DIR)/uart/lpuart_adapter.c \
	$(NXP_COMPONENTS_DIR)/i2c/lpi2c_adapter.c \
	$(NXP_COMPONENTS_DIR)/timer/gpt_adapter.c \
	$(BOARD_DRIVERS_DIR)/fsl_clock.c \
	$(BOARD_XIP_DIR)/fsl_flexspi_nor_boot.c \
	$(NXP_UTILITIES_DIR)/fsl_assert.c \
	$(NXP_UTILITIES_DIR)/debug_console/fsl_debug_console.c \
	$(NXP_UTILITIES_DIR)/str/fsl_str.c \
	$(NXP_COMPONENTS_DIR)/ft5406_rt/fsl_ft5406_rt.c \
	$(SRC_FreeRTOS) \
	$(SRC_NXP_MIDDLEWARE) \
	$(EXTMOD_SRC_C) \
	$(LVGL_SRC) \
	mainFreeRtos.c \
	gccollect.c \
	uart_core.c \
	pin.c \
	led.c \
	sdcard.c \
	mphalport.c \
	pybi2c.c \
	pybpinmux.c \
	pybgpt.c  \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	littlevgl_support.c \
	lib/timeutils/timeutils.c \
	modSDL.c \
	moduos.c \
	modpyb.c \
	extmod/vfs.c \
	extmod/vfs_fat.c \
	extmod/vfs_fat_file.c \
	extmod/vfs_posix.c \
	extmod/vfs_reader.c \
	extmod/vfs_fat_diskio.c
#	$(BUILD)/_frozen_mpy.c 
	
	


SRC_O = \
	$(STARTUP_FILE) \
	$(BOARD_INC_DIR)/system_MIMXRT1064.o \
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

#$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
#	$(ECHO) "MISC freezing bytecode"
#	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $(BUILD)/firmware.bin

include $(TOP)/py/mkrules.mk
