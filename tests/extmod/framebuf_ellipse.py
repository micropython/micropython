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

fbuf.ellipse(15, 15, 12, 8, False, 0xFF)  # Outline
fbuf.ellipse(15, 15, 8, 6, True, 0xAA)  # Fill
fbuf.ellipse(16, 10, 12, 8, False, 0xAA, 1)  # Outline segments
fbuf.ellipse(14, 10, 12, 8, False, 0xAA, 2)
fbuf.ellipse(14, 20, 12, 8, False, 0xAA, 4)
fbuf.ellipse(16, 20, 12, 8, False, 0xAA, 8)
printbuf()
# Filled segments
fbuf.fill(0)
fbuf.ellipse(16, 14, 6, 12, True, 0x55, 1)
fbuf.ellipse(14, 14, 6, 12, True, 0x55, 2)
fbuf.ellipse(14, 16, 6, 12, True, 0x55, 4)
fbuf.ellipse(16, 16, 6, 12, True, 0x55, 8)
printbuf()
