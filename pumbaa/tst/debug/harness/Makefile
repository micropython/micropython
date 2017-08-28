#
# @file Makefile
#
# @section License
# Copyright (C) 2016, Erik Moqvist
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# This file is part of the Pumbaa project.
#

NAME = harness_suite
BOARD ?= linux

PYSRC += \
	$(PUMBAA_ROOT)/src/py/harness.py

CDEFS += CONFIG_PUMBAA_MAIN_FRIENDLY_REPL=0

PUMBAA_ROOT ?= ../..
include $(PUMBAA_ROOT)/make/app.mk
