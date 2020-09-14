# Dev by Sonthaya Nongnuch

from machine import Pin, PWM
from time import sleep

buzzer = PWM(Pin(13), freq=2000, duty=0)

def tone(freq=2093, duration=0.5, duty=50):
    buzzer.freq(freq)
    buzzer.duty(duty)
    sleep(duration)
    buzzer.duty(0)

def on(freq=2000, duty=50):
    buzzer.freq(freq)
    buzzer.duty(duty)

def off():
    buzzer.duty(0)
