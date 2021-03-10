# Write characteristic from central to peripheral and time data rate.

from micropython import const
import time, machine, bluetooth

TIMEOUT_MS = 2000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_MTU_EXCHANGED = const(21)

# How long to run the test for.
_NUM_NOTIFICATIONS = const(40)
_MTU_SIZE = const(131)
_CHAR_SIZE = const(_MTU_SIZE - 3)

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")
CHAR = (CHAR_UUID, bluetooth.FLAG_WRITE | bluetooth.FLAG_WRITE_NO_RESPONSE)
SERVICE = (SERVICE_UUID, (CHAR,))
SERVICES = (SERVICE,)

packet_sequence = 0
waiting_events = {}


def irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        waiting_events[event] = data[0]
    elif event == _IRQ_PERIPHERAL_CONNECT:
        waiting_events[event] = data[0]
    elif event == _IRQ_GATTS_WRITE:
        global packet_sequence
        conn_handle, attr_handle = data
        data = ble.gatts_read(attr_handle)
        if not (data[0] == packet_sequence and data[-1] == (256 - packet_sequence) & 0xFF):
            print("_IRQ_GATTS_WRITE data invalid:", packet_sequence, data)
        elif packet_sequence % 10 == 0:
            print("_IRQ_GATTS_WRITE", packet_sequence)
        packet_sequence += 1
    elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
        # conn_handle, def_handle, value_handle, properties, uuid = data
        if data[-1] == CHAR_UUID:
            waiting_events[event] = data[2]
        else:
            return
    elif event == _IRQ_MTU_EXCHANGED:
        # ATT MTU exchange complete (either initiated by us or the remote device).
        conn_handle, mtu = data
        print("_IRQ_MTU_EXCHANGED:", mtu)

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
    ble.gatts_set_buffer(char_handle, _CHAR_SIZE)
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect to us.
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
        # Wait for central to disconnect us.
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, 30000)
        print("final packet_sequence:", packet_sequence)
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    global packet_sequence
    ((char_handle,),) = ble.gatts_register_services(SERVICES)
    multitest.next()
    try:
        # Connect to peripheral and then disconnect.
        print("gap_connect")
        ble.config(mtu=_MTU_SIZE)
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)
        ble.gattc_exchange_mtu(conn_handle)

        # Discover characteristics.
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Send data!
        data = bytearray(ord("A") + (i % 64) for i in range(_CHAR_SIZE))
        for mode in (0, 1):
            ticks_start = time.ticks_ms()
            for i in range(_NUM_NOTIFICATIONS):
                data[0] = packet_sequence
                data[-1] = 256 - packet_sequence
                if packet_sequence % 10 == 0:
                    print("gattc_write", packet_sequence)
                if mode == 0:
                    while True:
                        try:
                            ble.gattc_write(conn_handle, value_handle, data, mode)
                            break
                        except OSError:
                            pass
                else:
                    ble.gattc_write(conn_handle, value_handle, data, mode)
                    wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)
                packet_sequence += 1

            ticks_end = time.ticks_ms()
            ticks_total = time.ticks_diff(ticks_end, ticks_start)

            print(
                "Did {} writes in {} ms. {} ms/write, {} bytes/sec".format(
                    _NUM_NOTIFICATIONS,
                    ticks_total,
                    ticks_total / _NUM_NOTIFICATIONS,
                    _NUM_NOTIFICATIONS * len(data) * 1000 // ticks_total,
                )
            )

            time.sleep_ms(100)

        # DIsconnect the peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, 20000)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.active(1)
ble.irq(irq)
