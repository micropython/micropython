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

.PHONY: tags docs all help travis clean

PUMBAA_VERSION ?= $(shell cat VERSION.txt)

BOARD ?= linux

ifeq ($(BOARD), linux)
TESTS = \
	tst/smoke \
	tst/os \
	tst/select \
	tst/socket \
	tst/thread \
	tst/debug/harness \
	tst/drivers/dac \
	tst/drivers/exti \
	tst/drivers/pin \
	tst/drivers/spi \
	tst/drivers/sd \
	tst/kernel/timer \
	tst/sync/event \
	tst/sync/queue
endif

ifeq ($(BOARD), esp12e)
TESTS = \
	tst/smoke
endif

ifeq ($(BOARD), arduino_due)
TESTS = \
	tst/smoke
endif

all: $(TESTS:%=%.all)

clean: $(TESTS:%=%.clean)

rerun:
	for test in $(TESTS) ; do \
	    $(MAKE) -C $$test run || exit 1 ; \
	done

# Depend on 'all' to build all applications (optinally with -j) before
# uploading and running them one at a time.
run: all
	for test in $(TESTS) ; do \
	    if [ ! -e $$test/.$(BOARD).passed ] ; then \
	        $(MAKE) -C $$test run || exit 1 ; \
	        touch $$test/.$(BOARD).passed ; \
	    else \
	        echo ; \
	        echo "$$test already passed." ; \
	        echo ; \
	    fi \
	done

report:
	for test in $(TESTS) ; do \
	    $(MAKE) -C $(basename $$test) report ; \
	    echo ; \
	done

test: run
	$(MAKE) report

travis:
	$(MAKE) test

clean-arduino-due:
	$(MAKE) BOARD=arduino_due SERIAL_PORT=/dev/simba-arduino_due clean

clean-esp12e:
	$(MAKE) BOARD=esp12e SERIAL_PORT=/dev/simba-esp12e clean

test-arduino-due:
	@echo "Arduino Due"
	$(MAKE) BOARD=arduino_due SERIAL_PORT=/dev/simba-arduino_due test

test-esp12e:
	@echo "ESP12-E"
	$(MAKE) BOARD=esp12e SERIAL_PORT=/dev/simba-esp12e test

test-all-boards:
	$(MAKE) test-arduino-due
	$(MAKE) test-esp12e

clean-all-boards:
	$(MAKE) clean-arduino-due
	$(MAKE) clean-esp12e

codecov-coverage: $(TESTS:%=%.ccc)

$(TESTS:%=%.all):
	$(MAKE) -C $(basename $@) all

$(TESTS:%=%.clean):
	$(MAKE) -C $(basename $@) clean

$(TESTS:%=%.ccc):
	$(MAKE) -C $(basename $@) codecov-coverage

docs:
	+bin/dbgen.py > database.json
	+bin/docgen.py database.json
	$(MAKE) -C docs sphinx

tags:
	echo "Creating tags file .TAGS"
	etags -o .TAGS --declarations $$(find . -name "*.[hci]" | xargs)

arduino:
	+make/arduino/arduino.py --remove-outdir --version $(PUMBAA_VERSION)

help:
	@echo "--------------------------------------------------------------------------------"
	@echo "  target                      description"
	@echo "--------------------------------------------------------------------------------"
	@echo
