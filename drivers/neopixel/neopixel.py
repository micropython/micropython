# NeoPixel driver for MicroPython
# MIT license; Copyright (c) 2016 Damien P. George, 2021 Jim Mussared
#
# (C) 2021 CeDeROM Tomasz CEDRO.
# Note: Inverted DIN using single transistor (i.e. IRLML6344) is experimental!
# Below analysis of inverted DIN timings is presented. Works on WS2812B.
#     STREAM: 11001010
#        BUS: 110 110 100 100 110 100 110 100
#        LET: A B A B CD  CD  A B CD  A B CD
# INV STREAM: 00110101
#    INV BUS: 001 001 011 011 001 011 001 011
#    INV LET: D C D C BA  BA  D C BA  D C BA


from machine import bitstream

_BITSTREAM_TYPE_HIGH_LOW = const(0)
_TIMING_WS2818_800 = (400, 850, 800, 450)
_TIMING_WS2818_400 = (800, 1700, 1600, 900)
_TIMING_WS2818_800_INV = (850, 400, 450, 800)
_TIMING_WS2812_400_INV = (1700, 800, 900, 1600)


class NeoPixel:
    # G R B W
    ORDER = (1, 0, 2, 3)

    def __init__(self, pin, n, bpp=3, timing=1, invertedDIN=False):
        self.pin = pin
        self.n = n
        self.bpp = bpp
        self.buf = bytearray(n * bpp)
        self.invertedDIN = invertedDIN
        self.pin.init(pin.OUT)
        # Timing arg can either be 1 for 800kHz or 0 for 400kHz,
        # or a user-specified timing ns tuple (high_0, low_0, high_1, low_1).
        if self.invertedDIN:
            self.timing = (
                (_TIMING_WS2818_800_INV if timing else _TIMING_WS2818_400_INV)
                if isinstance(timing, int)
                else timing
            )
        else:
            self.timing = (
                (_TIMING_WS2818_800 if timing else _TIMING_WS2818_400)
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
        if self.invertedDIN:
            pin.value(1)
        bitstream(self.pin, _BITSTREAM_TYPE_HIGH_LOW, self.timing, self.buf)
        if self.invertedDIN:
            pin.value(1)
