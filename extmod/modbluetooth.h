/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
 * Copyright (c) 2019-2020 Jim Mussared
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_EXTMOD_MODBLUETOOTH_H
#define MICROPY_INCLUDED_EXTMOD_MODBLUETOOTH_H

#include <stdbool.h>

#include "py/obj.h"
#include "py/objlist.h"
#include "py/ringbuf.h"

// Port specific configuration.
#ifndef MICROPY_PY_BLUETOOTH_RINGBUF_SIZE
#define MICROPY_PY_BLUETOOTH_RINGBUF_SIZE (128)
#endif

#ifndef MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
#define MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE (0)
#endif

#ifndef MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT
// Enable the client by default if we're enabling central mode. It's possible
// to enable client without central though.
#define MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT (MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE)
#endif

#ifndef MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS
// This can be enabled if the BLE stack runs entirely in scheduler context
// and therefore is able to call directly into the VM to run Python callbacks.
#define MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS (0)
#endif

// A port can optionally enable support for L2CAP "Connection Oriented Channels".
#ifndef MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS
#define MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS (0)
#endif

// A port can optionally enable support for pairing and bonding.
// Requires MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS.
#ifndef MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
#define MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING (0)
#endif

// Optionally enable support for the `hci_cmd` function allowing
// Python to directly low-level HCI commands.
#ifndef MICROPY_PY_BLUETOOTH_ENABLE_HCI_CMD
#define MICROPY_PY_BLUETOOTH_ENABLE_HCI_CMD (0)
#endif

// This is used to protect the ringbuffer.
// A port may no-op this if MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS is enabled.
#ifndef MICROPY_PY_BLUETOOTH_ENTER
#define MICROPY_PY_BLUETOOTH_ENTER mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
#define MICROPY_PY_BLUETOOTH_EXIT MICROPY_END_ATOMIC_SECTION(atomic_state);
#endif

// Common constants.
#ifndef MP_BLUETOOTH_DEFAULT_ATTR_LEN
#define MP_BLUETOOTH_DEFAULT_ATTR_LEN (20)
#endif

#define MP_BLUETOOTH_CCCB_LEN (2)

// Advertisement packet lengths
#define MP_BLUETOOTH_GAP_ADV_MAX_LEN (32)

// Basic characteristic/descriptor flags.
// These match the spec values for these flags so can be passed directly to the stack.
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_BROADCAST                  (0x0001)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_READ                       (0x0002)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE_NO_RESPONSE          (0x0004)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE                      (0x0008)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_NOTIFY                     (0x0010)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_INDICATE                   (0x0020)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_AUTHENTICATED_SIGNED_WRITE (0x0040)

// TODO: NimBLE and BlueKitchen disagree on this one.
// #define MP_BLUETOOTH_CHARACTERISTIC_FLAG_RELIABLE_WRITE             (0x0080)

// Extended flags for security and privacy.
// These match NimBLE but might require mapping in the bindings for other stacks.
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_AUX_WRITE                  (0x0100)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_READ_ENCRYPTED             (0x0200)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_READ_AUTHENTICATED         (0x0400)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_READ_AUTHORIZED            (0x0800)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE_ENCRYPTED            (0x1000)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE_AUTHENTICATED        (0x2000)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE_AUTHORIZED           (0x4000)

// Return values from _IRQ_GATTS_READ_REQUEST.
#define MP_BLUETOOTH_GATTS_NO_ERROR                           (0x00)
#define MP_BLUETOOTH_GATTS_ERROR_READ_NOT_PERMITTED           (0x02)
#define MP_BLUETOOTH_GATTS_ERROR_WRITE_NOT_PERMITTED          (0x03)
#define MP_BLUETOOTH_GATTS_ERROR_INSUFFICIENT_AUTHENTICATION  (0x05)
#define MP_BLUETOOTH_GATTS_ERROR_INSUFFICIENT_AUTHORIZATION   (0x08)
#define MP_BLUETOOTH_GATTS_ERROR_INSUFFICIENT_ENCRYPTION      (0x0f)

// For mp_bluetooth_gattc_write, the mode parameter
#define MP_BLUETOOTH_WRITE_MODE_NO_RESPONSE     (0)
#define MP_BLUETOOTH_WRITE_MODE_WITH_RESPONSE   (1)

// Type value also doubles as length.
#define MP_BLUETOOTH_UUID_TYPE_16  (2)
#define MP_BLUETOOTH_UUID_TYPE_32  (4)
#define MP_BLUETOOTH_UUID_TYPE_128 (16)

// Event codes for the IRQ handler.
#define MP_BLUETOOTH_IRQ_CENTRAL_CONNECT                (1)
#define MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT             (2)
#define MP_BLUETOOTH_IRQ_GATTS_WRITE                    (3)
#define MP_BLUETOOTH_IRQ_GATTS_READ_REQUEST             (4)
#define MP_BLUETOOTH_IRQ_SCAN_RESULT                    (5)
#define MP_BLUETOOTH_IRQ_SCAN_DONE                      (6)
#define MP_BLUETOOTH_IRQ_PERIPHERAL_CONNECT             (7)
#define MP_BLUETOOTH_IRQ_PERIPHERAL_DISCONNECT          (8)
#define MP_BLUETOOTH_IRQ_GATTC_SERVICE_RESULT           (9)
#define MP_BLUETOOTH_IRQ_GATTC_SERVICE_DONE             (10)
#define MP_BLUETOOTH_IRQ_GATTC_CHARACTERISTIC_RESULT    (11)
#define MP_BLUETOOTH_IRQ_GATTC_CHARACTERISTIC_DONE      (12)
#define MP_BLUETOOTH_IRQ_GATTC_DESCRIPTOR_RESULT        (13)
#define MP_BLUETOOTH_IRQ_GATTC_DESCRIPTOR_DONE          (14)
#define MP_BLUETOOTH_IRQ_GATTC_READ_RESULT              (15)
#define MP_BLUETOOTH_IRQ_GATTC_READ_DONE                (16)
#define MP_BLUETOOTH_IRQ_GATTC_WRITE_DONE               (17)
#define MP_BLUETOOTH_IRQ_GATTC_NOTIFY                   (18)
#define MP_BLUETOOTH_IRQ_GATTC_INDICATE                 (19)
#define MP_BLUETOOTH_IRQ_GATTS_INDICATE_DONE            (20)
#define MP_BLUETOOTH_IRQ_MTU_EXCHANGED                  (21)
#define MP_BLUETOOTH_IRQ_L2CAP_ACCEPT                   (22)
#define MP_BLUETOOTH_IRQ_L2CAP_CONNECT                  (23)
#define MP_BLUETOOTH_IRQ_L2CAP_DISCONNECT               (24)
#define MP_BLUETOOTH_IRQ_L2CAP_RECV                     (25)
#define MP_BLUETOOTH_IRQ_L2CAP_SEND_READY               (26)
#define MP_BLUETOOTH_IRQ_CONNECTION_UPDATE              (27)
#define MP_BLUETOOTH_IRQ_ENCRYPTION_UPDATE              (28)
#define MP_BLUETOOTH_IRQ_GET_SECRET                     (29)
#define MP_BLUETOOTH_IRQ_SET_SECRET                     (30)
#define MP_BLUETOOTH_IRQ_PASSKEY_ACTION                 (31)

#define MP_BLUETOOTH_ADDRESS_MODE_PUBLIC (0)
#define MP_BLUETOOTH_ADDRESS_MODE_RANDOM (1)
#define MP_BLUETOOTH_ADDRESS_MODE_RPA (2)
#define MP_BLUETOOTH_ADDRESS_MODE_NRPA (3)

// These match the spec values, can be used directly by the stack.
#define MP_BLUETOOTH_IO_CAPABILITY_DISPLAY_ONLY        (0)
#define MP_BLUETOOTH_IO_CAPABILITY_DISPLAY_YESNO       (1)
#define MP_BLUETOOTH_IO_CAPABILITY_KEYBOARD_ONLY       (2)
#define MP_BLUETOOTH_IO_CAPABILITY_NO_INPUT_OUTPUT     (3)
#define MP_BLUETOOTH_IO_CAPABILITY_KEYBOARD_DISPLAY    (4)

// These match NimBLE BLE_SM_IOACT_.
#define MP_BLUETOOTH_PASSKEY_ACTION_NONE                (0)
#define MP_BLUETOOTH_PASSKEY_ACTION_INPUT               (2)
#define MP_BLUETOOTH_PASSKEY_ACTION_DISPLAY             (3)
#define MP_BLUETOOTH_PASSKEY_ACTION_NUMERIC_COMPARISON  (4)

// These match NimBLE BLE_SM_IOACT_.
#define MP_BLUETOOTH_PASSKEY_ACTION_NONE                (0)
#define MP_BLUETOOTH_PASSKEY_ACTION_INPUT               (2)
#define MP_BLUETOOTH_PASSKEY_ACTION_DISPLAY             (3)
#define MP_BLUETOOTH_PASSKEY_ACTION_NUMERIC_COMPARISON  (4)

/*
These aren't included in the module for space reasons, but can be used
in your Python code if necessary.

from micropython import const
_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)
_IRQ_GATTS_READ_REQUEST = const(4)
_IRQ_SCAN_RESULT = const(5)
_IRQ_SCAN_DONE = const(6)
_IRQ_PERIPHERAL_CONNECT = const(7)
_IRQ_PERIPHERAL_DISCONNECT = const(8)
_IRQ_GATTC_SERVICE_RESULT = const(9)
_IRQ_GATTC_SERVICE_DONE = const(10)
_IRQ_GATTC_CHARACTERISTIC_RESULT = const(11)
_IRQ_GATTC_CHARACTERISTIC_DONE = const(12)
_IRQ_GATTC_DESCRIPTOR_RESULT = const(13)
_IRQ_GATTC_DESCRIPTOR_DONE = const(14)
_IRQ_GATTC_READ_RESULT = const(15)
_IRQ_GATTC_READ_DONE = const(16)
_IRQ_GATTC_WRITE_DONE = const(17)
_IRQ_GATTC_NOTIFY = const(18)
_IRQ_GATTC_INDICATE = const(19)
_IRQ_GATTS_INDICATE_DONE = const(20)
_IRQ_MTU_EXCHANGED = const(21)
_IRQ_L2CAP_ACCEPT = const(22)
_IRQ_L2CAP_CONNECT = const(23)
_IRQ_L2CAP_DISCONNECT = const(24)
_IRQ_L2CAP_RECV = const(25)
_IRQ_L2CAP_SEND_READY = const(26)
_IRQ_CONNECTION_UPDATE = const(27)
_IRQ_ENCRYPTION_UPDATE = const(28)
_IRQ_GET_SECRET = const(29)
_IRQ_SET_SECRET = const(30)
_IRQ_PASSKEY_ACTION = const(31)

_FLAG_BROADCAST = const(0x0001)
_FLAG_READ = const(0x0002)
_FLAG_WRITE_NO_RESPONSE = const(0x0004)
_FLAG_WRITE = const(0x0008)
_FLAG_NOTIFY = const(0x0010)
_FLAG_INDICATE = const(0x0020)
_FLAG_AUTHENTICATED_SIGNED_WRITE = const(0x0040)

_FLAG_AUX_WRITE = const(0x0100)
_FLAG_READ_ENCRYPTED = const(0x0200)
_FLAG_READ_AUTHENTICATED = const(0x0400)
_FLAG_READ_AUTHORIZED = const(0x0800)
_FLAG_WRITE_ENCRYPTED = const(0x1000)
_FLAG_WRITE_AUTHENTICATED = const(0x2000)
_FLAG_WRITE_AUTHORIZED = const(0x4000)

_GATTS_NO_ERROR = const(0x00)
_GATTS_ERROR_READ_NOT_PERMITTED = const(0x02)
_GATTS_ERROR_WRITE_NOT_PERMITTED = const(0x03)
_GATTS_ERROR_INSUFFICIENT_AUTHENTICATION = const(0x05)
_GATTS_ERROR_INSUFFICIENT_AUTHORIZATION = const(0x08)
_GATTS_ERROR_INSUFFICIENT_ENCRYPTION = const(0x0f)

_IO_CAPABILITY_DISPLAY_ONLY = const(0)
_IO_CAPABILITY_DISPLAY_YESNO = const(1)
_IO_CAPABILITY_KEYBOARD_ONLY = const(2)
_IO_CAPABILITY_NO_INPUT_OUTPUT = const(3)
_IO_CAPABILITY_KEYBOARD_DISPLAY = const(4)

_PASSKEY_ACTION_NONE = const(0)
_PASSKEY_ACTION_INPUT = const(2)
_PASSKEY_ACTION_DISPLAY = const(3)
_PASSKEY_ACTION_NUMERIC_COMPARISON = const(4)
*/

// bluetooth.UUID type.
// Ports are expected to map this to their own internal UUID types.
// Internally the UUID data is little-endian, but the user should only
// ever see this if they use the buffer protocol, e.g. in order to
// construct an advertising payload (which needs to be in LE).
// Both the constructor and the print function work in BE.
typedef struct {
    mp_obj_base_t base;
    uint8_t type;
    uint8_t data[16];
} mp_obj_bluetooth_uuid_t;

extern const mp_obj_type_t mp_type_bluetooth_uuid;

//////////////////////////////////////////////////////////////
// API implemented by ports (i.e. called from modbluetooth.c):

// TODO: At the moment this only allows for a single `Bluetooth` instance to be created.
// Ideally in the future we'd be able to have multiple instances or to select a specific BT driver or HCI UART.
// So these global methods should be replaced with a struct of function pointers (like the machine.I2C implementations).

// Any method returning an int returns errno on failure, otherwise zero.

// Note: All methods dealing with addresses (as 6-byte uint8 pointers) are in big-endian format.
// (i.e. the same way they would be printed on a device sticker or in a UI), so the user sees
// addresses in a way that looks like what they'd expect.
// This means that the lower level implementation will likely need to reorder them (e.g. Nimble
// works in little-endian, as does BLE itself).

// Enables the Bluetooth stack.
int mp_bluetooth_init(void);

// Disables the Bluetooth stack. Is a no-op when not enabled.
void mp_bluetooth_deinit(void);

// Returns true when the Bluetooth stack is active.
bool mp_bluetooth_is_active(void);

// Gets the current address of this device in big-endian format.
void mp_bluetooth_get_current_address(uint8_t *addr_type, uint8_t *addr);

// Sets the addressing mode to use.
void mp_bluetooth_set_address_mode(uint8_t addr_mode);

#if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
// Set bonding flag in pairing requests (i.e. persist security keys).
void mp_bluetooth_set_bonding(bool enabled);
// Require MITM protection.
void mp_bluetooth_set_mitm_protection(bool enabled);
// Require LE Secure pairing (rather than Legacy Pairing)
void mp_bluetooth_set_le_secure(bool enabled);
// I/O capabilities for authentication (see MP_BLUETOOTH_IO_CAPABILITY_*).
void mp_bluetooth_set_io_capability(uint8_t capability);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING

// Get or set the GAP device name that will be used by service 0x1800, characteristic 0x2a00.
size_t mp_bluetooth_gap_get_device_name(const uint8_t **buf);
int mp_bluetooth_gap_set_device_name(const uint8_t *buf, size_t len);

// Start advertisement. Will re-start advertisement when already enabled.
// Returns errno on failure.
int mp_bluetooth_gap_advertise_start(bool connectable, int32_t interval_us, const uint8_t *adv_data, size_t adv_data_len, const uint8_t *sr_data, size_t sr_data_len);

// Stop advertisement. No-op when already stopped.
void mp_bluetooth_gap_advertise_stop(void);

// Start adding services. Must be called before mp_bluetooth_register_service.
int mp_bluetooth_gatts_register_service_begin(bool append);
// Add a service with the given list of characteristics to the queue to be registered.
// The value_handles won't be valid until after mp_bluetooth_register_service_end is called.
int mp_bluetooth_gatts_register_service(mp_obj_bluetooth_uuid_t *service_uuid, mp_obj_bluetooth_uuid_t **characteristic_uuids, uint16_t *characteristic_flags, mp_obj_bluetooth_uuid_t **descriptor_uuids, uint16_t *descriptor_flags, uint8_t *num_descriptors, uint16_t *handles, size_t num_characteristics);
// Register any queued services.
int mp_bluetooth_gatts_register_service_end(void);

// Read the value from the local gatts db (likely this has been written by a central).
int mp_bluetooth_gatts_read(uint16_t value_handle, uint8_t **value, size_t *value_len);
// Write a value to the local gatts db (ready to be queried by a central). Optionally send notifications/indications.
int mp_bluetooth_gatts_write(uint16_t value_handle, const uint8_t *value, size_t value_len, bool send_update);
// Notify the central that it should do a read.
int mp_bluetooth_gatts_notify(uint16_t conn_handle, uint16_t value_handle);
// Notify the central, including a data payload. (Note: does not set the gatts db value).
int mp_bluetooth_gatts_notify_send(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t value_len);
// Indicate the central.
int mp_bluetooth_gatts_indicate(uint16_t conn_handle, uint16_t value_handle);

// Resize and enable/disable append-mode on a value.
// Append-mode means that remote writes will append and local reads will clear after reading.
int mp_bluetooth_gatts_set_buffer(uint16_t value_handle, size_t len, bool append);

// Disconnect from a central or peripheral.
int mp_bluetooth_gap_disconnect(uint16_t conn_handle);

// Set/get the MTU that we will respond to a MTU exchange with.
int mp_bluetooth_get_preferred_mtu(void);
int mp_bluetooth_set_preferred_mtu(uint16_t mtu);

#if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
// Initiate pairing on the specified connection.
int mp_bluetooth_gap_pair(uint16_t conn_handle);

// Respond to a pairing request.
int mp_bluetooth_gap_passkey(uint16_t conn_handle, uint8_t action, mp_int_t passkey);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
// Start a discovery (scan). Set duration to zero to run continuously.
int mp_bluetooth_gap_scan_start(int32_t duration_ms, int32_t interval_us, int32_t window_us, bool active_scan);

// Stop discovery (if currently active).
int mp_bluetooth_gap_scan_stop(void);

// Connect to a found peripheral.
int mp_bluetooth_gap_peripheral_connect(uint8_t addr_type, const uint8_t *addr, int32_t duration_ms);
#endif

#if MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT
// Find all primary services on the connected peripheral.
int mp_bluetooth_gattc_discover_primary_services(uint16_t conn_handle, const mp_obj_bluetooth_uuid_t *uuid);

// Find all characteristics on the specified service on a connected peripheral.
int mp_bluetooth_gattc_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, const mp_obj_bluetooth_uuid_t *uuid);

// Find all descriptors on the specified characteristic on a connected peripheral.
int mp_bluetooth_gattc_discover_descriptors(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);

// Initiate read of a value from the remote peripheral.
int mp_bluetooth_gattc_read(uint16_t conn_handle, uint16_t value_handle);

// Write the value to the remote peripheral.
int mp_bluetooth_gattc_write(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len, unsigned int mode);

// Initiate MTU exchange for a specific connection using the preferred MTU.
int mp_bluetooth_gattc_exchange_mtu(uint16_t conn_handle);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT

#if MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS
int mp_bluetooth_l2cap_listen(uint16_t psm, uint16_t mtu);
int mp_bluetooth_l2cap_connect(uint16_t conn_handle, uint16_t psm, uint16_t mtu);
int mp_bluetooth_l2cap_disconnect(uint16_t conn_handle, uint16_t cid);
int mp_bluetooth_l2cap_send(uint16_t conn_handle, uint16_t cid, const uint8_t *buf, size_t len, bool *stalled);
int mp_bluetooth_l2cap_recvinto(uint16_t conn_handle, uint16_t cid, uint8_t *buf, size_t *len);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS

#if MICROPY_PY_BLUETOOTH_ENABLE_HCI_CMD
int mp_bluetooth_hci_cmd(uint16_t ogf, uint16_t ocf, const uint8_t *req, size_t req_len, uint8_t *resp, size_t resp_len, uint8_t *status);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_HCI_CMD

/////////////////////////////////////////////////////////////////////////////
// API implemented by modbluetooth (called by port-specific implementations):

// Notify modbluetooth that a connection/disconnection event has occurred.
void mp_bluetooth_gap_on_connected_disconnected(uint8_t event, uint16_t conn_handle, uint8_t addr_type, const uint8_t *addr);

// Call this when any connection parameters have been changed.
void mp_bluetooth_gap_on_connection_update(uint16_t conn_handle, uint16_t conn_interval, uint16_t conn_latency, uint16_t supervision_timeout, uint16_t status);

#if MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING
// Call this when any connection encryption has been changed (e.g. during pairing).
void mp_bluetooth_gatts_on_encryption_update(uint16_t conn_handle, bool encrypted, bool authenticated, bool bonded, uint8_t key_size);

// Call this when you need the application to manage persistent key data.
// For get, if key is NULL, then the implementation must return the index'th matching key. Otherwise it should return a specific key.
// For set, if value is NULL, then delete.
// The "type" is stack-specific, but could also be used to implement versioning.
bool mp_bluetooth_gap_on_get_secret(uint8_t type, uint8_t index, const uint8_t *key, size_t key_len, const uint8_t **value, size_t *value_len);
bool mp_bluetooth_gap_on_set_secret(uint8_t type, const uint8_t *key, size_t key_len, const uint8_t *value, size_t value_len);

// Call this when a passkey verification needs to be processed.
void mp_bluetooth_gap_on_passkey_action(uint16_t conn_handle, uint8_t action, mp_int_t passkey);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING

// Call this when a characteristic is written to.
void mp_bluetooth_gatts_on_write(uint16_t conn_handle, uint16_t value_handle);

// Call this when an acknowledgment is received for an indication.
void mp_bluetooth_gatts_on_indicate_complete(uint16_t conn_handle, uint16_t value_handle, uint8_t status);

// Call this when a characteristic is read from (giving the handler a chance to update the stored value).
// Return 0 to allow the read, otherwise a non-zero rejection reason (see MP_BLUETOOTH_GATTS_ERROR_*).
mp_int_t mp_bluetooth_gatts_on_read_request(uint16_t conn_handle, uint16_t value_handle);

// Call this when an MTU exchange completes.
void mp_bluetooth_gatts_on_mtu_exchanged(uint16_t conn_handle, uint16_t value);

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
// Notify modbluetooth that scan has finished, either timeout, manually, or by some other action (e.g. connecting).
void mp_bluetooth_gap_on_scan_complete(void);

// Notify modbluetooth of a scan result.
void mp_bluetooth_gap_on_scan_result(uint8_t addr_type, const uint8_t *addr, uint8_t adv_type, const int8_t rssi, const uint8_t *data, size_t data_len);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE

#if MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT
// Notify modbluetooth that a service was found (either by discover-all, or discover-by-uuid).
void mp_bluetooth_gattc_on_primary_service_result(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, mp_obj_bluetooth_uuid_t *service_uuid);

// Notify modbluetooth that a characteristic was found (either by discover-all-on-service, or discover-by-uuid-on-service).
void mp_bluetooth_gattc_on_characteristic_result(uint16_t conn_handle, uint16_t def_handle, uint16_t value_handle, uint8_t properties, mp_obj_bluetooth_uuid_t *characteristic_uuid);

// Notify modbluetooth that a descriptor was found.
void mp_bluetooth_gattc_on_descriptor_result(uint16_t conn_handle, uint16_t handle, mp_obj_bluetooth_uuid_t *descriptor_uuid);

// Notify modbluetooth that service, characteristic or descriptor discovery has finished.
void mp_bluetooth_gattc_on_discover_complete(uint8_t event, uint16_t conn_handle, uint16_t status);

// Notify modbluetooth that a read has completed with data (or notify/indicate data available, use `event` to disambiguate).
void mp_bluetooth_gattc_on_data_available(uint8_t event, uint16_t conn_handle, uint16_t value_handle, const uint8_t **data, uint16_t *data_len, size_t num);

// Notify modbluetooth that a read or write operation has completed.
void mp_bluetooth_gattc_on_read_write_status(uint8_t event, uint16_t conn_handle, uint16_t value_handle, uint16_t status);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_GATT_CLIENT

#if MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS
mp_int_t mp_bluetooth_on_l2cap_accept(uint16_t conn_handle, uint16_t cid, uint16_t psm, uint16_t our_mtu, uint16_t peer_mtu);
void mp_bluetooth_on_l2cap_connect(uint16_t conn_handle, uint16_t cid, uint16_t psm, uint16_t our_mtu, uint16_t peer_mtu);
void mp_bluetooth_on_l2cap_disconnect(uint16_t conn_handle, uint16_t cid, uint16_t psm, uint16_t status);
void mp_bluetooth_on_l2cap_send_ready(uint16_t conn_handle, uint16_t cid, uint8_t status);
void mp_bluetooth_on_l2cap_recv(uint16_t conn_handle, uint16_t cid);
#endif // MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS

// For stacks that don't manage attribute value data (currently all of them), helpers
// to store this in a map, keyed by value handle.

typedef struct {
    // Pointer to heap-allocated data.
    uint8_t *data;
    // Allocated size of data.
    size_t data_alloc;
    // Current bytes in use.
    size_t data_len;
    // Whether new writes append or replace existing data (default false).
    bool append;
} mp_bluetooth_gatts_db_entry_t;

typedef mp_map_t *mp_gatts_db_t;

STATIC inline void mp_bluetooth_gatts_db_create(mp_gatts_db_t *db) {
    *db = m_new(mp_map_t, 1);
}

STATIC inline void mp_bluetooth_gatts_db_reset(mp_gatts_db_t db) {
    mp_map_init(db, 0);
}

void mp_bluetooth_gatts_db_create_entry(mp_gatts_db_t db, uint16_t handle, size_t len);
mp_bluetooth_gatts_db_entry_t *mp_bluetooth_gatts_db_lookup(mp_gatts_db_t db, uint16_t handle);
int mp_bluetooth_gatts_db_read(mp_gatts_db_t db, uint16_t handle, uint8_t **value, size_t *value_len);
int mp_bluetooth_gatts_db_write(mp_gatts_db_t db, uint16_t handle, const uint8_t *value, size_t value_len);
int mp_bluetooth_gatts_db_resize(mp_gatts_db_t db, uint16_t handle, size_t len, bool append);

#endif // MICROPY_INCLUDED_EXTMOD_MODBLUETOOTH_H
