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

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_PY_USSL

#include "ssl.h"

typedef struct _mp_obj_ssl_socket_t {
    mp_obj_base_t base;
    mp_obj_t sock;
    SSL_CTX *ssl_ctx;
    SSL *ssl_sock;
    byte *buf;
    uint32_t bytes_left;
} mp_obj_ssl_socket_t;

STATIC const mp_obj_type_t ussl_socket_type;

STATIC mp_obj_ssl_socket_t *socket_new(mp_obj_t sock) {
    mp_obj_ssl_socket_t *o = m_new_obj(mp_obj_ssl_socket_t);
    o->base.type = &ussl_socket_type;
    o->buf = NULL;
    o->bytes_left = 0;
    o->sock = sock;

    uint32_t options = SSL_SERVER_VERIFY_LATER;
    if ((o->ssl_ctx = ssl_ctx_new(options, SSL_DEFAULT_CLNT_SESS)) == NULL)
    {
        fprintf(stderr, "Error: Client context is invalid\n");
        assert(0);
    }

    o->ssl_sock = ssl_client_new(o->ssl_ctx, (long)sock, NULL, 0);

    int res;
    /* check the return status */
    if ((res = ssl_handshake_status(o->ssl_sock)) != SSL_OK)
    {
        printf("ssl_handshake_status: %d\n", res);
        ssl_display_error(res);
        assert(0);
    }

    return o;
}

STATIC void socket_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ssl_socket_t *self = self_in;
    mp_printf(print, "<_SSLSocket %p>", self->ssl_sock);
}

STATIC mp_uint_t socket_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_ssl_socket_t *o = o_in;

    while (o->bytes_left == 0) {
        mp_int_t r = ssl_read(o->ssl_sock, &o->buf);
        if (r < 0) {
            if (r == SSL_CLOSE_NOTIFY || r == SSL_ERROR_CONN_LOST) {
                // EOF
                return 0;
            }
            *errcode = r;
            return MP_STREAM_ERROR;
        }
        o->bytes_left = r;
    }

    if (size > o->bytes_left) {
        size = o->bytes_left;
    }
    memcpy(buf, o->buf, size);
    o->buf += size;
    o->bytes_left -= size;
    return size;
}

STATIC mp_uint_t socket_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_ssl_socket_t *o = o_in;
    mp_int_t r = ssl_write(o->ssl_sock, buf, size);
    if (r < 0) {
        *errcode = r;
        return MP_STREAM_ERROR;
    }
    return r;
}

STATIC mp_obj_t socket_close(mp_obj_t self_in) {
    mp_obj_ssl_socket_t *self = self_in;
    ssl_free(self->ssl_sock);
    ssl_ctx_free(self->ssl_ctx);

    mp_obj_t dest[2];
    mp_load_method(self->sock, MP_QSTR_close, dest);
    return mp_call_method_n_kw(0, 0, dest);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socket_close_obj, socket_close);

STATIC const mp_map_elem_t ussl_socket_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall), (mp_obj_t)&mp_stream_readall_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), (mp_obj_t)&mp_stream_readinto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&socket_close_obj },
};

STATIC MP_DEFINE_CONST_DICT(ussl_socket_locals_dict, ussl_socket_locals_dict_table);

STATIC const mp_stream_p_t ussl_socket_stream_p = {
    .read = socket_read,
    .write = socket_write,
};

STATIC const mp_obj_type_t ussl_socket_type = {
    { &mp_type_type },
    // Save on qstr's, reuse same as for module
    .name = MP_QSTR_ussl,
    .print = socket_print,
    .getiter = NULL,
    .iternext = NULL,
    .stream_p = &ussl_socket_stream_p,
    .locals_dict = (mp_obj_t)&ussl_socket_locals_dict,
};

STATIC mp_obj_t mod_ssl_wrap_socket(mp_uint_t n_args, const mp_obj_t *args) {
    // TODO: Implement more args
    assert(n_args == 1);
    mp_obj_t sock = args[0];
    // TODO: Check that sock implements stream protocol
    return socket_new(sock);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_ssl_wrap_socket_obj, 1, 6, mod_ssl_wrap_socket);

STATIC const mp_map_elem_t mp_module_ssl_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ussl) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_wrap_socket), (mp_obj_t)&mod_ssl_wrap_socket_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ssl_globals, mp_module_ssl_globals_table);

const mp_obj_module_t mp_module_ussl = {
    .base = { &mp_type_module },
    .name = MP_QSTR_ussl,
    .globals = (mp_obj_dict_t*)&mp_module_ssl_globals,
};


// These functions might be split to stream_posix.c. They are referenced by
// axtls os_port.h .

int mp_stream_errno;

ssize_t mp_stream_posix_write(void *sock_obj, const void *buf, size_t len) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)sock_obj;
    mp_uint_t out_sz = o->type->stream_p->write(o, buf, len, &mp_stream_errno);
    if (out_sz == MP_STREAM_ERROR) {
        return -1;
    } else {
        return out_sz;
    }
}

ssize_t mp_stream_posix_read(void *sock_obj, void *buf, size_t len) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)sock_obj;
    mp_uint_t out_sz = o->type->stream_p->read(o, buf, len, &mp_stream_errno);
    if (out_sz == MP_STREAM_ERROR) {
        return -1;
    } else {
        return out_sz;
    }
}

#endif // MICROPY_PY_USSL
