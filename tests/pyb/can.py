from pyb import CAN

can = CAN(1)
print(can)
can.init(CAN.LOOPBACK)
print(can.any(0))
can.send('abcd', 123)
print(can.any(0))
print(can.recv(0))
