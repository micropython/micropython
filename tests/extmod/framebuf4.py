try:
    import framebuf
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

def printbuf():
    print("--8<--")
    for y in range(h):
        print(buf[y * w // 2:(y + 1) * w // 2])
    print("-->8--")

w = 16
h = 8
buf = bytearray(w * h // 2)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.GS4_HMSB)

# fill
fbuf.fill(0x0f)
printbuf()
fbuf.fill(0xa0)
printbuf()

# put pixel
fbuf.pixel(0, 0, 0x01)
printbuf()
fbuf.pixel(w-1, 0, 0x02)
printbuf()
fbuf.pixel(w-1, h-1, 0x03)
printbuf()
fbuf.pixel(0, h-1, 0x04)
printbuf()

# get pixel
print(fbuf.pixel(0, 0), fbuf.pixel(w-1, 0), fbuf.pixel(w-1, h-1), fbuf.pixel(0, h-1))
print(fbuf.pixel(1, 0), fbuf.pixel(w-2, 0), fbuf.pixel(w-2, h-1), fbuf.pixel(1, h-1))

# fill rect
fbuf.fill_rect(0, 0, w, h, 0x0f)
printbuf()
fbuf.fill_rect(0, 0, w, h, 0xf0)
fbuf.fill_rect(1, 0, w//2+1, 1, 0xf1)
printbuf()
fbuf.fill_rect(1, 0, w//2+1, 1, 0x10)
fbuf.fill_rect(1, 0, w//2, 1, 0xf1)
printbuf()
fbuf.fill_rect(1, 0, w//2, 1, 0x10)
fbuf.fill_rect(0, h-4, w//2+1, 4, 0xaf)
printbuf()
fbuf.fill_rect(0, h-4, w//2+1, 4, 0xb0)
fbuf.fill_rect(0, h-4, w//2, 4, 0xaf)
printbuf()
fbuf.fill_rect(0, h-4, w//2, 4, 0xb0)
