from machine import CAN
import time
from random import seed, randrange

import micropython

micropython.alloc_emergency_exception_buf(256)
seed(0)

# Testing that transmit order obeys the priority ordering

ID_LOW = 0x500
ID_HIGH = 0x200

NUM_MSGS = 255

MSG_LEN = 4

can = CAN(1, 500_000)


def check_sequence(items, label):
    # The full range of NUM_MSGS values should have been received or sent, in
    # order, without duplicates
    if len(items) != NUM_MSGS:
        print(label, "wrong count", len(items), "vs", NUM_MSGS)
    if items == list(range(NUM_MSGS)):
        print(label, "OK")
    else:
        print(label, "error:")
        print(items)


# Receiver

# lists of received messages, one list per ID
received_low = []
received_high = []


def irq_recv(can):
    while can.irq().flags() & can.IRQ_RX:
        can_id, data, flags, _errors = can.recv()

        if can_id == ID_LOW and len(data) == MSG_LEN:
            received_low.append(data[0])
        elif can_id == ID_HIGH and len(data) == MSG_LEN:
            received_high.append(data[0])
        else:
            print("unexpected recv", can_id, data, flags)


def instance0():
    can.irq(irq_recv, trigger=can.IRQ_RX, hard=False)
    can.set_filters([])  # receive all

    multitest.next()

    multitest.wait("sender done")
    check_sequence(received_low, "Low prio received")
    check_sequence(received_high, "High prio received")


# Sender

## Messages pending to send
pending_low = list(range(NUM_MSGS))
pending_high = list(range(NUM_MSGS))

# List of the messages currently queued to send
tx_queue = [None] * CAN.TX_QUEUE_LEN

# Messages sent, recorded in order as [high_prio, val]
sent = []
for _ in range(NUM_MSGS * 2):
    sent.append([None, None])
num_sent = 0


def irq_send(can):
    global num_sent

    while flags := can.irq().flags():
        assert flags & can.IRQ_TX  # the only enabled IRQ

        idx = (flags >> can.IRQ_TX_IDX_SHIFT) & can.IRQ_TX_IDX_MASK
        success = not (flags & can.IRQ_TX_FAILED)

        if not success:
            return  # We don't worry about failures here

        if not tx_queue[idx]:
            print("bad done", idx)
            return

        was_high, val = tx_queue[idx]
        tx_queue[idx] = None
        sent[num_sent][0] = was_high
        sent[num_sent][1] = val
        num_sent += 1


def instance1():
    can.irq(irq_send, trigger=can.IRQ_TX, hard=True)
    data = bytearray(MSG_LEN)

    multitest.next()

    while pending_low or pending_high:
        if pending_high:
            val = pending_high.pop(0)
            data[0] = val
            data[1] = 1
            while True:
                idx = can.send(ID_HIGH, data)
                if idx == -1:
                    continue  # keep trying until a queue spot opens up
                old = tx_queue[idx]
                tx_queue[idx] = (True, val)
                if old:
                    print("error high priority queue race", idx, val, old)
                break

        for _ in range(randrange(4)):
            # Try and queue many low priority messages (expecting most will fail)
            if pending_low:
                val = pending_low[0]
                data[0] = val
                data[1] = 0
                idx = can.send(ID_LOW, data)
                if idx == -1:
                    # don't retry indefinitely for low priority messages
                    continue

                old = tx_queue[idx]
                tx_queue[idx] = (False, val)
                pending_low.pop(0)
                if old is not None:
                    print("error low priority queue race", idx, val, old)

    print("waiting for tx queue to empty...")
    while any(x is not None for x in tx_queue):
        pass

    multitest.broadcast("sender done")

    # Check we sent the right number of messages
    if num_sent != 2 * NUM_MSGS:
        print("Sent %d expected %d" % (num_sent, 2 * NUM_MSGS))
    else:
        print("Sent right number of messages")

    # Check the low and high priority messages all arrived in order
    sent_low = [val for (prio, val) in sent[:num_sent] if prio == False]
    sent_high = [val for (prio, val) in sent[:num_sent] if prio == True]
    check_sequence(sent_low, "Low prio sent")
    check_sequence(sent_high, "High prio sent")

    # check that high priority message queue items always stayed ahead of the low priority
    high_val = -1
    for idx, (prio, val) in enumerate(sent):
        if prio:
            high_val = val
        elif high_val <= val and val < NUM_MSGS - 1:
            print(
                "Low priority message %d overtook high priority %d at index %d"
                % (val, high_val, idx)
            )
