# Test characteristic read/write/notify from both GATTS and GATTC.

from micropython import const
import time, machine, bluetooth

TIMEOUT_MS = 5000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)
_IRQ_GATTC_INDICATE = const(19)
_IRQ_GATTS_INDICATE_DONE = const(20)

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")
CHAR = (
    CHAR_UUID,
    bluetooth.FLAG_READ
    | bluetooth.FLAG_WRITE
    | bluetooth.FLAG_WRITE_NO_RESPONSE
    | bluetooth.FLAG_NOTIFY
    | bluetooth.FLAG_INDICATE,
)
SERVICE = (
    SERVICE_UUID,
    (CHAR,),
)
SERVICES = (SERVICE,)

waiting_events = {}


def irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_GATTS_WRITE:
        print("_IRQ_GATTS_WRITE", ble.gatts_read(data[-1]))
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
    elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
        # conn_handle, def_handle, value_handle, properties, uuid = data
        if data[-1] == CHAR_UUID:
            print("_IRQ_GATTC_CHARACTERISTIC_RESULT", data[-1])
            waiting_events[event] = data[2]
        else:
            return
    elif event == _IRQ_GATTC_CHARACTERISTIC_DONE:
        print("_IRQ_GATTC_CHARACTERISTIC_DONE")
    elif event == _IRQ_GATTC_READ_RESULT:
        print("_IRQ_GATTC_READ_RESULT", bytes(data[-1]))
    elif event == _IRQ_GATTC_READ_DONE:
        print("_IRQ_GATTC_READ_DONE", data[-1])
    elif event == _IRQ_GATTC_WRITE_DONE:
        print("_IRQ_GATTC_WRITE_DONE", data[-1])
    elif event == _IRQ_GATTC_NOTIFY:
        print("_IRQ_GATTC_NOTIFY", bytes(data[-1]))
    elif event == _IRQ_GATTC_INDICATE:
        print("_IRQ_GATTC_INDICATE", bytes(data[-1]))
    elif event == _IRQ_GATTS_INDICATE_DONE:
        print("_IRQ_GATTS_INDICATE_DONE", data[-1])

    if event not in waiting_events:
        waiting_events[event] = None


def wait_for_event(event, timeout_ms):
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        if event in waiting_events:
            return waiting_events.pop(event)
        machine.idle()
    raise ValueError("Timeout waiting for {}".format(event))


# Acting in peripheral role.
def instance0():
    multitest.globals(BDADDR=ble.config("mac"))
    ((char_handle,),) = ble.gatts_register_services(SERVICES)
    multitest.next()
    try:
        print("gap_advertise")
        ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
        multitest.broadcast("peripheral:adv")

        # Write initial characteristic value.
        ble.gatts_write(char_handle, "periph0")

        # Wait for central to connect to us.
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # A

        # Wait for a write to the characteristic from the central,
        # then reply with a notification.
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        print("gatts_write")
        ble.gatts_write(char_handle, "periph1")
        print("gatts_notify")
        ble.gatts_notify(conn_handle, char_handle)

        # B

        # Wait for a write to the characteristic from the central,
        # then reply with value-included notification.
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        print("gatts_notify")
        ble.gatts_notify(conn_handle, char_handle, "periph2")

        # C

        # Wait for a write to the characteristic from the central,
        # then reply with an indication.
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        print("gatts_write")
        ble.gatts_write(char_handle, "periph3")
        print("gatts_indicate")
        ble.gatts_indicate(conn_handle, char_handle)
        wait_for_event(_IRQ_GATTS_INDICATE_DONE, TIMEOUT_MS)

        # D
        # Wait for a write to the characteristic from the central,
        # then reply with no-payload notification.
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        print("gatts_notify")
        ble.gatts_notify(conn_handle, char_handle)

        # Wait for the central to disconnect.
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    multitest.next()
    try:
        multitest.wait("peripheral:adv")

        # Connect to peripheral and then disconnect.
        print("gap_connect")
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        # Discover characteristics.
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Issue read of characteristic, should get initial value.
        print("gattc_read")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Write to the characteristic, which will trigger a notification.
        print("gattc_write")
        ble.gattc_write(conn_handle, value_handle, "central0", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # A
        wait_for_event(_IRQ_GATTC_NOTIFY, TIMEOUT_MS)
        print("gattc_read")  # Read the new value set immediately before notification.
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Write to the characteristic, which will trigger a value-included notification.
        print("gattc_write")
        ble.gattc_write(conn_handle, value_handle, "central1", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # B
        wait_for_event(_IRQ_GATTC_NOTIFY, TIMEOUT_MS)
        print("gattc_read")  # Read value should be unchanged.
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Write to the characteristic, which will trigger an indication.
        print("gattc_write")
        ble.gattc_write(conn_handle, value_handle, "central2", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # C
        wait_for_event(_IRQ_GATTC_INDICATE, TIMEOUT_MS)
        print("gattc_read")  # Read the new value set immediately before indication.
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Write-without-response, which will trigger another notification with that value.
        ble.gattc_write(conn_handle, value_handle, "central3", 0)
        # D
        wait_for_event(_IRQ_GATTC_NOTIFY, TIMEOUT_MS)

        # Disconnect from peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
