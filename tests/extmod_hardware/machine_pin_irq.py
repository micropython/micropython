# Test machine.Pin.irq() method.
#
# IMPORTANT: This test requires hardware connections.

try:
    from machine import Pin

    Pin.irq
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit

import sys, time, unittest
from target_wiring import pin_loopback_pins

has_hardirq = sys.platform not in ("esp32",)

events_expected_rising = ["a", "b", 1, "c", "d", 1, "e", "f", "g", "h", "i"]
events_expected_falling = ["a", "b", "c", 0, "d", "e", 0, "f", "g", "h", "i"]
events_expected_rising_falling = ["a", "b", 1, "c", 0, "d", 1, "e", 0, "f", "g", "h", "i"]


class TestBase:
    @classmethod
    def setUpClass(cls):
        print("set up pins:", cls.p0_name, cls.p1_name)
        cls.p0 = Pin(cls.p0_name)
        cls.p1 = Pin(cls.p1_name)

    def setUp(self):
        self.p0.init(Pin.OUT)
        self.p1.init(Pin.IN)
        self.events = [0] * 100
        self.num_events = 0

    def tearDown(self):
        self.p0.init(Pin.IN)
        self.p1.init(Pin.IN)

    def add_event(self, data):
        self.events[self.num_events] = data
        self.num_events += 1

    def change_pin(self, value):
        self.p0(value)
        time.sleep(0)

    def irq_handler(self, p):
        if p is self.p1:
            self.add_event(p())
        else:
            print("expecting", self.p1)

    def _test_softirq(self, trigger, events_expected):
        # Uses time.sleep(0) to run the soft interrupt handler.
        self.num_events = 0

        self.change_pin(0)
        self.add_event("a")
        self.p1.irq(self.irq_handler, trigger)
        self.add_event("b")
        self.change_pin(1)
        self.add_event("c")
        self.change_pin(0)
        self.add_event("d")
        self.change_pin(1)
        self.add_event("e")
        self.change_pin(0)
        self.add_event("f")
        self.p1.irq(None)
        self.add_event("g")
        self.change_pin(1)
        self.add_event("h")
        self.change_pin(0)
        self.add_event("i")

        self.assertEqual(self.events[: self.num_events], events_expected)

    def _test_hardirq(self, trigger, events_expected):
        if not has_hardirq:
            self.skipTest("no hard IRQ")

        self.num_events = 0

        self.p0(0)
        self.add_event("a")
        self.p1.irq(self.irq_handler, trigger, hard=True)
        self.add_event("b")
        self.p0(1)
        self.add_event("c")
        self.p0(0)
        self.add_event("d")
        self.p0(1)
        self.add_event("e")
        self.p0(0)
        self.add_event("f")
        self.p1.irq(None)
        self.add_event("g")
        self.p0(1)
        self.add_event("h")
        self.p0(0)
        self.add_event("i")

        self.assertEqual(self.events[: self.num_events], events_expected)

    def test_rising_softirq(self):
        self._test_softirq(Pin.IRQ_RISING, events_expected_rising)

    def test_rising_hardirq(self):
        self._test_hardirq(Pin.IRQ_RISING, events_expected_rising)

    def test_falling_softirq(self):
        self._test_softirq(Pin.IRQ_FALLING, events_expected_falling)

    def test_falling_hardirq(self):
        self._test_hardirq(Pin.IRQ_FALLING, events_expected_falling)

    def test_rising_falling_softirq(self):
        self._test_softirq(Pin.IRQ_RISING | Pin.IRQ_FALLING, events_expected_rising_falling)

    def test_rising_falling_hardirq(self):
        self._test_hardirq(Pin.IRQ_RISING | Pin.IRQ_FALLING, events_expected_rising_falling)


# Generate test classes, one for each set of pins to test.
for p0_name, p1_name in pin_loopback_pins:
    cls_name = "Test_{}_{}".format(p0_name, p1_name)
    globals()[cls_name] = type(
        cls_name, (TestBase, unittest.TestCase), {"p0_name": p0_name, "p1_name": p1_name}
    )

if __name__ == "__main__":
    unittest.main()
