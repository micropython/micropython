#!/usr/bin/env python
import os.path
import struct
import sys

infile=sys.argv[1]
outfile=sys.argv[2]
with open(outfile, "wb") as fout:
    x = struct.pack('<H', os.path.getsize(infile) // 512 + 1)
    y = [0xda, 0xff, x[0], x[1], 0xff, 0xff, 0xff, 0xff]
    fout.write(bytearray(y))

    with open(infile, "rb") as fin:
        z = fin.read()
        fout.write(z)
