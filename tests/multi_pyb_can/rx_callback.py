from pyb import CAN
import time
import errno

# Test the various receive IRQs, including overflow

rx_overflow = False

REASONS = ["received", "full", "overflow"]

# CAN IDs
ID_SPAM = 0x345  # messages spammed into the receive FIFO
ID_ACK_OFLOW = 0x055  # message the receiver sends after it's seen an overflow
ID_AFTER = 0x100  # message the sender sends after the ACK


def cb0(bus, reason):
    global rx_overflow
    if reason != 0 and not rx_overflow:
        # exact timing of 'received' callbacks depends on controller type,
        # so only log the other two
        print("rx0 reason", REASONS[reason])
    if reason == 2:
        rx_overflow = True


# Accept all standard IDs on FIFO 0
def _enable_accept_all():
    if hasattr(CAN, "MASK"):  # FD-CAN controller
        can.setfilter(0, CAN.RANGE, 0, (0x0, 0x7FF), extframe=False)
    else:
        can.setfilter(0, CAN.MASK16, 0, (0, 0, 0, 0), extframe=False)


# Receiver
def instance0():
    _enable_accept_all()
    can.rxcallback(0, cb0)

    multitest.next()
    multitest.wait("sender ready")
    multitest.broadcast("receiver ready")

    while not rx_overflow:
        pass  # Resume ASAP after FIFO0 overflows

    can.send(b"overflow", ID_ACK_OFLOW)

    # drain the receive FIFO, making sure we read at least on ID_SPAM message
    rxed_spam = False
    while can.any(0):
        msg = can.recv(0, timeout=0)
        assert msg[0] == ID_SPAM
        rxed_spam = True
    print("rxed_spam", rxed_spam)

    # This should be the one message with ID_AFTER, there may be one or two spam messages as well
    for _ in range(10):
        msg = can.recv(0, timeout=500)
        if msg[0] == ID_AFTER:
            print(msg)
            break

    # RX FIFO should be empty now
    print("any", can.any(0))


# Sender
def instance1():
    _enable_accept_all()
    multitest.next()
    multitest.broadcast("sender ready")
    multitest.wait("receiver ready")

    # Spam out messages until the receiver tells us its RX FIFO is full.
    #
    # The RX FIFO on the receiver can vary from 3 deep (BXCAN) to 25 deep (STM32H7),
    # so we keep sending to it until we see a CAN message on ID_ACK_OFLOW indicating
    # the receiver's FIFO has overflowed
    for i in range(255):
        can.send(bytes([i] * 8), ID_SPAM, timeout=25)
        if can.any(0):
            print(can.recv(0))  # should be ID_ACK_OFLOW
            break
        # on boards like STM32H7 the TX FIFO is really deep, so don't fill it too quickly...
        time.sleep_ms(1)

    # give the receiver some time to make space in the FIFO
    time.sleep_ms(200)

    # send the final message, the receiver should get this one
    can.send(b"aaaaa", ID_AFTER)

    # Sender's RX FIFO should also be empty at this point
    print("any", can.any(0))


can = CAN(1, CAN.NORMAL, baudrate=500_000, sample_point=75)
