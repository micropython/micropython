/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Linaro Ltd.
 * Copyright (c) 2019 Paul Sokolovsky
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
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

#include "shared-bindings/ssl/SSLSocket.h"
#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/ssl/SSLContext.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "shared-bindings/socketpool/Socket.h"

#include "shared/runtime/interrupt_char.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/tick.h"

#if defined(MBEDTLS_ERROR_C)
#include "../../lib/mbedtls_errors/mp_mbedtls_errors.c"
#endif

#ifdef MBEDTLS_DEBUG_C
#include "mbedtls/debug.h"
STATIC void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str) {
    (void)ctx;
    (void)level;
    mp_printf(&mp_plat_print, "DBG:%s:%04d: %s\n", file, line, str);
}
#define DEBUG(fmt, ...) mp_printf(&mp_plat_print, "DBG:%s:%04d: " fmt "\n", __FILE__, __LINE__,##__VA_ARGS__)
#else
#define DEBUG(...) do {} while (0)
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

    // mp_uint_t out_sz = sock_stream->write(sock, buf, len, &err);
    mp_int_t out_sz = socketpool_socket_send(sock, buf, len);
    DEBUG("socket_send() -> %d", out_sz);
    if (out_sz < 0) {
        int err = -out_sz;
        DEBUG("sock_stream->write() -> %d nonblocking? %d", out_sz, mp_is_nonblocking_error(err));
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

    mp_int_t out_sz = socketpool_socket_recv_into(sock, buf, len);
    DEBUG("socket_recv() -> %d", out_sz);
    if (out_sz < 0) {
        int err = -out_sz;
        if (mp_is_nonblocking_error(err)) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }
        return -err;
    } else {
        return out_sz;
    }
}



ssl_sslsocket_obj_t *common_hal_ssl_sslcontext_wrap_socket(ssl_sslcontext_obj_t *self,
    socketpool_socket_obj_t *socket, bool server_side, const char *server_hostname) {

    if (socket->type != SOCKETPOOL_SOCK_STREAM) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid socket for TLS"));
    }

    ssl_sslsocket_obj_t *o = m_new_obj_with_finaliser(ssl_sslsocket_obj_t);
    o->base.type = &ssl_sslsocket_type;
    o->ssl_context = self;
    o->sock = socket;

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

    mbedtls_ssl_set_bio(&o->ssl, &o->sock, _mbedtls_ssl_send, _mbedtls_ssl_recv, NULL);

    if (self->cert_buf.buf != NULL) {
        ret = mbedtls_pk_parse_key(&o->pkey, self->key_buf.buf, self->key_buf.len + 1, NULL, 0);
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

mp_uint_t common_hal_ssl_sslsocket_recv_into(ssl_sslsocket_obj_t *self, uint8_t *buf, uint32_t len) {
    int ret = mbedtls_ssl_read(&self->ssl, buf, len);
    DEBUG("recv_into mbedtls_ssl_read() -> %d\n", ret);
    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        DEBUG("returning %d\n", 0);
        // end of stream
        return 0;
    }
    if (ret >= 0) {
        DEBUG("returning %d\n", ret);
        return ret;
    }
    if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
        ret = MP_EWOULDBLOCK;
    } else if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        // If handshake is not finished, read attempt may end up in protocol
        // wanting to write next handshake message. The same may happen with
        // renegotiation.
        ret = MP_EWOULDBLOCK;
    }
    DEBUG("raising errno [error case] %d\n", ret);
    mp_raise_OSError(ret);
}

mp_uint_t common_hal_ssl_sslsocket_send(ssl_sslsocket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int ret = mbedtls_ssl_write(&self->ssl, buf, len);
    DEBUG("send mbedtls_ssl_write() -> %d\n", ret);
    if (ret >= 0) {
        DEBUG("returning %d\n", ret);
        return ret;
    }
    if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        ret = MP_EWOULDBLOCK;
    } else if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
        // If handshake is not finished, write attempt may end up in protocol
        // wanting to read next handshake message. The same may happen with
        // renegotiation.
        ret = MP_EWOULDBLOCK;
    }
    DEBUG("raising errno [error case] %d\n", ret);
    mp_raise_OSError(ret);
}

bool common_hal_ssl_sslsocket_bind(ssl_sslsocket_obj_t *self, const char *host, size_t hostlen, uint32_t port) {
    return common_hal_socketpool_socket_bind(self->sock, host, hostlen, port);
}

void common_hal_ssl_sslsocket_close(ssl_sslsocket_obj_t *self) {
    self->closed = true;
    common_hal_socketpool_socket_close(self->sock);
    mbedtls_pk_free(&self->pkey);
    mbedtls_x509_crt_free(&self->cert);
    mbedtls_x509_crt_free(&self->cacert);
    mbedtls_ssl_free(&self->ssl);
    mbedtls_ssl_config_free(&self->conf);
    mbedtls_ctr_drbg_free(&self->ctr_drbg);
    mbedtls_entropy_free(&self->entropy);
}

STATIC void do_handshake(ssl_sslsocket_obj_t *self) {
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

void common_hal_ssl_sslsocket_connect(ssl_sslsocket_obj_t *self, const char *host, size_t hostlen, uint32_t port) {
    common_hal_socketpool_socket_connect(self->sock, host, hostlen, port);
    do_handshake(self);
}

bool common_hal_ssl_sslsocket_get_closed(ssl_sslsocket_obj_t *self) {
    return self->closed;
}

bool common_hal_ssl_sslsocket_get_connected(ssl_sslsocket_obj_t *self) {
    return !self->closed;
}

bool common_hal_ssl_sslsocket_listen(ssl_sslsocket_obj_t *self, int backlog) {
    return common_hal_socketpool_socket_listen(self->sock, backlog);
}

ssl_sslsocket_obj_t *common_hal_ssl_sslsocket_accept(ssl_sslsocket_obj_t *self, uint8_t *ip, uint32_t *port) {
    socketpool_socket_obj_t *sock = common_hal_socketpool_socket_accept(self->sock, ip, port);
    ssl_sslsocket_obj_t *sslsock = common_hal_ssl_sslcontext_wrap_socket(self->ssl_context, sock, true, NULL);
    do_handshake(sslsock);
    return sslsock;
}

void common_hal_ssl_sslsocket_settimeout(ssl_sslsocket_obj_t *self, uint32_t timeout_ms) {
    self->sock->timeout = timeout_ms;
}
