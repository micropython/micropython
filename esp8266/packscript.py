#! /usr/bin/env python3

import os
import sys
import struct

if len(sys.argv) < 2:
    print("Usage: ./packscript.py input.py output-0x60000.bin")

MAGIC = bytes([0x65, 0x73, 0x70, 0x38])

size = os.stat(sys.argv[1]).st_size

inp = open(sys.argv[1], "rb")
out = open(sys.argv[2], "wb+")

out.write(MAGIC)
out.write(struct.pack("<I", size))
out.write(inp.read())
