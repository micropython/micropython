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


w = 30
h = 30
buf = bytearray(w * h)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS8)

# Outline
fbuf.fill(0)
fbuf.ellipse(15, 15, 12, 6, 0xFF, False)
printbuf()

# Fill
fbuf.fill(0)
fbuf.ellipse(15, 15, 6, 12, 0xAA, True)
printbuf()

# Outline and fill some different quadrant combos.
for m in (0, 0b0001, 0b0010, 0b0100, 0b1000, 0b1010):
    fbuf.fill(0)
    fbuf.ellipse(15, 15, 6, 12, 0xAA, False, m)
    printbuf()
    fbuf.fill(0)
    fbuf.ellipse(15, 15, 6, 12, 0xAA, True, m)
    printbuf()

# Draw ellipses that will go out of bounds at each of the edges.
for x, y in (
    (
        4,
        4,
    ),
    (
        26,
        4,
    ),
    (
        26,
        26,
    ),
    (
        4,
        26,
    ),
):
    fbuf.fill(0)
    fbuf.ellipse(x, y, 6, 12, 0xAA, False)
    printbuf()
    fbuf.fill(0)
    fbuf.ellipse(x, y, 6, 12, 0xAA, True)
    printbuf()

# Draw an ellipse with both radius 0
fbuf.fill(0)
fbuf.ellipse(15, 15, 0, 0, 0xFF, True)
printbuf()

# Draw an ellipse with both radius 0 out of bounds
fbuf.fill(0)
fbuf.ellipse(45, 45, 0, 0, 0xFF, True)
printbuf()

# Draw an ellipse with radius 0 and all sectors masked out
fbuf.fill(0)
fbuf.ellipse(15, 15, 0, 0, 0xFF, True, 0)
printbuf()
