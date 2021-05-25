#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import os
import re
import sys

from elftools.elf.elffile import ELFFile

print()

internal_memory = [
    # Name, Start, Length
    ("RTC Fast Memory", (0x3FF9_E000, 0x4007_0000), 8 * 1024),
    ("RTC Slow Memory", (0x5000_0000,), 8 * 1024),
    ("Internal SRAM 0", (0x3FFB_0000, 0x4002_0000), 32 * 1024),
    ("Internal SRAM 1", (0x3FFB_8000, 0x4002_8000), 288 * 1024),
]


def partition_size(arg):
    if "4MB" in arg:
        return 1408 * 1024
    else:
        return 2048 * 1024


def align(n, m):
    return m * ((n + m - 1) // m)


regions = dict((name, 0) for name, _, _ in internal_memory)

# This file is the elf
with open(sys.argv[1], "rb") as stream:
    elffile = ELFFile(stream)
    for section in elffile.iter_sections():
        start = section["sh_addr"]
        size = section["sh_size"]
        offset = section["sh_offset"]
        if not size or not start:
            continue
        for name, starts, length in internal_memory:
            for mem_start in starts:
                mem_end = mem_start + length
                if start >= mem_start and start < mem_end:
                    regions[name] = max(regions.get(name, 0), size)
                    # print("# putting %s in %s (start=0x%x, size=%d)" % (section.name, name, start, size))

# This file is the sdkconfig
with open(sys.argv[2], "r") as f:
    for line in f:
        line = line.strip()
        if line.startswith("CONFIG_PARTITION_TABLE_FILENAME"):
            firmware_region = int(partition_size(line.split("=")[-1]))

# This file is the bin
used_flash = os.stat(sys.argv[3]).st_size

free_flash = firmware_region - used_flash
print(
    "{:7} bytes used, {:7} bytes free in flash firmware space out of {} bytes ({}kB).".format(
        used_flash, free_flash, firmware_region, firmware_region / 1024
    )
)
for name, mem_start, length in internal_memory:
    if name in regions:
        print(
            "{:7} bytes used, {:7} bytes free in '{}' out of {} bytes ({}kB).".format(
                regions[name], length - regions[name], name, length, length / 1024
            )
        )
print()

# Check that we have free flash space. GCC doesn't fail when the text + data
# sections don't fit in FLASH. It only counts data in RAM.
if free_flash < 0:
    print("Too little flash!!!")
    print()
    sys.exit(-1)
