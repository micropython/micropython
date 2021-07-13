# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import argparse

import os
import struct
import sys

sys.path.insert(0, "bitmap_font")
sys.path.insert(0, "../../tools/bitmap_font")

from adafruit_bitmap_font import bitmap_font

parser = argparse.ArgumentParser(description="Generate USB descriptors.")
parser.add_argument("--font", type=str, help="Font path", required=True)
parser.add_argument("--extra_characters", type=str, help="Unicode string of extra characters")
parser.add_argument(
    "--sample_file",
    type=argparse.FileType("r", encoding="utf-8"),
    help="Text file that includes strings to support.",
)
parser.add_argument("--output_c_file", type=argparse.FileType("w"), required=True)

args = parser.parse_args()


class BitmapStub:
    def __init__(self, width, height, color_depth):
        self.width = width
        self.rows = [b""] * height

    def _load_row(self, y, row):
        self.rows[y] = bytes(row)


f = bitmap_font.load_font(args.font, BitmapStub)

# Load extra characters from the sample file.
sample_characters = set()
if args.sample_file:
    for line in args.sample_file:
        # Skip comments because we add additional characters in our huffman comments.
        if line.startswith("//"):
            continue
        for c in line.strip():
            sample_characters.add(c)

# Merge visible ascii, sample characters and extra characters.
visible_ascii = bytes(range(0x20, 0x7F)).decode("utf-8")
all_characters = visible_ascii
for c in sample_characters:
    if c not in all_characters:
        all_characters += c
if args.extra_characters:
    all_characters.extend(args.extra_characters)
all_characters = "".join(sorted(set(all_characters)))
filtered_characters = all_characters

# Try to pre-load all of the glyphs. Misses will still be slow later.
f.load_glyphs(set(ord(c) for c in all_characters))

missing = 0
# Get each glyph.
for c in set(all_characters):
    if ord(c) not in f._glyphs:
        missing += 1
        filtered_characters = filtered_characters.replace(c, "")
        continue
    g = f.get_glyph(ord(c))
    if g["shift"][1] != 0:
        raise RuntimeError("y shift")

if missing > 0:
    print("Font missing", missing, "characters", file=sys.stderr)

x, y, dx, dy = f.get_bounding_box()
tile_x, tile_y = x - dx, y - dy
total_bits = tile_x * len(all_characters)
total_bits += 32 - total_bits % 32
bytes_per_row = total_bits // 8
b = bytearray(bytes_per_row * tile_y)

for x, c in enumerate(filtered_characters):
    g = f.get_glyph(ord(c))
    start_bit = x * tile_x + g["bounds"][2]
    start_y = (tile_y - 2) - (g["bounds"][1] + g["bounds"][3])
    for y, row in enumerate(g["bitmap"].rows):
        for i in range(g["bounds"][0]):
            byte = i // 8
            bit = i % 8
            if row[byte] & (1 << (7 - bit)) != 0:
                overall_bit = start_bit + (start_y + y) * bytes_per_row * 8 + i
                b[overall_bit // 8] |= 1 << (7 - (overall_bit % 8))


extra_characters = ""
for c in filtered_characters:
    if c not in visible_ascii:
        extra_characters += c

c_file = args.output_c_file

c_file.write(
    """\

#include "shared-bindings/displayio/Palette.h"
#include "supervisor/shared/display.h"

"""
)

c_file.write(
    """\
_displayio_color_t terminal_colors[2] = {
    {
        .rgb888 = 0x000000,
        .rgb565 = 0x0000,
        .luma = 0x00,
        .chroma = 0
    },
    {
        .rgb888 = 0xffffff,
        .rgb565 = 0xffff,
        .luma = 0xff,
        .chroma = 0
    },
};

displayio_palette_t supervisor_terminal_color = {
    .base = {.type = &displayio_palette_type },
    .colors = terminal_colors,
    .color_count = 2,
    .needs_refresh = false
};
"""
)

c_file.write(
    """\
displayio_tilegrid_t supervisor_terminal_text_grid = {{
    .base = {{ .type = &displayio_tilegrid_type }},
    .bitmap = (displayio_bitmap_t*) &supervisor_terminal_font_bitmap,
    .pixel_shader = &supervisor_terminal_color,
    .x = 16,
    .y = 0,
    .pixel_width = {1},
    .pixel_height = {2},
    .bitmap_width_in_tiles = {0},
    .tiles_in_bitmap = {0},
    .width_in_tiles = 1,
    .height_in_tiles = 1,
    .tile_width = {1},
    .tile_height = {2},
    .tiles = NULL,
    .partial_change = false,
    .full_change = false,
    .hidden = false,
    .hidden_by_parent = false,
    .moved = false,
    .inline_tiles = false,
    .in_group = true
}};
""".format(
        len(all_characters), tile_x, tile_y
    )
)

c_file.write(
    """\
const uint32_t font_bitmap_data[{}] = {{
""".format(
        bytes_per_row * tile_y // 4
    )
)

for i, word in enumerate(struct.iter_unpack(">I", b)):
    c_file.write("0x{:08x}, ".format(word[0]))
    if (i + 1) % (bytes_per_row // 4) == 0:
        c_file.write("\n")

c_file.write(
    """\
};
"""
)

c_file.write(
    """\
displayio_bitmap_t supervisor_terminal_font_bitmap = {{
    .base = {{.type = &displayio_bitmap_type }},
    .width = {},
    .height = {},
    .data = (size_t*) font_bitmap_data,
    .stride = {},
    .bits_per_value = 1,
    .x_shift = 5,
    .x_mask = 0x1f,
    .bitmask = 0x1,
    .read_only = true
}};
""".format(
        len(all_characters) * tile_x, tile_y, bytes_per_row / 4
    )
)


c_file.write(
    """\
const fontio_builtinfont_t supervisor_terminal_font = {{
    .base = {{.type = &fontio_builtinfont_type }},
    .bitmap = &supervisor_terminal_font_bitmap,
    .width = {},
    .height = {},
    .unicode_characters = (const uint8_t*) "{}",
    .unicode_characters_len = {}
}};
""".format(
        tile_x, tile_y, extra_characters, len(extra_characters.encode("utf-8"))
    )
)

c_file.write(
    """\
terminalio_terminal_obj_t supervisor_terminal = {
    .base = { .type = &terminalio_terminal_type },
    .font = &supervisor_terminal_font,
    .cursor_x = 0,
    .cursor_y = 0,
    .tilegrid = &supervisor_terminal_text_grid
};
"""
)
