# Test BLE GAP pairing/bonding with bond persistence across multiple BLE restart cycles
# This test verifies that bond data is properly stored and reloaded across ble.active(0)/ble.active(1) cycles

from micropython import const
import time, machine, bluetooth

if not hasattr(bluetooth.BLE, "gap_pair"):
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
        waiting_events[event] = data  # Store full data for bonding info
    elif event == _IRQ_GET_SECRET:
        if data[-1] is None:
            return None
        key = bytes(data[-1])
        result = secrets.get(key, None)
        print("_IRQ_GET_SECRET key={} found={}".format(len(key), result is not None))
        return result
    elif event == _IRQ_SET_SECRET:
        key = bytes(data[-2])
        value = bytes(data[-1]) if data[-1] else None
        if value is None:
            print("_IRQ_SET_SECRET delete key={}".format(len(key)))
            secrets.pop(key, None)
        else:
            print("_IRQ_SET_SECRET store key={} value={}".format(len(key), len(value)))
            secrets[key] = value
        return True

    if event not in waiting_events:
        waiting_events[event] = None


def wait_for_event(event, timeout_ms):
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < timeout_ms:
        if event in waiting_events:
            return waiting_events.pop(event)
        machine.idle()
    raise ValueError("Timeout waiting for {}".format(event))


def ble_restart(ble):
    """Simulate BLE restart by deactivating and reactivating"""
    print("ble_restart")
    ble.active(0)
    time.sleep_ms(100)  # Allow cleanup
    # CRITICAL: Set IRQ handler BEFORE activating BLE so BTstack can load bonds during init
    ble.irq(irq)
    ble.active(1)


# Acting in peripheral role.
def instance0():
    ble = bluetooth.BLE()
    ble.config(mitm=True, le_secure=True, bond=True)
    # CRITICAL: Set IRQ handler BEFORE activating BLE so BTstack can load bonds during init
    ble.irq(irq)
    ble.active(1)

    multitest.globals(BDADDR=ble.config("mac"))

    # Initial pairing phase
    print("=== INITIAL PAIRING ===")
    ((char_handle,),) = ble.gatts_register_services((SERVICE,))
    ble.gatts_write(char_handle, "encrypted_initial")
    print("gap_advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()

    try:
        # Wait for central to connect and pair
        wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
        encryption_data = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
        print("initial_bonded", encryption_data[3])  # bonded flag
        wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)

        # Check that secrets were stored
        print("secrets_stored", len(secrets))

        # First restart cycle
        print("=== RESTART 1 ===")
        ble_restart(ble)
        ((char_handle,),) = ble.gatts_register_services((SERVICE,))
        ble.gatts_write(char_handle, "encrypted_restart1")
        ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
        multitest.next()

        # Should reconnect with stored bond
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # Check if automatically encrypted
        encryption_data = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
        print("restart1_auto_encrypted", encryption_data[1])  # encrypted flag
        print("restart1_auto_bonded", encryption_data[3])  # bonded flag

        wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)

        # Second restart cycle
        print("=== RESTART 2 ===")
        ble_restart(ble)
        ((char_handle,),) = ble.gatts_register_services((SERVICE,))
        ble.gatts_write(char_handle, "encrypted_restart2")
        ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
        multitest.next()

        # Should still reconnect with stored bond
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # Check if automatically encrypted
        encryption_data = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
        print("restart2_auto_encrypted", encryption_data[1])  # encrypted flag
        print("restart2_auto_bonded", encryption_data[3])  # bonded flag

        wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)

        # Third restart cycle
        print("=== RESTART 3 ===")
        ble_restart(ble)
        ((char_handle,),) = ble.gatts_register_services((SERVICE,))
        ble.gatts_write(char_handle, "encrypted_restart3")
        ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
        multitest.next()

        # Should still reconnect with stored bond
        conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # Check if automatically encrypted
        encryption_data = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
        print("restart3_auto_encrypted", encryption_data[1])  # encrypted flag
        print("restart3_auto_bonded", encryption_data[3])  # bonded flag

        wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)

        print("=== FINAL STATE ===")
        print("final_secrets_count", len(secrets))

    finally:
        ble.active(0)


# Acting in central role.
def instance1():
    ble = bluetooth.BLE()
    ble.config(mitm=True, le_secure=True, bond=True)
    # CRITICAL: Set IRQ handler BEFORE activating BLE so BTstack can load bonds during init
    ble.irq(irq)
    ble.active(1)

    multitest.next()

    try:
        # Initial pairing phase
        print("=== CENTRAL INITIAL PAIRING ===")
        print("gap_connect")
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        # Discover characteristics before pairing
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Pair with the peripheral
        print("gap_pair")
        ble.gap_pair(conn_handle)
        encryption_data = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
        print("central_initial_bonded", encryption_data[3])  # bonded flag

        # Read encrypted characteristic
        print("gattc_read")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Disconnect
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)

        # Check that secrets were stored
        print("central_secrets_stored", len(secrets))

        # First restart cycle
        print("=== CENTRAL RESTART 1 ===")
        ble_restart(ble)
        multitest.next()

        # Reconnect - should use stored bond
        print("gap_connect_restart1")
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        # Should be automatically encrypted
        encryption_data = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
        print("central_restart1_auto_encrypted", encryption_data[1])
        print("central_restart1_auto_bonded", encryption_data[3])

        # Re-discover and read
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)

        # Second restart cycle
        print("=== CENTRAL RESTART 2 ===")
        ble_restart(ble)
        multitest.next()

        # Reconnect - should still use stored bond
        print("gap_connect_restart2")
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        # Should be automatically encrypted
        encryption_data = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
        print("central_restart2_auto_encrypted", encryption_data[1])
        print("central_restart2_auto_bonded", encryption_data[3])

        # Re-discover and read
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)

        # Third restart cycle
        print("=== CENTRAL RESTART 3 ===")
        ble_restart(ble)
        multitest.next()

        # Reconnect - should still use stored bond
        print("gap_connect_restart3")
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        # Should be automatically encrypted
        encryption_data = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
        print("central_restart3_auto_encrypted", encryption_data[1])
        print("central_restart3_auto_bonded", encryption_data[3])

        # Re-discover and read
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)

        print("=== CENTRAL FINAL STATE ===")
        print("central_final_secrets_count", len(secrets))

    finally:
        ble.active(0)
