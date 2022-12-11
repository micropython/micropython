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

# Select the board to build for.
define show_board_error
$(info Valid boards:)
$(shell printf '%s\n' $(patsubst boards/%/mpconfigboard.mk,%,$(wildcard boards/*/mpconfigboard.mk)) | column -xc $$(tput cols || echo 80) 1>&2)
$(error Rerun with $(MAKE) BOARD=<board>)
endef

ifeq ($(BOARD),)
    $(info No BOARD specified)
    $(call show_board_error)
else
  ifeq ($(wildcard boards/$(BOARD)/.),)
    $(info Invalid BOARD specified)
    $(call show_board_error)
  endif
endif

# If the flash PORT is not given, use the default /dev/tty.SLAB_USBtoUART.
PORT ?= /dev/tty.SLAB_USBtoUART

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

include ../../py/mkenv.mk

# Board-specific
include boards/$(BOARD)/mpconfigboard.mk

# Port-specific
include mpconfigport.mk

# CircuitPython-specific
include $(TOP)/py/circuitpy_mpconfig.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

include $(TOP)/supervisor/supervisor.mk

# Include make rules and variables common across CircuitPython builds.
include $(TOP)/py/circuitpy_defns.mk
