from pyb import CAN
import time
import errno

# Test for the filtering capabilities for RX FIFO 0 and 1.


# Receiver
def instance0():
    # Configure to receive standard frames (in a range) on FIFO 0
    # and extended frames (in a range) on FIFO 1.
    if hasattr(CAN, "MASK"):
        # FD-CAN has independent filter banks for standard and extended IDs
        can.setfilter(0, CAN.MASK, 0, (0x300, 0x700), extframe=False)
        can.setfilter(0, CAN.MASK, 1, (0x3000, 0x7000), extframe=True)
    else:
        # pyb.CAN only supports MASK32 for extended ids
        can.setfilter(0, CAN.MASK16, 0, (0x300, 0x700, 0x300, 0x700), extframe=False)
        can.setfilter(1, CAN.MASK32, 1, (0x3000, 0x7000), extframe=True)

    multitest.next()
    multitest.wait("sender ready")
    multitest.broadcast("receiver ready")
    for i in range(3):
        print(i)
        print("fifo0", can.recv(0, timeout=200))
        print("fifo1", can.recv(1, timeout=200))

    try:
        can.recv(0, timeout=100)  # should time out
    except OSError as e:
        assert e.errno == errno.ETIMEDOUT
        print("Timed out as expected")


# Sender
def instance1():
    multitest.next()
    multitest.broadcast("sender ready")
    multitest.wait("receiver ready")

    for i in range(3):
        print(i)
        can.send(bytes([i, 3] * i), 0x345)
        can.send(bytes([0xEE] * i), 0x3700 + i, extframe=True)
        can.send(b"abcdef", 0x123)  # matches no filter, expect ACKed but not received
        time.sleep_ms(5)  # avoid flooding either our or the receiver's FIFO


can = CAN(1, CAN.NORMAL, baudrate=500_000, sample_point=75)
