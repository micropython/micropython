#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import csv
import json
import os
import sys

from elftools.elf.elffile import ELFFile


internal_memory = {
    "esp32": [
        # Name, Start, Length
        ("RTC Fast Memory", (0x3FF8_0000, 0x400C_0000), 8 * 1024),
        ("RTC Slow Memory", (0x5000_0000,), 8 * 1024),
        # First 64kB of Internal SRAM 0 can be configured as cached, and starts at 0x4007_0000
        ("Internal SRAM 0", (0x4008_0000,), 128 * 1024),
        ("Internal SRAM 1", (0x3FFE_0000, 0x400A_0000), 128 * 1024),
        ("Internal SRAM 2", (0x3FFA_E000,), 200 * 1024),
    ],
    "esp32s2": [
        # Name, Start, Length
        ("RTC Fast Memory", (0x3FF9_E000, 0x4007_0000), 8 * 1024),
        ("RTC Slow Memory", (0x5000_0000,), 8 * 1024),
        ("Internal SRAM 0", (0x3FFB_0000, 0x4002_0000), 32 * 1024),
        ("Internal SRAM 1", (0x3FFB_8000, 0x4002_8000), 288 * 1024),
    ],
    "esp32s3": [
        # Name, Start, Length
        ("RTC Fast Memory", (0x600F_E000,), 8 * 1024),
        ("RTC Slow Memory", (0x5000_0000,), 8 * 1024),
        ("Internal SRAM 0", (0x4037_0000,), 32 * 1024),
        ("Internal SRAM 1", (0x3FC8_0000, 0x4037_8000), 416 * 1024),
        ("Internal SRAM 2", (0x3FCF_0000,), 64 * 1024),
    ],
    "esp32c3": [
        # Name, Start, Length
        ("RTC Fast Memory", (0x5000_0000,), 8 * 1024),
        ("Internal SRAM 0", (0x4037_C000,), 16 * 1024),
        ("Internal SRAM 1", (0x3FC8_0000, 0x4038_0000), 384 * 1024),
    ],
}


def align(n, m):
    return m * ((n + m - 1) // m)


def find_region(start_address):
    for name, starts, length in internal_memory[target]:
        for mem_start in starts:
            mem_end = mem_start + length
            if mem_start <= start_address < mem_end:
                return (name, mem_start + length)


target = None

# This file is the sdkconfig
with open(sys.argv[2], "r") as f:
    for line in f:
        line = line.strip()
        if line.startswith("CONFIG_IDF_TARGET="):
            target = line.split('"')[1]
        elif line.startswith("CONFIG_PARTITION_TABLE_FILENAME"):
            partitions_file = line.split('"')[1]
            with open(partitions_file, "r") as f:
                ota = None
                app = None
                for partition in csv.reader(f):
                    if partition[0][0] == "#":
                        continue
                    subtype = partition[2].strip()
                    if subtype == "factory":
                        app = partition[4].strip()
                    elif subtype == "ota_0":
                        ota = partition[4].strip()
                size = app if ota is None else ota
                if size[-1] in ("k", "K"):
                    firmware_region = int(size[:-1]) * 1024
                elif size[-1] in ("m", "M"):
                    firmware_region = int(size[:-1]) * 1024 * 1024
                else:
                    raise RuntimeError("Unhandled partition size suffix:", size[-1])

regions = dict((name, 0) for name, _, _ in internal_memory[target])

# This file is the elf
with open(sys.argv[1], "rb") as stream:
    elffile = ELFFile(stream)
    for section in elffile.iter_sections():
        start = section["sh_addr"]
        size = section["sh_size"]
        offset = section["sh_offset"]
        if not size or not start:
            continue
        # This handles sections that span two memory regions, not more than that.
        # print(start, size, offset, section.name)
        region = find_region(start)
        if region is None:
            continue
        name, region_end = region
        region_size = min(size, region_end - start)
        regions[name] += region_size
        # print("# putting %s in %s (start=0x%x, size=%d)" % (section.name, name, start, region_size))
        if region_size < size:
            name, _ = find_region(region_end)
            remaining_size = size - region_size
            regions[name] += remaining_size
            # print("# putting %s in %s (start=0x%x, size=%d)" % (section.name, name, region_end, remaining_size))

# This file is the bin
used_flash = os.stat(sys.argv[3]).st_size
free_flash = firmware_region - used_flash

with open(f"{sys.argv[4]}/firmware.size.json", "w") as f:
    json.dump({"used_flash": used_flash, "firmware_region": firmware_region}, f)

print()
print(
    "{:7} bytes used, {:7} bytes free in flash firmware space out of {} bytes ({}kB).".format(
        used_flash, free_flash, firmware_region, firmware_region / 1024
    )
)
for name, mem_start, length in internal_memory[target]:
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
