import time
from machine import Timer

t = Timer(0)
t.init(period=2000, mode=Timer.ONE_SHOT, callback=lambda t: print("hello"))
time.sleep_ms(3000)
