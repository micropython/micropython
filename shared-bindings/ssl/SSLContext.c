/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "shared-bindings/ssl/SSLContext.h"

//| class SSLContext:
//|     """Settings related to SSL that can be applied to a socket by wrapping it.
//|        This is useful to provide SSL certificates to specific connections
//|        rather than all of them."""
//|

STATIC mp_obj_t ssl_sslcontext_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 0, 1, false);

    ssl_sslcontext_obj_t *s = m_new_obj(ssl_sslcontext_obj_t);
    s->base.type = &ssl_sslcontext_type;

    common_hal_ssl_sslcontext_construct(s);

    return MP_OBJ_FROM_PTR(s);
}

//| def wrap_socket(sock: socketpool.Socket, *, server_side: bool = False, server_hostname: Optional[str] = None) -> ssl.SSLSocket:
//|     """Wraps the socket into a socket-compatible class that handles SSL negotiation.
//|        The socket must be of type SOCK_STREAM."""
//|     ...
//|

STATIC mp_obj_t ssl_sslcontext_wrap_socket(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sock, ARG_server_side, ARG_server_hostname };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sock, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_server_side, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_server_hostname, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    ssl_sslcontext_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *server_hostname = mp_obj_str_get_str(args[ARG_server_hostname].u_obj);
    bool server_side = args[ARG_server_side].u_bool;
    if (server_side && server_hostname != NULL) {
        mp_raise_ValueError(translate("Server side context cannot have hostname"));
    }

    socketpool_socket_obj_t *sock = args[ARG_sock].u_obj;

    return common_hal_ssl_sslcontext_wrap_socket(self, sock, server_side, server_hostname);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ssl_sslcontext_wrap_socket_obj, 2, ssl_sslcontext_wrap_socket);

STATIC const mp_rom_map_elem_t ssl_sslcontext_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_wrap_socket), MP_ROM_PTR(&ssl_sslcontext_wrap_socket_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ssl_sslcontext_locals_dict, ssl_sslcontext_locals_dict_table);

const mp_obj_type_t ssl_sslcontext_type = {
    { &mp_type_type },
    .name = MP_QSTR_SSLContext,
    .make_new = ssl_sslcontext_make_new,
    .locals_dict = (mp_obj_dict_t *)&ssl_sslcontext_locals_dict,
};
