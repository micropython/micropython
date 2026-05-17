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
_IRQ_GATTC_DESCRIPTOR_RESULT = const(13)
_IRQ_GATTC_DESCRIPTOR_DONE = const(14)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)
_IRQ_GATTC_INDICATE = const(19)
_IRQ_GATTS_INDICATE_DONE = const(20)

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")
DESC_UUID = bluetooth.UUID("00000000-1111-2222-3333-555555555555")
CHAR = (
    CHAR_UUID,
    bluetooth.FLAG_READ | bluetooth.FLAG_WRITE,
    ((DESC_UUID, bluetooth.FLAG_READ | bluetooth.FLAG_WRITE | bluetooth.FLAG_WRITE_NO_RESPONSE),),
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
        # conn_handle, end_handle, value_handle, properties, uuid = data
        if data[-1] == CHAR_UUID:
            print("_IRQ_GATTC_CHARACTERISTIC_RESULT", data[-1])
            waiting_events[event] = (data[2], data[1])
        else:
            return
    elif event == _IRQ_GATTC_CHARACTERISTIC_DONE:
        print("_IRQ_GATTC_CHARACTERISTIC_DONE")
    elif event == _IRQ_GATTC_DESCRIPTOR_RESULT:
        # conn_handle, dsc_handle, uuid = data
        if data[-1] == DESC_UUID:
            print("_IRQ_GATTC_DESCRIPTOR_RESULT", data[-1])
            waiting_events[event] = data[1]
        else:
            return
    elif event == _IRQ_GATTC_DESCRIPTOR_DONE:
        print("_IRQ_GATTC_DESCRIPTOR_DONE")
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
    ((char_handle, desc_handle),) = ble.gatts_register_services(SERVICES)
    multitest.next()
    try:
        print("gap_advertise")
        ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")

        multitest.broadcast("peripheral:adv")

        # Write initial characteristic value.
        ble.gatts_write(char_handle, "periph0")
        ble.gatts_write(desc_handle, "desc_periph0")

        # Wait for central to connect to us.
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # Write to characteristic.
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        # Write to descriptor.
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        # Write-without-response to descriptor.
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)

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
        value_handle, end_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Discover characteristics.
        ble.gattc_discover_descriptors(conn_handle, value_handle, end_handle)
        desc_handle = wait_for_event(_IRQ_GATTC_DESCRIPTOR_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_DESCRIPTOR_DONE, TIMEOUT_MS)

        # Issue read of characteristic, should get initial value.
        print("gattc_read characteristic")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Issue read of descriptor, should get initial value.
        print("gattc_read descriptor")
        ble.gattc_read(conn_handle, desc_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Write-with-response to the characteristic.
        print("gattc_write characteristic")
        ble.gattc_write(conn_handle, value_handle, "central0", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)

        # Write-with-response to the descriptor.
        print("gattc_write descriptor")
        ble.gattc_write(conn_handle, desc_handle, "desc_central0", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)

        # Write-without-response to the descriptor.
        print("gattc_write descriptor")
        ble.gattc_write(conn_handle, desc_handle, "desc_central1", 0)

        time.sleep_ms(500)

        # Disconnect from peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
