"""
ADCBlock test for the PSoC6 port.
"""
import os
from machine import Pin, ADCBlock

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin_name = "P10_3"
elif "CY8CPROTO-063-BLE" in machine:
    print("SKIP")
    raise SystemExit

# Negative tests
try:
    adcBlock = ADCBlock(1)
except:
    print("TypeError: Specified ADC id not supported. Currently only block 0 is configured!")

try:
    adcBlock = ADCBlock(0, bits=10)
except:
    print("TypeError: Invalid bits. Current ADC configuration supports only 12 bits resolution!")

adcBlock = ADCBlock(0)

try:
    adcPin = adcBlock.connect(0, pin_name)
except:
    print("TypeError: Wrong pin specified for the mentioned channel")


adcPin = adcBlock.connect(3, pin_name)

print(adcPin.read_uv() > 0)
