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

#include "extmod/vfs.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "shared-bindings/ssl/SSLContext.h"

//| class SSLContext:
//|     """Settings related to SSL that can be applied to a socket by wrapping it.
//|     This is useful to provide SSL certificates to specific connections
//|     rather than all of them."""
//|

STATIC mp_obj_t ssl_sslcontext_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    ssl_sslcontext_obj_t *s = mp_obj_malloc(ssl_sslcontext_obj_t, &ssl_sslcontext_type);

    common_hal_ssl_sslcontext_construct(s);

    return MP_OBJ_FROM_PTR(s);
}

//|     def load_cert_chain(self, certfile: str, keyfile: str) -> None:
//|         """Load a private key and the corresponding certificate.
//|
//|         The certfile string must be the path to a single file in PEM format
//|         containing the certificate as well as any number of CA certificates
//|         needed to establish the certificate's authenticity.  The keyfile string
//|         must point to a file containing the private key.
//|         """

STATIC void get_file_contents(mp_obj_t name_obj, mp_buffer_info_t *bufinfo) {
    mp_obj_t file = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), name_obj, MP_OBJ_NEW_QSTR(MP_QSTR_rb));
    mp_obj_t dest[2];
    mp_load_method(file, MP_QSTR_read, dest);
    mp_obj_t result = mp_call_method_n_kw(0, 0, dest);
    mp_get_buffer_raise(result, bufinfo, MP_BUFFER_READ);
}

STATIC mp_obj_t ssl_sslcontext_load_cert_chain(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_certfile, ARG_keyfile };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_certfile, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_keyfile, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    ssl_sslcontext_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t cert_buf, key_buf;
    get_file_contents(args[ARG_certfile].u_obj, &cert_buf);
    if (args[ARG_keyfile].u_obj != mp_const_none) {
        get_file_contents(args[ARG_keyfile].u_obj, &key_buf);
    } else {
        key_buf = cert_buf;
    }

    common_hal_ssl_sslcontext_load_cert_chain(self, &cert_buf, &key_buf);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ssl_sslcontext_load_cert_chain_obj, 1, ssl_sslcontext_load_cert_chain);

//|     def load_verify_locations(
//|         self,
//|         cafile: Optional[str] = None,
//|         capath: Optional[str] = None,
//|         cadata: Optional[str] = None,
//|     ) -> None:
//|         """
//|         Load a set of certification authority (CA) certificates used to validate
//|         other peers' certificates.
//|
//|         :param str cafile: path to a file of contcatenated CA certificates in PEM format. **Not implemented**.
//|         :param str capath: path to a directory of CA certificate files in PEM format. **Not implemented**.
//|         :param str cadata: A single CA certificate in PEM format. **Limitation**: CPython allows one
//|           or more certificates, but this implementation is limited to one.
//|         """
STATIC mp_obj_t ssl_sslcontext_load_verify_locations(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_cafile, ARG_capath, ARG_cadata };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_cafile, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_capath, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_cadata, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    ssl_sslcontext_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_cafile].u_obj != mp_const_none) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_cafile);
    }

    if (args[ARG_capath].u_obj != mp_const_none) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_capath);
    }

    const char *cadata = mp_obj_str_get_str(args[ARG_cadata].u_obj);

    common_hal_ssl_sslcontext_load_verify_locations(self, cadata);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ssl_sslcontext_load_verify_locations_obj, 1, ssl_sslcontext_load_verify_locations);

//|     def set_default_verify_paths(self) -> None:
//|         """Load a set of default certification authority (CA) certificates."""

STATIC mp_obj_t ssl_sslcontext_set_default_verify_paths(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    ssl_sslcontext_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    common_hal_ssl_sslcontext_set_default_verify_paths(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ssl_sslcontext_set_default_verify_paths_obj, 1, ssl_sslcontext_set_default_verify_paths);

//|     check_hostname: bool
//|     """Whether to match the peer certificate's hostname."""

STATIC mp_obj_t ssl_sslcontext_get_check_hostname(mp_obj_t self_in) {
    ssl_sslcontext_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_ssl_sslcontext_get_check_hostname(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ssl_sslcontext_get_check_hostname_obj, ssl_sslcontext_get_check_hostname);

STATIC mp_obj_t ssl_sslcontext_set_check_hostname(mp_obj_t self_in, mp_obj_t value) {
    ssl_sslcontext_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_ssl_sslcontext_set_check_hostname(self, mp_obj_is_true(value));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ssl_sslcontext_set_check_hostname_obj, ssl_sslcontext_set_check_hostname);

MP_PROPERTY_GETSET(ssl_sslcontext_check_hostname_obj,
    (mp_obj_t)&ssl_sslcontext_get_check_hostname_obj,
    (mp_obj_t)&ssl_sslcontext_set_check_hostname_obj);

//|     def wrap_socket(
//|         self,
//|         sock: socketpool.Socket,
//|         *,
//|         server_side: bool = False,
//|         server_hostname: Optional[str] = None
//|     ) -> ssl.SSLSocket:
//|         """Wraps the socket into a socket-compatible class that handles SSL negotiation.
//|         The socket must be of type SOCK_STREAM."""
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

    const char *server_hostname = NULL;
    if (args[ARG_server_hostname].u_obj != mp_const_none) {
        server_hostname = mp_obj_str_get_str(args[ARG_server_hostname].u_obj);
    }
    bool server_side = args[ARG_server_side].u_bool;
    if (server_side && server_hostname != NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("Server side context cannot have hostname"));
    }

    socketpool_socket_obj_t *sock = args[ARG_sock].u_obj;

    return common_hal_ssl_sslcontext_wrap_socket(self, sock, server_side, server_hostname);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ssl_sslcontext_wrap_socket_obj, 1, ssl_sslcontext_wrap_socket);

STATIC const mp_rom_map_elem_t ssl_sslcontext_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_wrap_socket), MP_ROM_PTR(&ssl_sslcontext_wrap_socket_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_cert_chain), MP_ROM_PTR(&ssl_sslcontext_load_cert_chain_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_verify_locations), MP_ROM_PTR(&ssl_sslcontext_load_verify_locations_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_default_verify_paths), MP_ROM_PTR(&ssl_sslcontext_set_default_verify_paths_obj) },
    { MP_ROM_QSTR(MP_QSTR_check_hostname), MP_ROM_PTR(&ssl_sslcontext_check_hostname_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ssl_sslcontext_locals_dict, ssl_sslcontext_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    ssl_sslcontext_type,
    MP_QSTR_SSLContext,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, ssl_sslcontext_make_new,
    locals_dict, &ssl_sslcontext_locals_dict
    );
