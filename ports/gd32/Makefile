# Location of top-level MicroPython directory
TOP := $(abspath $(CURDIR)/../..)

include $(TOP)/py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

include $(TOP)/py/py.mk

CROSS_COMPILE ?= arm-none-eabi-

# ---- board selection ----
BOARD ?= $(error BOARD not specified. Use BOARD=GD32F407 or BOARD=GD32F470)
BOARD_DIR = boards/$(BOARD)
ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error BOARD=$(BOARD) not found in boards/)
endif

include $(BOARD_DIR)/mpconfigboard.mk

# ---- pin generation ----
GEN_PINS_PY  = gen_pins.py
BOARD_PINS   = $(BOARD_DIR)/pins.csv
GEN_PINS_DIR = $(BUILD)/genhdr
GEN_PINS_HDR = $(GEN_PINS_DIR)/pins.h
GEN_PINS_TBL = $(GEN_PINS_DIR)/pin_table.h


# ---- MCU / core flags ----
MCU_ARCH = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS += $(MCU_ARCH)
CFLAGS += -D$(CMSIS_MCU) -DUSE_STDPERIPH_DRIVER
CFLAGS += -Os -g -std=gnu99 -Wall -Werror
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-common -fno-strict-aliasing
CFLAGS += -DNDEBUG
CFLAGS += -fsingle-precision-constant -Wdouble-promotion

# ---- includes ----
INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(BOARD_DIR)
INC += -Igd32lib/CMSIS_include
INC += -Igd32lib/peripheral_include
INC += -I$(TOP)/lib/oofatfs

CFLAGS += $(INC)

# ---- sources ----
SRC_C = \
	main.c \
	extmod/modmachine.c \
	extmod/machine_timer.c \
	extmod/machine_wdt.c \
	extmod/machine_uart.c \
	extmod/machine_spi.c \
	extmod/machine_i2c.c \
	extmod/machine_adc.c \
	shared/runtime/softtimer.c \
	shared/runtime/mpirq.c \
	machine_pin.c \
	machine_spi_port.c \
	machine_i2c_port.c \
	machine_dac_port.c \
	machine_rtc_port.c \
	mphalport.c \
	shared/libc/printf.c \
	shared/libc/string0.c \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	gd32lib/system_gd32f4xx.c \
	gd32lib/peripheral_source/gd32f4xx_gpio.c \
	gd32lib/peripheral_source/gd32f4xx_rcu.c \
	gd32lib/peripheral_source/gd32f4xx_usart.c \
	gd32lib/peripheral_source/gd32f4xx_spi.c \
	gd32lib/peripheral_source/gd32f4xx_i2c.c \
	gd32lib/peripheral_source/gd32f4xx_adc.c \
	gd32lib/peripheral_source/gd32f4xx_dac.c \
	gd32lib/peripheral_source/gd32f4xx_rtc.c \
	gd32lib/peripheral_source/gd32f4xx_can.c \
	gd32lib/peripheral_source/gd32f4xx_misc.c \
	gd32lib/peripheral_source/gd32f4xx_fmc.c \
	gd32lib/peripheral_source/gd32f4xx_pmu.c \
	gd32lib/peripheral_source/gd32f4xx_fwdgt.c \

SRC_S = gd32lib/GCC/$(STARTUP_FILE)

SRC_QSTR += shared/readline/readline.c shared/runtime/pyexec.c machine_pin.c \
	extmod/modmachine.c extmod/machine_timer.c extmod/machine_wdt.c \
	extmod/machine_uart.c extmod/machine_spi.c extmod/machine_i2c.c machine_uart_port.c machine_spi_port.c machine_i2c_port.c machine_adc_port.c machine_dac_port.c machine_rtc_port.c extmod/machine_adc.c machine_wdt_port.c shared/runtime/mpirq.c

OBJ += $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.S=.o))

# ---- linker ----
LD_FILES = ld/$(LD_SCRIPT)
LDFLAGS += $(MCU_ARCH)
LDFLAGS += -T $(LD_FILES)
LDFLAGS += -Wl,-Map=$@.map,--cref -Wl,--gc-sections
LDFLAGS += -Wl,--print-memory-usage
LDFLAGS += -Wl,--defsym=__stack_size=0x2000
LIBS += --specs=nano.specs --specs=nosys.specs -lm -lc -lgcc

# ---- targets ----
all: $(BUILD)/firmware.elf $(BUILD)/firmware.bin $(BUILD)/firmware.hex

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "BIN $@"
	$(Q)$(OBJCOPY) -O binary $< $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(ECHO) "HEX $@"
	$(Q)$(OBJCOPY) -O ihex $< $@


# ---- generated pin tables ----
$(GEN_PINS_HDR) $(GEN_PINS_TBL): $(BOARD_PINS) $(GEN_PINS_PY)
	$(ECHO) "GEN pins for $(BOARD)"
	$(Q)mkdir -p $(GEN_PINS_DIR)
	$(Q)$(PYTHON) $(GEN_PINS_PY) $(BOARD_PINS) $(GEN_PINS_DIR) $(BOARD)

# ensure objects wait for generated headers
$(OBJ): $(GEN_PINS_HDR) $(GEN_PINS_TBL)

clean:
	$(RM) -rf $(BUILD)

.PHONY: all clean


# ---- export artifacts per board ----
DIST_DIR = dist/$(BOARD)

dist: all
	$(ECHO) "Export $@ artifacts"
	$(Q)mkdir -p $(DIST_DIR)
	$(Q)cp $(BUILD)/firmware.elf $(BUILD)/firmware.bin $(BUILD)/firmware.hex $(DIST_DIR)/ 2>/dev/null || true
	$(Q)ls -lh $(DIST_DIR)

.PHONY: dist

include $(TOP)/py/mkrules.mk
