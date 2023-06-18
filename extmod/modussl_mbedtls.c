/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Linaro Ltd.
 * Copyright (c) 2019 Paul Sokolovsky
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

#include "py/mpconfig.h"
#if MICROPY_PY_USSL && MICROPY_SSL_MBEDTLS

#include <stdio.h>
#include <string.h>
#include <errno.h> // needed because mp_is_nonblocking_error uses system error codes

#include "py/runtime.h"
#include "py/stream.h"
#include "py/objstr.h"

// mbedtls_time_t
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include "mbedtls/error.h"

#define MP_STREAM_POLL_RDWR (MP_STREAM_POLL_RD | MP_STREAM_POLL_WR)

typedef struct _mp_obj_ssl_socket_t {
    mp_obj_base_t base;
    mp_obj_t sock;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt cert;
    mbedtls_pk_context pkey;

    uintptr_t poll_mask; // Indicates which read or write operations the protocol needs next
    int last_error; // The last error code, if any
} mp_obj_ssl_socket_t;

struct ssl_args {
    mp_arg_val_t key;
    mp_arg_val_t cert;
    mp_arg_val_t server_side;
    mp_arg_val_t server_hostname;
    mp_arg_val_t cert_reqs;
    mp_arg_val_t cadata;
    mp_arg_val_t do_handshake;
};

STATIC const mp_obj_type_t ussl_socket_type;

#ifdef MBEDTLS_DEBUG_C
STATIC void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str) {
    (void)ctx;
    (void)level;
    mp_printf(&mp_plat_print, "DBG:%s:%04d: %s\n", file, line, str);
}
#endif

STATIC NORETURN void mbedtls_raise_error(int err) {
    // _mbedtls_ssl_send and _mbedtls_ssl_recv (below) turn positive error codes from the
    // underlying socket into negative codes to pass them through mbedtls. Here we turn them
    // positive again so they get interpreted as the OSError they really are. The
    // cut-off of -256 is a bit hacky, sigh.
    if (err < 0 && err > -256) {
        mp_raise_OSError(-err);
    }

    #if defined(MBEDTLS_ERROR_C)
    // Including mbedtls_strerror takes about 1.5KB due to the error strings.
    // MBEDTLS_ERROR_C is the define used by mbedtls to conditionally include mbedtls_strerror.
    // It is set/unset in the MBEDTLS_CONFIG_FILE which is defined in the Makefile.

    // Try to allocate memory for the message
    #define ERR_STR_MAX 80  // mbedtls_strerror truncates if it doesn't fit
    mp_obj_str_t *o_str = m_new_obj_maybe(mp_obj_str_t);
    byte *o_str_buf = m_new_maybe(byte, ERR_STR_MAX);
    if (o_str == NULL || o_str_buf == NULL) {
        mp_raise_OSError(err);
    }

    // print the error message into the allocated buffer
    mbedtls_strerror(err, (char *)o_str_buf, ERR_STR_MAX);
    size_t len = strlen((char *)o_str_buf);

    // Put the exception object together
    o_str->base.type = &mp_type_str;
    o_str->data = o_str_buf;
    o_str->len = len;
    o_str->hash = qstr_compute_hash(o_str->data, o_str->len);
    // raise
    mp_obj_t args[2] = { MP_OBJ_NEW_SMALL_INT(err), MP_OBJ_FROM_PTR(o_str)};
    nlr_raise(mp_obj_exception_make_new(&mp_type_OSError, 2, 0, args));
    #else
    // mbedtls is compiled without error strings so we simply return the err number
    mp_raise_OSError(err); // err is typically a large negative number
    #endif
}

STATIC int _mbedtls_ssl_send(void *ctx, const byte *buf, size_t len) {
    mp_obj_t sock = *(mp_obj_t *)ctx;

    const mp_stream_p_t *sock_stream = mp_get_stream(sock);
    int err;

    mp_uint_t out_sz = sock_stream->write(sock, buf, len, &err);
    if (out_sz == MP_STREAM_ERROR) {
        if (mp_is_nonblocking_error(err)) {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        }
        return -err; // convert an MP_ERRNO to something mbedtls passes through as error
    } else {
        return out_sz;
    }
}

// _mbedtls_ssl_recv is called by mbedtls to receive bytes from the underlying socket
STATIC int _mbedtls_ssl_recv(void *ctx, byte *buf, size_t len) {
    mp_obj_t sock = *(mp_obj_t *)ctx;

    const mp_stream_p_t *sock_stream = mp_get_stream(sock);
    int err;

    mp_uint_t out_sz = sock_stream->read(sock, buf, len, &err);
    if (out_sz == MP_STREAM_ERROR) {
        if (mp_is_nonblocking_error(err)) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }
        return -err;
    } else {
        return out_sz;
    }
}


STATIC mp_obj_ssl_socket_t *socket_new(mp_obj_t sock, struct ssl_args *args) {
    // Verify the socket object has the full stream protocol
    mp_get_stream_raise(sock, MP_STREAM_OP_READ | MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);

    #if MICROPY_PY_USSL_FINALISER
    mp_obj_ssl_socket_t *o = m_new_obj_with_finaliser(mp_obj_ssl_socket_t);
    #else
    mp_obj_ssl_socket_t *o = m_new_obj(mp_obj_ssl_socket_t);
    #endif
    o->base.type = &ussl_socket_type;
    o->sock = sock;
    o->poll_mask = 0;
    o->last_error = 0;

    int ret;
    mbedtls_ssl_init(&o->ssl);
    mbedtls_ssl_config_init(&o->conf);
    mbedtls_x509_crt_init(&o->cacert);
    mbedtls_x509_crt_init(&o->cert);
    mbedtls_pk_init(&o->pkey);
    mbedtls_ctr_drbg_init(&o->ctr_drbg);
    #ifdef MBEDTLS_DEBUG_C
    // Debug level (0-4) 1=warning, 2=info, 3=debug, 4=verbose
    mbedtls_debug_set_threshold(3);
    #endif

    mbedtls_entropy_init(&o->entropy);
    const byte seed[] = "upy";
    ret = mbedtls_ctr_drbg_seed(&o->ctr_drbg, mbedtls_entropy_func, &o->entropy, seed, sizeof(seed));
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_ssl_config_defaults(&o->conf,
        args->server_side.u_bool ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        goto cleanup;
    }

    mbedtls_ssl_conf_authmode(&o->conf, args->cert_reqs.u_int);
    mbedtls_ssl_conf_rng(&o->conf, mbedtls_ctr_drbg_random, &o->ctr_drbg);
    #ifdef MBEDTLS_DEBUG_C
    mbedtls_ssl_conf_dbg(&o->conf, mbedtls_debug, NULL);
    #endif

    ret = mbedtls_ssl_setup(&o->ssl, &o->conf);
    if (ret != 0) {
        goto cleanup;
    }

    if (args->server_hostname.u_obj != mp_const_none) {
        const char *sni = mp_obj_str_get_str(args->server_hostname.u_obj);
        ret = mbedtls_ssl_set_hostname(&o->ssl, sni);
        if (ret != 0) {
            goto cleanup;
        }
    }

    mbedtls_ssl_set_bio(&o->ssl, &o->sock, _mbedtls_ssl_send, _mbedtls_ssl_recv, NULL);

    if (args->key.u_obj != mp_const_none) {
        size_t key_len;
        const byte *key = (const byte *)mp_obj_str_get_data(args->key.u_obj, &key_len);
        // len should include terminating null
        ret = mbedtls_pk_parse_key(&o->pkey, key, key_len + 1, NULL, 0);
        if (ret != 0) {
            ret = MBEDTLS_ERR_PK_BAD_INPUT_DATA; // use general error for all key errors
            goto cleanup;
        }

        size_t cert_len;
        const byte *cert = (const byte *)mp_obj_str_get_data(args->cert.u_obj, &cert_len);
        // len should include terminating null
        ret = mbedtls_x509_crt_parse(&o->cert, cert, cert_len + 1);
        if (ret != 0) {
            ret = MBEDTLS_ERR_X509_BAD_INPUT_DATA; // use general error for all cert errors
            goto cleanup;
        }

        ret = mbedtls_ssl_conf_own_cert(&o->conf, &o->cert, &o->pkey);
        if (ret != 0) {
            goto cleanup;
        }
    }

    if (args->cadata.u_obj != mp_const_none) {
        size_t cacert_len;
        const byte *cacert = (const byte *)mp_obj_str_get_data(args->cadata.u_obj, &cacert_len);
        // len should include terminating null
        ret = mbedtls_x509_crt_parse(&o->cacert, cacert, cacert_len + 1);
        if (ret != 0) {
            ret = MBEDTLS_ERR_X509_BAD_INPUT_DATA; // use general error for all cert errors
            goto cleanup;
        }

        mbedtls_ssl_conf_ca_chain(&o->conf, &o->cacert, NULL);
    }

    if (args->do_handshake.u_bool) {
        while ((ret = mbedtls_ssl_handshake(&o->ssl)) != 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                goto cleanup;
            }
            #ifdef MICROPY_EVENT_POLL_HOOK
            MICROPY_EVENT_POLL_HOOK
            #endif
        }
    }

    return o;

cleanup:
    mbedtls_pk_free(&o->pkey);
    mbedtls_x509_crt_free(&o->cert);
    mbedtls_x509_crt_free(&o->cacert);
    mbedtls_ssl_free(&o->ssl);
    mbedtls_ssl_config_free(&o->conf);
    mbedtls_ctr_drbg_free(&o->ctr_drbg);
    mbedtls_entropy_free(&o->entropy);

    if (ret == MBEDTLS_ERR_SSL_ALLOC_FAILED) {
        mp_raise_OSError(MP_ENOMEM);
    } else if (ret == MBEDTLS_ERR_PK_BAD_INPUT_DATA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid key"));
    } else if (ret == MBEDTLS_ERR_X509_BAD_INPUT_DATA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid cert"));
    } else {
        mbedtls_raise_error(ret);
    }
}

STATIC mp_obj_t mod_ssl_getpeercert(mp_obj_t o_in, mp_obj_t binary_form) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(o_in);
    if (!mp_obj_is_true(binary_form)) {
        mp_raise_NotImplementedError(NULL);
    }
    const mbedtls_x509_crt *peer_cert = mbedtls_ssl_get_peer_cert(&o->ssl);
    if (peer_cert == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_bytes(peer_cert->raw.p, peer_cert->raw.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_ssl_getpeercert_obj, mod_ssl_getpeercert);

STATIC void socket_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ssl_socket_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<_SSLSocket %p>", self);
}

STATIC mp_uint_t socket_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(o_in);
    o->poll_mask = 0;

    if (o->last_error) {
        *errcode = o->last_error;
        return MP_STREAM_ERROR;
    }

    int ret = mbedtls_ssl_read(&o->ssl, buf, size);
    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        // end of stream
        return 0;
    }
    if (ret >= 0) {
        return ret;
    }
    if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
        ret = MP_EWOULDBLOCK;
    } else if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        // If handshake is not finished, read attempt may end up in protocol
        // wanting to write next handshake message. The same may happen with
        // renegotation.
        ret = MP_EWOULDBLOCK;
        o->poll_mask = MP_STREAM_POLL_WR;
    } else {
        o->last_error = ret;
    }
    *errcode = ret;
    return MP_STREAM_ERROR;
}

STATIC mp_uint_t socket_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(o_in);
    o->poll_mask = 0;

    if (o->last_error) {
        *errcode = o->last_error;
        return MP_STREAM_ERROR;
    }

    int ret = mbedtls_ssl_write(&o->ssl, buf, size);
    if (ret >= 0) {
        return ret;
    }
    if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        ret = MP_EWOULDBLOCK;
    } else if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
        // If handshake is not finished, write attempt may end up in protocol
        // wanting to read next handshake message. The same may happen with
        // renegotation.
        ret = MP_EWOULDBLOCK;
        o->poll_mask = MP_STREAM_POLL_RD;
    } else {
        o->last_error = ret;
    }
    *errcode = ret;
    return MP_STREAM_ERROR;
}

STATIC mp_obj_t socket_setblocking(mp_obj_t self_in, mp_obj_t flag_in) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(self_in);
    mp_obj_t sock = o->sock;
    mp_obj_t dest[3];
    mp_load_method(sock, MP_QSTR_setblocking, dest);
    dest[2] = flag_in;
    return mp_call_method_n_kw(1, 0, dest);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socket_setblocking_obj, socket_setblocking);

STATIC mp_uint_t socket_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_ssl_socket_t *self = MP_OBJ_TO_PTR(o_in);
    mp_uint_t ret = 0;
    uintptr_t saved_arg = 0;
    mp_obj_t sock = self->sock;
    if (sock == MP_OBJ_NULL || (request != MP_STREAM_CLOSE && self->last_error != 0)) {
        // Closed or error socket:
        return MP_STREAM_POLL_NVAL;
    }

    if (request == MP_STREAM_CLOSE) {
        self->sock = MP_OBJ_NULL;
        mbedtls_pk_free(&self->pkey);
        mbedtls_x509_crt_free(&self->cert);
        mbedtls_x509_crt_free(&self->cacert);
        mbedtls_ssl_free(&self->ssl);
        mbedtls_ssl_config_free(&self->conf);
        mbedtls_ctr_drbg_free(&self->ctr_drbg);
        mbedtls_entropy_free(&self->entropy);
    } else if (request == MP_STREAM_POLL) {
        // If the library signaled us that it needs reading or writing, only check that direction,
        // but save what the caller asked because we need to restore it later
        if (self->poll_mask && (arg & MP_STREAM_POLL_RDWR)) {
            saved_arg = arg & MP_STREAM_POLL_RDWR;
            arg = (arg & ~saved_arg) | self->poll_mask;
        }

        // Take into account that the library might have buffered data already
        int has_pending = 0;
        if (arg & MP_STREAM_POLL_RD) {
            has_pending = mbedtls_ssl_check_pending(&self->ssl);
            if (has_pending) {
                ret |= MP_STREAM_POLL_RD;
                if (arg == MP_STREAM_POLL_RD) {
                    // Shortcut if we only need to read and we have buffered data, no need to go to the underlying socket
                    return MP_STREAM_POLL_RD;
                }
            }
        }
    }

    // Pass all requests down to the underlying socket
    ret |= mp_get_stream(sock)->ioctl(sock, request, arg, errcode);

    if (request == MP_STREAM_POLL) {
        // The direction the library needed is available, return a fake result to the caller so that
        // it reenters a read or a write to allow the handshake to progress
        if (ret & self->poll_mask) {
            ret |= saved_arg;
        }
    }
    return ret;
}

STATIC const mp_rom_map_elem_t ussl_socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&socket_setblocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    #if MICROPY_PY_USSL_FINALISER
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    #endif
    #if MICROPY_UNIX_COVERAGE
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&mp_stream_ioctl_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_getpeercert), MP_ROM_PTR(&mod_ssl_getpeercert_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ussl_socket_locals_dict, ussl_socket_locals_dict_table);

STATIC const mp_stream_p_t ussl_socket_stream_p = {
    .read = socket_read,
    .write = socket_write,
    .ioctl = socket_ioctl,
};

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    ussl_socket_type,
    MP_QSTR_ussl,
    MP_TYPE_FLAG_NONE,
    // Save on qstr's, reuse same as for module
    print, socket_print,
    protocol, &ussl_socket_stream_p,
    locals_dict, &ussl_socket_locals_dict
    );

STATIC mp_obj_t mod_ssl_wrap_socket(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO: Implement more args
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_key, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_cert, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_server_side, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_server_hostname, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_cert_reqs, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MBEDTLS_SSL_VERIFY_NONE}},
        { MP_QSTR_cadata, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_do_handshake, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };

    // TODO: Check that sock implements stream protocol
    mp_obj_t sock = pos_args[0];

    struct ssl_args args;
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t *)&args);

    return MP_OBJ_FROM_PTR(socket_new(sock, &args));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mod_ssl_wrap_socket_obj, 1, mod_ssl_wrap_socket);

STATIC const mp_rom_map_elem_t mp_module_ssl_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ussl) },
    { MP_ROM_QSTR(MP_QSTR_wrap_socket), MP_ROM_PTR(&mod_ssl_wrap_socket_obj) },
    { MP_ROM_QSTR(MP_QSTR_CERT_NONE), MP_ROM_INT(MBEDTLS_SSL_VERIFY_NONE) },
    { MP_ROM_QSTR(MP_QSTR_CERT_OPTIONAL), MP_ROM_INT(MBEDTLS_SSL_VERIFY_OPTIONAL) },
    { MP_ROM_QSTR(MP_QSTR_CERT_REQUIRED), MP_ROM_INT(MBEDTLS_SSL_VERIFY_REQUIRED) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ssl_globals, mp_module_ssl_globals_table);

const mp_obj_module_t mp_module_ussl = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_ssl_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ussl, mp_module_ussl);

#endif // MICROPY_PY_USSL && MICROPY_SSL_MBEDTLS
