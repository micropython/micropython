/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
 * Copyright (c) 2019 Jim Mussared
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

#ifndef MICROPY_PY_BLUETOOTH_GATTS_ON_READ_CALLBACK
#define MICROPY_PY_BLUETOOTH_GATTS_ON_READ_CALLBACK (0)
#endif

// This is used to protect the ringbuffer.
#ifndef MICROPY_PY_BLUETOOTH_ENTER
#define MICROPY_PY_BLUETOOTH_ENTER mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
#define MICROPY_PY_BLUETOOTH_EXIT MICROPY_END_ATOMIC_SECTION(atomic_state);
#endif

// Common constants.
#ifndef MP_BLUETOOTH_MAX_ATTR_SIZE
#define MP_BLUETOOTH_MAX_ATTR_SIZE (20)
#endif

// Advertisement packet lengths
#define MP_BLUETOOTH_GAP_ADV_MAX_LEN (32)

#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_READ     (1 << 1)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_WRITE    (1 << 3)
#define MP_BLUETOOTH_CHARACTERISTIC_FLAG_NOTIFY   (1 << 4)

// For mp_bluetooth_gattc_write, the mode parameter
#define MP_BLUETOOTH_WRITE_MODE_NO_RESPONSE     (0)
#define MP_BLUETOOTH_WRITE_MODE_WITH_RESPONSE   (1)

// Type value also doubles as length.
#define MP_BLUETOOTH_UUID_TYPE_16  (2)
#define MP_BLUETOOTH_UUID_TYPE_32  (4)
#define MP_BLUETOOTH_UUID_TYPE_128 (16)

// Address types (for the addr_type params).
// Ports will need to map these to their own values.
#define MP_BLUETOOTH_ADDR_PUBLIC                        (0x00)  // Public (identity) address. (Same as NimBLE and NRF SD)
#define MP_BLUETOOTH_ADDR_RANDOM_STATIC                 (0x01)  // Random static (identity) address. (Same as NimBLE and NRF SD)
#define MP_BLUETOOTH_ADDR_PUBLIC_ID                     (0x02)  // (Same as NimBLE)
#define MP_BLUETOOTH_ADDR_RANDOM_ID                     (0x03)  // (Same as NimBLE)
#define MP_BLUETOOTH_ADDR_RANDOM_PRIVATE_RESOLVABLE     (0x12) // Random private resolvable address. (NRF SD 0x02)
#define MP_BLUETOOTH_ADDR_RANDOM_PRIVATE_NON_RESOLVABLE (0x13) // Random private non-resolvable address. (NRF SD 0x03)

// Event codes for the IRQ handler.
// Can also be combined to pass to the trigger param to select which events you
// are interested in.
// Note this is currently stored in a uint16_t (in irq_trigger, and the event
// arg to the irq handler), so one spare value remaining.
#define MP_BLUETOOTH_IRQ_CENTRAL_CONNECT              (1 << 0)
#define MP_BLUETOOTH_IRQ_CENTRAL_DISCONNECT           (1 << 1)
#define MP_BLUETOOTH_IRQ_GATTS_WRITE                  (1 << 2)
#define MP_BLUETOOTH_IRQ_GATTS_READ_REQUEST           (1 << 3)
#define MP_BLUETOOTH_IRQ_SCAN_RESULT                  (1 << 4)
#define MP_BLUETOOTH_IRQ_SCAN_COMPLETE                (1 << 5)
#define MP_BLUETOOTH_IRQ_PERIPHERAL_CONNECT           (1 << 6)
#define MP_BLUETOOTH_IRQ_PERIPHERAL_DISCONNECT        (1 << 7)
#define MP_BLUETOOTH_IRQ_GATTC_SERVICE_RESULT         (1 << 8)
#define MP_BLUETOOTH_IRQ_GATTC_CHARACTERISTIC_RESULT  (1 << 9)
#define MP_BLUETOOTH_IRQ_GATTC_DESCRIPTOR_RESULT      (1 << 10)
#define MP_BLUETOOTH_IRQ_GATTC_READ_RESULT            (1 << 11)
#define MP_BLUETOOTH_IRQ_GATTC_WRITE_STATUS           (1 << 12)
#define MP_BLUETOOTH_IRQ_GATTC_NOTIFY                 (1 << 13)
#define MP_BLUETOOTH_IRQ_GATTC_INDICATE               (1 << 14)
#define MP_BLUETOOTH_IRQ_ALL                          (0xffff)

/*
These aren't included in the module for space reasons, but can be used
in your Python code if necessary.

from micropython import const
_IRQ_CENTRAL_CONNECT                 = const(1 << 0)
_IRQ_CENTRAL_DISCONNECT              = const(1 << 1)
_IRQ_GATTS_WRITE                     = const(1 << 2)
_IRQ_GATTS_READ_REQUEST              = const(1 << 3)
_IRQ_SCAN_RESULT                     = const(1 << 4)
_IRQ_SCAN_COMPLETE                   = const(1 << 5)
_IRQ_PERIPHERAL_CONNECT              = const(1 << 6)
_IRQ_PERIPHERAL_DISCONNECT           = const(1 << 7)
_IRQ_GATTC_SERVICE_RESULT            = const(1 << 8)
_IRQ_GATTC_CHARACTERISTIC_RESULT     = const(1 << 9)
_IRQ_GATTC_DESCRIPTOR_RESULT         = const(1 << 10)
_IRQ_GATTC_READ_RESULT               = const(1 << 11)
_IRQ_GATTC_WRITE_STATUS              = const(1 << 12)
_IRQ_GATTC_NOTIFY                    = const(1 << 13)
_IRQ_GATTC_INDICATE                  = const(1 << 14)
_IRQ_ALL                             = const(0xffff)
*/

// Common UUID type.
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

// Returns true when the Bluetooth stack is enabled.
bool mp_bluetooth_is_enabled(void);

// Gets the MAC addr of this device in big-endian format.
void mp_bluetooth_get_device_addr(uint8_t *addr);

// Start advertisement. Will re-start advertisement when already enabled.
// Returns errno on failure.
int mp_bluetooth_gap_advertise_start(bool connectable, int32_t interval_us, const uint8_t *adv_data, size_t adv_data_len, const uint8_t *sr_data, size_t sr_data_len);

// Stop advertisement. No-op when already stopped.
void mp_bluetooth_gap_advertise_stop(void);

// Start adding services. Must be called before mp_bluetooth_register_service.
int mp_bluetooth_gatts_register_service_begin(bool append);
// // Add a service with the given list of characteristics to the queue to be registered.
// The value_handles won't be valid until after mp_bluetooth_register_service_end is called.
int mp_bluetooth_gatts_register_service(mp_obj_bluetooth_uuid_t *service_uuid, mp_obj_bluetooth_uuid_t **characteristic_uuids, uint8_t *characteristic_flags, mp_obj_bluetooth_uuid_t **descriptor_uuids, uint8_t *descriptor_flags, uint8_t *num_descriptors, uint16_t *handles, size_t num_characteristics);
// Register any queued services.
int mp_bluetooth_gatts_register_service_end();

// Read the value from the local gatts db (likely this has been written by a central).
int mp_bluetooth_gatts_read(uint16_t value_handle, uint8_t **value, size_t *value_len);
// Write a value to the local gatts db (ready to be queried by a central).
int mp_bluetooth_gatts_write(uint16_t value_handle, const uint8_t *value, size_t value_len);
// Notify the central that it should do a read.
int mp_bluetooth_gatts_notify(uint16_t conn_handle, uint16_t value_handle);
// Notify the central, including a data payload. (Note: does not set the gatts db value).
int mp_bluetooth_gatts_notify_send(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len);
// Indicate the central.
int mp_bluetooth_gatts_indicate(uint16_t conn_handle, uint16_t value_handle);

// Resize and enable/disable append-mode on a value.
// Append-mode means that remote writes will append and local reads will clear after reading.
int mp_bluetooth_gatts_set_buffer(uint16_t value_handle, size_t len, bool append);

// Disconnect from a central or peripheral.
int mp_bluetooth_gap_disconnect(uint16_t conn_handle);

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
// Start a discovery (scan). Set duration to zero to run continuously.
int mp_bluetooth_gap_scan_start(int32_t duration_ms, int32_t interval_us, int32_t window_us);

// Stop discovery (if currently active).
int mp_bluetooth_gap_scan_stop(void);

// Connect to a found peripheral.
int mp_bluetooth_gap_peripheral_connect(uint8_t addr_type, const uint8_t *addr, int32_t duration_ms);

// Find all primary services on the connected peripheral.
int mp_bluetooth_gattc_discover_primary_services(uint16_t conn_handle);

// Find all characteristics on the specified service on a connected peripheral.
int mp_bluetooth_gattc_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);

// Find all descriptors on the specified characteristic on a connected peripheral.
int mp_bluetooth_gattc_discover_descriptors(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);

// Initiate read of a value from the remote peripheral.
int mp_bluetooth_gattc_read(uint16_t conn_handle, uint16_t value_handle);

// Write the value to the remote peripheral.
int mp_bluetooth_gattc_write(uint16_t conn_handle, uint16_t value_handle, const uint8_t *value, size_t *value_len, unsigned int mode);
#endif

/////////////////////////////////////////////////////////////////////////////
// API implemented by modbluetooth (called by port-specific implementations):

// Notify modbluetooth that a connection/disconnection event has occurred.
void mp_bluetooth_gap_on_connected_disconnected(uint16_t event, uint16_t conn_handle, uint8_t addr_type, const uint8_t *addr);

// Call this when a characteristic is written to.
void mp_bluetooth_gatts_on_write(uint16_t conn_handle, uint16_t value_handle);

#if MICROPY_PY_BLUETOOTH_GATTS_ON_READ_CALLBACK
// Call this when a characteristic is read from. Return false to deny the read.
bool mp_bluetooth_gatts_on_read_request(uint16_t conn_handle, uint16_t value_handle);
#endif

#if MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
// Notify modbluetooth that scan has finished, either timeout, manually, or by some other action (e.g. connecting).
void mp_bluetooth_gap_on_scan_complete(void);

// Notify modbluetooth of a scan result.
void mp_bluetooth_gap_on_scan_result(uint8_t addr_type, const uint8_t *addr, bool connectable, const int8_t rssi, const uint8_t *data, size_t data_len);

// Notify modbluetooth that a service was found (either by discover-all, or discover-by-uuid).
void mp_bluetooth_gattc_on_primary_service_result(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle, mp_obj_bluetooth_uuid_t *service_uuid);

// Notify modbluetooth that a characteristic was found (either by discover-all-on-service, or discover-by-uuid-on-service).
void mp_bluetooth_gattc_on_characteristic_result(uint16_t conn_handle, uint16_t def_handle, uint16_t value_handle, uint8_t properties, mp_obj_bluetooth_uuid_t *characteristic_uuid);

// Notify modbluetooth that a descriptor was found.
void mp_bluetooth_gattc_on_descriptor_result(uint16_t conn_handle, uint16_t handle, mp_obj_bluetooth_uuid_t *descriptor_uuid);

// Notify modbluetooth that a read has completed with data (or notify/indicate data available, use `event` to disambiguate).
// Note: these functions are to be called in a group protected by MICROPY_PY_BLUETOOTH_ENTER/EXIT.
// _start returns the number of bytes to submit to the calls to _chunk, followed by a call to _end.
size_t mp_bluetooth_gattc_on_data_available_start(uint16_t event, uint16_t conn_handle, uint16_t value_handle, size_t data_len);
void mp_bluetooth_gattc_on_data_available_chunk(const uint8_t *data, size_t data_len);
void mp_bluetooth_gattc_on_data_available_end(void);

// Notify modbluetooth that a write has completed.
void mp_bluetooth_gattc_on_write_status(uint16_t conn_handle, uint16_t value_handle, uint16_t status);
#endif

#endif // MICROPY_INCLUDED_EXTMOD_MODBLUETOOTH_H
