from math import sin, cos, pi
from displayio import Bitmap
import bitmapfilter
import ulab
from dump_bitmap import dump_bitmap_rgb_swapped
from blinka_image import decode_resource


def func1(x):
    return sin(x * pi)


def func2(x):
    return cos(x * pi / 2)


def test_pattern():
    return decode_resource("testpattern", 2)


b = test_pattern()
dump_bitmap_rgb_swapped(b)

print("lookup3")
bitmapfilter.lookup(b, (func1, func2, func2))
dump_bitmap_rgb_swapped(b)
