import bitmapfilter
import displayio

b = displayio.Bitmap(5, 5, 65535)
b[2, 2] = 0x1F00  #  Blue in RGB565_SWAPPED
b[0, 0] = 0xE007  #  Green in RGB565_SWAPPED
b[0, 4] = 0x00F8  #  Red in RGB565_SWAPPED
weights = (1, 1, 1, 1, 1, 1, 1, 1, 1)


def print_bitmap(bitmap):
    for i in range(bitmap.height):
        for j in range(bitmap.width):
            p = bitmap[j, i]
            p = ((p << 8) & 0xFF00) | (p >> 8)

            r = (p >> 8) & 0xF8
            r |= r >> 5

            g = (p >> 3) & 0xFC
            g |= g >> 6

            b = (p << 3) & 0xF8
            b |= b >> 5
            print(f"{r:02x}{g:02x}{b:02x}", end=" ")
        print()
    print()


print(len(weights))

print_bitmap(b)
bitmapfilter.morph(b, b, weights, m=1 / 9)
print_bitmap(b)
