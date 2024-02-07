import displayio
import bitmaptools

src_bmp = displayio.Bitmap(10, 10, 2)
src_bmp.fill(1)

dest_bmp = displayio.Bitmap(10, 10, 2)
dest_bmp.fill(0)

# valid values, non-zero size
bitmaptools.blit(dest_bmp, src_bmp, x=0, y=0, x1=0, y1=0, x2=2, y2=2)
print("blit 1 successful")

# valid values, zero width (no effect)
bitmaptools.blit(dest_bmp, src_bmp, x=5, y=5, x1=1, y1=1, x2=1, y2=6)
print("blit 2 successful")

# valid values, zero height (no effect)
bitmaptools.blit(dest_bmp, src_bmp, x=5, y=5, x1=1, y1=1, x2=2, y2=1)
print("blit 3 successful")

try:
    # invalid values, x too large
    bitmaptools.blit(dest_bmp, src_bmp, x=20, y=5)
    print("blit 4 successful")
except ValueError as e:
    print(f"Error: {e}")

try:
    # invalid values, y too large
    bitmaptools.blit(dest_bmp, src_bmp, x=5, y=25)
    print("blit 5 successful")
except ValueError as e:
    print(f"Error: {e}")

try:
    # invalid values, x2 too large
    bitmaptools.blit(dest_bmp, src_bmp, x=5, y=5, x1=0, y1=0, x2=12, y2=5)
    print("blit 6 successful")
except ValueError as e:
    print(f"Error: {e}")

try:
    # invalid values, x2 too small
    bitmaptools.blit(dest_bmp, src_bmp, x=5, y=5, x1=3, y1=3, x2=0, y2=5)
    print("blit 7 successful")
except ValueError as e:
    print(f"Error: {e}")

try:
    # invalid values, y2 too small
    bitmaptools.blit(dest_bmp, src_bmp, x=5, y=5, x1=3, y1=3, x2=5, y2=0)
    print("blit 8 successful")
except ValueError as e:
    print(f"Error: {e}")

try:
    # invalid values, y2 too large
    bitmaptools.blit(dest_bmp, src_bmp, x=5, y=5, x1=3, y1=3, x2=5, y2=20)
    print("blit 9 successful")
except ValueError as e:
    print(f"Error: {e}")

# import board
# p = displayio.Palette(2)
# p[0] = 0xffffff
# p[1] = 0x0000ff
# dest_tg = displayio.TileGrid(bitmap=dest_bmp, pixel_shader=p)
#
# main_group = displayio.Group(scale=5)
# main_group.append(dest_tg)
#
# board.DISPLAY.root_group = main_group
# while True:
#     pass
