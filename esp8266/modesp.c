/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/gc.h"
#include "py/runtime.h"
#include MICROPY_HAL_H
#include "netutils.h"
#include "queue.h"
#include "user_interface.h"
#include "espconn.h"
#include "ip_addr.h"
#include "spi_flash.h"
#include "utils.h"

STATIC mp_obj_t esp_deepsleep(mp_uint_t n_args, const mp_obj_t *args) {
    system_deep_sleep(n_args > 0 ? mp_obj_get_int(args[0]) : 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_deepsleep_obj, 0, 1, esp_deepsleep);

STATIC mp_obj_t esp_flash_id() {
    return mp_obj_new_int(spi_flash_get_id());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_id_obj, esp_flash_id);

extern const mp_obj_type_t esp_socket_type;
extern const mp_obj_module_t esp_wifi_module;

STATIC const mp_map_elem_t esp_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_esp) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_deepsleep), (mp_obj_t)&esp_deepsleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flash_id), (mp_obj_t)&esp_flash_id_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&esp_socket_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_wifi), (mp_obj_t)&esp_wifi_module },
};

STATIC MP_DEFINE_CONST_DICT(esp_module_globals, esp_module_globals_table);

const mp_obj_module_t esp_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_esp,
    .globals = (mp_obj_dict_t*)&esp_module_globals,
};
