# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

# This helper generates the pinout tables in ../README.rst.

import os
import os.path

pins = [
    "PA00",
    "PA01",
    "PA02",
    "PA03",
    "PB08",
    "PB09",
    "PA04",
    "PA05",
    "PA06",
    "PA07",
    "PA08",
    "PA09",
    "PA10",
    "PA11",
    "PB10",
    "PB11",
    "PA12",
    "PA13",
    "PA14",
    "PA15",
    "PA16",
    "PA17",
    "PA18",
    "PA19",
    "PA20",
    "PA21",
    "PA22",
    "PA23",
    "PA24",
    "PA25",
    "PB22",
    "PB23",
    "PA27",
    "PA28",
    "PA29",
    "PA30",
    "PA31",
    "PB02",
    "PB03",
]

# Dictionary keys: [board][pin] = list of pin names
mapping = {}

QSTR = "    { MP_OBJ_NEW_QSTR(MP_QSTR_"

for board in os.listdir("boards"):
    if not os.path.isdir("boards/" + board):
        continue
    mapping[board] = {}
    with open("boards/" + board + "/pins.c", "r") as f:
        for line in f:
            if line.startswith(QSTR):
                board_name, _, pin = line.split(")")
                board_name = board_name[len(QSTR) :]
                pin = pin[-8:-4]
                if pin not in mapping[board]:
                    mapping[board][pin] = []
                mapping[board][pin].append(board_name)

column_width = {}
for board in mapping:
    column_width[board] = len(board)
    for pin in mapping[board]:
        l = len(" / ".join("``" + x + "``" for x in mapping[board][pin]))
        column_width[board] = max(l, column_width[board])

first_column_width = len("`microcontroller.pin`")
print("=" * first_column_width, end="")
total_board_width = -2
for board in column_width:
    column = "  " + "=" * column_width[board]
    total_board_width += len(column)
    print(column, end="")

print()
print("`microcontroller.pin`  `board`")
print("-" * first_column_width + "  " + "-" * total_board_width)

print("Datasheet".ljust(first_column_width), end="")
for board in column_width:
    print("  " + board.ljust(column_width[board]), end="")
print()

print("=" * first_column_width, end="")
for board in column_width:
    column = "  " + "=" * column_width[board]
    print(column, end="")
print()

for pin in pins:
    print(pin.ljust(first_column_width), end="")
    for board in column_width:
        if pin in mapping[board]:
            names = " / ".join("``" + x + "``" for x in mapping[board][pin])
            print("  " + names.ljust(column_width[board]), end="")
        else:
            print(" " * (column_width[board] + 2), end="")
    print()

print("=" * first_column_width, end="")
for board in column_width:
    column = "  " + "=" * column_width[board]
    print(column, end="")
print()

print()
print()
# Generate pin capabilities too.

ALL_BUT_USB = list(pins)
ALL_BUT_USB.remove("PA24")
ALL_BUT_USB.remove("PA25")

# dictionary is [module][class] = [pins]
capabilities = {
    "analogio": {
        "AnalogIn": [
            "PA02",
            "PA03",
            "PB08",
            "PB09",
            "PA04",
            "PA05",
            "PA06",
            "PA07",
            "PA08",
            "PA09",
            "PA10",
            "PA11",
            "PB02",
            "PB03",
        ],
        "AnalogOut": ["PA02"],
    },
    "audioio": {"AudioOut": ["PA02"]},
    "bitbangio": {"I2C": ALL_BUT_USB, "OneWire": ALL_BUT_USB, "SPI": ALL_BUT_USB},
    "busio": {
        "I2C - SDA": ["PA00", "PB08", "PA08", "PA12", "PA16", "PA22", "PB02"],  # SERCOM pad 0
        "I2C - SCL": ["PA01", "PB09", "PA09", "PA13", "PA17", "PA23", "PB03"],  # SERCOM pad 1
        "OneWire": ALL_BUT_USB,
        "SPI - MISO": [
            "PA00",
            "PA01",
            "PB08",
            "PB09",
            "PA04",
            "PA05",
            "PA06",
            "PA07",
            "PA08",
            "PA09",
            "PA10",
            "PA11",
            "PB10",
            "PB11",
            "PA12",
            "PA13",
            "PA14",
            "PA15",
            "PA16",
            "PA17",
            "PA18",
            "PA19",
            "PA20",
            "PA21",
            "PA22",
            "PA23",
            "PB22",
            "PB23",
            "PA30",
            "PA31",
            "PB02",
            "PB03",
        ],  # any SERCOM pad
        "SPI - MOSI": [
            "PA00",
            "PB08",
            "PA04",
            "PA06",
            "PA08",
            "PA10",
            "PA11",
            "PB10",
            "PB11",
            "PA14",
            "PA15",
            "PA16",
            "PA18",
            "PA19",
            "PA20",
            "PA21",
            "PA22",
            "PB22",
            "PB23",
            "PA30",
            "PA31",
            "PB02",
        ],  # any pad but 1
        "SPI - SCK": [
            "PA01",
            "PB09",
            "PA05",
            "PA07",
            "PA09",
            "PA11",
            "PB11",
            "PA13",
            "PA15",
            "PA17",
            "PA19",
            "PA21",
            "PA23",
            "PB23",
            "PA31",
            "PB03",
        ],  # 1 or 3
        "UART - RX": [
            "PA00",
            "PA01",
            "PB08",
            "PB09",
            "PA04",
            "PA05",
            "PA06",
            "PA07",
            "PA08",
            "PA09",
            "PA10",
            "PA11",
            "PB10",
            "PB11",
            "PA12",
            "PA13",
            "PA14",
            "PA15",
            "PA16",
            "PA17",
            "PA18",
            "PA19",
            "PA20",
            "PA21",
            "PA22",
            "PA23",
            "PB22",
            "PB23",
            "PA30",
            "PA31",
            "PB02",
            "PB03",
        ],  # any pad
        "UART - TX": [
            "PA00",
            "PB08",
            "PA04",
            "PA06",
            "PA08",
            "PA10",
            "PB10",
            "PA12",
            "PA14",
            "PA16",
            "PA18",
            "PA20",
            "PA22",
            "PB22",
            "PA30",
            "PB02",
        ],  # pad 0 or 2
    },
    "digitalio": {"DigitalInOut": ALL_BUT_USB},
    "pulseio": {
        "PulseIn": ALL_BUT_USB,
        "PWMOut": [
            "PA01",
            "PB09",
            "PA04",
            "PA05",
            "PA06",
            "PA07",
            "PA08",
            "PA09",
            "PA10",
            "PA11",
            "PB10",
            "PB11",
            "PA12",
            "PA13",
            "PA14",
            "PA15",
            "PA16",
            "PA17",
            "PA18",
            "PA19",
            "PA20",
            "PA21",
            "PA22",
            "PA23",
            "PA30",
            "PA31",
        ],
    },
    "ps2io": {"Ps2": ALL_BUT_USB},
    "touchio": {
        "TouchIn": ["PA02", "PA03", "PB08", "PB09", "PA04", "PA05", "PA06", "PA07", "PB02", "PB03"]
    },
}

column_width = {}
for module in capabilities:
    for c in capabilities[module]:
        column_width[module + c] = max(len("**Yes**"), len(c))

module_width = {}
for module in capabilities:
    module_width[module] = 0
    for c in capabilities[module]:
        module_width[module] += column_width[module + c] + 2
    module_width[module] -= 2

    if module_width[module] < (len(module) + 2):
        column_width[module + c] += len(module) + 2 - module_width[module]
        module_width[module] = len(module) + 2

first_column_width = len("`microcontroller.pin`")
print("=" * first_column_width, end="")
for module in capabilities:
    for c in capabilities[module]:
        print("  " + "=" * column_width[module + c], end="")
print()

print("`microcontroller.pin`", end="")
for module in capabilities:
    print("  " + ("`" + module + "`").ljust(module_width[module]), end="")
print()

print("-" * first_column_width, end="")
for module in capabilities:
    print("  " + "-" * module_width[module], end="")
print()

print("Datasheet".ljust(first_column_width), end="")
for module in capabilities:
    for c in capabilities[module]:
        print("  " + c.ljust(column_width[module + c]), end="")
print()

print("=" * first_column_width, end="")
for module in capabilities:
    for c in capabilities[module]:
        print("  " + "=" * column_width[module + c], end="")
print()

for pin in pins:
    print(pin.ljust(first_column_width), end="")
    for module in capabilities:
        for c in capabilities[module]:
            if pin in capabilities[module][c]:
                print("  " + "**Yes**".ljust(column_width[module + c]), end="")
            else:
                print(" " * (column_width[module + c] + 2), end="")
    print()

print("=" * first_column_width, end="")
for module in capabilities:
    for c in capabilities[module]:
        print("  " + "=" * column_width[module + c], end="")
print()
