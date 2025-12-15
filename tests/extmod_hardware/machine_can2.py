# Test machine.CAN(1) and machine.CAN(2) using loopback
#
# Single device test, assumes support for loopback and no connections to the CAN pins
#
# This test is ported from tests/ports/stm32/pyb_can2.py

try:
    from machine import CAN

    CAN(2, 125_000)
except (ImportError, ValueError):
    print("SKIP")
    raise SystemExit

import time

# Setting up each CAN peripheral independently is deliberate here, to catch
# catch cases where initialising CAN2 breaks CAN1

can1 = CAN(1, 125_000, mode=CAN.MODE_LOOPBACK)
can1.set_filters([(0x100, 0x700, 0)])

can2 = CAN(2, 125_000, mode=CAN.MODE_LOOPBACK)
can2.set_filters([(0x000, 0x7F0, 0)])

# Drain any old messages in RX FIFOs
for can in (can1, can2):
    while can.recv():
        pass

for id, can in ((1, can1), (2, can2)):
    print("testing", id)
    # message1 should only receive on can1, message2 on can2
    can.send(0x123, b"message1", 0)
    can.send(0x003, "message2", 0)
    time.sleep_ms(10)
    did_recv = False
    while res := can.recv():
        did_recv = True
        print(hex(res[0]), bytes(res[1]), res[2], res[3])
    if not did_recv:
        print("no rx!")

print("done")
