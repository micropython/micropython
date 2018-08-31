/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include "ble.h"
#include "ble_drv.h"
#include "common-hal/bleio/UUID.h"
#include "nrf_error.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/bleio/Adapter.h"
#include "shared-bindings/bleio/UUID.h"

#define UUID_STR_16BIT_LEN 6
#define UUID_STR_128BIT_LEN 36

static uint8_t xdigit_8b_value(byte nibble1, byte nibble2) {
    return unichar_xdigit_value(nibble1) |
            (unichar_xdigit_value(nibble2) << 4);
}

void common_hal_bleio_uuid_construct(bleio_uuid_obj_t *self, const mp_obj_t *uuid) {
    if (MP_OBJ_IS_INT(*uuid)) {
        self->type = UUID_TYPE_16BIT;

        self->value[1] = (mp_obj_get_int(*uuid) >> 8) & 0xFF;
        self->value[0] = (mp_obj_get_int(*uuid) >> 0) & 0xFF;
        return;
    }

    if (MP_OBJ_IS_STR(*uuid)) {
        GET_STR_DATA_LEN(*uuid, str_data, str_len);

        if (str_len == UUID_STR_16BIT_LEN) {
            self->type = UUID_TYPE_16BIT;

            self->value[0]  = xdigit_8b_value(str_data[5], str_data[4]);
            self->value[1]  = xdigit_8b_value(str_data[3], str_data[2]);
        } else if (str_len == UUID_STR_128BIT_LEN) {
            self->type = UUID_TYPE_128BIT;

            ble_uuid128_t vs_uuid;
            vs_uuid.uuid128[0] = xdigit_8b_value(str_data[35], str_data[34]);
            vs_uuid.uuid128[1] = xdigit_8b_value(str_data[33], str_data[32]);
            vs_uuid.uuid128[2] = xdigit_8b_value(str_data[31], str_data[30]);
            vs_uuid.uuid128[3] = xdigit_8b_value(str_data[29], str_data[28]);
            vs_uuid.uuid128[4] = xdigit_8b_value(str_data[27], str_data[26]);
            vs_uuid.uuid128[5] = xdigit_8b_value(str_data[25], str_data[24]);

            // 23 '-'
            vs_uuid.uuid128[6] = xdigit_8b_value(str_data[22], str_data[21]);
            vs_uuid.uuid128[7] = xdigit_8b_value(str_data[20], str_data[19]);

            // 18 '-'
            vs_uuid.uuid128[8] = xdigit_8b_value(str_data[17], str_data[16]);
            vs_uuid.uuid128[9] = xdigit_8b_value(str_data[15], str_data[14]);

            // 13 '-'
            vs_uuid.uuid128[10] = xdigit_8b_value(str_data[12], str_data[11]);
            vs_uuid.uuid128[11] = xdigit_8b_value(str_data[10], str_data[9]);

            // 8 '-'
            self->value[0] = xdigit_8b_value(str_data[7], str_data[6]);
            self->value[1] = xdigit_8b_value(str_data[5], str_data[4]);

            vs_uuid.uuid128[14] = xdigit_8b_value(str_data[3], str_data[2]);
            vs_uuid.uuid128[15] = xdigit_8b_value(str_data[1], str_data[0]);

            common_hal_bleio_adapter_set_enabled(true);

            const uint32_t err_code = sd_ble_uuid_vs_add(&vs_uuid, &self->uuid_vs_idx);
            if (err_code != NRF_SUCCESS) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                    translate("Failed to add Vendor Specific UUID, status: 0x%08lX"), err_code));
            }

        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                translate("Invalid UUID string length")));
        }

        return;
    }

    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
              translate("Invalid UUID parameter")));
}

void common_hal_bleio_uuid_print(bleio_uuid_obj_t *self, const mp_print_t *print) {
    if (self->type == UUID_TYPE_16BIT) {
        mp_printf(print, "UUID(uuid: 0x" HEX2_FMT HEX2_FMT ")",
            self->value[1], self->value[0]);
    } else {
        mp_printf(print, "UUID(uuid: 0x" HEX2_FMT HEX2_FMT ", VS idx: " HEX2_FMT ")",
            self->value[1], self->value[0], self->uuid_vs_idx);
    }
}

bleio_uuid_type_t common_hal_bleio_uuid_get_type(bleio_uuid_obj_t *self) {
    return self->type;
}
