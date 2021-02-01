/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include <stdint.h>

#include "simplelink.h"
#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "modnetwork.h"
#include "modusocket.h"

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define SSL_CERT_NONE                          (0)
#define SSL_CERT_OPTIONAL                      (1)
#define SSL_CERT_REQUIRED                      (2)

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct _mp_obj_ssl_socket_t {
    mp_obj_base_t base;
    mod_network_socket_base_t sock_base;
    mp_obj_t o_sock;
} mp_obj_ssl_socket_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC const mp_obj_type_t ssl_socket_type;

/******************************************************************************/
// MicroPython bindings; SSL class

// ssl sockets inherit from normal socket, so we take its
// locals and stream methods
STATIC const mp_obj_type_t ssl_socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_ussl,
    .getiter = NULL,
    .iternext = NULL,
    .protocol = &socket_stream_p,
    .locals_dict = (mp_obj_t)&socket_locals_dict,
};

STATIC mp_obj_t mod_ssl_wrap_socket(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_sock,             MP_ARG_REQUIRED | MP_ARG_OBJ,  },
        { MP_QSTR_keyfile,          MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_certfile,         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_server_side,      MP_ARG_KW_ONLY  | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_cert_reqs,        MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = SSL_CERT_NONE} },
        { MP_QSTR_ssl_version,      MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = SL_SO_SEC_METHOD_TLSV1} },
        { MP_QSTR_ca_certs,         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
    };

    // parse arguments
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // check if ca validation is required
    if (args[4].u_int != SSL_CERT_NONE && args[6].u_obj == mp_const_none) {
        goto arg_error;
    }

    // retrieve the file paths (with an 6 byte offset in order to strip it from the '/flash' prefix)
    const char *keyfile  = (args[1].u_obj == mp_const_none) ? NULL : &(mp_obj_str_get_str(args[1].u_obj)[6]);
    const char *certfile = (args[2].u_obj == mp_const_none) ? NULL : &(mp_obj_str_get_str(args[2].u_obj)[6]);
    const char *cafile   = (args[6].u_obj == mp_const_none || args[4].u_int != SSL_CERT_REQUIRED) ?
                           NULL : &(mp_obj_str_get_str(args[6].u_obj)[6]);

    // server side requires both certfile and keyfile
    if (args[3].u_bool && (!keyfile || !certfile)) {
        goto arg_error;
    }

    _i16 _errno;
    _i16 sd = ((mod_network_socket_obj_t *)args[0].u_obj)->sock_base.sd;

    // set the requested SSL method
    _u8 method = args[5].u_int;
    if ((_errno = sl_SetSockOpt(sd, SL_SOL_SOCKET, SL_SO_SECMETHOD, &method, sizeof(method))) < 0) {
        goto socket_error;
    }
    if (keyfile && (_errno = sl_SetSockOpt(sd, SL_SOL_SOCKET, SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME, keyfile, strlen(keyfile))) < 0) {
        goto socket_error;
    }
    if (certfile && (_errno = sl_SetSockOpt(sd, SL_SOL_SOCKET, SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME, certfile, strlen(certfile))) < 0) {
        goto socket_error;
    }
    if (cafile && (_errno = sl_SetSockOpt(sd, SL_SOL_SOCKET, SL_SO_SECURE_FILES_CA_FILE_NAME, cafile, strlen(cafile))) < 0) {
        goto socket_error;
    }

    // create the ssl socket
    mp_obj_ssl_socket_t *ssl_sock = m_new_obj(mp_obj_ssl_socket_t);
    // ssl sockets inherit all properties from the original socket
    memcpy (&ssl_sock->sock_base, &((mod_network_socket_obj_t *)args[0].u_obj)->sock_base, sizeof(mod_network_socket_base_t));
    ssl_sock->base.type = &ssl_socket_type;
    ssl_sock->sock_base.cert_req = (args[4].u_int == SSL_CERT_REQUIRED) ? true : false;
    ssl_sock->o_sock = args[0].u_obj;

    return ssl_sock;

socket_error:
    mp_raise_OSError(_errno);

arg_error:
    mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mod_ssl_wrap_socket_obj, 0, mod_ssl_wrap_socket);

STATIC const mp_rom_map_elem_t mp_module_ussl_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_ussl) },
    { MP_ROM_QSTR(MP_QSTR_wrap_socket),         MP_ROM_PTR(&mod_ssl_wrap_socket_obj) },

    // class exceptions
    { MP_ROM_QSTR(MP_QSTR_SSLError),            MP_ROM_PTR(&mp_type_OSError) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_CERT_NONE),           MP_ROM_INT(SSL_CERT_NONE) },
    { MP_ROM_QSTR(MP_QSTR_CERT_OPTIONAL),       MP_ROM_INT(SSL_CERT_OPTIONAL) },
    { MP_ROM_QSTR(MP_QSTR_CERT_REQUIRED),       MP_ROM_INT(SSL_CERT_REQUIRED) },

    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_SSLv3),      MP_ROM_INT(SL_SO_SEC_METHOD_SSLV3) },
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_TLSv1),      MP_ROM_INT(SL_SO_SEC_METHOD_TLSV1) },
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_TLSv1_1),    MP_ROM_INT(SL_SO_SEC_METHOD_TLSV1_1) },
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_TLSv1_2),    MP_ROM_INT(SL_SO_SEC_METHOD_TLSV1_2) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ussl_globals, mp_module_ussl_globals_table);

const mp_obj_module_t mp_module_ussl = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ussl_globals,
};

