### ADC
import os
import time
from machine import PWM, ADC

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin_name = "P12_0"
    adc_pin = "P10_0"
elif "CY8CPROTO-063-BLE" in machine:
    pin_name = "P6_2"
    adc_pin = "P10_0"

adc = ADC(adc_pin, sample_ns=1000)
pwm = PWM(pin_name, freq=1000, duty_u16=65535, invert =0)

print("Initial Voltage (microvolts) is max: ",adc.read_uv()>1500)
print("Initial Voltage (raw data) is max: ",adc.read_u16()>50)
pwm.duty_u16(0)
time.sleep_ms(100)
print("Off signal Voltage (microvolts) is ~0 : ",adc.read_uv()<1500)
print("Off signal Voltage (raw data) is ~0 : ",adc.read_u16()<50)
pwm.duty_u16(65535)
time.sleep_ms(100)
print("On signal Voltage (microvolts) is max: ",adc.read_uv()>1500)
print("On signal Voltage (raw data) is max: ",adc.read_u16()>50)
