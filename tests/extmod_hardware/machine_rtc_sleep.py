# Test for RTC ticks during normal sleep and lightsleep.

try:
    from machine import RTC, lightsleep
except ImportError:
    print("SKIP")
    raise SystemExit

import sys, time

if "rp2" in sys.platform:
    pass
else:
    # At time of writing most other ports with machine.RTC and
    # machine.lightsleep() aren't capable of passing this test due to
    # inconsistent or broken lightsleep behaviour... If you find another port
    # which can pass the test, please add it above!
    print("SKIP")
    raise SystemExit


_IDX_MINUTE = const(5)
_IDX_SECOND = const(6)

_SLEEP_SECS = const(2)

rtc = RTC()


def test_sleep(sleep_fn, label):
    attempts = 0
    while True:
        attempts += 1
        rtc0 = rtc.datetime()
        t0 = time.ticks_ms()
        sleep_fn()
        rtc1 = rtc.datetime()
        t1 = time.ticks_ms()
        if attempts < 3 and rtc0[_IDX_SECOND] - rtc1[_IDX_SECOND] == _SLEEP_SECS + 1:
            # allow the possibility that occasionally the sleep runs a few milliseconds long and
            # the RTC ticks over an additional second in that small window.
            continue
        if rtc0[_IDX_MINUTE] == rtc1[_IDX_MINUTE]:
            # To avoid having to make datetime calculations, re-run the sleep until it starts
            # and ends in the same minute!
            break

    sec0 = rtc0[_IDX_SECOND]
    sec1 = rtc1[_IDX_SECOND]
    ticks_diff = time.ticks_diff(t1, t0)

    if sec1 - sec0 == _SLEEP_SECS and abs(_SLEEP_SECS * 1000 - ticks_diff) < 100:
        print(label, "OK")
    else:
        print(label, "Bad sleep time")
        print("t0", t0)
        print("t1", t1)
        print("ticks_diff", ticks_diff)
        print("rtc0", rtc0)
        print("rtc1", rtc1)


def force_lightsleep(target_ms):
    # lightsleep for at least target_ms, not only until the next interrupt
    t_end = time.ticks_add(time.ticks_ms(), target_ms)
    remaining = time.ticks_diff(t_end, time.ticks_ms())
    while remaining > 0:
        lightsleep(remaining)
        remaining = time.ticks_diff(t_end, time.ticks_ms())


test_sleep(lambda: time.sleep(_SLEEP_SECS), "time.sleep")
test_sleep(lambda: time.sleep_ms(_SLEEP_SECS * 1000), "time.sleep_ms")
test_sleep(lambda: force_lightsleep(_SLEEP_SECS * 1000), "forced machine.lightsleep")
