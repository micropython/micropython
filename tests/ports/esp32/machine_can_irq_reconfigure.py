# Task-only driver operations must fail before mutating state in a hard IRQ.
# Requires a CAN-enabled board and a transceiver for bus-connected loopback.
try:
    from machine import CAN
except ImportError:
    print("SKIP")
    raise SystemExit

import errno
import micropython
import time

micropython.alloc_emergency_exception_buf(512)
can = CAN(1, 500_000, mode=CAN.MODE_LOOPBACK)
can.set_filters([(0x123, 0x7FF, 0)])
filters = [(0x124, 0x7FF, 0)]
frame = [None, memoryview(bytearray(8)), None, None]
seen = [0, 0]


def irq(can):
    can.irq().flags()
    assert can.recv(frame) is frame
    try:
        can.init(250_000)
    except OSError as exc:
        assert exc.args[0] == errno.EPERM
        seen[1] += 1
    try:
        can.deinit()
    except OSError as exc:
        assert exc.args[0] == errno.EPERM
        seen[1] += 1
    try:
        can.restart()
    except OSError as exc:
        assert exc.args[0] == errno.EPERM
        seen[1] += 1
    try:
        can.set_filters(filters)
    except OSError as exc:
        assert exc.args[0] == errno.EPERM
        seen[1] += 1
    seen[0] += 1


try:
    try:
        irq_obj = can.irq(irq, CAN.IRQ_RX, hard=True)
    except ValueError:
        # The board must opt into the larger ISR stack for hard CAN callbacks.
        print("SKIP")
        raise SystemExit
    for i in range(20):
        assert can.send(0x123, b"test") is not None
        deadline = time.ticks_add(time.ticks_ms(), 1000)
        while seen[0] <= i:
            assert time.ticks_diff(deadline, time.ticks_ms()) > 0
            time.sleep_ms(1)
        assert seen[1] == 4 * (i + 1)
        assert can.irq() is irq_obj and irq_obj.trigger() == CAN.IRQ_RX
        assert can.get_timings()[0] == 500_000
        assert can.state() == CAN.STATE_ACTIVE
    print("hard IRQ reconfiguration rejected; controller state preserved")
finally:
    can.deinit()
