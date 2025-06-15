# Test BLE GAP connect/disconnect with pairing and bonding, then reconnect
# to verify that bond persists (simulates reboot by recreating BLE instance)

from micropython import const
import time, machine, bluetooth
import json, binascii, os

if not hasattr(bluetooth.BLE, "gap_pair"):
    print("SKIP")
    raise SystemExit

TIMEOUT_MS = 4000
SECRETS_FILE = "test_bonds.json"

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


def load_secrets():
    """Load bond secrets from file"""
    global secrets
    secrets = {}
    try:
        with open(SECRETS_FILE, "r") as f:
            entries = json.load(f)
            for sec_type, key, value in entries:
                secrets[sec_type, binascii.a2b_base64(key)] = binascii.a2b_base64(value)
        print("secrets_loaded", len(secrets))
    except:
        print("secrets_file_not_found")


def save_secrets():
    """Save bond secrets to file"""
    try:
        with open(SECRETS_FILE, "w") as f:
            json_secrets = [
                (
                    sec_type,
                    binascii.b2a_base64(key).decode().strip(),
                    binascii.b2a_base64(value).decode().strip(),
                )
                for (sec_type, key), value in secrets.items()
            ]
            json.dump(json_secrets, f)
        print("secrets_saved", len(secrets))
    except Exception as e:
        print("secrets_save_failed", str(e))


def cleanup_secrets_file():
    """Clean up the secrets file"""
    try:
        os.remove(SECRETS_FILE)
        print("secrets_file_deleted")
    except:
        pass


def irq(event, data):
    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
        save_secrets()  # Save secrets when central disconnects
    elif event == _IRQ_GATTS_READ_REQUEST:
        print("_IRQ_GATTS_READ_REQUEST")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
        save_secrets()  # Save secrets when peripheral disconnects
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
        if data[-1] is None:
            return None
        key = (data[0], bytes(data[-1]))
        return secrets.get(key, None)
    elif event == _IRQ_SET_SECRET:
        key = (data[0], bytes(data[-2]))
        value = bytes(data[-1]) if data[-1] else None
        if value is None:
            secrets.pop(key, None)
        else:
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


# Acting in peripheral role.
def instance0():
    # Clean up any existing secrets file from previous tests
    cleanup_secrets_file()
    load_secrets()  # Load secrets (will be empty initially)

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

        # Wait for central to disconnect.
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)

    # Simulate reboot by recreating BLE instance (tests bond persistence in file storage)
    print("simulate_reboot")
    ble.active(0)
    time.sleep_ms(100)  # Allow cleanup
    load_secrets()  # Reload secrets from file
    # CRITICAL: Set IRQ handler BEFORE activating BLE so BTstack can load bonds during init
    ble.irq(irq)
    ble.active(1)

    # Re-register services after "reboot"
    ((char_handle,),) = ble.gatts_register_services((SERVICE,))
    ble.gatts_write(char_handle, "encrypted_after_reboot")
    print("gap_advertise_after_reboot")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")
    multitest.next()
    try:
        # Wait for central to reconnect using stored bond.
        wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)

        # Should automatically be encrypted due to stored bond.
        wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)

        # Wait for GATTS read request.
        wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)

        # Wait for central to disconnect.
        wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)
        cleanup_secrets_file()  # Clean up test file


# Acting in central role.
def instance1():
    # Clean up any existing secrets file from previous tests
    cleanup_secrets_file()
    load_secrets()  # Load secrets (will be empty initially)

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

        # Disconnect from the peripheral.
        print("gap_disconnect:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)

    # Recreate BLE instance to simulate reboot (tests bond persistence)
    ble.active(0)
    time.sleep_ms(100)  # Allow cleanup
    load_secrets()  # Reload secrets from file
    # CRITICAL: Set IRQ handler BEFORE activating BLE so BTstack can load bonds during init
    ble.irq(irq)
    ble.active(1)

    multitest.next()
    try:
        # Reconnect to peripheral after simulated reboot
        print("gap_connect_after_reboot")
        ble.gap_connect(*BDADDR)
        conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)

        # Should automatically be encrypted due to stored bond (no re-pairing needed).
        wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)

        # Discover characteristics again (connection state is lost on reboot).
        ble.gattc_discover_characteristics(conn_handle, 1, 65535)
        value_handle = wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
        wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

        # Read the peripheral's characteristic, should be encrypted without re-pairing.
        print("gattc_read_after_reboot")
        ble.gattc_read(conn_handle, value_handle)
        wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)

        # Disconnect from the peripheral.
        print("gap_disconnect_final:", ble.gap_disconnect(conn_handle))
        wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    finally:
        ble.active(0)
        cleanup_secrets_file()  # Clean up test file


ble = bluetooth.BLE()
ble.config(mitm=True, le_secure=True, bond=True)
# CRITICAL: Set IRQ handler BEFORE activating BLE so BTstack can load bonds during init
ble.irq(irq)
ble.active(1)
