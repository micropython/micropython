import time
from machine import PWM, Pin

def pulse():
    for i in range(0, 101):
        p = PWM(0, Pin("P17", mode=Pin.OUT), freq=PWM.FREQ_16MHZ, duty=i, period=16000)
        p.init()
        time.sleep_ms(10)
        p.deinit()

    for i in range(0, 101):
        p = PWM(0, Pin("P17", mode=Pin.OUT), freq=PWM.FREQ_16MHZ, duty=100-i, period=16000)
        p.init()
        time.sleep_ms(10)
        p.deinit()
