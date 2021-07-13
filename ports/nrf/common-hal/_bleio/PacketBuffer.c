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

#include "ble_drv.h"
#include "ble_gatts.h"
#include "nrf_nvic.h"

#include "lib/utils/interrupt_char.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "supervisor/shared/tick.h"

STATIC void write_to_ringbuf(bleio_packet_buffer_obj_t *self, uint8_t *data, uint16_t len) {
    if (len + sizeof(uint16_t) > ringbuf_capacity(&self->ringbuf)) {
        // This shouldn't happen but can if our buffer size was much smaller than
        // the writes the client actually makes.
        return;
    }
    // Push all the data onto the ring buffer.
    uint8_t is_nested_critical_region;
    sd_nvic_critical_region_enter(&is_nested_critical_region);
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
    sd_nvic_critical_region_exit(is_nested_critical_region);
}

STATIC uint32_t queue_next_write(bleio_packet_buffer_obj_t *self) {
    // Queue up the next outgoing buffer. We use two, one that has been passed to the SD for
    // transmission (when packet_queued is true) and the other is `pending` and can still be
    // modified. By primarily appending to the `pending` buffer we can reduce the protocol overhead
    // of the lower level link and ATT layers.
    self->packet_queued = false;
    if (self->pending_size > 0) {
        uint16_t conn_handle = self->conn_handle;
        uint32_t err_code;
        if (self->client) {
            ble_gattc_write_params_t write_params = {
                .write_op = self->write_type,
                .handle = self->characteristic->handle,
                .p_value = (const uint8_t *)self->outgoing[self->pending_index],
                .len = self->pending_size,
            };

            err_code = sd_ble_gattc_write(conn_handle, &write_params);
        } else {
            uint16_t hvx_len = self->pending_size;

            ble_gatts_hvx_params_t hvx_params = {
                .handle = self->characteristic->handle,
                .type = self->write_type,
                .offset = 0,
                .p_len = &hvx_len,
                .p_data = (const uint8_t *)self->outgoing[self->pending_index],
            };
            err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
        }
        if (err_code != NRF_SUCCESS) {
            // On error, simply skip updating the pending buffers so that the next HVC or WRITE
            // complete event triggers another attempt.
            return err_code;
        }
        self->pending_size = 0;
        self->pending_index = (self->pending_index + 1) % 2;
        self->packet_queued = true;
    }
    return NRF_SUCCESS;
}

STATIC bool packet_buffer_on_ble_client_evt(ble_evt_t *ble_evt, void *param) {
    const uint16_t evt_id = ble_evt->header.evt_id;
    bleio_packet_buffer_obj_t *self = (bleio_packet_buffer_obj_t *)param;
    if (evt_id == BLE_GAP_EVT_DISCONNECTED && self->conn_handle == ble_evt->evt.gap_evt.conn_handle) {
        self->conn_handle = BLE_CONN_HANDLE_INVALID;
    }
    // Check if this is a GATTC event so we can make sure the conn_handle is valid.
    if (evt_id < BLE_GATTC_EVT_BASE || evt_id > BLE_GATTC_EVT_LAST) {
        return false;
    }

    uint16_t conn_handle = ble_evt->evt.gattc_evt.conn_handle;
    if (conn_handle != self->conn_handle) {
        return false;
    }
    switch (evt_id) {
        case BLE_GATTC_EVT_HVX: {
            // A remote service wrote to this characteristic.
            ble_gattc_evt_hvx_t *evt_hvx = &ble_evt->evt.gattc_evt.params.hvx;
            // Must be a notification, and event handle must match the handle for my characteristic.
            if (evt_hvx->handle == self->characteristic->handle) {
                write_to_ringbuf(self, evt_hvx->data, evt_hvx->len);
                if (evt_hvx->type == BLE_GATT_HVX_INDICATION) {
                    sd_ble_gattc_hv_confirm(conn_handle, evt_hvx->handle);
                }
            }
            break;
        }
        case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:
            queue_next_write(self);
            break;
        case BLE_GATTC_EVT_WRITE_RSP:
            queue_next_write(self);
            break;
        default:
            return false;
            break;
    }
    return true;
}

STATIC bool packet_buffer_on_ble_server_evt(ble_evt_t *ble_evt, void *param) {
    bleio_packet_buffer_obj_t *self = (bleio_packet_buffer_obj_t *)param;
    switch (ble_evt->header.evt_id) {
        case BLE_GATTS_EVT_WRITE: {
            uint16_t conn_handle = ble_evt->evt.gatts_evt.conn_handle;
            // A client wrote to this server characteristic.

            ble_gatts_evt_write_t *evt_write = &ble_evt->evt.gatts_evt.params.write;

            // Event handle must match the handle for my characteristic.
            if (evt_write->handle == self->characteristic->handle) {
                if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
                    self->conn_handle = conn_handle;
                } else if (self->conn_handle != conn_handle) {
                    return false;
                }
                write_to_ringbuf(self, evt_write->data, evt_write->len);
            } else if (evt_write->handle == self->characteristic->cccd_handle) {
                uint16_t cccd = *((uint16_t *)evt_write->data);
                if (cccd & BLE_GATT_HVX_NOTIFICATION) {
                    self->conn_handle = conn_handle;
                } else {
                    self->conn_handle = BLE_CONN_HANDLE_INVALID;
                }
            }
            break;
        }
        case BLE_GAP_EVT_CONN_SEC_UPDATE: { // 0x1a
            if (self->conn_handle != BLE_CONN_HANDLE_INVALID) {
                break;
            }
            uint16_t conn_handle = ble_evt->evt.gatts_evt.conn_handle;
            // Check to see if the bond restored the HVX state.
            uint16_t cccd;
            ble_gatts_value_t value;
            value.len = sizeof(uint16_t);
            value.offset = 0;
            value.p_value = (uint8_t *)&cccd;
            sd_ble_gatts_value_get(conn_handle, self->characteristic->cccd_handle, &value);
            if (cccd & BLE_GATT_HVX_NOTIFICATION) {
                self->conn_handle = conn_handle;
            }
            break;
        }
        case BLE_GAP_EVT_CONNECTED:
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            if (self->conn_handle == ble_evt->evt.gap_evt.conn_handle) {
                self->conn_handle = BLE_CONN_HANDLE_INVALID;
            }
            break;
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            queue_next_write(self);
            break;
        default:
            return false;
            break;
    }
    return true;
}


void _common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    uint32_t *incoming_buffer, size_t incoming_buffer_size,
    uint32_t *outgoing_buffer1, uint32_t *outgoing_buffer2, size_t max_packet_size,
    void *static_handler_entry) {

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
        self->conn_handle = BLE_CONN_HANDLE_INVALID;
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

    if (self->client) {
        if (static_handler_entry != NULL) {
            ble_drv_add_event_handler_entry((ble_drv_evt_handler_entry_t *)static_handler_entry, packet_buffer_on_ble_client_evt, self);
        } else {
            ble_drv_add_event_handler(packet_buffer_on_ble_client_evt, self);
        }
        if (incoming) {
            // Prefer notify if both are available.
            if (incoming & CHAR_PROP_NOTIFY) {
                self->write_type = BLE_GATT_HVX_NOTIFICATION;
                common_hal_bleio_characteristic_set_cccd(self->characteristic, true, false);
            } else {
                common_hal_bleio_characteristic_set_cccd(self->characteristic, false, true);
            }
        }
        if (outgoing) {
            self->write_type = BLE_GATT_OP_WRITE_REQ;
            if (outgoing & CHAR_PROP_WRITE_NO_RESPONSE) {
                self->write_type = BLE_GATT_OP_WRITE_CMD;
            }
        }
    } else {
        if (static_handler_entry != NULL) {
            ble_drv_add_event_handler_entry((ble_drv_evt_handler_entry_t *)static_handler_entry, packet_buffer_on_ble_server_evt, self);
        } else {
            ble_drv_add_event_handler(packet_buffer_on_ble_server_evt, self);
        }
        if (outgoing) {
            self->write_type = BLE_GATT_HVX_INDICATION;
            if (outgoing & CHAR_PROP_NOTIFY) {
                self->write_type = BLE_GATT_HVX_NOTIFICATION;
            }
        }
    }

}

void common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    size_t buffer_size, size_t max_packet_size) {

    // Cap the packet size to our implementation limits.
    max_packet_size = MIN(max_packet_size, BLE_GATTS_VAR_ATTR_LEN_MAX - 3);

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
    uint8_t is_nested_critical_region;
    sd_nvic_critical_region_enter(&is_nested_critical_region);

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

    // Writes now OK.
    sd_nvic_critical_region_exit(is_nested_critical_region);

    return ret;
}

mp_int_t common_hal_bleio_packet_buffer_write(bleio_packet_buffer_obj_t *self, const uint8_t *data, size_t len, uint8_t *header, size_t header_len) {
    if (self->outgoing[0] == NULL) {
        mp_raise_bleio_BluetoothError(translate("Writes not supported on Characteristic"));
    }
    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return -1;
    }
    uint16_t outgoing_packet_length = common_hal_bleio_packet_buffer_get_outgoing_packet_length(self);

    uint16_t total_len = len + header_len;
    if (total_len > outgoing_packet_length) {
        // Supplied data will not fit in a single BLE packet.
        mp_raise_ValueError_varg(translate("Total data to write is larger than %q"), MP_QSTR_outgoing_packet_length);
    }
    if (total_len > self->max_packet_size) {
        // Supplied data will not fit in a single BLE packet.
        mp_raise_ValueError_varg(translate("Total data to write is larger than %q"), MP_QSTR_max_packet_size);
    }
    outgoing_packet_length = MIN(outgoing_packet_length, self->max_packet_size);

    if (len + self->pending_size > outgoing_packet_length) {
        // No room to append len bytes to packet. Wait until we get a free buffer,
        // and keep checking that we haven't been disconnected.
        while (self->pending_size != 0 &&
               self->conn_handle != BLE_CONN_HANDLE_INVALID &&
               !mp_hal_is_interrupted()) {
            RUN_BACKGROUND_TASKS;
        }
    }
    if (self->conn_handle == BLE_CONN_HANDLE_INVALID ||
        mp_hal_is_interrupted()) {
        return -1;
    }

    size_t num_bytes_written = 0;

    uint8_t is_nested_critical_region;
    sd_nvic_critical_region_enter(&is_nested_critical_region);

    uint32_t *pending = self->outgoing[self->pending_index];

    if (self->pending_size == 0) {
        memcpy(pending, header, header_len);
        self->pending_size += header_len;
        num_bytes_written += header_len;
    }
    memcpy(((uint8_t *)pending) + self->pending_size, data, len);
    self->pending_size += len;
    num_bytes_written += len;

    sd_nvic_critical_region_exit(is_nested_critical_region);

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
    // If we are talking to a remote service, we'll be bound by the MTU. (We don't actually
    // know the max size of the remote characteristic.)
    if (self->characteristic->service != NULL &&
        self->characteristic->service->is_remote) {
        // We are talking to a remote service so we're writing.
        if (self->conn_handle != BLE_CONN_HANDLE_INVALID) {
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
    while (self->pending_size != 0 &&
           self->packet_queued &&
           self->conn_handle != BLE_CONN_HANDLE_INVALID &&
           !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
}

bool common_hal_bleio_packet_buffer_deinited(bleio_packet_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_packet_buffer_deinit(bleio_packet_buffer_obj_t *self) {
    if (!common_hal_bleio_packet_buffer_deinited(self)) {
        ble_drv_remove_event_handler(packet_buffer_on_ble_client_evt, self);
    }
}
