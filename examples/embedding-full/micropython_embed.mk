# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2022-2023 Damien P. George

# Set the location of the top of the MicroPython repository.
MICROPYTHON_TOP = ../..

# Include modules from extmod in the output.
EMBED_EXTRA = extmod

# Freeze Python modules.
FROZEN_MANIFEST ?= manifest.py

# Add C modules.
USER_C_MODULES = modules

# Include the main makefile fragment to build the MicroPython component.
include $(MICROPYTHON_TOP)/ports/embed/embed.mk
