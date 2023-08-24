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

#include <string.h>
#include <stdio.h>
#include "py/runtime.h"
#include "common-hal/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/shared/tick.h"
#include "supervisor/serial.h"

EventGroupHandle_t xcharacteristic_event;

// Set the characteristic data from sl_bt_on_event
bool set_characteristic_value_on_ble_evt(uint8_t conn_handle,
    uint16_t char_handle,
    uint8_t *data,
    size_t data_len) {

    uint8_t serv_index;
    uint8_t charc_index;
    bleio_service_obj_t *service;
    bleio_characteristic_obj_t *characteristic;
    bleio_connection_internal_t *connection =
        bleio_conn_handle_to_connection(conn_handle);
    if (NULL == connection) {
        mp_raise_bleio_BluetoothError(translate("Get connection fail."));
        return false;
    }
    for (serv_index = 0; serv_index < connection->remote_service_list->len; serv_index++) {
        service = connection->remote_service_list->items[serv_index];

        for (charc_index = 0; charc_index < service->characteristic_list->len; charc_index++) {
            characteristic = service->characteristic_list->items[charc_index];

            if (char_handle == characteristic->handle) {
                characteristic->current_value = pvPortMalloc(data_len);
                characteristic->current_value_len = data_len;
                memcpy(characteristic->current_value, data, data_len);

                if (xcharacteristic_event != NULL) {
                    xEventGroupSetBits(xcharacteristic_event, 1 << 0);
                }
                return true;
            }
        }
    }
    return false;
}

// Get the characteristic data object
STATIC bool get_characteristic_value(uint8_t conn_handle,
    uint16_t char_handle,
    uint8_t *data,
    size_t *data_len) {

    uint8_t serv_index;
    uint8_t charc_index;
    bleio_service_obj_t *service;
    bleio_characteristic_obj_t *characteristic;
    bleio_connection_internal_t *connection = bleio_conn_handle_to_connection(conn_handle);

    if (NULL == connection) {
        mp_raise_bleio_BluetoothError(translate("Get connection fail."));
        return false;
    }
    for (serv_index = 0; serv_index < connection->remote_service_list->len; serv_index++) {
        service = connection->remote_service_list->items[serv_index];

        for (charc_index = 0; charc_index < service->characteristic_list->len; charc_index++) {
            characteristic = service->characteristic_list->items[charc_index];

            if (char_handle == characteristic->handle) {
                *data_len = characteristic->current_value_len;
                memcpy(data, characteristic->current_value, *data_len);
                vPortFree(characteristic->current_value);
                return true;
            }
        }
    }
    return false;
}

// Create new bleio characteristic
void common_hal_bleio_characteristic_construct(
    bleio_characteristic_obj_t *self,
    bleio_service_obj_t *service,
    uint16_t handle, bleio_uuid_obj_t *uuid,
    bleio_characteristic_properties_t props,
    bleio_attribute_security_mode_t read_perm,
    bleio_attribute_security_mode_t write_perm,
    mp_int_t max_length,
    bool fixed_length,
    mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {

    self->service = service;
    self->uuid = uuid;
    self->handle = BLEIO_HANDLE_INVALID;
    self->cccd_handle = BLEIO_HANDLE_INVALID;
    self->sccd_handle = BLEIO_HANDLE_INVALID;
    self->props = props;
    self->read_perm = read_perm;
    self->write_perm = write_perm;
    self->max_length = max_length > MAX_LENGTH_DATA ? MAX_LENGTH_DATA : max_length;
    self->max_length = self->max_length ? self->max_length : MAX_LENGTH_DATA;
    self->fixed_length = fixed_length;

    if (gc_alloc_possible()) {
        self->descriptor_list = mp_obj_new_list(0, NULL);
    } else {
        self->descriptor_list = NULL;
    }

    if (service->is_remote) {
        self->handle = handle;
    } else {
        common_hal_bleio_service_add_characteristic(self->service,
            self,
            initial_value_bufinfo,
            user_description);

        sl_bt_gatt_server_write_attribute_value(
            self->handle,
            0,
            initial_value_bufinfo->len,
            (uint8_t *)initial_value_bufinfo->buf);
    }
}

// A tuple of Descriptor that describe this characteristic
mp_obj_tuple_t *common_hal_bleio_characteristic_get_descriptors(
    bleio_characteristic_obj_t *self) {

    if (self->descriptor_list == NULL) {
        return mp_const_empty_tuple;
    }
    return mp_obj_new_tuple(self->descriptor_list->len,
        self->descriptor_list->items);
}

// The Service this Characteristic is a part of
bleio_service_obj_t *common_hal_bleio_characteristic_get_service(
    bleio_characteristic_obj_t *self) {

    return self->service;
}

// Get value of characteristic
size_t common_hal_bleio_characteristic_get_value(
    bleio_characteristic_obj_t *self,
    uint8_t *buf,
    size_t len) {

    sl_status_t sc = SL_STATUS_FAIL;
    EventBits_t ux_bits;
    uint8_t retry = 10;
    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t current_ticks = start_ticks;

    if (self->handle == BLEIO_HANDLE_INVALID) {
        return 0;
    }
    uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);

    if (common_hal_bleio_service_get_is_remote(self->service)) {
        // ble client gets characteristic value
        if (BT_GATT_CHRC_READ & self->props) {
            sc = sl_bt_gatt_read_characteristic_value(conn_handle, self->handle);
            while (SL_STATUS_OK != sc && retry > 0) {
                sc = sl_bt_gatt_read_characteristic_value(conn_handle, self->handle);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                retry--;
            }
        }

        xcharacteristic_event = xEventGroupCreate();
        if (xcharacteristic_event != NULL) {
            xEventGroupClearBits(xcharacteristic_event, 1 << 0);
        }

        while (current_ticks - start_ticks <= GET_CHARACTERISTIC_TIMEOUT_MS) {
            if (xcharacteristic_event != NULL) {
                ux_bits = xEventGroupWaitBits(
                    xcharacteristic_event, 1 << 0, pdTRUE, pdFALSE,
                        GET_CHARACTERISTIC_TIMEOUT_MS / portTICK_PERIOD_MS);

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
        get_characteristic_value(conn_handle, self->handle, buf, &len);
        vEventGroupDelete(xcharacteristic_event);
        return len;

    } else {
        sc = sl_bt_gatt_server_read_attribute_value(self->handle,
            0,
            self->max_length,
            &len,
            (uint8_t *)buf);
        if (SL_STATUS_OK != sc) {
            mp_raise_bleio_BluetoothError(
                translate("Read_attribute_value fail!"));
        }
        return len;
    }

    return 0;
}

// Get max length of charateristic
size_t common_hal_bleio_characteristic_get_max_length(
    bleio_characteristic_obj_t *self) {
    return self->max_length;
}

// Set value of this characteristic
void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self,
    mp_buffer_info_t *bufinfo) {

    sl_status_t sc = SL_STATUS_FAIL;
    uint16_t conn_handle;
    uint16_t sent_len;

    if (common_hal_bleio_service_get_is_remote(self->service)) {

        conn_handle = bleio_connection_get_conn_handle(self->service->connection);
        if ((self->props & CHAR_PROP_WRITE_NO_RESPONSE) != 0) {
            sc = sl_bt_gatt_write_characteristic_value_without_response(
                conn_handle,
                self->handle,
                bufinfo->len,
                bufinfo->buf,
                &sent_len);
        } else {
            sc = sl_bt_gatt_write_characteristic_value(conn_handle,
                self->handle,
                bufinfo->len,
                bufinfo->buf);
        }
    } else {
        if (self->props & BT_GATT_CHRC_READ) {
            sc = sl_bt_gatt_server_write_attribute_value(self->handle,
                0,
                bufinfo->len,
                (uint8_t *)bufinfo->buf);
            if (SL_STATUS_OK != sc) {
                mp_raise_bleio_BluetoothError(
                    translate("Write_attribute_value fail!"));
            }
        }

        if (self->props & BT_GATT_CHRC_NOTIFY) {
            sc = sl_bt_gatt_server_send_notification(
                1,
                self->handle,
                bufinfo->len,
                (uint8_t *)bufinfo->buf);
        }

        if (self->props & BT_GATT_CHRC_INDICATE) {
            sc = sl_bt_gatt_server_send_indication(
                1,
                self->handle,
                bufinfo->len,
                (uint8_t *)bufinfo->buf);
        }
    }
}

// Get UUID of this characteristic
bleio_uuid_obj_t *common_hal_bleio_characteristic_get_uuid(
    bleio_characteristic_obj_t *self) {
    return self->uuid;
}

// Get properties of this characteristic
bleio_characteristic_properties_t common_hal_bleio_characteristic_get_properties(
    bleio_characteristic_obj_t *self) {
    return self->props;
}

// Add new descriptor to characteristic
void common_hal_bleio_characteristic_add_descriptor(
    bleio_characteristic_obj_t *self,
    bleio_descriptor_obj_t *descriptor) {

    sl_status_t sc = SL_STATUS_FAIL;
    const uint8_t value;
    uuid_128 bt_uuid_128;
    sl_bt_uuid_16_t bt_uuid_16;
    uint16_t gattdb_session;

    sc = sl_bt_gattdb_new_session(&gattdb_session);

    if (SL_STATUS_OK != sc && SL_STATUS_ALREADY_EXISTS != sc) {
        mp_raise_bleio_BluetoothError(translate("Create new session fail."));
        return;
    }

    if (BLE_UUID_TYPE_16 == descriptor->uuid->efr_ble_uuid.uuid.type) {
        bt_uuid_16.data[0] = descriptor->uuid->efr_ble_uuid.uuid16.value & 0xff;
        bt_uuid_16.data[1] = descriptor->uuid->efr_ble_uuid.uuid16.value >> 8;

        sl_bt_gattdb_add_uuid16_descriptor(self->session,
            self->handle,
            descriptor->handle,
            0,
            bt_uuid_16,
            sl_bt_gattdb_user_managed_value,
            descriptor->max_length,
            2,
            &value,
            &descriptor->handle);
    } else {
        memcpy(bt_uuid_128.data, descriptor->uuid->efr_ble_uuid.uuid128.value, 16);
        sl_bt_gattdb_add_uuid128_descriptor(self->session,
            self->handle,
            descriptor->handle,
            0,
            bt_uuid_128,
            sl_bt_gattdb_user_managed_value,
            descriptor->max_length,
            2,
            &value,
            &descriptor->handle);
    }

    sc = sl_bt_gattdb_commit(gattdb_session);
    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(translate("Commit descriptor fail."));
        return;
    }

    mp_obj_list_append(MP_OBJ_FROM_PTR(self->descriptor_list),
        MP_OBJ_FROM_PTR(descriptor));
}

// Set the remote characteristic’s CCCD to enable or disable notification and indication.
void common_hal_bleio_characteristic_set_cccd(bleio_characteristic_obj_t *self,
    bool notify, bool indicate) {

    sl_status_t sc = SL_STATUS_FAIL;

    const uint16_t conn_handle = bleio_connection_get_conn_handle(
        self->service->connection);
    common_hal_bleio_check_connected(conn_handle);
    notify = 1;
    indicate = 0;
    if (notify) {
        sc = sl_bt_gatt_set_characteristic_notification(conn_handle,
            self->handle, sl_bt_gatt_notification);
        if (SL_STATUS_OK != sc) {
            mp_raise_bleio_BluetoothError(translate("Notify fail"));
        }
    }

    if (indicate) {
        sc = sl_bt_gatt_set_characteristic_notification(conn_handle,
            self->handle, sl_bt_gatt_indication);
        if (SL_STATUS_OK != sc) {
            mp_raise_bleio_BluetoothError(translate("Indicate fail"));
        }
    }

    if (0 == notify && 0 == indicate) {
        sc = sl_bt_gatt_set_characteristic_notification(conn_handle,
            self->handle, sl_bt_gatt_disable);
        if (SL_STATUS_OK != sc) {
            mp_raise_bleio_BluetoothError(translate("Indicate fail"));
        }
    }
}
