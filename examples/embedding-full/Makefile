# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2022-2023 Damien P. George
#
# This is a very simple makefile that demonstrates how to build the embed port.
# All it needs to do is build all *.c files in the micropython_embed directory.
# This makefile would be replaced with your custom build system.

EMBED_DIR = micropython_embed
PROG = embed

CFLAGS += -I.
CFLAGS += -I$(EMBED_DIR)
CFLAGS += -I$(EMBED_DIR)/port
CFLAGS += -I$(EMBED_DIR)/lib/littlefs
CFLAGS += -Wall -Og -fno-common

SRC += main.c mphal.c modules/c_hello/modc_hello.c
SRC += $(wildcard $(EMBED_DIR)/*/*.c)
# Filter out lib because the files in there cannot be compiled separately, they
# are #included by other .c files.
SRC += $(filter-out $(EMBED_DIR)/lib/%.c,$(wildcard $(EMBED_DIR)/*/*/*.c))
OBJ += $(SRC:.c=.o)

$(PROG): $(OBJ)
	$(CC) -o $@ $^ -lm

clean:
	/bin/rm -f $(OBJ) $(PROG)
