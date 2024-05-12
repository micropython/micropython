/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include <string.h>

#include "genhdr/mpversion.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-module/storage/__init__.h"
#include "supervisor/shared/bluetooth/serial.h"

#include "common-hal/_bleio/__init__.h"

#include "py/mpstate.h"

STATIC bleio_service_obj_t supervisor_ble_circuitpython_service;
STATIC bleio_uuid_obj_t supervisor_ble_circuitpython_service_uuid;
STATIC bleio_characteristic_obj_t supervisor_ble_circuitpython_rx_characteristic;
STATIC bleio_uuid_obj_t supervisor_ble_circuitpython_rx_uuid;
STATIC bleio_characteristic_obj_t supervisor_ble_circuitpython_tx_characteristic;
STATIC bleio_uuid_obj_t supervisor_ble_circuitpython_tx_uuid;
STATIC bleio_characteristic_obj_t supervisor_ble_circuitpython_version_characteristic;
STATIC bleio_uuid_obj_t supervisor_ble_circuitpython_version_uuid;

// This is the base UUID for the CircuitPython service.
const uint8_t circuitpython_base_uuid[16] = {0x6e, 0x68, 0x74, 0x79, 0x50, 0x74, 0x69, 0x75, 0x63, 0x72, 0x69, 0x43, 0x00, 0x00, 0xaf, 0xad };

STATIC mp_obj_list_t characteristic_list;
STATIC mp_obj_t characteristic_list_items[3];

STATIC uint32_t _outgoing1[BLE_GATTS_VAR_ATTR_LEN_MAX / 4];
STATIC uint32_t _outgoing2[BLE_GATTS_VAR_ATTR_LEN_MAX / 4];
STATIC ble_drv_evt_handler_entry_t rx_static_handler_entry;
STATIC ble_drv_evt_handler_entry_t tx_static_handler_entry;
STATIC bleio_packet_buffer_obj_t _tx_packet_buffer;
STATIC uint32_t _incoming[64];
STATIC bleio_characteristic_buffer_obj_t _rx_buffer;

// Internal enabling so we can disable while printing BLE debugging.
STATIC bool _enabled;

void supervisor_start_bluetooth_serial(void) {
    supervisor_ble_circuitpython_service_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_circuitpython_service_uuid, 0x0001, circuitpython_base_uuid);

    // We know we'll only be N characteristics so we can statically allocate it.
    characteristic_list.base.type = &mp_type_list;
    characteristic_list.alloc = sizeof(characteristic_list_items) / sizeof(characteristic_list_items[0]);
    characteristic_list.len = 0;
    characteristic_list.items = characteristic_list_items;
    mp_seq_clear(characteristic_list.items, 0, characteristic_list.alloc, sizeof(*characteristic_list.items));

    supervisor_ble_circuitpython_service.base.type = &bleio_service_type;
    _common_hal_bleio_service_construct(&supervisor_ble_circuitpython_service, &supervisor_ble_circuitpython_service_uuid, false /* is secondary */, &characteristic_list);

    // RX
    supervisor_ble_circuitpython_rx_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_circuitpython_rx_uuid, 0x0002, circuitpython_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_circuitpython_rx_characteristic,
        &supervisor_ble_circuitpython_service,
        0,                                       // handle (for remote only)
        &supervisor_ble_circuitpython_rx_uuid,
        CHAR_PROP_WRITE | CHAR_PROP_WRITE_NO_RESPONSE,
        SECURITY_MODE_NO_ACCESS,
        SECURITY_MODE_ENC_NO_MITM,
        BLE_GATTS_VAR_ATTR_LEN_MAX,                 // max length
        false,                                      // fixed length
        NULL,                                      // no initial value
        NULL);

    // TX
    supervisor_ble_circuitpython_tx_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_circuitpython_tx_uuid, 0x0003, circuitpython_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_circuitpython_tx_characteristic,
        &supervisor_ble_circuitpython_service,
        0,                                       // handle (for remote only)
        &supervisor_ble_circuitpython_tx_uuid,
        CHAR_PROP_NOTIFY,
        SECURITY_MODE_ENC_NO_MITM,
        SECURITY_MODE_NO_ACCESS,
        BLE_GATTS_VAR_ATTR_LEN_MAX,                  // max length
        false,                                       // fixed length
        NULL,                                       // no initial value
        NULL);

    // Version number
    const char *version = MICROPY_GIT_TAG;
    mp_buffer_info_t bufinfo;
    bufinfo.buf = (uint8_t *)version;
    bufinfo.len = strlen(version);

    supervisor_ble_circuitpython_version_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_circuitpython_version_uuid, 0x0100, circuitpython_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_circuitpython_version_characteristic,
        &supervisor_ble_circuitpython_service,
        0,                                       // handle (for remote only)
        &supervisor_ble_circuitpython_version_uuid,
        CHAR_PROP_READ,
        SECURITY_MODE_OPEN,
        SECURITY_MODE_NO_ACCESS,
        bufinfo.len,                                // max length
        true,                                       // fixed length
        NULL,                                       // no initial value
        NULL); // no description

    common_hal_bleio_characteristic_set_value(&supervisor_ble_circuitpython_version_characteristic, &bufinfo);

    // Use a PacketBuffer to transmit so that we glom characters to transmit
    // together and save BLE overhead.
    _common_hal_bleio_packet_buffer_construct(
        &_tx_packet_buffer, &supervisor_ble_circuitpython_tx_characteristic,
        NULL, 0,
        _outgoing1, _outgoing2, BLE_GATTS_VAR_ATTR_LEN_MAX,
        &tx_static_handler_entry);

    // Use a CharacteristicBuffer for rx so we can read a single character at a time.
    _common_hal_bleio_characteristic_buffer_construct(&_rx_buffer,
        &supervisor_ble_circuitpython_rx_characteristic,
        0.1f,
        (uint8_t *)_incoming, sizeof(_incoming) * sizeof(uint32_t),
        &rx_static_handler_entry,
        true /* watch for interrupt character */);

    _enabled = true;
}

void supervisor_stop_bluetooth_serial(void) {
    if (common_hal_bleio_packet_buffer_deinited(&_tx_packet_buffer)) {
        return;
    }
    if (!_enabled) {
        return;
    }
    common_hal_bleio_packet_buffer_flush(&_tx_packet_buffer);
}

bool ble_serial_connected(void) {
    return _tx_packet_buffer.conn_handle != BLE_CONN_HANDLE_INVALID;
}

uint32_t ble_serial_available(void) {
    if (_enabled && !common_hal_bleio_characteristic_buffer_deinited(&_rx_buffer)) {
        return common_hal_bleio_characteristic_buffer_rx_characters_available(&_rx_buffer);
    } else {
        return 0;
    }
}

char ble_serial_read_char(void) {
    if (common_hal_bleio_characteristic_buffer_deinited(&_rx_buffer)) {
        return -1;
    }
    if (!_enabled) {
        return -1;
    }
    uint8_t c;
    common_hal_bleio_characteristic_buffer_read(&_rx_buffer, &c, 1, NULL);
    return c;
}

void ble_serial_write(const char *text, size_t len) {
    if (common_hal_bleio_packet_buffer_deinited(&_tx_packet_buffer)) {
        return;
    }
    if (!_enabled) {
        return;
    }
    size_t sent = 0;
    while (sent < len) {
        uint16_t packet_size = MIN(len - sent, (size_t)common_hal_bleio_packet_buffer_get_outgoing_packet_length(&_tx_packet_buffer));
        mp_int_t written = common_hal_bleio_packet_buffer_write(&_tx_packet_buffer, (const uint8_t *)text + sent, packet_size, NULL, 0);
        // Error, so we drop characters to transmit.
        if (written < 0) {
            break;
        }
        sent += written;
    }
}

void ble_serial_enable(void) {
    _enabled = true;
}

void ble_serial_disable(void) {
    _enabled = false;
}
