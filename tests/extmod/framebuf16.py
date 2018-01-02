try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit


def printbuf(buf, w, h):
    print("--8<--")
    for y in range(h):
        print(buf[y * w * 2:(y + 1) * w * 2])
    print("-->8--")


def test_basic():
    w = 4
    h = 5
    buf = bytearray(w * h * 2)
    fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.RGB565)

    # fill
    fbuf.fill(0xffff)
    printbuf(buf, w, h)
    fbuf.fill(0x0000)
    printbuf(buf, w, h)

    # put pixel
    fbuf.pixel(0, 0, 0xeeee)
    fbuf.pixel(3, 0, 0xee00)
    fbuf.pixel(0, 4, 0x00ee)
    fbuf.pixel(3, 4, 0x0ee0)
    printbuf(buf, w, h)

    # get pixel
    print(fbuf.pixel(0, 4), fbuf.pixel(1, 1))

    # scroll
    fbuf.fill(0x0000)
    fbuf.pixel(2, 2, 0xffff)
    printbuf(buf, w, h)
    fbuf.scroll(0, 1)
    printbuf(buf, w, h)
    fbuf.scroll(1, 0)
    printbuf(buf, w, h)
    fbuf.scroll(-1, -2)
    printbuf(buf, w, h)

    w2 = 2
    h2 = 3
    buf2 = bytearray(w2 * h2 * 2)
    fbuf2 = framebuf.FrameBuffer(buf2, w2, h2, framebuf.RGB565)

    fbuf2.fill(0x0000)
    fbuf2.pixel(0, 0, 0x0ee0)
    fbuf2.pixel(0, 2, 0xee00)
    fbuf2.pixel(1, 0, 0x00ee)
    fbuf2.pixel(1, 2, 0xe00e)
    fbuf.fill(0xffff)
    fbuf.blit(fbuf2, 3, 3, 0x0000)
    fbuf.blit(fbuf2, -1, -1, 0x0000)
    fbuf.blit(fbuf2, 16, 16, 0x0000)
    printbuf(buf, w, h)


def test_endian():
    for endian in (framebuf.RGB565_BE, framebuf.RGB565_LE,):
        w = 4
        h = 4
        buf = bytearray(w * h * 2)
        fbuf = framebuf.FrameBuffer(buf, w, h, endian)
        fbuf.fill_rect(1, 1, 2, 2, 0xabcd)
        fbuf.pixel(2, 2, 0xdcba)
        printbuf(buf, w, h)


test_basic()
test_endian()
