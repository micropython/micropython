# Test BLE GAP connect/disconnect with pairing and bonding, authenticating with a passkey, read
# an encrypted characteristic, then reconnect and read again to check if the bonding was persisted

from micropython import const
import time, machine, bluetooth

if not hasattr(bluetooth.BLE, "gap_pair"):
    print("SKIP")
    raise SystemExit

TIMEOUT_MS = 6000

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_READ_REQUEST = const(4)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_ENCRYPTION_UPDATE = const(28)
_IRQ_GET_SECRET = const(29)
_IRQ_SET_SECRET = const(30)
_IRQ_PASSKEY_ACTION = const(31)

_FLAG_READ = const(0x0002)
_FLAG_READ_ENCRYPTED = const(0x0200)

_IO_CAPABILITY_DISPLAY_ONLY = const(0)
_IO_CAPABILITY_KEYBOARD_ONLY = const(2)

_PASSKEY_ACTION_INPUT = const(2)
_PASSKEY_ACTION_DISPLAY = const(3)

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")
CHAR = (CHAR_UUID, _FLAG_READ | _FLAG_READ_ENCRYPTED)
SERVICE = (SERVICE_UUID, (CHAR,))
PASSKEY = 123456

waiting_events = {}
secrets = {}


def irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
    elif event == _IRQ_GATTS_READ_REQUEST:
        print("_IRQ_GATTS_READ_REQUEST")
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
    elif event == _IRQ_GATTC_READ_RESULT:
        print("_IRQ_GATTC_READ_RESULT", bytes(data[-1]))
    elif event == _IRQ_ENCRYPTION_UPDATE:
        print("_IRQ_ENCRYPTION_UPDATE", data[1], data[2], data[3])
    elif event == _IRQ_GET_SECRET:
        type_secrets = secrets.get(data[0], {})
        if data[2] is not None:
            return type_secrets.get(bytes(data[2]))
        elif data[1] < len(type_secrets):
            return type_secrets[list(type_secrets.keys())[data[1]]]
        return None
    elif event == _IRQ_SET_SECRET:
        if data[0] not in secrets:
            secrets[data[0]] = {}
        secrets[data[0]][bytes(data[1])] = bytes(data[2])
        return True
    elif event == _IRQ_PASSKEY_ACTION:
        print("_IRQ_PASSKEY_ACTION", data[1], data[2])
        waiting_events[event] = data[0]

    if event not in waiting_events:
        waiting_events[event] = None


def wait_for_event(event, timeout_ms):
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        if event in waiting_events:
            return waiting_events.pop(event)
        machine.idle()
    msg = "Timeout waiting for {}".format(event)
    print(msg)
    raise ValueError(msg)


# Acting in peripheral role.
def instance0():
    ble.config(io=_IO_CAPABILITY_DISPLAY_ONLY)
    ble.active(1)
    multitest.globals(BDADDR=ble.config("mac"))
    ((char_handle,),) = ble.gatts_register_services((SERVICE,))
    print("gatts_write")
    ble.gatts_write(char_handle, "encrypted-one")
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect.
        wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # Wait for passkey event and provide it.
        conn_handle = wait_for_event(_IRQ_PASSKEY_ACTION, TIMEOUT_MS)
        print("gap_passkey")
        ble.gap_passkey(conn_handle, _PASSKEY_ACTION_DISPLAY, PASSKEY)

        # Wait for pairing event.
        wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)

        # Wait for GATTS read request.
        wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)

        # Wait for central to disconnect.
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)

        # Update the characteristic value.
        print("gatts_write")
        ble.gatts_write(char_handle, "encrypted-two")

        # Start advertising again so the central can reconnect.
        print("gap_advertise")
        ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")

        # Wait for central to connect again.
        wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # Wait for pairing event.
        wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)

        # Wait for GATTS read request.
        wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)

        # Wait for central to disconnect.
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    ble.config(io=_IO_CAPABILITY_KEYBOARD_ONLY)
    ble.active(1)
    multitest.next()
    try:
        # Connect to peripheral.
        print("gap_connect")
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        # Discover characteristics (before pairing, doesn't need to be encrypted).
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Pair with the peripheral.
        print("gap_pair")
        ble.gap_pair(conn_handle)

        # Wait for the passkey event.
        wait_for_event(_IRQ_PASSKEY_ACTION, TIMEOUT_MS)

        # Provide the passkey.
        print("gap_passkey")
        ble.gap_passkey(conn_handle, _PASSKEY_ACTION_INPUT, PASSKEY)

        # Wait for the pairing event.
        wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)

        # Read the peripheral's characteristic, should be "encrypted-one".
        print("gattc_read")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Disconnect from the peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)

        # Restart Bluetooth stack to flush cached secrets and force to to read them
        print("active(0)")
        ble.active(0)
        print("active(1)")
        ble.active(1)
        time.sleep_ms(5000)

        # Connect to peripheral again.
        print("gap_connect")
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        # Discover characteristics again.
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Pair with the peripheral again, reusing the bonding.
        print("gap_pair")
        ble.gap_pair(conn_handle)
        wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)

        # Read the peripheral's characteristic again, should be "encrypted-two".
        print("gattc_read")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Disconnect from the peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.config(mitm=True, le_secure=True, bond=True)
ble.irq(irq)
