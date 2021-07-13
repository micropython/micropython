/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "shared-bindings/_bleio/Address.h"
#include "shared-module/_bleio/Address.h"
#include "shared-module/_bleio/ScanEntry.h"

mp_obj_t common_hal_bleio_scanentry_get_address(bleio_scanentry_obj_t *self) {
    return MP_OBJ_FROM_PTR(self->address);
}

mp_obj_t common_hal_bleio_scanentry_get_advertisement_bytes(bleio_scanentry_obj_t *self) {
    return MP_OBJ_FROM_PTR(self->data);
}

mp_int_t common_hal_bleio_scanentry_get_rssi(bleio_scanentry_obj_t *self) {
    return self->rssi;
}

bool common_hal_bleio_scanentry_get_connectable(bleio_scanentry_obj_t *self) {
    return self->connectable;
}

bool common_hal_bleio_scanentry_get_scan_response(bleio_scanentry_obj_t *self) {
    return self->scan_response;
}

bool bleio_scanentry_data_matches(const uint8_t *data, size_t len, const uint8_t *prefixes, size_t prefixes_length, bool any) {
    if (prefixes_length == 0) {
        return true;
    }
    if (len == 0) {
        // Prefixes exist, but no data.
        return false;
    }
    size_t i = 0;
    while (i < prefixes_length) {
        uint8_t prefix_length = prefixes[i];
        i += 1;
        size_t j = 0;
        bool prefix_matched = false;
        while (j < len) {
            uint8_t structure_length = data[j];
            j += 1;
            if (structure_length == 0) {
                break;
            }
            if (structure_length >= prefix_length && memcmp(data + j, prefixes + i, prefix_length) == 0) {
                if (any) {
                    return true;
                }
                prefix_matched = true;
                break;
            }
            j += structure_length;
        }
        // If all (!any), the current prefix must have matched at least one field.
        if (!prefix_matched && !any) {
            return false;
        }
        i += prefix_length;
    }
    // All prefixes matched some field (if !any), or none did (if any).
    return !any;
}

bool common_hal_bleio_scanentry_matches(bleio_scanentry_obj_t *self, const uint8_t *prefixes, size_t prefixes_len, bool all) {
    return bleio_scanentry_data_matches(self->data->data, self->data->len, prefixes, prefixes_len, !all);
}
