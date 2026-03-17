try:
    from pyb import CAN
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(CAN, "LIST16"):
    # This test relies on the RTR-aware filters of Classic CAN,
    # so needs to be skipped on FDCAN hardware
    print("SKIP")
    raise SystemExit

can = CAN(1, CAN.LOOPBACK)
while can.any(0):
    can.recv(0)
can.setfilter(0, CAN.LIST32, 0, (1, 2), rtr=(True, True), extframe=True)
can.setfilter(1, CAN.LIST32, 0, (3, 4), rtr=(True, False), extframe=True)
can.setfilter(2, CAN.MASK32, 0, (16, 16), rtr=(False,), extframe=True)
can.setfilter(2, CAN.MASK32, 0, (32, 32), rtr=(True,), extframe=True)

can.send("", 1, rtr=True, extframe=True)
print(can.recv(0))
can.send("", 2, rtr=True, extframe=True)
print(can.recv(0))
can.send("", 3, rtr=True, extframe=True)
print(can.recv(0))
can.send("", 4, rtr=True, extframe=True)
print(can.any(0))

del can
