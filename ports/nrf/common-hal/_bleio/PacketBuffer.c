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
    if (len + sizeof(uint16_t) > self->ringbuf.size) {
        // This shouldn't happen.
        return;
    }
    // Push all the data onto the ring buffer.
    uint8_t is_nested_critical_region;
    sd_nvic_critical_region_enter(&is_nested_critical_region);
    // Make room for the new value by dropping the oldest packets first.
    while (self->ringbuf.size - ringbuf_count(&self->ringbuf) < (int) (len + sizeof(uint16_t))) {
        uint16_t packet_length;
        ringbuf_get_n(&self->ringbuf, (uint8_t*) &packet_length, sizeof(uint16_t));
        for (uint16_t i = 0; i < packet_length; i++) {
            ringbuf_get(&self->ringbuf);
        }
        // set an overflow flag?
    }
    ringbuf_put_n(&self->ringbuf, (uint8_t*) &len, sizeof(uint16_t));
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
                .p_value = self->outgoing[self->pending_index],
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
                .p_data = self->outgoing[self->pending_index],
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
    bleio_packet_buffer_obj_t *self = (bleio_packet_buffer_obj_t *) param;
    uint16_t conn_handle = ble_evt->evt.gattc_evt.conn_handle;
    if (conn_handle != self->conn_handle) {
        return false;
    }
    switch (ble_evt->header.evt_id) {
        case BLE_GATTC_EVT_HVX: {
            // A remote service wrote to this characteristic.
            ble_gattc_evt_hvx_t* evt_hvx = &ble_evt->evt.gattc_evt.params.hvx;
            // Must be a notification, and event handle must match the handle for my characteristic.
            if (evt_hvx->handle == self->characteristic->handle) {
                write_to_ringbuf(self, evt_hvx->data, evt_hvx->len);
                if (evt_hvx->type == BLE_GATT_HVX_INDICATION) {
                    sd_ble_gattc_hv_confirm(conn_handle, evt_hvx->handle);
                }
            }
            break;
        }
        case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE: {
            queue_next_write(self);
            break;
        }
        case BLE_GATTC_EVT_WRITE_RSP: {
            queue_next_write(self);
            break;
        }
        default:
            return false;
            break;
    }
    return true;
}

STATIC bool packet_buffer_on_ble_server_evt(ble_evt_t *ble_evt, void *param) {
    bleio_packet_buffer_obj_t *self = (bleio_packet_buffer_obj_t *) param;
    uint16_t conn_handle = ble_evt->evt.gatts_evt.conn_handle;
    switch (ble_evt->header.evt_id) {
        case BLE_GATTS_EVT_WRITE: {
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
                uint16_t cccd = *((uint16_t*) evt_write->data);
                if (cccd & BLE_GATT_HVX_NOTIFICATION) {
                    self->conn_handle = conn_handle;
                } else {
                    self->conn_handle = BLE_CONN_HANDLE_INVALID;
                }
            }
            break;
        }
        case BLE_GAP_EVT_DISCONNECTED: {
            if (self->conn_handle == conn_handle) {
                self->conn_handle = BLE_CONN_HANDLE_INVALID;
            }
        }
        case BLE_GATTS_EVT_HVN_TX_COMPLETE: {
            queue_next_write(self);
        }
        default:
            return false;
            break;
    }
    return true;
}

void common_hal_bleio_packet_buffer_construct(
        bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
        size_t buffer_size) {

    self->characteristic = characteristic;
    self->client = self->characteristic->service->is_remote;
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
        // This is a macro.
        ringbuf_alloc(&self->ringbuf, buffer_size * (sizeof(uint16_t) + characteristic->max_length), false);

        if (self->ringbuf.buf == NULL) {
            mp_raise_ValueError(translate("Buffer too large and unable to allocate"));
        }
    }

    if (outgoing) {
        self->packet_queued = false;
        self->pending_index = 0;
        self->pending_size = 0;
        self->outgoing[0] = m_malloc(characteristic->max_length, false);
        self->outgoing[1] = m_malloc(characteristic->max_length, false);
    } else {
        self->outgoing[0] = NULL;
        self->outgoing[1] = NULL;
    }

    if (self->client) {
        ble_drv_add_event_handler(packet_buffer_on_ble_client_evt, self);
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
        ble_drv_add_event_handler(packet_buffer_on_ble_server_evt, self);
        if (outgoing) {
            self->write_type = BLE_GATT_HVX_INDICATION;
            if (outgoing & CHAR_PROP_NOTIFY) {
                self->write_type = BLE_GATT_HVX_NOTIFICATION;
            }
        }
    }
}

int common_hal_bleio_packet_buffer_readinto(bleio_packet_buffer_obj_t *self, uint8_t *data, size_t len) {
    if (ringbuf_count(&self->ringbuf) < 2) {
        return 0;
    }

    uint16_t packet_length;
    ringbuf_get_n(&self->ringbuf, (uint8_t*) &packet_length, sizeof(uint16_t));

    // Copy received data. Lock out write interrupt handler while copying.
    uint8_t is_nested_critical_region;
    sd_nvic_critical_region_enter(&is_nested_critical_region);

    if (packet_length > len) {
        return len - packet_length;
    }

    ringbuf_get_n(&self->ringbuf, data, packet_length);

    // Writes now OK.
    sd_nvic_critical_region_exit(is_nested_critical_region);

    return packet_length;
}

void common_hal_bleio_packet_buffer_write(bleio_packet_buffer_obj_t *self, uint8_t *data, size_t len, uint8_t* header, size_t header_len) {
    if (self->outgoing[0] == NULL) {
        mp_raise_bleio_BluetoothError(translate("Writes not supported on Characteristic"));
    }
    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return;
    }
    uint16_t packet_size = common_hal_bleio_packet_buffer_get_packet_size(self);
    uint16_t max_size = packet_size - len;
    while (max_size < self->pending_size && self->conn_handle != BLE_CONN_HANDLE_INVALID) {
        RUN_BACKGROUND_TASKS;
    }
    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return;
    }
    uint8_t is_nested_critical_region;
    sd_nvic_critical_region_enter(&is_nested_critical_region);

    uint8_t* pending = self->outgoing[self->pending_index];

    if (self->pending_size == 0) {
        memcpy(pending, header, header_len);
        self->pending_size += header_len;
    }
    memcpy(pending + self->pending_size, data, len);
    self->pending_size += len;

    sd_nvic_critical_region_exit(is_nested_critical_region);

    // If no writes are queued then sneak in this data.
    if (!self->packet_queued) {
        queue_next_write(self);
    }
}

uint16_t common_hal_bleio_packet_buffer_get_packet_size(bleio_packet_buffer_obj_t *self) {
    uint16_t mtu;
    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return 0;
    }
    bleio_connection_internal_t *connection;
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        connection = &bleio_connections[i];
        if (connection->conn_handle == self->conn_handle) {
            break;
        }
    }
    if (connection->mtu == 0) {
        mtu = BLE_GATT_ATT_MTU_DEFAULT;
    }
    if (self->characteristic->max_length > mtu) {
        mtu = self->characteristic->max_length;
    }
    uint16_t att_overhead = 3;
    return mtu - att_overhead;
}

bool common_hal_bleio_packet_buffer_deinited(bleio_packet_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_packet_buffer_deinit(bleio_packet_buffer_obj_t *self) {
    if (!common_hal_bleio_packet_buffer_deinited(self)) {
        ble_drv_remove_event_handler(packet_buffer_on_ble_client_evt, self);
    }
}
