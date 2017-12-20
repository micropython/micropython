# test subclassing framebuf.FrameBuffer

try:
    import framebuf
except ImportError:
    print('SKIP')
    raise SystemExit

class FB(framebuf.FrameBuffer):
    def __init__(self, n):
        self.n = n
        super().__init__(bytearray(2 * n * n), n, n, framebuf.RGB565)

    def foo(self):
        self.hline(0, 2, self.n, 0x0304)

fb = FB(n=3)
fb.pixel(0, 0, 0x0102)
fb.foo()
print(bytes(fb))
