# Test FrameBuffer.blit method.

try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit


def printbuf():
    print("--8<--")
    for y in range(h):
        for x in range(w):
            print("%02x" % buf[(x + y * w)], end="")
        print()
    print("-->8--")


w = 5
h = 4
buf = bytearray(w * h)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS8)

fbuf2 = framebuf.FrameBuffer(bytearray(4), 2, 2, framebuf.GS8)
fbuf2.fill(0xFF)

# Blit another FrameBuffer, at various locations.
for x, y in ((-1, -1), (0, 0), (1, 1), (4, 3)):
    fbuf.fill(0)
    fbuf.blit(fbuf2, x, y)
    printbuf()

# Blit a bytes object.
fbuf.fill(0)
image = (b"\x10\x11\x12\x13", 2, 2, framebuf.GS8)
fbuf.blit(image, 1, 1)
printbuf()

# Blit a bytes object that has a stride.
fbuf.fill(0)
image = (b"\x20\x21\xff\x22\x23\xff", 2, 2, framebuf.GS8, 3)
fbuf.blit(image, 1, 1)
printbuf()

# Blit a bytes object with a bytes palette.
fbuf.fill(0)
image = (b"\x00\x01\x01\x00", 2, 2, framebuf.GS8)
palette = (b"\xa1\xa2", 2, 1, framebuf.GS8)
fbuf.blit(image, 1, 1, -1, palette)
printbuf()

# Not enough elements in the tuple.
try:
    fbuf.blit((0, 0, 0), 0, 0)
except ValueError:
    print("ValueError")

# Too many elements in the tuple.
try:
    fbuf.blit((0, 0, 0, 0, 0, 0), 0, 0)
except ValueError:
    print("ValueError")

# Bytes too small.
try:
    fbuf.blit((b"", 1, 1, framebuf.GS8), 0, 0)
except ValueError:
    print("ValueError")
