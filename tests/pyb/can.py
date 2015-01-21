from pyb import CAN

CAN.initfilterbanks(14)
can = CAN(1)
print(can)

can.init(CAN.LOOPBACK)
print(can)
print(can.any(0))

# Catch all filter
can.setfilter(0, CAN.MASK16, 0, (0, 0, 0, 0))

can.send('abcd', 123, timeout=5000)
print(can.any(0))
print(can.recv(0))

can.send('abcd', -1, timeout=5000)
print(can.recv(0))

can.send('abcd', 0x7FF + 1, timeout=5000)
print(can.recv(0))

# Test too long message
try:
    can.send('abcdefghi', 0x7FF, timeout=5000)
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
    can.send('abcde', 0x7FF + 1, timeout=5000)
except ValueError:
    print('failed')
else:
    r = can.recv(0)
    if r[0] == 0x7FF+1 and r[3] == b'abcde':
        print('passed')
    else:
        print('failed, wrong data received')
del can

# Testing asyncronous send
can = CAN(1, CAN.LOOPBACK)
can.setfilter(0, CAN.MASK16, 0, (0, 0, 0, 0))

while can.any(0):
    can.recv(0)

can.send('abcde', 1, timeout=0)
print(can.any(0))
while not can.any(0):
    pass

print(can.recv(0))

try:
    can.send('abcde', 2, timeout=0)
    can.send('abcde', 3, timeout=0)
    can.send('abcde', 4, timeout=0)
    can.send('abcde', 5, timeout=0)
except OSError as e:
    if str(e) == '16':
        print('passed')
    else:
        print('failed')

pyb.delay(500)
while can.any(0):
    print(can.recv(0))

