from machine import CAN
from micropython import const
import time

# Test that without a second CAN node on the network the controller will
# correctly go into the correct error states, and can then recover.
#
# Note this test depends on no other CAN node being active on the network apart
# from the two test instances. (Although it's OK for extra nodes to be in a
# "listen only" mode where they won't ACK messages.)

rx_overflow = False
rx_full = False
received = []

# CAN IDs
_ID = const(0x100)

# get_state() result list indexes, as constants
_IDX_TEC = const(0)
_IDX_REC = const(1)
_IDX_NUM_WARNING = const(2)
_IDX_NUM_PASSIVE = const(3)
_IDX_NUM_BUS_OFF = const(4)
_IDX_PEND_TX = const(5)

can = CAN(1, 500_000)


def state_name(state):
    for name in dir(CAN):
        if name.startswith("STATE_") and state == getattr(CAN, name):
            return name
    return f"UNKNOWN-{state}"


def irq_recv(can):
    while can.irq().flags() & can.IRQ_RX:
        can_id, data, _flags, errors = can.recv()
        print("recv", hex(can_id), data.hex())


# Receiver
def instance0():
    can.irq(irq_recv, trigger=can.IRQ_RX, hard=False)

    can.set_filters([])  # receive all

    multitest.next()

    # Receive at least one CAN message before sender asks us to disable the controller

    multitest.wait("disable receiver")
    can.deinit()
    print("can deinit()")
    multitest.broadcast("receiver disabled")

    # Wait for the sender to tell us to re-enable

    multitest.wait("enable receiver")
    # note the irq is no longer active after deinit()
    can.init(500_000)
    print("can init()")
    multitest.broadcast("receiver enabled")

    # Receive CAN messages until the sender asks us to switch to an invalid baud rate

    multitest.wait("switch baud")
    can.init(125_000)
    print("can switch baud")
    multitest.broadcast("switched baud")

    time.sleep_ms(1)

    print("sending bad msg")
    # trying to send this frame should introduce more bus errors
    idx_bad = can.send(_ID, b"BADBAUD")

    multitest.wait("fix baud")
    print("sending cancelling")
    print("cancelled", can.cancel_send(idx_bad))
    print("re-init")
    can.init(500_000)
    multitest.broadcast("fixed baud")

    # Should be receiving CAN messages OK again

    print("done")


def irq_sender(can):
    while flags := can.irq().flags():
        if flags & can.IRQ_STATE:
            print("irq state", can.get_state())
        if flags & can.IRQ_TX:
            print("irq sent", not (flags & can.IRQ_TX_FAILED))


# Sender
def instance1():
    can.irq(irq_sender, CAN.IRQ_STATE | CAN.IRQ_TX, hard=False)

    can.set_filters([])

    multitest.next()

    print("started", state_name(can.get_state()))  # should be ERROR_ACTIVE

    # Send a single message to the receiver, to verify it's working
    can.send(_ID, b"PAYLOAD")
    active_counters = can.get_counters()
    # print(active_counters)  # DEBUG

    multitest.broadcast("disable receiver")
    multitest.wait("receiver disabled")

    # Now the receiver shouldn't be ACKing our frames, queue will stay full
    # ... we will get the ISR for ERROR_WARNING but it'll go from ERROR_WARNING to ERROR_PASSIVE
    # very quickly as all messages are failing
    can.send(_ID, b"MORE")
    while can.get_state() in (CAN.STATE_ACTIVE, CAN.STATE_WARNING):
        pass

    print(state_name(can.get_state()))  # should be ERROR_PASSIVE now
    passive_counters = can.get_counters()
    # print(passive_counters)  # DEBUG
    print("tec increased", passive_counters[_IDX_TEC] > active_counters[_IDX_TEC])
    print("tec over thresh", passive_counters[_IDX_TEC] >= 128)
    # we should have counted exactly one ERROR_WARNING and ERROR_PASSIVE transition
    print(
        "counted warning",
        passive_counters[_IDX_NUM_WARNING] == active_counters[_IDX_NUM_WARNING] + 1,
    )
    print(
        "counted passive",
        passive_counters[_IDX_NUM_PASSIVE] == active_counters[_IDX_NUM_PASSIVE] + 1,
    )
    print("some pending tx", passive_counters[_IDX_PEND_TX] > 0)

    # Re-enable the receiver which should allow us to go from ERROR_PASSIVE to ERROR_WARNING
    multitest.broadcast("enable receiver")
    multitest.wait("receiver enabled")

    can.send(_ID, b"MORE")
    while can.get_state() == CAN.STATE_PASSIVE:
        pass

    print(state_name(can.get_state()))  # should be ERROR_WARNING now
    warning_counters = can.get_counters()
    # print(warning_counters)  # DEBUG
    print("tec decreased", warning_counters[_IDX_TEC] < passive_counters[_IDX_TEC])
    print(
        "tec below thresh", warning_counters[_IDX_TEC] < 128
    )  # and should be more than error passive threshold
    # error warning count should stay the same, as we went "down" in severity not up
    print(
        "no new warning", warning_counters[_IDX_NUM_WARNING] == passive_counters[_IDX_NUM_WARNING]
    )
    print(
        "no new passive", warning_counters[_IDX_NUM_PASSIVE] == passive_counters[_IDX_NUM_PASSIVE]
    )

    # Tell the receiver to change to the wrong baud rate, which should create both RX and TX errorxs
    multitest.broadcast("switch baud")
    multitest.wait("switched baud")

    # queue another message. This will keep trying to send until we revert back to ERROR_PASSIVE
    idx = can.send(_ID, b"YETMORE")
    print("queued yetmore", idx > -1)
    while can.get_state() != CAN.STATE_PASSIVE:
        pass

    print(state_name(can.get_state()))  # should be ERROR_PASSIVE again
    passive_counters = can.get_counters()
    # print(passive_counters)  # DEBUG
    # we can't say for sure which error counter will hit the ERROR_PASSIVE threshold first
    print(
        "one over thresh", passive_counters[_IDX_TEC] >= 128 or passive_counters[_IDX_REC] >= 128
    )
    print(
        "no new warning", passive_counters[_IDX_NUM_WARNING] == warning_counters[_IDX_NUM_WARNING]
    )
    print(
        "counted passive",
        passive_counters[_IDX_NUM_PASSIVE] == warning_counters[_IDX_NUM_PASSIVE] + 1,
    )

    # Note that we can't get all the way to the most severe BUS_OFF error state
    # with this test setup, as Bus Off requires more than just "normal" frame
    # transmit errors.

    # restarting the controller may cause it to leave its error state, or not, depending
    # on the implementation - but it shouldn't cause any recovery issues. Also cancels all pending TX
    # (note: have to do this before 'fix baud' or we create a race condition for pending tx)
    can.restart()

    # tell the receiver to go back to a valid baud rate
    multitest.broadcast("fix baud")
    multitest.wait("fixed baud")

    idx_more = can.send(_ID, b"MOREMORE")
    time.sleep_ms(50)  # irq_sender should fire during this window
    print("queued moremore", idx_more > -1)

    print("done")
