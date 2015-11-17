'''
os module test for the CC3200 based boards
'''

from machine import SD
import os

mch = os.uname().machine
if 'LaunchPad' in mch:
    sd_pins = ('GP16', 'GP17', 'GP15')
elif 'WiPy' in mch:
    sd_pins = ('GP10', 'GP11', 'GP15')
else:
    raise Exception('Board not supported!')

sd = SD(pins=sd_pins)

os.mount(sd, '/sd')
os.mkfs('/sd')
os.chdir('/flash')
print(os.listdir())

os.chdir('/sd')
print(os.listdir())

# create a test directory in flash
os.mkdir('/flash/test')
os.chdir('/flash/test')
print(os.getcwd())
os.chdir('..')
print(os.getcwd())
os.chdir('test')
print(os.getcwd())
# create a new file
f = open('test.txt', 'w')
test_bytes = os.urandom(1024)
n_w = f.write(test_bytes)
print(n_w == len(test_bytes))
f.close()
f = open('test.txt', 'r')
r = bytes(f.readall(), 'ascii')
# check that we can write and read it correctly
print(r == test_bytes)
f.close()
os.rename('test.txt', 'newtest.txt')
print(os.listdir())
os.rename('/flash/test', '/flash/newtest')
print(os.listdir('/flash'))
os.remove('newtest.txt')
os.chdir('..')
os.rmdir('newtest')

# create a test directory in the sd card
os.mkdir('/sd/test')
os.chdir('/sd/test')
print(os.getcwd())
os.chdir('..')
print(os.getcwd())
os.chdir('test')
print(os.getcwd())
# create a new file
f = open('test.txt', 'w')
test_bytes = os.urandom(1024)
n_w = f.write(test_bytes)
print(n_w == len(test_bytes))
f.close()
f = open('test.txt', 'r')
r = bytes(f.readall(), 'ascii')
# check that we can write and read it correctly
print(r == test_bytes)
f.close()

print('CC3200' in os.uname().machine)
print('WiPy' == os.uname().sysname)

os.sync()
os.stat('/flash')
os.stat('/flash/sys')
os.stat('/flash/boot.py')
os.stat('/sd')
os.stat('/')
os.chdir('/sd/test')
os.remove('test.txt')
os.chdir('/sd')
os.rmdir('test')
os.listdir('/sd')
print(os.listdir('/'))
os.unmount('/sd')
print(os.listdir('/'))
os.mkfs(sd)
os.mount(sd, '/sd')
print(os.listdir('/'))
os.chdir('/flash')

# next ones must raise
sd.deinit()
try:
    os.listdir('/sd')
except:
    print('Exception')

#re-initialization must work
sd.init()
print(os.listdir('/sd'))

try:
    os.mount(sd, '/sd')
except:
    print('Exception')

try:
    os.mount(sd, '/sd2')
except:
    print('Exception')

os.unmount('/sd')
try:
    os.listdir('/sd')
except:
    print('Exception')

try:
    os.unmount('/flash')
except:
    print('Exception')

try:
    os.unmount('/something')
except:
    print('Exception')

try:
    os.unmount('something')
except:
    print('Exception')

try:
    os.mkfs('flash') # incorrect path format
except:
    print('Exception')

try:
    os.remove('/flash/nofile.txt')
except:
    print('Exception')

try:
    os.rename('/flash/nofile.txt', '/flash/nofile2.txt')
except:
    print('Exception')

try:
    os.chdir('/flash/nodir')
except:
    print('Exception')

try:
    os.listdir('/flash/nodir')
except:
    print('Exception')

os.mount(sd, '/sd')
print(os.listdir('/'))
os.unmount('/sd')
