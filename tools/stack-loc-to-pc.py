"""Prints the pcs that access each stack location in a function. Useful for finding
   infrequently used stack space.

   Pipe in disassembly like so:

   arm-none-eabi-objdump --disassemble=mp_execute_bytecode build-metro_m0_express/firmware.elf | python ../../tools/stack-loc-to-pc.py
   """

import sys
import re

offset = re.compile(r"sp, #(\d+)")

offsets = {}
for line in sys.stdin:
    if "sp" in line:
        m = offset.search(line)
        o = int(m.groups()[0])
        pc = line.split(":")[0]
        if o not in offsets:
            offsets[o] = []
        offsets[o].append(pc.strip())

print("Offset", "Size", "PCs", sep="\t")
last_o = 0
for o in sorted(offsets):
    print(o, o - last_o, offsets[o], sep="\t")
    last_o = o
