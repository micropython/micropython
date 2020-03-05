# Test BLE GAP connect/disconnect

from micropython import const
import time, machine, bluetooth

TIMEOUT_MS = 4000

_IRQ_CENTRAL_CONNECT = const(1 << 0)
_IRQ_CENTRAL_DISCONNECT = const(1 << 1)
_IRQ_PERIPHERAL_CONNECT = const(1 << 6)
_IRQ_PERIPHERAL_DISCONNECT = const(1 << 7)

last_event = None
last_data = None


def irq(event, data):
    global last_event, last_data
    last_event = event
    last_data = data
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")


def wait_for_event(event, timeout_ms):
    t0 = time.ticks_ms()
    while last_event != event and time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        machine.idle()


# Acting in peripheral role.
def instance0():
    multitest.globals(BDADDR=ble.config("mac"))
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect, then wait for it to disconnect.
        wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
        if last_event != _IRQ_CENTRAL_DISCONNECT:
            return

        # Start advertising again.
        ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")

        # Wait for central to connect, then disconnect it.
        wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
        if last_event != _IRQ_CENTRAL_CONNECT:
            return
        print("gap_disconnect:", ble.gap_disconnect(last_data[0]))
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    multitest.next()
    try:
        # Connect to peripheral and then disconnect.
        print("gap_connect")
        ble.gap_connect(0, BDADDR)
        wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)
        if last_event != _IRQ_PERIPHERAL_CONNECT:
            return
        print("gap_disconnect:", ble.gap_disconnect(last_data[0]))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
        if last_event != _IRQ_PERIPHERAL_DISCONNECT:
            return

        # Wait for peripheral to start advertising again.
        time.sleep_ms(100)

        # Connect to peripheral and then let the peripheral disconnect us.
        print("gap_connect")
        ble.gap_connect(0, BDADDR)
        wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)
        if last_event != _IRQ_PERIPHERAL_CONNECT:
            return
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
