# Test FrameBuffer.blit method.

import sys

try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit

if not framebuf.ALPHA:
    print("SKIP")
    raise SystemExit


def printbuf(bpp=1):
    print("--8<--")
    for y in range(h):
        for x in range(w * bpp):
            print("%02x" % buf[(x + y * w * bpp)], end="")
        print()
    print("-->8--")


w = 5
h = 4
buf = bytearray(w * h)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS8)

fbuf2 = framebuf.FrameBuffer(bytearray(4), 2, 2, framebuf.GS8)
fbuf2.fill(0x7F)

# set pixel at various locations with alpha.
for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
    fbuf.fill(0)
    fbuf.pixel(x, y, 0x7F, 0x7F)
    printbuf()

# check that alpha 0x01 gives *something* for color 0x80 or more
fbuf.fill(0)
fbuf.pixel(0, 0, 0x80, 0x01)
printbuf()

# rect at various locations with alpha.
for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
    fbuf.fill(0)
    fbuf.fill_rect(x, y, 2, 2, 0x7F, 0x7F)
    printbuf()

# hline at various locations with alpha.
for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
    fbuf.fill(0)
    fbuf.hline(x, y, 2, 0x7F, 0x7F)
    printbuf()

# vline at various locations with alpha.
for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
    fbuf.fill(0)
    fbuf.vline(x, y, 2, 0x7F, 0x7F)
    printbuf()

# unfilled rect at various locations with alpha.
for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
    fbuf.fill(0)
    fbuf.rect(x, y, 3, 3, 0x7F, False, 0x7F)
    printbuf()

# steep antialiased line.
fbuf.fill(0)
fbuf.line(1, 1, 2, 3, 0xFF)
printbuf()

# shallow antialiased line.
fbuf.fill(0)
fbuf.line(1, 1, 3, 2, 0xFF)
printbuf()

# steep antialiased line with alpha.
fbuf.fill(0)
fbuf.line(1, 1, 2, 3, 0xFF, 0x7F)
printbuf()

# shallow antialiased line with alpha.
fbuf.fill(0)
fbuf.line(1, 1, 3, 2, 0xFF, 0x7F)
printbuf()

# Blit another FrameBuffer, at various locations with alpha.
for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
    fbuf.fill(0)
    fbuf.blit(fbuf2, x, y, -1, None, 0x7F)
    printbuf()

# Blit another FrameBuffer with 0 alpha (test no-op branch).
fbuf.fill(0)
fbuf.blit(fbuf2, 1, 1, -1, None, 0x00)
printbuf()

# Blit another FrameBuffer, with alpha mask.
alphas = [[0, 0x3F], [0x7F, 0xFF]]
for bpp, format in [
    (8, framebuf.GS8),
    (4, framebuf.GS4_HMSB),
    (2, framebuf.GS2_HMSB),
    (1, framebuf.MONO_HLSB),
]:
    mask = framebuf.FrameBuffer(bytearray(4), 2, 2, format)
    for x in [0, 1]:
        for y in [0, 1]:
            mask.pixel(x, y, alphas[x][y] >> (8 - bpp))

    fbuf.fill(0)
    fbuf.blit(fbuf2, 1, 1, -1, None, mask)
    printbuf()

# Blit another FrameBuffer, with alpha mask, non-black background.
alphas = [[0, 0x3F], [0x7F, 0xFF]]
for bpp, format in [
    (8, framebuf.GS8),
    (4, framebuf.GS4_HMSB),
    (2, framebuf.GS2_HMSB),
    (1, framebuf.MONO_HLSB),
]:
    mask = framebuf.FrameBuffer(bytearray(4), 2, 2, format)
    for x in [0, 1]:
        for y in [0, 1]:
            mask.pixel(x, y, alphas[x][y] >> (8 - bpp))

    fbuf.fill(0xEF)
    fbuf.blit(fbuf2, 1, 1, -1, None, mask)
    printbuf()

# Try to blit with wrong-shaped mask (ValueError).
alphas = [[0, 0x3F], [0x7F, 0xFF]]
mask = framebuf.FrameBuffer(bytearray(6), 3, 2, framebuf.GS8)
for x in [0, 1]:
    for y in [0, 1]:
        mask.pixel(x, y, alphas[x][y] >> (8 - bpp))
fbuf.fill(0x00)
try:
    fbuf.blit(fbuf2, 1, 1, -1, None, mask)
except ValueError as exc:
    print(exc)
except Exception as exc:
    print("Unexpected error:", exc)
else:
    print("No Error")

# Try to blit with color mask (ValueError).
mask = framebuf.FrameBuffer(bytearray(8), 2, 2, framebuf.RGB565)
fbuf.fill(0x00)
try:
    fbuf.blit(fbuf2, 1, 1, -1, None, mask)
except ValueError as exc:
    print(exc)
except Exception as exc:
    print("Unexpected error:", exc)
else:
    print("No Error")

# text at various locations with alpha.
for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
    fbuf.fill(0)
    fbuf.text("x", x, y, 0x7F, 0x7F)
    printbuf()

# drawing with alpha 0 does nothing
fbuf.fill(0)

fbuf.pixel(1, 1, 0x7F, 0)
fbuf.hline(1, 1, 2, 0x7F, 0)
fbuf.vline(1, 1, 2, 0x7F, 0)
fbuf.line(1, 1, 2, 2, 0x7F, 0)
fbuf.rect(1, 1, 2, 2, 0x7F, True, 0)
fbuf.rect(1, 1, 2, 2, 0x7F, False, 0)
fbuf.ellipse(1, 1, 2, 2, 0x7F, True, 0xF, 0)
fbuf.text("x", 1, 1, 0x7F, 0)

printbuf()

# Ellipse
w = 30
h = 30
buf = bytearray(w * h)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS8)

# Outline
fbuf.fill(0)
fbuf.ellipse(15, 15, 12, 6, 0x7F, False, 0b1111, 0x7F)
printbuf()

# Fill
fbuf.fill(0)
fbuf.ellipse(15, 15, 6, 12, 0xAA, True, 0b1111, 0x7F)
printbuf()

# Circle which might double-draw
# Outline
fbuf.fill(0)
fbuf.ellipse(15, 15, 6, 6, 0x7F, False, 0b1111, 0x7F)
printbuf()

# Fill
fbuf.fill(0)
fbuf.ellipse(15, 15, 6, 6, 0xAA, True, 0b1111, 0x7F)
printbuf()

# Now in color
for format in [framebuf.RGB565_LE, framebuf.RGB565_BE]:
    w = 5
    h = 4
    buf = bytearray(2 * w * h)
    fbuf = framebuf.FrameBuffer(buf, w, h, format)

    # set pixel at various locations with alpha.
    fbuf.fill(0)
    for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
        fbuf.pixel(x, y, 0b1111101111100000, 0x7F)
    printbuf(2)

    fbuf2 = framebuf.FrameBuffer(bytearray(8), 2, 2, format)
    fbuf2.fill(0b1111101111100000)

    # Blit a color FrameBuffer, at various locations with alpha.
    for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
        fbuf.fill(0)
        fbuf.blit(fbuf2, x, y, -1, None, 0x7F)
        printbuf(2)

    # Blit a color FrameBuffer, with alpha mask.
    alphas = [[0, 0x3F], [0x7F, 0xFF]]
    for bpp, format in [
        (8, framebuf.GS8),
        (4, framebuf.GS4_HMSB),
        (2, framebuf.GS2_HMSB),
        (1, framebuf.MONO_HLSB),
    ]:
        mask = framebuf.FrameBuffer(bytearray(4), 2, 2, format)
        for x in [0, 1]:
            for y in [0, 1]:
                mask.pixel(x, y, alphas[x][y] >> (8 - bpp))

        fbuf.fill(0)
        fbuf.blit(fbuf2, 1, 1, -1, None, mask)
        printbuf(2)

    # Blit a color FrameBuffer, with alpha mask, non-black background.
    alphas = [[0, 0x3F], [0x7F, 0xFF]]
    for bpp, format in [
        (8, framebuf.GS8),
        (4, framebuf.GS4_HMSB),
        (2, framebuf.GS2_HMSB),
        (1, framebuf.MONO_HLSB),
    ]:
        mask = framebuf.FrameBuffer(bytearray(4), 2, 2, format)
        for x in [0, 1]:
            for y in [0, 1]:
                mask.pixel(x, y, alphas[x][y] >> (8 - bpp))

        fbuf.fill(0b00000_111111_00000)
        fbuf.blit(fbuf2, 1, 1, -1, None, mask)
        printbuf(2)

# Basic tests of native and "byte-swapped" formats.
# Ensure order in output is little-endian, then big-endian
if sys.byteorder == "big":
    formats = [framebuf.RGB565_BS, framebuf.RGB565]
else:
    formats = [framebuf.RGB565, framebuf.RGB565_BS]

for format in formats:
    w = 5
    h = 4
    buf = bytearray(2 * w * h)
    fbuf = framebuf.FrameBuffer(buf, w, h, format)

    if format == framebuf.RGB565_BS:
        # Python provides byte-swapped color values.
        col = 0b11100000_11111011
    else:
        col = 0b11111011_11100000

    # set pixel at various locations with alpha.
    fbuf.fill(0)
    for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
        fbuf.pixel(x, y, col, 0x7F)
    printbuf(2)
