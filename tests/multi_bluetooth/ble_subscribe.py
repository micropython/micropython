# Test for sending notifications to subscribed clients.

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

_CCCD_UUID = bluetooth.UUID(const(0x2902))

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")
CHAR = (
    CHAR_UUID,
    bluetooth.FLAG_READ | bluetooth.FLAG_WRITE | bluetooth.FLAG_NOTIFY | bluetooth.FLAG_INDICATE,
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
    elif event == _IRQ_GATTC_DESCRIPTOR_RESULT:
        # conn_handle, dsc_handle, uuid = data
        if data[-1] == _CCCD_UUID:
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
        print("_IRQ_GATTC_NOTIFY", bytes(data[-1]))

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
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")  # \x04\x09MPY
    multitest.next()
    try:
        # Write initial characteristic value (will be read by client).
        ble.gatts_write(char_handle, "periph0")  ###

        # Wait for central to connect to us.
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS * 10)

        # A
        # Wait for a write to the characteristic from the central (to synchronise).
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        print("sync A")
        # This should be local-only.
        ble.gatts_write(char_handle, "periph1")
        time.sleep_ms(100)
        # Update local-only, then force notify.
        ble.gatts_write(char_handle, "periph2")
        ble.gatts_notify(conn_handle, char_handle)  ###
        time.sleep_ms(100)
        # Update local and notify subscribers. No notification should be sent.
        ble.gatts_write(char_handle, "periph3", True)
        time.sleep_ms(100)
        multitest.broadcast("A")

        # B
        # Synchronise with the client (which should now be subscribed for notify).
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        print("sync B")
        # This should be local-only (send_update=False).
        ble.gatts_write(char_handle, "periph4", False)
        time.sleep_ms(100)
        # This should notify the subscribed client.
        ble.gatts_write(char_handle, "periph5", True)  ###
        time.sleep_ms(100)
        multitest.broadcast("B")

        # C
        # Synchronise with the client (which should now be subscribed for indicate).
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        print("sync C")
        # This should be local-only (send_update=False).
        ble.gatts_write(char_handle, "periph6", False)
        time.sleep_ms(100)
        # This should indicate the subscribed client.
        ble.gatts_write(char_handle, "periph7", True)  ###
        time.sleep_ms(100)
        multitest.broadcast("C")

        # D
        # Synchronise with the client (which should now be unsubscribed).
        wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
        print("sync D")
        # This should be local-only (send_update=False).
        ble.gatts_write(char_handle, "periph8", False)
        time.sleep_ms(100)
        # This should be local-only (no more subscribers).
        ble.gatts_write(char_handle, "periph9", True)
        time.sleep_ms(100)
        # Update local-only, then another force notify.
        ble.gatts_write(char_handle, "periph10")
        ble.gatts_notify(conn_handle, char_handle)  ###
        time.sleep_ms(100)
        multitest.broadcast("D")

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
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        # Discover characteristics.
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Discover CCCD.
        ble.gattc_discover_descriptors(conn_handle, value_handle, value_handle + 5)
        cccd_handle = wait_for_event(_IRQ_GATTC_DESCRIPTOR_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_DESCRIPTOR_DONE, TIMEOUT_MS)

        # Issue read of characteristic, should get initial value.
        print("gattc_read")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # While the four states are active, all incoming notifications
        # and indications will be printed by the event handler. We
        # should only expect to see certain ones.

        # Start unsubscribed.
        # Write to the characteristic (triggers A).
        print("gattc_write")
        ble.gattc_write(conn_handle, value_handle, "central0", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # Wait for A to complete.
        multitest.wait("A")

        # Subscribe for notify.
        ble.gattc_write(conn_handle, cccd_handle, b"\x01\x00", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # Write to the characteristic (triggers B).
        print("gattc_write")
        ble.gattc_write(conn_handle, value_handle, "central1", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # Wait for B to complete.
        multitest.wait("B")

        # Subscribe for indicate.
        ble.gattc_write(conn_handle, cccd_handle, b"\x02\x00", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # Write to the characteristic (triggers C).
        print("gattc_write")
        ble.gattc_write(conn_handle, value_handle, "central2", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # Wait for C to complete.
        multitest.wait("C")

        # Unsubscribe.
        ble.gattc_write(conn_handle, cccd_handle, b"\x00\x00", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # Write to the characteristic (triggers D).
        print("gattc_write")
        ble.gattc_write(conn_handle, value_handle, "central3", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
        # Wait for D to complete.
        multitest.wait("D")

        # Disconnect from peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
