try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit


def printbuf():
    print("--8<--")
    for y in range(h):
        print(buf[y * w * 2 : (y + 1) * w * 2])
    print("-->8--")


w = 4
h = 5
buf = bytearray(w * h * 2)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.RGB565)

# fill
fbuf.fill(0xFFFF)
printbuf()
fbuf.fill(0x0000)
printbuf()

# put pixel
fbuf.pixel(0, 0, 0xEEEE)
fbuf.pixel(3, 0, 0xEE00)
fbuf.pixel(0, 4, 0x00EE)
fbuf.pixel(3, 4, 0x0EE0)
printbuf()

# get pixel
print(fbuf.pixel(0, 4), fbuf.pixel(1, 1))

# scroll
fbuf.fill(0x0000)
fbuf.pixel(2, 2, 0xFFFF)
printbuf()
fbuf.scroll(0, 1)
printbuf()
fbuf.scroll(1, 0)
printbuf()
fbuf.scroll(-1, -2)
printbuf()

w2 = 2
h2 = 3
buf2 = bytearray(w2 * h2 * 2)
fbuf2 = framebuf.FrameBuffer(buf2, w2, h2, framebuf.RGB565)

fbuf2.fill(0x0000)
fbuf2.pixel(0, 0, 0x0EE0)
fbuf2.pixel(0, 2, 0xEE00)
fbuf2.pixel(1, 0, 0x00EE)
fbuf2.pixel(1, 2, 0xE00E)
fbuf.fill(0xFFFF)
fbuf.blit(fbuf2, 3, 3, 0x0000)
fbuf.blit(fbuf2, -1, -1, 0x0000)
fbuf.blit(fbuf2, 16, 16, 0x0000)
printbuf()
