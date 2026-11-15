from machine import CAN
import time

# Verify isolated loopback without driving the bus, independently of the modes
# supported by every controller. The peer must see no frame and receive no ACK.
can = CAN(1, 500_000)


def instance0():
    try:
        can.init(500_000, mode=CAN.MODE_SILENT_LOOPBACK)
    except ValueError as exc:
        if str(exc) != "mode":
            raise
        can.deinit()
        multitest.skip()
    can.set_filters(None)
    multitest.next()
    multitest.wait("peer ready")

    assert can.send(0x54, b"SiLoop") is not None
    time.sleep_ms(50)
    frame = can.recv()
    print("loopback", frame[0] == 0x54 and bytes(frame[1]) == b"SiLoop")
    multitest.broadcast("loopback done")
    multitest.wait("peer done")
    print("no external RX", can.recv() is None)
    can.deinit()


def instance1():
    can.set_filters(None)
    multitest.next()
    multitest.broadcast("peer ready")
    multitest.wait("loopback done")
    print("no external TX", can.recv() is None)
    can.send(0x53, b"no ACK")
    time.sleep_ms(20)
    print("no ACK", can.get_counters()[0] >= 128)
    can.deinit()
    multitest.broadcast("peer done")
