# Test machine.Pin.
#
# IMPORTANT: This test requires hardware connections.

try:
    from machine import Pin
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest
from target_wiring import pin_loopback_pins


class TestBase:
    @classmethod
    def setUpClass(cls):
        print("set up pins:", cls.p0_name, cls.p1_name)
        cls.p0 = Pin(cls.p0_name)
        cls.p1 = Pin(cls.p1_name)

    def tearDown(self):
        self.p0.init(Pin.IN)
        self.p1.init(Pin.IN)

    def test_init_with_value(self):
        self.p1.init(Pin.IN)
        for _ in range(4):
            self.p0.init(Pin.OUT, value=0)
            self.assertEqual(self.p1(), 0)
            self.p0.init(Pin.OUT, value=1)
            self.assertEqual(self.p1(), 1)

    def test_call(self):
        self.p0.init(Pin.OUT)
        self.p1.init(Pin.IN)
        for _ in range(4):
            self.p0(0)
            self.assertEqual(self.p1(), 0)
            self.p0(1)
            self.assertEqual(self.p1(), 1)

    def test_value(self):
        self.p0.init(Pin.OUT)
        self.p1.init(Pin.IN)
        for _ in range(4):
            self.p0.value(0)
            self.assertEqual(self.p1.value(), 0)
            self.p0.value(1)
            self.assertEqual(self.p1.value(), 1)

    @unittest.skipUnless(hasattr(Pin, "low"), "no low/high methods")
    def test_low_high(self):
        self.p0.init(Pin.OUT)
        self.p1.init(Pin.IN)
        for _ in range(4):
            self.p0.low()
            self.assertEqual(self.p1(), 0)
            self.p0.high()
            self.assertEqual(self.p1(), 1)

    def test_off_on(self):
        self.p0.init(Pin.OUT)
        self.p1.init(Pin.IN)
        for _ in range(4):
            self.p0.off()
            self.assertEqual(self.p1(), 0)
            self.p0.on()
            self.assertEqual(self.p1(), 1)

    def _test_pull(self, pull, value):
        inv_value = 1 - value
        self.p1.init(Pin.IN)
        for _ in range(4):
            self.p0.init(Pin.OUT, value=inv_value)
            self.assertEqual(self.p1(), inv_value)
            self.p0.init(Pin.IN, pull)
            self.assertEqual(self.p1(), value)

    def test_pull_up(self):
        self._test_pull(Pin.PULL_UP, 1)

    def test_pull_down(self):
        self._test_pull(Pin.PULL_DOWN, 0)

    def test_open_drain(self):
        self.p0.init(Pin.OPEN_DRAIN, Pin.PULL_UP)
        self.p1.init(Pin.OPEN_DRAIN, Pin.PULL_UP)
        for _ in range(4):
            self.p0(1)
            self.p1(1)
            self.assertEqual(self.p0(), 1)
            self.assertEqual(self.p1(), 1)
            self.p0(0)
            self.p1(1)
            self.assertEqual(self.p0(), 0)
            self.assertEqual(self.p1(), 0)
            self.p0(1)
            self.p1(0)
            self.assertEqual(self.p0(), 0)
            self.assertEqual(self.p1(), 0)
            self.p0(0)
            self.p1(0)
            self.assertEqual(self.p0(), 0)
            self.assertEqual(self.p1(), 0)


# Generate test classes, one for each set of pins to test.
for p0_name, p1_name in pin_loopback_pins:
    cls_name = "Test_{}_{}".format(p0_name, p1_name)
    globals()[cls_name] = type(
        cls_name, (TestBase, unittest.TestCase), {"p0_name": p0_name, "p1_name": p1_name}
    )

if __name__ == "__main__":
    unittest.main()
