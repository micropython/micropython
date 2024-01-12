from ulab import numpy as np
from displayio import Bitmap
import bitmapfilter
from dump_bitmap import dump_bitmap_rgb_swapped
from blinka_image import decode_resource


def test_pattern():
    return decode_resource("testpattern", 2)


blur = (1, 2, 1, 2, 4, 2, 1, 2, 1)


def blur_i(i):
    result = np.zeros(9 * 9, dtype=np.int16)
    result[i::9] = blur
    print(list(result))
    return result


b = test_pattern()
dump_bitmap_rgb_swapped(b)

for i in range(9):
    b = test_pattern()
    bitmapfilter.morph9(b, weights=blur_i(i))
    # bitmapfilter.morph9(b, weights=blur_i(i), mul=(1/16, 1/16, 1/16))
    dump_bitmap_rgb_swapped(b)
