// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2017 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/ScanEntry.h"
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

bool common_hal_bleio_scanentry_matches(bleio_scanentry_obj_t *self, const uint8_t *prefixes, size_t prefixes_len, bool match_all) {
    return bleio_scanentry_data_matches(self->data->data, self->data->len, prefixes, prefixes_len, !match_all);
}
