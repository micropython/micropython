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

COUNTER_UP = Counter.UP
COUNTER_DOWN = Counter.DOWN

id = encoder_loopback_id
out0_pin, out1_pin = encoder_loopback_out_pins
in0_pin, in1_pin = encoder_loopback_in_pins

out_pin = Pin(out0_pin, mode=Pin.OUT, value=0)
in_pin = Pin(in0_pin, mode=Pin.IN)

out_dir_pin = Pin(out1_pin, mode=Pin.OUT, value=COUNTER_UP)
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
            f"\n    callbacks:{number_of_callbacks}, counter.value():{callback_counter_value}, counter.cycles():{callback_counter_cycles}, counter.irq().flags():{counter_irq_flags}, MATCH:{counter_irq_flags & Counter.IRQ_MATCH}, HERE:{counter_irq_flags & Counter.IRQ_HERE}, MAX:{counter_irq_flags & Counter.IRQ_MAX},  MIN:{counter_irq_flags & Counter.IRQ_MIN}, Z:{counter_irq_flags & 3}"
        )
    else:
        print(
            f"\n  callbacks:{number_of_callbacks}, counter.value():{callback_counter_value}, counter.cycles():{callback_counter_cycles}, counter.irq().flags():{counter_irq_flags}, MATCH:{counter_irq_flags & Counter.IRQ_MATCH}, ROLL_OVER:{counter_irq_flags & Counter.IRQ_ROLL_OVER}, ROLL_UNDER:{counter_irq_flags & Counter.IRQ_ROLL_UNDER}"
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
        global number_of_callbacks, callback_counter_value
        number_of_callbacks = 0
        callback_counter_value = None

        out_pin(0)
        self.counter = Counter(id, in_pin)

    def tearDown(self):
        self.counter.deinit()
        # print("self.counter.deinit()")

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
        toggle(35_000)
        self.assertCounter(100 - 35_000)

    @unittest.skipIf(sys.platform == "mimxrt", "FALLING edge not supported")
    def test_count_falling_UP_DOWN(self):
        self.counter.init(in_pin, edge=Counter.FALLING)
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

    def test_irq_UP_DOWN(self):
        self.counter.init(in_pin, match=150_000)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)
        toggle(160_000)
        self.assertCounter(160_000)
        self.assertEqual(callback_counter_value, 150_000)
        self.assertEqual(number_of_callbacks, 1)

        self.counter.init(direction=Counter.DOWN, match=-150_000)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)
        toggle(160_000)
        self.assertCounter(-160_000)
        self.assertEqual(callback_counter_value, -150_000)
        self.assertEqual(number_of_callbacks, 2)

    def test_cycles(self):
        self.counter.init(in_pin, max=10, min=-10)
        toggle(9)
        self.assertCounter(9)
        toggle(10)
        self.assertCounter(19)
        self.assertEqual(self.counter.cycles(), 0)
        toggle(1)
        self.assertCounter(20)
        self.assertEqual(self.counter.cycles(), 1)
        toggle(1)
        self.assertCounter(21)
        self.assertEqual(self.counter.cycles(), 1)
        toggle(78)
        self.assertCounter(99)
        self.assertEqual(self.counter.cycles(), 4)
        toggle(1)
        self.assertCounter(100)
        self.assertEqual(self.counter.cycles(), 5)
        toggle(1)
        self.assertCounter(101)
        self.assertEqual(self.counter.cycles(), 5)

    def test_max_UP_DOWN(self):
        global callback_counter_value
        self.counter.init(in_pin, direction=in_dir_pin, match=225, max=100)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)

        out_dir_pin(COUNTER_UP)
        toggle(225)
        self.assertCounter(225)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, 225)
            self.assertEqual(number_of_callbacks, 1)
        toggle(225)
        self.assertCounter(450)

        callback_counter_value = 0
        out_dir_pin(COUNTER_DOWN)
        toggle(225)
        self.assertCounter(225)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, 225)
            self.assertEqual(number_of_callbacks, 2)
        toggle(225)
        self.assertCounter(0)

    def test_max_DOWN_UP(self):
        global callback_counter_value
        self.counter.init(in_pin, direction=in_dir_pin, match=-225, max=100)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)

        out_dir_pin(COUNTER_DOWN)
        toggle(450)
        self.assertCounter(-450)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, -225)
            self.assertEqual(number_of_callbacks, 1)

        callback_counter_value = 0
        out_dir_pin(COUNTER_UP)
        toggle(450)
        self.assertCounter(0)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, -225)
            self.assertEqual(number_of_callbacks, 2)

    def test_count_pin_direction(self):
        self.counter.init(in_pin, direction=in_dir_pin, match=5_000)

        out_dir_pin(COUNTER_UP)
        out_pin(0)
        self.assertCounter(0)  # no rising edge
        out_pin(1)
        self.assertCounter(1)  # rising edge
        out_pin(0)
        self.assertCounter(1)  # no rising edge
        toggle(100)
        self.assertCounter(101)

        out_dir_pin(COUNTER_DOWN)
        out_pin(0)
        self.assertCounter(101)  # no rising edge
        out_pin(1)
        self.assertCounter(100)  # rising edge
        out_pin(0)
        self.assertCounter(100)  # no rising edge
        toggle(100)
        self.assertCounter(0)

        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH)
        self.assertEqual(self.counter.irq().trigger(), Counter.IRQ_MATCH)

        out_dir_pin(COUNTER_UP)
        toggle(6_000)
        self.assertCounter(6_000)
        self.assertEqual(callback_counter_value, 5_000)
        self.assertEqual(number_of_callbacks, 1)

        self.counter.init(match=5_000)
        self.counter.irq().trigger(Counter.IRQ_MATCH)
        self.counter.value(6_000)

        out_dir_pin(COUNTER_DOWN)
        toggle(6_000)
        self.assertCounter(0)
        self.assertEqual(callback_counter_value, 5_000)
        self.assertEqual(number_of_callbacks, 2)

        self.counter.init(match=50_000)
        self.counter.irq().trigger(Counter.IRQ_MATCH)

        out_dir_pin(COUNTER_UP)
        toggle(60_000)
        self.assertCounter(60_000)
        self.assertEqual(callback_counter_value, 50_000)
        self.assertEqual(number_of_callbacks, 3)

        self.counter.init(match=-50_000)
        self.counter.irq().trigger(Counter.IRQ_MATCH)

        out_dir_pin(COUNTER_DOWN)
        toggle(60_000)
        self.assertCounter(-60_000)
        self.assertEqual(callback_counter_value, -50_000)
        self.assertEqual(number_of_callbacks, 4)

        self.counter.irq().trigger(Counter.IRQ_MATCH)

    @unittest.skipIf(sys.platform == "mimxrt", "HERE not supported")
    def test_irq_HERE_DOWN_UP_DOWN(self):
        self.counter.init(in_pin, direction=in_dir_pin, match=60_000)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH | Counter.IRQ_HERE)

        out_dir_pin(COUNTER_DOWN)
        toggle(100)
        self.assertCounter(-100)

        out_dir_pin(COUNTER_UP)
        toggle(100 + 100)
        self.assertCounter(100)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, 0)
            self.assertEqual(number_of_callbacks, 1)

        out_dir_pin(COUNTER_DOWN)
        toggle(100 + 100)
        self.assertCounter(-100)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, 0)
            self.assertEqual(number_of_callbacks, 2)

        out_dir_pin(COUNTER_UP)
        toggle(100 + 100)
        self.assertCounter(100)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, 0)
            self.assertEqual(number_of_callbacks, 3)

        out_dir_pin(COUNTER_DOWN)
        toggle(70_000 + 100)
        self.assertCounter(-70_000)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, 0)
            self.assertEqual(number_of_callbacks, 4)

        out_dir_pin(COUNTER_UP)
        toggle(2 * 70_000)
        self.assertCounter(70_000)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, 60_000)
            self.assertEqual(number_of_callbacks, 6)

        out_dir_pin(COUNTER_DOWN)
        toggle(2 * 70_000)
        self.assertCounter(-70_000)
        if self.counter.irq().trigger():
            self.assertEqual(callback_counter_value, 0)
            self.assertEqual(number_of_callbacks, 8)

    @unittest.skipIf(sys.platform == "mimxrt", "HERE not supported")
    def test_irq_HERE_UP_DOWN_UP(self):
        global callback_counter_value
        self.counter.init(in_pin, direction=in_dir_pin, match=-90_000)
        self.counter.irq(handler=callback, trigger=Counter.IRQ_MATCH | Counter.IRQ_HERE)

        out_dir_pin(COUNTER_UP)
        toggle(100)
        self.assertCounter(100)

        out_dir_pin(COUNTER_DOWN)
        toggle(100)
        self.assertCounter(0)
        self.assertEqual(callback_counter_value, 0)
        self.assertEqual(number_of_callbacks, 1)

        toggle(95_000)
        self.assertCounter(-95_000)
        self.assertEqual(callback_counter_value, -90_000)
        self.assertEqual(number_of_callbacks, 2)

        callback_counter_value = 0
        out_dir_pin(COUNTER_UP)
        toggle(5_000)
        self.assertCounter(-90_000)
        self.assertEqual(callback_counter_value, -90_000)
        self.assertEqual(number_of_callbacks, 3)

        toggle(90_000 + 100)
        self.assertCounter(100)
        self.assertEqual(callback_counter_value, 0)
        self.assertEqual(number_of_callbacks, 4)


if __name__ == "__main__":
    unittest.main()
