try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit


def printbuf():
    print("--8<--")
    for y in range(h):
        for x in range(w):
            print("%u" % ((buf[(x + y * w) // 4] >> ((x & 3) << 1)) & 3), end="")
        print()
    print("-->8--")


w = 8
h = 5
buf = bytearray(w * h // 4)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS2_HMSB)

# fill
fbuf.fill(3)
printbuf()
fbuf.fill(0)
printbuf()

# put pixel
fbuf.pixel(0, 0, 1)
fbuf.pixel(3, 0, 2)
fbuf.pixel(0, 4, 3)
fbuf.pixel(3, 4, 2)
printbuf()

# get pixel
print(fbuf.pixel(0, 4), fbuf.pixel(1, 1))

# scroll
fbuf.fill(0)
fbuf.pixel(2, 2, 3)
printbuf()
fbuf.scroll(0, 1)
printbuf()
fbuf.scroll(1, 0)
printbuf()
fbuf.scroll(-1, -2)
printbuf()

w2 = 2
h2 = 3
buf2 = bytearray(h2 * ((w2 + 3) // 4))
fbuf2 = framebuf.FrameBuffer(buf2, w2, h2, framebuf.GS2_HMSB)

# blit
fbuf2.fill(0)
fbuf2.pixel(0, 0, 1)
fbuf2.pixel(0, 2, 2)
fbuf2.pixel(1, 0, 1)
fbuf2.pixel(1, 2, 2)
fbuf.fill(3)
fbuf.blit(fbuf2, 3, 3, 0)
fbuf.blit(fbuf2, -1, -1, 0)
fbuf.blit(fbuf2, 16, 16, 0)
printbuf()
