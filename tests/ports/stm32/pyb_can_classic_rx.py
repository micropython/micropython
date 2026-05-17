try:
    from pyb import CAN
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(CAN, "LIST16"):
    # This test relies on some specific behaviours of the Classic CAN RX FIFO
    # interrupt, so needs to be skipped on FDCAN hardware
    print("SKIP")
    raise SystemExit

# Test RxCallbacks
print("==== TEST rx callbacks ====")

can = CAN(1, CAN.LOOPBACK)
can.setfilter(0, CAN.LIST16, 0, (1, 2, 3, 4))
can.setfilter(1, CAN.LIST16, 1, (5, 6, 7, 8))


def cb0(bus, reason):
    print("cb0")
    if reason == 0:
        print("pending")
    if reason == 1:
        print("full")
    if reason == 2:
        print("overflow")


def cb1(bus, reason):
    print("cb1")
    if reason == 0:
        print("pending")
    if reason == 1:
        print("full")
    if reason == 2:
        print("overflow")


def cb0a(bus, reason):
    print("cb0a")
    if reason == 0:
        print("pending")
    if reason == 1:
        print("full")
    if reason == 2:
        print("overflow")


def cb1a(bus, reason):
    print("cb1a")
    if reason == 0:
        print("pending")
    if reason == 1:
        print("full")
    if reason == 2:
        print("overflow")


can.rxcallback(0, cb0)
can.rxcallback(1, cb1)

can.send("11111111", 1, timeout=5000)
can.send("22222222", 2, timeout=5000)
can.send("33333333", 3, timeout=5000)
can.rxcallback(0, cb0a)
can.send("44444444", 4, timeout=5000)

can.send("55555555", 5, timeout=5000)
can.send("66666666", 6, timeout=5000)
can.send("77777777", 7, timeout=5000)
can.rxcallback(1, cb1a)
can.send("88888888", 8, timeout=5000)

print(can.recv(0))
print(can.recv(0))
print(can.recv(0))
print(can.recv(1))
print(can.recv(1))
print(can.recv(1))

can.send("11111111", 1, timeout=5000)
can.send("55555555", 5, timeout=5000)

print(can.recv(0))
print(can.recv(1))

del can
