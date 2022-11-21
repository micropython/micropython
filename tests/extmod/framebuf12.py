try:
    import framebuf, usys
except ImportError:
    print("SKIP")
    raise SystemExit

# This test and its .exp file is based on a little-endian architecture.
if usys.byteorder != "little":
    print("SKIP")
    raise SystemExit


def printbuf():
    print("--8<--")
    for y in range(h):
        print(buf[int(y * w * 1.5) : int((y + 1) * w * 1.5)])
    print("-->8--")


w = 4
h = 5
buf = bytearray(int(w * h * 1.5))
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.RGB444)

# fill
fbuf.fill(0xFFF)
printbuf()
fbuf.fill(0x000)
printbuf()

# put pixel
fbuf.pixel(0, 0, 0xAAA)
fbuf.pixel(3, 0, 0xB0B)
fbuf.pixel(0, 4, 0xABC)
fbuf.pixel(3, 4, 0xCBA)
printbuf()

# get pixel
print(fbuf.pixel(0, 4), fbuf.pixel(1, 1))

# scroll
fbuf.fill(0x000)
fbuf.pixel(2, 2, 0xFFF)
printbuf()
fbuf.scroll(0, 1)
printbuf()
fbuf.scroll(1, 0)
printbuf()
fbuf.scroll(-1, -2)
printbuf()

w2 = 2
h2 = 3
buf2 = bytearray(int(w2 * h2 * 1.5))
fbuf2 = framebuf.FrameBuffer(buf2, w2, h2, framebuf.RGB444)

fbuf2.fill(0x000)
fbuf2.pixel(0, 0, 0xABC)
fbuf2.pixel(0, 2, 0x123)
fbuf2.pixel(1, 0, 0x321)
fbuf2.pixel(1, 2, 0xCBA)
fbuf.fill(0xFFF)
fbuf.blit(fbuf2, 3, 3, 0x000)
fbuf.blit(fbuf2, -1, -1, 0x000)
fbuf.blit(fbuf2, 16, 16, 0x000)
printbuf()
