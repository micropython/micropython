import time
import machine
from esp import neopixel_write

class NeoPixel:
    def __init__(self, pin, n):
        self.pin = pin
        self.n = n
        self.buf = bytearray(n * 3)

    def __setitem__(self, index, val):
        r, g, b = val
        self.buf[index * 3] = g
        self.buf[index * 3 + 1] = r
        self.buf[index * 3 + 2] = b

    def __getitem__(self, index):
        i = index * 3
        return self.buf[i], self.buf[i + 1], self.buf[i + 2]

    def write(self):
        neopixel_write(self.pin, self.buf, True)

def test():
    # put a neopixel strip on GPIO4
    p = machine.Pin(4, machine.Pin.OUT)
    np = NeoPixel(p, 8)
    n = np.n

    # cycle
    for i in range(4 * n):
        for j in range(n):
            np[j] = (0, 0, 0)
        np[i % n] = (255, 255, 255)
        np.write()
        time.sleep_ms(25)

    # bounce
    for i in range(4 * n):
        for j in range(n):
            np[j] = (0, 0, 128)
        if (i // n) % 2 == 0:
            np[i % n] = (0, 0, 0)
        else:
            np[n - 1 - (i % n)] = (0, 0, 0)
        np.write()
        time.sleep_ms(60)

    # fade in/out
    for i in range(0, 4 * 256, 8):
        for j in range(n):
            if (i // 256) % 2 == 0:
                val = i & 0xff
            else:
                val = 255 - (i & 0xff)
            np[j] = (val, 0, 0)
        np.write()

    # clear
    for i in range(n):
        np[i] = (0, 0, 0)
    np.write()

test()
