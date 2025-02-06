# Verify that a thread running on CPU1 can go to lightsleep
# and wake up in the expected timeframe
import _thread
import time
import unittest
from machine import lightsleep, Pin

try:
    led = Pin(Pin.board.LED, Pin.OUT)
except AttributeError:
    led = None

N_SLEEPS = 5
SLEEP_MS = 250

IDEAL_RUNTIME = N_SLEEPS * SLEEP_MS
MAX_RUNTIME = (N_SLEEPS + 1) * SLEEP_MS


class LightSleepInThread(unittest.TestCase):
    def thread_entry(self, is_thread=True):
        led.toggle()
        for _ in range(N_SLEEPS):
            lightsleep(SLEEP_MS)
            led.toggle()
        if is_thread:
            self.thread_done = True

    def elapsed_ms(self):
        return time.ticks_diff(time.ticks_ms(), self.t0)

    def setUp(self):
        self.thread_done = False
        self.t0 = time.ticks_ms()

    def test_cpu0_busy(self):
        _thread.start_new_thread(self.thread_entry, ())
        # CPU0 is busy-waiting not asleep itself
        while not self.thread_done:
            self.assertLessEqual(self.elapsed_ms(), MAX_RUNTIME)
        self.assertAlmostEqual(self.elapsed_ms(), IDEAL_RUNTIME, delta=IDEAL_RUNTIME / 2)

    def test_cpu0_sleeping(self):
        _thread.start_new_thread(self.thread_entry, ())
        time.sleep_ms(MAX_RUNTIME)
        self.assertTrue(self.thread_done)
        self.assertAlmostEqual(self.elapsed_ms(), IDEAL_RUNTIME, delta=IDEAL_RUNTIME / 2)

    def test_cpu0_also_lightsleep(self):
        _thread.start_new_thread(self.thread_entry, ())
        time.sleep(0.050)  # account for any delay in starting the thread
        self.thread_entry(False)  # does the same lightsleep loop, doesn't set the done flag
        self.assertTrue(self.thread_done)
        # only one thread can actually be in lightsleep at a time to avoid races, so the total
        # runtime is doubled by doing it on both CPUs
        self.assertAlmostEqual(self.elapsed_ms(), IDEAL_RUNTIME * 2, delta=IDEAL_RUNTIME)


if __name__ == "__main__":
    unittest.main()
