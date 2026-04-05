try:
    from pyb import CAN
except ImportError:
    print("SKIP")
    raise SystemExit

from array import array
import micropython
import pyb
import sys

# Classic CAN (aka bxCAN) hardware has a different filter API
# and some different behaviours to newer FDCAN hardware
IS_CLASSIC = hasattr(CAN, "MASK16")

# STM32H7 series has a gold-plated FDCAN peripheral with much deeper TX Queue
# than all other parts
IS_H7 = (not IS_CLASSIC) and "STM32H7" in str(sys.implementation)

# test we can correctly create by id (2 handled in can2.py test)
for bus in (-1, 0, 1, 3):
    try:
        CAN(bus, CAN.LOOPBACK)
        print("CAN", bus)
    except ValueError:
        print("ValueError", bus)
CAN(1).deinit()

can = CAN(1)
print(can)

# Test state when de-init'd
print(can.state() == can.STOPPED)

can.init(CAN.LOOPBACK, num_filter_banks=14)
print(can)
print("any", can.any(0))

# Test state when freshly created
print("error_active", can.state() == can.ERROR_ACTIVE)

# Test that restart can be called
can.restart()

# Test info returns a sensible value
print("info", can.info())

# Catch all filter (standard IDs)
if IS_CLASSIC:
    can.setfilter(0, CAN.MASK16, 0, (0, 0, 0, 0))
else:
    can.setfilter(0, CAN.MASK, 0, (0, 0), extframe=False)

can.send("abcd", 123, timeout=5000)
pyb.delay(10)  # For FDCAN, needs some time to send
print("any+info", can.any(0), can.info())
print(can.recv(0))

can.send("abcd", -1, timeout=5000)
print(can.recv(0))

can.send("abcd", 0x7FF + 1, timeout=5000)
print(can.recv(0))

# Test too long message
try:
    if IS_CLASSIC:
        payload = "abcdefghi"  # 9 bytes long
    else:
        # the pyb.CAN API for FDCAN always accepts messages up to 64 bytes and sends as FDCAN
        payload = b"x" * 65
    can.send(payload, 0x7FF, timeout=5000)
except ValueError:
    print("overlong passed")
else:
    print("overlong failed")

# Test that recv can work without allocating memory on the heap

buf = bytearray(10)
l = [0, 0, 0, 0, memoryview(buf)]
l2 = None

micropython.heap_lock()

can.send("", 42)
l2 = can.recv(0, l)
assert l is l2
print(l, len(l[4]), buf)

can.send("1234", 42)
l2 = can.recv(0, l)
assert l is l2
print(l, len(l[4]), buf)

can.send("01234567", 42)
l2 = can.recv(0, l)
assert l is l2
print(l, len(l[4]), buf)

can.send("abc", 42)
l2 = can.recv(0, l)
assert l is l2
print(l, len(l[4]), buf)

micropython.heap_unlock()

# Test that recv can work with different arrays behind the memoryview
can.send("abc", 1)
print(bytes(can.recv(0, [0, 0, 0, 0, memoryview(array("B", range(8)))])[4]))
can.send("def", 1)
print(bytes(can.recv(0, [0, 0, 0, 0, memoryview(array("b", range(8)))])[4]))

# Test for non-list passed as second arg to recv
can.send("abc", 1)
try:
    can.recv(0, 1)
except TypeError:
    print("TypeError")

# Test for too-short-list passed as second arg to recv
can.send("abc", 1)
try:
    can.recv(0, [0, 0, 0])
except ValueError:
    print("ValueError")

# Test for non-memoryview passed as 4th element to recv
can.send("abc", 1)
try:
    can.recv(0, [0, 0, 0, 0, 0])
except TypeError:
    print("TypeError")

# Test for read-only-memoryview passed as 4th element to recv
can.send("abc", 1)
try:
    can.recv(0, [0, 0, 0, memoryview(bytes(8))])
except ValueError:
    print("ValueError")

# Test for bad-typecode-memoryview passed as 4th element to recv
can.send("abc", 1)
try:
    can.recv(0, [0, 0, 0, memoryview(array("i", range(8)))])
except ValueError:
    print("ValueError")

del can

# Testing extended IDs
print("==== TEST extframe=True ====")

can = CAN(1, CAN.LOOPBACK)
# Catch all filter, but only for extframe's
if IS_CLASSIC:
    can.setfilter(0, CAN.MASK32, 0, (0, 0), extframe=True)
else:
    # FDCAN manages standard and extframe IDs independently, so need to
    # clear the standard ID filter and then set the extended ID filter
    can.clearfilter(0, extframe=False)
    can.setfilter(0, CAN.MASK, 0, (0, 0), extframe=True)

print(can)

try:
    can.send("abcde", 0x7FF + 1, timeout=5000, extframe=True)
except ValueError:
    print("failed")
else:
    pyb.delay(10)
    r = can.recv(0)
    if r[0] == 0x7FF + 1 and r[4] == b"abcde":
        print("extframe passed")
    else:
        print("failed, wrong data received", r)

# Test filters
for n in [0, 8, 16, 24]:
    filter_id = 0b00001000 << n
    filter_mask = 0b00011100 << n
    id_ok = 0b00001010 << n
    id_fail = 0b00011010 << n

    can.clearfilter(0, extframe=True)
    if IS_CLASSIC:
        can.setfilter(0, CAN.MASK32, 0, (filter_id, filter_mask), extframe=True)
    else:
        can.setfilter(0, CAN.MASK, 0, (filter_id, filter_mask), extframe=True)

    can.send("ok", id_ok, timeout=5, extframe=True)
    pyb.delay(10)
    if can.any(0):
        msg = can.recv(0)
        print((hex(filter_id), hex(filter_mask), hex(msg[0]), msg[1], msg[4]))

    can.send("fail", id_fail, timeout=5, extframe=True)
    pyb.delay(10)
    if can.any(0):
        msg = can.recv(0)
        print((hex(filter_id), hex(filter_mask), hex(msg[0]), msg[1], msg[4]))

del can

# Testing asynchronous send
print("==== TEST async send ====")

can = CAN(1, CAN.LOOPBACK)
# Catch all filter (standard IDs)
if IS_CLASSIC:
    can.setfilter(0, CAN.MASK16, 0, (0, 0, 0, 0))
else:
    can.setfilter(0, CAN.MASK, 0, (0, 0), extframe=False)

while can.any(0):
    can.recv(0)

can.send("abcde", 1, timeout=0)
print("any", can.any(0))
while not can.any(0):
    pass

print(can.recv(0))

try:
    can.send("abcde", 2, timeout=0)
    can.send("abcde", 3, timeout=0)
    can.send("abcde", 4, timeout=0)
    if not IS_H7:
        can.send("abcde", 5, timeout=0)
    else:
        # Hack around the STM32H7's deeper transmit queue by pretending this call failed
        # (STM32G4 will fail here, using otherwise the same code, so there is still some test coverage.)
        print("send fail ok")
except OSError as e:
    # When send() fails Classic CAN raises OSError(MP_EBUSY) (16), FDCAN raises OSError(MP_ETIMEDOUT) (110)
    if e.errno == (16 if IS_CLASSIC else 110):
        print("send fail ok")
    else:
        print("send fail not ok", e)

pyb.delay(500)
while can.any(0):
    print(can.recv(0))

del can

# Testing rtr messages
print("==== TEST rtr messages ====")

bus1 = CAN(1, CAN.LOOPBACK)
while bus1.any(0):
    bus1.recv(0)

if IS_CLASSIC:
    # pyb.CAN Classic API allows distinguishing between RTR in the filter
    bus1.setfilter(0, CAN.LIST16, 0, (1, 2, 3, 4))
    bus1.setfilter(1, CAN.LIST16, 0, (5, 6, 7, 8), rtr=(True, True, True, True))
    bus1.setfilter(2, CAN.MASK16, 0, (64, 64, 32, 32), rtr=(False, True))
else:
    # pyb.CAN FDCAN API does not allow distinguishing RTR in filter args, so
    # instead we'll only filter the message IDs where Classic CAN equivalent is
    # setting the RTR flag (meaning we're verifying RTR is received correctly, but
    # not verifying the missing filter behaviour.)
    bus1.setfilter(0, CAN.RANGE, 0, (5, 8))
    bus1.setfilter(1, CAN.MASK, 0, (32, 32))


def print_rtr(msg):
    if msg:
        # Skip printing msg[3] as this is the filter match index, and the value
        # is different between Classic and FDCAN implementations
        print(msg[0], msg[1], msg[2], msg[4])
    else:
        print(msg)


bus1.send("", 1, rtr=True)
print("any", bus1.any(0))
bus1.send("", 5, rtr=True)
print_rtr(bus1.recv(0))
bus1.send("", 6, rtr=True)
print_rtr(bus1.recv(0))
bus1.send("", 7, rtr=True)
print_rtr(bus1.recv(0))
bus1.send("", 16, rtr=True)
print("any", bus1.any(0))
bus1.send("", 32, rtr=True)
print_rtr(bus1.recv(0))

del bus1

# test HAL error, timeout
print("==== TEST errors ====")

# Note: this test requires no other CAN node is attached to the CAN peripheral
can = pyb.CAN(1, pyb.CAN.NORMAL)
try:
    if IS_CLASSIC:
        can.send("1", 1, timeout=50)
    else:
        # Difference between pyb.CAN on Classic vs FDCAN - Classic waits until the message is sent to the bus,
        # FDCAN only times out if the TX queue is full
        while True:
            can.send("1", 1, timeout=50)
except OSError as e:
    print("timeout", repr(e))
