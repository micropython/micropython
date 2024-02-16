from displayio import Bitmap
import bitmapfilter
import ulab
from dump_bitmap import dump_bitmap_rgb_swapped
from blinka_image import decode_resource
from ironbow import ironbow_palette


def test_pattern():
    return decode_resource("testpattern", 2)


def make_quadrant_bitmap():
    b = Bitmap(17, 17, 1)
    for i in range(b.height):
        for j in range(b.width):
            b[i, j] = (i < 8) ^ (j < 8)
    return b


q = make_quadrant_bitmap()
b = test_pattern()
dump_bitmap_rgb_swapped(b)

sepia_weights = [0.393, 0.769, 0.189, 0.349, 0.686, 0.168, 0.272, 0.534, 0.131]

print("ironbow (masked)")
bitmapfilter.false_color(b, ironbow_palette, mask=q)
dump_bitmap_rgb_swapped(b)
