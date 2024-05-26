# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
#
# SPDX-License-Identifier: MIT

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
    print("    { MP_ROM_QSTR(MP_QSTR_ID), MP_ROM_PTR(&board_module_id_obj) },")

    for row in csv_reader:
        label = row[0]
        pin = row[1]
        if len(pin) < 4:
            pin = pin[:2] + "0" + pin[2:]
        print("{ MP_ROM_QSTR(MP_QSTR_" + label + "), MP_ROM_PTR(&pin_" + pin + ") },")
        line_count += 1

    print(f"Processed {line_count} lines.")
