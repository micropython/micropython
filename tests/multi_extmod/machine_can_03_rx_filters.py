from machine import CAN
import time

# Test for filtering capabilities

can = CAN(1, 500_000)


# Receiver
def receiver_irq_recv(can):
    assert can.irq().flags() & can.IRQ_RX  # the only enabled IRQ
    can_id, data, _flags, _errors = can.recv()
    print("recv", hex(can_id), data.hex())


def instance0():
    # Configure to receive standard frames (in a range), and
    # extended frames (in a range).
    can.set_filters([(0x300, 0x300, 0), (0x3000, 0x3000, CAN.FLAG_EXT_ID)])
    can.irq(receiver_irq_recv, trigger=can.IRQ_RX, hard=False)

    multitest.next()
    multitest.wait("sender ready")
    multitest.broadcast("receiver ready")

    time.sleep_ms(500)


# Sender
def instance1():
    multitest.next()
    multitest.broadcast("sender ready")
    multitest.wait("receiver ready")

    for i in range(3):
        print(i)
        for can_id, payload, flags in [
            (0x345, bytes([i, 0xFF] * (i + 1)), 0),
            (0x3700 + i, bytes([0xEE] * (i + 1)), CAN.FLAG_EXT_ID),
            (0x123, b"abcdef", 0),  # matches no filter, expect ACKed but not received
        ]:
            r = can.send(can_id, payload, flags)
            # result should indicate the message was queued successfully
            print("sent", hex(can_id), payload.hex(), r is not None)
            time.sleep_ms(5)  # avoid flooding either our or the receiver's FIFO
