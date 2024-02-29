### ADC Functional test
""" Setup description: 
    Connect 3.3V input to adc_pin0 and 0V input to adc_pin1. With the setup done, run the test.

    *Known issue: The max output voltage currently is ~2.3V for 3.3V input.
"""
import os
import time
from machine import PWM, ADC

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    adc_pin0 = "P10_0"
    adc_pin1 = "P10_1"
    # TODO: Refactor test for more functionality coverage
    print("SKIP")
    raise SystemExit
elif "CY8CPROTO-063-BLE" in machine:
    adc_pin0 = "P10_0"
    adc_pin1 = "P10_1"
    # TODO: Refactor test for more functionality coverage
    print("SKIP")
    raise SystemExit

adc0 = ADC(adc_pin0, sample_ns=1000)
adc1 = ADC(adc_pin1, sample_ns=1000)

print("Voltage (in microvolts) on pin", adc_pin0, "is max: ", adc0.read_uv() > 1000000)
print("Voltage (raw count) on pin", adc_pin0, "is max: ", adc0.read_u16() > 500)
print("Voltage (in microvolts) on pin", adc_pin1, "is max: ", adc1.read_uv() < 1000000)
print("Voltage (raw count) on pin", adc_pin1, "is max: ", adc1.read_u16() < 500)
