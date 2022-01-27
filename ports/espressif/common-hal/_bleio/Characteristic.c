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
    mp_raise_NotImplementedError(NULL);
    self->service = service;
    self->uuid = uuid;
    self->handle = BLEIO_HANDLE_INVALID;
    self->props = props;
    self->read_perm = read_perm;
    self->write_perm = write_perm;
    self->initial_value_len = 0;
    self->initial_value = NULL;
    if (initial_value_bufinfo != NULL) {
        // Copy the initial value if it's on the heap. Otherwise it's internal and we may not be able
        // to allocate.
        self->initial_value_len = initial_value_bufinfo->len;
        if (gc_alloc_possible()) {
            if (gc_nbytes(initial_value_bufinfo->buf) > 0) {
                uint8_t *initial_value = m_malloc(self->initial_value_len, false);
                memcpy(initial_value, initial_value_bufinfo->buf, self->initial_value_len);
                self->initial_value = initial_value;
            } else {
                self->initial_value = initial_value_bufinfo->buf;
            }
            self->descriptor_list = mp_obj_new_list(0, NULL);
        } else {
            self->initial_value = initial_value_bufinfo->buf;
            self->descriptor_list = NULL;
        }
    }

    // const mp_int_t max_length_max = fixed_length ? BLE_GATTS_FIX_ATTR_LEN_MAX : BLE_GATTS_VAR_ATTR_LEN_MAX;
    // if (max_length < 0 || max_length > max_length_max) {
    //     mp_raise_ValueError_varg(translate("max_length must be 0-%d when fixed_length is %s"),
    //         max_length_max, fixed_length ? "True" : "False");
    // }
    // TODO: Implement this.
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

size_t common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self, uint8_t *buf, size_t len) {
    // TODO: Implement this.
    return 0;
}

size_t common_hal_bleio_characteristic_get_max_length(bleio_characteristic_obj_t *self) {
    return self->max_length;
}

void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    // TODO: Implement this.
}

bleio_uuid_obj_t *common_hal_bleio_characteristic_get_uuid(bleio_characteristic_obj_t *self) {
    return self->uuid;
}

bleio_characteristic_properties_t common_hal_bleio_characteristic_get_properties(bleio_characteristic_obj_t *self) {
    return self->props;
}

void common_hal_bleio_characteristic_add_descriptor(bleio_characteristic_obj_t *self, bleio_descriptor_obj_t *descriptor) {
    // TODO: Implement this.
}

void common_hal_bleio_characteristic_set_cccd(bleio_characteristic_obj_t *self, bool notify, bool indicate) {
    // TODO: Implement this.
}
