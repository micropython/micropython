# Ping-pong GATT notifications between two devices.

from micropython import const
import time, machine, bluetooth

TIMEOUT_MS = 2000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_NOTIFY = const(18)

# How long to run the test for.
_NUM_NOTIFICATIONS = const(50)

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")
CHAR = (
    CHAR_UUID,
    bluetooth.FLAG_NOTIFY,
)
SERVICE = (
    SERVICE_UUID,
    (CHAR,),
)
SERVICES = (SERVICE,)

is_central = False

waiting_events = {}


def irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        waiting_events[event] = data[0]
    elif event == _IRQ_PERIPHERAL_CONNECT:
        waiting_events[event] = data[0]
    elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
        # conn_handle, def_handle, value_handle, properties, uuid = data
        if data[-1] == CHAR_UUID:
            waiting_events[event] = data[2]
        else:
            return
    elif event == _IRQ_GATTC_NOTIFY:
        if is_central:
            conn_handle, value_handle, notify_data = data
            ble.gatts_notify(conn_handle, value_handle, b"central" + notify_data)

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
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect to us.
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # Discover characteristics.
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Give the central enough time to discover chars.
        time.sleep_ms(500)

        ticks_start = time.ticks_ms()

        for i in range(_NUM_NOTIFICATIONS):
            # Send a notification and wait for a response.
            ble.gatts_notify(conn_handle, value_handle, "peripheral" + str(i))
            wait_for_event(_IRQ_GATTC_NOTIFY, TIMEOUT_MS)

        ticks_end = time.ticks_ms()
        ticks_total = time.ticks_diff(ticks_end, ticks_start)
        print(
            "Acknowledged {} notifications in {} ms. {} ms/notification.".format(
                _NUM_NOTIFICATIONS, ticks_total, ticks_total // _NUM_NOTIFICATIONS
            )
        )

        # Disconnect the central.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    global is_central
    is_central = True
    ((char_handle,),) = ble.gatts_register_services(SERVICES)
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

        # The IRQ handler will respond to each notification.

        # Wait for the peripheral to disconnect us.
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, 20000)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
