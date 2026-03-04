from machine import CAN
import time

# Check that cancelling a low priority outgoing message and replacing it with a
# high priority message causes it to be transmitted successfully onto a busy bus

recv = []

ITERS = 5

can = CAN(1, 500_000)


def irq_recv(can):
    global recv_std_id
    while can.irq().flags() & can.IRQ_RX:
        can_id, data, flags, _errors = can.recv()
        assert flags & CAN.FLAG_EXT_ID  # test uses all extended IDs
        if len(recv) < ITERS:
            recv.append(can_id)


def instance0():
    can.irq(irq_recv, trigger=can.IRQ_RX, hard=False)
    can.set_filters([])  # receive all

    multitest.next()

    # "Babble" medium priority messages onto the bus to prevent
    # instance1() from sending anything lower priority than this
    while len(recv) < ITERS:
        for id in range(0x5000, 0x6000):
            can.send(id, b"BABBLE", CAN.FLAG_EXT_ID)
            if len(recv) >= ITERS:
                break

    print("received", ITERS, "messages")
    for can_id in recv:
        print(hex(can_id))  # should be the high priority messages from instance1, only

    multitest.wait("sender done")
    print("done")


last_idx = 0
total_cancels = 0
total_sent = 0


def irq_send(can):
    global total_cancels, total_sent

    while flags := can.irq().flags():
        assert flags & can.IRQ_TX  # the only enabled IRQ

        idx = (flags >> can.IRQ_TX_IDX_SHIFT) & can.IRQ_TX_IDX_MASK

        if flags & can.IRQ_TX_FAILED:
            # we should only see failed transmits due to cancels in buffer 'last_idx'
            assert idx == last_idx
            total_cancels += 1
        else:
            # this includes the messages we explicitly send, plus queued low
            # priority messages once the receiver stops 'babbling' on the bus
            total_sent += 1


def instance1():
    global last_idx
    can.irq(irq_send, trigger=can.IRQ_TX, hard=True)

    multitest.next()

    for i in range(ITERS):
        # Fill the transmit queue with low priority messages (all extended IDs)
        last_idx = 0
        if i < 3:
            # For the first 3 iterations, send unique message IDs
            id_range = range(0x7000, 0x7FFF)
            flags = CAN.FLAG_EXT_ID
        else:
            # For the last iterations, repeat the same ID but tell controller to ignore
            # ordering (allows it to queue more than one despite hardware limitations)
            id_range = [0x50000 + i] * CAN.TX_QUEUE_LEN
            flags = CAN.FLAG_EXT_ID | CAN.FLAG_UNORDERED

        for id in id_range:
            idx = can.send(id, b"LOWPRIO", flags)
            if idx == -1:
                break  # send queue is full, stop trying to send
            last_idx = idx

        time.sleep_ms(50)  # the send queue shouldn't empty as instance0 is "babbling"

        # try and cancel the last message we queued
        res = can.cancel_send(last_idx)
        print(i, "cancel result", res)

        # send a high priority message, that we expect to go out
        idx = can.send(0x500 + i, b"HIPRIO", CAN.FLAG_EXT_ID)
        print(i, "send result", idx > -1)

        # make sure this message is sent onto the bus
        time.sleep_ms(1)

    multitest.broadcast("sender done")

    # let the entire transmit queue drain, now instance0 should have gone quiet
    time.sleep_ms(50)

    print("total cancels", total_cancels)  # should equal ITERS

    if total_sent == CAN.TX_QUEUE_LEN - 1 + ITERS:
        # expect we send one message for each of ITERS, plus all low priority
        # queued messages once instance0 stops babbling on the bus
        print("total sent OK")
    else:
        print("total sent", total_sent, CAN.TX_QUEUE_LEN - 1 + ITERS)
