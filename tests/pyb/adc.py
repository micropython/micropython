from pyb import ADC

adc = ADC('X22')
print(adc)

# read single sample
val = adc.read()
assert val >= 0
assert val < 4095

# timer for read_timed
tim = pyb.Timer(5, freq=500)

# read into bytearray
buf = bytearray(50)
adc.read_timed(buf, tim)
print(len(buf))
for i in buf:
    assert i >= 0
    assert i < 4095

# read into arrays with different element sizes
import array
ar = array.array('h', 25 * [0])
adc.read_timed(ar, tim)
print(len(ar))
for i in buf:
    assert i >= 0
    assert i < 4095
ar = array.array('i', 30 * [0])
adc.read_timed(ar, tim)
print(len(ar))
for i in buf:
    assert i >= 0
    assert i < 4095
