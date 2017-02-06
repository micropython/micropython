from pyb import ADC
from pyb import Pin

pin = Pin('X22', mode=Pin.IN, pull=Pin.PULL_DOWN)
adc = ADC('X22')
print(adc)

# read single sample
val = adc.read()
assert val < 500

# timer for read_timed
tim = pyb.Timer(5, freq=500)

# read into bytearray
buf = bytearray(50)
adc.read_timed(buf, tim)
print(len(buf))
for i in buf:
    assert i < 500

# read into arrays with different element sizes
import array
ar = array.array('h', 25 * [0])
adc.read_timed(ar, tim)
print(len(ar))
for i in buf:
    assert i < 500
ar = array.array('i', 30 * [0])
adc.read_timed(ar, tim)
print(len(ar))
for i in buf:
    assert i < 500
