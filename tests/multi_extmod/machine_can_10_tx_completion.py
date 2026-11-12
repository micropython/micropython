from machine import CAN
import micropython
import time

# Defer soft callbacks while the hardware continues transmitting. Completed
# buffers must not be reused if doing so would lose their completion events.
can = CAN(1, 500_000)
can.set_filters([])
accepted = []
completed = []


def irq(can):
    while flags := can.irq().flags():
        assert flags & CAN.IRQ_TX
        assert not (flags & CAN.IRQ_TX_FAILED)
        completed.append((flags >> CAN.IRQ_TX_IDX_SHIFT) & CAN.IRQ_TX_IDX_MASK)


def burst(_):
    for value in range(3 * CAN.TX_QUEUE_LEN):
        idx = can.send(0x123, bytes([value]))
        if idx is not None:
            accepted.append(idx)
        # Soft callbacks cannot run until this scheduled callback returns.
        time.sleep_us(1000)


def instance0():
    can.irq(irq, CAN.IRQ_TX, hard=False)
    multitest.next()
    multitest.wait("peer ready")
    for _ in range(3):
        accepted.clear()
        completed.clear()
        micropython.schedule(burst, None)
        time.sleep_ms(100)
        print("completion count", len(accepted) > 0 and len(accepted) == len(completed))
        print("completion indexes", sorted(accepted) == sorted(completed))
        print("bounded queue", len(accepted) <= CAN.TX_QUEUE_LEN)
        print("no pending TX", can.get_counters()[5] == 0)
    multitest.broadcast("done")


def instance1():
    multitest.next()
    multitest.broadcast("peer ready")
    multitest.wait("done")
    print("done")
