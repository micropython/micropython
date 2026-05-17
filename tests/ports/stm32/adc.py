import sys
from pyb import ADC, Timer

if "STM32WB" in sys.implementation._machine:
    print("SKIP")
    raise SystemExit

adct = ADC(16)  # Temperature 930 -> 20C
print(str(adct)[:19])
adcv = ADC(17)  # Voltage 1500 -> 3.3V
print(adcv)

# read single sample; 2.5V-5V is pass range
val = adcv.read()
assert val > 1000 and val < 2000

# timer for read_timed
tim = Timer(5, freq=500)

# read into bytearray
buf = bytearray(b"\xff" * 50)
adcv.read_timed(buf, tim)
print(len(buf))
for i in buf:
    assert i > 50 and i < 150

# read into arrays with different element sizes
import array

arv = array.array("h", 25 * [0x7FFF])
adcv.read_timed(arv, tim)
print(len(arv))
for i in arv:
    assert i > 1000 and i < 2000

arv = array.array("i", 30 * [-1])
adcv.read_timed(arv, tim)
print(len(arv))
for i in arv:
    assert i > 1000 and i < 2000

# Test read_timed_multi
arv = bytearray(b"\xff" * 50)
art = bytearray(b"\xff" * 50)
ADC.read_timed_multi((adcv, adct), (arv, art), tim)
for i in arv:
    assert i > 60 and i < 125
# Wide range: unsure of accuracy of temp sensor.
for i in art:
    assert i > 15 and i < 200

arv = array.array("i", 25 * [-1])
art = array.array("i", 25 * [-1])
ADC.read_timed_multi((adcv, adct), (arv, art), tim)
for i in arv:
    assert i > 1000 and i < 2000
# Wide range: unsure of accuracy of temp sensor.
for i in art:
    assert i > 50 and i < 2000

arv = array.array("h", 25 * [0x7FFF])
art = array.array("h", 25 * [0x7FFF])
ADC.read_timed_multi((adcv, adct), (arv, art), tim)
for i in arv:
    assert i > 1000 and i < 2000
# Wide range: unsure of accuracy of temp sensor.
for i in art:
    assert i > 50 and i < 2000
