try:
    import framebuf
except ImportError:
    print("SKIP")
    raise SystemExit

def printbuf():
    print("----+",end='')
    [print("----{:3}---+".format(i),end='') for i in range(w)]
    print()
    for y in range(h):
        print("{:3} | ".format(y),end='')
        for x in range(w):
            index = y*w*3 + x*3
            print("0x{:02X}{:02X}{:02X} | ".format(buf[index],buf[index+1],buf[index+2]),end='')
        print()
    print("----+"+"----------+"*w)

w = 6
h = 5
buf = bytearray(w * h * 3)
fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.RGB888)

# fill
fbuf.fill(0xffffff)
printbuf()
fbuf.fill(0x000000)
printbuf()

# put pixel
fbuf.pixel(0, 0, 0xaabbcc)
fbuf.pixel(3, 0, 0xaabbcc)
fbuf.pixel(0, 4, 0xddeeff)
fbuf.pixel(3, 4, 0xddeeff)
printbuf()

# get pixel
print(fbuf.pixel(0, 4), fbuf.pixel(1, 1))
print(fbuf.pixel(0,5))
# scroll
fbuf.fill(0x000000)
fbuf.pixel(2, 2, 0xffffff)
printbuf()
fbuf.scroll(0, 1)
printbuf()
fbuf.scroll(1, 0)
printbuf()
fbuf.scroll(-1, -2)
printbuf()

w2 = 2
h2 = 3
buf2 = bytearray(w2 * h2 * 3)
fbuf2 = framebuf.FrameBuffer(buf2, w2, h2, framebuf.RGB888)

fbuf2.fill(0xaabbcc)
fbuf.fill(0xffffff)
fbuf.blit(fbuf2, 3, 3, 0x000000)
fbuf.blit(fbuf2, -1, -1, 0x000000)
fbuf.blit(fbuf2, 16, 16, 0x000000)
printbuf()
