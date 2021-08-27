# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import sys

output_filename = sys.argv[1]
input_filenames = {}
i = 2
while i < len(sys.argv):
    offset = int(sys.argv[i], 16)
    filename = sys.argv[i + 1]
    input_filenames[offset] = filename
    i += 2

with open(output_filename, "wb") as output_file:
    offsets = sorted(input_filenames.keys())
    for offset in offsets:
        input_filename = input_filenames[offset]
        # Fill with ones to save NOR flash.
        while output_file.tell() < offset:
            output_file.write(b"\xff")
        with open(input_filename, "rb") as input_file:
            output_file.write(input_file.read())
