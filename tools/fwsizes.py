#!/usr/bin/env python3
# Run this from within an unzipped directory of build logs from github
# to get a CSV file full of firmware sizes.  Super useful to compare sizes
# from various builds.
# Taken from https://github.com/adafruit/circuitpython/pull/4564#issuecomment-816655101
import os, re

for fn in os.listdir():
    if os.path.isfile(fn) and ("build-arm " in fn or "build-riscv " in fn):
        board = re.split("[()]", fn)[1]
        if board in (
            "spresense",
            "teensy40",
            "teensy41",
            "feather_m7_1011",
            "feather_mimxrt1011",
            "feather_mimxrt1062",
            "imxrt1010_evk",
            "imxrt1020_evk",
            "imxrt1060_evk",
            "metro_m7_1011",
        ):
            continue
        with open(fn, "r") as f:
            head = "Build " + board + " for "
            lines = iter(f)
            for line in lines:
                if head in line:
                    tr = line.split(head)[1].split()[0]
                    assert "make: Entering directory" in next(lines)
                    assert "Use make V=1, make V=2" in next(lines)
                    while re.search(
                        r"\{\}|QSTR updated|FREEZE|\{'sku':|hex\tfilename|boot2.elf|Including User C Module from|Font missing|section `.bss' type changed to PROGBITS",
                        next(lines),
                    ):
                        pass
                    free = next(lines).split("bytes used, ")[1].split()[0]
                    print(board + "," + tr + "," + free)
