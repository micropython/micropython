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
fbuf.ellipse(15, 15, 12, 6, False, 0xFF)
printbuf()
fbuf.fill(0)
fbuf.ellipse(15, 15, 6, 12, True, 0xAA)
printbuf()

