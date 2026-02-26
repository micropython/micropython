from machine import CAN
import time

# Test CAN remote transmission requests

ID = 0x750

FILTER_ID = 0x101
FILTER_MASK = 0xFFF

can = CAN(1, 500_000)


def receiver_irq_recv(can):
    assert can.irq().flags() & can.IRQ_RX  # the only enabled IRQ
    can_id, data, flags, _errors = can.recv()
    is_rtr = flags == CAN.FLAG_RTR
    print(
        "recv",
        hex(can_id),
        is_rtr,
        len(data) if is_rtr else bytes(data),
    )
    if is_rtr:
        # The 'data' response of a remote request should be all zeroes
        assert bytes(data) == b"\x00" * len(data)


# Receiver
def instance0():
    can.irq(receiver_irq_recv, trigger=can.IRQ_RX, hard=False)
    can.set_filters([])  # receive all

    multitest.next()

    multitest.wait("enable filter")
    can.set_filters([(FILTER_ID, FILTER_MASK, 0)])
    multitest.broadcast("filter set")

    multitest.wait("done")


# Sender
def instance1():
    multitest.next()

    can.send(ID, b"abc", CAN.FLAG_RTR)  # length==3 remote request
    time.sleep_ms(5)
    can.send(ID, b"abc", 0)  # regular message using the same ID
    time.sleep_ms(5)
    can.send(ID, b"abcde", CAN.FLAG_RTR)  # length==5 remote request
    time.sleep_ms(5)

    multitest.broadcast("enable filter")
    multitest.wait("filter set")

    # these two messages should be filtered out
    can.send(ID, b"abc", CAN.FLAG_RTR)  # length==3 remote request
    time.sleep_ms(5)
    can.send(ID, b"abc", 0)  # regular message using the same ID
    time.sleep_ms(5)

    # these messages should be filtered in
    can.send(FILTER_ID, b"def", CAN.FLAG_RTR)  # length==3 remote request
    time.sleep_ms(5)
    can.send(FILTER_ID, b"hij", 0)  # regular message using the same ID
    time.sleep_ms(5)

    multitest.broadcast("done")
    print("done")
