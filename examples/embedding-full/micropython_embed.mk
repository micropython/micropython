# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2022-2023 Damien P. George

# Set the location of the top of the MicroPython repository.
MICROPYTHON_TOP = ../..

# Include modules from extmod in the output.
EMBED_EXTRA = extmod

# Include helper sources for the time module in the output.
EMBED_EXTRA += \
	shared/timeutils/timeutils.c \
	shared/timeutils/timeutils.h \
	shared/timeutils/modtime_mphal.h

# Include source for mphal-backed stdio in the output.
# Disable when using POSIX-backed stdio (MICROPY_VFS_POSIX).
EMBED_EXTRA += \
	shared/runtime/sys_stdio_mphal.c

# Freeze Python modules.
FROZEN_MANIFEST ?= manifest.py

# Add C modules.
USER_C_MODULES = modules

# Include the main makefile fragment to build the MicroPython component.
include $(MICROPYTHON_TOP)/ports/embed/embed.mk
