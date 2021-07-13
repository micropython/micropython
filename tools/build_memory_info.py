#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import re
import sys

# Handle size constants with K or M suffixes (allowed in .ld but not in Python).
K_PATTERN = re.compile(r"([0-9]+)[kK]")
K_REPLACE = r"(\1*1024)"

M_PATTERN = re.compile(r"([0-9]+)[mM]")
M_REPLACE = r"(\1*1024*1024)"

print()

text = 0
data = 0
bss = 0
# stdin is the linker output.
for line in sys.stdin:
    # Uncomment to see linker output.
    # print(line)
    line = line.strip()
    if not line.startswith("text"):
        text, data, bss = map(int, line.split()[:3])

regions = {}
# This file is the linker script.
with open(sys.argv[1], "r") as f:
    for line in f:
        line = line.strip()
        if line.startswith(("FLASH_FIRMWARE", "RAM")):
            regions[line.split()[0]] = line.split("=")[-1]

for region in regions:
    space = regions[region]
    if "/*" in space:
        space = space.split("/*")[0]
    space = K_PATTERN.sub(K_REPLACE, space)
    space = M_PATTERN.sub(M_REPLACE, space)
    regions[region] = int(eval(space))

firmware_region = regions["FLASH_FIRMWARE"]
ram_region = regions["RAM"]

used_flash = data + text
free_flash = firmware_region - used_flash
used_ram = data + bss
free_ram = ram_region - used_ram
print(
    "{} bytes used, {} bytes free in flash firmware space out of {} bytes ({}kB).".format(
        used_flash, free_flash, firmware_region, firmware_region / 1024
    )
)
print(
    "{} bytes used, {} bytes free in ram for stack and heap out of {} bytes ({}kB).".format(
        used_ram, free_ram, ram_region, ram_region / 1024
    )
)
print()

# Check that we have free flash space. GCC doesn't fail when the text + data
# sections don't fit in FLASH. It only counts data in RAM.
if free_flash < 0:
    print("Too little flash!!!")
    print()
    sys.exit(-1)
