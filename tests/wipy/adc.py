'''
ADC test for the CC3200 based boards.
'''

from machine import ADC
import os

mch = os.uname().machine
if 'LaunchPad' in mch:
    adc_pin = 'GP5'
    adc_channel = 3
elif 'WiPy' in mch:
    adc_pin = 'GP3'
    adc_channel = 1
else:
    raise Exception('Board not supported!')

adc = ADC(0)
print(adc)
adc = ADC()
print(adc)
adc = ADC(0, bits=12)
print(adc)

apin = adc.channel(adc_channel)
print(apin)
apin = adc.channel(id=adc_channel)
print(apin)
apin = adc.channel(adc_channel, pin=adc_pin)
print(apin)
apin = adc.channel(id=adc_channel, pin=adc_pin)
print(apin)

print(apin.value() > 3000)
print(apin() > 3000)

# de-init must work
apin.deinit()
print(apin)

adc.deinit()
print(adc)
print(apin)
adc.init()
print(adc)
print(apin)
apin.init()
print(apin)
print(apin() > 3000)

# check for memory leaks...
for i in range (0, 1000):
    adc = ADC()
    apin = adc.channel(adc_channel)

# next ones should raise
try:
    adc = ADC(bits=17)
except:
    print('Exception')

try:
    adc = ADC(id=1)
except:
    print('Exception')

try:
    adc = ADC(0, 16)
except:
    print('Exception')

adc = ADC()
try:
    apin = adc.channel(4)
except:
    print('Exception')

try:
    apin = adc.channel(-1)
except:
    print('Exception')

try:
    apin = adc.channel(0, pin='GP3')
except:
    print('Exception')

apin = adc.channel(1)
apin.deinit()
try:
    apin()
except:
    print('Exception')

try:
    apin.value()
except:
    print('Exception')

adc.deinit()
try:
    apin.value()
except:
    print('Exception')

try:
    apin = adc.channel(1)
except:
    print('Exception')

# re-init must work
adc.init()
apin.init()
print(apin)
print(apin() > 3000)
