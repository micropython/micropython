# where embed object files go (they have a name prefix to prevent filename clashes)
EMBED_BUILD = $(BUILD)/embed

# embed object files
EMBED_O_BASENAME = \
	hal_gpio.o \

# prepend the build destination prefix to the embed object files
EMBED_O = $(addprefix $(EMBED_BUILD)/, $(EMBED_O_BASENAME))

# Relative to the top of the mbed tree

MBED_TOP     = $(TOP)/../mbed/libraries/mbed
MBED_API     = $(MBED_TOP)/api
MBED_COMMON  = $(MBED_TOP)/common
MBED_HAL_INC = $(MBED_TOP)/hal
MBED_HAL_SRC = $(MBED_TOP)/targets/hal/TARGET_STM/TARGET_STM32F4XX

CFLAGS_EMBED = -I$(TOP)/embed -I$(MBED_API) -I$(MBED_HAL_INC) -I$(MBED_HAL_SRC)

vpath %.c $(MBED_HAL_SRC)
SRC_HAL = \
	pinmap.c \
	analogin_api.c \
	gpio_api.c \

vpath %.c $(MBED_COMMON)
SRC_HAL += \
	pinmap_common.c \
	exit.c \

