from machine import CAN
import time

# Test the CAN.IRQ_RX irq handler, including overflow

rx_overflow = False
rx_full = False
received = []

# CAN IDs
ID_SPAM = 0x345  # messages spammed into the receive FIFO
ID_ACK_OFLOW = 0x055  # message the receiver sends after it's seen an overflow
ID_AFTER = 0x100  # message the sender sends after the ACK

can = CAN(1, 500_000)


# A very basic "soft" receiver handler that stores received messages into a global list
def receiver_irq_recv(can):
    global rx_overflow, rx_full

    assert can.irq().flags() & can.IRQ_RX  # the only enabled IRQ

    can_id, data, _flags, errors = can.recv()

    received.append((can_id, None))

    # The FIFO is expected not to overflow by itself, wait until 40 messages
    # have been received and then block the receive handler to induce an overflow
    if len(received) == 40:
        assert not rx_overflow  # shouldn't have already happened, either
        time.sleep_ms(500)

    if not rx_overflow and (errors & CAN.RECV_ERR_OVERRUN):
        # expected this should happen on the very next message after
        # the one where we slept for 500ms
        print("irq_recv overrun", len(received))
        received.clear()  # check we still get some messages, see rx_spam print line below
        rx_overflow = True

    # also expect the FIFO to be FULL again immediately after overrunning and rx_overflow event
    if rx_overflow and (errors & CAN.RECV_ERR_OVERRUN | CAN.RECV_ERR_FULL) == CAN.RECV_ERR_FULL:
        rx_full = True


# Receiver
def instance0():
    can.irq(receiver_irq_recv, trigger=can.IRQ_RX, hard=False)

    can.set_filters([])  # receive all

    multitest.next()

    while not rx_overflow:
        pass  # Resume ASAP after FIFO0 overflows

    can.send(ID_ACK_OFLOW, b"overflow")

    # at least one ID_SPAM message should have been received
    # *after* we overflowed and 'received' was clear in the irq handler
    print("rx_spam", any(r[0] == ID_SPAM for r in received))

    # wait until the "after" message is received
    for n in range(100):
        if any(r[0] == ID_AFTER for r in received):
            break
        time.sleep_ms(10)

    can.irq(None)  # disable the IRQ
    received.clear()

    # at some point while waiting for ID_AFTER the FIFO should have gotten
    # full again
    print("rx_full", rx_full)

    # now IRQ is disabled, no new messages should be received
    time.sleep_ms(250)
    print("len", len(received))


received_ack = False

# reusing the result buffer so sender_irq_recv can be 'hard'
sender_irq_result = [None, memoryview(bytearray(64)), None, None]


def sender_irq_recv(can):
    global received_ack

    assert can.irq().flags() & can.IRQ_RX  # the only enabled IRQ

    can_id, data, _flags, _errors = can.recv(sender_irq_result)
    print("sender_irq_recv", can_id, len(data))  # should be ID_ACK_OFLOW and "overflow" payload
    received_ack = True


# Sender
def instance1():
    can.irq(sender_irq_recv, CAN.IRQ_RX, hard=True)

    can.set_filters([])

    multitest.next()

    # Spam out messages until the receiver tells us its RX FIFO is full.
    #
    # The RX FIFO on the receiver can vary from 3 deep (BXCAN) to 25 deep (STM32H7),
    # so we keep sending to it until we see a CAN message on ID_ACK_OFLOW indicating
    # the receiver's FIFO has overflowed
    while not received_ack:
        for i in range(255):
            while can.send(ID_SPAM, bytes([i] * 8)) is None and not received_ack:
                # Don't overflow the TX FIFO
                time.sleep_ms(1)
            if received_ack:
                break

    # give the receiver some time to make space in the FIFO
    time.sleep_ms(200)

    # send the final message, the receiver should get this one
    can.send(ID_AFTER, b"aaaaa")
