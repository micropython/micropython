# Test MTU exchange (initiated by peripheral) and the effect on notify and
# write size.
#
# This is not supported on btstack (only the central may initiate MTU exchange).
#
# Test | Requested | Preferred | Result | Notes
#   0  |  290 (P)  |  256 (C)  |  256   |
#   1  |  290 (P)  |  190 (C)  |  190   |
#   2  |  290 (P)  |  350 (C)  |  290   |
#
# For each connection a notification is sent by the server (peripheral) and a characteristic
# is written by the client (central) to ensure that the expected size is transmitted.

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
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)
_IRQ_MTU_EXCHANGED = const(21)

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

waiting_events = {}


def irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_GATTS_WRITE:
        print("_IRQ_GATTS_WRITE")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
    elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
        if data[-1] == CHAR_UUID:
            print("_IRQ_GATTC_CHARACTERISTIC_RESULT", data[-1])
            waiting_events[event] = data[2]
        else:
            return
    elif event == _IRQ_GATTC_CHARACTERISTIC_DONE:
        print("_IRQ_GATTC_CHARACTERISTIC_DONE")
    elif event == _IRQ_GATTC_WRITE_DONE:
        print("_IRQ_GATTC_WRITE_DONE")
    elif event == _IRQ_GATTC_NOTIFY:
        print("_IRQ_GATTC_NOTIFY", len(data[-1]), chr(data[-1][0]))
    elif event == _IRQ_MTU_EXCHANGED:
        print("_IRQ_MTU_EXCHANGED", data[-1])
        waiting_events[event] = data[-1]

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
    ble.gatts_set_buffer(char_handle, 500, False)
    multitest.next()
    try:
        for i in range(3):
            ble.config(mtu=290)

            print("gap_advertise")
            ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
            multitest.broadcast(f"peripheral:adv:{i}")

            # Wait for central to connect to us.
            conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

            # Peripheral-initiated mtu exchange.
            print("gattc_exchange_mtu")
            ble.gattc_exchange_mtu(conn_handle)

            mtu = wait_for_event(_IRQ_MTU_EXCHANGED, TIMEOUT_MS)

            multitest.wait(f"client:discovery:{i}")

            print("gatts_notify")
            ble.gatts_notify(conn_handle, char_handle, str(i) * 64)

            wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)

            print("gatts_read")
            data = ble.gatts_read(char_handle)
            print("characteristic len:", len(data), chr(data[0]))

            # Wait for the central to disconnect.
            wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)

            print("gap_advertise")
            ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")

    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    multitest.next()
    try:
        default_mtus = [256, 190, 350]
        for i in range(3):
            ble.config(mtu=default_mtus[i])

            multitest.wait(f"peripheral:adv:{i}")

            print("gap_connect")
            ble.gap_connect(BDADDR[0], BDADDR[1], 5000)
            conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

            mtu = wait_for_event(_IRQ_MTU_EXCHANGED, TIMEOUT_MS)

            print("gattc_discover_characteristics")
            ble.gattc_discover_characteristics(conn_handle, 1, 65535)
            value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
            wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

            multitest.broadcast(f"client:discovery:{i}")

            wait_for_event(_IRQ_GATTC_NOTIFY, TIMEOUT_MS)

            # Write 20 more than the MTU to test truncation.
            print("gattc_write")
            ble.gattc_write(conn_handle, value_handle, chr(ord("a") + i) * (mtu + 20), 1)
            wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)

            # Disconnect from peripheral.
            print("gap_disconnect:", ble.gap_disconnect(conn_handle))
            wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
