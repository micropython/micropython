import time
import micropython
from array import array

try:
    from esp32 import CAN
except ImportError:
    print("SKIP")
    raise SystemExit

# setup states
try:
    can = CAN(0)
    if can.state() != can.STOPPED:
        CAN(0).deinit()
except (ValueError, OSError):
    pass


def s2b(s):
    return [ord(x) for x in s]


can = CAN(0)
print(can)

# Test state when de-init'd
print(can.state() == can.STOPPED)

can.init(mode=CAN.SILENT_LOOPBACK)
print(can)
print(can.any())

# Test state when freshly created
print(can.state() == can.ERROR_ACTIVE)

# Test info returns a sensible value
print(can.info())

# Catch all filter
can.clearfilter()
can.send(s2b("A"), 123)

can.send([25], 123)
time.sleep_ms(10)
print(can.any(), can.info())
print(can.recv())

# Negative id
can.send(s2b("B"), -1)
print(can.recv())

# id overflow
can.send(s2b("C"), 0x7FF + 1)
print(can.recv())

# Test too long message
try:
    can.send(s2b("D") * 9, 0x7FF)
except ValueError:
    print("passed")
else:
    print("failed")

# Test that recv can work without allocating memory on the heap
buf = bytearray(10)
l = [0, 0, 0, memoryview(buf)]
l2 = None
message = s2b("E")

micropython.heap_lock()

can.send(message, 42)
l2 = can.recv(l)
assert l is l2
print(l, len(l[3]), buf)

message[0] = 0
can.send(message, 42)
l2 = can.recv(l)
assert l is l2
print(l, len(l[3]), buf)

micropython.heap_unlock()

# Test for non-list passed as second arg to recv
can.send(s2b("F"), 0)
try:
    can.recv(0)
except TypeError:
    print("TypeError")
    can.clear_tx_queue()

# Test for too-short-list passed as second arg to recv
can.send(s2b("F"), 1)
try:
    can.recv([0, 0, 0])
except ValueError:
    print("ValueError")
    can.clear_tx_queue()

# Test for non-memoryview passed as 4th element to recv
can.send(s2b("F"), 1)
try:
    can.recv([0, 0, 0, 0, 0])
except TypeError:
    print("TypeError")
    can.clear_tx_queue()

# Test for read-only-memoryview passed as 4th element to recv
can.send(s2b("F"), 1)
try:
    can.recv([0, 0, 0, memoryview(bytes(8))])
except ValueError:
    print("ValueError")
    can.clear_tx_queue()

# Test for bad-typecode-memoryview passed as 4th element to recv
can.send(s2b("F"), 1)
try:
    can.recv([0, 0, 0, memoryview(array("i", range(8)))])
except ValueError:
    print("ValueError")
    can.clear_tx_queue()

# Testing extended IDs
print("==== TEST extframe=True filter ====")

# Catch all filter, but only for extframe's
# can.setfilter(0, CAN.MASK32, 0, (0, 0), extframe=True)
can.setfilter(0, CAN.FILTER_ADDRESS, [0x101], extframe=True)

# matches filter
can.send(s2b("G"), 0x101, extframe=True)
time.sleep_ms(10)
print(can.any(), can.info())
print(can.recv())

# not matches filter
can.send(s2b("G"), 0x100, extframe=True)
time.sleep_ms(10)
print(can.any(), can.info())

print("==== TEST extframe=False filter ====")
can.setfilter(0, CAN.FILTER_ADDRESS, [0x101], extframe=False)

# matches filter
can.send(s2b("H"), 0x101, extframe=False)
time.sleep_ms(100)
print(can.any(), can.info())
print(can.recv())

# not matches filter
can.send(s2b("H"), 0x100, extframe=False)
time.sleep_ms(10)
print(can.any(), can.info())

# not matches filter (extframe)
can.send(s2b("H"), 0x100, extframe=True)
time.sleep_ms(10)
print(can.any(), can.info())

# Std filter with data bytes
can.setfilter(0, CAN.FILTER_ADDRESS, [0x101, int.from_bytes(b"J", "little")], extframe=False)

# matches filter
can.send(s2b("J"), 0x101, extframe=False)
time.sleep_ms(10)
print(can.any(), can.info())
print(can.recv())

# not matches filter bt content
can.send(s2b("1"), 0x101, extframe=False)
time.sleep_ms(10)
print(can.any(), can.info())

can.deinit()

print("==== TEST rx callbacks ====")

can = CAN(0, mode=CAN.SILENT_LOOPBACK, tx_queue=5, rx_queue=3)


def cb0(reason):
    if reason == 0:
        print("pending")
    if reason == 1:
        print("full")
    if reason == 2:
        print("overflow")


can.rxcallback(cb0)

can.send(s2b("K1"), 0x101, timeout=100)
time.sleep_ms(100)
can.send(s2b("K2"), 0x101, timeout=100)
time.sleep_ms(100)
can.send(s2b("K3"), 0x101, timeout=100)
time.sleep_ms(100)
can.send(s2b("K4"), 0x101, timeout=100)
time.sleep_ms(100)
print(can.any() and can.recv())
print(can.any() and can.recv())
print(can.any() and can.recv())
# rx_queue is 3, so the last one is None
print(can.any() and can.recv())

can.rxcallback(None)

# Testing asynchronous send
print("==== TEST async send ====")
can.send(s2b("L"), 1, timeout=0)
print(can.any())
while not can.any():
    pass

print(can.recv())

# Testing rtr messages
print("==== TEST rtr messages ====")
can.send([], 1, rtr=True)
time.sleep_ms(10)
print(can.any() and can.recv())

# test HAL error, timeout
print("==== TEST errors ====")

can = CAN(0, mode=CAN.NORMAL)
try:
    # can.send([], 1, timeout=10)
    can.recv(timeout=10)
except OSError as e:
    print(repr(e))
