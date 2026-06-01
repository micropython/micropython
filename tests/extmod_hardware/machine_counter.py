# Test machine.Counter implementation
#
# IMPORTANT: This test requires hardware connections: the out_pin and in_pin, out_dir_pin and in_dir_pin
# must be wired together.

try:
    from machine import Counter
except ImportError:
    print("SKIP")
    raise SystemExit

import sys
import unittest
from machine import Pin
from target_wiring import encoder_loopback_id, encoder_loopback_out_pins, encoder_loopback_in_pins

id = encoder_loopback_id
out0_pin, out1_pin = encoder_loopback_out_pins
in0_pin, in1_pin = encoder_loopback_in_pins

out_pin = Pin(out0_pin, mode=Pin.OUT, value=0)
in_pin = Pin(in0_pin, mode=Pin.IN)

out_dir_pin = Pin(out1_pin, mode=Pin.OUT, value=0)
in_dir_pin = Pin(in1_pin, mode=Pin.IN)


def toggle(times):
    for _ in range(times):
        out_pin(1)
        out_pin(0)


number_of_callbacks = 0
callback_counter_value = None
callback_counter_cycles = None


def callback(counter):
    global number_of_callbacks, callback_counter_value, callback_counter_cycles
    number_of_callbacks += 1
    callback_counter_value = counter.value()
    callback_counter_cycles = counter.cycles()
    counter_irq_flags = counter.irq().flags()
    if "esp32" in sys.platform:
        print(
            f"\n callbacks:{number_of_callbacks}, counter.value():{callback_counter_value}, counter.cycles():{callback_counter_cycles}, counter.irq().flags():{counter_irq_flags}, MATCH:{counter_irq_flags & Counter.IRQ_MATCH}, ZERO:{counter_irq_flags & Counter.IRQ_ZERO}, MAX:{counter_irq_flags & Counter.IRQ_MAX}, MIN:{counter_irq_flags & Counter.IRQ_MIN}"
        )
    else:
        print(
            f"\n callbacks:{number_of_callbacks}, counter.value():{callback_counter_value}, counter.cycles():{callback_counter_cycles}, counter.irq().flags():{counter_irq_flags}, MATCH:{counter_irq_flags & Counter.IRQ_MATCH}, ROLL_OVER:{counter_irq_flags & Counter.IRQ_ROLL_OVER}, ROLL_UNDER:{counter_irq_flags & Counter.IRQ_ROLL_UNDER}"
        )
    # print(counter)


class TestConnections(unittest.TestCase):
    def test_connections(self):
        # Test the hardware connections are correct. If this test fails, all tests will fail.
        out_pin(1)
        self.assertEqual(1, in_pin())
        out_pin(0)
        self.assertEqual(0, in_pin())

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

    # @unittest.skipIf(sys.platform == "mimxrt", "Pin as a count direction is not supported")
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

        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)
        self.assertEqual(self.counter.irq().trigger(), Counter.IRQ_MATCH)

        out_dir_pin(1)  # count UP
        toggle(6_000)
        self.assertCounter(6_100)
        self.assertEqual(callback_counter_value, 5_000)
        self.assertEqual(number_of_callbacks, 1)

        self.counter.init(match=5_000)
        self.counter.value(6_100)

        out_dir_pin(0)  # count DOWN
        toggle(6_000)
        self.assertCounter(100)
        self.assertEqual(callback_counter_value, 5_000)
        self.assertEqual(number_of_callbacks, 2)

        self.counter.init(match=50_000)

        out_dir_pin(1)  # count UP
        toggle(60_000)
        self.assertCounter(60_000)
        self.assertEqual(callback_counter_value, 50_000)
        self.assertEqual(number_of_callbacks, 3)

        self.counter.init(match=-50_000)

        out_dir_pin(0)  # count DOWN
        toggle(60_000)
        self.assertCounter(-60_000)
        self.assertEqual(callback_counter_value, -50_000)
        self.assertEqual(number_of_callbacks, 4)

    def test_irq_UP_DOWN(self):
        self.counter.init(in_pin, direction=Counter.UP, match=150_000)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)
        toggle(160_000)
        self.assertCounter(160_000)
        self.assertEqual(callback_counter_value, 150_000)
        self.assertEqual(number_of_callbacks, 5)

        self.counter.init(in_pin, direction=Counter.DOWN, match=-150_000)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)
        toggle(160_000)
        self.assertCounter(-160_000)
        self.assertEqual(callback_counter_value, -150_000)
        self.assertEqual(number_of_callbacks, 6)


if __name__ == "__main__":
    unittest.main()
