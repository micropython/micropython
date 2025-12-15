from machine import CAN
import time

# Test for filtering capabilities

can = CAN(1, 500_000)

# IDs and filter phases used for the 'single id' part of the test
SINGLE_EXT_ID = (0x1234_5678, 0x1FFF_FFFF, CAN.FLAG_EXT_ID)
SINGLE_STD_ID = (0x505, 0x7FF, 0)
SINGLE_ID_PHASES = [
    ("single ext id", [SINGLE_EXT_ID]),
    ("single std id", [SINGLE_STD_ID]),
    ("ext+std ids", [SINGLE_EXT_ID, SINGLE_STD_ID]),
    ("std+ext ids", [SINGLE_STD_ID, SINGLE_EXT_ID]),  # these two should be equivalent
    ("accept none", []),
    ("accept all", None),
    ("accept none again", ()),
]


# Receiver
def receiver_irq_recv(can):
    assert can.irq().flags() & can.IRQ_RX  # the only enabled IRQ
    can_id, data, _flags, _errors = can.recv()
    print("recv", hex(can_id), data.hex())


def instance0():
    can.irq(receiver_irq_recv, trigger=can.IRQ_RX, hard=False)

    multitest.next()

    # Configure to receive standard frames (in a range), and
    # extended frames (in a range).
    can.set_filters([(0x300, 0x300, 0), (0x3000, 0x3000, CAN.FLAG_EXT_ID)])
    multitest.broadcast("ready id ranges")

    # Run through the phases of filtering for individual IDs
    for phase, filters in SINGLE_ID_PHASES:
        multitest.wait("configure " + phase)
        if filters and len(filters) > CAN.FILTERS_MAX:
            # this check really exists to add test coverage for the FILTERS_MAX constant
            print("Warning: Too many filters for hardware!")
        can.set_filters(filters)
        print("receiver configured " + phase)
        multitest.broadcast("ready " + phase)

    multitest.wait("Sender done")


def send_messages(messages):
    for can_id, payload, flags in messages:
        r = can.send(can_id, payload, flags)
        if r is None:
            print("Failed to send:", hex(can_id), payload.hex())
        time.sleep_ms(5)  # avoid flooding either our or the receiver's FIFO


# Sender
def instance1():
    multitest.next()
    multitest.wait("ready id ranges")

    print("Sending ID ranges...")
    for i in range(3):
        send_messages(
            [
                (0x345, bytes([i, 0xFF] * (i + 1)), 0),
                (0x3700 + i, bytes([0xEE] * (i + 1)), CAN.FLAG_EXT_ID),
                (0x123, b"abcdef", 0),  # matches no filter, expect ACKed but not received
            ]
        )

    # Now move on to single ID filtering

    single_id_messages = [
        (0x1234_5678, b"\x01\x02\x03\x04\x05", CAN.FLAG_EXT_ID),  # matches ext id
        (0x0234_5678, b"\x00\x00", CAN.FLAG_EXT_ID),  # no match
        (0x678, b"\x00\x01", 0),  # no match
        (0x505, b"\x06\x07\x08\x09\x0a\x0b", 0),  # matches standard id
        (0x345, b"\x00\x02", 0),  # no match (in prev filter)
        (0x1234_5679, b"\x00\x03", CAN.FLAG_EXT_ID),  # no match
        (0x3705, b"\x00\x04", CAN.FLAG_EXT_ID),  # no match (in prev filter)
        (0x1234_5678, b"\x01\x02\x03", CAN.FLAG_EXT_ID),  # matches ext id
        (0x505, b"\x04\x05\x06", 0),  # matches standard id
        (0x505, b"\x00\x05", CAN.FLAG_EXT_ID),  # no match (is ext id)
        (0x507, b"\x00\x06", 0),  # no match
        (0x1334_5678, b"\x00\x07", CAN.FLAG_EXT_ID),  # no match
        (0x1234_5670, b"\x00\x08", CAN.FLAG_EXT_ID),  # no match
    ]

    # Send the same list of messages for each phase of the test.
    # The receiver will have configured different filters, and the .exp
    # file is what selects which messages should be received or not.
    for phase, _ in SINGLE_ID_PHASES:
        multitest.broadcast("configure " + phase)
        multitest.wait("ready " + phase)
        print("Sending for " + phase + "...")
        send_messages(single_id_messages)

    print("Sender done")
    multitest.broadcast("Sender done")
