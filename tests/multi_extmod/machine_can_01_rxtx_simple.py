from machine import CAN
import time

ID_0 = 0x50
ID_1 = 0x50333
FLAGS_1 = CAN.FLAG_EXT_ID  # ID_1 is an extended CAN ID

can = CAN(1, 500_000)
can.set_filters([])  # receive all


def print_rx(can_id, data, flags, errors):
    print(hex(can_id), bytes(data), hex(flags), hex(errors))


def instance0():
    multitest.next()

    # receive from instance1 first
    while not (rx := can.recv()):
        time.sleep(0)
    print_rx(*rx)

    # now send one
    can.send(ID_0, b"1234")


def instance1():
    multitest.next()

    can.send(ID_1, b"ABCD", FLAGS_1)

    while not (rx := can.recv()):
        time.sleep(0)
    print_rx(*rx)
