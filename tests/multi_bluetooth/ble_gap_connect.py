# Test BLE GAP connect/disconnect

from micropython import const
import time, machine, bluetooth

TIMEOUT_MS = 4000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)

central_connected = False
central_disconnected = False
peripheral_connected = False
peripheral_disconnected = False
conn_handle = None


def irq(event, data):
    global central_connected, central_disconnected, peripheral_connected, peripheral_disconnected, conn_handle
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
        central_connected = True
        conn_handle = data[0]
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
        central_disconnected = True
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
        peripheral_connected = True
        conn_handle = data[0]
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
        peripheral_disconnected = True
        remote_addr = data[0]


def wait_for(fn, timeout_ms):
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        if fn():
            return True
        machine.idle()
    return False


# Acting in peripheral role.
def instance0():
    global central_connected, central_disconnected

    multitest.globals(BDADDR=ble.config("mac"))
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect, then wait for it to disconnect.
        if not wait_for(lambda: central_connected, TIMEOUT_MS):
            return
        if not wait_for(lambda: central_disconnected, TIMEOUT_MS):
            return

        central_connected = False
        central_disconnected = False

        # Start advertising again.
        print("gap_advertise")
        ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")

        # Wait for central to connect, then disconnect it.
        if not wait_for(lambda: central_connected, TIMEOUT_MS):
            return
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        if not wait_for(lambda: central_disconnected, TIMEOUT_MS):
            return
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    global peripheral_connected, peripheral_disconnected

    multitest.next()
    try:
        # Connect to peripheral and then disconnect.
        print("gap_connect")
        ble.gap_connect(*BDADDR)
        if not wait_for(lambda: peripheral_connected, TIMEOUT_MS):
            return
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        if not wait_for(lambda: peripheral_disconnected, TIMEOUT_MS):
            return

        peripheral_connected = False
        peripheral_disconnected = False

        # Wait for peripheral to start advertising again.
        time.sleep_ms(100)

        # Connect to peripheral and then let the peripheral disconnect us.
        print("gap_connect")
        ble.gap_connect(*BDADDR)
        if not wait_for(lambda: peripheral_connected, TIMEOUT_MS):
            return
        if not wait_for(lambda: peripheral_disconnected, TIMEOUT_MS):
            return
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
