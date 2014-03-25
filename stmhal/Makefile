include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CMSIS_DIR=cmsis
HAL_DIR=hal
USBDEV_DIR=usbdev
#USBHOST_DIR=usbhost
FATFS_DIR=fatfs
#CC3K_DIR=cc3k
DFU=../tools/dfu.py

CROSS_COMPILE = arm-none-eabi-

INC =  -I.
INC += -I$(PY_SRC)
INC += -I$(CMSIS_DIR)/inc
INC += -I$(CMSIS_DIR)/devinc
INC += -I$(HAL_DIR)/inc
INC += -I$(USBDEV_DIR)/core/inc -I$(USBDEV_DIR)/class/cdc_msc/inc
#INC += -I$(USBHOST_DIR)
INC += -I$(FATFS_DIR)/src
#INC += -I$(CC3K_DIR)

CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 $(CFLAGS_CORTEX_M4) $(COPT)

BOARD ?= PYBv10
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif
CFLAGS += -Iboards/$(BOARD)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g -DPENDSV_DEBUG
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

LDFLAGS = --nostdlib -T stm32f405.ld -Map=$(@:.elf=.map) --cref
LIBS =

# uncomment this if you want libgcc
#LIBS += $(shell $(CC) -print-libgcc-file-name)

SRC_C = \
	main.c \
	string0.c \
	system_stm32f4xx.c \
	stm32f4xx_it.c \
	stm32f4xx_hal_msp.c \
	usbd_conf.c \
	usbd_desc_cdc_msc.c \
	usbd_cdc_interface.c \
	usbd_msc_storage.c \
	pendsv.c \
	systick.c  \
	led.c \
	pin.c \
	pin_map.c \
	pin_named_pins.c \
	usart.c \
	usb.c \
	printf.c \
	math.c \
	malloc0.c \
	gccollect.c \
	pyexec.c \
	help.c \
	input.c \
	modpyb.c \
	modos.c \
	modtime.c \
	import.c \
	lexerfatfs.c \
	gpio.c \
	exti.c \
	usrsw.c \
	rng.c \
	rtc.c \
	flash.c \
	storage.c \
	file.c \
	sdcard.c \
	diskio.c \
	lcd.c \
	accel.c \
	servo.c \
	dac.c \
	adc.c \
	i2c.c \

#	timer.c \
#	pybwlan.c \

SRC_S = \
	startup_stm32f40xx.s \
	gchelper.s \

SRC_HAL = $(addprefix $(HAL_DIR)/src/,\
	stm32f4xx_hal.c \
	stm32f4xx_hal_adc.c \
	stm32f4xx_hal_adc_ex.c \
	stm32f4xx_hal_cortex.c \
	stm32f4xx_hal_dac.c \
	stm32f4xx_hal_dac_ex.c \
	stm32f4xx_hal_dma.c \
	stm32f4xx_hal_flash.c \
	stm32f4xx_hal_flash_ex.c \
	stm32f4xx_hal_gpio.c \
	stm32f4xx_hal_i2c.c \
	stm32f4xx_hal_pcd.c \
	stm32f4xx_hal_rcc.c \
	stm32f4xx_hal_rcc_ex.c \
	stm32f4xx_hal_rng.c \
	stm32f4xx_hal_rtc.c \
	stm32f4xx_hal_rtc_ex.c \
	stm32f4xx_hal_sd.c \
	stm32f4xx_hal_tim.c \
	stm32f4xx_hal_tim_ex.c \
	stm32f4xx_hal_uart.c \
	stm32f4xx_ll_sdmmc.c \
	stm32f4xx_ll_usb.c \
	)

SRC_USBDEV = $(addprefix $(USBDEV_DIR)/,\
	core/src/usbd_core.c \
	core/src/usbd_ctlreq.c \
	core/src/usbd_ioreq.c \
	class/cdc_msc/src/usbd_cdc_msc.c \
	class/cdc_msc/src/usbd_msc_bot.c \
	class/cdc_msc/src/usbd_msc_scsi.c \
	class/cdc_msc/src/usbd_msc_data.c \
	)

#	class/cdc/src/usbd_cdc.c \
	class/msc/src/usbd_msc.c \
#	usbd_core.c \
	usbd_ioreq.c \
	usbd_req.c \
	usbd_usr.c \
	usbd_desc.c \
	usbd_pyb_core.c \
	usbd_pyb_core2.c \
	usbd_cdc_vcp.c \
	usbd_msc_bot.c  \
	usbd_msc_data.c \
	usbd_msc_scsi.c \
	usbd_storage_msd.c \
	)

SRC_FATFS = $(addprefix $(FATFS_DIR)/src/,\
	ff.c \
	option/ccsbcs.c \
	)

SRC_CC3K = $(addprefix $(CC3K_DIR)/,\
	cc3000_common.c \
	evnt_handler.c \
	hci.c \
	netapp.c \
	nvmem.c \
	security.c \
	socket.c \
	wlan.c \
	ccspi.c \
	pybcc3k.c \
	)

OBJ =
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_HAL:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_USBDEV:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_FATFS:.c=.o))
#OBJ += $(addprefix $(BUILD)/, $(SRC_CC3K:.c=.o))
OBJ += $(BUILD)/pins_$(BOARD).o

all: $(BUILD)/flash.dfu

$(BUILD)/flash.dfu: $(BUILD)/flash0.bin $(BUILD)/flash1.bin
	$(ECHO) "Create $@"
	$(Q)python $(DFU) -b 0x08000000:$(BUILD)/flash0.bin -b 0x08020000:$(BUILD)/flash1.bin $@

$(BUILD)/flash0.bin: $(BUILD)/flash.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector $^ $@

$(BUILD)/flash1.bin: $(BUILD)/flash.elf
	$(Q)$(OBJCOPY) -O binary -j .text -j .data $^ $@

$(BUILD)/flash.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

MAKE_PINS = boards/make-pins.py
BOARD_PINS = boards/$(BOARD)/pins.csv
AF_FILE = boards/stm32f4xx-af.csv
PREFIX_FILE = boards/stm32f4xx-prefix.c
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(BUILD)/pins.h

# Making OBJ use an order-only depenedency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(BUILD)/pins.h

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_$(BOARD).c and pins.h
$(BUILD)/%_$(BOARD).c $(BUILD)/%.h: boards/$(BOARD)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE)
	$(ECHO) "Create $@"
	$(Q)python $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) > $(GEN_PINS_SRC)

$(BUILD)/pins_$(BOARD).o: $(BUILD)/pins_$(BOARD).c
	$(call compile_c)

include ../py/mkrules.mk
