# Test BLE GAP connect/disconnect

from micropython import const
import time, machine, bluetooth

TIMEOUT_MS = 4000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)

waiting_event = None
waiting_data = None


def irq(event, data):
    global waiting_event, waiting_data
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")

    if waiting_event is not None:
        if event == waiting_event:
            waiting_event = None
            waiting_data = data


def wait_for_event(event, timeout_ms):
    global waiting_event, waiting_data
    waiting_event = event
    waiting_data = None

    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        if waiting_data:
            return True
        machine.idle()
    return False


# Acting in peripheral role.
def instance0():
    multitest.globals(BDADDR=ble.config("mac"))
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect, then wait for it to disconnect.
        if not wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS):
            return
        if not wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS):
            return

        # Start advertising again.
        ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")

        # Wait for central to connect, then disconnect it.
        if not wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS):
            return
        print("gap_disconnect:", ble.gap_disconnect(waiting_data[0]))
        if not wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS):
            return
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    multitest.next()
    try:
        # Connect to peripheral and then disconnect.
        print("gap_connect")
        ble.gap_connect(0, BDADDR)
        if not wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS):
            return
        print("gap_disconnect:", ble.gap_disconnect(waiting_data[0]))
        if not wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS):
            return

        # Wait for peripheral to start advertising again.
        time.sleep_ms(100)

        # Connect to peripheral and then let the peripheral disconnect us.
        print("gap_connect")
        ble.gap_connect(0, BDADDR)
        if not wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS):
            return
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
