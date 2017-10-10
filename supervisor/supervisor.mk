SRC_SUPERVISOR = \
	main.c \
	supervisor/port.c \
	supervisor/shared/autoreload.c \
	supervisor/shared/rgb_led_status.c

ifeq ($(wildcard atmel-samd/supervisor/filesystem.c),)
	SRC_SUPERVISOR += supervisor/filesystem.c
else
	SRC_SUPERVISOR += supervisor/stub/filesystem.c
endif

ifeq ($(wildcard atmel-samd/supervisor/serial.c),)
	SRC_SUPERVISOR += supervisor/serial.c
else
	SRC_SUPERVISOR += supervisor/stub/serial.c
endif

SUPERVISOR_O = $(addprefix $(BUILD)/, $(SRC_SUPERVISOR:.c=.o))
