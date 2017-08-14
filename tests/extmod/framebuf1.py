try:
    import framebuf
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

w = 5
h = 16
buf = bytearray(w * h // 8)
fbuf = framebuf.FrameBuffer1(buf, w, h, w)

# fill
fbuf.fill(1)
print(buf)
fbuf.fill(0)
print(buf)

# put pixel
fbuf.pixel(0, 0, 1)
fbuf.pixel(4, 0, 1)
fbuf.pixel(0, 15, 1)
fbuf.pixel(4, 15, 1)
print(buf)

# get pixel
print(fbuf.pixel(0, 0), fbuf.pixel(1, 1))

# scroll
fbuf.fill(0)
fbuf.pixel(2, 7, 1)
fbuf.scroll(0, 1)
print(buf)
fbuf.scroll(0, -2)
print(buf)
fbuf.scroll(1, 0)
print(buf)
fbuf.scroll(-1, 0)
print(buf)
fbuf.scroll(2, 2)
print(buf)
