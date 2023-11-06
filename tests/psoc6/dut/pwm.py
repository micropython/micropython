### PWM - WIP
""" Setup description: 
    Connect pwm_pin to gpio_pin. PWM signal is generated at 50% duty cycle currently. Every time the level changes, the gpio_pin 
    is triggered and elapsed tick is calculated. Based on values ton and toff, experimental duty cycle is calculated.

    *Known issue: This test will not work for any duty cycle except for 65535 or 0 values since there are fixes required in module.
"""
import os
import time
from machine import PWM, Pin
import time

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pwm_pin = "P12_0"
    gpio_pin = "P13_6"

elif "CY8CPROTO-063-BLE" in machine:
    pwm_pin = "P6_2"
    gpio_pin = "P5_2"

gpio_flag = 0

input_pin = Pin(gpio_pin, Pin.IN)

t0 = time.ticks_cpu()

pwm = PWM(pwm_pin, freq=10, duty_u16=32768, invert=0)

while gpio_flag != 1:
    if input_pin.value() == 1:
        gpio_flag = 1
        t1 = time.ticks_cpu()
        toff = t1 - t0

while gpio_flag != 0:
    if input_pin.value() == 0:
        gpio_flag = 0
        t2 = time.ticks_cpu()
        ton = t2 - t1

duty_cycle = (ton / (ton + toff)) * 100

print("Experimental duty cycle(%) = ", duty_cycle)
