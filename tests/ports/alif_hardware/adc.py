# MIT license; Copyright (c) 2026 OpenMV LLC.
#
# Test the ADC12 peripherals: internal channels, and a pin driven by a loopback.
#
# IMPORTANT: This test requires hardware connections: the output and analog input
# pins in `adc_loopback_pins` must be wired together.
#
# requires: target-wiring

import time
from machine import ADC, Pin
from target_wiring import adc_loopback_pins

# Internal channels must read something non-zero (the internal reference can
# read close to full scale).
for name in ("CORE_VREF", "CORE_TEMP"):
    adc = ADC(getattr(ADC, name))
    print(name, adc.read_u16() > 0x0100)

# Drive the analog input low and high through the loopback.
out_pin, adc_pin = adc_loopback_pins[0]
pin_out = Pin(out_pin, Pin.OUT, value=0)
adc = ADC(Pin(adc_pin))
time.sleep_ms(1)
low = adc.read_u16()
pin_out.value(1)
time.sleep_ms(1)
high = adc.read_u16()
pin_out.init(Pin.IN)
print("loopback", low < 0x2000, high > 0x8000)

# Repeated conversions on the same channel must all complete.
print("repeat", all(0 <= adc.read_u16() <= 0xFFFF for _ in range(100)))
