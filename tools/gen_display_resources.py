import argparse

import os
import sys

sys.path.append("bitmap_font")

from adafruit_bitmap_font import bitmap_font

parser = argparse.ArgumentParser(description='Generate USB descriptors.')
parser.add_argument('--font', type=str,
                    help='manufacturer of the device', required=True)
parser.add_argument('--output_c_file', type=argparse.FileType('w'), required=True)

args = parser.parse_args()

args.font

c_file = args.output_c_file

c_file.write("""\

#include "supervisor/shared/display.h"

""")

class BitmapStub:
    def __init__(self, width, height, color_depth):
        self.width = width
        self.rows = [b''] * height

    def _load_row(self, y, row):
        self.rows[y] = bytes(row)

f = bitmap_font.load_font(args.font, BitmapStub)
f.load_glyphs(range(0x20, 0x7f))

print(f.get_bounding_box())
real_bb = [0, 0]

visible_ascii = bytes(range(0x20, 0x7f)).decode("utf-8")
all_characters = visible_ascii
for c in all_characters:
    g = f.get_glyph(ord(c))
    x, y, dx, dy = g["bounds"]
    print(c, g["bounds"], g["shift"])
    if g["shift"][1] != 0:
        raise RuntimeError("y shift")
    real_bb[0] = max(max(real_bb[0], x - dx), g["shift"][0])
    real_bb[1] = max(real_bb[1], y - dy)

real_bb[1] += 1
print(real_bb)

tile_x, tile_y = real_bb

for c in all_characters:
    g = f.get_glyph(ord(c))
    #print(c, g["bounds"], g["shift"])
    for row in g["bitmap"].rows:
        for i in range(g["bounds"][0]):
            byte = i // 8
            bit = i % 8
            # if row[byte] & (1 << (7-bit)) != 0:
            #     print("*",end="")
            # else:
            #     print("_",end="")
        #print()
