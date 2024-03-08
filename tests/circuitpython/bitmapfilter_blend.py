from displayio import Bitmap
import bitmapfilter
from dump_bitmap import dump_bitmap_rgb_swapped
from blinka_image import decode_resource


def test_pattern():
    return decode_resource("testpattern", 2)


def blinka():
    return decode_resource("blinka_32x32", 0)


def blendfunc(frac):
    nfrac = 1 - frac

    def inner(x, y):
        return x * frac + y * nfrac

    return inner


def make_quadrant_bitmap():
    b = Bitmap(17, 17, 1)
    for i in range(b.height):
        for j in range(b.width):
            b[i, j] = (i < 8) ^ (j < 8)
    return b


b = Bitmap(32, 32, 65535)
print(test_pattern().width)
print(blinka().width)
print(b.width)
print(test_pattern().height)
print(blinka().height)
print(b.height)

mask = make_quadrant_bitmap()
blend_table = bitmapfilter.blend_precompute(blendfunc(0.1))
bitmapfilter.blend(b, test_pattern(), blinka(), blend_table, mask)
dump_bitmap_rgb_swapped(b)

bitmapfilter.blend(b, test_pattern(), blinka(), blendfunc(0.5), mask)
dump_bitmap_rgb_swapped(b)

bitmapfilter.blend(b, test_pattern(), blinka(), max, mask)
dump_bitmap_rgb_swapped(b)
bitmapfilter.blend(b, test_pattern(), blinka(), min)
dump_bitmap_rgb_swapped(b)
