### PWM
from machine import PWM

pwm = PWM("P13_7", freq=50, duty_u16=8192, invert=0)
print(pwm)
print(pwm.freq())
print(pwm.duty_u16())
pwm.duty_ns(1000)
print(pwm.duty_ns())
