VARIANT ?= standard
BUILD ?= build-$(VARIANT)
include ../../py/mkenv.mk
PORT_DIR ?= $(TOP)/ports/unix
BUILD_DIR = $(PORT_DIR)/$(BUILD)

CFLAGS = \
	-Wall -Werror -I$(TOP) -I$(TOP)/py -I$(PORT_DIR) -I$(PORT_DIR)/variants/$(VARIANT) -I$(BUILD_DIR)

ifeq ($(MICROPY_FORCE_32BIT),1)
CC += -m32
endif

all:
	$(CC) -fPIC $(CFLAGS) -c dynmod.c -o $(BUILD_DIR)/dynmod.o
	$(CC) -shared -o $(BUILD_DIR)/dynmod.so $(BUILD_DIR)/dynmod.o

clean:
	$(RM) $(BUILD_DIR)/dynmod.*
