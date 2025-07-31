APP_INC =  -I.
APP_INC += -I$(TOP)
APP_INC += -Ifatfs/src
APP_INC += -Ifatfs/src/drivers
APP_INC += -IFreeRTOS
APP_INC += -IFreeRTOS/Source/include
APP_INC += -IFreeRTOS/Source/portable/GCC/ARM_CM3
APP_INC += -Iftp
APP_INC += -Ihal
APP_INC += -Ihal/inc
APP_INC += -Imisc
APP_INC += -Imods
APP_INC += -I$(TOP)/drivers/cc3100/inc
APP_INC += -Isimplelink
APP_INC += -Isimplelink/oslib
APP_INC += -Itelnet
APP_INC += -Iutil
APP_INC += -Ibootmgr
APP_INC += -I$(BUILD)
APP_INC += -I$(BUILD)/genhdr

APP_CPPDEFINES = -Dgcc -DTARGET_IS_CC3200 -DSL_FULL -DUSE_FREERTOS

APP_FATFS_SRC_C = $(addprefix fatfs/src/,\
	drivers/sflash_diskio.c \
	drivers/sd_diskio.c \
	)

APP_RTOS_SRC_C = $(addprefix FreeRTOS/Source/,\
	croutine.c \
	event_groups.c \
	list.c \
	queue.c \
	tasks.c \
	timers.c \
	portable/GCC/ARM_CM3/port.c \
	portable/MemMang/heap_4.c \
	)

APP_FTP_SRC_C = $(addprefix ftp/,\
	ftp.c \
	updater.c \
	)

APP_HAL_SRC_C = $(addprefix hal/,\
	adc.c \
	aes.c \
	cc3200_hal.c \
	cpu.c \
	crc.c \
	des.c \
	gpio.c \
	i2c.c \
	i2s.c \
	interrupt.c \
	pin.c \
	prcm.c \
	sdhost.c \
	shamd5.c \
	spi.c \
	startup_gcc.c \
	systick.c \
	timer.c \
	uart.c \
	utils.c \
	wdt.c \
	)

APP_MISC_SRC_C = $(addprefix misc/,\
	antenna.c \
	FreeRTOSHooks.c \
	help.c \
	mpirq.c \
	mperror.c \
	)

APP_MODS_SRC_C = $(addprefix mods/,\
	modnetwork.c \
	modsocket.c \
	modssl.c \
	modwipy.c \
	modwlan.c \
	pybadc.c \
	pybpin.c \
	pybi2c.c \
	pybrtc.c \
	pybflash.c \
	pybsd.c \
	pybsleep.c \
	pybspi.c \
	pybtimer.c \
	pybuart.c \
	)

APP_CC3100_SRC_C = $(addprefix drivers/cc3100/src/,\
	device.c \
	driver.c \
	flowcont.c \
	fs.c \
	netapp.c \
	netcfg.c \
	socket.c \
	wlan.c \
	)

APP_SL_SRC_C = $(addprefix simplelink/,\
	oslib/osi_freertos.c \
	cc_pal.c \
	)

APP_TELNET_SRC_C = $(addprefix telnet/,\
	telnet.c \
	)

APP_UTIL_SRC_C = $(addprefix util/,\
	cryptohash.c \
	fifo.c \
	gccollect.c \
	random.c \
	socketfifo.c \
	)

APP_UTIL_SRC_S = $(addprefix util/,\
	cortex_m3_get_sp.s \
	sleeprestore.s \
	)

APP_MAIN_SRC_C = \
	main.c \
	mptask.c \
	mpthreadport.c \
	serverstask.c \
	fatfs_port.c \

APP_LIB_SRC_C = $(addprefix lib/,\
	oofatfs/ff.c \
	oofatfs/ffunicode.c \
	)

APP_SHARED_SRC_C = $(addprefix shared/,\
	libc/string0.c \
	readline/readline.c \
	netutils/netutils.c \
	timeutils/timeutils.c \
	runtime/gchelper_native.c \
	runtime/pyexec.c \
	runtime/interrupt_char.c \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	)

APP_STM_SRC_C = $(addprefix ports/stm32/,\
	bufhelper.c \
	)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(APP_FATFS_SRC_C:.c=.o) $(APP_RTOS_SRC_C:.c=.o) $(APP_FTP_SRC_C:.c=.o) $(APP_HAL_SRC_C:.c=.o) $(APP_MISC_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(APP_MODS_SRC_C:.c=.o) $(APP_CC3100_SRC_C:.c=.o) $(APP_SL_SRC_C:.c=.o) $(APP_TELNET_SRC_C:.c=.o) $(APP_UTIL_SRC_C:.c=.o) $(APP_UTIL_SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(APP_MAIN_SRC_C:.c=.o) $(APP_SHARED_SRC_C:.c=.o) $(APP_LIB_SRC_C:.c=.o) $(APP_STM_SRC_C:.c=.o))
OBJ += $(BUILD)/shared/runtime/gchelper_thumb2.o
OBJ += $(BUILD)/pins.o

# List of sources for qstr extraction
SRC_QSTR += $(APP_MODS_SRC_C) $(APP_MISC_SRC_C) $(APP_STM_SRC_C) $(APP_SHARED_SRC_C) $(APP_HAL_SRC_C) $(GEN_PINS_SRC)

# Add the linker script
LINKER_SCRIPT = application.lds
LDFLAGS += -T $(LINKER_SCRIPT)

# Add the application specific CFLAGS
CFLAGS += $(APP_CPPDEFINES) $(APP_INC)

# Disable strict aliasing for the simplelink driver
$(BUILD)/drivers/cc3100/src/driver.o: CFLAGS += -fno-strict-aliasing

# Check if we would like to debug the port code
ifeq ($(BTYPE), release)
CFLAGS += -DNDEBUG
else
ifeq ($(BTYPE), debug)
CFLAGS += -DDEBUG
else
$(error Invalid BTYPE specified)
endif
endif

SHELL = bash
APP_SIGN = appsign.sh
UPDATE_WIPY ?= tools/update-wipy.py
WIPY_IP ?= '192.168.1.1'
WIPY_USER ?= 'micro'
WIPY_PWD ?= 'python'

all: $(BUILD)/firmware.zip

.PHONY: deploy-ota

deploy-ota: $(BUILD)/mcuimg.bin
	$(ECHO) "Writing $< to the board"
	$(Q)$(PYTHON) $(UPDATE_WIPY) --verify --ip $(WIPY_IP) --user $(WIPY_USER) --password $(WIPY_PWD) --file $<

$(BUILD)/application.axf: $(OBJ) $(LINKER_SCRIPT)
	$(ECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/application.bin: $(BUILD)/application.axf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $< $@

$(BUILD)/mcuimg.bin: $(BUILD)/application.bin
	$(ECHO) "Create $@"
	$(Q)$(SHELL) $(APP_SIGN) $(BUILD)

$(BUILD)/firmware.zip: $(BUILD)/mcuimg.bin
	$(ECHO) "Create $@"
	$(Q)$(ZIP) -j $@ $<

MAKE_PINS = boards/make-pins.py
BOARD_PINS = boards/$(BOARD)/pins.csv
AF_FILE = boards/cc3200_af.csv
PREFIX_FILE = boards/cc3200_prefix.c
GEN_PINS_SRC = $(BUILD)/pins.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h

# Making OBJ use an order-only dependency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(GEN_PINS_HDR)

# Call make-pins.py to generate both pins_gen.c and pins.h
$(GEN_PINS_SRC) $(GEN_PINS_HDR): $(BOARD_PINS) $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board-csv $(BOARD_PINS) --af-csv $(AF_FILE) --prefix $(PREFIX_FILE) \
	    --output-source $(GEN_PINS_SRC) --output-header $(GEN_PINS_HDR)
