// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Linaro Ltd.
// SPDX-FileCopyrightText: Copyright (c) 2019 Paul Sokolovsky
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/ssl/SSLSocket.h"
#include "shared-bindings/ssl/SSLContext.h"

#include "shared/runtime/interrupt_char.h"
#include "shared/netutils/netutils.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/tick.h"

#include "shared-bindings/socketpool/enum.h"

#include "mbedtls/version.h"

#define MP_STREAM_POLL_RDWR (MP_STREAM_POLL_RD | MP_STREAM_POLL_WR)

#if defined(MBEDTLS_ERROR_C)
#include "../../lib/mbedtls_errors/mp_mbedtls_errors.c"
#endif

#if MBEDTLS_VERSION_MAJOR >= 3
#include "shared-bindings/os/__init__.h"
#endif

#ifdef MBEDTLS_DEBUG_C
#include "mbedtls/debug.h"
static void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str) {
    (void)ctx;
    (void)level;
    mp_printf(&mp_plat_print, "DBG:%s:%04d: %s\n", file, line, str);
}
#define DEBUG_PRINT(fmt, ...) mp_printf(&mp_plat_print, "DBG:%s:%04d: " fmt "\n", __FILE__, __LINE__,##__VA_ARGS__)
#else
#define DEBUG_PRINT(...) do {} while (0)
#endif

static NORETURN void mbedtls_raise_error(int err) {
    // _mbedtls_ssl_send and _mbedtls_ssl_recv (below) turn positive error codes from the
    // underlying socket into negative codes to pass them through mbedtls. Here we turn them
    // positive again so they get interpreted as the OSError they really are. The
    // cut-off of -256 is a bit hacky, sigh.
    if (err < 0 && err > -256) {
        mp_raise_OSError(-err);
    }

    if (err == MBEDTLS_ERR_SSL_WANT_WRITE || err == MBEDTLS_ERR_SSL_WANT_READ) {
        mp_raise_OSError(MP_EWOULDBLOCK);
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

// Because ssl_socket_send and ssl_socket_recv_into are callbacks from mbedtls code,
// it is not OK to exit them by raising an exception (nlr_jump'ing through
// foreign code is not permitted). Instead, preserve the error number of any OSError
// and turn anything else into -MP_EINVAL.
static int call_method_errno(size_t n_args, const mp_obj_t *args) {
    nlr_buf_t nlr;
    mp_int_t result = -MP_EINVAL;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t obj_result = mp_call_method_n_kw(n_args, 0, args);
        result = (obj_result == mp_const_none) ? 0 : mp_obj_get_int(obj_result);
        nlr_pop();
        return result;
    } else {
        mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);
        if (nlr_push(&nlr) == 0) {
            result = -mp_obj_get_int(mp_load_attr(exc, MP_QSTR_errno));
            nlr_pop();
        }
    }
    return result;
}

static int ssl_socket_send(ssl_sslsocket_obj_t *self, const byte *buf, size_t len) {
    mp_obj_array_t mv;
    mp_obj_memoryview_init(&mv, 'B', 0, len, (void *)buf);

    self->send_args[2] = MP_OBJ_FROM_PTR(&mv);
    return call_method_errno(1, self->send_args);
}

static int ssl_socket_recv_into(ssl_sslsocket_obj_t *self, byte *buf, size_t len) {
    mp_obj_array_t mv;
    mp_obj_memoryview_init(&mv, 'B' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW, 0, len, buf);

    self->recv_into_args[2] = MP_OBJ_FROM_PTR(&mv);
    return call_method_errno(1, self->recv_into_args);
}

static void ssl_socket_connect(ssl_sslsocket_obj_t *self, mp_obj_t addr_in) {
    self->connect_args[2] = addr_in;
    mp_call_method_n_kw(1, 0, self->connect_args);
}

static void ssl_socket_bind(ssl_sslsocket_obj_t *self, mp_obj_t addr_in) {
    self->bind_args[2] = addr_in;
    mp_call_method_n_kw(1, 0, self->bind_args);
}

static void ssl_socket_close(ssl_sslsocket_obj_t *self) {
    // swallow any exception raised by the underlying close method.
    // This is not ideal. However, it avoids printing "MemoryError:"
    // when attempting to close a userspace socket object during gc_sweep_all
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_call_method_n_kw(0, 0, self->close_args);
        nlr_pop();
    } else {
        nlr_pop();
    }
}

static void ssl_socket_setsockopt(ssl_sslsocket_obj_t *self, mp_obj_t level_obj, mp_obj_t opt_obj, mp_obj_t optval_obj) {
    self->setsockopt_args[2] = level_obj;
    self->setsockopt_args[3] = opt_obj;
    self->setsockopt_args[4] = optval_obj;
    mp_call_method_n_kw(3, 0, self->setsockopt_args);
}

static void ssl_socket_settimeout(ssl_sslsocket_obj_t *self, mp_obj_t timeout_obj) {
    self->settimeout_args[2] = timeout_obj;
    mp_call_method_n_kw(1, 0, self->settimeout_args);
}

static void ssl_socket_listen(ssl_sslsocket_obj_t *self, mp_int_t backlog) {
    self->listen_args[2] = MP_OBJ_NEW_SMALL_INT(backlog);
    mp_call_method_n_kw(1, 0, self->listen_args);
}

static mp_obj_t ssl_socket_accept(ssl_sslsocket_obj_t *self) {
    return mp_call_method_n_kw(0, 0, self->accept_args);
}

static int _mbedtls_ssl_send(void *ctx, const byte *buf, size_t len) {
    ssl_sslsocket_obj_t *self = (ssl_sslsocket_obj_t *)ctx;

    mp_int_t out_sz = ssl_socket_send(self, buf, len);
    DEBUG_PRINT("socket_send() -> %d", out_sz);
    if (out_sz < 0) {
        int err = -out_sz;
        DEBUG_PRINT("sock_stream->write() -> %d nonblocking? %d", out_sz, mp_is_nonblocking_error(err));
        if (mp_is_nonblocking_error(err)) {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        }
    }
    return out_sz;
}

static int _mbedtls_ssl_recv(void *ctx, byte *buf, size_t len) {
    ssl_sslsocket_obj_t *self = (ssl_sslsocket_obj_t *)ctx;

    mp_int_t out_sz = ssl_socket_recv_into(self, buf, len);
    DEBUG_PRINT("socket_recv() -> %d", out_sz);
    if (out_sz < 0) {
        int err = -out_sz;
        if (mp_is_nonblocking_error(err)) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }
    }
    return out_sz;
}


#if MBEDTLS_VERSION_MAJOR >= 3
static int urandom_adapter(void *unused, unsigned char *buf, size_t n) {
    int result = common_hal_os_urandom(buf, n);
    if (result) {
        return 0;
    }
    return MBEDTLS_ERR_SSL_INTERNAL_ERROR;
}
#endif

ssl_sslsocket_obj_t *common_hal_ssl_sslcontext_wrap_socket(ssl_sslcontext_obj_t *self,
    mp_obj_t socket, bool server_side, const char *server_hostname) {

    mp_int_t socket_type = mp_obj_get_int(mp_load_attr(socket, MP_QSTR_type));
    if (socket_type != SOCKETPOOL_SOCK_STREAM) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid socket for TLS"));
    }

    ssl_sslsocket_obj_t *o = m_new_obj_with_finaliser(ssl_sslsocket_obj_t);
    o->base.type = &ssl_sslsocket_type;
    o->ssl_context = self;
    o->sock_obj = socket;
    o->poll_mask = 0;

    mp_load_method(socket, MP_QSTR_accept, o->accept_args);
    mp_load_method(socket, MP_QSTR_bind, o->bind_args);
    mp_load_method(socket, MP_QSTR_close, o->close_args);
    mp_load_method(socket, MP_QSTR_connect, o->connect_args);
    mp_load_method(socket, MP_QSTR_listen, o->listen_args);
    mp_load_method(socket, MP_QSTR_recv_into, o->recv_into_args);
    mp_load_method(socket, MP_QSTR_send, o->send_args);
    mp_load_method(socket, MP_QSTR_settimeout, o->settimeout_args);
    mp_load_method(socket, MP_QSTR_setsockopt, o->setsockopt_args);

    mbedtls_ssl_init(&o->ssl);
    mbedtls_ssl_config_init(&o->conf);
    mbedtls_x509_crt_init(&o->cacert);
    mbedtls_x509_crt_init(&o->cert);
    mbedtls_pk_init(&o->pkey);
    mbedtls_ctr_drbg_init(&o->ctr_drbg);
    #ifdef MBEDTLS_DEBUG_C
    // Debug level (0-4) 1=warning, 2=info, 3=debug, 4=verbose
    mbedtls_debug_set_threshold(4);
    #endif

    mbedtls_entropy_init(&o->entropy);
    const byte seed[] = "upy";
    int ret = mbedtls_ctr_drbg_seed(&o->ctr_drbg, mbedtls_entropy_func, &o->entropy, seed, sizeof(seed));
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_ssl_config_defaults(&o->conf,
        server_side ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        goto cleanup;
    }

    if (self->crt_bundle_attach != NULL) {
        mbedtls_ssl_conf_authmode(&o->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        self->crt_bundle_attach(&o->conf);
    } else if (self->cacert_buf && self->cacert_bytes) {
        ret = mbedtls_x509_crt_parse(&o->cacert, self->cacert_buf, self->cacert_bytes);
        if (ret != 0) {
            goto cleanup;
        }
        mbedtls_ssl_conf_authmode(&o->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_ca_chain(&o->conf, &o->cacert, NULL);

    } else {
        mbedtls_ssl_conf_authmode(&o->conf, MBEDTLS_SSL_VERIFY_NONE);
    }
    mbedtls_ssl_conf_rng(&o->conf, mbedtls_ctr_drbg_random, &o->ctr_drbg);
    #ifdef MBEDTLS_DEBUG_C
    mbedtls_ssl_conf_dbg(&o->conf, mbedtls_debug, NULL);
    #endif

    ret = mbedtls_ssl_setup(&o->ssl, &o->conf);
    if (ret != 0) {
        goto cleanup;
    }

    if (server_hostname != NULL) {
        ret = mbedtls_ssl_set_hostname(&o->ssl, server_hostname);
        if (ret != 0) {
            goto cleanup;
        }
    }

    mbedtls_ssl_set_bio(&o->ssl, o, _mbedtls_ssl_send, _mbedtls_ssl_recv, NULL);

    if (self->cert_buf.buf != NULL) {
        #if MBEDTLS_VERSION_MAJOR >= 3
        ret = mbedtls_pk_parse_key(&o->pkey, self->key_buf.buf, self->key_buf.len + 1, NULL, 0, urandom_adapter, NULL);
        #else
        ret = mbedtls_pk_parse_key(&o->pkey, self->key_buf.buf, self->key_buf.len + 1, NULL, 0);
        #endif
        if (ret != 0) {
            goto cleanup;
        }
        ret = mbedtls_x509_crt_parse(&o->cert, self->cert_buf.buf, self->cert_buf.len + 1);
        if (ret != 0) {
            goto cleanup;
        }

        ret = mbedtls_ssl_conf_own_cert(&o->conf, &o->cert, &o->pkey);
        if (ret != 0) {
            goto cleanup;
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
        mp_raise_type(&mp_type_MemoryError);
    } else if (ret == MBEDTLS_ERR_PK_BAD_INPUT_DATA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid key"));
    } else if (ret == MBEDTLS_ERR_X509_BAD_INPUT_DATA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid cert"));
    } else {
        mbedtls_raise_error(ret);
    }
}

mp_uint_t common_hal_ssl_sslsocket_recv_into(ssl_sslsocket_obj_t *self, uint8_t *buf, mp_uint_t len) {
    self->poll_mask = 0;
    int ret = mbedtls_ssl_read(&self->ssl, buf, len);
    DEBUG_PRINT("recv_into mbedtls_ssl_read() -> %d\n", ret);
    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        DEBUG_PRINT("returning %d\n", 0);
        // end of stream
        return 0;
    }
    if (ret >= 0) {
        DEBUG_PRINT("returning %d\n", ret);
        return ret;
    }
    if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        self->poll_mask = MP_STREAM_POLL_WR;
    }
    DEBUG_PRINT("raising errno [error case] %d\n", ret);
    mbedtls_raise_error(ret);
}

mp_uint_t common_hal_ssl_sslsocket_send(ssl_sslsocket_obj_t *self, const uint8_t *buf, mp_uint_t len) {
    self->poll_mask = 0;
    int ret = mbedtls_ssl_write(&self->ssl, buf, len);
    DEBUG_PRINT("send mbedtls_ssl_write() -> %d\n", ret);
    if (ret >= 0) {
        DEBUG_PRINT("returning %d\n", ret);
        return ret;
    }
    if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
        self->poll_mask = MP_STREAM_POLL_RD;
    }
    DEBUG_PRINT("raising errno [error case] %d\n", ret);
    mbedtls_raise_error(ret);
}

void common_hal_ssl_sslsocket_bind(ssl_sslsocket_obj_t *self, mp_obj_t addr_in) {
    ssl_socket_bind(self, addr_in);
}

void common_hal_ssl_sslsocket_close(ssl_sslsocket_obj_t *self) {
    if (self->closed) {
        return;
    }
    self->closed = true;
    ssl_socket_close(self);
    mbedtls_pk_free(&self->pkey);
    mbedtls_x509_crt_free(&self->cert);
    mbedtls_x509_crt_free(&self->cacert);
    mbedtls_ssl_free(&self->ssl);
    mbedtls_ssl_config_free(&self->conf);
    mbedtls_ctr_drbg_free(&self->ctr_drbg);
    mbedtls_entropy_free(&self->entropy);
}

static void do_handshake(ssl_sslsocket_obj_t *self) {
    int ret;
    while ((ret = mbedtls_ssl_handshake(&self->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            goto cleanup;
        }
        RUN_BACKGROUND_TASKS;
        if (MP_STATE_THREAD(mp_pending_exception) != MP_OBJ_NULL) {
            mp_handle_pending(true);
        }
        mp_hal_delay_ms(1);
    }

    return;

cleanup:
    self->closed = true;
    mbedtls_pk_free(&self->pkey);
    mbedtls_x509_crt_free(&self->cert);
    mbedtls_x509_crt_free(&self->cacert);
    mbedtls_ssl_free(&self->ssl);
    mbedtls_ssl_config_free(&self->conf);
    mbedtls_ctr_drbg_free(&self->ctr_drbg);
    mbedtls_entropy_free(&self->entropy);

    if (ret == MBEDTLS_ERR_SSL_ALLOC_FAILED) {
        mp_raise_type(&mp_type_MemoryError);
    } else if (ret == MBEDTLS_ERR_PK_BAD_INPUT_DATA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid key"));
    } else if (ret == MBEDTLS_ERR_X509_BAD_INPUT_DATA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid cert"));
    } else {
        mbedtls_raise_error(ret);
    }
}

void common_hal_ssl_sslsocket_connect(ssl_sslsocket_obj_t *self, mp_obj_t addr_in) {
    ssl_socket_connect(self, addr_in);
    do_handshake(self);
}

bool common_hal_ssl_sslsocket_get_closed(ssl_sslsocket_obj_t *self) {
    return self->closed;
}

bool common_hal_ssl_sslsocket_get_connected(ssl_sslsocket_obj_t *self) {
    return !self->closed;
}

void common_hal_ssl_sslsocket_listen(ssl_sslsocket_obj_t *self, int backlog) {
    return ssl_socket_listen(self, backlog);
}

mp_obj_t common_hal_ssl_sslsocket_accept(ssl_sslsocket_obj_t *self) {
    mp_obj_t accepted = ssl_socket_accept(self);
    mp_obj_t sock = mp_obj_subscr(accepted, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_SENTINEL);
    ssl_sslsocket_obj_t *sslsock = common_hal_ssl_sslcontext_wrap_socket(self->ssl_context, sock, true, NULL);
    do_handshake(sslsock);
    mp_obj_t peer = mp_obj_subscr(accepted, MP_OBJ_NEW_SMALL_INT(1), MP_OBJ_SENTINEL);
    mp_obj_t tuple_contents[2];
    tuple_contents[0] = MP_OBJ_FROM_PTR(sslsock);
    tuple_contents[1] = peer;
    return mp_obj_new_tuple(2, tuple_contents);
}

void common_hal_ssl_sslsocket_setsockopt(ssl_sslsocket_obj_t *self, mp_obj_t level_obj, mp_obj_t optname_obj, mp_obj_t optval_obj) {
    ssl_socket_setsockopt(self, level_obj, optname_obj, optval_obj);
}

void common_hal_ssl_sslsocket_settimeout(ssl_sslsocket_obj_t *self, mp_obj_t timeout_obj) {
    ssl_socket_settimeout(self, timeout_obj);
}

static bool poll_common(ssl_sslsocket_obj_t *self, uintptr_t arg) {
    // Take into account that the library might have buffered data already
    int has_pending = 0;
    if (arg & MP_STREAM_POLL_RD) {
        has_pending = mbedtls_ssl_check_pending(&self->ssl);
        if (has_pending) {
            // Shortcut if we only need to read and we have buffered data, no need to go to the underlying socket
            return true;
        }
    }

    // If the library signaled us that it needs reading or writing, only
    // check that direction
    if (self->poll_mask && (arg & MP_STREAM_POLL_RDWR)) {
        arg = (arg & ~MP_STREAM_POLL_RDWR) | self->poll_mask;
    }

    // If direction the library needed is available, return a fake
    // result to the caller so that it reenters a read or a write to
    // allow the handshake to progress
    const mp_stream_p_t *stream_p = mp_get_stream_raise(self->sock_obj, MP_STREAM_OP_IOCTL);
    int errcode;
    mp_int_t ret = stream_p->ioctl(self->sock_obj, MP_STREAM_POLL, arg, &errcode);
    return ret != 0;
}

bool common_hal_ssl_sslsocket_readable(ssl_sslsocket_obj_t *self) {
    return poll_common(self, MP_STREAM_POLL_RD);
}

bool common_hal_ssl_sslsocket_writable(ssl_sslsocket_obj_t *self) {
    return poll_common(self, MP_STREAM_POLL_WR);
}
