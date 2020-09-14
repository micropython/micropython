# Dev by Sonthaya Nongnuch

from machine import Pin, PWM

SV1 = PWM(Pin(15), freq=50)
SV2 = PWM(Pin(17), freq=50)

def angle(n, angle):
    n.duty(int(25.57 + ((angle / 180.0) * 102.3)))
