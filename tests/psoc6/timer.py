import time
from machine import Timer

t = Timer(0)
t.init(period=2000, mode=Timer.ONE_SHOT, callback=lambda t: print("Oneshot Timer"))
time.sleep(30)
t.deinit()
t1 = Timer(0)
t1.init(period=2000, mode=Timer.PERIODIC, callback=lambda t: print("Periodic Timer"))
for i in range(200000):
    pass

t.deinit()

