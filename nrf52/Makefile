include ../py/mkenv.mk

CROSS_COMPILE ?= arm-none-eabi-

BTYPE ?= debug

ifeq (${BTYPE}, release)
DEFINES += NDEBUG
CFLAGS += -Os
# CFLAGS += -flto
else
ifeq (${BTYPE}, debug)
DEBUG := 1
DEFINES += DEBUG
CFLAGS += -Og -g
else
$(error Invalid BTYPE specified)
endif
endif

BOARD ?= PCA10040

BUILD = build/${BOARD}/${BTYPE}

LINKER_PATH = .
LINKER_SCRIPT = gcc_nrf52_s132.ld

# don't set PROG as it's used by mkrules.mk
PROGRAM ?= ${BUILD}/firmware

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

ifeq ($(wildcard boards/${BOARD}/.),)
$(error Invalid BOARD specified)
else
# include board makefile (if any)
-include boards/${BOARD}/build.mk
endif

# include nordic makefile
include nordic/build.mk

SRC_C += \
	main.c \
	mp_functions.c \
	nrf52_app_error.c \
	nrf52_ble.c \
	lib/mp-readline/readline.c \
	lib/utils/pyexec.c \
	lib/utils/stdout_helpers.c

# XXX I simply copied gcc_startup_nrf52.s to .S so mkrules can compile it
OBJ += $(PY_O) $(addprefix ${BUILD}/, $(SRC_C:.c=.o))

INC += -I.
INC += -I..
INC += -I${BUILD}
INC += -Iboards/${BOARD}
INC += -I../lib/mp-readline

# transform all DEFINES entry in -DDEFINE c flags
CFLAGS += $(patsubst %,-D%,${DEFINES})
CFLAGS += ${INC}
CFLAGS += -Wall -Werror -std=gnu99
CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mabi=aapcs -fsingle-precision-constant
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums

LDFLAGS += -Wl,-Map=${PROGRAM}.map
LDFLAGS += -mthumb -mabi=aapcs -L${LINKER_PATH} -T${LINKER_SCRIPT}
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nano.specs -lc -lnosys

# mkenv doesn't set these
OBJDUMP = $(CROSS_COMPILE)objdump
GDB = $(CROSS_COMPILE)gdb


all: ${PROGRAM}.hex ${PROGRAM}.bin
.PHONY: all

${PROGRAM}.elf: ${OBJ}
	$(ECHO) "LINK $@"
	${Q}${CC} -o $@ ${CFLAGS} ${LDFLAGS} $^ ${LDLIBS}
ifndef DEBUG
	${Q}$(STRIP) $(STRIPFLAGS_EXTRA) $@
endif
	${Q}$(SIZE) $@

%.hex: %.elf
	${Q}${OBJCOPY} -O ihex $< $@

%.bin: %.elf
	${Q}${OBJCOPY} -O binary $< $@

%.jlink: %.hex
	${OBJDUMP} -h $< | \
	awk '$$1 ~ /^[0-9]+$$/ {addr="0x"$$5; if (!min || addr < min) min = addr} END { printf "\
	loadbin %s,%s\n\
	sleep 100\n\
	r\n\
	g\n\
	exit\n", f, min}' f="$<" > $@

%-all.jlink: %.jlink ${SOFTDEV_HEX}
	@[ -e "${SOFTDEV_HEX}" ] || echo "cannot find softdevice hex image '${SOFTDEV_HEX}'" >&2
	# w4 0x4001e504, 0x2 -> enable erase: CONFIG.WEN = EEN
	# w4 0x4001e50c, 0x1 -> erase all: ERASEALL = 1
	printf "\
	device nrf52\n\
	halt\n\
	w4 0x4001e504, 0x2\n\
	w4 0x4001e50c, 0x1\n\
	sleep 100\n\
	r\n\
	loadbin %s,0\n" ${SOFTDEV_HEX} > $@
	cat $< >> $@

flash: ${PROGRAM}.hex ${PROGRAM}.jlink
	JLinkExe -device nrf52 -if SWD ${PROGRAM}.jlink
.PHONY: flash

flash-all: ${PROGRAM}.hex ${PROGRAM}-all.jlink
	JLinkExe -device nrf52 -if SWD ${PROGRAM}-all.jlink
.PHONY: flash-all

gdbserver: ${PROGRAM}.elf
	JLinkGDBServer -device nrf52 -if SWD
.PHONY: gdbserver

gdb: ${PROGRAM}.elf
	${GDB} ${PROGRAM}.elf -ex 'target remote :2331'
.PHONY: gdb

include ../py/mkrules.mk
