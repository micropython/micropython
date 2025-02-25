# Test machine.PWM, frequency and duty cycle (using machine.time_pulse_us).
#
# IMPORTANT: This test requires hardware connections: the PWM-output and pulse-input
# pins must be wired together (see the variable `pwm_pulse_pins`).

import sys
import time

try:
    from machine import time_pulse_us, Pin, PWM
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

pwm_freq_limit = 1000000
freq_margin_per_thousand = 0
duty_margin_per_thousand = 0
timing_margin_us = 5

# Configure pins based on the target.
if "esp32" in sys.platform:
    pwm_pulse_pins = ((4, 5),)
    freq_margin_per_thousand = 2
    duty_margin_per_thousand = 1
    timing_margin_us = 20
elif "esp8266" in sys.platform:
    pwm_pulse_pins = ((4, 5),)
    pwm_freq_limit = 1_000
    duty_margin_per_thousand = 3
    timing_margin_us = 50
elif "mimxrt" in sys.platform:
    if "Teensy" in sys.implementation._machine:
        # Teensy 4.x
        pwm_pulse_pins = (
            ("D0", "D1"),  # FLEXPWM X and UART 1
            ("D2", "D3"),  # FLEXPWM A/B
            ("D11", "D12"),  # QTMR and MOSI/MISO of SPI 0
        )
    else:
        pwm_pulse_pins = (("D0", "D1"),)
elif "rp2" in sys.platform:
    pwm_pulse_pins = (("GPIO0", "GPIO1"),)
elif "samd" in sys.platform:
    pwm_pulse_pins = (("D0", "D1"),)
    if "SAMD21" in sys.implementation._machine:
        # MCU is too slow to capture short pulses.
        pwm_freq_limit = 2_000
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


# Test a specific frequency and duty cycle.
def _test_freq_duty(self, pulse_in, pwm, freq, duty_u16):
    print("freq={:<5} duty_u16={:<5} :".format(freq, duty_u16), end="")

    # Check configured freq/duty_u16 is within error bound.
    freq_error = abs(pwm.freq() - freq) * 1000 // freq
    duty_error = abs(pwm.duty_u16() - duty_u16) * 1000 // (duty_u16 or 1)
    print(" freq={} freq_er={}".format(pwm.freq(), freq_error), end="")
    print(" duty={} duty_er={}".format(pwm.duty_u16(), duty_error), end="")
    print(" :", end="")
    self.assertLessEqual(freq_error, freq_margin_per_thousand)
    self.assertLessEqual(duty_error, duty_margin_per_thousand)

    # Calculate expected timing.
    expected_total_us = 1_000_000 // freq
    expected_high_us = expected_total_us * duty_u16 // 65535
    expected_low_us = expected_total_us - expected_high_us
    expected_us = (expected_low_us, expected_high_us)
    timeout = 2 * expected_total_us

    # Wait for output to settle.
    time_pulse_us(pulse_in, 0, timeout)
    time_pulse_us(pulse_in, 1, timeout)

    if duty_u16 == 0 or duty_u16 == 65535:
        # Expect a constant output level.
        no_pulse = (
            time_pulse_us(pulse_in, 0, timeout) < 0 and time_pulse_us(pulse_in, 1, timeout) < 0
        )
        self.assertTrue(no_pulse)
        if expected_high_us == 0:
            # Expect a constant low level.
            self.assertEqual(pulse_in(), 0)
        else:
            # Expect a constant high level.
            self.assertEqual(pulse_in(), 1)
    else:
        # Test timing of low and high pulse.
        n_averaging = 10
        for level in (0, 1):
            t = 0
            time_pulse_us(pulse_in, level, timeout)
            for _ in range(n_averaging):
                t += time_pulse_us(pulse_in, level, timeout)
            t //= n_averaging
            expected = expected_us[level]
            print(" level={} timing_er={}".format(level, abs(t - expected)), end="")
            self.assertLessEqual(abs(t - expected), timing_margin_us)

    print()


# Test a specific frequency with multiple duty cycles.
def _test_freq(self, freq):
    print()
    self.pwm.freq(freq)
    for duty in (0, 10, 25, 50, 75, 90, 100):
        duty_u16 = duty * 65535 // 100
        if sys.platform == "esp32":
            # TODO why is this bit needed to get it working on esp32?
            self.pwm.init(freq=freq, duty_u16=duty_u16)
            time.sleep(0.1)
        self.pwm.duty_u16(duty_u16)
        _test_freq_duty(self, self.pulse_in, self.pwm, freq, duty_u16)


# Given a set of pins, this test class will test multiple frequencies and duty cycles.
class TestBase:
    @classmethod
    def setUpClass(cls):
        print("set up pins:", cls.pwm_pin, cls.pulse_pin)
        cls.pwm = PWM(cls.pwm_pin)
        cls.pulse_in = Pin(cls.pulse_pin, Pin.IN)

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

    @unittest.skipIf(pwm_freq_limit < 2000, "frequency too high")
    def test_freq_2000(self):
        _test_freq(self, 2000)

    @unittest.skipIf(pwm_freq_limit < 5000, "frequency too high")
    def test_freq_5000(self):
        _test_freq(self, 5000)

    @unittest.skipIf(pwm_freq_limit < 10000, "frequency too high")
    def test_freq_10000(self):
        _test_freq(self, 10000)


# Generate test classes, one for each set of pins to test.
for pwm, pulse in pwm_pulse_pins:
    cls_name = "Test_{}_{}".format(pwm, pulse)
    globals()[cls_name] = type(
        cls_name, (TestBase, unittest.TestCase), {"pwm_pin": pwm, "pulse_pin": pulse}
    )

if __name__ == "__main__":
    unittest.main()
