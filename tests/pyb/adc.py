from pyb import ADC, Timer

adct = ADC(16) # Temperature 930 -> 20C
print(adct)
adcv = ADC(17) # Voltage 1500 -> 3.3V
print(adcv)

# read single sample; 2.5V-5V is pass range
val = adcv.read()
assert val > 1000 and val < 2000

# timer for read_timed
tim = Timer(5, freq=500)

# read into bytearray
buf = bytearray(b'\xff' * 50)
adcv.read_timed(buf, tim)
print(len(buf))
for i in buf:
    assert i > 50 and i < 150

# read into arrays with different element sizes
import array
arv = array.array('h', 25 * [0x7fff])
adcv.read_timed(arv, tim)
print(len(arv))
for i in arv:
    assert i > 1000 and i < 2000

arv = array.array('i', 30 * [-1])
adcv.read_timed(arv, tim)
print(len(arv))
for i in arv:
    assert i > 1000 and i < 2000
