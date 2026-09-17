/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MASSDRIVER EI (massdriver.space)
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

#if MICROPY_PY_ZEPHYR_TLS

BUILD_ASSERT(IS_ENABLED(CONFIG_NET_SOCKETS) && IS_ENABLED(CONFIG_NET_SOCKETS_SOCKOPT_TLS),
    "CONFIG_NET_SOCKETS and CONFIG_NET_SOCKETS_SOCKOPT_TLS must be enabled to use MICROPY_PY_ZEPHYR_TLS");

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/tls_credentials.h>

#include "py/builtin.h"
#include "py/objmodule.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "modsocket.h"

#ifndef NO_QSTR
#include <mbedtls/ssl.h>
#endif

#define MP_PROTOCOL_SERVER      (0x10)
#define MP_PROTOCOL_CLIENT      (0x0)
#define MP_PROTOCOL_TLS         (0x0)
#define MP_PROTOCOL_DTLS        (0x1)
#define MP_PROTOCOL_xTLS_MSK    (0xf)
#define MP_PROTOCOL_ROLE_MSK    (0xf0)

#define MP_PROTOCOL_TLS_CLIENT  (MP_PROTOCOL_TLS | MP_PROTOCOL_CLIENT)
#define MP_PROTOCOL_TLS_SERVER  (MP_PROTOCOL_TLS | MP_PROTOCOL_SERVER)
#define MP_PROTOCOL_DTLS_CLIENT  (MP_PROTOCOL_DTLS | MP_PROTOCOL_CLIENT)
#define MP_PROTOCOL_DTLS_SERVER  (MP_PROTOCOL_DTLS | MP_PROTOCOL_SERVER)

// This corresponds to an SSLContext object.
typedef struct _ssl_context_obj_t {
    mp_obj_base_t base;
    uint32_t protocol;
    int *ciphersuites;
    size_t ciphersuite_cnt;
    sec_tag_t owned_tags[CONFIG_TLS_MAX_CREDENTIALS_NUMBER];
    int authmode;
    #if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED) || defined(NO_QSTR)
    sec_tag_t psk_tag;
    mp_obj_t psk_identity;
    mp_obj_t psk_key;
    #endif
    #if defined(CONFIG_NET_SOCKETS_TLS_CERT_VERIFY_CALLBACK)
    mp_obj_t verify_handler;
    struct zsock_tls_cert_verify_cb verify_cb_s;
    #endif
} ssl_context_obj_t;

static const mp_obj_type_t ssl_context_type;

/* Helper Functions ----------------------------------------------------------------------------- */

/* Global tag counter, 0 is always unused, negative are reserved */
static sec_tag_t sec_tag_cnt = 0;

static sec_tag_t ssl_context_get_tag(void) {
    sec_tag_cnt++;
    if (sec_tag_cnt >= INT_MAX) {
        sec_tag_cnt = 1;
    }
    return sec_tag_cnt;
}

static int ssl_context_update_local_tags(ssl_context_obj_t *self, sec_tag_t tag, bool add) {
    size_t last_free_entry = CONFIG_TLS_MAX_CREDENTIALS_NUMBER;

    for (size_t i = 0; i < CONFIG_TLS_MAX_CREDENTIALS_NUMBER; i++) {
        if (self->owned_tags[i] == tag) {
            if (!add) {
                self->owned_tags[i] = 0;
            }
            return 0;
        } else if (self->owned_tags[i] == 0) {
            last_free_entry = i;
        }
    }

    if (add) {
        if (last_free_entry >= CONFIG_TLS_MAX_CREDENTIALS_NUMBER) {
            return -1;
        }
        self->owned_tags[last_free_entry] = tag;
    }

    return 0;
}

static const char *ssl_context_protocol_get_str(const uint32_t protocol) {
    switch (protocol) {
        case MP_PROTOCOL_TLS_CLIENT:
            return STRINGIFY(MP_PROTOCOL_TLS_CLIENT);
        case MP_PROTOCOL_TLS_SERVER:
            return STRINGIFY(MP_PROTOCOL_TLS_SERVER);
        #if defined(CONFIG_NET_SOCKETS_ENABLE_DTLS)
        case MP_PROTOCOL_DTLS_CLIENT:
            return STRINGIFY(MP_PROTOCOL_DTLS_CLIENT);
        case MP_PROTOCOL_DTLS_SERVER:
            return STRINGIFY(MP_PROTOCOL_DTLS_SERVER);
        #endif
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%d is not a ssl context protocol"), protocol);
    return NULL;
}

/* Returns bytes */
static mp_obj_t ssl_context_file_or_bytes_get_data(mp_obj_t in) {
    if (mp_obj_is_str(in)) {
        mp_obj_t args[2] = {
            in,
            MP_OBJ_NEW_QSTR(MP_QSTR_rb),
        };
        mp_obj_t file = mp_call_function_n_kw(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), 2, 0, args);
        mp_obj_t dest[2];
        mp_load_method(file, MP_QSTR_read, dest);
        mp_obj_t data = mp_call_method_n_kw(0, 0, dest);
        mp_stream_close(file);
        return data;
    }

    return in;
}

/* Methods -------------------------------------------------------------------------------------- */

static void ssl_context_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    ssl_context_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<SSLContext %p protocol=%s>",
        self,
        ssl_context_protocol_get_str(self->protocol));
}

#if defined(CONFIG_NET_SOCKETS_TLS_CERT_VERIFY_CALLBACK)
static int ssl_sock_cert_verify(void *ptr, mbedtls_x509_crt *crt, int depth, uint32_t *flags) {
    ssl_context_obj_t *self = ptr;
    if (self->verify_handler == mp_const_none) {
        return 0;
    }
    mp_obj_array_t cert;
    mp_obj_memoryview_init(&cert, 'B', 0, crt->raw.len, crt->raw.p);
    return mp_obj_get_int(mp_call_function_2(self->verify_handler, MP_OBJ_FROM_PTR(&cert), MP_OBJ_NEW_SMALL_INT(depth)));
}
#endif

static mp_obj_t ssl_context_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    const uint32_t protocol = mp_obj_get_int(args[0]);
    (void)ssl_context_protocol_get_str(protocol);

    ssl_context_obj_t *self = mp_obj_malloc_with_finaliser(ssl_context_obj_t, type_in);
    self->protocol = protocol;
    self->ciphersuites = NULL;
    self->ciphersuite_cnt = 0;
    self->authmode = ZSOCK_TLS_PEER_VERIFY_OPTIONAL;
    #if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED) || defined(NO_QSTR)
    self->psk_tag = 0;
    self->psk_identity = mp_const_none;
    self->psk_key = mp_const_none;
    #endif
    #if defined(CONFIG_NET_SOCKETS_TLS_CERT_VERIFY_CALLBACK)
    self->verify_handler = mp_const_none;
    self->verify_cb_s.cb = ssl_sock_cert_verify;
    self->verify_cb_s.ctx = self;
    #endif
    memset(self->owned_tags, 0, sizeof(self->owned_tags));

    return MP_OBJ_FROM_PTR(self);
}

/* Copy paste from modtls_mbedtls.c */
static mp_obj_t ssl_context_get_ciphers(mp_obj_t self_in) {
    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (const int *cipher_list = mbedtls_ssl_list_ciphersuites(); *cipher_list; ++cipher_list) {
        const char *cipher_name = mbedtls_ssl_get_ciphersuite_name(*cipher_list);
        mp_obj_list_append(list, MP_OBJ_FROM_PTR(mp_obj_new_str_from_cstr(cipher_name)));
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(ssl_context_get_ciphers_obj, ssl_context_get_ciphers);

static mp_obj_t ssl_context_set_ciphers(mp_obj_t self_in, mp_obj_t ciphersuite) {
    ssl_context_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Check that ciphersuite is a list or tuple.
    size_t len = 0;
    mp_obj_t *ciphers;
    mp_obj_get_array(ciphersuite, &len, &ciphers);
    if (len == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid cipher list"));
    }

    // Parse list of ciphers.
    self->ciphersuites = m_new(int, len);
    for (size_t i = 0; i < len; ++i) {
        const char *ciphername = mp_obj_str_get_str(ciphers[i]);
        const int id = mbedtls_ssl_get_ciphersuite_id(ciphername);
        if (id == 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid cipher"));
        }
        self->ciphersuites[i] = id;
    }
    self->ciphersuite_cnt = len;

    /* Save list for socket creation */

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_context_set_ciphers_obj, ssl_context_set_ciphers);

static mp_obj_t ssl_context_load_cert_chain(mp_obj_t self_in, mp_obj_t cert, mp_obj_t key) {
    ssl_context_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t certdata = ssl_context_file_or_bytes_get_data(cert);
    mp_obj_t keydata = ssl_context_file_or_bytes_get_data(cert);
    mp_buffer_info_t certbuf, keybuf;
    int ret;
    sec_tag_t tag;

    mp_get_buffer_raise(certdata, &certbuf, MP_BUFFER_READ);
    mp_get_buffer_raise(keydata, &keybuf, MP_BUFFER_READ);

    do {
        tag = ssl_context_get_tag();
        ret = tls_credential_add(tag, TLS_CREDENTIAL_PUBLIC_CERTIFICATE, certbuf.buf, certbuf.len);
    } while (ret == -EEXIST);
    if (ret < 0) {
        if (ret == -EACCES) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("access denied"));
        } else if (ret == -ENOMEM) {
            mp_raise_type(&mp_type_MemoryError);
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid certificate data: %d"), ret);
        }
    }

    ret = tls_credential_add(tag, TLS_CREDENTIAL_PRIVATE_KEY, keybuf.buf, keybuf.len);
    if (ret < 0) {
        (void)tls_credential_delete(tag, TLS_CREDENTIAL_PUBLIC_CERTIFICATE);
        if (ret == -EACCES) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("access denied"));
        } else if (ret == -ENOMEM) {
            mp_raise_type(&mp_type_MemoryError);
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid key data: %d"), ret);
        }
    }

    ret = ssl_context_update_local_tags(self, tag, true);
    if (ret < 0) {
        (void)tls_credential_delete(tag, TLS_CREDENTIAL_PUBLIC_CERTIFICATE);
        (void)tls_credential_delete(tag, TLS_CREDENTIAL_PRIVATE_KEY);
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("No free credential tags in listing"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(ssl_context_load_cert_chain_obj, ssl_context_load_cert_chain);

static mp_obj_t ssl_context_load_verify_locations(mp_obj_t self_in, mp_obj_t cadata) {
    ssl_context_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t certdata = ssl_context_file_or_bytes_get_data(cadata);
    mp_buffer_info_t certbuf;
    int ret;
    sec_tag_t tag;

    mp_get_buffer_raise(certdata, &certbuf, MP_BUFFER_READ);

    do {
        tag = ssl_context_get_tag();
        ret = tls_credential_add(tag, TLS_CREDENTIAL_CA_CERTIFICATE, certbuf.buf, certbuf.len);
    } while (ret == -EEXIST);
    if (ret < 0) {
        if (ret == -EACCES) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("access denied"));
        } else if (ret == -ENOMEM) {
            mp_raise_type(&mp_type_MemoryError);
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid certificate data: %d"), ret);
        }
    }

    ret = ssl_context_update_local_tags(self, tag, true);
    if (ret < 0) {
        (void)tls_credential_delete(tag, TLS_CREDENTIAL_CA_CERTIFICATE);
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("No free credential tag in listing"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_context_load_verify_locations_obj, ssl_context_load_verify_locations);

static mp_obj_t ssl_context___del__(mp_obj_t self_in) {
    ssl_context_obj_t *self = MP_OBJ_TO_PTR(self_in);
    for (size_t i = 0; i < CONFIG_TLS_MAX_CREDENTIALS_NUMBER; i++) {
        if (self->owned_tags[i] != 0) {
            /* Just try delete all possible for tag */
            (void)tls_credential_delete(self->owned_tags[i], TLS_CREDENTIAL_CA_CERTIFICATE);
            (void)tls_credential_delete(self->owned_tags[i], TLS_CREDENTIAL_PUBLIC_CERTIFICATE);
            (void)tls_credential_delete(self->owned_tags[i], TLS_CREDENTIAL_PRIVATE_KEY);
            (void)tls_credential_delete(self->owned_tags[i], TLS_CREDENTIAL_PSK);
            (void)tls_credential_delete(self->owned_tags[i], TLS_CREDENTIAL_PSK_ID);
            (void)tls_credential_delete(self->owned_tags[i], TLS_CREDENTIAL_PRIVATE_KEY_PSA);
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(ssl_context___del___obj, ssl_context___del__);

/* Recreate socket reusing the wrapped socket's infos, but with TLS or DTLS protocol */
static mp_obj_t ssl_context_wrap_socket_recreate(ssl_context_obj_t *self, socket_obj_t *in_socket) {
    int prot = NET_IPPROTO_TLS_1_0;
    int family;
    int type;

    size_t len = sizeof(family);
    int ret = zsock_getsockopt(in_socket->ctx, ZSOCK_SOL_SOCKET, ZSOCK_SO_DOMAIN, &family, &len);
    if (ret < 0) {
        mp_raise_OSError(errno);
    }
    len = sizeof(type);
    ret = zsock_getsockopt(in_socket->ctx, ZSOCK_SOL_SOCKET, ZSOCK_SO_TYPE, &type, &len);
    if (ret < 0) {
        mp_raise_OSError(errno);
    }

    if (in_socket->state == SOCK_STATE_CONNECTED || in_socket->state == SOCK_STATE_ACCEPTED) {
        mp_warning(MP_WARN_CAT(NotImplementedWarning),
            "wrapping a connected socket interrupts the connection as it needs to be recreated");
    }

    #ifdef CONFIG_NET_SOCKETS_ENABLE_DTLS
    if ((self->protocol & MP_PROTOCOL_xTLS_MSK) == MP_PROTOCOL_DTLS) {
        if (type != NET_SOCK_DGRAM) {
            mp_raise_msg(&mp_type_ValueError, "socket type is not datagram for DTLS");
        }
        prot = NET_IPPROTO_DTLS_1_0;
    }
    #endif

    if ((self->protocol & MP_PROTOCOL_xTLS_MSK) == MP_PROTOCOL_TLS && type != NET_SOCK_STREAM) {
        mp_raise_msg(&mp_type_ValueError, "socket type is not stream for TLS");
    }

    mp_obj_t func_args[4] = {
        mp_obj_new_int(in_socket->family),
        mp_obj_new_int(in_socket->type),
        mp_obj_new_int(prot),
    };

    mp_obj_t out_socket_obj = mp_call_function_n_kw(MP_OBJ_FROM_PTR(&zephyr_socket_type), 3, 0, func_args);

    if (in_socket->timeout == 0) {
        mp_load_method(out_socket_obj, MP_QSTR_setblocking, func_args);
        func_args[2] = mp_const_false;
        mp_call_method_n_kw(1, 0, func_args);
    }

    return out_socket_obj;
}

static mp_obj_t ssl_context_wrap_socket(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_server_side, ARG_do_handshake_on_connect, ARG_server_hostname, ARG_client_id };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_server_side, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_do_handshake_on_connect, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_server_hostname, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        #ifdef CONFIG_NET_SOCKETS_ENABLE_DTLS
        { MP_QSTR_client_id, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        #endif
    };

    // Parse arguments.
    ssl_context_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_obj_t in_sock = pos_args[1];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #ifdef CONFIG_NET_SOCKETS_ENABLE_DTLS
    client_id = args[ARG_client_id].u_obj;
    #endif

    if (!mp_obj_is_type(in_sock, &zephyr_socket_type)) {
        mp_raise_msg(&mp_type_TypeError, "socket is not a zephyr socket");
    }
    socket_obj_t *in_socket = MP_OBJ_TO_PTR(in_sock);
    mp_obj_t out_sock = MP_OBJ_NULL;
    socket_obj_t *out_socket;
    int prot;
    size_t len = sizeof(prot);
    struct net_sockaddr saddr;
    struct net_sockaddr saddr_peer;

    int ret = zsock_getsockopt(in_socket->ctx, ZSOCK_SOL_SOCKET, ZSOCK_SO_PROTOCOL, &prot, &len);
    if (ret < 0) {
        if (errno == EINVAL) {
            mp_raise_msg(&mp_type_ValueError, "socket is invalid");
        }
        mp_raise_OSError(errno);
    }

    if (!args[ARG_server_side].u_bool && self->authmode == ZSOCK_TLS_PEER_VERIFY_REQUIRED) {
        mp_raise_ValueError(MP_ERROR_TEXT("VERIFY_REQUIRED requires server_hostname"));
    }

    /* Socket doesn't need to be recreated */
    if (((prot == NET_IPPROTO_TLS_1_0 || prot == NET_IPPROTO_TLS_1_1 || prot == NET_IPPROTO_TLS_1_2
          || prot == NET_IPPROTO_TLS_1_3) && (self->protocol & MP_PROTOCOL_xTLS_MSK) == MP_PROTOCOL_TLS)
        || ((prot == NET_IPPROTO_DTLS_1_0 || prot == NET_IPPROTO_DTLS_1_2)
            && (self->protocol & MP_PROTOCOL_xTLS_MSK) == MP_PROTOCOL_DTLS)) {
        out_socket = in_socket;
    } else {
        if (args[ARG_server_side].u_bool) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("zephyr sockets can not convert an existing connection"));
        }
        out_sock = ssl_context_wrap_socket_recreate(self, in_socket);
        out_socket = MP_OBJ_TO_PTR(out_sock);
    }

    if (in_socket->bound) {
        len = sizeof(saddr);
        ret = zsock_getsockname(in_socket->ctx, &saddr, &len);
        if (ret < 0) {
            mp_raise_OSError(errno);
        }
    }

    if (in_socket->state == SOCK_STATE_CONNECTED || in_socket->state == SOCK_STATE_ACCEPTED
        || in_socket->state == SOCK_STATE_CONNECTING) {
        len = sizeof(saddr_peer);
        ret = zsock_getpeername(in_socket->ctx, &saddr_peer, &len);
        if (errno == ENOTCONN) {
            in_socket->state = SOCK_STATE_PEER_CLOSED;
        } else if (ret < 0) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to get peer information"));
        }
    }

    if (self->ciphersuites != NULL) {
        ret = zsock_setsockopt(out_socket->ctx, ZSOCK_SOL_TLS, ZSOCK_TLS_CIPHERSUITE_LIST, self->ciphersuites, self->ciphersuite_cnt * sizeof(int));
        if (ret < 0) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to set ciphersuite list: %d"), errno);
        }
    }

    ret = zsock_setsockopt(out_socket->ctx, ZSOCK_SOL_TLS, ZSOCK_TLS_PEER_VERIFY, &self->authmode, sizeof(int));
    if (ret < 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to set authmode: %d"), errno);
    }

    int tags[CONFIG_TLS_MAX_CREDENTIALS_NUMBER];
    len = 0;
    for (int i = 0; i < CONFIG_TLS_MAX_CREDENTIALS_NUMBER; i++) {
        if (self->owned_tags[i] != 0) {
            tags[len] = self->owned_tags[i];
            len++;
        }
    }
    if (len > 0) {
        ret = zsock_setsockopt(out_socket->ctx, ZSOCK_SOL_TLS, ZSOCK_TLS_SEC_TAG_LIST, tags, len * sizeof(int));
        if (ret < 0) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to set certificate tag list: %d"), errno);
        }
    }

    const char *server_hostname = NULL;
    len = 0;
    if (args[ARG_server_hostname].u_obj != mp_const_none) {
        server_hostname = mp_obj_str_get_str(args[ARG_server_hostname].u_obj);
        len = mp_obj_get_int(mp_obj_len(args[ARG_server_hostname].u_obj)) + 1;
    }
    ret = zsock_setsockopt(out_socket->ctx, ZSOCK_SOL_TLS, ZSOCK_TLS_HOSTNAME, server_hostname, len * sizeof(char));
    if (ret < 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to set server hostname: %d"), errno);
    }

    #ifdef CONFIG_NET_SOCKETS_ENABLE_DTLS
    if ((self->protocol & MP_PROTOCOL_xTLS_MSK) == MP_PROTOCOL_DTLS) {
        int val = ZSOCK_TLS_DTLS_ROLE_CLIENT;
        if ((self->protocol & MP_PROTOCOL_ROLE_MSK) == MP_PROTOCOL_SERVER) {
            val = ZSOCK_TLS_DTLS_ROLE_SERVER;
        }
        ret = zsock_setsockopt(out_socket->ctx, ZSOCK_SOL_TLS, ZSOCK_TLS_DTLS_ROLE, &val, sizeof(int));
        if (ret < 0) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to set DTLS role: %d"), errno);
        }

        val = 1;
        if (!args[ARG_do_handshake_on_connect].u_bool) {
            val = 0;
        }
        /* Not doing handshake at connect is a TODO for regular TLS on zephyr */
        ret = zsock_setsockopt(out_socket->ctx, ZSOCK_SOL_TLS, ZSOCK_TLS_DTLS_HANDSHAKE_ON_CONNECT, &val, sizeof(int));
        if (ret < 0) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to set handshake settings: %d"), errno);
        }
        mp_buffer_info_t buf;
        mp_get_buffer_raise(client_id, &buf, MP_BUFFER_READ);
        ret = zsock_setsockopt(out_socket->ctx, ZSOCK_SOL_TLS, ZSOCK_TLS_DTLS_CID_VALUE, buf.buf, buf.len);
        if (ret < 0) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to set client ID: %d"), errno);
        }
    }
    #endif

    #if defined(CONFIG_NET_SOCKETS_TLS_CERT_VERIFY_CALLBACK)
    ret = zsock_setsockopt(out_socket->ctx, ZSOCK_SOL_TLS, ZSOCK_TLS_CERT_VERIFY_CALLBACK, &self->verify_cb_s, sizeof(self->verify_cb_s));
    if (ret < 0) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to set verify callback: %d"), errno);
    }
    #endif

    /* Resume operation with recreated socket */
    if (in_socket != out_socket) {
        mp_stream_close(in_sock);
        if (in_socket->bound) {
            int ret = zsock_bind(out_socket->ctx, &saddr, sizeof(saddr));
            if (ret < 0) {
                mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to restore binding: %d"), errno);
            }
            out_socket->bound = true;
        }
        if (in_socket->state == SOCK_STATE_CONNECTED || in_socket->state == SOCK_STATE_CONNECTING) {
            int ret = zsock_connect(out_socket->ctx, &saddr_peer, sizeof(saddr_peer));
            if (ret < 0) {
                if (out_socket->timeout == 0 && errno == EINPROGRESS) {
                    out_socket->state = SOCK_STATE_CONNECTING;
                } else {
                    mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("failed to restore connection: %d"), errno);
                }
            }
            out_socket->state = SOCK_STATE_CONNECTED;
        }
        return out_sock;
    }
    return in_sock;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(ssl_context_wrap_socket_obj, 2, ssl_context_wrap_socket);

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED) || defined(NO_QSTR)
static void ssl_context_attr_update_psk(ssl_context_obj_t *self, mp_obj_t data, enum tls_credential_type kind) {
    int ret;
    mp_buffer_info_t pskbuf;
    mp_get_buffer_raise(data, &pskbuf, MP_BUFFER_READ);

    if (self->psk_tag == 0) {
        do {
            self->psk_tag = ssl_context_get_tag();
            ret = tls_credential_add(self->psk_tag, kind, pskbuf.buf, pskbuf.len);
        } while (ret == -EEXIST);
        if (ret < 0) {
            if (ret == -EACCES) {
                mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("access denied"));
            } else if (ret == -ENOMEM) {
                mp_raise_type(&mp_type_MemoryError);
            } else {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid PSK data: %d"), ret);
            }
        }
    }

    ret = ssl_context_update_local_tags(self, self->psk_tag, true);
    if (ret < 0) {
        (void)tls_credential_delete(self->psk_tag, kind);
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("No free credential tags in listing"));
    }
}

#endif

static void ssl_context_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    ssl_context_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute.
        if (attr == MP_QSTR_verify_mode) {
            dest[0] = MP_OBJ_NEW_SMALL_INT(self->authmode);
        #if defined(CONFIG_NET_SOCKETS_TLS_CERT_VERIFY_CALLBACK)
        } else if (attr == MP_QSTR_verify_callback) {
            dest[0] = self->verify_handler;
        #endif
        #if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED) || defined(NO_QSTR)
        } else if (attr == MP_QSTR_psk_identity) {
            dest[0] = self->psk_identity;
        } else if (attr == MP_QSTR_psk_key) {
            dest[0] = self->psk_key;
        #endif
        } else {
            // Continue lookup in locals_dict.
            dest[1] = MP_OBJ_SENTINEL;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store attribute.
        if (attr == MP_QSTR_verify_mode) {
            self->authmode = mp_obj_get_int(dest[1]);
            dest[0] = MP_OBJ_NULL;
        #if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED) || defined(NO_QSTR)
        } else if (attr == MP_QSTR_psk_identity) {
            dest[0] = MP_OBJ_NULL;
            ssl_context_attr_update_psk(self, dest[1], TLS_CREDENTIAL_PSK_ID);
            self->psk_identity = dest[1];
        } else if (attr == MP_QSTR_psk_key) {
            dest[0] = MP_OBJ_NULL;
            ssl_context_attr_update_psk(self, dest[1], TLS_CREDENTIAL_PSK);
            self->psk_key = dest[1];
        #endif
        #if defined(CONFIG_NET_SOCKETS_TLS_CERT_VERIFY_CALLBACK)
        } else if (attr == MP_QSTR_verify_callback) {
            dest[0] = MP_OBJ_NULL;
            self->verify_handler = dest[1];
        }
        #else
        }
        #endif
    }
}

static const mp_rom_map_elem_t ssl_context_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ssl_context___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_get_ciphers), MP_ROM_PTR(&ssl_context_get_ciphers_obj)},
    { MP_ROM_QSTR(MP_QSTR_set_ciphers), MP_ROM_PTR(&ssl_context_set_ciphers_obj)},
    { MP_ROM_QSTR(MP_QSTR_load_cert_chain), MP_ROM_PTR(&ssl_context_load_cert_chain_obj)},
    { MP_ROM_QSTR(MP_QSTR_load_verify_locations), MP_ROM_PTR(&ssl_context_load_verify_locations_obj)},
    { MP_ROM_QSTR(MP_QSTR_wrap_socket), MP_ROM_PTR(&ssl_context_wrap_socket_obj) },
};
static MP_DEFINE_CONST_DICT(ssl_context_locals_dict, ssl_context_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    ssl_context_type,
    MP_QSTR_SSLContext,
    MP_TYPE_FLAG_NONE,
    make_new, ssl_context_make_new,
    print, ssl_context_print,
    attr, ssl_context_attr,
    locals_dict, &ssl_context_locals_dict
    );

static const MP_DEFINE_STR_OBJ(mbedtls_version_obj, MBEDTLS_VERSION_STRING_FULL);

static const mp_rom_map_elem_t mp_module_tls_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tls) },

    // Classes.
    { MP_ROM_QSTR(MP_QSTR_SSLContext), MP_ROM_PTR(&ssl_context_type) },

    // Constants.
    { MP_ROM_QSTR(MP_QSTR_MBEDTLS_VERSION), MP_ROM_PTR(&mbedtls_version_obj)},
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_TLS_CLIENT), MP_ROM_INT(MP_PROTOCOL_TLS_CLIENT) },
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_TLS_SERVER), MP_ROM_INT(MP_PROTOCOL_TLS_SERVER) },
    #ifdef CONFIG_NET_SOCKETS_ENABLE_DTLS
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_DTLS_CLIENT), MP_ROM_INT(MP_PROTOCOL_DTLS_CLIENT) },
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_DTLS_SERVER), MP_ROM_INT(MP_PROTOCOL_DTLS_SERVER) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_CERT_NONE), MP_ROM_INT(ZSOCK_TLS_PEER_VERIFY_NONE) },
    { MP_ROM_QSTR(MP_QSTR_CERT_OPTIONAL), MP_ROM_INT(ZSOCK_TLS_PEER_VERIFY_OPTIONAL) },
    { MP_ROM_QSTR(MP_QSTR_CERT_REQUIRED), MP_ROM_INT(ZSOCK_TLS_PEER_VERIFY_REQUIRED) },
};
static MP_DEFINE_CONST_DICT(mp_module_tls_globals, mp_module_tls_globals_table);

const mp_obj_module_t mp_module_tls = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_tls_globals,
};

MP_REGISTER_MODULE(MP_QSTR_tls, mp_module_tls);

#endif // MICROPY_PY_ZEPHYR_TLS
