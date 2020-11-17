#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

#
# This tool converts binary resource files passed on the command line
# into a Python source file containing data from these files, which can
# be accessed using standard pkg_resources.resource_stream() function
# from micropython-lib:
# https://github.com/micropython/micropython-lib/tree/master/pkg_resources
#
import sys

print("R = {")

for fname in sys.argv[1:]:
    with open(fname, "rb") as f:
        b = f.read()
        print("%r: %r," % (fname, b))

print("}")
