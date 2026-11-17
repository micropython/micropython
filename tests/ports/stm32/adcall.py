import sys
from pyb import Pin, ADCAll

# This test requires PA0-PA3 with ADC, so can't run on:
# - boards with UART REPL on PA2/PA3
# - STM32N6 which does not have ADC on PA3
if (
    sys.implementation._build in ("NUCLEO_G0B1RE", "NUCLEO_L152RE")
    or "STM32N6" in sys.implementation._machine
):
    print("SKIP")
    raise SystemExit

pins = [Pin.cpu.A0, Pin.cpu.A1, Pin.cpu.A2, Pin.cpu.A3]
pa0_adc_channel = 0
skip_temp_test = False

if "STM32WB" in sys.implementation._machine:
    pa0_adc_channel = 5
    skip_temp_test = True  # temperature fails on WB55
elif "STM32H7" in sys.implementation._machine:
    # ADCAll is connected to ADC3 with the following pins.
    pins = [Pin.cpu.F9, Pin.cpu.F7, Pin.cpu.F5, Pin.cpu.F3]
    pa0_adc_channel = 2

# set pins to IN mode, init ADCAll, then check pins are ANALOG
for p in pins:
    p.init(p.IN)
adc = ADCAll(12)
for p in pins:
    print(p.mode())

# set pins to IN mode, init ADCAll with mask, then check some pins are ANALOG
for p in pins:
    p.init(p.IN)
adc = ADCAll(12, 0x70000 | 3 << pa0_adc_channel)
for p in pins:
    print(p.mode())

# init all pins to ANALOG
adc = ADCAll(12)
print(adc)

# read all channels
for c in range(19):
    print(type(adc.read_channel(c)))

# call special reading functions
print(skip_temp_test or 0 < adc.read_core_temp() < 100)
print(0 < adc.read_core_vbat() < 4)
print(0 < adc.read_core_vref() < 2)
print(0 < adc.read_vref() < 4)

if sys.implementation._build == "NUCLEO_WB55":
    # Restore button pin settings.
    Pin("SW", Pin.IN, Pin.PULL_UP)
