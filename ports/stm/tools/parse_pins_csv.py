# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

import csv
import sys

# Use: parse_pins_csv.py Filename.csv
# Designed for use with .csv files from Micropython, or in identical format
# created via Datasheet peripheral tables with a Sheets program.
#
# See examples/nucleo_h743.csv for example formatting.

# Open target file
with open(sys.argv[1]) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=",")
    line_count = 0

    print("STATIC const mp_rom_map_elem_t board_module_globals_table[] = {")

    for row in csv_reader:
        label = row[0]
        pin = row[1]
        if len(pin) < 4:
            pin = pin[:2] + "0" + pin[2:]
        print("{ MP_ROM_QSTR(MP_QSTR_" + label + "), MP_ROM_PTR(&pin_" + pin + ") },")
        line_count += 1

    print(f"Processed {line_count} lines.")
