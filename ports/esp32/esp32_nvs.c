/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 by Thorsten von Eicken
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
#include "mphalport.h"
#include "modesp32.h"
#include "nvs_flash.h"
#include "nvs.h"

// This file implements the NVS (Non-Volatile Storage) class in the esp32 module.
// It provides simple access to the NVS feature provided by ESP-IDF.

// NVS python object that represents an NVS namespace.
typedef struct _esp32_nvs_obj_t {
    mp_obj_base_t base;
    nvs_handle_t namespace;
} esp32_nvs_obj_t;

// *esp32_nvs_new allocates a python NVS object given a handle to an esp-idf namespace C obj.
STATIC esp32_nvs_obj_t *esp32_nvs_new(nvs_handle_t namespace) {
    esp32_nvs_obj_t *self = mp_obj_malloc(esp32_nvs_obj_t, &esp32_nvs_type);
    self->namespace = namespace;
    return self;
}

// esp32_nvs_print prints an NVS object, unfortunately it doesn't seem possible to extract the
// namespace string or anything else from the opaque handle provided by esp-idf.
STATIC void esp32_nvs_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    // esp32_nvs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<NVS namespace>");
}

// esp32_nvs_make_new constructs a handle to an NVS namespace.
STATIC mp_obj_t esp32_nvs_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check args
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // Get requested nvs namespace
    const char *ns_name = mp_obj_str_get_str(all_args[0]);
    nvs_handle_t namespace;
    check_esp_err(nvs_open(ns_name, NVS_READWRITE, &namespace));
    return MP_OBJ_FROM_PTR(esp32_nvs_new(namespace));
}

// esp32_nvs_set_i32 sets a 32-bit integer value
STATIC mp_obj_t esp32_nvs_set_i32(mp_obj_t self_in, mp_obj_t key_in, mp_obj_t value_in) {
    esp32_nvs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *key = mp_obj_str_get_str(key_in);
    int32_t value = mp_obj_get_int(value_in);
    check_esp_err(nvs_set_i32(self->namespace, key, value));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_nvs_set_i32_obj, esp32_nvs_set_i32);

// esp32_nvs_get_i32 reads a 32-bit integer value
STATIC mp_obj_t esp32_nvs_get_i32(mp_obj_t self_in, mp_obj_t key_in) {
    esp32_nvs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *key = mp_obj_str_get_str(key_in);
    int32_t value;
    check_esp_err(nvs_get_i32(self->namespace, key, &value));
    return mp_obj_new_int(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_nvs_get_i32_obj, esp32_nvs_get_i32);

// esp32_nvs_set_blob writes a buffer object into a binary blob value.
STATIC mp_obj_t esp32_nvs_set_blob(mp_obj_t self_in, mp_obj_t key_in, mp_obj_t value_in) {
    esp32_nvs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *key = mp_obj_str_get_str(key_in);
    mp_buffer_info_t value;
    mp_get_buffer_raise(value_in, &value, MP_BUFFER_READ);
    check_esp_err(nvs_set_blob(self->namespace, key, value.buf, value.len));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_nvs_set_blob_obj, esp32_nvs_set_blob);

// esp32_nvs_get_blob reads a binary blob value into a bytearray. Returns actual length.
STATIC mp_obj_t esp32_nvs_get_blob(mp_obj_t self_in, mp_obj_t key_in, mp_obj_t value_in) {
    esp32_nvs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *key = mp_obj_str_get_str(key_in);
    // get buffer to be filled
    mp_buffer_info_t value;
    mp_get_buffer_raise(value_in, &value, MP_BUFFER_WRITE);
    size_t length = value.len;
    // fill the buffer with the value, will raise an esp-idf error if the length of
    // the provided buffer (bytearray) is too small
    check_esp_err(nvs_get_blob(self->namespace, key, value.buf, &length));
    return MP_OBJ_NEW_SMALL_INT(length);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp32_nvs_get_blob_obj, esp32_nvs_get_blob);

// esp32_nvs_erase_key erases one key.
STATIC mp_obj_t esp32_nvs_erase_key(mp_obj_t self_in, mp_obj_t key_in) {
    esp32_nvs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *key = mp_obj_str_get_str(key_in);
    check_esp_err(nvs_erase_key(self->namespace, key));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_nvs_erase_key_obj, esp32_nvs_erase_key);

// esp32_nvs_commit commits any changes to flash.
STATIC mp_obj_t esp32_nvs_commit(mp_obj_t self_in) {
    esp32_nvs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_esp_err(nvs_commit(self->namespace));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_nvs_commit_obj, esp32_nvs_commit);

STATIC const mp_rom_map_elem_t esp32_nvs_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_i32), MP_ROM_PTR(&esp32_nvs_get_i32_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_i32), MP_ROM_PTR(&esp32_nvs_set_i32_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_blob), MP_ROM_PTR(&esp32_nvs_get_blob_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_blob), MP_ROM_PTR(&esp32_nvs_set_blob_obj) },
    { MP_ROM_QSTR(MP_QSTR_erase_key), MP_ROM_PTR(&esp32_nvs_erase_key_obj) },
    { MP_ROM_QSTR(MP_QSTR_commit), MP_ROM_PTR(&esp32_nvs_commit_obj) },
};
STATIC MP_DEFINE_CONST_DICT(esp32_nvs_locals_dict, esp32_nvs_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32_nvs_type,
    MP_QSTR_NVS,
    MP_TYPE_FLAG_NONE,
    make_new, esp32_nvs_make_new,
    print, esp32_nvs_print,
    locals_dict, &esp32_nvs_locals_dict
    );
