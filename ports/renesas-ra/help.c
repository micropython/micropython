/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021, 2022 Renesas Electronics Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/builtin.h"

const char ra_help_text[] =
    "Welcome to MicroPython for Renesas RA!\n"
    "\n"
    "For online docs please visit http://docs.micropython.org/.\n"
    "\n"
    "For access to hardware please use machine module.\n"
    "import machine\n"
    "from machine import Pin\n"
    "machine.info() -- print some general information\n"
    "machine.freq() -- print some general information\n"
    "p0 = Pin('P106',Pin.OUT) -- get a pin 'P106'\n"
    "p0.value(1)    -- Write 1 to pin 'P106'\n"
    "p0.value()     -- Read from pin 'P106'\n"
    "Pin name: 'P000' - 'P915', Pin mode: Pin.IN, Pin.OUT\n"
    "Pin pull: PULL_NONE, PULL_UP, PULL_DOWN\n"
    "\n"
    "For change boot mode by reset with pressing switch\n"
    "Normal: release switch after LED1 flashes 4 times or more\n"
    "Safe: release switch after LED1 flashes 2 times\n"
    "Factory filesystem: release switch after LED1 flashes 3 times\n"
    "*Note that Flash file system is erased and initialized.\n"
    "\n"
    "Control commands:\n"
    "  CTRL-A        -- on a blank line, enter raw REPL mode\n"
    "  CTRL-B        -- on a blank line, enter normal REPL mode\n"
    "  CTRL-C        -- interrupt a running program\n"
    "  CTRL-D        -- on a blank line, do a soft reset of the board\n"
    "  CTRL-E        -- on a blank line, enter paste mode\n"
    "\n"
    "For further help on a specific object, type help(obj)\n"
    "For a list of available modules, type help('modules')\n"
;
