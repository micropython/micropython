"""
ADC test for the PSoC6 port.
"""

from machine import ADC, Pin

adc_pin = Pin("P10_0")
adc_wrong_pin = Pin("P13_7")

# Exceptions should be raised
try:
    adc = ADC(adc_wrong_pin)
except:
    print("Invalid ADC Pin")

adc = ADC(adc_pin, sample_ns=1000)
print(adc)

print(adc.read_u16() > 0)
print(adc.read_uv() > 0)
