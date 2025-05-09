# Verify that a thread running on CPU1 can go to lightsleep
# and wake up in the expected timeframe
import _thread
import time
import unittest
from machine import lightsleep

N_SLEEPS = 5
SLEEP_MS = 250

IDEAL_RUNTIME = N_SLEEPS * SLEEP_MS
MAX_RUNTIME = (N_SLEEPS + 1) * SLEEP_MS
MAX_DELTA = 20


class LightSleepInThread(unittest.TestCase):
    def thread_entry(self, is_thread=True):
        for _ in range(N_SLEEPS):
            lightsleep(SLEEP_MS)
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
        self.assertAlmostEqual(self.elapsed_ms(), IDEAL_RUNTIME, delta=MAX_DELTA)

    def test_cpu0_sleeping(self):
        _thread.start_new_thread(self.thread_entry, ())
        time.sleep_ms(MAX_RUNTIME)
        self.assertTrue(self.thread_done)
        self.assertAlmostEqual(self.elapsed_ms(), MAX_RUNTIME, delta=MAX_DELTA)

    def test_cpu0_also_lightsleep(self):
        _thread.start_new_thread(self.thread_entry, ())
        time.sleep_ms(50)  # account for any delay in starting the thread
        self.thread_entry(False)  # does the same lightsleep loop, doesn't set the done flag
        while not self.thread_done:
            time.sleep_ms(10)
        #
        # Only one thread can actually be in lightsleep at a time to avoid
        # races, but otherwise the behaviour when both threads call lightsleep()
        # is unspecified.
        #
        # Currently, the other thread will return immediately if one is already
        # in lightsleep. Therefore, runtime can be between IDEAL_RUNTIME and
        # IDEAL_RUNTIME * 2 depending on how many times the calls to lightsleep() race
        # each other.
        #
        # Note this test case is really only here to ensure that the rp2 hasn't
        # hung or failed to sleep at all - not to verify any correct behaviour
        # when there's a race to call lightsleep().
        self.assertGreaterEqual(self.elapsed_ms(), IDEAL_RUNTIME - MAX_DELTA)
        self.assertLessEqual(self.elapsed_ms(), IDEAL_RUNTIME * 2 + MAX_DELTA)


if __name__ == "__main__":
    unittest.main()
