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
import unittest
from machine import Pin

callback_counter_value = None

if "esp32" in sys.platform:
    id = 0
    out_pin = 4
    in_pin = 5
    out_dir_pin = 16
    in_dir_pin = 17
elif sys.platform == "mimxrt":
    if "Teensy" in sys.implementation._machine:
        id = 0
        out_pin = "D2"
        in_pin = "D3"
else:
    print("Please add support for this test on this platform.")
    raise SystemExit

out_pin = Pin(out_pin, mode=Pin.OUT, value=0)
in_pin = Pin(in_pin, mode=Pin.IN)
if "esp32" in sys.platform:
    out_dir_pin = Pin(out_dir_pin, mode=Pin.OUT, value=0)
    in_dir_pin = Pin(in_dir_pin, mode=Pin.IN)


def toggle(times):
    for _ in range(times):
        out_pin(1)
        out_pin(0)


def callback(counter):
    global callback_counter_value
    callback_counter_value = counter.value()
    counter_irq_flags = counter.irq().flags()
    print(
        f"\n callback counter.value():{callback_counter_value:10}, counter.irq().flags():{counter_irq_flags:3}, flags.IRQ_MATCH:{counter_irq_flags & Counter.IRQ_MATCH > 0}"
    )
    # print(counter)


class TestConnections(unittest.TestCase):
    def test_connections(self):
        # Test the hardware connections are correct. If this test fails, all tests will fail.
        out_pin(1)
        self.assertEqual(1, in_pin())
        out_pin(0)
        self.assertEqual(0, in_pin())

        if "esp32" in sys.platform:
            out_dir_pin(1)
            self.assertEqual(1, in_dir_pin())
            out_dir_pin(0)
            self.assertEqual(0, in_dir_pin())


class TestCounter(unittest.TestCase):
    def setUp(self):
        out_pin(0)
        self.counter = Counter(id, in_pin)

    def tearDown(self):
        self.counter.deinit()

    def assertCounter(self, value):
        self.assertEqual(self.counter.value(), value)

    def test_count_rising_UP(self):
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
        out_pin(0)
        self.assertCounter(1)  # no rising edge

    def test_count_rising_DOWN(self):
        self.assertCounter(0)
        toggle(100)
        self.assertCounter(100)
        self.counter.init(in_pin, direction=Counter.DOWN)
        self.assertCounter(0)  # calling init() zeroes the counter
        self.counter.value(100)  # need to manually reset the value
        self.assertCounter(100)
        toggle(25)
        self.assertCounter(75)

    @unittest.skipIf(sys.platform == "mimxrt", "FALLING edge not supported")
    def test_count_falling_UP_DOWN(self):
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
        self.counter.init(in_pin, direction=Counter.DOWN)
        self.counter.value(-(2**24))
        toggle(20)
        self.assertCounter(-(2**24 + 20))

    @unittest.skipIf(sys.platform == "mimxrt", "Pin as a count direction is not supported")
    def test_count_pin_direction(self):
        self.counter.init(in_pin, direction=in_dir_pin, match=5_000)

        out_dir_pin(1)  # count UP
        out_pin(0)
        self.assertCounter(0)  # no rising edge
        out_pin(1)
        self.assertCounter(1)  # rising edge
        out_pin(0)
        self.assertCounter(1)  # no rising edge
        toggle(100)
        self.assertCounter(101)

        out_dir_pin(0)  # count DOWN
        out_pin(0)
        self.assertCounter(101)  # no rising edge
        out_pin(1)
        self.assertCounter(100)  # rising edge
        out_pin(0)
        self.assertCounter(100)  # no rising edge
        toggle(100)
        self.assertCounter(0)

        out_dir_pin(1)  # count UP
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)
        toggle(6_000)
        self.assertCounter(6_000)
        self.assertEqual(callback_counter_value, 5_000)

        out_dir_pin(0)  # count DOWN
        toggle(6_000 + 10)
        self.assertCounter(-10)
        self.assertEqual(callback_counter_value, 5_000)

    def test_irq(self):
        self.counter.init(in_pin, direction=Counter.UP, match=50_000)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)
        toggle(60_000)
        self.assertCounter(60_000)
        self.assertEqual(callback_counter_value, 50_000)

        self.counter.init(in_pin, direction=Counter.DOWN, match=-50_000)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)
        toggle(60_000)
        self.assertCounter(-60_000)
        self.assertEqual(callback_counter_value, -50_000)


if __name__ == "__main__":
    unittest.main()
