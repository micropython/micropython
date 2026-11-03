// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Linaro Ltd.
// SPDX-FileCopyrightText: Copyright (c) 2019 Paul Sokolovsky
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Needed to reach mbedtls_pk_context's private pub_raw/pub_raw_len fields --
// see the mbedtls_pk_wrap_psa() workaround in
// common_hal_ssl_sslcontext_wrap_socket() below.
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include <string.h>

#include "shared-bindings/ssl/SSLSocket.h"
#include "shared-bindings/ssl/SSLContext.h"

#include "shared/runtime/interrupt_char.h"
#include "shared/netutils/netutils.h"
#include "mbedtls/version.h"
#include "psa/crypto.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/objarray.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/tick.h"

#include "shared-bindings/socketpool/enum.h"

#define MP_STREAM_POLL_RDWR (MP_STREAM_POLL_RD | MP_STREAM_POLL_WR)

#if defined(MBEDTLS_ERROR_C)
#include "../../lib/mbedtls_errors/mp_mbedtls_errors.c"
#endif

#ifdef MBEDTLS_DEBUG_C
#include "mbedtls/debug.h"
static void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str) {
    (void)ctx;
    (void)level;
    mp_printf(&mp_plat_print, "DBG:%s:%04d: %s", file, line, str);
}
#define DEBUG_PRINT(fmt, ...) mp_printf(&mp_plat_print, "DBG:%s:%04d: " fmt "\n", __FILE__, __LINE__,##__VA_ARGS__)
#else
#define DEBUG_PRINT(...) do {} while (0)
#endif

// Raise an OSError for an mbedtls error code.
// `flags` is a bitmask from mbedtls_ssl_get_verify_result(), or 0 if not a verify error.
static MP_NORETURN void mbedtls_raise_error_flags(int err, uint32_t flags) {
    // In mbedtls 4.x with TF-PSA-Crypto 1.x, the legacy mbedtls error constants
    // were renumbered into the PSA status space (mbedtls/compat-3-crypto.h), so
    // crypto-layer failures surface from the TLS layer as PSA statuses
    // (-132 .. -153). These collide both with legacy mbedtls codes and with the
    // socket errno passthrough below, so translate them to conventional POSIX
    // errnos first.
    switch (err) {
        case PSA_ERROR_GENERIC_ERROR:
        case PSA_ERROR_COMMUNICATION_FAILURE:
        case PSA_ERROR_STORAGE_FAILURE:
        case PSA_ERROR_HARDWARE_FAILURE:
        case PSA_ERROR_INSUFFICIENT_ENTROPY:
        case PSA_ERROR_INSUFFICIENT_DATA:
        case PSA_ERROR_SERVICE_FAILURE:
        case PSA_ERROR_CORRUPTION_DETECTED:
        case PSA_ERROR_DATA_CORRUPT:
            mp_raise_OSError(MP_EIO);
        case PSA_ERROR_NOT_PERMITTED:
        case PSA_ERROR_BAD_STATE:
            mp_raise_OSError(MP_EPERM);
        case PSA_ERROR_NOT_SUPPORTED:
            mp_raise_OSError(MP_EOPNOTSUPP);
        case PSA_ERROR_BUFFER_TOO_SMALL:
            mp_raise_OSError(MP_ENOBUFS);
        case PSA_ERROR_ALREADY_EXISTS:
            mp_raise_OSError(MP_EEXIST);
        case PSA_ERROR_DOES_NOT_EXIST:
            mp_raise_OSError(MP_ENOENT);
        case PSA_ERROR_INVALID_HANDLE:
            mp_raise_OSError(MP_EBADF);
        case PSA_ERROR_INSUFFICIENT_MEMORY:
            mp_raise_OSError(MP_ENOMEM);
        case PSA_ERROR_INSUFFICIENT_STORAGE:
            mp_raise_OSError(MP_ENOSPC);
        case PSA_ERROR_INVALID_SIGNATURE:
        case PSA_ERROR_INVALID_PADDING:
        case PSA_ERROR_INVALID_ARGUMENT:
        case PSA_ERROR_DATA_INVALID:
        default:
            if (err >= -153 && err <= -132) {
                mp_raise_OSError(MP_EINVAL);
            }
            break;
    }

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

    // All ones means mbedtls says it has nothing to report: it set to all ones
    // when a verify callback fails, and mbedtls_ssl_get_verify_result() returns
    // this when there is no session at all.
    if (flags == UINT32_MAX) {
        flags = 0;
    }

    #if defined(MBEDTLS_ERROR_C)
    // Including mbedtls_strerror takes about 1.5KB due to the error strings.
    // MBEDTLS_ERROR_C is the define used by mbedtls to conditionally include mbedtls_strerror.
    // It is set/unset in the MBEDTLS_CONFIG_FILE which is defined in the Makefile.

    // Large enough for the longest mbedtls_strerror() output, which is about 100
    // characters when it joins a high-level and a low-level name with '+', and for
    // the longest certificate verification failure name, which is 81 characters.
    #define ERR_STR_MAX 128  // mbedtls_strerror truncates if it doesn't fit
    // One byte larger than the bound passed to mbedtls_strerror(), so that the
    // strncpy() inside it has a bound smaller than the size of buf. Truncation
    // there is safe and deliberate, but an equal bound trips -Wstringop-truncation.
    char buf[ERR_STR_MAX + 1];

    // Assemble the error message in a vstr.
    // If we run out of heap, catch the MemoryError and fall back to just the error number.
    mp_obj_t exc;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        vstr_t vstr;
        vstr_init(&vstr, ERR_STR_MAX);

        mbedtls_strerror(err, buf, ERR_STR_MAX);
        vstr_add_str(&vstr, buf);

        #if !defined(MBEDTLS_X509_REMOVE_INFO)
        // Call mbedtls_x509_crt_verify_info() to get the error string
        // for each individual verify error bit.
        // This allows for easier string management.
        // Several verify errorbits are often set at once:
        // mbedtls or's together the error flags of every certificate in the chain.
        // For instance, a cert served under the wrong name and signed by an
        // unknown CA reports CN_MISMATCH and NOT_TRUSTED together.
        for (uint32_t bit = 1; bit != 0; bit <<= 1) {
            if ((flags & bit) == 0) {
                continue;
            }
            // The prefix is added to the beginning of the message.
            // We drop the supplied trailing newline.
            int info_len = mbedtls_x509_crt_verify_info(buf, ERR_STR_MAX, "; ", bit);
            if (info_len > 1) {
                // -1 to drop the newline.
                vstr_add_strn(&vstr, buf, info_len - 1);
            }
        }
        #endif

        mp_obj_t args[2] = { MP_OBJ_NEW_SMALL_INT(err), mp_obj_new_str_from_utf8_vstr(&vstr) };
        exc = mp_obj_exception_make_new(&mp_type_OSError, 2, 0, args);
        nlr_pop();
    } else {
        // Could not build the message, so report the number by itself.
        mp_raise_OSError(err);
    }
    nlr_raise(exc);
    #else
    // mbedtls is compiled without error strings, so just return the err number
    mp_raise_OSError(err); // err is typically a large negative number
    #endif
}

static MP_NORETURN void mbedtls_raise_error(int err) {
    mbedtls_raise_error_flags(err, 0);
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


ssl_sslsocket_obj_t *common_hal_ssl_sslcontext_wrap_socket(ssl_sslcontext_obj_t *self,
    mp_obj_t socket, bool server_side, const char *server_hostname) {

    mp_int_t socket_type = mp_obj_get_int(mp_load_attr(socket, MP_QSTR_type));
    if (socket_type != SOCKETPOOL_SOCK_STREAM) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid socket for TLS"));
    }

    ssl_sslsocket_obj_t *o = mp_obj_malloc_with_finaliser(ssl_sslsocket_obj_t, &ssl_sslsocket_type);
    o->ssl_context = self;
    o->sock_obj = socket;
    o->poll_mask = 0;
    // A missing method raises below. Keep the finaliser away until the object is complete.
    o->closed = true;

    mp_load_method(socket, MP_QSTR_accept, o->accept_args);
    mp_load_method(socket, MP_QSTR_bind, o->bind_args);
    mp_load_method(socket, MP_QSTR_close, o->close_args);
    mp_load_method(socket, MP_QSTR_connect, o->connect_args);
    mp_load_method(socket, MP_QSTR_listen, o->listen_args);
    mp_load_method(socket, MP_QSTR_recv_into, o->recv_into_args);
    mp_load_method(socket, MP_QSTR_send, o->send_args);
    mp_load_method(socket, MP_QSTR_settimeout, o->settimeout_args);
    mp_load_method(socket, MP_QSTR_setsockopt, o->setsockopt_args);
    o->closed = false;

    mbedtls_ssl_init(&o->ssl);
    mbedtls_ssl_config_init(&o->conf);
    mbedtls_x509_crt_init(&o->cacert);
    mbedtls_x509_crt_init(&o->cert);
    mbedtls_pk_init(&o->pkey);
    #ifdef MBEDTLS_DEBUG_C
    // Debug level (0-4) 1=warning, 2=info, 3=debug, 4=verbose
    mbedtls_debug_set_threshold(4);
    #endif

    int ret = mbedtls_ssl_config_defaults(&o->conf,
        server_side ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        goto cleanup;
    }

    if (self->crt_bundle_attach != NULL && !server_side) {
        // The root bundle checks servers. A server checks clients only
        // against a CA loaded with load_verify_locations().
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
        // Parsed first so its public key is available for the
        // mbedtls_pk_wrap_psa() workaround below.
        ret = mbedtls_x509_crt_parse(&o->cert, self->cert_buf.buf, self->cert_buf.len + 1);
        if (ret != 0) {
            goto cleanup;
        }

        if (self->hw_key_id != 0) {
            // mbedtls_pk_wrap_psa() unconditionally calls
            // psa_export_public_key() on the handle as part of wrapping it,
            // but the DS peripheral's PSA driver has no export_public_key
            // entry point (the private key, and anything derivable from it,
            // never leaves the peripheral by design), so that call fails
            // before the wrap can complete. It skips the export once
            // pub_raw_len is already set, so pre-populate it from the
            // certificate's own public key -- the public half of this same
            // key, by construction. (mbedtls_pk_write_pubkey_psa() would do
            // this copy too, but it needs MBEDTLS_PK_WRITE_C, which not every
            // port enables, so copy the field directly instead.)
            memcpy(o->pkey.pub_raw, o->cert.pk.pub_raw, o->cert.pk.pub_raw_len);
            o->pkey.pub_raw_len = o->cert.pk.pub_raw_len;
            ret = mbedtls_pk_wrap_psa(&o->pkey, self->hw_key_id);
        } else {
            ret = mbedtls_pk_parse_key(&o->pkey, self->key_buf.buf, self->key_buf.len + 1, NULL, 0);
        }
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

    if (ret == MBEDTLS_ERR_SSL_ALLOC_FAILED) {
        mp_raise_type(&mp_type_MemoryError);
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

    // Verification flags are only valid for CERT_VERIFY_FAILED.
    // Read them before mbedtls_ssl_free() below: they live in the ssl context's
    // session_negotiate and are gone once it is freed.
    uint32_t verify_flags = 0;
    if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
        verify_flags = mbedtls_ssl_get_verify_result(&self->ssl);
    }

    mbedtls_pk_free(&self->pkey);
    mbedtls_x509_crt_free(&self->cert);
    mbedtls_x509_crt_free(&self->cacert);
    mbedtls_ssl_free(&self->ssl);
    mbedtls_ssl_config_free(&self->conf);

    if (ret == MBEDTLS_ERR_SSL_ALLOC_FAILED) {
        mp_raise_type(&mp_type_MemoryError);
    } else if (ret == MBEDTLS_ERR_X509_BAD_INPUT_DATA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid cert"));
    } else {
        mbedtls_raise_error_flags(ret, verify_flags);
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
