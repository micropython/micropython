/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include <std.h>

#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "modnetwork.h"
#include "mpexception.h"
#include "serverstask.h"
#include "simplelink.h"


/// \module network - network configuration
///
/// This module provides network drivers and server configuration.

void mod_network_init0(void) {
}

#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
STATIC mp_obj_t network_server_running(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args > 0) {
        // set
        if (mp_obj_is_true(args[0])) {
            servers_start();
        } else {
            servers_stop();
        }
        return mp_const_none;
    } else {
        // get
        return MP_BOOL(servers_are_enabled());
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_server_running_obj, 0, 1, network_server_running);

STATIC mp_obj_t network_server_login(mp_obj_t user, mp_obj_t pass) {
    const char *_user = mp_obj_str_get_str(user);
    const char *_pass = mp_obj_str_get_str(pass);
    if (strlen(user) > SERVERS_USER_PASS_LEN_MAX || strlen(pass) > SERVERS_USER_PASS_LEN_MAX) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
    servers_set_login ((char *)_user, (char *)_pass);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(network_server_login_obj, network_server_login);

// timeout value given in seconds
STATIC mp_obj_t network_server_timeout(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args > 0) {
        uint32_t _timeout = mp_obj_get_int(args[0]);
        if (!servers_set_timeout(_timeout * 1000)) {
            // timeout is too low
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
        }
        return mp_const_none;
    } else {
        // get
        return mp_obj_new_int(servers_get_timeout() / 1000);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_server_timeout_obj, 0, 1, network_server_timeout);
#endif

STATIC const mp_map_elem_t mp_module_network_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),            MP_OBJ_NEW_QSTR(MP_QSTR_network) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WLAN),                (mp_obj_t)&mod_network_nic_type_wlan },

#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    { MP_OBJ_NEW_QSTR(MP_QSTR_server_running),      (mp_obj_t)&network_server_running_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_server_login),        (mp_obj_t)&network_server_login_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_server_timeout),      (mp_obj_t)&network_server_timeout_obj },
#endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .name = MP_QSTR_network,
    .globals = (mp_obj_dict_t*)&mp_module_network_globals,
};
