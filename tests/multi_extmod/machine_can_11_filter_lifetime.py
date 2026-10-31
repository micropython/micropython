from machine import CAN
import time

can = CAN(1, 500_000)
received = []
E = CAN.FLAG_EXT_ID
R = CAN.FLAG_RTR
FRAMES = [(0x123, 0), (0x124, 0), (0x456, E), (0x457, E), (0x123, R), (0x456, E | R)]
STD = [frame for frame in FRAMES if not frame[1] & E]
EXT = [frame for frame in FRAMES if frame[1] & E]
EXACT_STD = [frame for frame in FRAMES if frame[0] == 0x123 and not frame[1] & E]
EXACT_EXT = [frame for frame in FRAMES if frame[0] == 0x456 and frame[1] & E]


def irq(can):
    while (frame := can.recv()) is not None:
        received.append((frame[0], frame[2]))


def check(label, expected):
    received.clear()
    multitest.broadcast("send")
    multitest.wait("sent")
    time.sleep_ms(10)
    print(label, received == expected)


def instance0():
    can.irq(irq, CAN.IRQ_RX, hard=False)
    multitest.next()
    check("default reject", [])
    can.set_filters([(0, 0, 0)])
    check("standard zero mask", STD)
    can.set_filters([(0, 0, E)])
    check("extended zero mask", EXT)
    can.set_filters([(0, 0, 0), (0x456, 0x1FFFFFFF, E)])
    check("mixed filters", [frame for frame in FRAMES if frame in STD + EXACT_EXT])
    can.set_filters([(0x456, 0x1FFFFFFF, E), (0, 0, 0)])
    check("mixed reversed", [frame for frame in FRAMES if frame in STD + EXACT_EXT])
    can.restart()
    check("mixed restart", [frame for frame in FRAMES if frame in STD + EXACT_EXT])
    can.set_filters([(0x123, 0x7FF, 0)])
    check("exact standard", EXACT_STD)
    can.restart()
    check("standard restart", EXACT_STD)
    can.set_filters([(0x456, 0x1FFFFFFF, E)])
    can.restart()
    check("extended restart", EXACT_EXT)
    can.set_filters(None)
    can.restart()
    check("accept all restart", FRAMES)
    can.set_filters([])
    can.restart()
    check("reject all restart", [])
    can.init(500_000)
    can.irq(irq, CAN.IRQ_RX, hard=False)
    check("reinit default reject", [])
    multitest.broadcast("done")


def instance1():
    multitest.next()
    for _ in range(12):
        multitest.wait("send")
        for can_id, flags in FRAMES:
            assert can.send(can_id, b"x", flags) is not None
            time.sleep_ms(10)
        multitest.broadcast("sent")
    multitest.wait("done")
    print("done")
