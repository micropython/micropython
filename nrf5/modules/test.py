import time
from machine import PWM, Pin

def pulse():
    for i in range(0, 101):
        p = PWM(0, Pin("A17"), freq=PWM.FREQ_16MHZ, duty=i, period=16000)
        time.sleep_ms(10)
    for i in range(0, 101):
        p = PWM(0, Pin("A17"), freq=PWM.FREQ_16MHZ, duty=100-i, period=16000)
        time.sleep_ms(10)
