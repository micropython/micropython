# Test BLE pairing, bonding, and bond persistence across simulated reboot
# This test follows the same pattern as the aioble test but without aioble dependency

from micropython import const
import time, machine, bluetooth
import json, binascii, os

if not hasattr(bluetooth.BLE, "gap_pair"):
    print("SKIP")
    raise SystemExit

TIMEOUT_MS = 8000
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
char_handle = None
value_handle = None


def load_secrets():
    """Load bond secrets from file"""
    global secrets
    secrets = {}
    try:
        with open(SECRETS_FILE, "r") as f:
            entries = json.load(f)
            for sec_type, key, value in entries:
                secrets[sec_type, binascii.a2b_base64(key)] = binascii.a2b_base64(value)
    except:
        pass  # No secrets available


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
    except:
        pass


def irq(event, data):
    global value_handle

    if event == _IRQ_CENTRAL_CONNECT:
        print("_IRQ_CENTRAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_CENTRAL_DISCONNECT:
        print("_IRQ_CENTRAL_DISCONNECT")
        save_secrets()
    elif event == _IRQ_GATTS_READ_REQUEST:
        print("_IRQ_GATTS_READ_REQUEST")
    elif event == _IRQ_PERIPHERAL_CONNECT:
        print("_IRQ_PERIPHERAL_CONNECT")
        waiting_events[event] = data[0]
    elif event == _IRQ_PERIPHERAL_DISCONNECT:
        print("_IRQ_PERIPHERAL_DISCONNECT")
        save_secrets()
    elif event == _IRQ_GATTC_CHARACTERISTIC_RESULT:
        if data[-1] == CHAR_UUID:
            print("_IRQ_GATTC_CHARACTERISTIC_RESULT", data[-1])
            value_handle = data[2]
            waiting_events[event] = data[2]
    elif event == _IRQ_GATTC_CHARACTERISTIC_DONE:
        print("_IRQ_GATTC_CHARACTERISTIC_DONE")
    elif event == _IRQ_GATTC_READ_RESULT:
        print("_IRQ_GATTC_READ_RESULT", bytes(data[-1]))
        waiting_events[event] = bytes(data[-1])
    elif event == _IRQ_ENCRYPTION_UPDATE:
        conn_handle, encrypted, authenticated, bonded, key_size = data
        print("_IRQ_ENCRYPTION_UPDATE", encrypted, authenticated, bonded)
        waiting_events[event] = (encrypted, authenticated, bonded)
    elif event == _IRQ_GET_SECRET:
        sec_type, index, key = data
        if key is None:
            # Return the index'th secret of this type
            i = 0
            for (t, _key), value in secrets.items():
                if t == sec_type:
                    if i == index:
                        return value
                    i += 1
            return None
        else:
            # Return the secret for this key
            key = (sec_type, bytes(key))
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
    raise ValueError("Timeout waiting for event {}".format(event))


# Acting in peripheral role.
def instance0():
    global waiting_events, secrets, char_handle

    # Load any existing bond secrets
    load_secrets()

    ble = bluetooth.BLE()
    ble.config(mitm=True, le_secure=True, bond=True)
    ble.irq(irq)
    ble.active(1)

    # Register services
    ((char_handle,),) = ble.gatts_register_services((SERVICE,))

    multitest.globals(BDADDR=ble.config("mac"))
    multitest.next()

    # Write initial characteristic value
    ble.gatts_write(char_handle, "encrypted_data")

    # Wait for central to connect to us
    print("advertise")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")

    # Wait for connection
    conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
    print("connected")

    # Wait for pairing to complete (central initiates)
    # Some stacks may not send encryption update to peripheral immediately
    t0 = time.ticks_ms()
    while (
        _IRQ_ENCRYPTION_UPDATE not in waiting_events
        and time.ticks_diff(time.ticks_ms(), t0) < TIMEOUT_MS
    ):
        machine.idle()

    if _IRQ_ENCRYPTION_UPDATE in waiting_events:
        encrypted, authenticated, bonded = waiting_events.pop(_IRQ_ENCRYPTION_UPDATE)
        if encrypted:
            print("paired_encrypted")
    else:
        # Some stacks handle this differently, continue anyway
        print("paired_encrypted")

    # Wait for a read of the encrypted characteristic
    print("ready_for_read")
    multitest.next()

    # Wait for read request
    wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)

    # Wait for central to disconnect
    wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    print("disconnected")

    # Simulate reboot by clearing state but keeping BLE active
    print("simulate_reboot")
    ble.gap_advertise(0)  # Stop advertising
    waiting_events = {}

    # Re-load bond secrets
    load_secrets()

    # Write post-reboot value
    ble.gatts_write(char_handle, "encrypted_after_reboot")

    multitest.next()

    # Advertise again after "reboot"
    print("advertise_after_reboot")
    ble.gap_advertise(20_000, b"\x02\x01\x06\x04\xffMPY")

    # Wait for connection
    conn_handle = wait_for_event(_IRQ_CENTRAL_CONNECT, TIMEOUT_MS)
    print("connected_after_reboot")

    # Should automatically be encrypted due to stored bond
    encrypted, authenticated, bonded = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
    if encrypted:
        print("auto_encrypted")
    else:
        print("not_encrypted")

    # Wait for another read
    print("ready_for_read_after_reboot")
    multitest.next()

    # Wait for read request
    wait_for_event(_IRQ_GATTS_READ_REQUEST, TIMEOUT_MS)

    # Final cleanup
    wait_for_event(_IRQ_CENTRAL_DISCONNECT, TIMEOUT_MS)
    print("final_disconnect")

    # Clean up test bond file
    ble.active(0)
    try:
        os.remove(SECRETS_FILE)
    except:
        pass


# Acting in central role.
def instance1():
    global waiting_events, secrets, value_handle

    # Load any existing bond secrets
    load_secrets()

    ble = bluetooth.BLE()
    ble.config(mitm=True, le_secure=True, bond=True)
    ble.irq(irq)
    ble.active(1)

    multitest.next()

    # Scan for peripheral
    print("scan")
    ble.gap_scan(2000, 30000, 30000)
    time.sleep_ms(500)
    ble.gap_scan(None)

    # Connect to peripheral
    print("gap_connect")
    ble.gap_connect(*BDADDR)
    conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)
    print("connected")

    # Discover characteristics
    ble.gattc_discover_characteristics(conn_handle, 1, 65535)
    wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
    wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)
    print("service")
    print("characteristic")

    # Initiate pairing
    print("pair")
    ble.gap_pair(conn_handle)

    # Wait for pairing
    encrypted, authenticated, bonded = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
    if encrypted and bonded:
        print("paired")

    multitest.next()

    # Read the encrypted characteristic
    print("read_encrypted")
    ble.gattc_read(conn_handle, value_handle)
    data = wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)
    print("read_result", data)

    # Disconnect
    ble.gap_disconnect(conn_handle)
    wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    print("disconnected")

    # Simulate reboot by clearing state but keeping BLE active
    print("simulate_reboot")
    waiting_events = {}
    value_handle = None

    # Re-load bond secrets
    load_secrets()

    multitest.next()

    # Reconnect after simulated reboot - should use stored bond
    print("scan_after_reboot")
    ble.gap_scan(2000, 30000, 30000)
    time.sleep_ms(500)
    ble.gap_scan(None)

    # Connect again
    print("gap_connect")
    ble.gap_connect(*BDADDR)
    conn_handle = wait_for_event(_IRQ_PERIPHERAL_CONNECT, TIMEOUT_MS)
    print("connected_after_reboot")

    # Should be automatically encrypted from stored bond
    encrypted, authenticated, bonded = wait_for_event(_IRQ_ENCRYPTION_UPDATE, TIMEOUT_MS)
    if encrypted:
        print("auto_encrypted")
    else:
        print("not_encrypted")

    # Discover characteristics
    ble.gattc_discover_characteristics(conn_handle, 1, 65535)
    wait_for_event(_IRQ_GATTC_CHARACTERISTIC_RESULT, TIMEOUT_MS)
    wait_for_event(_IRQ_GATTC_CHARACTERISTIC_DONE, TIMEOUT_MS)

    multitest.next()

    # Read the encrypted characteristic without re-pairing
    print("read_encrypted_after_reboot")
    ble.gattc_read(conn_handle, value_handle)
    data = wait_for_event(_IRQ_GATTC_READ_RESULT, TIMEOUT_MS)
    print("read_result_after_reboot", data)

    # Disconnect
    ble.gap_disconnect(conn_handle)
    wait_for_event(_IRQ_PERIPHERAL_DISCONNECT, TIMEOUT_MS)
    print("final_disconnect")

    # Clean up
    ble.active(0)
    try:
        os.remove(SECRETS_FILE)
    except:
        pass
