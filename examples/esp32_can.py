from machine import CAN
import time


def send_and_check(can_bus, name, id, expected_result=True, extended=False):
    can_bus.clear_tx_queue()
    can_bus.clear_rx_queue()
    can_bus.send([], id, extframe=extended)
    time.sleep_ms(100)
    if can_bus.any() == expected_result:
        print("{}: OK".format(name))
        if expected_result:
            can_bus.recv()
    else:
        print("{}: FAILED".format(name))


# 4 and 5 pins must be connected to each other, see documentation
can = CAN(
    0, extframe=False, tx=4, rx=5, mode=CAN.SILENT_LOOPBACK, bitrate=50000, auto_restart=False
)

# Test send/receive message
print("Loopback Test: no filter - STD")
send_and_check(can, "No filter", 0x100, True)

# Set filter1
print("Loopback Test: one filter - STD")
can.set_filters(0, CAN.FILTER_ADDRESS, [0x101, 0])
send_and_check(can, "Passing Message", 0x101, True)
send_and_check(can, "Blocked Message", 0x100, False)

# Set filter2
print("Loopback Test: second filter - STD")
can.set_filters(0, CAN.FILTER_ADDRESS, [0x102, 0])
send_and_check(can, "Passing Message - Bank 1", 0x102, True)
send_and_check(can, "Passing Message - Bank 0", 0x101, True)
send_and_check(can, "Blocked Message", 0x100, False)

# Remove filter
print("Loopback Test: clear filter - STD")
can.clearfilter()
send_and_check(can, "Passing Message - Bank 1", 0x102, True)
send_and_check(can, "Passing Message - Bank 0", 0x101, True)
send_and_check(can, "Passing any Message", 0x100, True)

# Extended message tests
# Move to Extended
can = CAN(
    0,
    extframe=True,
    mode=CAN.SILENT_LOOPBACK,
    bitrate=500_000,
    tx=4,
    rx=5,
    auto_restart=False,
)

# Test send/receive message
print("Loopback Test: no filter - Extd")
send_and_check(can, "No filter", 0x100, True, extended=True)

# Set filter1
print("Loopback Test: one filter - Extd")
can.set_filters(0, CAN.FILTER_ADDRESS, [0x101, 0], extframe=True)
send_and_check(can, "Passing Message", 0x101, True, extended=True)
send_and_check(can, "Blocked Message", 0x100, False, extended=True)

# Remove filter
print("Loopback Test: clear filter - Extd")
can.clearfilter()
send_and_check(can, "Passing Message - Bank 0", 0x101, True, extended=True)
send_and_check(can, "Passing any Message", 0x100, True, extended=True)
