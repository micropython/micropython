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

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-module/storage/__init__.h"

#include "common-hal/_bleio/__init__.h"

#include "py/mpstate.h"

STATIC bleio_service_obj_t supervisor_ble_serial_service;
STATIC bleio_uuid_obj_t supervisor_ble_serial_service_uuid;
STATIC bleio_characteristic_obj_t supervisor_ble_rx_characteristic;
STATIC bleio_uuid_obj_t supervisor_ble_rx_uuid;
STATIC bleio_characteristic_obj_t supervisor_ble_tx_characteristic;
STATIC bleio_uuid_obj_t supervisor_ble_tx_uuid;

// This is the base UUID for the nordic uart service.
const uint8_t nordic_uart_base_uuid[16] = {0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x00, 0x00, 0x40, 0x6e };

STATIC mp_obj_list_t characteristic_list;
STATIC mp_obj_t characteristic_list_items[2];

STATIC uint32_t _outgoing1[BLE_GATTS_VAR_ATTR_LEN_MAX / 4];
STATIC uint32_t _outgoing2[BLE_GATTS_VAR_ATTR_LEN_MAX / 4];
STATIC ble_drv_evt_handler_entry_t rx_static_handler_entry;
STATIC ble_drv_evt_handler_entry_t tx_static_handler_entry;
STATIC bleio_packet_buffer_obj_t _tx_packet_buffer;
STATIC uint32_t _incoming[64];
STATIC bleio_characteristic_buffer_obj_t _rx_buffer;

void supervisor_start_bluetooth_serial(void) {
    supervisor_ble_serial_service_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_serial_service_uuid, 0x0001, nordic_uart_base_uuid);

    // We know we'll only be N characteristics so we can statically allocate it.
    characteristic_list.base.type = &mp_type_list;
    characteristic_list.alloc = sizeof(characteristic_list_items) / sizeof(characteristic_list_items[0]);
    characteristic_list.len = 0;
    characteristic_list.items = characteristic_list_items;
    mp_seq_clear(characteristic_list.items, 0, characteristic_list.alloc, sizeof(*characteristic_list.items));

    _common_hal_bleio_service_construct(&supervisor_ble_serial_service, &supervisor_ble_serial_service_uuid, false /* is secondary */, &characteristic_list);

    // RX
    supervisor_ble_rx_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_rx_uuid, 0x0002, nordic_uart_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_rx_characteristic,
        &supervisor_ble_serial_service,
        0,                                       // handle (for remote only)
        &supervisor_ble_rx_uuid,
        CHAR_PROP_WRITE | CHAR_PROP_WRITE_NO_RESPONSE,
        SECURITY_MODE_NO_ACCESS,
        SECURITY_MODE_ENC_NO_MITM,
        BLE_GATTS_VAR_ATTR_LEN_MAX,                 // max length
        false,                                      // fixed length
        NULL,                                      // no initial value
        "CircuitPython Serial");

    // TX
    supervisor_ble_tx_uuid.base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(&supervisor_ble_tx_uuid, 0x0003, nordic_uart_base_uuid);
    common_hal_bleio_characteristic_construct(&supervisor_ble_tx_characteristic,
        &supervisor_ble_serial_service,
        0,                                       // handle (for remote only)
        &supervisor_ble_tx_uuid,
        CHAR_PROP_NOTIFY,
        SECURITY_MODE_ENC_NO_MITM,
        SECURITY_MODE_NO_ACCESS,
        BLE_GATTS_VAR_ATTR_LEN_MAX,                  // max length
        false,                                       // fixed length
        NULL,                                       // no initial value
        "CircuitPython Serial");

    // Use a PacketBuffer to transmit so that we glom characters to transmit
    // together and save BLE overhead.
    _common_hal_bleio_packet_buffer_construct(
        &_tx_packet_buffer, &supervisor_ble_tx_characteristic,
        NULL, 0,
        _outgoing1, _outgoing2, BLE_GATTS_VAR_ATTR_LEN_MAX,
        &tx_static_handler_entry);

    // Use a CharacteristicBuffer for rx so we can read a single character at a time.
    _common_hal_bleio_characteristic_buffer_construct(&_rx_buffer,
        &supervisor_ble_rx_characteristic,
        0.1f,
        (uint8_t *)_incoming, sizeof(_incoming) * sizeof(uint32_t),
        &rx_static_handler_entry);
}

bool ble_serial_connected(void) {
    return _tx_packet_buffer.conn_handle != BLE_CONN_HANDLE_INVALID;
}

bool ble_serial_available(void) {
    return !common_hal_bleio_characteristic_buffer_deinited(&_rx_buffer) && common_hal_bleio_characteristic_buffer_rx_characters_available(&_rx_buffer);
}

char ble_serial_read_char(void) {
    if (common_hal_bleio_characteristic_buffer_deinited(&_rx_buffer)) {
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
    size_t sent = 0;
    while (sent < len) {
        uint16_t packet_size = MIN(len, (size_t)common_hal_bleio_packet_buffer_get_outgoing_packet_length(&_tx_packet_buffer));
        mp_int_t written = common_hal_bleio_packet_buffer_write(&_tx_packet_buffer, (const uint8_t *)text + sent, packet_size, NULL, 0);
        // Error, so we drop characters to transmit.
        if (written < 0) {
            break;
        }
        sent += written;
    }
}
