# Dev by Sonthaya Nongnuch

from machine import Pin, PWM

SV1 = Pin(15)
SV2 = Pin(17)

def angle(pin, angle):
    PWM(pin, freq=50).duty(int(25.57 + ((angle / 180.0) * 102.3)))
