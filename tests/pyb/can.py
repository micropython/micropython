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
