# Test BLE GAP unpair functionality
# Tests the new gap_unpair method added to MicroPython
# gap_unpair expects a key from _IRQ_GET_SECRET/_IRQ_SET_SECRET events

from micropython import const
import time, machine, bluetooth

if not hasattr(bluetooth.BLE, "gap_unpair"):
    print("SKIP")
    raise SystemExit

TIMEOUT_MS = 4000

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

_FLAG_READ = const(0x0002)
_FLAG_READ_ENCRYPTED = const(0x0200)

SERVICE_UUID = bluetooth.UUID("A5A5A5A5-FFFF-9999-1111-5A5A5A5A5A5A")
CHAR_UUID = bluetooth.UUID("00000000-1111-2222-3333-444444444444")
CHAR = (CHAR_UUID, _FLAG_READ | _FLAG_READ_ENCRYPTED)
SERVICE = (SERVICE_UUID, (CHAR,))

waiting_events = {}
bond_keys = []  # Store bond keys for unpair testing


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
        print("_IRQ_GET_SECRET", "key:", data[1])
        # data[1] is index (integer), not usable for gap_unpair
    elif event == _IRQ_SET_SECRET:
        print("_IRQ_SET_SECRET", "key:", data[1])
        # data[1] contains the ble_addr_t key (7 bytes) - convert to bytes immediately!
        # Memoryview is only valid during the IRQ callback
        bond_keys.append(bytes(data[1]))

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
    ((char_handle,),) = ble.gatts_register_services((SERVICE,))
    ble.gatts_write(char_handle, "encrypted")
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to connect.
        wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # Wait for pairing event.
        wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)

        # Wait for GATTS read request.
        wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)

        multitest.next()

        # Wait for central to disconnect after initial pairing.
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)

        # Test gap_unpair functionality
        print("gap_unpair_test")
        print("bond_keys_captured:", len(bond_keys))

        # Test gap_unpair with captured bond keys
        # Make a copy because gap_unpair triggers new _IRQ_SET_SECRET events
        keys_to_test = list(bond_keys)
        if keys_to_test:
            print("attempting_gap_unpair")
            for i, key in enumerate(keys_to_test):
                print("unpair_iteration:", i)
                try:
                    print("calling_gap_unpair_with_key_type:", type(key).__name__)
                    result = ble.gap_unpair(key)
                    print("gap_unpair_key_", i, "_result:", result)
                except Exception as e:
                    print("gap_unpair_key_", i, "_error:", type(e).__name__, str(e))
        else:
            print("gap_unpair_no_keys_captured")

        print("gap_unpair_test_complete")

        # TODO: Add fake key and wrong key format tests
        # These currently cause device crashes and need investigation

    finally:
        ble.active(0)


# Acting in central role.
def instance1():
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

        # Wait for the pairing event.
        wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)

        # Read the peripheral's characteristic, should be encrypted.
        print("gattc_read")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        multitest.next()

        # Disconnect from the peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)

        # Test gap_unpair on central side too
        print("central_gap_unpair_test")
        print("central_bond_keys_captured:", len(bond_keys))

        # Test gap_unpair with captured bond keys on central side
        # Make a copy because gap_unpair triggers new _IRQ_SET_SECRET events
        keys_to_test = list(bond_keys)
        if keys_to_test:
            print("central_attempting_gap_unpair")
            for i, key in enumerate(keys_to_test):
                print("central_unpair_iteration:", i)
                try:
                    print("central_calling_gap_unpair_with_key_type:", type(key).__name__)
                    result = ble.gap_unpair(key)
                    print("central_gap_unpair_key_", i, "_result:", result)
                except Exception as e:
                    print("central_gap_unpair_key_", i, "_error:", type(e).__name__, str(e))
        else:
            print("central_gap_unpair_no_keys")

        print("central_gap_unpair_test_complete")

    finally:
        ble.active(0)


ble = bluetooth.BLE()
ble.config(mitm=True, le_secure=True, bond=True)  # Enable bonding for unpair test
ble.active(1)
ble.irq(irq)
