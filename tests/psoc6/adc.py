"""
ADC test for the PSoC6 port.
"""

import os
from machine import ADC, Pin

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    adc_pin_name = "P10_0"
    adc_wrong_pin_name = "P13_7"
elif "CY8CPROTO-063-BLE" in machine:
    print("SKIP")
    raise SystemExit

adc_pin = Pin(adc_pin_name)
adc_wrong_pin = Pin(adc_wrong_pin_name)

# Exceptions should be raised
try:
    adc = ADC(adc_wrong_pin)
except:
    print("Invalid ADC Pin")

adc = ADC(adc_pin, sample_ns=1000)
print(adc)

print(adc.read_u16() > 0)
print(adc.read_uv() > 0)
