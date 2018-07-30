from pyb import Pin, ADCAll

pins = [Pin.cpu.A0, Pin.cpu.A1, Pin.cpu.A2, Pin.cpu.A3]

# set pins to IN mode, init ADCAll, then check pins are ANALOG
for p in pins:
    p.init(p.IN)
adc = ADCAll(12)
for p in pins:
    print(p)

# set pins to IN mode, init ADCAll with mask, then check some pins are ANALOG
for p in pins:
    p.init(p.IN)
adc = ADCAll(12, 0x70003)
for p in pins:
    print(p)

# init all pins to ANALOG
adc = ADCAll(12)
print(adc)

# read all channels
for c in range(19):
    print(type(adc.read_channel(c)))

# call special reading functions
print(0 < adc.read_core_temp() < 100)
print(0 < adc.read_core_vbat() < 4)
print(0 < adc.read_core_vref() < 2)
print(0 < adc.read_vref() < 4)
