#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import sys

if len(sys.argv) != 2:
    print(
        """\
Usage: print_status.py STATUS_FILENAME
    STATUS_FILENAME contains one line with an integer status."""
    )
    sys.exit(1)
with open(sys.argv[1], "r") as status_in:
    status = int(status_in.readline())

print(
    "{} with status {}".format(
        "\033[32msucceeded\033[0m" if status == 0 else "\033[31mfailed\033[0m", status
    )
)
