import time
import machine
from esp import apa102_write

class APA102:
    def __init__(self, clock_pin, data_pin, n):
        self.clock_pin = clock_pin
        self.data_pin = data_pin
        self.n = n
        self.buf = bytearray(n * 4)

    def __setitem__(self, index, val):
        r, g, b, brightness = val
        self.buf[index * 4] = r
        self.buf[index * 4 + 1] = g
        self.buf[index * 4 + 2] = b
        self.buf[index * 4 + 3] = brightness

    def __getitem__(self, index):
        i = index * 4
        return self.buf[i], self.buf[i + 1], self.buf[i + 2], self.buf[i + 3]

    def write(self):
        apa102_write(self.clock_pin, self.data_pin, self.buf)

def test():
    # put a neopixel strip on GPIO14 as clock and GPIO13 as data pin
    c = machine.Pin(14, machine.Pin.OUT)
    d = machine.Pin(13, machine.Pin.OUT)
    apa = APA102(c, d, 8)
    n = apa.n

    # cycle
    for i in range(4 * n):
        for j in range(n):
            apa[j] = (0, 0, 0, 0)
        apa[i % n] = (255, 255, 255, 31)
        apa.write()
        time.sleep_ms(25)

    # bounce
    for i in range(4 * n):
        for j in range(n):
            apa[j] = (0, 0, 128, 31)
        if (i // n) % 2 == 0:
            apa[i % n] = (0, 0, 0, 0)
        else:
            apa[n - 1 - (i % n)] = (0, 0, 0, 0)
        apa.write()
        time.sleep_ms(60)

    # fade in/out
    for i in range(0, 4 * 256, 8):
        for j in range(n):
            if (i // 256) % 2 == 0:
                val = i & 0xff
            else:
                val = 255 - (i & 0xff)
            apa[j] = (val, 0, 0, 31)
        apa.write()

    # clear
    for i in range(n):
        apa[i] = (0, 0, 0, 0)

    apa.write()

test()
