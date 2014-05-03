from pyb import ADC
from pyb import Pin

adc = ADC('X22')
print(adc)

adc.read()

buf = bytearray(100)
adc.read_timed(buf, 500)
