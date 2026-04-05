try:
    from pyb import CAN

    CAN(2)
except (ImportError, ValueError):
    print("SKIP")
    raise SystemExit

# Classic CAN (aka bxCAN) hardware has a different filter API
# and some different behaviours to newer FDCAN hardware
IS_CLASSIC = hasattr(CAN, "MASK16")

# Setting up each CAN peripheral independently is deliberate here, to catch
# catch cases where initialising CAN2 breaks CAN1

can1 = CAN(1, CAN.LOOPBACK)
if IS_CLASSIC:
    can1.setfilter(0, CAN.LIST16, 0, (123, 124, 125, 126))
else:
    can1.setfilter(0, CAN.RANGE, 0, (123, 126))

can2 = CAN(2, CAN.LOOPBACK)
if IS_CLASSIC:
    can2.setfilter(0, CAN.LIST16, 0, (3, 4, 5, 6))
else:
    can2.setfilter(0, CAN.RANGE, 0, (3, 6))

# Drain any old messages in RX FIFOs
for can in (can1, can2):
    while can.any(0):
        can.recv(0)

for id, can in ((1, can1), (2, can2)):
    print("testing", id)
    # message1 should only receive on can1, message2 on can2
    can.send("message1", 123)
    can.send("message2", 3)
    did_recv = False
    try:
        while True:
            res = can.recv(0, timeout=50)
            # not printing all of 'res' as the filter index result is different
            # on Classic vs FD-CAN
            print("rx", res[0], res[4])
            did_recv = True
    except OSError:
        if not did_recv:
            print("no rx!")

print("done")
