### PWM
import os
import time
from machine import PWM

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin_name = "P12_0"
elif "CY8CPROTO-063-BLE" in machine:
    pin_name = "P6_2"

def generate_new_signal(pwm_obj, freq, duty_cycle_u16):
    pwm_obj.freq(freq)
    pwm_obj.duty_u16(duty_cycle_u16)
    
pwm = PWM(pin_name, freq=50, duty_u16=32768, invert =0)

print("Initial freq: ", pwm.freq())
print("Initial duty cycle: ", pwm.duty_u16())

# Generate signal with freq = 1Hz and Duty cycle of 25%
generate_new_signal(pwm, 1, 16384)
print("Signal with freq = 1Hz and Duty cycle of 25%")
print("Current freq: ", pwm.freq())
print("Current duty cycle: ", pwm.duty_u16())

# Generate signal with freq = 1Hz and Duty cycle of 50%
generate_new_signal(pwm, 1, 32768)
print("Signal with freq = 1Hz and Duty cycle of 50%")
print("Current freq: ", pwm.freq())
print("Current duty cycle: ", pwm.duty_u16())

# Generate signal with freq = 1Hz and Duty cycle of 75%
generate_new_signal(pwm, 1, 49151)
print("Signal with freq = 1Hz and Duty cycle of 75%")
print("Current freq: ", pwm.freq())
print("Current duty cycle: ", pwm.duty_u16())

# Generate signal with freq = 1Hz and Duty cycle of 100%
generate_new_signal(pwm, 1, 65535)
print("Signal with freq = 1Hz and Duty cycle of 100%")
print("Current freq: ", pwm.freq())
print("Current duty cycle: ", pwm.duty_u16())

# Generate signal with freq = 25Hz and Duty cycle of 25%
generate_new_signal(pwm, 25, 16384)
print("Signal with freq = 25Hz and Duty cycle of 250%")
print("Current freq: ", pwm.freq())
print("Current duty cycle: ", pwm.duty_u16())

# Generate signal with freq = 50Hz and Duty cycle of 50%
generate_new_signal(pwm, 50, 32768)
print("Signal with freq = 50Hz and Duty cycle of 50%")
print("Current freq: ", pwm.freq())
print("Current duty cycle: ", pwm.duty_u16())

# Generate signal with freq = 75Hz and Duty cycle of 75%
generate_new_signal(pwm, 75, 49151)
print("Signal with freq = 75Hz and Duty cycle of 75%")
print("Current freq: ", pwm.freq())
print("Current duty cycle: ", pwm.duty_u16())

# Generate signal with freq = 100Hz and Duty cycle of 100%
generate_new_signal(pwm, 100, 65535)
print("Signal with freq = 100Hz and Duty cycle of 100%")
print("Current freq: ", pwm.freq())
print("Current duty cycle: ", pwm.duty_u16())