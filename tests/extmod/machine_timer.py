try:
    from machine import Timer
    from time import sleep_ms
except:
    print("SKIP")
    raise SystemExit

import sys

if sys.platform == "nrf":
    # Note: The nrf port supports machine.Timer, but is not compatible: It lacks
    # the .init() method and freq argument, the period argument is microseconds
    # instead milliseconds, and the ONE_SHOT constant is named ONESHOT.
    print("SKIP")
    raise SystemExit

import unittest

# Hardware timers are only supported on the esp32 port
SUPPORTS_HARDWARE_TIMERS = sys.platform == "esp32"

# Hard IRQs are not supported on the esp32 port
SUPPORTS_HARD_IRQ = sys.platform != "esp32"


class Test(unittest.TestCase):
    def test_virtual_create(self):
        self._test_create(-1)
        self._test_create_multiple(-1, -1)

    @unittest.skipUnless(SUPPORTS_HARDWARE_TIMERS, "no hardware timers")
    def test_hardware_create(self):
        self._test_create(0)
        self._test_create_multiple(0, 1)

    def test_virtual_softirq(self):
        self._test_all_freq_period(-1, Timer.ONE_SHOT, False)
        self._test_all_freq_period(-1, Timer.PERIODIC, False)

    @unittest.skipUnless(SUPPORTS_HARD_IRQ, "no hard-irq support")
    def test_virtual_hardirq(self):
        self._test_all_freq_period(-1, Timer.ONE_SHOT, True)
        self._test_all_freq_period(-1, Timer.PERIODIC, True)

    @unittest.skipUnless(SUPPORTS_HARDWARE_TIMERS, "no hardware timers")
    def test_hardware_softirq(self):
        self._test_all_freq_period(0, Timer.ONE_SHOT, False)
        self._test_all_freq_period(0, Timer.PERIODIC, False)

    @unittest.skipUnless(SUPPORTS_HARDWARE_TIMERS, "no hardware timers")
    @unittest.skipUnless(SUPPORTS_HARD_IRQ, "no hard-irq support")
    def test_hardware_hardirq(self):
        self._test_all_freq_period(0, Timer.ONE_SHOT, True)
        self._test_all_freq_period(0, Timer.PERIODIC, True)

    def _test_create(self, id):
        # create and deinit
        t = Timer(id)
        t.init(freq=1)
        t.deinit()

        # deinit again
        t.deinit()

        # init a large number of times to catch bugs like
        # https://github.com/micropython/micropython/issues/19162
        for _ in range(256):
            t.init(freq=1)
        t.deinit()

    def _test_create_multiple(self, *ids):
        # create and deinit
        timers = []
        for id in ids:
            t = Timer(id)
            self.assertFalse(t in timers)
            t.init(freq=1)
            timers.append(t)
        for t in timers:
            t.deinit()

        # create and deinit in reverse order
        timers = []
        for id in ids:
            t = Timer(id)
            self.assertFalse(t in timers)
            t.init(freq=1)
            timers.append(t)
        for t in reversed(timers):
            t.deinit()

    def _test_all_freq_period(self, id, mode, hard):
        # test two different freq and period arguments
        for period, freq_period_arg in (
            (1000 // 50, {"freq": 50}),
            (1000 // 25, {"freq": 25}),
            (20, {"period": 20}),
            (40, {"period": 40}),
        ):
            callback_results = []

            def _callback(_):
                try:
                    allocate = bytearray(1)
                    locked = False
                except MemoryError:
                    locked = True
                callback_results.append(locked)

            t = Timer(id)
            t.init(
                mode=mode,
                callback=_callback,
                hard=hard,
                **freq_period_arg,
            )

            # Note: These sleep durations are such that there is always at least
            # 10ms between when the callback fires and we perform our checks,
            # thus this test also implicitly checks that timers and/or sleep_ms
            # do not drift more than 10ms.
            total = 0
            for duration in (0, 10, 20, 40, 20):
                sleep_ms(duration)
                total += duration

                # number of times the callback should have been called
                n = total // period
                if mode == Timer.ONE_SHOT:
                    n = min(n, 1)

                # callback reports whether memory was locked, which
                # should equal whether the timer uses hard IRQs
                self.assertEqual([hard] * n, callback_results)
            t.deinit()


if __name__ == "__main__":
    unittest.main()
