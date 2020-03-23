try:
    from pyb import CAN

    CAN(2)
except (ImportError, ValueError):
    print("SKIP")
    raise SystemExit

# Testing rtr messages
bus2 = CAN(2, CAN.LOOPBACK, extframe=True)
while bus2.any(0):
    bus2.recv(0)
bus2.setfilter(0, CAN.LIST32, 0, (1, 2), rtr=(True, True))
bus2.setfilter(1, CAN.LIST32, 0, (3, 4), rtr=(True, False))
bus2.setfilter(2, CAN.MASK32, 0, (16, 16), rtr=(False,))
bus2.setfilter(2, CAN.MASK32, 0, (32, 32), rtr=(True,))

bus2.send("", 1, rtr=True)
print(bus2.recv(0))
bus2.send("", 2, rtr=True)
print(bus2.recv(0))
bus2.send("", 3, rtr=True)
print(bus2.recv(0))
bus2.send("", 4, rtr=True)
print(bus2.any(0))
