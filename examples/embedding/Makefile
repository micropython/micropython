# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2025 Anson Mansfield

all: mpy
	$(MAKE) -f makefile.mk
clean::
	$(MAKE) -f makefile.mk clean

mpy:
	$(MAKE) -f micropython_embed.mk
clean::
	$(MAKE) -f micropython_embed.mk clean

.PHONY: all clean mpy
