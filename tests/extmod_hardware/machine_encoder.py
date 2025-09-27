# Test machine.Encoder implementation
#
# IMPORTANT: This test requires hardware connections:
# - out0_pin and in0_pin must be wired together.
# - out1_pin and in1_pin must be wired together.

try:
    from machine import Encoder
except ImportError:
    print("SKIP")
    raise SystemExit

import sys
from machine import Pin

if "esp32" in sys.platform:
    id = 0
    out0_pin = 4
    in0_pin = 5
    out1_pin = 12
    in1_pin = 13
else:
    print("Please add support for this test on this platform.")
    raise SystemExit

import unittest

out0_pin = Pin(out0_pin, mode=Pin.OUT)
in0_pin = Pin(in0_pin, mode=Pin.IN)
out1_pin = Pin(out1_pin, mode=Pin.OUT)
in1_pin = Pin(in1_pin, mode=Pin.IN)


class TestEncoder(unittest.TestCase):
    def setUp(self):
        out0_pin(0)
        out1_pin(0)
        self.enc = Encoder(id, in0_pin, in1_pin)
        self.pulses = 0  # track the expected encoder position in software

    def tearDown(self):
        self.enc.deinit()

    def rotate(self, pulses):
        for _ in range(abs(pulses)):
            self.pulses += 1 if (pulses > 0) else -1
            q = self.pulses % 4
            # Only one pin should change state each "step" so output won't glitch
            out0_pin(q in (1, 2))
            out1_pin(q in (2, 3))

    def assertPosition(self, value):
        self.assertEqual(self.enc.value(), value)

    def test_connections(self):
        # Test the hardware connections are correct. If this test fails, all tests will fail.
        for ch, outp, inp in ((0, out0_pin, in0_pin), (1, out1_pin, in1_pin)):
            print("Testing channel ", ch)
            outp(1)
            self.assertEqual(1, inp())
            outp(0)
            self.assertEqual(0, inp())

    def test_basics(self):
        self.assertPosition(0)
        self.rotate(100)
        self.assertPosition(100 // 4)
        self.rotate(-100)
        self.assertPosition(0)

    def test_partial(self):
        # With phase=1 (default), need 4x pulses to count a rotation
        self.assertPosition(0)
        self.rotate(1)
        self.assertPosition(0)
        self.rotate(1)
        self.assertPosition(0)
        self.rotate(1)
        self.assertPosition(1)  # only 3 pulses to count first rotation?
        self.rotate(1)
        self.assertPosition(1)
        self.rotate(1)
        self.assertPosition(1)
        self.rotate(1)
        self.assertPosition(1)
        self.rotate(1)
        self.assertPosition(2)  # 4 for next rotation
        self.rotate(-1)
        self.assertPosition(1)
        self.rotate(-4)
        self.assertPosition(0)
        self.rotate(-4)
        self.assertPosition(-1)
        self.rotate(-3)
        self.assertPosition(-1)


if __name__ == "__main__":
    unittest.main()
