/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Scott Shawcroft for Adafruit Industries
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
#include <stdio.h>

#include "shared/runtime/interrupt_char.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "supervisor/shared/tick.h"

#include "supervisor/shared/bluetooth/serial.h"

#include "host/ble_att.h"

void _common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    uint32_t *incoming_buffer, size_t incoming_buffer_size,
    uint32_t *outgoing_buffer1, uint32_t *outgoing_buffer2, size_t max_packet_size,
    void *static_handler_entry) {

    mp_raise_NotImplementedError(NULL);
    self->characteristic = characteristic;
    self->client = self->characteristic->service->is_remote;
    self->max_packet_size = max_packet_size;
    bleio_characteristic_properties_t incoming = self->characteristic->props & (CHAR_PROP_WRITE_NO_RESPONSE | CHAR_PROP_WRITE);
    bleio_characteristic_properties_t outgoing = self->characteristic->props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE);

    if (self->client) {
        // Swap if we're the client.
        bleio_characteristic_properties_t temp = incoming;
        incoming = outgoing;
        outgoing = temp;
        self->conn_handle = bleio_connection_get_conn_handle(MP_OBJ_TO_PTR(self->characteristic->service->connection));
    } else {
        self->conn_handle = BLEIO_HANDLE_INVALID;
    }

    if (incoming) {
        self->ringbuf.buf = (uint8_t *)incoming_buffer;
        self->ringbuf.size = incoming_buffer_size;
        self->ringbuf.iget = 0;
        self->ringbuf.iput = 0;
    }

    self->packet_queued = false;
    self->pending_index = 0;
    self->pending_size = 0;
    self->outgoing[0] = outgoing_buffer1;
    self->outgoing[1] = outgoing_buffer2;

}

void common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    size_t buffer_size, size_t max_packet_size) {

    // Cap the packet size to our implementation limits.
    max_packet_size = MIN(max_packet_size, BLE_ATT_ATTR_MAX_LEN - 3);

    bleio_characteristic_properties_t incoming = characteristic->props & (CHAR_PROP_WRITE_NO_RESPONSE | CHAR_PROP_WRITE);
    bleio_characteristic_properties_t outgoing = characteristic->props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE);
    if (characteristic->service->is_remote) {
        // Swap if we're the client.
        bleio_characteristic_properties_t temp = incoming;
        incoming = outgoing;
        outgoing = temp;
    }
    size_t incoming_buffer_size = 0;
    uint32_t *incoming_buffer = NULL;
    if (incoming) {
        incoming_buffer_size = buffer_size * (sizeof(uint16_t) + max_packet_size);
        incoming_buffer = m_malloc(incoming_buffer_size, false);
    }

    uint32_t *outgoing1 = NULL;
    uint32_t *outgoing2 = NULL;
    if (outgoing) {
        outgoing1 = m_malloc(max_packet_size, false);
        outgoing2 = m_malloc(max_packet_size, false);
    }
    _common_hal_bleio_packet_buffer_construct(self, characteristic,
        incoming_buffer, incoming_buffer_size,
        outgoing1, outgoing2, max_packet_size,
        NULL);
}

mp_int_t common_hal_bleio_packet_buffer_readinto(bleio_packet_buffer_obj_t *self, uint8_t *data, size_t len) {
    if (ringbuf_num_filled(&self->ringbuf) < 2) {
        return 0;
    }

    // Copy received data. Lock out write interrupt handler while copying.
    // TODO: Implement this.
    return 0;
}

mp_int_t common_hal_bleio_packet_buffer_write(bleio_packet_buffer_obj_t *self, const uint8_t *data, size_t len, uint8_t *header, size_t header_len) {
    if (self->outgoing[0] == NULL) {
        mp_raise_bleio_BluetoothError(translate("Writes not supported on Characteristic"));
    }
    if (self->conn_handle == BLEIO_HANDLE_INVALID) {
        return -1;
    }
    mp_int_t outgoing_packet_length = common_hal_bleio_packet_buffer_get_outgoing_packet_length(self);
    if (outgoing_packet_length < 0) {
        return -1;
    }

    mp_int_t total_len = len + header_len;
    if (total_len > outgoing_packet_length) {
        // Supplied data will not fit in a single BLE packet.
        mp_raise_ValueError_varg(translate("Total data to write is larger than %q"), MP_QSTR_outgoing_packet_length);
    }
    if (total_len > self->max_packet_size) {
        // Supplied data will not fit in a single BLE packet.
        mp_raise_ValueError_varg(translate("Total data to write is larger than %q"), MP_QSTR_max_packet_size);
    }
    outgoing_packet_length = MIN(outgoing_packet_length, self->max_packet_size);

    if (len + self->pending_size > (size_t)outgoing_packet_length) {
        // No room to append len bytes to packet. Wait until we get a free buffer,
        // and keep checking that we haven't been disconnected.
        while (self->pending_size != 0 &&
               self->conn_handle != BLEIO_HANDLE_INVALID &&
               !mp_hal_is_interrupted()) {
            RUN_BACKGROUND_TASKS;
        }
    }
    if (self->conn_handle == BLEIO_HANDLE_INVALID ||
        mp_hal_is_interrupted()) {
        return -1;
    }

    size_t num_bytes_written = 0;

    uint32_t *pending = self->outgoing[self->pending_index];

    if (self->pending_size == 0) {
        memcpy(pending, header, header_len);
        self->pending_size += header_len;
        num_bytes_written += header_len;
    }
    memcpy(((uint8_t *)pending) + self->pending_size, data, len);
    self->pending_size += len;
    num_bytes_written += len;

    // TODO: Implement this.

    // If no writes are queued then sneak in this data.
    if (!self->packet_queued) {
    }
    return num_bytes_written;
}

mp_int_t common_hal_bleio_packet_buffer_get_incoming_packet_length(bleio_packet_buffer_obj_t *self) {
    // If this PacketBuffer is coming from a remote service via NOTIFY or INDICATE
    // the maximum size is what can be sent in one
    // BLE packet. But we must be connected to know that value.
    //
    // Otherwise it can be as long as the characteristic
    // will permit, whether or not we're connected.

    if (self->characteristic == NULL) {
        return -1;
    }

    if (self->characteristic->service != NULL &&
        self->characteristic->service->is_remote &&
        (common_hal_bleio_characteristic_get_properties(self->characteristic) &
         (CHAR_PROP_INDICATE | CHAR_PROP_NOTIFY))) {
        // We are talking to a remote service, and data is arriving via NOTIFY or INDICATE.
        if (self->conn_handle != BLEIO_HANDLE_INVALID) {
            bleio_connection_internal_t *connection = bleio_conn_handle_to_connection(self->conn_handle);
            if (connection) {
                return common_hal_bleio_connection_get_max_packet_length(connection);
            }
        }
        // There's no current connection, so we don't know the MTU, and
        // we can't tell what the largest incoming packet length would be.
        return -1;
    }
    return self->characteristic->max_length;
}

mp_int_t common_hal_bleio_packet_buffer_get_outgoing_packet_length(bleio_packet_buffer_obj_t *self) {
    // If we are sending data via NOTIFY or INDICATE, the maximum size
    // is what can be sent in one BLE packet. But we must be connected
    // to know that value.
    //
    // Otherwise it can be as long as the characteristic
    // will permit, whether or not we're connected.

    if (self->characteristic == NULL) {
        return -1;
    }

    if (self->characteristic->service != NULL &&
        !self->characteristic->service->is_remote &&
        (common_hal_bleio_characteristic_get_properties(self->characteristic) &
         (CHAR_PROP_INDICATE | CHAR_PROP_NOTIFY))) {
        // We are sending to a client, via NOTIFY or INDICATE.
        if (self->conn_handle != BLEIO_HANDLE_INVALID) {
            bleio_connection_internal_t *connection = bleio_conn_handle_to_connection(self->conn_handle);
            if (connection) {
                return MIN(MIN(common_hal_bleio_connection_get_max_packet_length(connection),
                    self->max_packet_size),
                    self->characteristic->max_length);
            }
        }
        // There's no current connection, so we don't know the MTU, and
        // we can't tell what the largest outgoing packet length would be.
        return -1;
    }
    // If we are talking to a remote service, we'll be bound by the MTU. (We don't actually
    // know the max size of the remote characteristic.)
    if (self->characteristic->service != NULL &&
        self->characteristic->service->is_remote) {
        // We are talking to a remote service so we're writing.
        if (self->conn_handle != BLEIO_HANDLE_INVALID) {
            bleio_connection_internal_t *connection = bleio_conn_handle_to_connection(self->conn_handle);
            if (connection) {
                return MIN(common_hal_bleio_connection_get_max_packet_length(connection),
                    self->max_packet_size);
            }
        }
    }
    return MIN(self->characteristic->max_length, self->max_packet_size);
}

void common_hal_bleio_packet_buffer_flush(bleio_packet_buffer_obj_t *self) {
    while ((self->pending_size != 0 ||
            self->packet_queued) &&
           self->conn_handle != BLEIO_HANDLE_INVALID &&
           !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
}

bool common_hal_bleio_packet_buffer_deinited(bleio_packet_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_packet_buffer_deinit(bleio_packet_buffer_obj_t *self) {
    if (!common_hal_bleio_packet_buffer_deinited(self)) {
    }
    // TODO: Implement this.
}
