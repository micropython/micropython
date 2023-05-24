### PWM
from machine import PWM

pwm = PWM("P13_7", freq=2, duty_us=32678)
print(pwm)
pwm.freq()
pwm.duty_16()
