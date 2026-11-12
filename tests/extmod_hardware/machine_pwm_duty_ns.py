# Test machine.PWM.duty_ns() method.
# This doesn't actually need pins connected in loopback mode (at the moment) but
# does reuse target_wiring.pwm_loopback_pins for convenience.
#
# TODO clean up and get working on all supported boards...
#
# requires: target-wiring

import sys

try:
    from machine import PWM
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest
from target_wiring import pwm_loopback_pins as pwm_standalone_pins
# need to test both 32-bit and 16-bit timers (on stm32 at least)
#pwm_standalone_pins = ["D0", "D3"]
#pwm_standalone_pins = ["X1", "X7"]

# Test a specific frequency with multiple duty cycles.
def _test_freq(self, freq):
    print()
    self.pwm.freq(freq)

    # Test 0ns.
    self.pwm.duty_ns(0)
    self.assertEqual(self.pwm.duty_u16(), 0)

    # Test maximum ns.
    max_ns = 1_000_000_000 // freq
    self.pwm.duty_ns(max_ns)
    self.assertEqual(self.pwm.duty_u16(), 65535)

    # Test over maximum ns.
    max_ns_delta = 100
    if sys.platform == "mimxrt":
        # mimxrt checks for out-of-bounds, so use a smaller value
        max_ns_delta = 10
    self.pwm.duty_ns(max_ns + max_ns_delta)
    self.assertEqual(self.pwm.duty_u16(), 65535)

    # Test various values.
    for duty in (0, 10, 25, 50, 75, 90, 100):
        duty_u16 = duty * 65535 // 100
        duty_ns = 1_000_000_000 * duty_u16 // freq // 65535

        # Set duty_u16, check duty_ns is accurate.
        self.pwm.duty_u16(duty_u16)
        er = self.pwm.duty_ns() - duty_ns
        self.assertLessEqual(er, 500)

        # Set duty_ns, check duty_u16 is accurate.
        self.pwm.duty_ns(duty_ns)
        er = self.pwm.duty_u16() - duty_u16
        self.assertLessEqual(er, 1)

    # Set duty back to 0 using duty_u16 so freq can be freely changed.
    self.pwm.duty_u16(0)


# Given a set of pins, this test class will test multiple frequencies and duty cycles.
class TestBase:
    @classmethod
    def setUpClass(cls):
        print("set up pins:", cls.pwm_pin)
        cls.pwm = PWM(cls.pwm_pin)
        print(cls.pwm)

    @classmethod
    def tearDownClass(cls):
        cls.pwm.deinit()

    def test_freq_50(self):
        _test_freq(self, 50)

    def test_freq_100(self):
        _test_freq(self, 100)

    def test_freq_500(self):
        _test_freq(self, 500)

    def test_freq_1000(self):
        _test_freq(self, 1000)


# Generate test classes, one for each set of pins to test.
for pwm, pulse in pwm_standalone_pins:
    cls_name = "Test_{}".format(pwm)
    globals()[cls_name] = type(
        cls_name, (TestBase, unittest.TestCase), {"pwm_pin": pwm}
    )

if __name__ == "__main__":
    unittest.main()
