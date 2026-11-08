# Unrotated rotozoom with exact scales takes a fixed-point path. Its output must match
# the generic float loop, which the expected output was recorded from.
import bitmaptools
import displayio


def checksum(bitmap):
    total = 0
    for y in range(bitmap.height):
        for x in range(bitmap.width):
            total = (total * 31 + bitmap[x, y] + 1) % 1000000007
    return total


def source_bitmap(width, height, colors):
    bitmap = displayio.Bitmap(width, height, colors)
    for y in range(height):
        for x in range(width):
            bitmap[x, y] = (x * 7 + y * 13 + 1) % colors
    return bitmap


cases = [
    # name, source size, source colors, dest colors, keyword arguments
    ("scale 1", (12, 14), 256, 256, {}),
    ("scale 2", (12, 14), 256, 256, {"scale": 2.0}),
    ("scale 4 offset", (12, 14), 256, 256, {"scale": 4.0, "ox": 21, "oy": 17}),
    ("scale 0.5", (12, 14), 256, 256, {"scale": 0.5}),
    ("scale 0.5 offset", (12, 14), 256, 256, {"scale": 0.5, "ox": 7, "oy": 9}),
    ("negative offset", (12, 14), 256, 256, {"scale": 2.0, "ox": -5, "oy": -3}),
    ("odd width", (13, 9), 256, 256, {"scale": 1.0, "ox": 5, "oy": 4}),
    ("pivot", (12, 14), 256, 256, {"scale": 2.0, "px": 3, "py": 5}),
    (
        "source clip",
        (12, 14),
        256,
        256,
        {"scale": 2.0, "source_clip0": (2, 3), "source_clip1": (9, 11)},
    ),
    (
        "source clip edges",
        (12, 14),
        256,
        256,
        {"scale": 2.0, "source_clip0": (5, 6), "source_clip1": (12, 14)},
    ),
    (
        "dest clip",
        (12, 14),
        256,
        256,
        {"scale": 2.0, "dest_clip0": (5, 4), "dest_clip1": (40, 30)},
    ),
    (
        "nothing sampled",
        (12, 14),
        256,
        256,
        {"scale": 2.0, "source_clip0": (0, 0), "source_clip1": (12, 14), "ox": 300},
    ),
    ("skip index", (12, 14), 256, 256, {"scale": 2.0, "skip_index": 1}),
    ("16 bit", (12, 14), 65536, 65536, {"scale": 2.0}),
    ("16 bit odd width", (13, 9), 65536, 65536, {"scale": 1.0, "ox": 5, "oy": 4}),
    ("16 bit skip", (12, 14), 65536, 65536, {"scale": 2.0, "skip_index": 1}),
    ("4 bit", (12, 14), 16, 16, {"scale": 2.0}),
    ("dest deeper", (12, 14), 256, 65536, {"scale": 2.0}),
    ("not exact scale", (12, 14), 256, 256, {"scale": 1.3}),
    ("rotated", (12, 14), 256, 256, {"scale": 2.0, "angle": 0.5}),
]

for name, size, source_colors, dest_colors, kwargs in cases:
    source = source_bitmap(size[0], size[1], source_colors)
    dest = displayio.Bitmap(48, 40, dest_colors)
    dest.fill(dest_colors - 1)
    bitmaptools.rotozoom(dest, source, **kwargs)
    print(name, checksum(dest))
