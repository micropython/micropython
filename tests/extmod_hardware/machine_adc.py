# Test machine.ADC.
#
# IMPORTANT: This test requires hardware connections.

try:
    from machine import ADC, Pin
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest
from target_wiring import adc_loopback_pins


class TestBase:
    @classmethod
    def setUpClass(cls):
        print("set up pins:", cls.pin_adc, cls.pin_out)
        cls.adc = ADC(cls.pin_adc)
        cls.pin = Pin(cls.pin_out)

    def setUp(self):
        self.pin.init(Pin.OUT)

    def test_read_u16(self):
        self.pin(0)
        for _ in range(10):
            self.assertLessEqual(self.adc.read_u16(), 5000)
        self.pin(1)
        for _ in range(10):
            self.assertGreaterEqual(self.adc.read_u16(), 60000)


# Generate test classes, one for each set of pins to test.
for pin_adc, pin_out in adc_loopback_pins:
    cls_name = "Test_{}_{}".format(pin_adc, pin_out)
    globals()[cls_name] = type(
        cls_name, (TestBase, unittest.TestCase), {"pin_adc": pin_adc, "pin_out": pin_out}
    )

if __name__ == "__main__":
    unittest.main()
