/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "mpconfigport.h"


#if MICROPY_PY_USSL && MICROPY_SSL_MBEDTLS
#include "mpconfigboard.h"
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
#include "mbedtls/x509.h"
#include "helios_debug.h"

#define QPY_MPUSSL_LOG(msg, ...)      custom_log(Ussl, msg, ##__VA_ARGS__)

typedef struct _mp_obj_ssl_socket_t {
    mp_obj_base_t base;
    mp_obj_t sock;
    mbedtls_entropy_context *entropy;
    mbedtls_ctr_drbg_context *ctr_drbg;
    mbedtls_ssl_context *ssl;
    mbedtls_ssl_config *conf;
    mbedtls_x509_crt *cacert;
    mbedtls_x509_crt *cert;
    mbedtls_pk_context *pkey;
} mp_obj_ssl_socket_t;

struct ssl_args {
    mp_arg_val_t ca;
    mp_arg_val_t key;
    mp_arg_val_t cert;
    mp_arg_val_t server_side;
    mp_arg_val_t server_hostname;
    mp_arg_val_t do_handshake;
};

extern const mp_obj_type_t ussl_socket_type;


#ifdef MBEDTLS_DEBUG_C
static void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str) {
    (void)ctx;
    (void)level;
    QPY_MPUSSL_LOG("DBG:%s:%04d: %s\n", file, line, str);
}
#endif

static NORETURN void mbedtls_raise_error(int err) {
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

static int _mbedtls_ssl_send(void *ctx, const byte *buf, size_t len) {
    mp_obj_t sock = *(mp_obj_t *)ctx;

    const mp_stream_p_t *sock_stream = mp_get_stream(sock);
    int err;

    mp_printf(&mp_plat_print, "[mbedtls](write buf = %s ) :\n", buf);

    mp_uint_t out_sz = sock_stream->write(sock, buf, len, &err);
    mp_printf(&mp_plat_print, "[mbedtls](write out_sz = %d ) :\n", out_sz);
    if (out_sz == MP_STREAM_ERROR) {
        if (mp_is_nonblocking_error(err)) {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        }
        return -err; // convert an MP_ERRNO to something mbedtls passes through as error
    } else {
        return out_sz;
    }
}

static int _mbedtls_ssl_recv(void *ctx, byte *buf, size_t len) {
    mp_obj_t sock = *(mp_obj_t *)ctx;

    const mp_stream_p_t *sock_stream = mp_get_stream(sock);
    int err;

    mp_printf(&mp_plat_print, "[mbedtls](read len = %d ) :\n", len);

    mp_uint_t out_sz = sock_stream->read(sock, buf, len, &err);

    mp_printf(&mp_plat_print, "[mbedtls](read out_sz = %d ) :\n", out_sz);

    if (out_sz == MP_STREAM_ERROR) {
        if (mp_is_nonblocking_error(err)) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }
        return -err;
    } else {
        return out_sz;
    }
}

#define SSL_IGNORE_BADCERT_EXPIRED             0x01  /**< The certificate validity has expired. */
#define SSL_IGNORE_BADCERT_REVOKED             0x02  /**< The certificate has been revoked (is on a CRL). */
#define SSL_IGNORE_BADCERT_CN_MISMATCH         0x04  /**< The certificate Common Name (CN) does not match with the expected CN. */
#define SSL_IGNORE_BADCERT_NOT_TRUSTED         0x08  /**< The certificate is not correctly signed by the trusted CA. */
#define SSL_IGNORE_BADCRL_NOT_TRUSTED          0x10  /**< The CRL is not correctly signed by the trusted CA. */
#define SSL_IGNORE_BADCRL_EXPIRED              0x20  /**< The CRL is expired. */
#define SSL_IGNORE_BADCERT_MISSING             0x40  /**< Certificate was missing. */
#define SSL_IGNORE_BADCERT_SKIP_VERIFY         0x80  /**< Certificate verification was skipped. */
#define SSL_IGNORE_BADCERT_FUTURE            0x0200  /**< The certificate validity starts in the future. */

static int my_verify( void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags )
{
    char buf[1024];
    ((void) data);

    mbedtls_x509_crt_info( buf, sizeof( buf ) - 1, "", crt );

    mp_printf(&mp_plat_print, "[mbedtls](*flags %d error) :\n", *flags);
    if(*flags & (SSL_IGNORE_BADCERT_FUTURE))
    {
        *flags &= ~(SSL_IGNORE_BADCERT_FUTURE);
        return( 0 );
    }

    return *flags;
}

static mp_obj_ssl_socket_t *socket_new(mp_obj_t sock, struct ssl_args *args) {
    // Verify the socket object has the full stream protocol
    mp_get_stream_raise(sock, MP_STREAM_OP_READ | MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);
    #if MICROPY_PY_USSL_FINALISER
    mp_obj_ssl_socket_t *o = m_new_obj_with_finaliser(mp_obj_ssl_socket_t);
    #else
    //mp_obj_ssl_socket_t *o = m_new_obj(mp_obj_ssl_socket_t);
    mp_obj_ssl_socket_t *o = (mp_obj_ssl_socket_t *) m_malloc_with_finaliser(sizeof(mp_obj_ssl_socket_t));
    #endif

    int ret = -1;
    o->entropy = (mbedtls_entropy_context *)malloc(sizeof(mbedtls_entropy_context));
    if (o->entropy == NULL){
        goto cleanup;
    }
    o->ctr_drbg = (mbedtls_ctr_drbg_context *)malloc(sizeof(mbedtls_ctr_drbg_context));
    if (o->ctr_drbg == NULL){
        goto cleanup;
    }
    o->ssl = (mbedtls_ssl_context *)malloc(sizeof(mbedtls_ssl_context));
    if (o->ssl == NULL){
        goto cleanup;
    }
    o->conf = (mbedtls_ssl_config *)malloc(sizeof(mbedtls_ssl_config));
    if (o->conf == NULL){
        goto cleanup;
    }
    o->cacert = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
    if (o->cacert == NULL){
        goto cleanup;
    }
    o->cert = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
    if (o->cert == NULL){
        goto cleanup;
    }
    o->pkey = (mbedtls_pk_context *)malloc(sizeof(mbedtls_pk_context));
    if (o->pkey == NULL){
        goto cleanup;
    }

    o->base.type = &ussl_socket_type;
    o->sock = sock;

    mbedtls_platform_setup(NULL);

    mbedtls_ssl_init(o->ssl);
    mbedtls_ssl_config_init(o->conf);
    mbedtls_x509_crt_init(o->cacert);
    mbedtls_x509_crt_init(o->cert);
    mbedtls_pk_init(o->pkey);
    mbedtls_ctr_drbg_init(o->ctr_drbg);
    #ifdef MBEDTLS_DEBUG_C
    // Debug level (0-4)
    mbedtls_debug_set_threshold(100);
    #endif

    mbedtls_entropy_init(o->entropy);
    const byte seed[] = "upy";
    ret = mbedtls_ctr_drbg_seed(o->ctr_drbg, mbedtls_entropy_func, o->entropy, seed, sizeof(seed));
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_ssl_config_defaults(o->conf,
        args->server_side.u_bool ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT);
    
    if (ret != 0) {
        goto cleanup;
    }

    mbedtls_ssl_conf_authmode(o->conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(o->conf, mbedtls_ctr_drbg_random, o->ctr_drbg);
    #ifdef MBEDTLS_DEBUG_C
    mbedtls_ssl_conf_dbg(o->conf, mbedtls_debug, NULL);
    #endif

    ret = mbedtls_ssl_setup(o->ssl, o->conf);
    if (ret != 0) {
        goto cleanup;
    }

    if (args->server_hostname.u_obj != mp_const_none) {
        const char *sni = mp_obj_str_get_str(args->server_hostname.u_obj);
        ret = mbedtls_ssl_set_hostname(o->ssl, sni);
        if (ret != 0) {
            goto cleanup;
        }
    }

    mbedtls_ssl_set_bio(o->ssl, &o->sock, _mbedtls_ssl_send, _mbedtls_ssl_recv, NULL);
    
    if (args->ca.u_obj != mp_const_none) {
        size_t cert_len;
        const byte *cert = (const byte *)mp_obj_str_get_data(args->ca.u_obj, &cert_len);
        
        ret = mbedtls_x509_crt_parse(o->cacert, cert, cert_len + 1);
        if (ret != 0) {
            ret = MBEDTLS_ERR_X509_BAD_INPUT_DATA; // use general error for all cert errors
            goto cleanup;
        }

        uint32_t flags;
        char xcbuf[256];
        memset( xcbuf, 0, 256);

        ret = mbedtls_x509_crt_verify( o->cacert, o->cacert, NULL, NULL, &flags, NULL, NULL );
        if( ret != 0 )
        {
            QPY_MPUSSL_LOG("flags: %0x\n", flags);
            ret = mbedtls_x509_crt_verify_info(xcbuf, sizeof(xcbuf), "\n", flags);

            if (flags == 0x200) 
            {
                QPY_MPUSSL_LOG("mbedtls_x509_crt_verify IGNORE\n");
            }
            else
            {
                mp_printf(&mp_plat_print, "[mbedtls](mbedtls_x509_crt_verify FAIL: %s) :\n", xcbuf);
                ret = MBEDTLS_ERR_X509_FILE_IO_ERROR; // use general error for all cert errors
                goto cleanup;
            }
        }
        else
        {
            QPY_MPUSSL_LOG("mbedtls_x509_crt_verify succeeded\n");
        }

        mbedtls_ssl_conf_ca_chain(o->conf, o->cacert, NULL);
        mbedtls_ssl_conf_authmode(o->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_verify( o->conf, my_verify, NULL );
    }

    if (args->key.u_obj != mp_const_none) {
        size_t key_len;
        const byte *key = (const byte *)mp_obj_str_get_data(args->key.u_obj, &key_len);
        // len should include terminating null
        ret = mbedtls_pk_parse_key(o->pkey, key, key_len + 1, NULL, 0);
        if (ret != 0) {
            ret = MBEDTLS_ERR_PK_BAD_INPUT_DATA; // use general error for all key errors
            goto cleanup;
        }

        size_t cert_len;
        const byte *cert = (const byte *)mp_obj_str_get_data(args->cert.u_obj, &cert_len);
        // len should include terminating null
        ret = mbedtls_x509_crt_parse(o->cert, cert, cert_len + 1);
        if (ret != 0) {
            ret = MBEDTLS_ERR_X509_BAD_INPUT_DATA; // use general error for all cert errors
            goto cleanup;
        }

        ret = mbedtls_ssl_conf_own_cert(o->conf, o->cert, o->pkey);
        if (ret != 0) {
            goto cleanup;
        }
    }

    if (args->do_handshake.u_bool) {
        while ((ret = mbedtls_ssl_handshake(o->ssl)) != 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                goto cleanup;
            }
        }
    }

    return MP_OBJ_FROM_PTR(o);

cleanup:
    if (o->pkey != NULL)
    {
        mbedtls_pk_free(o->pkey); free(o->pkey); o->pkey = NULL;
    }    
    if (o->cert != NULL)
    {
        mbedtls_x509_crt_free(o->cert); free(o->cert); o->cert = NULL;
    }
    if (o->cacert != NULL)
    {
        mbedtls_x509_crt_free(o->cacert); free(o->cacert); o->cacert = NULL;
    }
    if (o->ssl != NULL)
    {
        mbedtls_ssl_free(o->ssl); free(o->ssl); o->ssl = NULL;
    }
    if (o->conf != NULL)
    {
        mbedtls_ssl_config_free(o->conf); free(o->conf); o->conf = NULL;
    }
    if (o->ctr_drbg != NULL)
    {
        mbedtls_ctr_drbg_free(o->ctr_drbg); free(o->ctr_drbg); o->ctr_drbg = NULL;
    }
    if (o->entropy != NULL)
    {
        mbedtls_entropy_free(o->entropy); free(o->entropy); o->entropy = NULL;
    }

    if (ret == MBEDTLS_ERR_SSL_ALLOC_FAILED) {
        mp_raise_OSError(MP_ENOMEM);
    } else if (ret == MBEDTLS_ERR_PK_BAD_INPUT_DATA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid key"));
    } else if (ret == MBEDTLS_ERR_X509_BAD_INPUT_DATA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid cert"));
    } else if (ret == MBEDTLS_ERR_X509_FILE_IO_ERROR) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid ca"));
    } else {
        mbedtls_raise_error(ret);
    }
}

static mp_obj_t mod_ssl_getpeercert(mp_obj_t o_in, mp_obj_t binary_form) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(o_in);
    if (!mp_obj_is_true(binary_form)) {
        mp_raise_NotImplementedError(NULL);
    }
    const mbedtls_x509_crt *peer_cert = mbedtls_ssl_get_peer_cert(o->ssl);
    if (peer_cert == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_bytes(peer_cert->raw.p, peer_cert->raw.len);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mod_ssl_getpeercert_obj, mod_ssl_getpeercert);

static void socket_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ssl_socket_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<_SSLSocket %p>", self);
}

static mp_uint_t socket_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(o_in);

    mp_printf(&mp_plat_print, "[mbedtls_ssl_read](socket_read 1:\n");
    int ret = mbedtls_ssl_read(o->ssl, buf, size);
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
    }
    *errcode = -ret;
    return MP_STREAM_ERROR;
}

static mp_uint_t socket_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(o_in);

    mp_printf(&mp_plat_print, "[mbedtls_ssl_write](socket_write 1:\n");

    int ret = mbedtls_ssl_write(o->ssl, buf, size);
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
    }
    *errcode = ret;
    return MP_STREAM_ERROR;
}

static mp_obj_t socket_setblocking(mp_obj_t self_in, mp_obj_t flag_in) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(self_in);
    mp_obj_t sock = o->sock;
    mp_obj_t dest[3];
    mp_load_method(sock, MP_QSTR_setblocking, dest);
    dest[2] = flag_in;
    return mp_call_method_n_kw(1, 0, dest);
}
static MP_DEFINE_CONST_FUN_OBJ_2(socket_setblocking_obj, socket_setblocking);

static mp_uint_t socket_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_ssl_socket_t *self = MP_OBJ_TO_PTR(o_in);
    if (request == MP_STREAM_CLOSE) {
        if (self->pkey)
        {
            mbedtls_pk_free(self->pkey);
            free(self->pkey);
            self->pkey = NULL;
        }
        if (self->cert)
        {
            mbedtls_x509_crt_free(self->cert); 
			free(self->cert);
            self->cert = NULL;
        }
        if (self->cacert)
        {
            mbedtls_x509_crt_free(self->cacert);
            free(self->cacert);
            self->cacert = NULL;
        }
        if (self->ssl)
        {
            mbedtls_ssl_free(self->ssl);
            free(self->ssl);
            self->ssl = NULL;
        }
        if (self->conf)
        {
            mbedtls_ssl_config_free(self->conf);
            free(self->conf);
            self->conf = NULL;
        }
        if (self->ctr_drbg)
        {
            mbedtls_ctr_drbg_free(self->ctr_drbg);
            free(self->ctr_drbg);
            self->ctr_drbg = NULL;
        }
        if (self->entropy)
        {
            mbedtls_entropy_free(self->entropy);
            free(self->entropy);
            self->entropy = NULL;
        }
    }
    // Pass all requests down to the underlying socket
    return mp_get_stream(self->sock)->ioctl(self->sock, request, arg, errcode);
}

static const mp_rom_map_elem_t ussl_socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&socket_setblocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    #if MICROPY_PY_USSL_FINALISER
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_getpeercert), MP_ROM_PTR(&mod_ssl_getpeercert_obj) },
};

static MP_DEFINE_CONST_DICT(ussl_socket_locals_dict, ussl_socket_locals_dict_table);

static const mp_stream_p_t ussl_socket_stream_p = {
    .read = socket_read,
    .write = socket_write,
    .ioctl = socket_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    ussl_socket_type,
    MP_QSTR_ussl,
    MP_TYPE_FLAG_NONE,
    protocol, &ussl_socket_stream_p,
    locals_dict, &ussl_socket_locals_dict
    );

bool mp_obj_is_ussl_socket(mp_obj_t o) {
    return mp_obj_is_type(o, &ussl_socket_type);
}

static mp_obj_t mod_ssl_wrap_socket(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO: Implement more args
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ca, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_key, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_cert, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_server_side, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_server_hostname, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_do_handshake, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };

    // TODO: Check that sock implements stream protocol
    mp_obj_t sock = pos_args[0];
    struct ssl_args args;
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t *)&args);

    return MP_OBJ_FROM_PTR(socket_new(sock, &args));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mod_ssl_wrap_socket_obj, 1, mod_ssl_wrap_socket);

static const mp_rom_map_elem_t mp_module_ssl_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ussl) },
    { MP_ROM_QSTR(MP_QSTR_wrap_socket), MP_ROM_PTR(&mod_ssl_wrap_socket_obj) },
};

static MP_DEFINE_CONST_DICT(mp_module_ssl_globals, mp_module_ssl_globals_table);

const mp_obj_module_t mp_module_ussl = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_ssl_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ussl, mp_module_ussl);

#endif // MICROPY_PY_USSL
