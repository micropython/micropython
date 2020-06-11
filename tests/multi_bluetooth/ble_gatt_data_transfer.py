# Test GATTC/S data transfer between peripheral and central, and use of gatts_set_buffer()

from micropython import const
import time, machine, bluetooth

TIMEOUT_MS = 5000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_SERVICE_RESULT = const(9)
_IRQ_GATTC_SERVICE_DONE = const(10)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)

SERVICE_UUID = bluetooth.UUID("00000001-1111-2222-3333-444444444444")
CHAR_CTRL_UUID = bluetooth.UUID("00000002-1111-2222-3333-444444444444")
CHAR_RX_UUID = bluetooth.UUID("00000003-1111-2222-3333-444444444444")
CHAR_TX_UUID = bluetooth.UUID("00000004-1111-2222-3333-444444444444")
CHAR_CTRL = (CHAR_CTRL_UUID, bluetooth.FLAG_WRITE | bluetooth.FLAG_NOTIFY)
CHAR_RX = (CHAR_RX_UUID, bluetooth.FLAG_WRITE | bluetooth.FLAG_WRITE_NO_RESPONSE)
CHAR_TX = (CHAR_TX_UUID, bluetooth.FLAG_NOTIFY)
SERVICE = (SERVICE_UUID, (CHAR_CTRL, CHAR_RX, CHAR_TX))
SERVICES = (SERVICE,)

waiting_event = None
waiting_data = None
ctrl_value_handle = 0
rx_value_handle = 0
tx_value_handle = 0


def irq(event, data):
    global waiting_event, waiting_data, ctrl_value_handle, rx_value_handle, tx_value_handle
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_GATTS_WRITE:
        print("_IRQ_GATTS_WRITE")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
    elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
        if data[-1] == CHAR_CTRL_UUID:
            print("_IRQ_GATTC_CHARACTERISTIC_RESULT", data[-1])
            ctrl_value_handle = data[2]
        elif data[-1] == CHAR_RX_UUID:
            print("_IRQ_GATTC_CHARACTERISTIC_RESULT", data[-1])
            rx_value_handle = data[2]
        elif data[-1] == CHAR_TX_UUID:
            print("_IRQ_GATTC_CHARACTERISTIC_RESULT", data[-1])
            tx_value_handle = data[2]
    elif event == _IRQ_GATTC_CHARACTERISTIC_DONE:
        print("_IRQ_GATTC_CHARACTERISTIC_DONE")
    elif event == _IRQ_GATTC_READ_RESULT:
        print("_IRQ_GATTC_READ_RESULT", data[-1])
    elif event == _IRQ_GATTC_READ_DONE:
        print("_IRQ_GATTC_READ_DONE", data[-1])
    elif event == _IRQ_GATTC_WRITE_DONE:
        print("_IRQ_GATTC_WRITE_DONE", data[-1])
    elif event == _IRQ_GATTC_NOTIFY:
        print("_IRQ_GATTC_NOTIFY", data[-1])

    if waiting_event is not None:
        if (isinstance(waiting_event, int) and event == waiting_event) or (
            not isinstance(waiting_event, int) and waiting_event(event, data)
        ):
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
    ((char_ctrl_handle, char_rx_handle, char_tx_handle),) = ble.gatts_register_services(SERVICES)

    # Increase the size of the rx buffer and enable append mode.
    ble.gatts_set_buffer(char_rx_handle, 100, True)

    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect to us.
        if not wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS):
            return
        conn_handle, _, _ = waiting_data

        # Wait for the central to signal that it's done with its part of the test.
        wait_for_event(
            lambda event, data: event == _IRQ_GATTS_WRITE and data[1] == char_ctrl_handle,
            2 * TIMEOUT_MS,
        )

        # Read all accumulated data from the central.
        print("gatts_read:", ble.gatts_read(char_rx_handle))

        # Notify the central a few times.
        for i in range(4):
            time.sleep_ms(300)
            ble.gatts_notify(conn_handle, char_tx_handle, "message{}".format(i))

        # Notify the central that we are done with our part of the test.
        time.sleep_ms(300)
        ble.gatts_notify(conn_handle, char_ctrl_handle, "OK")

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
        if not wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS):
            return
        conn_handle, _, _ = waiting_data

        # Discover characteristics.
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        wait_for_event(
            lambda event, data: ctrl_value_handle and rx_value_handle and tx_value_handle,
            TIMEOUT_MS,
        )
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Write to the characteristic a few times, with and without response.
        for i in range(4):
            print("gattc_write")
            ble.gattc_write(conn_handle, rx_value_handle, "central{}".format(i), i & 1)
            if i & 1:
                wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
            time.sleep_ms(400)

        # Write to say that we are done with our part of the test.
        ble.gattc_write(conn_handle, ctrl_value_handle, "OK", 1)
        wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)

        # Wait for notification that peripheral is done with its part of the test.
        wait_for_event(
            lambda event, data: event == _IRQ_GATTC_NOTIFY and data[1] == ctrl_value_handle,
            TIMEOUT_MS,
        )

        # Disconnect from peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
