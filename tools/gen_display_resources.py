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

parser = argparse.ArgumentParser(description="Generate displayio resources.")
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
all_characters = list(visible_ascii)
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

tile_x, tile_y, dx, dy = f.get_bounding_box()
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

#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/Palette.h"
#include "supervisor/shared/display.h"

"""
)


c_file.write(
    """\
#if CIRCUITPY_REPL_LOGO
"""
)
if tile_y == 16:
    blinka_size = 16
    c_file.write(
        """\
const uint32_t blinka_bitmap_data[32] = {
    0x00000011, 0x11000000,
    0x00000111, 0x53100000,
    0x00000111, 0x56110000,
    0x00000111, 0x11140000,
    0x00000111, 0x20002000,
    0x00000011, 0x13000000,
    0x00000001, 0x11200000,
    0x00000000, 0x11330000,
    0x00000000, 0x01122000,
    0x00001111, 0x44133000,
    0x00032323, 0x24112200,
    0x00111114, 0x44113300,
    0x00323232, 0x34112200,
    0x11111144, 0x44443300,
    0x11111111, 0x11144401,
    0x23232323, 0x21111110
};
"""
    )
else:
    blinka_size = 12
    c_file.write(
        """\
const uint32_t blinka_bitmap_data[28] = {
    0x00000111, 0x00000000,
    0x00001153, 0x10000000,
    0x00001156, 0x11000000,
    0x00001111, 0x14000000,
    0x00000112, 0x00200000,
    0x00000011, 0x30000000,
    0x00000011, 0x20000000,
    0x00011144, 0x13000000,
    0x00232324, 0x12000000,
    0x01111444, 0x13000000,
    0x32323234, 0x12010000,
    0x11111144, 0x44100000
};
"""
    )

c_file.write(
    """\
const displayio_bitmap_t blinka_bitmap = {{
    .base = {{.type = &displayio_bitmap_type }},
    .width = {0},
    .height = {0},
    .data = (uint32_t*) blinka_bitmap_data,
    .stride = 2,
    .bits_per_value = 4,
    .x_shift = 3,
    .x_mask = 0x7,
    .bitmask = 0xf,
    .read_only = true
}};

_displayio_color_t blinka_colors[7] = {{
    {{
        .rgb888 = 0x000000,
        .transparent = true
    }},
    {{ // Purple
        .rgb888 = 0x8428bc
    }},
    {{ // Pink
        .rgb888 = 0xff89bc
    }},
    {{ // Light blue
        .rgb888 = 0x7beffe
    }},
    {{ // Dark purple
        .rgb888 = 0x51395f
    }},
    {{ // White
        .rgb888 = 0xffffff
    }},
    {{ // Dark Blue
        .rgb888 = 0x0736a0
    }},
}};

displayio_palette_t blinka_palette = {{
    .base = {{.type = &displayio_palette_type }},
    .colors = blinka_colors,
    .color_count = 7,
    .needs_refresh = false
}};

displayio_tilegrid_t supervisor_blinka_sprite = {{
    .base = {{.type = &displayio_tilegrid_type }},
    .bitmap = (displayio_bitmap_t*) &blinka_bitmap,
    .pixel_shader = &blinka_palette,
    .x = 0,
    .y = 0,
    .pixel_width = {0},
    .pixel_height = {0},
    .bitmap_width_in_tiles = 1,
    .width_in_tiles = 1,
    .height_in_tiles = 1,
    .tile_width = {0},
    .tile_height = {0},
    .top_left_x = {0},
    .top_left_y = {0},
    .tiles = 0,
    .partial_change = false,
    .full_change = false,
    .hidden = false,
    .hidden_by_parent = false,
    .moved = false,
    .inline_tiles = true,
    .in_group = true
}};
#endif
""".format(
        blinka_size
    )
)

c_file.write(
    """\
#if CIRCUITPY_TERMINALIO
_displayio_color_t terminal_colors[2] = {
    {
        .rgb888 = 0x000000
    },
    {
        .rgb888 = 0xffffff
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
displayio_tilegrid_t supervisor_terminal_scroll_area_text_grid = {{
    .base = {{ .type = &displayio_tilegrid_type }},
    .bitmap = (displayio_bitmap_t*) &supervisor_terminal_font_bitmap,
    .pixel_shader = &supervisor_terminal_color,
    .x = 0,
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
displayio_tilegrid_t supervisor_terminal_status_bar_text_grid = {{
    .base = {{ .type = &displayio_tilegrid_type }},
    .bitmap = (displayio_bitmap_t*) &supervisor_terminal_font_bitmap,
    .pixel_shader = &supervisor_terminal_color,
    .x = 0,
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
    .data = (uint32_t*) font_bitmap_data,
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
    .scroll_area = NULL,
    .status_bar = NULL
};

#endif
"""
)
