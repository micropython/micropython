/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include <stdio.h>
#include <string.h>
#include "py/runtime.h"
#include "py/stream.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared/runtime/interrupt_char.h"
#include "common-hal/_bleio/Connection.h"
#include "supervisor/shared/serial.h"
#include "supervisor/shared/tick.h"

// List packet buffer of peripheral device
bleio_packet_buffer_obj_list_t bleio_packet_buffer_list;

// Write data to ringbuf of packet buffer
STATIC void write_to_ringbuf(bleio_packet_buffer_obj_t *self,
    uint8_t *data,
    uint16_t len) {

    uint16_t packet_length;
    uint16_t packet_index;
    if (len + sizeof(uint16_t) > ringbuf_size(&self->ringbuf)) {
        // This shouldn't happen but can if our buffer size was much smaller than
        // the writes the client actually makes.
        return;
    }

    taskENTER_CRITICAL();
    // Push all the data onto the ring buffer.
    // Make room for the new value by dropping the oldest packets first.
    while (ringbuf_size(&self->ringbuf) - ringbuf_num_filled(&self->ringbuf) < len + sizeof(uint16_t)) {
        ringbuf_get_n(&self->ringbuf,
            (uint8_t *)&packet_length, sizeof(uint16_t));

        for (packet_index = 0; packet_index < packet_length; packet_index++) {
            ringbuf_get(&self->ringbuf);
        }
    }
    ringbuf_put_n(&self->ringbuf, (uint8_t *)&len, sizeof(uint16_t));
    ringbuf_put_n(&self->ringbuf, data, len);
    taskEXIT_CRITICAL();
}

// Write characteristic or attribute value
STATIC int queue_next_write(bleio_packet_buffer_obj_t *self) {
    self->packet_queued = false;

    uint32_t sc = SL_STATUS_OK;
    if (self->pending_size > 0) {
        if (self->client) {
            if (self->write_type & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
                uint16_t sent_len;
                sc = sl_bt_gatt_write_characteristic_value_without_response(
                    self->conn_handle, self->characteristic->handle,
                    self->pending_size,
                    (uint8_t *)self->outgoing[self->pending_index],
                    &sent_len);
            } else {
                sc = sl_bt_gatt_write_characteristic_value(
                    self->conn_handle, self->characteristic->handle,
                    self->pending_size,
                    (uint8_t *)self->outgoing[self->pending_index]);
            }

        } else {
            if (self->write_type & BT_GATT_CHRC_READ) {
                sc = sl_bt_gatt_server_write_attribute_value(self->characteristic->handle,
                    0,
                    self->pending_size,
                    (uint8_t *)self->outgoing[self->pending_index]);
            }

            if (self->write_type & BT_GATT_CHRC_NOTIFY) {
                sc = sl_bt_gatt_server_send_notification(
                    self->conn_handle,
                    self->characteristic->handle,
                    self->pending_size,
                    (uint8_t *)self->outgoing[self->pending_index]);
            }

            if (self->write_type & BT_GATT_CHRC_INDICATE) {
                sl_bt_gatt_server_send_indication(
                    self->conn_handle,
                    self->characteristic->handle,
                    self->pending_size,
                    (uint8_t *)self->outgoing[self->pending_index]);
            }
        }
        self->pending_size = 0;
        self->pending_index = (self->pending_index + 1) % 2;
        self->packet_queued = true;
    }
    return sc;
}


// This funttion is called in sl_bt_on_event to receive
bool packet_buffer_on_ble_evt(uint16_t attribute, uint8_t *data, uint16_t len) {
    uint16_t cindex = 0;
    for (cindex = 0; cindex < bleio_packet_buffer_list.len; cindex++) {
        if (bleio_packet_buffer_list.data[cindex]->characteristic->handle == attribute) {
            taskENTER_CRITICAL();
            write_to_ringbuf(bleio_packet_buffer_list.data[cindex], data, len);
            taskEXIT_CRITICAL();

            return true;
        }
    }
    return false;
}

void _common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    uint32_t *incoming_buffer,
    size_t incoming_buffer_size,
    uint32_t *outgoing_buffer1,
    uint32_t *outgoing_buffer2,
    size_t max_packet_size,
    ble_event_handler_t static_handler_entry) {

    bleio_characteristic_properties_t temp_prop;
    self->characteristic = characteristic;
    self->client = self->characteristic->service->is_remote;
    self->max_packet_size = max_packet_size;
    bleio_characteristic_properties_t incoming = self->characteristic->props & (BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_WRITE);
    bleio_characteristic_properties_t outgoing = self->characteristic->props & (BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_INDICATE);

    if (self->client) {
        // Swap if we're the client.
        temp_prop = incoming;
        incoming = outgoing;
        outgoing = temp_prop;
        self->conn_handle = bleio_connection_get_conn_handle(
            MP_OBJ_TO_PTR(self->characteristic->service->connection));
    } else {
        self->conn_handle = BLE_CONN_HANDLE_INVALID;
    }

    if (incoming) {
        ringbuf_init(&self->ringbuf,
            (uint8_t *)incoming_buffer,
            incoming_buffer_size);
    }

    self->packet_queued = false;
    self->pending_index = 0;
    self->pending_size = 0;
    self->outgoing[0] = outgoing_buffer1;
    self->outgoing[1] = outgoing_buffer2;

    if (self->client) {
        if (incoming) {
            if (incoming & BT_GATT_CHRC_NOTIFY) {
                common_hal_bleio_characteristic_set_cccd(self->characteristic,
                    true, false);
            } else {
                common_hal_bleio_characteristic_set_cccd(self->characteristic,
                    false, true);
            }
        }
        if (outgoing) {
            self->write_type = BT_GATT_CHRC_WRITE;
            if (outgoing & BT_GATT_CHRC_WRITE_WITHOUT_RESP) {
                self->write_type = BT_GATT_CHRC_WRITE_WITHOUT_RESP;
            }
        }
    } else {
        self->write_type = outgoing;
    }
}

// Init packet buffer
void common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    size_t buffer_size, size_t max_packet_size) {

    size_t incoming_buffer_size = 0;
    uint32_t *incoming_buffer = NULL;
    uint32_t *outgoing1 = NULL;
    uint32_t *outgoing2 = NULL;

    bleio_characteristic_properties_t temp_properties;
    // Cap the packet size to our implementation limits.
    max_packet_size = MIN(max_packet_size, BLE_GATTS_VAR_ATTR_LEN_MAX - 3);

    bleio_characteristic_properties_t incoming = characteristic->props & (BT_GATT_CHRC_WRITE_WITHOUT_RESP | BT_GATT_CHRC_WRITE);
    bleio_characteristic_properties_t outgoing = characteristic->props & (BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_INDICATE);
    if (characteristic->service->is_remote) {
        // Swap if we're the client.
        temp_properties = incoming;
        incoming = outgoing;
        outgoing = temp_properties;
    }

    if (incoming) {
        incoming_buffer_size = buffer_size * (sizeof(uint16_t) + max_packet_size);
        incoming_buffer = m_malloc(incoming_buffer_size);
    }

    if (outgoing) {
        outgoing1 = m_malloc(max_packet_size);
        outgoing2 = m_malloc(max_packet_size);
    }
    _common_hal_bleio_packet_buffer_construct(self, characteristic,
        incoming_buffer, incoming_buffer_size,
        outgoing1, outgoing2, max_packet_size,
        NULL);

    bleio_packet_buffer_list.data[bleio_packet_buffer_list.len] = self;
    bleio_packet_buffer_list.len++;
}

// Reads a single BLE packet into the buffer
mp_int_t common_hal_bleio_packet_buffer_readinto(
    bleio_packet_buffer_obj_t *self,
    uint8_t *data,
    size_t len) {

    mp_int_t ret;
    uint16_t packet_length;

    if (ringbuf_num_filled(&self->ringbuf) < 1) {
        return 0;
    }
    taskENTER_CRITICAL();
    // Get packet length, which is in first two bytes of packet.
    packet_length = 5;
    ringbuf_get_n(&self->ringbuf, (uint8_t *)&packet_length, sizeof(uint16_t));

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
    taskEXIT_CRITICAL();
    return ret;
}

// Writes all bytes from data into the same outgoing packet
mp_int_t common_hal_bleio_packet_buffer_write(bleio_packet_buffer_obj_t *self,
    const uint8_t *data,
    size_t len,
    uint8_t *header,
    size_t header_len) {

    mp_int_t outgoing_packet_length;
    mp_int_t total_len;
    size_t num_bytes_written;
    uint32_t *pending;

    if (!self->client) {
        self->conn_handle = bleio_connections[0].conn_handle;
    }
    if (self->outgoing[0] == NULL) {
        mp_raise_bleio_BluetoothError(
            MP_ERROR_TEXT("Writes not supported on Characteristic"));
    }
    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return -1;
    }
    outgoing_packet_length =
        common_hal_bleio_packet_buffer_get_outgoing_packet_length(self);

    if (outgoing_packet_length < 0) {
        return -1;
    }

    total_len = len + header_len;
    if (total_len > outgoing_packet_length) {
        // Supplied data will not fit in a single BLE packet.
        mp_raise_ValueError_varg(
            MP_ERROR_TEXT("Total data to write is larger than %q"),
            MP_QSTR_outgoing_packet_length);
    }
    if (total_len > self->max_packet_size) {
        // Supplied data will not fit in a single BLE packet.
        mp_raise_ValueError_varg(
            MP_ERROR_TEXT("Total data to write is larger than %q"),
            MP_QSTR_max_packet_size);
    }
    outgoing_packet_length = MIN(outgoing_packet_length, self->max_packet_size);

    if (len + self->pending_size > (size_t)outgoing_packet_length) {
        // No room to append len bytes to packet. Wait until we get a free buffer
        // and keep checking that we haven't been disconnected.
        while (self->pending_size != 0 &&
               self->conn_handle != BLE_CONN_HANDLE_INVALID &&
               !mp_hal_is_interrupted()) {
            RUN_BACKGROUND_TASKS;
        }
        if (mp_hal_is_interrupted()) {
            return -1;
        }
    }
    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return -1;
    }

    num_bytes_written = 0;
    pending = self->outgoing[self->pending_index];

    if (self->pending_size == 0) {
        memcpy(pending, header, header_len);
        self->pending_size += header_len;
        num_bytes_written += header_len;
    }
    memcpy(((uint8_t *)pending) + self->pending_size, data, len);
    self->pending_size += len;
    num_bytes_written += len;

    queue_next_write(self);
    return num_bytes_written;
}

// Get length of receiving packet
mp_int_t common_hal_bleio_packet_buffer_get_incoming_packet_length(
    bleio_packet_buffer_obj_t *self) {

    if (self->characteristic == NULL) {
        return -1;
    }
    return self->characteristic->max_length;
}

// Get length of outgoing packet
mp_int_t common_hal_bleio_packet_buffer_get_outgoing_packet_length(
    bleio_packet_buffer_obj_t *self) {

    if (self->characteristic == NULL) {
        return -1;
    }
    return MIN(self->max_packet_size, self->characteristic->max_length);
}

// Flush ring buffer og packer buffer
void common_hal_bleio_packet_buffer_flush(bleio_packet_buffer_obj_t *self) {
    while ((self->pending_size != 0 ||
            self->packet_queued) &&
           self->conn_handle != BLEIO_HANDLE_INVALID &&
           !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
}

// Check status of packet buffer obj
bool common_hal_bleio_packet_buffer_deinited(bleio_packet_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

// Deinit packet buffer
void common_hal_bleio_packet_buffer_deinit(bleio_packet_buffer_obj_t *self) {
    if (!common_hal_bleio_packet_buffer_deinited(self)) {
        ringbuf_deinit(&self->ringbuf);
    }
}

// Remove packet_buffer list when reload
void reset_packet_buffer_list() {
    // Remove packet_buffer list
    memset(bleio_packet_buffer_list.data, 0,
        sizeof(bleio_packet_buffer_list.data));
    bleio_packet_buffer_list.len = 0;
}
