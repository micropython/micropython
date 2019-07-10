/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#include "py/mperrno.h"
#include "modesp32.h"
#include "esp_ota_ops.h"

// esp_partition_read and esp_partition_write can operate on arbitrary bytes
// but esp_partition_erase_range operates on 4k blocks.  But to make a partition
// implement the standard block protocol all operations are done on 4k blocks.
#define BLOCK_SIZE_BYTES (4096)

enum {
    ESP32_PARTITION_FIRST,
    ESP32_PARTITION_BOOT,
    ESP32_PARTITION_RUNNING,
};

typedef struct _esp32_partition_obj_t {
    mp_obj_base_t base;
    const esp_partition_t *part;
} esp32_partition_obj_t;

static inline void check_esp_err(esp_err_t e) {
    if (e != ESP_OK) {
        mp_raise_OSError(-e);
    }
}

STATIC esp32_partition_obj_t *esp32_partition_new(const esp_partition_t *part) {
    if (part == NULL) {
        mp_raise_OSError(MP_ENOENT);
    }
    esp32_partition_obj_t *self = m_new_obj(esp32_partition_obj_t);
    self->base.type = &esp32_partition_type;
    self->part = part;
    return self;
}

STATIC void esp32_partition_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<Partition type=%u, subtype=%u, address=%u, size=%u, label=%s, encrypted=%u>",
        self->part->type, self->part->subtype,
        self->part->address, self->part->size,
        &self->part->label[0], self->part->encrypted
    );
}

STATIC mp_obj_t esp32_partition_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Parse args
    enum { ARG_id, ARG_type, ARG_subtype, ARG_label };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_type, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = ESP_PARTITION_TYPE_APP} },
        { MP_QSTR_subtype, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = ESP_PARTITION_SUBTYPE_ANY} },
        { MP_QSTR_label, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get requested partition
    const esp_partition_t *part;
    switch (args[ARG_id].u_int) {
        case ESP32_PARTITION_FIRST: {
            const char *label = NULL;
            if (args[ARG_label].u_obj != mp_const_none) {
                label = mp_obj_str_get_str(args[ARG_label].u_obj);
            }
            part = esp_partition_find_first(args[ARG_type].u_int, args[ARG_subtype].u_int, label);
            break;
        }
        case ESP32_PARTITION_BOOT:
            part = esp_ota_get_boot_partition();
            break;
        case ESP32_PARTITION_RUNNING:
            part = esp_ota_get_running_partition();
            break;
        default:
            mp_raise_ValueError(NULL);
    }

    // Return new object
    return MP_OBJ_FROM_PTR(esp32_partition_new(part));
}

STATIC mp_obj_t esp32_partition_metadata(mp_obj_t self_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t tuple[] = {
        MP_OBJ_NEW_SMALL_INT(self->part->type),
        MP_OBJ_NEW_SMALL_INT(self->part->subtype),
        mp_obj_new_int_from_uint(self->part->address),
        mp_obj_new_int_from_uint(self->part->size),
        mp_obj_new_str(&self->part->label[0], strlen(&self->part->label[0])),
        mp_obj_new_bool(self->part->encrypted),
    };
    return mp_obj_new_tuple(6, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_partition_metadata_obj, esp32_partition_metadata);

STATIC mp_obj_t esp32_partition_readblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t offset = mp_obj_get_int(block_num) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    check_esp_err(esp_partition_read(self->part, offset, bufinfo.buf, bufinfo.len));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_partition_readblocks_obj, esp32_partition_readblocks);

STATIC mp_obj_t esp32_partition_writeblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t buf_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t offset = mp_obj_get_int(block_num) * BLOCK_SIZE_BYTES;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    check_esp_err(esp_partition_write(self->part, offset, bufinfo.buf, bufinfo.len));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_partition_writeblocks_obj, esp32_partition_writeblocks);

// TODO could be an ioctl?
STATIC mp_obj_t esp32_partition_eraseblocks(mp_obj_t self_in, mp_obj_t block_num, mp_obj_t size_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t start_addr = mp_obj_get_int_truncated(block_num) * BLOCK_SIZE_BYTES;
    uint32_t size = mp_obj_get_int_truncated(size_in);
    check_esp_err(esp_partition_erase_range(self->part, start_addr, size));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_partition_eraseblocks_obj, esp32_partition_eraseblocks);

STATIC mp_obj_t esp32_partition_set_boot(mp_obj_t self_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    esp_ota_set_boot_partition(self->part);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_partition_set_boot_obj, esp32_partition_set_boot);

STATIC mp_obj_t esp32_partition_get_next_update(mp_obj_t self_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(esp32_partition_new(esp_ota_get_next_update_partition(self->part)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_partition_get_next_update_obj, esp32_partition_get_next_update);

STATIC const mp_rom_map_elem_t esp32_partition_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_metadata), MP_ROM_PTR(&esp32_partition_metadata_obj) },

    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&esp32_partition_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&esp32_partition_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_eraseblocks), MP_ROM_PTR(&esp32_partition_eraseblocks_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_boot), MP_ROM_PTR(&esp32_partition_set_boot_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_next_update), MP_ROM_PTR(&esp32_partition_get_next_update_obj) },

    { MP_ROM_QSTR(MP_QSTR_FIRST), MP_ROM_INT(ESP32_PARTITION_FIRST) },
    { MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_INT(ESP32_PARTITION_BOOT) },
    { MP_ROM_QSTR(MP_QSTR_RUNNING), MP_ROM_INT(ESP32_PARTITION_RUNNING) },

    { MP_ROM_QSTR(MP_QSTR_TYPE_APP), MP_ROM_INT(ESP_PARTITION_TYPE_APP) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_DATA), MP_ROM_INT(ESP_PARTITION_TYPE_DATA) },
};
STATIC MP_DEFINE_CONST_DICT(esp32_partition_locals_dict, esp32_partition_locals_dict_table);

const mp_obj_type_t esp32_partition_type = {
    { &mp_type_type },
    .name = MP_QSTR_Partition,
    .print = esp32_partition_print,
    .make_new = esp32_partition_make_new,
    .locals_dict = (mp_obj_dict_t*)&esp32_partition_locals_dict,
};
