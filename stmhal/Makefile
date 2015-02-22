# Select the board to build for: if not given on the command line,
# then default to PYBV10.
BOARD ?= PYBV10
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

include ../py/mkenv.mk
-include mpconfigport.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(BUILD)/pins_qstr.h

# include py core make definitions
include ../py/py.mk

CMSIS_DIR=cmsis
HAL_DIR=hal
USBDEV_DIR=usbdev
#USBHOST_DIR=usbhost
FATFS_DIR=lib/fatfs
DFU=../tools/dfu.py
# may need to prefix dfu-util with sudo
USE_PYDFU ?= 0
PYDFU = ../tools/pydfu.py
DFU_UTIL ?= dfu-util
DEVICE=0483:df11

CROSS_COMPILE = arm-none-eabi-

INC =  -I.
INC += -I..
INC += -I$(BUILD)
INC += -I$(CMSIS_DIR)/inc
INC += -I$(CMSIS_DIR)/devinc
INC += -I$(HAL_DIR)/inc
INC += -I$(USBDEV_DIR)/core/inc -I$(USBDEV_DIR)/class/inc
#INC += -I$(USBHOST_DIR)
INC += -I../$(FATFS_DIR)
INC += -I../lib/mp-readline

CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS = $(INC) -Wall -Wpointer-arith -Werror -ansi -std=gnu99 -nostdlib $(CFLAGS_MOD) $(CFLAGS_CORTEX_M4) $(COPT)
CFLAGS += -Iboards/$(BOARD)

LDFLAGS = -nostdlib -T stm32f405.ld -Map=$(@:.elf=.map) --cref
LIBS =

# Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g -DPENDSV_DEBUG
COPT = -O0
else
CFLAGS += -fdata-sections -ffunction-sections
COPT += -Os -DNDEBUG
LDFLAGS += --gc-sections
endif

# uncomment this if you want libgcc
#LIBS += $(shell $(CC) -print-libgcc-file-name)

SRC_LIB = $(addprefix lib/,\
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
	fatfs/option/ccsbcs.c \
	mp-readline/readline.c \
	)

SRC_C = \
	main.c \
	string0.c \
	system_stm32f4xx.c \
	stm32f4xx_it.c \
	usbd_conf.c \
	usbd_desc.c \
	usbd_cdc_interface.c \
	usbd_msc_storage.c \
	mphal.c \
	irq.c \
	pendsv.c \
	systick.c  \
	timer.c \
	led.c \
	pin.c \
	pin_defs_stmhal.c \
	pin_named_pins.c \
	bufhelper.c \
	i2c.c \
	spi.c \
	uart.c \
	can.c \
	usb.c \
	printf.c \
	gccollect.c \
	pybstdio.c \
	pyexec.c \
	help.c \
	input.c \
	modpyb.c \
	modstm.c \
	moduos.c \
	modutime.c \
	moduselect.c \
	modusocket.c \
	modnetwork.c \
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
	fsusermount.c \
	diskio.c \
	ffconf.c \
	lcd.c \
	accel.c \
	servo.c \
	dac.c \
	adc.c \

SRC_S = \
	startup_stm32f40xx.s \
	gchelper.s \

SRC_HAL = $(addprefix $(HAL_DIR)/src/,\
	stm32f4xx_hal.c \
	stm32f4xx_hal_adc.c \
	stm32f4xx_hal_adc_ex.c \
	stm32f4xx_hal_can.c \
	stm32f4xx_hal_cortex.c \
	stm32f4xx_hal_dac.c \
	stm32f4xx_hal_dac_ex.c \
	stm32f4xx_hal_dma.c \
	stm32f4xx_hal_flash.c \
	stm32f4xx_hal_flash_ex.c \
	stm32f4xx_hal_gpio.c \
	stm32f4xx_hal_i2c.c \
	stm32f4xx_hal_pcd.c \
	stm32f4xx_hal_pcd_ex.c \
	stm32f4xx_hal_pwr.c \
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
	class/src/usbd_cdc_msc_hid.c \
	class/src/usbd_msc_bot.c \
	class/src/usbd_msc_scsi.c \
	class/src/usbd_msc_data.c \
	)

ifeq ($(MICROPY_PY_WIZNET5K),1)
WIZNET5K_DIR=drivers/wiznet5k
INC += -I$(TOP)/$(WIZNET5K_DIR)
CFLAGS_MOD += -DMICROPY_PY_WIZNET5K=1
SRC_MOD += modnwwiznet5k.c
SRC_MOD += $(addprefix $(WIZNET5K_DIR)/,\
	ethernet/w5200/w5200.c \
	ethernet/wizchip_conf.c \
	ethernet/socket.c \
	internet/dns/dns.c \
	)
endif

# for CC3000 module
ifeq ($(MICROPY_PY_CC3K),1)
CC3000_DIR=drivers/cc3000
INC += -I$(TOP)/$(CC3000_DIR)/inc
CFLAGS_MOD += -DMICROPY_PY_CC3K=1
SRC_MOD += modnwcc3k.c
SRC_MOD += $(addprefix $(CC3000_DIR)/src/,\
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
	patch.c \
	patch_prog.c \
	)
endif

OBJ =
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_HAL:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_USBDEV:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))
OBJ += $(BUILD)/pins_$(BOARD).o

# We put ff.o and stm32f4xx_hal_sd.o into the first 16K section with the ISRs.
# If we compile these using -O0 then it won't fit. So if you really want these
# to be compiled with -O0, then edit stm32f405.ld (in the .isr_vector section)
# and comment out the following 2 lines.
$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(BUILD)/$(HAL_DIR)/src/stm32f4xx_hal_sd.o: COPT += -Os

all: $(BUILD)/firmware.dfu $(BUILD)/firmware.hex

.PHONY: deploy

deploy: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board"
ifeq ($(USE_PYDFU),1)
	$(Q)$(PYTHON) $(PYDFU) -u $<
else
	$(Q)$(DFU_UTIL) -a 0 -d $(DEVICE) -D $<
endif

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
AF_FILE = boards/stm32f4xx_af.csv
PREFIX_FILE = boards/stm32f4xx_prefix.c
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(BUILD)/pins_qstr.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_PY = $(BUILD)/pins_af.py

INSERT_USB_IDS = ../tools/insert-usb-ids.py
FILE2H = ../tools/file2h.py

USB_IDS_FILE = usbd_desc.c
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
$(BUILD)/%_$(BOARD).c $(HEADER_BUILD)/%.h $(HEADER_BUILD)/%_af_const.h $(BUILD)/%_qstr.h: boards/$(BOARD)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) --af-const $(GEN_PINS_AF_CONST) --af-py $(GEN_PINS_AF_PY) > $(GEN_PINS_SRC)

$(BUILD)/pins_$(BOARD).o: $(BUILD)/pins_$(BOARD).c
	$(call compile_c)

$(GEN_CDCINF_HEADER): $(GEN_CDCINF_FILE) $(FILE2H)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(FILE2H) $< > $@

$(GEN_CDCINF_FILE): $(CDCINF_TEMPLATE) $(INSERT_USB_IDS) $(USB_IDS_FILE)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(INSERT_USB_IDS) $(USB_IDS_FILE) $< > $@

include ../py/mkrules.mk
