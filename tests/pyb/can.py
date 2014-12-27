from pyb import CAN

CAN.initfilterbanks(14)
can = CAN(1)
print(can)

can.init(CAN.LOOPBACK)
print(can)
print(can.any(0))

# Catch all filter
can.setfilter(0, CAN.MASK16, 0, (0, 0, 0, 0))

can.send('abcd', 123)
print(can.any(0))
print(can.recv(0))

can.send('abcd', -1)
print(can.recv(0))

can.send('abcd', 0x7FF + 1)
print(can.recv(0))

# Test too long message
try:
    can.send('abcdefghi', 0x7FF)
except ValueError:
    print('passed')
else:
    print('failed')

del can

# Testing extended IDs
can = CAN(1, CAN.LOOPBACK, extframe = True)
# Catch all filter
can.setfilter(0, CAN.MASK32, 0, (0, 0))

print(can)

try:
    can.send('abcde', 0x7FF + 1)
except ValueError:
    print('failed')
else:
    r = can.recv(0)
    if r[0] == 0x7FF+1 and r[3] == b'abcde':
        print('passed')
    else:
        print('failed, wrong data received')

del can

# Testing callbacks
from pyb import CAN

class callback():
    def cb10(bus):
        print('cb10')

    def cb11(bus):
        print('cb11')

    def cb20(bus):
        print('cb20')

    def cb21(bus):
        print('cb21')


bus1 = CAN(1, CAN.LOOPBACK)
bus2 = CAN(2, CAN.LOOPBACK)
bus1.setfilter(0, CAN.LIST16, 0, (1, 2, 3, 4))
bus1.setfilter(1, CAN.LIST16, 1, (5, 6, 7, 8))
bus2.setfilter(0, CAN.LIST16, 0, (1, 2, 3, 4))
bus2.setfilter(1, CAN.LIST16, 1, (5, 6, 7, 8))

bus1.rxcallback(0, callback.cb10)
bus1.rxcallback(1, callback.cb11)
bus2.rxcallback(0, callback.cb20)
bus2.rxcallback(1, callback.cb21)

bus1.send('one', 1)
bus1.send('five',5)
bus2.send('one',1)
bus2.send('five',5)

bus1.recv(0)
bus1.recv(1)
bus2.recv(0)
bus2.recv(1)

#Disabling fifo0 callbacks')
bus1.rxcallback(0, None)
bus2.rxcallback(0, None)

bus1.send('one',1)
bus1.send('five',5)
bus2.send('one',1)
bus2.send('five',5)

bus1.recv(0)
bus1.recv(1)
bus2.recv(0)
bus2.recv(1)

del bus1
del bus2