# Test BLE GAP device name get/set

from micropython import const
import time, machine, bluetooth

TIMEOUT_MS = 5000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)

GAP_DEVICE_NAME_UUID = bluetooth.UUID(0x2A00)

waiting_event = None
waiting_data = None
value_handle = 0


def irq(event, data):
    global waiting_event, waiting_data, value_handle
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
    elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
        if data[-1] == GAP_DEVICE_NAME_UUID:
            print("_IRQ_GATTC_CHARACTERISTIC_RESULT", data[-1])
            value_handle = data[2]
    elif event == _IRQ_GATTC_READ_RESULT:
        print("_IRQ_GATTC_READ_RESULT", data[-1])

    if waiting_event is not None:
        if isinstance(waiting_event, int) and event == waiting_event:
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

    # Test setting and getting the GAP device name before registering services.
    ble.config(gap_name="GAP_NAME")
    print(ble.config("gap_name"))

    # Create an empty service and start advertising.
    ble.gatts_register_services([])
    print("gap_advertise")
    multitest.next()

    try:
        # Do multiple iterations to test changing the name.
        for iteration in range(2):
            # Set the GAP device name and start advertising.
            ble.config(gap_name="GAP_NAME{}".format(iteration))
            print(ble.config("gap_name"))
            ble.gap_advertise(20_000)

            # Wait for central to connect, then wait for it to disconnect.
            if not wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS):
                return
            if not wait_for_event(_IRQ_CENTRAL_DISCONNECT, 4 * TIMEOUT_MS):
                return
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    multitest.next()
    try:
        for iteration in range(2):
            # Wait for peripheral to start advertising.
            time.sleep_ms(500)

            # Connect to peripheral.
            print("gap_connect")
            ble.gap_connect(0, BDADDR)
            if not wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS):
                return
            conn_handle, _, _ = waiting_data

            if iteration == 0:
                print("gattc_discover_characteristics")
                ble.gattc_discover_characteristics(conn_handle, 1, 65535)
                wait_for_event(lambda: value_handle, TIMEOUT_MS)

                # Wait for all characteristic results to come in (there should be an event for it).
                time.sleep_ms(500)

            # Read the peripheral's GAP device name.
            print("gattc_read")
            ble.gattc_read(conn_handle, value_handle)
            wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

            # Disconnect from peripheral.
            print("gap_disconnect:", ble.gap_disconnect(conn_handle))
            if not wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS):
                return
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
