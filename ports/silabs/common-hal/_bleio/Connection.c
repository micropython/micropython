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
#include "py/gc.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/qstr.h"
#include "py/runtime.h"

#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Attribute.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-module/_bleio/Characteristic.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/shared/tick.h"
#include "supervisor/serial.h"

// Give 10 seconds for discovery
#define DISCOVERY_TIMEOUT_MS 10000
EventGroupHandle_t xdiscovery_event;

// Get the remote peer status
bool common_hal_bleio_connection_get_paired(bleio_connection_obj_t *self) {
    if (self->connection == NULL) {
        return false;
    }

    return self->connection->pair_status == PAIR_PAIRED;
}

// Get connected status
bool common_hal_bleio_connection_get_connected(bleio_connection_obj_t *self) {
    if (self->connection == NULL) {
        return false;
    }
    return self->connection->conn_handle != BLEIO_HANDLE_INVALID;
}

// Disconnects from the remote peripheral
void common_hal_bleio_connection_disconnect(bleio_connection_internal_t *self) {
    sl_bt_connection_close(self->conn_handle);
}

// Pair to the peer to improve security
void common_hal_bleio_connection_pair(bleio_connection_internal_t *self,
    bool bond) {

    while (self->pair_status == PAIR_WAITING && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    if (self->pair_status == PAIR_PAIRED) {
        return;
    }
    self->pair_status = PAIR_WAITING;

    while (self->pair_status == PAIR_WAITING && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    if (mp_hal_is_interrupted()) {
        return;
    }
}

// Get time between transmissions in milliseconds
mp_float_t common_hal_bleio_connection_get_connection_interval(
    bleio_connection_internal_t *self) {
    // TODO: Implement this.
    return 0;
}

// The maximum number of data bytes that can be sent in a single transmission
mp_int_t common_hal_bleio_connection_get_max_packet_length(
    bleio_connection_internal_t *self) {

    sl_status_t sc = sl_bt_gatt_server_get_mtu(self->conn_handle,&self->mtu);
    if (sc != SL_STATUS_OK) {
        mp_raise_bleio_BluetoothError(translate("gatt_server_get_mtu fail."));
    }

    return self->mtu;
}

// Set time between transmissions in milliseconds
void common_hal_bleio_connection_set_connection_interval(
    bleio_connection_internal_t *self,
    mp_float_t new_interval) {
    self->conn_params_updating = true;
    // TODO: Implement this.
}

// Do BLE discovery for all services
mp_obj_tuple_t *common_hal_bleio_connection_discover_remote_services(
    bleio_connection_obj_t *self,
    mp_obj_t service_uuids_whitelist) {

    EventBits_t ux_bits;
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable;
    mp_obj_t uuid_obj;
    mp_obj_tuple_t *services_tuple;
    sl_status_t sc = SL_STATUS_FAIL;
    bleio_uuid_obj_t *uuid;
    uint8_t uuid16_value[2];

    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t current_ticks = start_ticks;

    xdiscovery_event = xEventGroupCreate();
    if (xdiscovery_event != NULL) {
        xEventGroupClearBits(xdiscovery_event,1 << 0);
    }
    self->connection->remote_service_list = mp_obj_new_list(0, NULL);
    bleio_connection_ensure_connected(self);
    if (NULL == self->connection->remote_service_list) {
        mp_raise_bleio_BluetoothError(
            translate("Create new remote service list fail."));
        return mp_const_none;
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);

    if (service_uuids_whitelist == mp_const_none) {

        sc = sl_bt_gatt_discover_primary_services(self->connection->conn_handle);
        if (SL_STATUS_OK != sc) {
            mp_raise_bleio_BluetoothError(translate("Discover uuid fail."));
            return mp_const_none;
        }

        while (current_ticks - start_ticks <= DISCOVERY_TIMEOUT_MS) {

            if (xdiscovery_event != NULL) {
                ux_bits = xEventGroupWaitBits(
                    xdiscovery_event,
                    1 << 0,
                        pdTRUE,pdFALSE,
                        DISCOVERY_TIMEOUT_MS / portTICK_PERIOD_MS);

                if ((ux_bits & (1 << 0)) == (1 << 0)) {
                    break;
                }
            }
            current_ticks = supervisor_ticks_ms64();
            // Allow user to break out of a timeout with a KeyboardInterrupt.
            if (mp_hal_is_interrupted()) {
                break;
            }
        }

    } else {

        iterable = mp_getiter(service_uuids_whitelist, &iter_buf);
        while ((uuid_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if (!mp_obj_is_type(uuid_obj, &bleio_uuid_type)) {
                mp_raise_TypeError(
                    translate("non-UUID found in service_uuids_whitelist"));
            }
            uuid = MP_OBJ_TO_PTR(uuid_obj);

            if (BLE_UUID_TYPE_16 == uuid->efr_ble_uuid.uuid.type) {
                uuid16_value[0] = uuid->efr_ble_uuid.uuid16.value & 0xff;
                uuid16_value[1] = uuid->efr_ble_uuid.uuid16.value >> 8;
                sc = sl_bt_gatt_discover_primary_services_by_uuid(
                    self->connection->conn_handle,2,uuid16_value);

            } else if (BLE_UUID_TYPE_128 == uuid->efr_ble_uuid.uuid.type) {
                sc = sl_bt_gatt_discover_primary_services_by_uuid(
                    self->connection->conn_handle,
                    16,
                    uuid->efr_ble_uuid.uuid128.value);
            }

            if (SL_STATUS_OK != sc) {
                mp_raise_bleio_BluetoothError(translate("Discover fail."));
                return mp_const_none;
            }

            while (current_ticks - start_ticks <= DISCOVERY_TIMEOUT_MS) {

                if (xdiscovery_event != NULL) {
                    ux_bits = xEventGroupWaitBits(
                        xdiscovery_event,
                        1 << 0,
                            pdTRUE,
                            pdFALSE,
                            DISCOVERY_TIMEOUT_MS / portTICK_PERIOD_MS);

                    if ((ux_bits & (1 << 0)) == (1 << 0)) {
                        break;
                    }
                }
                current_ticks = supervisor_ticks_ms64();
                if (mp_hal_is_interrupted()) {
                    break;
                }
            }
        }
    }

    vEventGroupDelete(xdiscovery_event);
    services_tuple = mp_obj_new_tuple(self->connection->remote_service_list->len,
        self->connection->remote_service_list->items);

    if (NULL == services_tuple) {
        mp_raise_ValueError(translate("Create new service tuple fail."));
        return mp_const_none;
    }
    return services_tuple;
}

// Get connection handle
uint16_t bleio_connection_get_conn_handle(bleio_connection_obj_t *self) {
    if (self == NULL || self->connection == NULL) {
        return BLEIO_HANDLE_INVALID;
    }
    return self->connection->conn_handle;
}

mp_obj_t bleio_connection_new_from_internal(
    bleio_connection_internal_t *internal) {

    bleio_connection_obj_t *connection;
    if (internal->connection_obj != mp_const_none) {
        return internal->connection_obj;
    }
    connection = m_new_obj(bleio_connection_obj_t);
    connection->base.type = &bleio_connection_type;
    connection->connection = internal;
    internal->connection_obj = connection;

    return MP_OBJ_FROM_PTR(connection);
}

// Get internal connection object by handle connection
bleio_connection_internal_t *bleio_conn_handle_to_connection(
    uint16_t conn_handle) {
    bleio_connection_internal_t *connection;
    uint8_t conn_index;
    for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
        connection = &bleio_connections[conn_index];
        if (connection->conn_handle == conn_handle) {
            return connection;
        }
    }
    return NULL;
}
