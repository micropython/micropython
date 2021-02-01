# Test concurrency between filesystem access and BLE host. This is
# particularly relevant on STM32WB where the second core is stalled while
# flash operations are in progress.

from micropython import const
import time, machine, bluetooth, os

TIMEOUT_MS = 10000

LOG_PATH_INSTANCE0 = "stress_log_filesystem_0.log"
LOG_PATH_INSTANCE1 = "stress_log_filesystem_1.log"

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_GATTS_READ_REQUEST = const(4)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_SERVICE_RESULT = const(9)
_IRQ_GATTC_SERVICE_DONE = const(10)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)

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
log_file = None


def write_log(*args):
    if log_file:
        print(*args, file=log_file)
        log_file.flush()


last_file_write = 0


def periodic_log_write():
    global last_file_write
    t = time.ticks_ms()
    if time.ticks_diff(t, last_file_write) > 50:
        write_log("tick")
        last_file_write = t


def irq(event, data):
    write_log("event", event)

    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
    elif event == _IRQ_GATTC_SERVICE_RESULT:
        # conn_handle, start_handle, end_handle, uuid = data
        if data[-1] == SERVICE_UUID:
            print("_IRQ_GATTC_SERVICE_RESULT", data[3])
            waiting_events[event] = (data[1], data[2])
        else:
            return
    elif event == _IRQ_GATTC_SERVICE_DONE:
        print("_IRQ_GATTC_SERVICE_DONE")
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
    elif event == _IRQ_GATTS_WRITE:
        print("_IRQ_GATTS_WRITE")
    elif event == _IRQ_GATTS_READ_REQUEST:
        print("_IRQ_GATTS_READ_REQUEST")

    if event not in waiting_events:
        waiting_events[event] = None


def wait_for_event(event, timeout_ms):
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        periodic_log_write()
        if event in waiting_events:
            return waiting_events.pop(event)
        machine.idle()
    raise ValueError("Timeout waiting for {}".format(event))


# Acting in peripheral role.
def instance0():
    global log_file
    log_file = open(LOG_PATH_INSTANCE0, "w")
    write_log("start")
    ble.active(1)
    ble.irq(irq)
    multitest.globals(BDADDR=ble.config("mac"))
    ((char_handle,),) = ble.gatts_register_services(SERVICES)
    multitest.next()
    try:
        for repeat in range(2):
            print("gap_advertise")
            ble.gap_advertise(50_000, b"\x02\x01\x06\x04\xffMPY")
            # Wait for central to connect, do a sequence of read/write, then disconnect.
            wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
            for op in range(4):
                wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)
                wait_for_event(_IRQ_GATTS_WRITE, TIMEOUT_MS)
            wait_for_event(_IRQ_CENTRAL_DISCONNECT, 2 * TIMEOUT_MS)
    finally:
        ble.active(0)
        log_file.close()
        os.unlink(LOG_PATH_INSTANCE0)


# Acting in central role.
def instance1():
    global log_file
    log_file = open(LOG_PATH_INSTANCE1, "w")
    write_log("start")
    ble.active(1)
    ble.irq(irq)
    multitest.next()
    try:
        for repeat in range(2):
            # Connect to peripheral and then disconnect.
            print("gap_connect")
            ble.gap_connect(BDADDR[0], BDADDR[1], 5000)
            conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

            # Discover services.
            print("gattc_discover_services")
            ble.gattc_discover_services(conn_handle)
            start_handle, end_handle = wait_for_event(_IRQ_GATTC_SERVICE_RESULT, TIMEOUT_MS)
            wait_for_event(_IRQ_GATTC_SERVICE_DONE, TIMEOUT_MS)

            # Discover characteristics.
            print("gattc_discover_characteristics")
            ble.gattc_discover_characteristics(conn_handle, start_handle, end_handle)
            value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
            wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

            for op in range(4):
                print("gattc_read")
                ble.gattc_read(conn_handle, value_handle)
                wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)
                wait_for_event(_IRQ_GATTC_READ_DONE, TIMEOUT_MS)
                print("gattc_write")
                ble.gattc_write(conn_handle, value_handle, "{}".format(op), 1)
                wait_for_event(_IRQ_GATTC_WRITE_DONE, TIMEOUT_MS)

            # Disconnect.
            print("gap_disconnect:", ble.gap_disconnect(conn_handle))
            wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, 2 * TIMEOUT_MS)
    finally:
        ble.active(0)
        log_file.close()
        os.unlink(LOG_PATH_INSTANCE1)


ble = bluetooth.BLE()
