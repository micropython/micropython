import time
from machine import PWM

def pulse():
    for i in range(0, 101):
        p = PWM(0, pin=17, freq=50, duty=i, period=16000)
        time.sleep_ms(10)
    for i in range(0, 101):
        p = PWM(0, pin=17, freq=50, duty=100-i, period=16000)
        time.sleep_ms(10)
