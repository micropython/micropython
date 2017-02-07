try:
    from pyb import CAN
except ImportError:
    print('SKIP')
    import sys
    sys.exit()

import pyb

# test we can correctly create by id or name
for bus in (-1, 0, 1, 2, 3, "YA", "YB", "YC"):
    try:
        CAN(bus, CAN.LOOPBACK)
        print("CAN", bus)
    except ValueError:
        print("ValueError", bus)
CAN(1).deinit()

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

# Test filters
for n in [0, 8, 16, 24]:
    filter_id = 0b00001000 << n
    filter_mask = 0b00011100 << n
    id_ok = 0b00001010 << n
    id_fail = 0b00011010 << n

    can.clearfilter(0)
    can.setfilter(0, pyb.CAN.MASK32, 0, (filter_id, filter_mask))

    can.send('ok', id_ok, timeout=3)
    if can.any(0):
        msg = can.recv(0)
        print((hex(filter_id), hex(filter_mask), hex(msg[0]), msg[3]))

    can.send("fail", id_fail, timeout=3)
    if can.any(0):
        msg = can.recv(0)
        print((hex(filter_id), hex(filter_mask), hex(msg[0]), msg[3]))

del can

# Test RxCallbacks
can = CAN(1, CAN.LOOPBACK)
can.setfilter(0, CAN.LIST16, 0, (1, 2, 3, 4))
can.setfilter(1, CAN.LIST16, 1, (5, 6, 7, 8))
def cb0(bus, reason):
    print('cb0')
    if reason == 0:
        print('pending')
    if reason == 1:
        print('full')
    if reason == 2:
        print('overflow')

def cb1(bus, reason):
    print('cb1')
    if reason == 0:
        print('pending')
    if reason == 1:
        print('full')
    if reason == 2:
        print('overflow')

def cb0a(bus, reason):
    print('cb0a')
    if reason == 0:
        print('pending')
    if reason == 1:
        print('full')
    if reason == 2:
        print('overflow')

def cb1a(bus, reason):
    print('cb1a')
    if reason == 0:
        print('pending')
    if reason == 1:
        print('full')
    if reason == 2:
        print('overflow')


can.rxcallback(0, cb0)
can.rxcallback(1, cb1)

can.send('11111111',1, timeout=5000)
can.send('22222222',2, timeout=5000)
can.send('33333333',3, timeout=5000)
can.rxcallback(0, cb0a)
can.send('44444444',4, timeout=5000)

can.send('55555555',5, timeout=5000)
can.send('66666666',6, timeout=5000)
can.send('77777777',7, timeout=5000)
can.rxcallback(1, cb1a)
can.send('88888888',8, timeout=5000)

print(can.recv(0))
print(can.recv(0))
print(can.recv(0))
print(can.recv(1))
print(can.recv(1))
print(can.recv(1))

can.send('11111111',1, timeout=5000)
can.send('55555555',5, timeout=5000)

print(can.recv(0))
print(can.recv(1))

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

bus1.send('',1,rtr=True)
print(bus1.any(0))
bus1.send('',5,rtr=True)
print(bus1.recv(0))
bus1.send('',6,rtr=True)
print(bus1.recv(0))
bus1.send('',7,rtr=True)
print(bus1.recv(0))
bus1.send('',16,rtr=True)
print(bus1.any(0))
bus1.send('',32,rtr=True)
print(bus1.recv(0))

bus2.send('',1,rtr=True)
print(bus2.recv(0))
bus2.send('',2,rtr=True)
print(bus2.recv(0))
bus2.send('',3,rtr=True)
print(bus2.recv(0))
bus2.send('',4,rtr=True)
print(bus2.any(0))
