# Test rp2.DMATimer functionality.

import time
import machine
import rp2
import unittest

K = 16

SRC = bytes(i & 0xFF for i in range(K << 10))


class Test(unittest.TestCase):
    def setUp(self):
        self.timer = rp2.DMATimer(0)
        self.dma = rp2.DMA()

    def tearDown(self):
        self.dma.close()
        self.timer.close()

    def test_printing(self):
        timer = self.timer
        self.assertEqual(str(timer), "DMATimer(0)")

    def test_set_ratio_unity(self):
        timer = self.timer
        timer.ratio = (1, 1)
        self.assertEqual(timer.ratio, (1, 1))

    def test_set_ratio(self):
        timer = self.timer
        timer.ratio = (1, 1)
        base_clock = timer.freq
        num, den = (47, 97)
        target = base_clock * num // den
        timer.ratio = (num, den)
        self.assertEqual(timer.freq, target)

    def test_set_freq(self):
        timer = self.timer
        timer.ratio = (1, 1)
        base_clock = timer.freq
        num, den = (53, 101)
        target = base_clock * num // den
        timer.freq = target
        self.assertEqual(timer.ratio, (num, den))

    def test_speed_throttle(self):
        def time_dma(d):
            start = time.ticks_us()
            d.active(1)
            while d.active():
                pass
            end = time.ticks_us()
            return time.ticks_diff(end, start)

        speeds = [5000 * (10**i) for i in range(4)]
        dma = self.dma
        timer = self.timer
        ctrl = dma.pack_ctrl(treq_sel=timer)
        dest = bytearray(2048)

        times = []
        deltas = []

        for i, speed in enumerate(speeds):
            timer.freq = speed
            dma.config(read=SRC, write=dest, count=len(dest) // 4, ctrl=ctrl)
            t = time_dma(dma)
            times.append(t)
            if i > 0:
                deltas.append(times[i - 1] - t)

        # The ratio between the first two times should be very close to 10
        self.assertIn(times[0] * 10 // times[1], range(95, 105))

        # The ratios between successive deltas (which removes the overhead) should also be about 10
        for i in range(len(deltas) - 1):
            self.assertIn(deltas[i] * 10 // deltas[i + 1], range(95, 105))


if __name__ == "__main__":
    unittest.main()
