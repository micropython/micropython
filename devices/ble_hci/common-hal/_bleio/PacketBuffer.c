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

#include "lib/utils/interrupt_char.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "supervisor/shared/tick.h"

STATIC void write_to_ringbuf(bleio_packet_buffer_obj_t *self, uint8_t *data, uint16_t len) {
    if (len + sizeof(uint16_t) > ringbuf_capacity(&self->ringbuf)) {
        // This shouldn't happen.
        return;
    }
    // Push all the data onto the ring buffer.
    // Make room for the new value by dropping the oldest packets first.
    while (ringbuf_capacity(&self->ringbuf) - ringbuf_num_filled(&self->ringbuf) < len + sizeof(uint16_t)) {
        uint16_t packet_length;
        ringbuf_get_n(&self->ringbuf, (uint8_t *)&packet_length, sizeof(uint16_t));
        for (uint16_t i = 0; i < packet_length; i++) {
            ringbuf_get(&self->ringbuf);
        }
        // set an overflow flag?
    }
    ringbuf_put_n(&self->ringbuf, (uint8_t *)&len, sizeof(uint16_t));
    ringbuf_put_n(&self->ringbuf, data, len);
}

STATIC uint32_t queue_next_write(bleio_packet_buffer_obj_t *self) {
    // Queue up the next outgoing buffer. We use two, one that has been passed to the SD for
    // transmission (when packet_queued is true) and the other is `pending` and can still be
    // modified. By primarily appending to the `pending` buffer we can reduce the protocol overhead
    // of the lower level link and ATT layers.
    self->packet_queued = false;
    if (self->pending_size > 0) {
        mp_buffer_info_t bufinfo = {
            .buf = self->outgoing[self->pending_index],
            .len = self->pending_size,
        };
        common_hal_bleio_characteristic_set_value(self->characteristic, &bufinfo);

        self->pending_size = 0;
        self->pending_index = (self->pending_index + 1) % 2;
        self->packet_queued = true;
    }
    return 0;
}

void bleio_packet_buffer_update(bleio_packet_buffer_obj_t *self, mp_buffer_info_t *bufinfo) {
    write_to_ringbuf(self, bufinfo->buf, bufinfo->len);
}

void common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    size_t buffer_size, size_t max_packet_size) {

    self->characteristic = characteristic;
    self->client = self->characteristic->service->is_remote;
    bleio_characteristic_properties_t incoming =
        self->characteristic->props & (CHAR_PROP_WRITE_NO_RESPONSE | CHAR_PROP_WRITE);
    bleio_characteristic_properties_t outgoing =
        self->characteristic->props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE);

    if (self->client) {
        // Swap if we're the client.
        bleio_characteristic_properties_t temp = incoming;
        incoming = outgoing;
        outgoing = temp;
        self->conn_handle = bleio_connection_get_conn_handle(MP_OBJ_TO_PTR(self->characteristic->service->connection));
    } else {
        self->conn_handle = BLE_CONN_HANDLE_INVALID;
    }

    if (incoming) {
        if (!ringbuf_alloc(&self->ringbuf, buffer_size * (sizeof(uint16_t) + max_packet_size), false)) {
            mp_raise_ValueError(translate("Buffer too large and unable to allocate"));
        }
    }

    if (outgoing) {
        self->packet_queued = false;
        self->pending_index = 0;
        self->pending_size = 0;
        self->outgoing[0] = m_malloc(max_packet_size, false);
        self->outgoing[1] = m_malloc(max_packet_size, false);
    } else {
        self->outgoing[0] = NULL;
        self->outgoing[1] = NULL;
    }
    self->max_packet_size = max_packet_size;

    bleio_characteristic_set_observer(self->characteristic, self);
}

mp_int_t common_hal_bleio_packet_buffer_readinto(bleio_packet_buffer_obj_t *self, uint8_t *data, size_t len) {
    if (ringbuf_num_filled(&self->ringbuf) < 2) {
        return 0;
    }

    // Copy received data.
    // Get packet length, which is in first two bytes of packet.
    uint16_t packet_length;
    ringbuf_get_n(&self->ringbuf, (uint8_t *)&packet_length, sizeof(uint16_t));

    mp_int_t ret;
    if (packet_length > len) {
        // Packet is longer than requested. Return negative of overrun value.
        ret = len - packet_length;
        // Discard the packet if it's too large. Don't fill data.
        while (packet_length--) {
            (void)ringbuf_get(&self->ringbuf);
        }
    } else {
        // Read as much as possible, but might be shorter than len.
        ringbuf_get_n(&self->ringbuf, data, packet_length);
        ret = packet_length;
    }

    return ret;
}

mp_int_t common_hal_bleio_packet_buffer_write(bleio_packet_buffer_obj_t *self,
    const uint8_t *data, size_t len, uint8_t *header, size_t header_len) {
    if (self->outgoing[0] == NULL) {
        mp_raise_bleio_BluetoothError(translate("Writes not supported on Characteristic"));
    }
    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return -1;
    }
    uint16_t outgoing_packet_length = common_hal_bleio_packet_buffer_get_outgoing_packet_length(self);

    if (len + header_len > outgoing_packet_length) {
        // Supplied data will not fit in a single BLE packet.
        mp_raise_ValueError(translate("Total data to write is larger than outgoing_packet_length"));
    }

    if (len + self->pending_size > outgoing_packet_length) {
        // No room to append len bytes to packet. Wait until we get a free buffer,
        // and keep checking that we haven't been disconnected.
        while (self->pending_size != 0 && self->conn_handle != BLE_CONN_HANDLE_INVALID) {
            RUN_BACKGROUND_TASKS;
        }
    }
    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return -1;
    }

    size_t num_bytes_written = 0;

    uint8_t *pending = self->outgoing[self->pending_index];

    if (self->pending_size == 0) {
        memcpy(pending, header, header_len);
        self->pending_size += header_len;
        num_bytes_written += header_len;
    }
    memcpy(pending + self->pending_size, data, len);
    self->pending_size += len;
    num_bytes_written += len;

    // If no writes are queued then sneak in this data.
    if (!self->packet_queued) {
        queue_next_write(self);
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
        if (self->conn_handle != BLE_CONN_HANDLE_INVALID) {
            bleio_connection_internal_t *connection = bleio_conn_handle_to_connection(self->conn_handle);
            if (connection) {
                return MIN(common_hal_bleio_connection_get_max_packet_length(connection),
                    self->characteristic->max_length);
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
        if (self->conn_handle != BLE_CONN_HANDLE_INVALID) {
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
    return MIN(self->characteristic->max_length, self->max_packet_size);
}

bool common_hal_bleio_packet_buffer_deinited(bleio_packet_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_packet_buffer_deinit(bleio_packet_buffer_obj_t *self) {
    if (!common_hal_bleio_packet_buffer_deinited(self)) {
        bleio_characteristic_clear_observer(self->characteristic);
    }
}
