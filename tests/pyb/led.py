import pyb
from pyb import LED

for i in range(4):
    print(LED(i+1))

for i in range(4):
    LED(i+1).on()
pyb.delay(10)
for i in range(4):
    LED(i+1).off()
pyb.delay(10)
for i in range(4):
    LED(i+1).toggle()
pyb.delay(10)
for i in range(4):
    LED(i+1).intensity(0)

for i in range(256):
    LED(4).intensity(i)
    if LED(4).intensity() != i:
        print('fail', i)
    pyb.delay(1)
for i in range(256):
    LED(4).intensity(255 - i)
    pyb.delay(1)

for i in range(4):
    LED(i+1).off()
