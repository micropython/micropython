# NeoPixel driver for MicroPython
from machine import bitstream


class NeoPixel:
    # Internal buffer byte order: G R B W
    # ORDER maps the user-visible (R, G, B[, W]) index to the buffer position.
    ORDER = (1, 0, 2, 3)

    def __init__(self, pin, n, bpp=3, timing=1):
        self.pin = pin
        self.n = n
        self.bpp = bpp
        self.buf = bytearray(n * bpp)
        self.pin.init(pin.OUT)
        # timing=1  - 800 kHz (WS2812 standard)
        # timing=0  - 400 kHz
        # tuple     - custom (T0H, T0L, T1H, T1L) in nanoseconds
        self.timing = (
            ((400, 850, 800, 450) if timing else (800, 1700, 1600, 900))
            if isinstance(timing, int)
            else timing
        )

    def __len__(self):
        return self.n

    def __setitem__(self, i, v):
        offset = i * self.bpp
        for i in range(self.bpp):
            self.buf[offset + self.ORDER[i]] = v[i]

    def __getitem__(self, i):
        offset = i * self.bpp
        return tuple(self.buf[offset + self.ORDER[i]] for i in range(self.bpp))

    def fill(self, v):
        b = self.buf
        l = len(self.buf)
        bpp = self.bpp
        for i in range(bpp):
            c = v[i]
            j = self.ORDER[i]
            while j < l:
                b[j] = c
                j += bpp

    def write(self):
        # BITSTREAM_TYPE_HIGH_LOW = 0
        bitstream(self.pin, 0, self.timing, self.buf)
