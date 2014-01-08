# define main target
all: all2

# include py core make definitions
include ../py/py.mk

# program for deletion
RM = /bin/rm

STMSRC=lib
#STMOTGSRC=lib-otg
FATFSSRC=fatfs
CC3KSRC=cc3k
DFU=../tools/dfu.py
TARGET=PYBOARD

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion -DSTM32F40XX -DHSE_VALUE=8000000
CFLAGS = -I. -I$(PY_SRC) -I$(FATFSSRC) -I$(STMSRC) -Wall -ansi -std=gnu99 -Os -DNDEBUG $(CFLAGS_CORTEX_M4) -D$(TARGET)
#CFLAGS += -I$(STMOTGSRC) -DUSE_HOST_MODE -DUSE_OTG_MODE
LDFLAGS = --nostdlib -T stm32f405.ld

SRC_C = \
	main.c \
	printf.c \
	system_stm32f4xx.c \
	stm32fxxx_it.c \
	string0.c \
	malloc0.c \
	systick.c  \
	lexerstr.c \
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
	pybwlan.c \
	i2c.c \
	usrsw.c \

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
	usb_core.c \
	usb_dcd.c \
	usb_dcd_int.c \
	usb_bsp.c \
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
	stm324x7i_eval.c \
	stm324x7i_eval_sdio_sd.c \

#SRC_STM_OTG = \
#	usb_hcd.c \
#	usb_hcd_int.c \
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

OBJ = $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o) $(SRC_FATFS:.c=.o) $(SRC_STM:.c=.o) $(SRC_CC3K:.c=.o)) $(PY_O)
#OBJ += $(addprefix $(BUILD)/, $(SRC_STM_OTG:.c=.o))

all2: $(BUILD) $(BUILD)/flash.dfu

$(BUILD)/flash.dfu: $(BUILD)/flash0.bin $(BUILD)/flash1.bin
	python $(DFU) -b 0x08000000:$(BUILD)/flash0.bin -b 0x08020000:$(BUILD)/flash1.bin $@

$(BUILD)/flash0.bin: $(BUILD)/flash.elf
	arm-none-eabi-objcopy -O binary -j .isr_vector $^ $@

$(BUILD)/flash1.bin: $(BUILD)/flash.elf
	arm-none-eabi-objcopy -O binary -j .text -j .data $^ $@

$(BUILD)/flash.elf: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)
	arm-none-eabi-size $@

$(BUILD)/%.o: %.s
	$(AS) -o $@ $<

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(FATFSSRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(STMSRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

#$(BUILD)/%.o: $(STMOTGSRC)/%.c
#	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(CC3KSRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) -rf $(BUILD)

.PHONY: all all2 clean
