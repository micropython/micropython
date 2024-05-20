// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/gc.h"
#include "py/objnamedtuple.h"
#include "shared-bindings/qrio/__init__.h"
#include "shared-bindings/qrio/QRInfo.h"
#include "shared-module/qrio/QRDecoder.h"

void shared_module_qrio_qrdecoder_construct(qrdecoder_qrdecoder_obj_t *self, int width, int height) {
    self->quirc = quirc_new();
    quirc_resize(self->quirc, width, height);
}

int shared_module_qrio_qrdecoder_get_height(qrdecoder_qrdecoder_obj_t *self) {
    int height;
    quirc_begin(self->quirc, NULL, &height);
    return height;
}

int shared_module_qrio_qrdecoder_get_width(qrdecoder_qrdecoder_obj_t *self) {
    int width;
    quirc_begin(self->quirc, &width, NULL);
    return width;
}
void shared_module_qrio_qrdecoder_set_height(qrdecoder_qrdecoder_obj_t *self, int height) {
    if (height != shared_module_qrio_qrdecoder_get_height(self)) {
        int width = shared_module_qrio_qrdecoder_get_width(self);
        quirc_resize(self->quirc, width, height);
    }
}

void shared_module_qrio_qrdecoder_set_width(qrdecoder_qrdecoder_obj_t *self, int width) {
    if (width != shared_module_qrio_qrdecoder_get_width(self)) {
        int height = shared_module_qrio_qrdecoder_get_height(self);
        quirc_resize(self->quirc, width, height);
    }
}

static mp_obj_t data_type(int type) {
    switch (type) {
        case QUIRC_ECI_ISO_8859_1:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_1);
        case QUIRC_ECI_IBM437:
            return MP_OBJ_NEW_QSTR(MP_QSTR_cp437);
        case QUIRC_ECI_ISO_8859_2:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_2);
        case QUIRC_ECI_ISO_8859_3:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_3);
        case QUIRC_ECI_ISO_8859_4:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_4);
        case QUIRC_ECI_ISO_8859_5:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_5);
        case QUIRC_ECI_ISO_8859_6:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_6);
        case QUIRC_ECI_ISO_8859_7:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_7);
        case QUIRC_ECI_ISO_8859_8:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_8);
        case QUIRC_ECI_ISO_8859_9:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_9);
        case QUIRC_ECI_WINDOWS_874:
            return MP_OBJ_NEW_QSTR(MP_QSTR_cp874);
        case QUIRC_ECI_ISO_8859_13:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_13);
        case QUIRC_ECI_ISO_8859_15:
            return MP_OBJ_NEW_QSTR(MP_QSTR_iso_8859_hyphen_15);
        case QUIRC_ECI_SHIFT_JIS:
            return MP_OBJ_NEW_QSTR(MP_QSTR_shift_underscore_jis);
        case QUIRC_ECI_UTF_8:
            return MP_OBJ_NEW_QSTR(MP_QSTR_utf_hyphen_8);
    }
    return mp_obj_new_int(type);
}

static void quirc_fill_buffer(qrdecoder_qrdecoder_obj_t *self, void *buf, qrio_pixel_policy_t policy) {
    int width, height;
    uint8_t *framebuffer = quirc_begin(self->quirc, &width, &height);
    uint8_t *src = buf;

    switch (policy) {
        case QRIO_RGB565: {
            uint16_t *src16 = buf;
            for (int i = 0; i < width * height; i++) {
                framebuffer[i] = (src16[i] >> 3) & 0xfc;
            }
            break;
        }
        case QRIO_RGB565_SWAPPED: {
            uint16_t *src16 = buf;
            for (int i = 0; i < width * height; i++) {
                framebuffer[i] = (__builtin_bswap16(src16[i]) >> 3) & 0xfc;
            }
            break;
        }
        case QRIO_EVERY_BYTE:
            memcpy(framebuffer, src, width * height);
            break;

        case QRIO_ODD_BYTES:
            src++;
            MP_FALLTHROUGH;

        case QRIO_EVEN_BYTES:
            for (int i = 0; i < width * height; i++) {
                framebuffer[i] = src[2 * i];
            }
            break;
    }
    quirc_end(self->quirc);
}


mp_obj_t shared_module_qrio_qrdecoder_decode(qrdecoder_qrdecoder_obj_t *self, const mp_buffer_info_t *bufinfo, qrio_pixel_policy_t policy) {
    quirc_fill_buffer(self, bufinfo->buf, policy);
    int count = quirc_count(self->quirc);
    mp_obj_t result = mp_obj_new_list(0, NULL);
    for (int i = 0; i < count; i++) {
        quirc_extract(self->quirc, i, &self->code);
        mp_obj_t code_obj;
        if (quirc_decode(&self->code, &self->data) != QUIRC_SUCCESS) {
            continue;
        }
        mp_obj_t elems[2] = {
            mp_obj_new_bytes(self->data.payload, self->data.payload_len),
            data_type(self->data.data_type),
        };
        code_obj = namedtuple_make_new((const mp_obj_type_t *)&qrio_qrinfo_type_obj, 2, 0, elems);
        mp_obj_list_append(result, code_obj);
    }
    return result;
}


mp_obj_t shared_module_qrio_qrdecoder_find(qrdecoder_qrdecoder_obj_t *self, const mp_buffer_info_t *bufinfo, qrio_pixel_policy_t policy) {
    quirc_fill_buffer(self, bufinfo->buf, policy);
    int count = quirc_count(self->quirc);
    mp_obj_t result = mp_obj_new_list(0, NULL);
    for (int i = 0; i < count; i++) {
        quirc_extract(self->quirc, i, &self->code);
        mp_obj_t code_obj;
        mp_obj_t elems[9] = {
            mp_obj_new_int(self->code.corners[0].x),
            mp_obj_new_int(self->code.corners[0].y),
            mp_obj_new_int(self->code.corners[1].x),
            mp_obj_new_int(self->code.corners[1].y),
            mp_obj_new_int(self->code.corners[2].x),
            mp_obj_new_int(self->code.corners[2].y),
            mp_obj_new_int(self->code.corners[3].x),
            mp_obj_new_int(self->code.corners[3].y),
            mp_obj_new_int(self->code.size),
        };
        code_obj = namedtuple_make_new((const mp_obj_type_t *)&qrio_qrposition_type_obj, 9, 0, elems);
        mp_obj_list_append(result, code_obj);
    }
    return result;
}
