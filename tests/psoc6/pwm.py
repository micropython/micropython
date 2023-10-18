### PWM
import os
from machine import PWM

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin_name = "P12_0"
elif "CY8CPROTO-063-BLE" in machine:
    pin_name = "P6_3"

pwm = PWM(pin_name, freq=50, duty_u16=8192, invert=0)
print(pwm)
print(pwm.freq())
print(pwm.duty_u16())
pwm.duty_ns(1000)
print(pwm.duty_ns())
