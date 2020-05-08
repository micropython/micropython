# Test characteristic read/write/notify from both GATTS and GATTC.

from micropython import const
import time, machine, bluetooth

TIMEOUT_MS = 5000

_IRQ_CENTRAL_CONNECT = const(1 << 0)
_IRQ_CENTRAL_DISCONNECT = const(1 << 1)
_IRQ_GATTS_WRITE = const(1 << 2)
_IRQ_PERIPHERAL_CONNECT = const(1 << 6)
_IRQ_PERIPHERAL_DISCONNECT = const(1 << 7)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(1 << 9)
_IRQ_GATTC_READ_RESULT = const(1 << 11)
_IRQ_GATTC_WRITE_STATUS = const(1 << 12)
_IRQ_GATTC_NOTIFY = const(1 << 13)

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")
CHAR = (
    CHAR_UUID,
    bluetooth.FLAG_READ | bluetooth.FLAG_WRITE | bluetooth.FLAG_NOTIFY,
)
SERVICE = (
    SERVICE_UUID,
    (CHAR,),
)
SERVICES = (SERVICE,)

last_event = None
last_data = None
value_handle = 0


def irq(event, data):
    global last_event, last_data, value_handle
    last_event = event
    last_data = data
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_GATTS_WRITE:
        print("_IRQ_GATTS_WRITE", ble.gatts_read(data[-1]))
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
    elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
        # conn_handle, def_handle, value_handle, properties, uuid = data
        if data[-1] == CHAR_UUID:
            print("_IRQ_GATTC_CHARACTERISTIC_RESULT", data[-1])
            value_handle = data[2]
    elif event == _IRQ_GATTC_READ_RESULT:
        print("_IRQ_GATTC_READ_RESULT", data[-1])
    elif event == _IRQ_GATTC_WRITE_STATUS:
        print("_IRQ_GATTC_WRITE_STATUS", data[-1])
    elif event == _IRQ_GATTC_NOTIFY:
        print("_IRQ_GATTC_NOTIFY", data[-1])


def wait_for_event(event, timeout_ms):
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        if isinstance(event, int):
            if last_event == event:
                break
        elif event():
            break
        machine.idle()


# Acting in peripheral role.
def instance0():
    multitest.globals(BDADDR=ble.config("mac"))
    ((char_handle,),) = ble.gatts_register_services(SERVICES)
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Write initial characteristic value.
        ble.gatts_write(char_handle, "periph0")

        # Wait for central to connect to us.
        wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
        if last_event != _IRQ_CENTRAL_CONNECT:
            return
        conn_handle, _, _ = last_data

        # Wait for a write to the characteristic from the central.
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)

        # Wait a bit, then write the characteristic and notify it.
        time.sleep_ms(1000)
        ble.gatts_write(char_handle, "periph1")
        ble.gatts_notify(conn_handle, char_handle)

        # Wait for a write to the characteristic from the central.
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)

        # Wait a bit, then notify a new value on the characteristic.
        time.sleep_ms(1000)
        ble.gatts_notify(conn_handle, char_handle, "periph2")

        # Wait for the central to disconnect.
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
        conn_handle, _, _ = last_data

        # Discover characteristics.
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        wait_for_event(lambda: value_handle, TIMEOUT_MS)

        # Issue read of characteristic, should get initial value.
        print("gattc_read")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Write to the characteristic, and ask for a response.
        print("gattc_write")
        ble.gattc_write(conn_handle, value_handle, "central0", 1)
        wait_for_event(_IRQ_GATTC_WRITE_STATUS, TIMEOUT_MS)

        # Wait for a notify, then read new value.
        wait_for_event(_IRQ_GATTC_NOTIFY, TIMEOUT_MS)
        print("gattc_read")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Write to the characteristic, and ask for a response.
        print("gattc_write")
        ble.gattc_write(conn_handle, value_handle, "central1", 1)
        wait_for_event(_IRQ_GATTC_WRITE_STATUS, TIMEOUT_MS)

        # Wait for a notify (should have new data), then read old value (should be unchanged).
        wait_for_event(_IRQ_GATTC_NOTIFY, TIMEOUT_MS)
        print("gattc_read")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Disconnect from peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
