#!/usr/bin/env python3
#
# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

import sys

print()

text = 0
data = 0
bss = 0
# stdin is the linker output.
for line in sys.stdin:
    line = line.strip()
    if not line.startswith("text"):
        text, data, bss = map(int, line.split()[:3])

regions = {}
# This file is the linker script.
with open(sys.argv[1], "r") as f:
    for line in f:
        line = line.strip()
        if line.startswith(("FLASH", "RAM")):
            regions[line.split()[0]] = line.split("=")[-1]

for region in regions:
    space = regions[region]
    if "/*" in space:
        space = space.split("/*")[0]
    regions[region] = eval(space)

free_flash = regions["FLASH"] - text - data
free_ram = regions["RAM"] - data - bss
print(free_flash, "bytes free in flash out of", regions["FLASH"], "bytes (", regions["FLASH"] / 1024, "kb ).")
print(free_ram, "bytes free in ram for stack out of", regions["RAM"], "bytes (", regions["RAM"] / 1024, "kb ).")
print()

# Check that we have free flash space. GCC doesn't fail when the text + data
# sections don't fit in FLASH. It only counts data in RAM.
if free_flash < 0:
    print("Too little flash!!!")
    print()
    sys.exit(-1)
