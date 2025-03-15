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
#include "py/mphal.h"
#include "extmod/vfs.h"
#include "modesp32.h"
#include "esp_ota_ops.h"

// esp_partition_read and esp_partition_write can operate on arbitrary bytes
// but esp_partition_erase_range operates on 4k blocks.  The default block size
// for a Partition object is therefore 4k, to make writes efficient, and also
// make it work well with filesystems like littlefs.  The Partition object also
// supports smaller block sizes, in which case a cache is used and writes may
// be less efficient.
#define NATIVE_BLOCK_SIZE_BYTES (4096)

enum {
    ESP32_PARTITION_BOOT,
    ESP32_PARTITION_RUNNING,
};

typedef struct _esp32_partition_obj_t {
    mp_obj_base_t base;
    const esp_partition_t *part;
    uint8_t *cache;
    uint16_t block_size;
} esp32_partition_obj_t;

static esp32_partition_obj_t *esp32_partition_new(const esp_partition_t *part, uint16_t block_size) {
    if (part == NULL) {
        mp_raise_OSError(MP_ENOENT);
    }
    esp32_partition_obj_t *self = mp_obj_malloc(esp32_partition_obj_t, &esp32_partition_type);
    self->part = part;
    self->block_size = block_size;
    if (self->block_size < NATIVE_BLOCK_SIZE_BYTES) {
        self->cache = m_new(uint8_t, NATIVE_BLOCK_SIZE_BYTES);
    } else {
        self->cache = NULL;
    }
    return self;
}

static void esp32_partition_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<Partition type=%u, subtype=%u, address=%u, size=%u, label=%s, encrypted=%u>",
        self->part->type, self->part->subtype,
        self->part->address, self->part->size,
        &self->part->label[0], self->part->encrypted
        );
}

static mp_obj_t esp32_partition_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check args
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

    // Get requested partition
    const esp_partition_t *part;
    if (mp_obj_is_int(all_args[0])) {
        // Integer given, get that particular partition
        switch (mp_obj_get_int(all_args[0])) {
            case ESP32_PARTITION_BOOT:
                part = esp_ota_get_boot_partition();
                break;
            case ESP32_PARTITION_RUNNING:
                part = esp_ota_get_running_partition();
                break;
            default:
                mp_raise_ValueError(NULL);
        }
    } else {
        // String given, search for partition with that label
        const char *label = mp_obj_str_get_str(all_args[0]);
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, label);
        if (part == NULL) {
            part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, label);
        }
    }

    // Get block size if given
    uint16_t block_size = NATIVE_BLOCK_SIZE_BYTES;
    if (n_args == 2) {
        block_size = mp_obj_get_int(all_args[1]);
    }

    // Return new object
    return MP_OBJ_FROM_PTR(esp32_partition_new(part, block_size));
}

static mp_obj_t esp32_partition_find(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // Parse args
    enum { ARG_type, ARG_subtype, ARG_label, ARG_block_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_type, MP_ARG_INT, {.u_int = ESP_PARTITION_TYPE_APP} },
        { MP_QSTR_subtype, MP_ARG_INT, {.u_int = ESP_PARTITION_SUBTYPE_ANY} },
        { MP_QSTR_label, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_block_size, MP_ARG_INT, {.u_int = NATIVE_BLOCK_SIZE_BYTES} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get optional label string
    const char *label = NULL;
    if (args[ARG_label].u_obj != mp_const_none) {
        label = mp_obj_str_get_str(args[ARG_label].u_obj);
    }

    // Get block size
    uint16_t block_size = args[ARG_block_size].u_int;

    // Build list of matching partitions
    mp_obj_t list = mp_obj_new_list(0, NULL);
    esp_partition_iterator_t iter = esp_partition_find(args[ARG_type].u_int, args[ARG_subtype].u_int, label);
    while (iter != NULL) {
        mp_obj_list_append(list, MP_OBJ_FROM_PTR(esp32_partition_new(esp_partition_get(iter), block_size)));
        iter = esp_partition_next(iter);
    }
    esp_partition_iterator_release(iter);

    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(esp32_partition_find_fun_obj, 0, esp32_partition_find);
static MP_DEFINE_CONST_STATICMETHOD_OBJ(esp32_partition_find_obj, MP_ROM_PTR(&esp32_partition_find_fun_obj));

static mp_obj_t esp32_partition_info(mp_obj_t self_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t tuple[] = {
        MP_OBJ_NEW_SMALL_INT(self->part->type),
        MP_OBJ_NEW_SMALL_INT(self->part->subtype),
        mp_obj_new_int_from_uint(self->part->address),
        mp_obj_new_int_from_uint(self->part->size),
        mp_obj_new_str_from_cstr(&self->part->label[0]),
        mp_obj_new_bool(self->part->encrypted),
    };
    return mp_obj_new_tuple(6, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_partition_info_obj, esp32_partition_info);

static mp_obj_t esp32_partition_readblocks(size_t n_args, const mp_obj_t *args) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * self->block_size;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }
    check_esp_err(esp_partition_read(self->part, offset, bufinfo.buf, bufinfo.len));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_partition_readblocks_obj, 3, 4, esp32_partition_readblocks);

static mp_obj_t esp32_partition_writeblocks(size_t n_args, const mp_obj_t *args) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * self->block_size;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    if (n_args == 3) {
        // A simple write, which requires erasing first.
        if (self->block_size >= NATIVE_BLOCK_SIZE_BYTES) {
            // Block size is at least native erase-page size, so do an efficient erase.
            check_esp_err(esp_partition_erase_range(self->part, offset, bufinfo.len));
        } else {
            // Block size is less than native erase-page size, so do erase in sections.
            uint32_t addr = (offset / NATIVE_BLOCK_SIZE_BYTES) * NATIVE_BLOCK_SIZE_BYTES;
            uint32_t o = offset % NATIVE_BLOCK_SIZE_BYTES;
            uint32_t top_addr = offset + bufinfo.len;
            while (addr < top_addr) {
                if (o > 0 || top_addr < addr + NATIVE_BLOCK_SIZE_BYTES) {
                    check_esp_err(esp_partition_read(self->part, addr, self->cache, NATIVE_BLOCK_SIZE_BYTES));
                }
                check_esp_err(esp_partition_erase_range(self->part, addr, NATIVE_BLOCK_SIZE_BYTES));
                if (o > 0) {
                    check_esp_err(esp_partition_write(self->part, addr, self->cache, o));
                }
                if (top_addr < addr + NATIVE_BLOCK_SIZE_BYTES) {
                    check_esp_err(esp_partition_write(self->part, top_addr, self->cache + (top_addr - addr), addr + NATIVE_BLOCK_SIZE_BYTES - top_addr));
                }
                o = 0;
                addr += NATIVE_BLOCK_SIZE_BYTES;
            }
        }
    } else {
        // An extended write, erasing must have been done explicitly before this write.
        offset += mp_obj_get_int(args[3]);
    }
    check_esp_err(esp_partition_write(self->part, offset, bufinfo.buf, bufinfo.len));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_partition_writeblocks_obj, 3, 4, esp32_partition_writeblocks);

static mp_obj_t esp32_partition_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_DEINIT:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC:
            return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            return MP_OBJ_NEW_SMALL_INT(self->part->size / self->block_size);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            return MP_OBJ_NEW_SMALL_INT(self->block_size);
        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            if (self->block_size != NATIVE_BLOCK_SIZE_BYTES) {
                return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
            }
            uint32_t offset = mp_obj_get_int(arg_in) * NATIVE_BLOCK_SIZE_BYTES;
            check_esp_err(esp_partition_erase_range(self->part, offset, NATIVE_BLOCK_SIZE_BYTES));
            return MP_OBJ_NEW_SMALL_INT(0);
        }
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(esp32_partition_ioctl_obj, esp32_partition_ioctl);

static mp_obj_t esp32_partition_set_boot(mp_obj_t self_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_esp_err(esp_ota_set_boot_partition(self->part));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_partition_set_boot_obj, esp32_partition_set_boot);

static mp_obj_t esp32_partition_get_next_update(mp_obj_t self_in) {
    esp32_partition_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(esp32_partition_new(esp_ota_get_next_update_partition(self->part), NATIVE_BLOCK_SIZE_BYTES));
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_partition_get_next_update_obj, esp32_partition_get_next_update);

static mp_obj_t esp32_partition_mark_app_valid_cancel_rollback(mp_obj_t cls_in) {
    check_esp_err(esp_ota_mark_app_valid_cancel_rollback());
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(esp32_partition_mark_app_valid_cancel_rollback_fun_obj,
    esp32_partition_mark_app_valid_cancel_rollback);
static MP_DEFINE_CONST_CLASSMETHOD_OBJ(esp32_partition_mark_app_valid_cancel_rollback_obj,
    MP_ROM_PTR(&esp32_partition_mark_app_valid_cancel_rollback_fun_obj));

static const mp_rom_map_elem_t esp32_partition_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_find), MP_ROM_PTR(&esp32_partition_find_obj) },

    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&esp32_partition_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&esp32_partition_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&esp32_partition_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&esp32_partition_ioctl_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_boot), MP_ROM_PTR(&esp32_partition_set_boot_obj) },
    { MP_ROM_QSTR(MP_QSTR_mark_app_valid_cancel_rollback), MP_ROM_PTR(&esp32_partition_mark_app_valid_cancel_rollback_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_next_update), MP_ROM_PTR(&esp32_partition_get_next_update_obj) },

    { MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_INT(ESP32_PARTITION_BOOT) },
    { MP_ROM_QSTR(MP_QSTR_RUNNING), MP_ROM_INT(ESP32_PARTITION_RUNNING) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_APP), MP_ROM_INT(ESP_PARTITION_TYPE_APP) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_DATA), MP_ROM_INT(ESP_PARTITION_TYPE_DATA) },
};
static MP_DEFINE_CONST_DICT(esp32_partition_locals_dict, esp32_partition_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_partition_type,
    MP_QSTR_Partition,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_partition_make_new,
    print, esp32_partition_print,
    locals_dict, &esp32_partition_locals_dict
    );
