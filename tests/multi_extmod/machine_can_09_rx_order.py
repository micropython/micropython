from machine import CAN
import time

# Each burst fits in the three-element FIFO on bxCAN and some FDCAN parts.
# Wait for each TX completion so the expected order is the actual wire order,
# not the order in which frames were queued for arbitration.
can = CAN(1, 500_000)
MESSAGES = (
    (0x1FFFFF01, b"a", CAN.FLAG_EXT_ID),
    (0x101, b"b", 0),
    (0x1FFFFF02, b"c", CAN.FLAG_EXT_ID),
    (0x102, b"d", 0),
    (0x1FFFFF01, b"e", CAN.FLAG_EXT_ID),
    (0x101, b"f", 0),
)
FILTERS = (
    (0x101, 0x7FF, 0),
    (0x102, 0x7FF, 0),
    (0x1FFFFF01, 0x1FFFFFFF, CAN.FLAG_EXT_ID),
    (0x1FFFFF02, 0x1FFFFFFF, CAN.FLAG_EXT_ID),
)


def instance0():
    multitest.next()
    for phase, filters in (("filters", FILTERS), ("all", None)):
        can.set_filters(filters)
        for burst in range(2):
            key = phase + str(burst)
            multitest.broadcast("ready " + key)
            multitest.wait("sent " + key)
            received = []
            deadline = time.ticks_add(time.ticks_ms(), 1000)
            while len(received) < 3 and time.ticks_diff(deadline, time.ticks_ms()) > 0:
                r = can.recv()
                if r is not None:
                    can_id, data, flags, errors = r
                    received.append((can_id, bytes(data), flags & CAN.FLAG_EXT_ID))
            print(key, received == list(MESSAGES[burst * 3 : burst * 3 + 3]))


tx_result = None


def tx_irq(can):
    global tx_result
    while flags := can.irq().flags():
        if flags & can.IRQ_TX:
            tx_result = not bool(flags & can.IRQ_TX_FAILED)


def instance1():
    global tx_result
    can.irq(tx_irq, trigger=can.IRQ_TX, hard=False)
    multitest.next()
    for phase in ("filters", "all"):
        for burst in range(2):
            key = phase + str(burst)
            multitest.wait("ready " + key)
            for can_id, data, flags in MESSAGES[burst * 3 : burst * 3 + 3]:
                tx_result = None
                assert can.send(can_id, data, flags) is not None
                deadline = time.ticks_add(time.ticks_ms(), 1000)
                while tx_result is None and time.ticks_diff(deadline, time.ticks_ms()) > 0:
                    time.sleep_ms(1)
                assert tx_result is True
            multitest.broadcast("sent " + key)
    print("sent OK")
