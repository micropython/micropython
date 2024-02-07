import time
from machine import Timer
import os

# machine = os.uname().machine
# if "CY8CPROTO-063-BLE" in machine:
# TODO: Not working correctly. Neither the timer timing is correct.
# TODO: Review test and module.
print("SKIP")
raise SystemExit

t = Timer(0)
t.init(period=2000, mode=Timer.ONE_SHOT, callback=lambda t: print("Oneshot Timer"))
time.sleep(30)
t.deinit()


def blocking_delay_ms(delay_ms):
    start = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), start) < delay_ms:
        pass


t1 = Timer(0)
t1.init(period=2000, mode=Timer.PERIODIC, callback=lambda t: print("Periodic Timer"))
blocking_delay_ms(2000)
t.deinit()
