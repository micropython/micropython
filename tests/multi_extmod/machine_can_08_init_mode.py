from machine import CAN
from micropython import const
import time

# instance0 transitions through various modes, instance1
# listens for various messages (or not)
#
# Note this test assumes no other CAN nodes are connected apart from the test
# instances (or if they are connected they must be in silent mode.)
#
# TODO: This test needs to eventually support the case where modes aren't supported
# on a controller, maybe by printing fake output if the mode switch fails?

# MODE_NORMAL, MODE_SLEEP, MODE_LOOPBACK, MODE_SILENT, MODE_SILENT_LOOPBACK
can = CAN(1, 500_000, mode=CAN.MODE_NORMAL)

# While instance0 is in Silent mode, instance1 sends a message with this ID
# that will be retried for 100ms (as instance0 won't ACK). So don't print every one.
_SILENT_RX_ID = const(0x53)
silent_rx_count = 0


def irq_print(can):
    global silent_rx_count
    while flags := can.irq().flags():
        if flags & can.IRQ_RX:
            can_id, data, _flags, errors = can.recv()
            if can_id != _SILENT_RX_ID:
                print("recv", hex(can_id), bytes(data))
            else:
                silent_rx_count += 1
        if flags & can.IRQ_TX:  # note: only enabled on instance1 to avoid race conditions
            print("send", "failed" if flags & can.IRQ_TX_FAILED else "ok")


def reinit_with_mode(mode):
    can.deinit()
    can.init(bitrate=500_000, mode=mode)
    can.irq(irq_print, trigger=can.IRQ_RX, hard=False)
    can.set_filters([])  # receive all


def instance0():
    multitest.next()
    multitest.wait("instance1 ready")

    reinit_with_mode(can.MODE_NORMAL)
    print("Normal", "MODE_NORMAL" in str(can))
    can.send(0x50, b"Normal")
    time.sleep_ms(100)

    # Skipping MODE_SLEEP as means different things on different hardware

    reinit_with_mode(can.MODE_LOOPBACK)
    print("Loopback", "MODE_LOOPBACK" in str(can))

    # This message should go out to the bus, but will also be received by instance0 itself
    can.send(0x51, b"Loopback")
    time.sleep_ms(100)

    reinit_with_mode(can.MODE_SILENT)
    print("Silent", "MODE_SLIENT" in str(can))

    # This message shouldn't go out onto the bus
    idx = can.send(0x52, b"Silent")
    multitest.broadcast("silent")
    multitest.wait("silent done")
    # we should have received the message from instance1 many times, as instance0 won't have ACKed it
    print("silent_rx_count", silent_rx_count > 5)
    can.cancel_send(idx)

    reinit_with_mode(can.MODE_SILENT_LOOPBACK)
    print("Silent Loopback", "MODE_SILENT_LOOPBACK" in str(can))

    # This message should be received by instance0 only
    idx = can.send(0x54, b"SiLoop")
    time.sleep_ms(50)

    reinit_with_mode(can.MODE_NORMAL)
    print("Normal again", "MODE_NORMAL" in str(can))
    can.send(0x55, b"Normal2")  # should be received by instance1 only, again
    multitest.broadcast("normal done")


# Receiver
def instance1():
    can.irq(irq_print, trigger=can.IRQ_RX | can.IRQ_TX, hard=False)
    can.set_filters([])  # receive all
    multitest.next()

    multitest.broadcast("instance1 ready")

    # The IRQ does most of the work on this instance

    multitest.wait("silent")
    # Sending this message back, it should fail to send as Silent mode won't ACK it
    idx = can.send(0x53, b"Silent2")
    time.sleep_ms(20)
    can.cancel_send(idx)
    multitest.broadcast("silent done")

    multitest.wait("normal done")
