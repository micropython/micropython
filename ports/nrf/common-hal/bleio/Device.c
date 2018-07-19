/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include <stdio.h>

#include "ble_drv.h"
#include "ble_gap.h"
#include "ble_gatt.h"
#include "ble_types.h"
#include "py/runtime.h"
#include "shared-bindings/bleio/Characteristic.h"
#include "shared-bindings/bleio/Device.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"

static volatile bool m_disc_evt_received;

STATIC void gap_event_handler(bleio_device_obj_t *device, uint16_t event_id, uint16_t conn_handle, uint16_t length, uint8_t * data) {
    if (event_id == BLE_GAP_EVT_CONNECTED) {
        device->conn_handle = conn_handle;
    } else if (event_id == BLE_GAP_EVT_DISCONNECTED) {
        device->conn_handle = BLE_CONN_HANDLE_INVALID;
    }
}

STATIC void gatts_event_handler(bleio_device_obj_t *device, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data) {

}

STATIC void gattc_event_handler(bleio_device_obj_t *device, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data) {
    m_disc_evt_received = true;
}

STATIC void disc_add_char(bleio_service_obj_t *service, ble_drv_char_data_t *chara_data) {
    bleio_characteristic_obj_t *chara = m_new_obj(bleio_characteristic_obj_t);
    chara->base.type = &bleio_characteristic_type;

    bleio_uuid_obj_t * p_uuid = m_new_obj(bleio_uuid_obj_t);
    p_uuid->base.type = &bleio_uuid_type;

    chara->uuid = p_uuid;

    p_uuid->type = chara_data->uuid_type;
    p_uuid->value[0] = chara_data->uuid & 0xFF;
    p_uuid->value[1] = chara_data->uuid >> 8;

    // add characteristic specific data from discovery
    chara->props.broadcast = chara_data->props.broadcast;
    chara->props.indicate = chara_data->props.indicate;
    chara->props.notify = chara_data->props.notify;
    chara->props.read = chara_data->props.read;
    chara->props.write = chara_data->props.write;
    chara->props.write_wo_resp = chara_data->props.write_wo_resp;
    chara->handle = chara_data->value_handle;

    chara->service_handle = service->handle;
    chara->service = service;

    mp_obj_list_append(service->char_list, MP_OBJ_FROM_PTR(chara));
}

void common_hal_bleio_device_start_advertising(bleio_device_obj_t *device, bleio_advertisement_data_t *adv_data) {
    if (adv_data->connectable) {
        ble_drv_gap_event_handler_set(device, gap_event_handler);
        ble_drv_gatts_event_handler_set(device, gatts_event_handler);
    }

    ble_drv_advertise_data(adv_data);
}

void common_hal_bleio_device_stop_advertising(bleio_device_obj_t *device) {
    (void)device;

    ble_drv_advertise_stop();
}

void common_hal_bleio_device_connect(bleio_device_obj_t *device) {
    ble_drv_gap_event_handler_set(device, gap_event_handler);

    ble_drv_connect(device);

    while (device->conn_handle == BLE_CONN_HANDLE_INVALID) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }

    ble_drv_gattc_event_handler_set(device, gattc_event_handler);

    // TODO: read name

    // find services
    bool found_service = ble_drv_discover_services(device, BLE_GATT_HANDLE_START);
    while (found_service) {
        const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(device->service_list);
        const bleio_service_obj_t *service = service_list->items[service_list->len - 1];

        found_service = ble_drv_discover_services(device, service->end_handle + 1);
    }

    // find characteristics in each service
    const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(device->service_list);
    for (size_t i = 0; i < service_list->len; ++i) {
        bleio_service_obj_t *service = service_list->items[i];

        bool found_char = ble_drv_discover_characteristic(device, service, service->start_handle, disc_add_char);
        while (found_char) {
            const mp_obj_list_t *char_list = MP_OBJ_TO_PTR(service->char_list);
            const bleio_characteristic_obj_t *characteristic = char_list->items[char_list->len - 1];

            const uint16_t next_handle = characteristic->handle + 1;
            if (next_handle >= service->end_handle) {
                break;
            }

            found_char = ble_drv_discover_characteristic(device, service, next_handle, disc_add_char);
        }
    }
}

void common_hal_bleio_device_disconnect(bleio_device_obj_t *device) {
    ble_drv_disconnect(device);
}
