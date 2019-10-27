DRIVERS_SECUREBOOT_DIR = drivers/secureboot

SRC_SECUREBOOT += $(addprefix $(DRIVERS_SECUREBOOT_DIR)/,\
        secureboot_main.c \
        )

SRC_SECUREBOOT += $(addprefix device/,\
        startup_nrf9160.c \
        )

SRC_SECUREBOOT += $(addprefix $(TOP)/lib/nrfx/mdk/,\
        system_nrf9160.c \
        )

.PHONY: secureboot clean

INC_SECUREBOOT += -I./../../lib/nrfx/mdk
INC_SECUREBOOT += -I./../../lib/cmsis/inc

MCU_SERIES = m33

CFLAGS_CORTEX_M = -mthumb -mabi=aapcs -fsingle-precision-constant -Wdouble-promotion

CFLAGS_MCU_m33 = $(CFLAGS_CORTEX_M) -mcpu=cortex-m33 -march=armv8-m.main+dsp -mcmse -mfpu=fpv5-sp-d16 -mfloat-abi=hard


CFLAGS_SECUREBOOT += -DNRF9160_XXAA
CFLAGS_SECUREBOOT += $(CFLAGS_MCU_$(MCU_SERIES))
CFLAGS_SECUREBOOT += -Wall -Werror -g -ansi -std=c11 -nostdlib $(COPT)
CFLAGS_SECUREBOOT += -fno-strict-aliasing

LD_FILES_SECUREBOOT += nrf9160_1M_256k_secure.ld common.ld

LDFLAGS_SECUREBOOT = $(CFLAGS_SECUREBOOT)
LDFLAGS_SECUREBOOT += -Xlinker -Map=$(@:.elf=.map)
LDFLAGS_SECUREBOOT += -mthumb -mabi=aapcs $(addprefix -T,$(LD_FILES_SECUREBOOT)) -L ./boards

CC = arm-none-eabi-gcc
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS_SECUREBOOT) -print-libgcc-file-name)
LIBC_FILE_NAME = $(shell $(CC) $(CFLAGS_SECUREBOOT) -print-file-name=libc.a)
LIBS_SECUREBOOT += -L $(dir $(LIBGCC_FILE_NAME)) -lgcc
LIBS_SECUREBOOT += -L $(dir $(LIBC_FILE_NAME)) -lc

$(BUILD)/secureboot.elf:
	$(Q)$(CC) $(LDFLAGS_SECUREBOOT) $(SRC_SECUREBOOT) $(INC_SECUREBOOT) -O3 -o $@ $(LIBS_SECUREBOOT)
	$(SIZE) $@

$(BUILD)/secureboot.hex: $(BUILD)/secureboot.elf
	$(OBJCOPY) -O ihex $< $@

secureboot: $(BUILD)/secureboot.hex
	@echo "Secure boot"
