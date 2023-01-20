/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#include "shared-bindings/_bleio/Connection.h"

#include <string.h>
#include <stdio.h>

#include "py/gc.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/qstr.h"
#include "py/runtime.h"

#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Attribute.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"

#include "supervisor/shared/tick.h"

#include "common-hal/_bleio/ble_events.h"

#include "host/ble_att.h"

// Give 20 seconds for discovery
#define DISCOVERY_TIMEOUT_MS 20000

int bleio_connection_event_cb(struct ble_gap_event *event, void *connection_in) {
    bleio_connection_internal_t *connection = (bleio_connection_internal_t *)connection_in;

    switch (event->type) {
        case BLE_GAP_EVENT_DISCONNECT: {
            connection->conn_handle = BLEIO_HANDLE_INVALID;
            connection->pair_status = PAIR_NOT_PAIRED;

            #if CIRCUITPY_VERBOSE_BLE
            mp_printf(&mp_plat_print, "disconnected %02x\n", event->disconnect.reason);
            #endif
            if (connection->connection_obj != mp_const_none) {
                bleio_connection_obj_t *obj = connection->connection_obj;
                obj->connection = NULL;
                obj->disconnect_reason = event->disconnect.reason;
            }

            break;
        }

        case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE: {
            #if CIRCUITPY_VERBOSE_BLE
            mp_printf(&mp_plat_print, "TODO connection event: PHY update complete\n");
            #endif
            break;
        }

        case BLE_GAP_EVENT_CONN_UPDATE: {
            #if CIRCUITPY_VERBOSE_BLE
            mp_printf(&mp_plat_print, "TODO connection event: connection update\n");
            #endif
            break;
        }
        case BLE_GAP_EVENT_L2CAP_UPDATE_REQ: {
            #if CIRCUITPY_VERBOSE_BLE
            mp_printf(&mp_plat_print, "TODO connection event: l2cap update request\n");
            #endif
            break;
        }

        // These events are actually att specific so forward to all registered
        // handlers for them. The handlers themselves decide whether an event
        // is interesting to them.
        case BLE_GAP_EVENT_NOTIFY_RX:
            MP_FALLTHROUGH;
        case BLE_GAP_EVENT_NOTIFY_TX:
            MP_FALLTHROUGH;
        case BLE_GAP_EVENT_SUBSCRIBE:
            return ble_event_run_handlers(event);

        default:
            #if CIRCUITPY_VERBOSE_BLE
            mp_printf(&mp_plat_print, "Unhandled connection event: %d\n", event->type);
            #endif
            return 0;
    }
    return 0;
}

bool common_hal_bleio_connection_get_paired(bleio_connection_obj_t *self) {
    if (self->connection == NULL) {
        return false;
    }
    return self->connection->pair_status == PAIR_PAIRED;
}

bool common_hal_bleio_connection_get_connected(bleio_connection_obj_t *self) {
    if (self->connection == NULL) {
        return false;
    }
    return self->connection->conn_handle != BLEIO_HANDLE_INVALID;
}

void common_hal_bleio_connection_disconnect(bleio_connection_internal_t *self) {
    ble_gap_terminate(self->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

void common_hal_bleio_connection_pair(bleio_connection_internal_t *self, bool bond) {
    // TODO: Implement this.
}

mp_float_t common_hal_bleio_connection_get_connection_interval(bleio_connection_internal_t *self) {
    // TODO: Implement this.
    while (self->conn_params_updating && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    return 0;
}

// Return the current negotiated MTU length, minus overhead.
mp_int_t common_hal_bleio_connection_get_max_packet_length(bleio_connection_internal_t *self) {
    return (self->mtu == 0 ? BLE_ATT_MTU_DFLT : self->mtu) - 3;
}

void common_hal_bleio_connection_set_connection_interval(bleio_connection_internal_t *self, mp_float_t new_interval) {
    self->conn_params_updating = true;
    // TODO: Implement this.
}

STATIC volatile int _last_discovery_status;
static TaskHandle_t discovery_task = NULL;

STATIC int _discovered_service_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    const struct ble_gatt_svc *svc,
    void *arg) {
    bleio_connection_internal_t *self = (bleio_connection_internal_t *)arg;

    if (error->status != BLE_ERR_SUCCESS) {
        // Keep the first error in case it's due to memory.
        if (_last_discovery_status == BLE_ERR_SUCCESS) {
            _last_discovery_status = error->status;
            xTaskNotifyGive(discovery_task);
        }
        return 0;
    }

    // If any of these memory allocations fail, we set _last_discovery_status
    // and let the process continue.
    if (_last_discovery_status != BLE_ERR_SUCCESS) {
        return 0;
    }
    bleio_service_obj_t *service = m_new_obj(bleio_service_obj_t);
    if (service == NULL) {
        _last_discovery_status = BLE_ERR_MEM_CAPACITY;
        return 0;
    }
    service->base.type = &bleio_service_type;

    // Initialize several fields at once.
    bleio_service_from_connection(service, bleio_connection_new_from_internal(self));

    service->is_remote = true;
    service->start_handle = svc->start_handle;
    service->end_handle = svc->end_handle;
    service->handle = svc->start_handle;

    bleio_uuid_obj_t *uuid = m_new_obj(bleio_uuid_obj_t);
    if (uuid == NULL) {
        _last_discovery_status = BLE_ERR_MEM_CAPACITY;
        return 0;
    }
    uuid->base.type = &bleio_uuid_type;
    uuid->nimble_ble_uuid = svc->uuid;
    service->uuid = uuid;

    mp_obj_list_append(MP_OBJ_FROM_PTR(self->remote_service_list),
        MP_OBJ_FROM_PTR(service));
    return 0;
}

STATIC int _discovered_characteristic_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    const struct ble_gatt_chr *chr,
    void *arg) {
    bleio_service_obj_t *service = (bleio_service_obj_t *)arg;

    if (error->status != BLE_ERR_SUCCESS) {
        // Keep the first error in case it's due to memory.
        if (_last_discovery_status == BLE_ERR_SUCCESS) {
            _last_discovery_status = error->status;
            xTaskNotifyGive(discovery_task);
        }
    }
    // If any of these memory allocations fail, we set _last_discovery_status
    // and let the process continue.
    if (_last_discovery_status != BLE_ERR_SUCCESS) {
        return 0;
    }

    bleio_characteristic_obj_t *characteristic = m_new_obj(bleio_characteristic_obj_t);
    if (characteristic == NULL) {
        _last_discovery_status = BLE_ERR_MEM_CAPACITY;
        return 0;
    }
    characteristic->base.type = &bleio_characteristic_type;

    // Known characteristic UUID.
    bleio_uuid_obj_t *uuid = m_new_obj(bleio_uuid_obj_t);
    if (uuid == NULL) {
        _last_discovery_status = BLE_ERR_MEM_CAPACITY;
        return 0;
    }
    uuid->base.type = &bleio_uuid_type;
    uuid->nimble_ble_uuid = chr->uuid;

    bleio_characteristic_properties_t props =
        ((chr->properties & BLE_GATT_CHR_PROP_BROADCAST) != 0 ? CHAR_PROP_BROADCAST : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_INDICATE) != 0 ? CHAR_PROP_INDICATE : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_NOTIFY) != 0 ? CHAR_PROP_NOTIFY : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_READ) != 0 ? CHAR_PROP_READ : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_WRITE) != 0 ? CHAR_PROP_WRITE : 0) |
        ((chr->properties & BLE_GATT_CHR_PROP_WRITE_NO_RSP) != 0 ? CHAR_PROP_WRITE_NO_RESPONSE : 0);

    // Call common_hal_bleio_characteristic_construct() to initalize some fields and set up evt handler.
    common_hal_bleio_characteristic_construct(
        characteristic, service, chr->val_handle, uuid,
        props, SECURITY_MODE_OPEN, SECURITY_MODE_OPEN,
        0, false,   // max_length, fixed_length: values don't matter for gattc
        mp_const_empty_bytes,
        NULL);
    // Set def_handle directly since it is only used in discovery.
    characteristic->def_handle = chr->def_handle;

    mp_obj_list_append(MP_OBJ_FROM_PTR(service->characteristic_list),
        MP_OBJ_FROM_PTR(characteristic));
    return 0;
}

STATIC int _discovered_descriptor_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    uint16_t chr_val_handle,
    const struct ble_gatt_dsc *dsc,
    void *arg) {
    bleio_characteristic_obj_t *characteristic = (bleio_characteristic_obj_t *)arg;

    if (error->status != BLE_ERR_SUCCESS) {
        // Keep the first error in case it's due to memory.
        if (_last_discovery_status == BLE_ERR_SUCCESS) {
            _last_discovery_status = error->status;
        }
        xTaskNotifyGive(discovery_task);
    }
    // If any of these memory allocations fail, we set _last_discovery_status
    // and let the process continue.
    if (_last_discovery_status != BLE_ERR_SUCCESS) {
        return 0;
    }

    // Remember handles for certain well-known descriptors.
    switch (dsc->uuid.u16.value) {
        case 0x2902:
            characteristic->cccd_handle = dsc->handle;
            break;

        case 0x2903:
            characteristic->sccd_handle = dsc->handle;
            break;

        case 0x2901:
            characteristic->user_desc_handle = dsc->handle;
            break;

        default:
            break;
    }

    bleio_descriptor_obj_t *descriptor = m_new_obj(bleio_descriptor_obj_t);
    if (descriptor == NULL) {
        _last_discovery_status = BLE_ERR_MEM_CAPACITY;
        return 0;
    }
    descriptor->base.type = &bleio_descriptor_type;

    bleio_uuid_obj_t *uuid = m_new_obj(bleio_uuid_obj_t);
    if (uuid == NULL) {
        _last_discovery_status = BLE_ERR_MEM_CAPACITY;
        return 0;
    }
    uuid->base.type = &bleio_uuid_type;
    uuid->nimble_ble_uuid = dsc->uuid;

    common_hal_bleio_descriptor_construct(
        descriptor, characteristic, uuid,
        SECURITY_MODE_OPEN, SECURITY_MODE_OPEN,
        0, false, mp_const_empty_bytes);
    descriptor->handle = dsc->handle;

    mp_obj_list_append(MP_OBJ_FROM_PTR(characteristic->descriptor_list),
        MP_OBJ_FROM_PTR(descriptor));
    return 0;
}

STATIC void discover_remote_services(bleio_connection_internal_t *self, mp_obj_t service_uuids_whitelist) {
    // Start over with an empty list.
    self->remote_service_list = mp_obj_new_list(0, NULL);

    discovery_task = xTaskGetCurrentTaskHandle();
    if (service_uuids_whitelist == mp_const_none) {
        _last_discovery_status = BLE_ERR_SUCCESS;
        CHECK_NIMBLE_ERROR(ble_gattc_disc_all_svcs(self->conn_handle, _discovered_service_cb, self));

        // Wait for sync.
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(DISCOVERY_TIMEOUT_MS));
        if (_last_discovery_status != BLE_HS_EDONE) {
            CHECK_BLE_ERROR(_last_discovery_status);
        }
    } else {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(service_uuids_whitelist, &iter_buf);
        mp_obj_t uuid_obj;
        while ((uuid_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if (!mp_obj_is_type(uuid_obj, &bleio_uuid_type)) {
                mp_raise_TypeError(translate("non-UUID found in service_uuids_whitelist"));
            }
            bleio_uuid_obj_t *uuid = MP_OBJ_TO_PTR(uuid_obj);

            _last_discovery_status = BLE_ERR_SUCCESS;
            // Make sure we start with a clean notification state
            ulTaskNotifyValueClear(discovery_task, 0xffffffff);
            CHECK_NIMBLE_ERROR(ble_gattc_disc_svc_by_uuid(self->conn_handle, &uuid->nimble_ble_uuid.u,
                _discovered_service_cb, self));
            // Wait for sync.
            CHECK_NOTIFY(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(DISCOVERY_TIMEOUT_MS)));
            if (_last_discovery_status != BLE_HS_EDONE) {
                CHECK_BLE_ERROR(_last_discovery_status);
            }
        }
    }

    for (size_t i = 0; i < self->remote_service_list->len; i++) {
        bleio_service_obj_t *service = MP_OBJ_TO_PTR(self->remote_service_list->items[i]);

        _last_discovery_status = BLE_ERR_SUCCESS;
        CHECK_NIMBLE_ERROR(ble_gattc_disc_all_chrs(self->conn_handle,
            service->start_handle,
            service->end_handle,
            _discovered_characteristic_cb,
            service));

        // Wait for sync.
        CHECK_NOTIFY(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(DISCOVERY_TIMEOUT_MS)));
        if (_last_discovery_status != BLE_HS_EDONE) {
            CHECK_BLE_ERROR(_last_discovery_status);
        }

        // Got characteristics for this service. Now discover descriptors for each characteristic.
        size_t char_list_len = service->characteristic_list->len;
        for (size_t char_idx = 0; char_idx < char_list_len; ++char_idx) {
            bleio_characteristic_obj_t *characteristic =
                MP_OBJ_TO_PTR(service->characteristic_list->items[char_idx]);
            // Determine the handle range for the given characteristic's descriptors.
            // The end of the range is dictated by the next characteristic or the end
            // handle of the service.
            const bool last_characteristic = char_idx == char_list_len - 1;
            bleio_characteristic_obj_t *next_characteristic = last_characteristic
                ? NULL
                : MP_OBJ_TO_PTR(service->characteristic_list->items[char_idx + 1]);

            uint16_t end_handle = next_characteristic == NULL
                ? service->end_handle
                : next_characteristic->def_handle - 1;

            // Pre-check if characteristic is empty so descriptor discovery doesn't fail
            if (end_handle <= characteristic->handle) {
                continue;
            }

            _last_discovery_status = BLE_ERR_SUCCESS;
            CHECK_NIMBLE_ERROR(ble_gattc_disc_all_dscs(self->conn_handle, characteristic->handle,
                end_handle,
                _discovered_descriptor_cb, characteristic));
            // Wait for sync.
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(DISCOVERY_TIMEOUT_MS));
            if (_last_discovery_status != BLE_HS_EDONE) {
                CHECK_BLE_ERROR(_last_discovery_status);
            }
        }
    }
}

mp_obj_tuple_t *common_hal_bleio_connection_discover_remote_services(bleio_connection_obj_t *self, mp_obj_t service_uuids_whitelist) {
    discover_remote_services(self->connection, service_uuids_whitelist);
    bleio_connection_ensure_connected(self);
    // Convert to a tuple and then clear the list so the callee will take ownership.
    mp_obj_tuple_t *services_tuple =
        mp_obj_new_tuple(self->connection->remote_service_list->len,
            self->connection->remote_service_list->items);
    mp_obj_list_clear(MP_OBJ_FROM_PTR(self->connection->remote_service_list));

    return services_tuple;
}

uint16_t bleio_connection_get_conn_handle(bleio_connection_obj_t *self) {
    if (self == NULL || self->connection == NULL) {
        return BLEIO_HANDLE_INVALID;
    }
    return self->connection->conn_handle;
}

mp_obj_t bleio_connection_new_from_internal(bleio_connection_internal_t *internal) {
    if (internal->connection_obj != mp_const_none) {
        return internal->connection_obj;
    }
    bleio_connection_obj_t *connection = m_new_obj(bleio_connection_obj_t);
    connection->base.type = &bleio_connection_type;
    connection->connection = internal;
    internal->connection_obj = connection;

    return MP_OBJ_FROM_PTR(connection);
}

// Find the connection that uses the given conn_handle. Return NULL if not found.
bleio_connection_internal_t *bleio_conn_handle_to_connection(uint16_t conn_handle) {
    bleio_connection_internal_t *connection;
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        connection = &bleio_connections[i];
        if (connection->conn_handle == conn_handle) {
            return connection;
        }
    }

    return NULL;
}
