BOARD ?= evkmimxrt1064

BUILD ?= build-$(BOARD)

Toolchain ?= gcc

Series ?= imx

Device ?= MIMXRT1064

SDCARD_INF = usdhc

include board/$(BOARD)/mpconfigboard.mk

#FreeRTos variable
FreeRTOS_DIR = lib/nxplib/rtos

CMSIS_DRIVER_INCLUDE = lib/nxplib/CMSIS/Driver/Include
#NXP lib variable
NXP_DRIVER_DIR = lib/nxplib/devices/$(Device)/drivers
NXP_MIDDLEWARE_DIR = lib/nxplib/middleware
NXP_DEVICE_DIR = lib/nxplib/devices/$(Device)
NXP_COMPONENTS_DIR = lib/nxplib/components

NXP_UTILITIES_DIR = $(NXP_DEVICE_DIR)/utilities
NXP_TOOLCHAIN_DIR = $(NXP_DEVICE_DIR)/$(Toolchain)
NXP_XIP_DIR = $(NXP_DEVICE_DIR)/xip
NXP_CMSIS_DRIVER_DIR = $(NXP_DEVICE_DIR)/cmsis_drivers

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

ifeq ($(LITTLEVGL_ENABLE), 1)
QSTR_GLOBAL_DEPENDENCIES += $(LVGL_MPY)
endif

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
INC += -I$(TOP)/$(CMSIS_DRIVER_INCLUDE)
INC += -I$(TOP)/$(NXP_DEVICE_DIR)
INC += -I$(TOP)/$(NXP_MIDDLEWARE_DIR)/sdmmc/inc
INC += -I$(TOP)/$(NXP_MIDDLEWARE_DIR)/sdmmc/port
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/list
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/serial_manager
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/uart
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/ft5406_rt
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)/i2c
INC += -I$(TOP)/$(NXP_COMPONENTS_DIR)
INC += -I$(TOP)/$(NXP_CMSIS_DRIVER_DIR)
INC += -I$(TOP)/$(NXP_XIP_DIR)
INC += -I$(TOP)/$(NXP_TEMPLATE_DIR)
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
		-DFSL_RTOS_FREE_RTOS \
		-DFFCONF_H=\"ports/nxp/ffconf.h\" \
		--specs=nosys.specs \
		 --specs=nano.specs \
		 -D__START=main \


# ignore unused function
CFLAGS += -Wno-unused-function

LDFLAGS = --defsym=__heap_size__=$(HEAP_SIZE) -T $(LD_DIR)/$(LINK_FILE) -Map=$(@:.elf=.map) --cref --gc-sections 

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
	utils/printf.c \
	utils/pyexec.c \
	libc/string0.c \
	mp-readline/readline.c \
	timeutils/timeutils.c \
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
	vfs.c \
	vfs_fat.c \
	vfs_fat_file.c \
	vfs_posix.c \
	vfs_reader.c \
	vfs_fat_diskio.c \
)

ifeq ($(Series), imx)
SRC_FSL_DRIVERS = $(addprefix $(NXP_DRIVER_DIR)/, \
	fsl_common.c \
	fsl_gpio.c \
	fsl_lpuart.c \
	fsl_usdhc.c \
	fsl_cache.c \
	fsl_lpi2c.c \
	fsl_gpt.c \
	fsl_clock.c \
	fsl_lpspi.c \
	fsl_lpspi_edma.c \
	fsl_dmamux.c \
	fsl_edma.c \
	fsl_lpuart_edma.c \
	$(BOARD_FSL_DRIVERS) \
)
SRC_FSL_DRIVERS += $(NXP_XIP_DIR)/fsl_flexspi_nor_boot.c
endif

ifeq ($(Series), kinetis)
SRC_FSL_DRIVERS = $(addprefix $(NXP_DRIVER_DIR)/, \
	fsl_clock.c \
	fsl_common.c \
	fsl_ftfx_cache.c \
	fsl_ftfx_controller.c \
	fsl_ftfx_flash.c \
	fsl_ftfx_flexnvm.c \
	fsl_gpio.c \
	fsl_smc.c \
	fsl_uart.c \
	fsl_sdhc.c \
	fsl_sysmpu.c \
	fsl_dmamux.c \
	fsl_uart_edma.c \
	fsl_dspi.c \
)
endif

ifeq ($(Series), lpc)
SRC_FSL_DRIVERS = $(addprefix $(NXP_DRIVER_DIR)/, \
	fsl_clock.c \
	fsl_common.c \
	fsl_emc.c \
	fsl_flexcomm.c \
	fsl_gpio.c \
	fsl_power.c \
	fsl_reset.c \
	fsl_sdif.c \
	fsl_usart.c \
	fsl_i2c.c \
	fsl_spi.c \
	fsl_pint.c \
	fsl_inputmux.c \
	fsl_power_lib.c \
)

endif

SRC_MIDDLEWARE = $(addprefix $(NXP_MIDDLEWARE_DIR)/, \
	sdmmc/port/$(SDCARD_INF)/freertos/fsl_sdmmc_host.c \
	sdmmc/port/$(SDCARD_INF)/freertos/fsl_sdmmc_event.c \
	sdmmc/src/fsl_mmc.c \
	sdmmc/src/fsl_sd.c \
	sdmmc/src/fsl_sdio.c \
	sdmmc/src/fsl_sdmmc_common.c \
)

ifeq ($(Series), imx)
SRC_COMPONENT = $(addprefix $(NXP_COMPONENTS_DIR)/, \
	lists/generic_list.c \
	serial_manager/serial_manager.c \
	serial_manager/serial_port_uart.c \
	uart/lpuart_adapter.c \
	i2c/lpi2c_adapter.c \
)
ifeq ($(LITTLEVGL_ENABLE), 1)
SRC_COMPONENT += $(addprefix $(NXP_COMPONENTS_DIR)/, \
	ft5406_rt/fsl_ft5406_rt.c \
)
endif
endif

ifeq ($(Series), kinetis)
SRC_COMPONENT = $(addprefix $(NXP_COMPONENTS_DIR)/, \
	lists/generic_list.c \
	serial_manager/serial_manager.c \
	serial_manager/serial_port_uart.c \
	uart/uart_adapter.c \
	gpio/gpio_kinetis.c \
)
endif

ifeq ($(Series), lpc)
SRC_COMPONENT = $(addprefix $(NXP_COMPONENTS_DIR)/, \
	lists/generic_list.c \
	serial_manager/serial_manager.c \
	serial_manager/serial_port_uart.c \
	uart/usart_adapter.c \
	gpio/gpio_lpc.c \
)
endif

ifeq ($(Series), imx)
SRC_CMSIS_DRIVER = $(addprefix $(NXP_CMSIS_DRIVER_DIR)/, \
	fsl_lpspi_cmsis.c \
	fsl_lpuart_cmsis.c \
	fsl_lpi2c_cmsis.c \
)
endif

ifeq ($(Series), kinetis)
SRC_CMSIS_DRIVER = $(addprefix $(NXP_CMSIS_DRIVER_DIR)/, \
	fsl_dspi_cmsis.c \
	fsl_uart_cmsis.c \
	fsl_i2c_cmsis.c \
)
endif

ifeq ($(Series), lpc)
SRC_CMSIS_DRIVER = $(addprefix $(NXP_CMSIS_DRIVER_DIR)/, \
	fsl_spi_cmsis.c \
	fsl_usart_cmsis.c \
	fsl_i2c_cmsis.c \
)
endif

SRC_UTILITIES = $(addprefix $(NXP_UTILITIES_DIR)/, \
	fsl_assert.c \
	debug_console/fsl_debug_console.c \
	str/fsl_str.c \
)

SRC_C = \
	main.c \
	gccollect.c \
	bufferhelper.c \
	pybled.c \
	sdcard.c \
	mphalport.c \
	pybi2c.c \
	pybpinmux.c \
	mpgpio.c \
	pybgpio.c \
	spi.c \
	pybspi.c \
	usart.c \
	pybusart.c \
	i2c.c \
	moduos.c \
	modpyb.c


SRC_O = \
	$(STARTUP_FILE) \
	$(NXP_DEVICE_DIR)/system_$(Device).o \
	lib/utils/gchelper_m3.o \

OBJ += $(addprefix $(BUILD)/, $(SRC_CMSIS_DRIVER:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(SRC_FSL_DRIVERS:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_FreeRTOS:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMPONENT:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MIDDLEWARE:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_UTILITIES:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_BOARD:.c=.o))	
OBJ += $(addprefix $(BUILD)/, $(SRC_SPECIFIC:.c=.o))	
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LVGL_SRC:.c=.o))
OBJ += $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os



# List of source for qstr extraction
SRC_QSTR += $(LVGL_SRC)
SRC_QSTR += $(SRC_C) $(SRC_LIB) $(EXTMOD_SRC_C) $(SRC_BOARD)

all: $(BUILD)/firmware.bin

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $(BUILD)/firmware.bin

include $(TOP)/py/mkrules.mk
