from displayio import Bitmap
import bitmapfilter
import ulab
from dump_bitmap import dump_bitmap_rgb_swapped
from blinka_image import decode_resource


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

sepia_weights = bitmapfilter.ChannelMixer(
    0.393, 0.769, 0.189, 0.349, 0.686, 0.168, 0.272, 0.534, 0.131
)

print("sepia")
bitmapfilter.mix(b, sepia_weights)
dump_bitmap_rgb_swapped(b)

# Red channel only
print("red channel only (note: masked)")
b = test_pattern()
bitmapfilter.mix(b, bitmapfilter.ChannelScale(1, 0, 0), mask=q)
dump_bitmap_rgb_swapped(b)

# Scale green channel
print("scale green channel (note: masked)")
b = test_pattern()
bitmapfilter.mix(b, bitmapfilter.ChannelScale(1, 2, 0), mask=q)
dump_bitmap_rgb_swapped(b)

# Swap R & G channels
print("swap R&G")
b = test_pattern()
bitmapfilter.mix(b, bitmapfilter.ChannelMixerOffset(0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0))
dump_bitmap_rgb_swapped(b)

# invert B
print("invert B")
b = test_pattern()
bitmapfilter.mix(b, bitmapfilter.ChannelScaleOffset(1, 0, 1, 0, -1, 1))
dump_bitmap_rgb_swapped(b)
