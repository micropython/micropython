# Select the board to build for: if not given on the command line,
# then default to PYBV10.
BOARD ?= PYBV10
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

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
CC3K_DIR=cc3k
DFU=../tools/dfu.py
# may need to prefix dfu-util with sudo
DFU_UTIL=dfu-util
DEVICE=0483:df11

CROSS_COMPILE = arm-none-eabi-

INC =  -I.
INC += -I$(PY_SRC)
INC += -I$(BUILD)
INC += -I$(CMSIS_DIR)/inc
INC += -I$(CMSIS_DIR)/devinc
INC += -I$(HAL_DIR)/inc
INC += -I$(USBDEV_DIR)/core/inc -I$(USBDEV_DIR)/class/cdc_msc_hid/inc
#INC += -I$(USBHOST_DIR)
INC += -I$(FATFS_DIR)/src
INC += -I$(CC3K_DIR)/inc

CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)

CFLAGS += -Iboards/$(BOARD)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g -DPENDSV_DEBUG
COPT = -O0
else
COPT += -Os -DNDEBUG
endif

LDFLAGS = -nostdlib -T stm32f405.ld -Map=$(@:.elf=.map) --cref
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
	timer.c \
	led.c \
	pin.c \
	pin_named_pins.c \
	bufhelper.c \
	i2c.c \
	spi.c \
	uart.c \
	usb.c \
	printf.c \
	math.c \
	mathsincos.c \
	gccollect.c \
	pybstdio.c \
	readline.c \
	pyexec.c \
	help.c \
	input.c \
	modos.c \
	modpyb.c \
	modstm.c \
	modtime.c \
	import.c \
	lexerfatfs.c \
	extint.c \
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
	modwlan.c \
	modsocket.c \
	modselect.c \

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
	stm32f4xx_hal_spi.c \
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
	class/cdc_msc_hid/src/usbd_cdc_msc_hid.c \
	class/cdc_msc_hid/src/usbd_msc_bot.c \
	class/cdc_msc_hid/src/usbd_msc_scsi.c \
	class/cdc_msc_hid/src/usbd_msc_data.c \
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

SRC_CC3K = $(addprefix $(CC3K_DIR)/src/,\
	cc3000_common.c \
	evnt_handler.c \
	hci.c \
	netapp.c \
	nvmem.c \
	security.c \
	socket.c \
	wlan.c \
	ccspi.c \
	inet_ntop.c \
	inet_pton.c \
	)

OBJ =
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_HAL:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_USBDEV:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_FATFS:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CC3K:.c=.o))
OBJ += $(BUILD)/pins_$(BOARD).o

all: $(BUILD)/firmware.dfu $(BUILD)/firmware.hex

.PHONY: deploy

deploy: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board"
	$(Q)$(DFU_UTIL) -a 0 -d $(DEVICE) -D $<

$(BUILD)/firmware.dfu: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .isr_vector $^ $(BUILD)/firmware0.bin
	$(Q)$(OBJCOPY) -O binary -j .text -j .data $^ $(BUILD)/firmware1.bin
	$(Q)$(PYTHON) $(DFU) -b 0x08000000:$(BUILD)/firmware0.bin -b 0x08020000:$(BUILD)/firmware1.bin $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O ihex $< $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

MAKE_PINS = boards/make-pins.py
BOARD_PINS = boards/$(BOARD)/pins.csv
AF_FILE = boards/stm32f4xx-af.csv
PREFIX_FILE = boards/stm32f4xx-prefix.c
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h

INSERT_USB_IDS = ../tools/insert-usb-ids.py
FILE2H = ../tools/file2h.py

USB_IDS_FILE = usbd_desc_cdc_msc.c
CDCINF_TEMPLATE = pybcdc.inf_template
GEN_CDCINF_FILE = $(HEADER_BUILD)/pybcdc.inf
GEN_CDCINF_HEADER = $(HEADER_BUILD)/pybcdc_inf.h

# Making OBJ use an order-only depenedency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(HEADER_BUILD)/pins.h

$(BUILD)/main.o: $(GEN_CDCINF_HEADER)

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_$(BOARD).c and pins.h
$(BUILD)/%_$(BOARD).c $(HEADER_BUILD)/%.h: boards/$(BOARD)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) > $(GEN_PINS_SRC)

$(BUILD)/pins_$(BOARD).o: $(BUILD)/pins_$(BOARD).c
	$(call compile_c)

$(GEN_CDCINF_HEADER): $(GEN_CDCINF_FILE) $(FILE2H)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(FILE2H) $< > $@

$(GEN_CDCINF_FILE): $(CDCINF_TEMPLATE) $(INSERT_USB_IDS) $(USB_IDS_FILE)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(INSERT_USB_IDS) $(USB_IDS_FILE) $< > $@

include ../py/mkrules.mk
