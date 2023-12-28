# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2022 MicroDev
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Common Makefile items that can be shared across CircuitPython ports.

ALL_BOARDS_IN_PORT := $(patsubst boards/%/mpconfigboard.mk,%,$(wildcard boards/*/mpconfigboard.mk))
# An incorrect BOARD might have been specified, so check against the list.
# There is deliberately no space after the :=
VALID_BOARD :=$(filter $(BOARD),$(ALL_BOARDS_IN_PORT))

# If the flash PORT is not given, use the default /dev/tty.SLAB_USBtoUART.
PORT ?= /dev/tty.SLAB_USBtoUART

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

# First makefile with targets. Defines the default target.
include ../../py/mkenv.mk

# Board-specific. Skip if the rule requested is not board-specific.
ifneq ($(VALID_BOARD),)
include boards/$(BOARD)/mpconfigboard.mk
endif

# Port-specific
include mpconfigport.mk

# Also board-specific. Skip if the rule requested is not board-specific.
ifneq ($(VALID_BOARD),)
# CircuitPython-specific
include $(TOP)/py/circuitpy_mpconfig.mk
endif

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

include $(TOP)/supervisor/supervisor.mk

# Include make rules and variables common across CircuitPython builds.
include $(TOP)/py/circuitpy_defns.mk
