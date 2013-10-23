STMSRC=lib
FATFSSRC=fatfs
PYSRC=../py
BUILD=build

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfloat-abi=hard -DSTM32F40XX -DHSE_VALUE=8000000
CFLAGS = -I. -I$(PYSRC) -I$(FATFSSRC) -I$(STMSRC) -Wall -ansi -std=gnu99 -Os -DNDEBUG $(CFLAGS_CORTEX_M4)
LDFLAGS = --nostdlib -T stm32f405.ld

SRC_C = \
	main.c \
	printf.c \
	system_stm32f4xx.c \
	led.c \
	lcd.c \
	flash.c \
	storage.c \
	mma.c \
	string0.c \
	malloc0.c \
	systick.c  \
	stm32fxxx_it.c \
	usb.c \
	lexerstm.c \
#	sd.c \

SRC_S = \
	startup_stm32f40xx.s \
	gchelper.s \

PY_O = \
	nlrthumb.o \
	gc.o \
	malloc.o \
	qstr.o \
	vstr.o \
	misc.o \
	lexer.o \
	parse.o \
	scope.o \
	compile.o \
	emitcommon.o \
	emitpass1.o \
	emitbc.o \
	asmthumb.o \
	emitnthumb.o \
	emitinlinethumb.o \
	runtime.o \
	vm.o \
	repl.o \

SRC_FATFS = \
	ff.c \
	diskio.c \

SRC_STM = \
	stm32f4xx_rcc.c \
	stm32f4xx_flash.c \
	stm32f4xx_dma.c \
	stm32f4xx_exti.c \
	stm32f4xx_gpio.c \
	stm32f4xx_tim.c \
	stm_misc.c \
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
	usbd_cdc_vcp.c \
	usbd_msc_bot.c  \
	usbd_msc_data.c \
	usbd_msc_scsi.c \
	usbd_storage_msd.c \

# not needed
#	usb_otg.c \
#	usb_hcd.c \
#	usb_hcd_int.c \

# for SD card
#	stm32f4xx_sdio.c \
#	stm324x7i_eval.c \
#	stm324x7i_eval_sdio_sd.c \

OBJ = $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o) $(PY_O) $(SRC_FATFS:.c=.o) $(SRC_STM:.c=.o))

all: $(BUILD) $(BUILD)/flash.dfu

$(BUILD)/flash.dfu: $(BUILD)/flash0.bin $(BUILD)/flash1.bin
	python2 ~/stm/dfu/dfu.py -b 0x08000000:$(BUILD)/flash0.bin -b 0x08020000:$(BUILD)/flash1.bin $@

$(BUILD)/flash0.bin: $(BUILD)/flash.elf
	arm-none-eabi-objcopy -O binary -j .isr_vector $^ $@

$(BUILD)/flash1.bin: $(BUILD)/flash.elf
	arm-none-eabi-objcopy -O binary -j .text -j .data $^ $@

$(BUILD)/flash.elf: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)
	arm-none-eabi-size $@

$(BUILD):
	mkdir $@

$(BUILD)/%.o: %.s
	$(AS) -o $@ $<

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(FATFSSRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(STMSRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(PYSRC)/%.s
	$(AS) -c -o $@ $<

$(BUILD)/%.o: $(PYSRC)/%.c mpyconfig.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/emitnthumb.o: $(PYSRC)/emitnative.c $(PYSRC)/emit.h
	$(CC) $(CFLAGS) -DN_THUMB -c -o $@ $<

# optimising gc for speed; 5ms down to 4ms
$(BUILD)/gc.o: $(PYSRC)/gc.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

# optimising vm for speed, adds only a small amount to code size but makes a huge difference to speed (20% faster)
$(BUILD)/vm.o: $(PYSRC)/vm.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

$(BUILD)/parse.o: $(PYSRC)/grammar.h
$(BUILD)/compile.o: $(PYSRC)/grammar.h
$(BUILD)/emitbc.o: $(PYSRC)/emit.h

clean:
	/bin/rm -r $(BUILD)

.PHONY: all clean
