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


w = 8
h = 5
buf = bytearray(w * h)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS8)

# fill
fbuf.fill(0x55)
printbuf()

# put pixel
fbuf.pixel(0, 0, 0x11)
fbuf.pixel(w - 1, 0, 0x22)
fbuf.pixel(0, h - 1, 0x33)
fbuf.pixel(w - 1, h - 1, 0xFF)
printbuf()

# get pixel
print(hex(fbuf.pixel(0, h - 1)), hex(fbuf.pixel(1, 1)))
