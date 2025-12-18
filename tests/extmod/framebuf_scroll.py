try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit


def printbuf():
    print("--8<--")
    bytes_per_row = w // 2
    for y in range(h):
        for x in range(bytes_per_row):
            print("%02x" % buf[(x + y * bytes_per_row)], end="")
        print()
    print("-->8--")


w = 10
h = 10
buf = bytearray(w * h // 2)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS4_HMSB)


def prepare_buffer():
    fbuf.fill(0)
    fbuf.rect(2, 0, 6, 10, 0x07, True)
    fbuf.rect(0, 2, 10, 6, 0x01, True)


prepare_buffer()
printbuf()

fbuf.scroll(5, -1)
printbuf()

prepare_buffer()
fbuf.scroll(-5, 5)
printbuf()

prepare_buffer()
# Scrolling by at least the size of buffer, no change to buffer.
fbuf.scroll(15, 7)
fbuf.scroll(10, -1)
fbuf.scroll(1, -10)
try:
    fbuf.scroll(1000000000000, -1)
except OverflowError:
    # When mp_int_t is 32 bits, this throws OverflowError.
    pass
printbuf()
