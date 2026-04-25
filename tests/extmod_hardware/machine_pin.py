# Test machine.Pin.
#
# IMPORTANT: This test requires hardware connections.

try:
    from machine import Pin
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

try:
    from target_wiring import pin_loopback_pins
except:
    from target_wiring import pwm_loopback_pins as pin_loopback_pins


class TestBase:
    @classmethod
    def setUpClass(cls):
        print("set up pins:", cls.pin_out, cls.pin_in)
        cls.p0 = Pin(cls.pin_out)
        cls.p1 = Pin(cls.pin_in)

    def setUp(self):
        self.p0.init(Pin.OUT)
        self.p1.init(Pin.IN)

    def test_connections(self):
        print(self.pin_out, self.pin_in)
        print(self.p0, self.p1)
        self.p0(1)
        self.assertEqual(1, self.p1())
        self.p0(0)
        self.assertEqual(0, self.p1())


# Generate test classes, one for each set of pins to test.
for pin_out, pin_in in pin_loopback_pins:
    cls_name = "Test_{}_{}".format(pin_out, pin_in)
    globals()[cls_name] = type(
        cls_name, (TestBase, unittest.TestCase), {"pin_out": pin_out, "pin_in": pin_in}
    )

if __name__ == "__main__":
    unittest.main()
