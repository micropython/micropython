import pyb
import os

"""
Execute it like this:

python3 run-tests --target wipy --device 192.168.1.1 ../cc3200/tools/smoke.py
"""

pin_map = [2, 1, 23, 24, 11, 12, 13, 14, 15, 16, 17, 22, 28, 10, 9, 8, 7, 6, 30, 31, 3, 0, 4, 5]
test_bytes = os.urandom(2048)

def test_pin_read (type):
    # enable the pull resistor on all pins, then read the value
    for p in pin_map:
        pin = pyb.Pin('GP' + str(p), af= 0, mode=pyb.Pin.IN, type=type)
        # read the pin value
        print (pin.value())

def test_pin_shorts (type):
    if type == pyb.Pin.STD_PU:
        type_inverted = pyb.Pin.STD_PD
    else:
        type_inverted = pyb.Pin.STD_PU
    # enable all pulls of the specified type
    for p in pin_map:
        pin = pyb.Pin('GP' + str(p), af= 0, mode=pyb.Pin.IN, type=type_inverted)
    # then change the pull one pin at a time and read its value
    i = 0
    while i < len(pin_map):
        pin = pyb.Pin('GP' + str(pin_map[i]), af= 0, mode=pyb.Pin.IN, type=type)
        pyb.Pin('GP' + str(pin_map[i - 1]), af= 0, mode=pyb.Pin.IN, type=type_inverted)
        i += 1
        # read the pin value
        print (pin.value())

test_pin_read(pyb.Pin.STD_PU)
test_pin_read(pyb.Pin.STD_PD)
test_pin_shorts(pyb.Pin.STD_PU)
test_pin_shorts(pyb.Pin.STD_PD)

# create a test directory
os.mkdir('/flash/test')
os.chdir('/flash/test')
print(os.getcwd())
# create a new file
f = open ('test.txt', 'w')
n_w = f.write (test_bytes)
print (n_w == len(test_bytes))
f.close()
f = open('test.txt', 'r')
r = bytes(f.readall(), 'ascii')
# check that we can write and read it correctly
print (r == test_bytes)
f.close()
os.remove('test.txt')
os.chdir('..')
os.rmdir('test')

ls = os.listdir()
print('test' not in ls)
print(ls)

