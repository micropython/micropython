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

# Testing rtr messages
bus1 = CAN(1, CAN.LOOPBACK)
bus2 = CAN(2, CAN.LOOPBACK, extframe = True)
while bus1.any(0):
    bus1.recv(0)
while bus2.any(0):
    bus2.recv(0)
bus1.setfilter(0, CAN.LIST16, 0, (1, 2, 3, 4))
bus1.setfilter(1, CAN.LIST16, 0, (5, 6, 7, 8), rtr=(True, True, True, True))
bus1.setfilter(2, CAN.MASK16, 0, (64, 64, 32, 32), rtr=(False, True))
bus2.setfilter(0, CAN.LIST32, 0, (1, 2), rtr=(True, True))
bus2.setfilter(1, CAN.LIST32, 0, (3, 4), rtr=(True, False))
bus2.setfilter(2, CAN.MASK32, 0, (16, 16), rtr=(False,))
bus2.setfilter(2, CAN.MASK32, 0, (32, 32), rtr=(True,))

bus1.rtr(1)
print(bus1.any(0))
bus1.rtr(5)
print(bus1.recv(0))
bus1.rtr(6)
print(bus1.recv(0))
bus1.rtr(7)
print(bus1.recv(0))
bus1.rtr(16)
print(bus1.any(0))
bus1.rtr(32)
print(bus1.recv(0))

bus2.rtr(1)
print(bus2.recv(0))
bus2.rtr(2)
print(bus2.recv(0))
bus2.rtr(3)
print(bus2.recv(0))
bus2.rtr(4)
print(bus2.any(0))
