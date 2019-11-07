include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

# On Linux
#XC32 = /opt/microchip/xc32/v2.30
# On Windows with MINGW
XC32 = "C:\Program Files (x86)\Microchip\xc32\v2.30"
CROSS_COMPILE = $(XC32)/bin/xc32-

# Part family and part name
PARTFAMILY = pic32mx
PART = 32MX170F256B

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(XC32)/$(PARTFAMILY)/include

CFLAGS_PIC32 = -mprocessor=$(PART)
CFLAGS = $(INC) -DXPRJ_default=default -no-legacy-libc -std=gnu99 $(CFLAGS_PIC32) $(COPT)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -O1 -DNDEBUG
endif

# LDFLAGS and LIBS is for xc32-gcc to invoke the linker, not common linker flags, and comma(,) matters!
LDFLAGS = -mprocessor=$(PART) -O1 -DXPRJ_default=default -no-legacy-libc -std=gnu99
LIBS = -Wl,--defsym=__MPLAB_BUILD=1,--no-code-in-dinit,--no-dinit-in-serial-mem

SRC_C = \
	main.c \
	mphalport.c \
	modpyb.c \
	modpybled.c \
	modpybswitch.c \
	lib/utils/pyexec.c \
	lib/utils/sys_stdio_mphal.c \
	lib/mp-readline/readline.c \
	mcc_generated_files/delay.c \
	mcc_generated_files/interrupt_manager.c \
	mcc_generated_files/mcc.c \
	mcc_generated_files/nvm.c \
	mcc_generated_files/pin_manager.c \
	mcc_generated_files/tmr1.c \
	mcc_generated_files/uart1.c \
	

SRC_S = \
#	gchelper.s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C)
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

# To generate only "genhdr" folder with auto-generated .h files(mpversion.h, moduledefs.h, qstrdefs.generated.h)
#all: $(HEADER_BUILD)/qstrdefs.generated.h $(HEADER_BUILD)/mpversion.h
# To generate hex file
all: $(BUILD)/firmware.hex

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(CROSS_COMPILE)bin2hex $<

# The easist way to make linker work is using xc32-gcc to invoke the linker(use CC instead of LD). Need workaround in LDFLAGS and LIBS
$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)size $@

$(PY_BUILD)/gc.o: CFLAGS += -O1
$(PY_BUILD)/vm.o: CFLAGS += -O1

include $(TOP)/py/mkrules.mk
