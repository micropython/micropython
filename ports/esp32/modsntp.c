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

// SNTP service singleton object
typedef struct modsntp_obj_t {
    mp_obj_base_t base;
    uint32_t poll;   // polling frequency in seconds
    uint32_t smooth; // smooth update threshold in milliseconds
} modsntp_obj_t;
STATIC mp_obj_t modsntp_obj = NULL;

STATIC mp_obj_t modsntp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    // return singleton
    if (modsntp_obj == NULL) {
        modsntp_obj = m_new_obj(modsntp_obj_t);
        modsntp_obj.poll = 0;
        modsntp_obj.smooth = 0;
    }
    return modsntp_obj;
}

// modsntp_start starts the SNTP service by stopping it, confguring it, and starting it.
// The LwIP SNTP service has a number of config knobs and the code here makes some assumptions...
// Two in particular are that SNTP_SERVER_DNS is enabled and SNTP_MAX_SERVERS==1.
// It would be easy to pass an array of servers into modsntp_start but that makes getting good
// status out in the status() method much more complicated and in the end good status is
// probably more useful 'cause the app can always restart SNTP with a different server if it
// doesn't converge.
STATIC mp_obj_t modsntp_start(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_server, ARG_poll, ARG_smooth }
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_server,  MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_poll,  MP_ARG_INT, {.u_int = 3600} },
        { MP_QSTR_smooth,  MP_ARG_INT, {.u_int = 600} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    sntp_stop();

    sntp_setservername(0, mp_obj_str_get_str(servers[i]));
    sntp_setoperatingmode(args[ARG_poll].u_bool ? SNTP_OPMODE_POLL : SNTP_OPMODE_LISTENONLY);
    sntp_set_sync_mode(args[ARG_smooth].u_bool ? SNTP_SYNC_MODE_SMOOTH : SNTP_SYNC_MODE_IMMED);

    sntp_init();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(modsntp_start_obj, 0, modsntp_start);

STATIC mp_obj_t modsntp_stop(void) {
    sntp_stop();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modsntp_stop_obj, modsntp_stop);

STATIC mp_obj_t modsntp_status(void) {
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modsntp_status_obj, modsntp_status);

STATIC const mp_rom_map_elem_t sntp_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&modsntp_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&modsntp_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&modsntp_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_STOPPED), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_INSYNC), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_ADJUSTING), MP_ROM_INT(2) },
};
STATIC MP_DEFINE_CONST_DICT(sntp_locals_dict, sntp_locals_dict_table);

const mp_obj_type_t network_sntp_type = {
    { &mp_type_type },
    .name = MP_QSTR_SNTP,
    .make_new = modsntp_make_new,
    .locals_dict = (mp_obj_t)&sntp_locals_dict,
};
