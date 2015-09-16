import pyb
import os

"""
Execute it like this:

python3 run-tests --target wipy --device 192.168.1.1 ../cc3200/tools/smoke.py
"""

pin_map = [23, 24, 11, 12, 13, 14, 15, 16, 17, 22, 28, 10, 9, 8, 7, 6, 30, 31, 3, 0, 4, 5]
test_bytes = os.urandom(1024)

def test_pin_read (pull):
    # enable the pull resistor on all pins, then read the value
    for p in pin_map:
        pin = pyb.Pin('GP' + str(p), mode=pyb.Pin.IN, pull=pull)
        # read the pin value
        print(pin())

def test_pin_shorts (pull):
    if pull == pyb.Pin.PULL_UP:
        pull_inverted = pyb.Pin.PULL_DOWN
    else:
        pull_inverted = pyb.Pin.PULL_UP
    # enable all pulls of the specified type
    for p in pin_map:
        pin = pyb.Pin('GP' + str(p), mode=pyb.Pin.IN, pull=pull_inverted)
    # then change the pull one pin at a time and read its value
    i = 0
    while i < len(pin_map):
        pin = pyb.Pin('GP' + str(pin_map[i]), mode=pyb.Pin.IN, pull=pull)
        pyb.Pin('GP' + str(pin_map[i - 1]), mode=pyb.Pin.IN, pull=pull_inverted)
        i += 1
        # read the pin value
        print(pin())

test_pin_read(pyb.Pin.PULL_UP)
test_pin_read(pyb.Pin.PULL_DOWN)
test_pin_shorts(pyb.Pin.PULL_UP)
test_pin_shorts(pyb.Pin.PULL_DOWN)

# create a test directory
os.mkdir('/flash/test')
os.chdir('/flash/test')
print(os.getcwd())
# create a new file
f = open('test.txt', 'w')
n_w = f.write(test_bytes)
print(n_w == len(test_bytes))
f.close()
f = open('test.txt', 'r')
r = bytes(f.readall(), 'ascii')
# check that we can write and read it correctly
print(r == test_bytes)
f.close()
os.remove('test.txt')
os.chdir('..')
os.rmdir('test')

ls = os.listdir()
print('test' not in ls)
print(ls)

# test the real time clock
rtc = pyb.RTC()
while rtc.now()[6] > 800:
    pass

time1 = rtc.now()
pyb.delay(1000)
time2 = rtc.now()
print(time2[5] - time1[5] == 1)
print(time2[6] - time1[6] < 5000) # microseconds

