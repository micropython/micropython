/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) Dan Halbert for Adafruit Industries
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

#include <string.h>

#include "py/runtime.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"

#include "common-hal/_bleio/Adapter.h"
// #include "common-hal/_bleio/bonding.h"

void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self, bleio_service_obj_t *service,
    uint16_t handle, bleio_uuid_obj_t *uuid, bleio_characteristic_properties_t props,
    bleio_attribute_security_mode_t read_perm, bleio_attribute_security_mode_t write_perm,
    mp_int_t max_length, bool fixed_length, mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {
    self->service = service;
    self->uuid = uuid;
    self->handle = BLEIO_HANDLE_INVALID;
    self->cccd_handle = BLEIO_HANDLE_INVALID;
    self->sccd_handle = BLEIO_HANDLE_INVALID;
    self->props = props;
    self->read_perm = read_perm;
    self->write_perm = write_perm;

    if (initial_value_bufinfo != NULL) {
        // Copy the initial value if it's on the heap. Otherwise it's internal and we may not be able
        // to allocate.
        self->current_value_len = initial_value_bufinfo->len;
        if (gc_alloc_possible()) {
            if (gc_nbytes(initial_value_bufinfo->buf) > 0) {
                uint8_t *initial_value = m_malloc(self->current_value_len);
                self->current_value_alloc = self->current_value_len;
                memcpy(initial_value, initial_value_bufinfo->buf, self->current_value_len);
                self->current_value = initial_value;
            } else {
                self->current_value_alloc = 0;
                self->current_value = initial_value_bufinfo->buf;
            }
        } else {
            self->current_value = initial_value_bufinfo->buf;
        }
    }

    if (gc_alloc_possible()) {
        self->descriptor_list = mp_obj_new_list(0, NULL);
    } else {
        self->descriptor_list = NULL;
    }

    self->max_length = max_length;
    self->fixed_length = fixed_length;

    if (service->is_remote) {
        self->handle = handle;
    } else {
        common_hal_bleio_service_add_characteristic(self->service, self, initial_value_bufinfo, user_description);
    }
}

mp_obj_tuple_t *common_hal_bleio_characteristic_get_descriptors(bleio_characteristic_obj_t *self) {
    if (self->descriptor_list == NULL) {
        return mp_const_empty_tuple;
    }
    return mp_obj_new_tuple(self->descriptor_list->len, self->descriptor_list->items);
}

bleio_service_obj_t *common_hal_bleio_characteristic_get_service(bleio_characteristic_obj_t *self) {
    return self->service;
}

typedef struct {
    TaskHandle_t task;
    uint8_t *buf;
    uint16_t len;
} _read_info_t;

STATIC int _read_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    struct ble_gatt_attr *attr,
    void *arg) {
    _read_info_t *read_info = (_read_info_t *)arg;
    switch (error->status) {
        case 0: {
            int len = MIN(read_info->len, OS_MBUF_PKTLEN(attr->om));
            os_mbuf_copydata(attr->om, attr->offset, len, read_info->buf);
            read_info->len = len;
        }
            MP_FALLTHROUGH;

        default:
            #if CIRCUITPY_VERBOSE_BLE
            // For debugging.
            mp_printf(&mp_plat_print, "Read status: %d\n", error->status);
            #endif
            xTaskNotify(read_info->task, error->status, eSetValueWithOverwrite);
            break;
    }

    return 0;
}

size_t common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self, uint8_t *buf, size_t len) {
    // Do GATT operations only if this characteristic has been added to a registered service.
    if (self->handle == BLEIO_HANDLE_INVALID) {
        return 0;
    }
    uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
    if (common_hal_bleio_service_get_is_remote(self->service)) {
        _read_info_t read_info = {
            .task = xTaskGetCurrentTaskHandle(),
            .buf = buf,
            .len = len
        };
        CHECK_NIMBLE_ERROR(ble_gattc_read(conn_handle, self->handle, _read_cb, &read_info));
        int error_code;
        xTaskNotifyWait(0, 0, (uint32_t *)&error_code, 200);
        CHECK_BLE_ERROR(error_code);
        return read_info.len;
    } else {
        len = MIN(self->current_value_len, len);
        memcpy(buf, self->current_value, len);
        return len;
    }

    return 0;
}

size_t common_hal_bleio_characteristic_get_max_length(bleio_characteristic_obj_t *self) {
    return self->max_length;
}

STATIC int _write_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    struct ble_gatt_attr *attr,
    void *arg) {
    TaskHandle_t task = (TaskHandle_t)arg;
    xTaskNotify(task, error->status, eSetValueWithOverwrite);

    return 0;
}

void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    if (common_hal_bleio_service_get_is_remote(self->service)) {
        uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
        if ((self->props & CHAR_PROP_WRITE_NO_RESPONSE) != 0) {
            CHECK_NIMBLE_ERROR(ble_gattc_write_no_rsp_flat(conn_handle, self->handle, bufinfo->buf, bufinfo->len));
        } else {
            CHECK_NIMBLE_ERROR(ble_gattc_write_flat(conn_handle, self->handle, bufinfo->buf, bufinfo->len, _write_cb, xTaskGetCurrentTaskHandle()));
            int error_code;
            xTaskNotifyWait(0, 0, (uint32_t *)&error_code, 200);
            CHECK_BLE_ERROR(error_code);
        }
    } else {
        // Validate data length for local characteristics only.
        // TODO: Test this once we can get servers going.
        if (self->fixed_length && bufinfo->len != self->max_length) {
            mp_raise_ValueError(MP_ERROR_TEXT("Value length != required fixed length"));
        }
        if (bufinfo->len > self->max_length) {
            mp_raise_ValueError(MP_ERROR_TEXT("Value length > max_length"));
        }

        if (bufinfo == NULL) {
            self->current_value_len = 0;
            ble_gatts_chr_updated(self->handle);
            return;
        }

        self->current_value_len = bufinfo->len;
        // If we've already allocated an internal buffer or the provided buffer
        // is on the heap, then copy into the internal buffer.
        if (self->current_value_alloc > 0 || gc_nbytes(bufinfo->buf) > 0) {
            if (self->current_value_alloc < bufinfo->len) {
                self->current_value = m_realloc(self->current_value, bufinfo->len);
                // Get the number of bytes from the heap because it may be more
                // than the len due to gc block size.
                self->current_value_alloc = gc_nbytes(self->current_value);
            }
            memcpy(self->current_value, bufinfo->buf, bufinfo->len);
        } else {
            // Otherwise, use the provided buffer to delay any heap allocation.
            self->current_value = bufinfo->buf;
            self->current_value_alloc = 0;
        }

        ble_gatts_chr_updated(self->handle);
    }
}

bleio_uuid_obj_t *common_hal_bleio_characteristic_get_uuid(bleio_characteristic_obj_t *self) {
    return self->uuid;
}

bleio_characteristic_properties_t common_hal_bleio_characteristic_get_properties(bleio_characteristic_obj_t *self) {
    return self->props;
}

void common_hal_bleio_characteristic_add_descriptor(bleio_characteristic_obj_t *self,
    bleio_descriptor_obj_t *descriptor) {
    // TODO: Implement this.

    mp_obj_list_append(MP_OBJ_FROM_PTR(self->descriptor_list),
        MP_OBJ_FROM_PTR(descriptor));
}

void common_hal_bleio_characteristic_set_cccd(bleio_characteristic_obj_t *self, bool notify, bool indicate) {
    if (self->cccd_handle == BLEIO_HANDLE_INVALID) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("No CCCD for this Characteristic"));
    }

    if (!common_hal_bleio_service_get_is_remote(self->service)) {
        mp_raise_bleio_RoleError(MP_ERROR_TEXT("Can't set CCCD on local Characteristic"));
    }

    const uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
    common_hal_bleio_check_connected(conn_handle);

    uint16_t cccd_value =
        (notify ? 1 << 0 : 0) |
        (indicate ? 1 << 1: 0);

    CHECK_NIMBLE_ERROR(ble_gattc_write_flat(conn_handle, self->cccd_handle, &cccd_value, 2, _write_cb, xTaskGetCurrentTaskHandle()));
    int error_code;
    xTaskNotifyWait(0, 0, (uint32_t *)&error_code, 200);
    CHECK_BLE_ERROR(error_code);
}
