# define main target
all: all2

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

# program for deletion
RM = /bin/rm
ECHO = @echo

CMSIS=cmsis
STMSRC=lib
#STMOTGSRC=lib-otg
FATFSSRC=fatfs
CC3KSRC=cc3k
DFU=../tools/dfu.py

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS = -I. -I$(PY_SRC) -I$(FATFSSRC) -I$(CMSIS) -I$(STMSRC) -Wall -ansi -std=gnu99 $(CFLAGS_CORTEX_M4)
#CFLAGS += -I$(STMOTGSRC) -DUSE_HOST_MODE #-DUSE_OTG_MODE

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

LDFLAGS = --nostdlib -T stm32f405.ld
LIBS =

# uncomment this if you want libgcc
#LIBS += $(shell $(CC) -print-libgcc-file-name)

SRC_C = \
	main.c \
	printf.c \
	system_stm32f4xx.c \
	stm32fxxx_it.c \
	string0.c \
	malloc0.c \
	systick.c  \
	lexerfatfs.c \
	led.c \
	lcd.c \
	servo.c \
	flash.c \
	storage.c \
	mma.c \
	usart.c \
	usb.c \
	timer.c \
	audio.c \
	sdio.c \
	i2c.c \
	usrsw.c \
	adc.c \
#	pybwlan.c \

SRC_S = \
	startup_stm32f40xx.s \
	gchelper.s \

SRC_FATFS = \
	ff.c \
	diskio.c \

SRC_STM = \
	stm32f4xx_rcc.c \
	stm32f4xx_syscfg.c \
	stm_misc.c \
	stm32f4xx_flash.c \
	stm32f4xx_dma.c \
	stm32f4xx_gpio.c \
	stm32f4xx_exti.c \
	stm32f4xx_tim.c \
	stm32f4xx_sdio.c \
	stm32f4xx_pwr.c \
	stm32f4xx_rtc.c \
	stm32f4xx_usart.c \
	stm32f4xx_spi.c \
	stm32f4xx_dac.c \
	stm32f4xx_rng.c \
	stm32f4xx_i2c.c \
	stm32f4xx_adc.c \
	stm324x7i_eval.c \
	stm324x7i_eval_sdio_sd.c \
	usb_core.c \
	usb_bsp.c \
	usb_dcd.c \
	usb_dcd_int.c \
	usbd_core.c \
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

#SRC_STM_OTG = \
	usb_hcd.c \
	usb_hcd_int.c \
	usbh_core.c \
	usbh_hcs.c \
	usbh_stdreq.c \
	usbh_ioreq.c \
	usbh_usr.c \
	usbh_hid_core.c \
	usbh_hid_mouse.c \
	usbh_hid_keybd.c \
#	usb_otg.c \

SRC_CC3K = \
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

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o) $(SRC_FATFS:.c=.o) $(SRC_STM:.c=.o)) # $(SRC_CC3K:.c=.o))
#OBJ += $(addprefix $(BUILD)/, $(SRC_STM_OTG:.c=.o))

all2: $(BUILD) $(BUILD)/flash.dfu

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

$(BUILD)/%.o: %.s
	$(ECHO) "AS $<"
	$(Q)$(AS) -o $@ $<

$(BUILD)/%.o: %.c $(QSTR_DEFS)
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(FATFSSRC)/%.c
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(STMSRC)/%.c
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

#$(BUILD)/%.o: $(STMOTGSRC)/%.c
#	$(ECHO) "CC $<"
#	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(CC3KSRC)/%.c
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) -rf $(BUILD)

.PHONY: all all2 clean
