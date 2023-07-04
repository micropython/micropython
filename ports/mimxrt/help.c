/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

const char mimxrt_help_text[] =
    "Welcome to MicroPython!\n"
    "\n"
    "For online docs please visit http://docs.micropython.org/\n"
    "\n"
    "For access to the hardware use the 'machine' module. \n"
    "\n"
    "Quick overview of some objects:\n"
    "  machine.Pin(pin) -- get a pin, eg machine.Pin(0)\n"
    "  machine.Pin(pin, m, [p]) -- get a pin and configure it for IO mode m, pull mode p\n"
    "    methods: init(..), value([v]), high(), low())\n"
    "\n"
    "    Pins are numbered board specific, either 0-n, or 'D0'-'Dn', or 'A0' - 'An',\n"
    "    according to the boards's pinout sheet.\n"
    "    Pin IO modes are: Pin.IN, Pin.OUT, Pin.OPEN_DRAIN\n"
    "    Pin pull modes are: Pin.PULL_UP, Pin.PULL_UP_47K, Pin.PULL_UP_22K, Pin.PULL_DOWN, Pin.PULL_HOLD\n"
    "  machine.ADC(pin) -- make an analog object from a pin\n"
    "    methods: read_u16()\n"
    "  machine.UART(id, baudrate=115200) -- create an UART object (id=1 - 8, board-specific)\n"
    "    methods: init(), write(buf), any()\n"
    "             buf=read(n), readinto(buf), buf=readline()\n"
    "    The RX and TX pins are fixed and board-specific.\n"
    "  machine.SoftI2C() -- create a Soft I2C object\n"
    "  machine.I2C(id) -- create a HW I2C object\n"
    "    methods: readfrom(addr, buf, stop=True), writeto(addr, buf, stop=True)\n"
    "             readfrom_mem(addr, memaddr, arg), writeto_mem(addr, memaddr, arg)\n"
    "    SoftI2C allows to use any pin for sda and scl, HW I2C id's and pins are fixed\n"
    "  machine.SoftSPI(baudrate=1000000) -- create a Soft SPI object\n"
    "  machine.SPI(id, baudrate=1000000) -- create a HW SPI object\n"
    "    methods: read(nbytes, write=0x00), write(buf), write_readinto(wr_buf, rd_buf)\n"
    "    SoftSPI allows to use any pin for SPI, HW SPI id's and pins are fixed\n"
    "  machine.Timer(id, freq, callback) -- create a hardware timer object (id=0,1,2)\n"
    "    eg: machine.Timer(freq=1, callback=lambda t:print(t))\n"
    "  machine.RTC() -- create a Real Time Clock object\n"
    "    methods: init(), datetime([dateime_tuple]), now()\n"
    "  machine.PWM(pin, freq, duty_u16[, kw_opts]) -- create a PWM object\n"
    "    methods: init(), duty_u16([value]), duty_ns([value]), freq([value])\n"
    "\n"
    "Useful control commands:\n"
    "  CTRL-C -- interrupt a running program\n"
    "  CTRL-D -- on a blank line, do a soft reset of the board\n"
    "  CTRL-E -- on a blank line, enter paste mode\n"
    "\n"
    "For further help on a specific object, type help(obj)\n"
    "For a list of available modules, type help('modules')\n"
;
