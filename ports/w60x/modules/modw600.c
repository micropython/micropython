/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
 * Copyright (c) 2016 Damien P. George
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

#include "wm_include.h"
#include "wm_flash_map.h"
#include "wm_internal_flash.h"

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "lib/oofatfs/ff.h"
#include "drivers/dht/dht.h"

extern const char FirmWareVer[];
extern const char HwVer[];
extern const u8 SysCreatedDate[];
extern const u8 SysCreatedTime[];

extern int tls_fls_get_param(u8 type, void *param);

STATIC mp_obj_t w600_flash_read(mp_obj_t offset_in, mp_obj_t buf_in) {
    mp_int_t offset = mp_obj_get_int(offset_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    mp_int_t res = tls_fls_read(offset, bufinfo.buf, bufinfo.len);
    if (res != TLS_FLS_STATUS_OK) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(w600_flash_read_obj, w600_flash_read);

STATIC mp_obj_t w600_flash_write(mp_obj_t offset_in, mp_obj_t buf_in) {
    mp_int_t offset = mp_obj_get_int(offset_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    mp_int_t res = tls_fls_write(offset, bufinfo.buf, bufinfo.len);
    if (res != TLS_FLS_STATUS_OK) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(w600_flash_write_obj, w600_flash_write);

STATIC mp_obj_t w600_flash_erase(mp_obj_t sector_in) {
    mp_int_t sector = mp_obj_get_int(sector_in);
    mp_int_t res = tls_fls_erase(sector);
    if (res != TLS_FLS_STATUS_OK) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(w600_flash_erase_obj, w600_flash_erase);

STATIC mp_obj_t w600_flash_id() {
    mp_uint_t flsid = 0;
    tls_fls_get_param(TLS_FLS_PARAM_TYPE_ID, &flsid);
    return mp_obj_new_int_from_uint(flsid);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(w600_flash_id_obj, w600_flash_id);

STATIC mp_obj_t w600_flash_size(void) {
    mp_uint_t flssize = 0;
    tls_fls_get_param(TLS_FLS_PARAM_TYPE_SIZE, &flssize);
    return mp_obj_new_int_from_uint(flssize);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(w600_flash_size_obj, w600_flash_size);

STATIC mp_obj_t w600_flash_user_start(void) {
    return MP_OBJ_NEW_SMALL_INT(USER_ADDR_START);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(w600_flash_user_start_obj, w600_flash_user_start);

STATIC mp_obj_t w600_run_ftpserver(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_port, ARG_username, ARG_password };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_port, MP_ARG_INT, {.u_int = 21} },
        { MP_QSTR_username, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_password, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(0, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    int port = args[ARG_port].u_int;
    const char *username  = (args[ARG_username].u_obj == MP_OBJ_NULL) ? NULL : mp_obj_str_get_str(args[ARG_username].u_obj);
    const char *password = (args[ARG_password].u_obj == MP_OBJ_NULL) ? NULL : mp_obj_str_get_str(args[ARG_password].u_obj);
    w600_ftps_start(port, username, password);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(w600_run_ftpserver_obj, 0, w600_run_ftpserver);

STATIC mp_obj_t w600_get_version(void) {
    char buf[128];
    int len = sprintf(buf, "%s,%s,%c%x.%02x.%02x.%02x%02x,%c%x.%02x.%02x@ %s %s",
                      MICROPY_W600_VERSION,
#ifdef W60X_USE_2M_FLASH
                      "2M Flash",
#else
                      "1M Flash",
#endif
                      HwVer[0], HwVer[1], HwVer[2], HwVer[3], HwVer[4], HwVer[5],
                      FirmWareVer[0], FirmWareVer[1], FirmWareVer[2], FirmWareVer[3],
                      SysCreatedTime, SysCreatedDate);
    return mp_obj_new_str(buf, len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(w600_get_version_obj, w600_get_version);

STATIC const mp_rom_map_elem_t w600_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_w600) },

    { MP_ROM_QSTR(MP_QSTR_flash_read), MP_ROM_PTR(&w600_flash_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_write), MP_ROM_PTR(&w600_flash_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_erase), MP_ROM_PTR(&w600_flash_erase_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_id), MP_ROM_PTR(&w600_flash_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_size), MP_ROM_PTR(&w600_flash_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_user_start), MP_ROM_PTR(&w600_flash_user_start_obj) },

    { MP_ROM_QSTR(MP_QSTR_run_ftpserver), MP_ROM_PTR(&w600_run_ftpserver_obj) },

    { MP_ROM_QSTR(MP_QSTR_dht_readinto), MP_ROM_PTR(&dht_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&w600_get_version_obj) },
};

STATIC MP_DEFINE_CONST_DICT(w600_module_globals, w600_module_globals_table);

const mp_obj_module_t w600_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *) &w600_module_globals,
};

