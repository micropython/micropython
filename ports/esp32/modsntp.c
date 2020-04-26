/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 by Thorsten von Eicken
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

#include <time.h>
#include <sys/time.h>
#include <sntp.h>
#include <lwip/apps/sntp.h>
#include <esp_sntp.h>

#include "py/obj.h"
#include "py/runtime.h"

STATIC mp_obj_t sntp_make_new(void) {
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sntp_make_new_obj, sntp_make_new);

STATIC mp_obj_t sntp_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_poll,  MP_ARG_BOOL, {.u_bool = 1} },
        { MP_QSTR_slew,  MP_ARG_BOOL, {.u_bool = 1} },
        { MP_QSTR_servers,  MP_ARG_OBJ, {.u_obj = mp_obj_new_str()} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(sntp_start_obj, 0, sntp_start);

STATIC mp_obj_t sntp_stop(void) {
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sntp_stop_obj, sntp_stop);

STATIC mp_obj_t sntp_status(void) {
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sntp_status_obj, sntp_status);

STATIC const mp_rom_map_elem_t sntp_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&status_obj) },
    { MP_ROM_QSTR(MP_QSTR_STOPPED), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_INSYNC), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_ADJUSTING), MP_ROM_INT(2) },
};
STATIC MP_DEFINE_CONST_DICT(sntp_locals_dict, sntp_locals_dict_table);

const mp_obj_type_t network_sntp_type = {
    { &mp_type_type },
    .name = MP_QSTR_SNTP,
    .make_new = sntp_make_new,
    .locals_dict = (mp_obj_t)&sntp_locals_dict,
};
