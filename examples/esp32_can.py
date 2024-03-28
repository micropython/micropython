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
dev = CAN(0, extframe=False, tx=5, rx=4, mode=CAN.SILENT_LOOPBACK, baudrate=50000, auto_restart=False)

# Test send/receive message
print("Loopback Test: no filter - STD")
send_and_check(dev, "No filter", 0x100, True)

# Set filter1
print("Loopback Test: one filter - STD")
dev.setfilter(0, CAN.FILTER_ADDRESS, [0x101, 0])
send_and_check(dev, "Passing Message", 0x101, True)
send_and_check(dev, "Blocked Message", 0x100, False)

# Set filter2
print("Loopback Test: second filter - STD")
dev.setfilter(0, CAN.FILTER_ADDRESS, [0x102, 0])
send_and_check(dev, "Passing Message - Bank 1", 0x102, True)
send_and_check(dev, "Passing Message - Bank 0", 0x101, True)
send_and_check(dev, "Blocked Message", 0x100, False)

# Remove filter
print("Loopback Test: clear filter - STD")
dev.clearfilter()
send_and_check(dev, "Passing Message - Bank 1", 0x102, True)
send_and_check(dev, "Passing Message - Bank 0", 0x101, True)
send_and_check(dev, "Passing any Message", 0x100, True)

# Extended message tests
# Move to Extended
dev = CAN(0,
          extframe=True,
          mode=CAN.SILENT_LOOPBACK,
          baudrate=CAN.BAUDRATE_500k,
          tx_io=18, rx_io=19, auto_restart=False)

# Test send/receive message
print("Loopback Test: no filter - Extd")
send_and_check(dev, "No filter", 0x100, True, extended=True)

# Set filter1
print("Loopback Test: one filter - Extd")
dev.setfilter(0, CAN.FILTER_ADDRESS, [0x101, 0], extframe=True)
send_and_check(dev, "Passing Message", 0x101, True, extended=True)
send_and_check(dev, "Blocked Message", 0x100, False, extended=True)

# Remove filter
print("Loopback Test: clear filter - Extd")
dev.clearfilter()
send_and_check(dev, "Passing Message - Bank 0", 0x101, True, extended=True)
send_and_check(dev, "Passing any Message", 0x100, True, extended=True)
