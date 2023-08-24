#!/usr/bin/env python3
import os
import sys
import pathlib

for f in sys.argv[1:]:
    if not f.endswith(".out"):
        print(f"{f}:0: Not a .out file")
        continue
    p = pathlib.Path(f).stem
    a, _, b = p.partition("_")
    print(a, b)
    p = pathlib.Path(a) / (b + ".exp")
    print(f"{p}: Updating expected result from {f}")
    os.rename(f, p)
