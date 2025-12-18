# Test machine.Counter implementation
#
# IMPORTANT: This test requires hardware connections: the out_pin and in_pin
# must be wired together.

try:
    from machine import Counter
except ImportError:
    print("SKIP")
    raise SystemExit

import sys
from machine import Pin

if "esp32" in sys.platform:
    id = 0
    out_pin = 4
    in_pin = 5
else:
    print("Please add support for this test on this platform.")
    raise SystemExit

import unittest

out_pin = Pin(out_pin, mode=Pin.OUT)
in_pin = Pin(in_pin, mode=Pin.IN)


def toggle(times):
    for _ in range(times):
        out_pin(1)
        out_pin(0)


class TestCounter(unittest.TestCase):
    def setUp(self):
        out_pin(0)
        self.counter = Counter(id, in_pin)

    def tearDown(self):
        self.counter.deinit()

    def assertCounter(self, value):
        self.assertEqual(self.counter.value(), value)

    def test_connections(self):
        # Test the hardware connections are correct. If this test fails, all tests will fail.
        out_pin(1)
        self.assertEqual(1, in_pin())
        out_pin(0)
        self.assertEqual(0, in_pin())

    def test_count_rising(self):
        self.assertCounter(0)
        toggle(100)
        self.assertCounter(100)
        out_pin(1)
        self.assertEqual(self.counter.value(0), 101)
        self.assertCounter(0)  # calling value(0) resets
        out_pin(0)
        self.assertCounter(0)  # no rising edge
        out_pin(1)
        self.assertCounter(1)

    def test_change_directions(self):
        self.assertCounter(0)
        toggle(100)
        self.assertCounter(100)
        self.counter.init(in_pin, direction=Counter.DOWN)
        self.assertCounter(0)  # calling init() zeroes the counter
        self.counter.value(100)  # need to manually reset the value
        self.assertCounter(100)
        toggle(25)
        self.assertCounter(75)

    def test_count_falling(self):
        self.counter.init(in_pin, direction=Counter.UP, edge=Counter.FALLING)
        toggle(20)
        self.assertCounter(20)
        out_pin(1)
        self.assertCounter(20)  # no falling edge
        out_pin(0)
        self.assertCounter(21)
        self.counter.value(-(2**24))
        toggle(20)
        self.assertCounter(-(2**24 - 20))


if __name__ == "__main__":
    unittest.main()
