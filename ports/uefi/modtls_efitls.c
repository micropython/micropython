// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// The `tls` module (TLS=efi backend), driving the firmware EFI_TLS_PROTOCOL
// (TlsDxe) instead of vendoring mbedTLS. Same Python surface as
// extmod/modtls_mbedtls.c; we own the socket and shuttle ciphertext. Handshake
// and record framing mirror EDK2's NetworkPkg/HttpDxe/HttpsSupport.c.

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/objstr.h"

#if MICROPY_PY_SSL && MICROPY_SSL_EFITLS

#include "uefi_port.h"
#include "efi.h"
#include "efi_tls.h"

#define BS (mp_uefi_st->BootServices)

#define MP_STREAM_POLL_RDWR (MP_STREAM_POLL_RD | MP_STREAM_POLL_WR)

// Protocol identifiers (same layout as MicroPython's mbedtls constants so the
// `ssl` shim and PROTOCOL_TLS_* values match across backends).
#define MP_ENDPOINT_IS_SERVER   (1 << 0)
#define MP_PROTOCOL_TLS_CLIENT  0
#define MP_PROTOCOL_TLS_SERVER  MP_ENDPOINT_IS_SERVER

// verify_mode values (match the mbedtls backend: NONE/OPTIONAL/REQUIRED = 0/1/2).
#define MP_CERT_NONE     0
#define MP_CERT_OPTIONAL 1
#define MP_CERT_REQUIRED 2

static const EFI_GUID g_tls_sb_guid = EFI_TLS_SERVICE_BINDING_PROTOCOL_GUID;
static const EFI_GUID g_tls_guid = EFI_TLS_PROTOCOL_GUID;
static const EFI_GUID g_tls_cfg_guid = EFI_TLS_CONFIGURATION_PROTOCOL_GUID;

/******************************************************************************/
// A small growable byte buffer (GC-tracked via the owning object).

typedef struct {
    byte *buf;
    size_t len;
    size_t cap;
} bytebuf_t;

static void bb_reserve(bytebuf_t *bb, size_t extra) {
    if (bb->len + extra <= bb->cap) {
        return;
    }
    size_t ncap = bb->cap ? bb->cap : 512;
    while (ncap < bb->len + extra) {
        ncap *= 2;
    }
    bb->buf = m_renew(byte, bb->buf, bb->cap, ncap);
    bb->cap = ncap;
}

static void bb_append(bytebuf_t *bb, const void *data, size_t n) {
    if (n == 0) {
        return;
    }
    bb_reserve(bb, n);
    memcpy(bb->buf + bb->len, data, n);
    bb->len += n;
}

// Drop the first n bytes, shifting the tail to the front.
static void bb_consume(bytebuf_t *bb, size_t n) {
    if (n >= bb->len) {
        bb->len = 0;
        return;
    }
    memmove(bb->buf, bb->buf + n, bb->len - n);
    bb->len -= n;
}

/******************************************************************************/
// SSLContext.

typedef struct _mp_obj_ssl_context_t {
    mp_obj_base_t base;
    int authmode;                // MP_CERT_NONE / OPTIONAL / REQUIRED
    mp_obj_t cadata;             // bytes/str or MP_OBJ_NULL
    mp_obj_t cert;               // bytes/str or MP_OBJ_NULL
    mp_obj_t key;                // bytes/str or MP_OBJ_NULL
    mp_obj_t ciphers;            // list or MP_OBJ_NULL
} mp_obj_ssl_context_t;

static const mp_obj_type_t ssl_context_type;
static const mp_obj_type_t ssl_socket_type;

typedef struct _mp_obj_ssl_socket_t {
    mp_obj_base_t base;
    mp_obj_t sock;               // underlying stream (MP_OBJ_NULL once closed)
    EFI_TLS_SERVICE_BINDING_PROTOCOL *tls_sb;
    EFI_TLS_PROTOCOL *tls;
    EFI_HANDLE child;
    bool server_side;
    bool established;
    int hs_state;                // 0=init, 1=running
    int last_error;              // sticky errno
    mp_uint_t poll_mask;         // WANT_READ/WANT_WRITE during would-block
    bytebuf_t rx;                // inbound ciphertext, unframed
    bytebuf_t plain;             // decrypted plaintext awaiting the caller
    size_t plain_off;            // read cursor into `plain`
    bytebuf_t tx;                // outbound ciphertext awaiting the socket
    size_t tx_off;               // write cursor into `tx`
} mp_obj_ssl_socket_t;

#define HS_INIT 0
#define HS_RUNNING 1

// Non-raising status codes for the internal socket helpers.
#define SR_OK 0
#define SR_WOULDBLOCK 1
#define SR_EOF 2

/******************************************************************************/
// EFI pool helpers.

static void *efi_alloc(UINTN size) {
    void *p = NULL;
    if (EFI_ERROR(BS->AllocatePool(EfiBootServicesData, size, &p)) || p == NULL) {
        m_malloc_fail(size);
    }
    return p;
}

static void efi_free(void *p) {
    if (p != NULL) {
        BS->FreePool(p);
    }
}

static MP_NORETURN void efitls_raise(EFI_STATUS st, const char *what) {
    mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("TLS %s failed (efi 0x%x)"),
        what, (unsigned int)(UINTN)st);
}

/******************************************************************************/
// Transport helpers over the underlying (Python) socket stream.

// Read from the socket into rx until it holds at least `need` bytes.
static int efitls_fill(mp_obj_ssl_socket_t *o, size_t need) {
    const mp_stream_p_t *sp = mp_get_stream(o->sock);
    byte tmp[1536];
    while (o->rx.len < need) {
        int err;
        mp_uint_t r = sp->read(o->sock, tmp, sizeof(tmp), &err);
        if (r == MP_STREAM_ERROR) {
            if (mp_is_nonblocking_error(err)) {
                return SR_WOULDBLOCK;
            }
            mp_raise_OSError(err);
        }
        if (r == 0) {
            return SR_EOF;
        }
        bb_append(&o->rx, tmp, r);
    }
    return SR_OK;
}

// Ensure rx holds one complete TLS record; *rec_len is its total length.
static int efitls_get_record(mp_obj_ssl_socket_t *o, size_t *rec_len) {
    int s = efitls_fill(o, TLS_RECORD_HEADER_LENGTH);
    if (s != SR_OK) {
        return s;
    }
    size_t payload = ((size_t)o->rx.buf[3] << 8) | (size_t)o->rx.buf[4];
    size_t need = TLS_RECORD_HEADER_LENGTH + payload;
    s = efitls_fill(o, need);
    if (s != SR_OK) {
        return s;
    }
    *rec_len = need;
    return SR_OK;
}

// Drain tx to the socket. Partial writes are retained (tx_off advances).
static int efitls_flush(mp_obj_ssl_socket_t *o) {
    const mp_stream_p_t *sp = mp_get_stream(o->sock);
    while (o->tx.len > o->tx_off) {
        int err;
        mp_uint_t w = sp->write(o->sock, o->tx.buf + o->tx_off, o->tx.len - o->tx_off, &err);
        if (w == MP_STREAM_ERROR) {
            if (mp_is_nonblocking_error(err)) {
                return SR_WOULDBLOCK;
            }
            mp_raise_OSError(err);
        }
        o->tx_off += w;
    }
    o->tx.len = 0;
    o->tx_off = 0;
    return SR_OK;
}

/******************************************************************************/
// EFI_TLS record-engine wrappers (mirror HttpsSupport.c).

// BuildResponsePacket(in) -> append outbound bytes to tx. Two-call sizing.
static EFI_STATUS efitls_build(mp_obj_ssl_socket_t *o, UINT8 *in, UINTN insize) {
    UINTN outsize = 2048;
    UINT8 *out = efi_alloc(outsize);
    EFI_STATUS st = o->tls->BuildResponsePacket(o->tls, in, insize, out, &outsize);
    if (st == EFI_BUFFER_TOO_SMALL) {
        efi_free(out);
        out = efi_alloc(outsize);
        st = o->tls->BuildResponsePacket(o->tls, in, insize, out, &outsize);
    }
    if (!EFI_ERROR(st) && outsize > 0) {
        bb_append(&o->tx, out, outsize);
    }
    efi_free(out);
    return st;
}

// ProcessPacket(msg) -> *out (efi pool, caller frees), *outlen. Mirrors the
// FragmentTable reallocation/ownership dance in TlsProcessMessage().
static EFI_STATUS efitls_process(EFI_TLS_PROTOCOL *tls, UINT8 *msg, UINTN msgsize,
    EFI_TLS_CRYPT_MODE mode, UINT8 **out, UINTN *outlen) {
    *out = NULL;
    *outlen = 0;
    EFI_TLS_FRAGMENT_DATA *ft = efi_alloc(sizeof(EFI_TLS_FRAGMENT_DATA));
    ft->FragmentLength = (UINT32)msgsize;
    ft->FragmentBuffer = msg;
    EFI_TLS_FRAGMENT_DATA *orig = ft;
    UINT32 fcount = 1;

    EFI_STATUS st = tls->ProcessPacket(tls, &ft, &fcount, mode);
    if (!EFI_ERROR(st)) {
        UINTN total = 0;
        for (UINT32 i = 0; i < fcount; i++) {
            total += ft[i].FragmentLength;
        }
        UINT8 *buf = efi_alloc(total ? total : 1);
        UINTN off = 0;
        for (UINT32 i = 0; i < fcount; i++) {
            memcpy(buf + off, ft[i].FragmentBuffer, ft[i].FragmentLength);
            off += ft[i].FragmentLength;
            efi_free(ft[i].FragmentBuffer);   // driver-allocated output
        }
        *out = buf;
        *outlen = total;
    }
    if (ft == orig) {
        ft = NULL;
    }
    efi_free(orig);
    if (ft != NULL) {
        efi_free(ft);
    }
    return st;
}

/******************************************************************************/
// Handshake pump.

static int efitls_handshake(mp_obj_ssl_socket_t *o) {
    if (o->hs_state == HS_INIT) {
        if (!o->server_side) {
            // Client initiates: BuildResponsePacket(NULL) -> ClientHello.
            EFI_STATUS st = efitls_build(o, NULL, 0);
            if (EFI_ERROR(st)) {
                efitls_raise(st, "ClientHello");
            }
        }
        o->hs_state = HS_RUNNING;
    }

    for (;;) {
        int fs = efitls_flush(o);
        if (fs == SR_WOULDBLOCK) {
            o->poll_mask = MP_STREAM_POLL_WR;
            return SR_WOULDBLOCK;
        }

        EFI_TLS_SESSION_STATE state = EfiTlsSessionNotStarted;
        UINTN sz = sizeof(state);
        EFI_STATUS st = o->tls->GetSessionData(o->tls, EfiTlsSessionState, &state, &sz);
        if (!EFI_ERROR(st)) {
            if (state == EfiTlsSessionDataTransferring) {
                o->established = true;
                return SR_OK;
            }
            if (state == EfiTlsSessionError) {
                efitls_raise(EFI_ABORTED, "handshake");
            }
        }

        size_t rec_len;
        int s = efitls_get_record(o, &rec_len);
        if (s == SR_WOULDBLOCK) {
            o->poll_mask = MP_STREAM_POLL_RD;
            return SR_WOULDBLOCK;
        }
        if (s == SR_EOF) {
            efitls_raise(EFI_ABORTED, "handshake (peer closed)");
        }

        st = efitls_build(o, o->rx.buf, rec_len);
        bb_consume(&o->rx, rec_len);
        if (EFI_ERROR(st)) {
            efitls_raise(st, "handshake");
        }
    }
}

/******************************************************************************/
// Child lifecycle + context application.

static void efitls_destroy_child(mp_obj_ssl_socket_t *o) {
    if (o->tls_sb != NULL && o->child != NULL) {
        o->tls_sb->DestroyChild(o->tls_sb, o->child);
    }
    o->tls_sb = NULL;
    o->tls = NULL;
    o->child = NULL;
}

static void efitls_set_session(mp_obj_ssl_socket_t *o, EFI_TLS_SESSION_DATA_TYPE type,
    void *data, UINTN size, const char *what) {
    EFI_STATUS st = o->tls->SetSessionData(o->tls, type, data, size);
    if (EFI_ERROR(st)) {
        efitls_destroy_child(o);
        efitls_raise(st, what);
    }
}

static void efitls_set_config(mp_obj_ssl_socket_t *o, EFI_TLS_CONFIGURATION_PROTOCOL *cfg,
    EFI_TLS_CONFIG_DATA_TYPE type, mp_obj_t data_obj, const char *what) {
    size_t len;
    const char *data = mp_obj_str_get_data(data_obj, &len);
    EFI_STATUS st = cfg->SetData(cfg, type, (void *)data, len);
    if (EFI_ERROR(st)) {
        efitls_destroy_child(o);
        efitls_raise(st, what);
    }
}

static mp_obj_t ssl_socket_make_new(mp_obj_ssl_context_t *ctx, mp_obj_t sock,
    bool server_side, bool do_handshake_on_connect, mp_obj_t server_hostname) {

    mp_get_stream_raise(sock, MP_STREAM_OP_READ | MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);

    // Locate the firmware TLS provider (TlsDxe). Absent in the lean firmware —
    // surface a clean OSError so callers/tests can skip.
    EFI_TLS_SERVICE_BINDING_PROTOCOL *sb = NULL;
    EFI_STATUS st = BS->LocateProtocol((EFI_GUID *)&g_tls_sb_guid, NULL, (void **)&sb);
    if (EFI_ERROR(st) || sb == NULL) {
        mp_raise_OSError(MP_ENODEV);
    }

    mp_obj_ssl_socket_t *o = mp_obj_malloc_with_finaliser(mp_obj_ssl_socket_t, &ssl_socket_type);
    o->sock = sock;
    o->tls_sb = sb;
    o->tls = NULL;
    o->child = NULL;
    o->server_side = server_side;
    o->established = false;
    o->hs_state = HS_INIT;
    o->last_error = 0;
    o->poll_mask = 0;
    o->rx.buf = NULL;
    o->rx.len = 0;
    o->rx.cap = 0;
    o->plain.buf = NULL;
    o->plain.len = 0;
    o->plain.cap = 0;
    o->plain_off = 0;
    o->tx.buf = NULL;
    o->tx.len = 0;
    o->tx.cap = 0;
    o->tx_off = 0;

    // Create the TLS child and bind the record + configuration protocols.
    st = sb->CreateChild(sb, &o->child);
    if (EFI_ERROR(st)) {
        o->tls_sb = NULL;
        efitls_raise(st, "CreateChild");
    }
    EFI_TLS_CONFIGURATION_PROTOCOL *cfg = NULL;
    if (EFI_ERROR(BS->HandleProtocol(o->child, (EFI_GUID *)&g_tls_guid, (void **)&o->tls)) ||
        EFI_ERROR(BS->HandleProtocol(o->child, (EFI_GUID *)&g_tls_cfg_guid, (void **)&cfg))) {
        efitls_destroy_child(o);
        mp_raise_OSError(MP_EIO);
    }

    // ---- Apply the context: connection end, verify method, certs, SNI/host ----
    EFI_TLS_CONNECTION_END end = server_side ? EfiTlsServer : EfiTlsClient;
    efitls_set_session(o, EfiTlsConnectionEnd, &end, sizeof(end), "ConnectionEnd");

    EFI_TLS_VERIFY verify = EFI_TLS_VERIFY_NONE;
    if (ctx->authmode != MP_CERT_NONE) {
        verify = EFI_TLS_VERIFY_PEER;
        if (server_side && ctx->authmode == MP_CERT_REQUIRED) {
            verify |= EFI_TLS_VERIFY_FAIL_IF_NO_PEER_CERT;
        }
    }
    efitls_set_session(o, EfiTlsVerifyMethod, &verify, sizeof(verify), "VerifyMethod");

    if (ctx->cadata != MP_OBJ_NULL) {
        efitls_set_config(o, cfg, EfiTlsConfigDataTypeCACertificate, ctx->cadata, "CACertificate");
    }
    if (ctx->cert != MP_OBJ_NULL) {
        efitls_set_config(o, cfg, EfiTlsConfigDataTypeHostPublicCert, ctx->cert, "HostPublicCert");
    }
    if (ctx->key != MP_OBJ_NULL) {
        efitls_set_config(o, cfg, EfiTlsConfigDataTypeHostPrivateKey, ctx->key, "HostPrivateKey");
    }

    // SNI + hostname verification share the one VerifyHost call (SSL_set_tlsext_host_name
    // + X509_VERIFY_PARAM_set1_host in TlsDxe). Only valid when verifying.
    if (!server_side && verify != EFI_TLS_VERIFY_NONE && server_hostname != mp_const_none) {
        EFI_TLS_VERIFY_HOST vh;
        vh.Flags = EFI_TLS_VERIFY_FLAG_NONE;
        vh.HostName = (CHAR8 *)mp_obj_str_get_str(server_hostname);
        efitls_set_session(o, EfiTlsVerifyHost, &vh, sizeof(vh), "VerifyHost");
    }

    EFI_TLS_SESSION_STATE start = EfiTlsSessionNotStarted;
    efitls_set_session(o, EfiTlsSessionState, &start, sizeof(start), "SessionState");

    if (do_handshake_on_connect) {
        efitls_handshake(o);   // blocks on a blocking socket; raises EAGAIN otherwise
    }
    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/
// SSLContext type.

static mp_obj_t ssl_context_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw,
    const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t protocol = mp_obj_get_int(args[0]);

    mp_obj_ssl_context_t *self = mp_obj_malloc(mp_obj_ssl_context_t, type_in);
    // Client verifies by default; server does not (mirrors the mbedtls backend).
    self->authmode = (protocol & MP_ENDPOINT_IS_SERVER) ? MP_CERT_NONE : MP_CERT_REQUIRED;
    self->cadata = MP_OBJ_NULL;
    self->cert = MP_OBJ_NULL;
    self->key = MP_OBJ_NULL;
    self->ciphers = MP_OBJ_NULL;
    return MP_OBJ_FROM_PTR(self);
}

static void ssl_context_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_ssl_context_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load.
        if (attr == MP_QSTR_verify_mode) {
            dest[0] = MP_OBJ_NEW_SMALL_INT(self->authmode);
        } else {
            // Fall back to the locals dict (methods); unknown -> AttributeError.
            dest[1] = MP_OBJ_SENTINEL;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store (dest[1] holds the value; unknown attr leaves dest[0]!=NULL -> AttributeError).
        if (attr == MP_QSTR_verify_mode) {
            self->authmode = mp_obj_get_int(dest[1]);
            dest[0] = MP_OBJ_NULL;   // indicate success
        }
    }
}

static mp_obj_t ssl_context_load_cert_chain(size_t n_args, const mp_obj_t *args) {
    mp_obj_ssl_context_t *self = MP_OBJ_TO_PTR(args[0]);
    self->cert = args[1];
    if (n_args > 2 && args[2] != mp_const_none) {
        self->key = args[2];
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ssl_context_load_cert_chain_obj, 2, 3, ssl_context_load_cert_chain);

// Signature matches the mbedTLS backend exactly: a single positional `cadata`
// (PEM/DER trust anchors), not CPython's cafile-first form — this is the parity
// contract, and the `ssl` shim calls it positionally.
static mp_obj_t ssl_context_load_verify_locations(mp_obj_t self_in, mp_obj_t cadata) {
    mp_obj_ssl_context_t *self = MP_OBJ_TO_PTR(self_in);
    self->cadata = cadata;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_context_load_verify_locations_obj, ssl_context_load_verify_locations);

static mp_obj_t ssl_context_get_ciphers(mp_obj_t self_in) {
    mp_obj_ssl_context_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->ciphers != MP_OBJ_NULL) {
        return self->ciphers;
    }
    return mp_obj_new_list(0, NULL);
}
static MP_DEFINE_CONST_FUN_OBJ_1(ssl_context_get_ciphers_obj, ssl_context_get_ciphers);

static mp_obj_t ssl_context_set_ciphers(mp_obj_t self_in, mp_obj_t ciphers) {
    // Stored for API parity; the EFI backend negotiates suites in firmware.
    mp_obj_ssl_context_t *self = MP_OBJ_TO_PTR(self_in);
    self->ciphers = ciphers;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_context_set_ciphers_obj, ssl_context_set_ciphers);

static mp_obj_t ssl_context_wrap_socket(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_server_side, ARG_do_handshake_on_connect, ARG_server_hostname };
    static const mp_arg_t allowed[] = {
        { MP_QSTR_server_side, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_do_handshake_on_connect, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_server_hostname, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_obj_ssl_context_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_obj_t sock = pos_args[1];
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed), allowed, vals);
    return ssl_socket_make_new(self, sock, vals[ARG_server_side].u_bool,
        vals[ARG_do_handshake_on_connect].u_bool, vals[ARG_server_hostname].u_obj);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(ssl_context_wrap_socket_obj, 2, ssl_context_wrap_socket);

static const mp_rom_map_elem_t ssl_context_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_ciphers), MP_ROM_PTR(&ssl_context_get_ciphers_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_ciphers), MP_ROM_PTR(&ssl_context_set_ciphers_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_cert_chain), MP_ROM_PTR(&ssl_context_load_cert_chain_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_verify_locations), MP_ROM_PTR(&ssl_context_load_verify_locations_obj) },
    { MP_ROM_QSTR(MP_QSTR_wrap_socket), MP_ROM_PTR(&ssl_context_wrap_socket_obj) },
};
static MP_DEFINE_CONST_DICT(ssl_context_locals_dict, ssl_context_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    ssl_context_type,
    MP_QSTR_SSLContext,
    MP_TYPE_FLAG_NONE,
    make_new, ssl_context_make_new,
    attr, ssl_context_attr,
    locals_dict, &ssl_context_locals_dict
    );

/******************************************************************************/
// SSLSocket stream methods.

static mp_uint_t ssl_socket_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(o_in);
    o->poll_mask = 0;
    if (o->last_error) {
        *errcode = o->last_error;
        return MP_STREAM_ERROR;
    }
    if (o->sock == MP_OBJ_NULL) {
        return 0;
    }
    if (!o->established) {
        if (efitls_handshake(o) == SR_WOULDBLOCK) {
            *errcode = MP_EAGAIN;
            return MP_STREAM_ERROR;
        }
    }
    // Push any pending outbound bytes (e.g. our request) before waiting.
    if (efitls_flush(o) == SR_WOULDBLOCK) {
        o->poll_mask = MP_STREAM_POLL_WR;
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    for (;;) {
        if (o->plain.len > o->plain_off) {
            size_t avail = o->plain.len - o->plain_off;
            size_t n = size < avail ? size : avail;
            memcpy(buf, o->plain.buf + o->plain_off, n);
            o->plain_off += n;
            if (o->plain_off == o->plain.len) {
                o->plain.len = 0;
                o->plain_off = 0;
            }
            return n;
        }

        size_t rec_len;
        int s = efitls_get_record(o, &rec_len);
        if (s == SR_WOULDBLOCK) {
            o->poll_mask = MP_STREAM_POLL_RD;
            *errcode = MP_EAGAIN;
            return MP_STREAM_ERROR;
        }
        if (s == SR_EOF) {
            return 0;
        }

        UINT8 ct = o->rx.buf[0];
        if (ct == TLS_CONTENT_TYPE_APPLICATION_DATA) {
            UINT8 *out;
            UINTN outlen;
            EFI_STATUS st = efitls_process(o->tls, o->rx.buf, rec_len, EfiTlsDecrypt, &out, &outlen);
            bb_consume(&o->rx, rec_len);
            if (EFI_ERROR(st)) {
                o->last_error = MP_EIO;
                *errcode = MP_EIO;
                return MP_STREAM_ERROR;
            }
            if (outlen > TLS_RECORD_HEADER_LENGTH) {
                bb_append(&o->plain, out + TLS_RECORD_HEADER_LENGTH, outlen - TLS_RECORD_HEADER_LENGTH);
            }
            efi_free(out);
            continue;   // serve from plain
        } else if (ct == TLS_CONTENT_TYPE_ALERT) {
            // Most alerts here are close_notify -> end of stream.
            bb_consume(&o->rx, rec_len);
            return 0;
        } else {
            // Post-handshake handshake/CCS record (e.g. TLS1.3 session ticket):
            // feed it to the engine, flush any reply, keep reading.
            EFI_STATUS st = efitls_build(o, o->rx.buf, rec_len);
            bb_consume(&o->rx, rec_len);
            if (EFI_ERROR(st)) {
                o->last_error = MP_EIO;
                *errcode = MP_EIO;
                return MP_STREAM_ERROR;
            }
            efitls_flush(o);
            continue;
        }
    }
}

static mp_uint_t ssl_socket_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(o_in);
    o->poll_mask = 0;
    if (o->last_error) {
        *errcode = o->last_error;
        return MP_STREAM_ERROR;
    }
    if (o->sock == MP_OBJ_NULL) {
        *errcode = MP_EPIPE;
        return MP_STREAM_ERROR;
    }
    if (!o->established) {
        if (efitls_handshake(o) == SR_WOULDBLOCK) {
            *errcode = MP_EAGAIN;
            return MP_STREAM_ERROR;
        }
    }
    // Drain a previous partial record before accepting new data (so we never
    // double-encrypt on retry).
    if (efitls_flush(o) == SR_WOULDBLOCK) {
        o->poll_mask = MP_STREAM_POLL_WR;
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    const byte *p = buf;
    size_t remaining = size;
    while (remaining) {
        size_t chunk = remaining > 16384 ? 16384 : remaining;
        UINT8 *msg = efi_alloc(TLS_RECORD_HEADER_LENGTH + chunk);
        msg[0] = TLS_CONTENT_TYPE_APPLICATION_DATA;
        msg[1] = TLS12_VERSION_MAJOR;
        msg[2] = TLS12_VERSION_MINOR;
        // TlsEncryptPacket reads the header Length field in HOST byte order (no
        // NTOHS) at struct offset 3 as the plaintext size — so little-endian here.
        msg[3] = (UINT8)(chunk & 0xff);
        msg[4] = (UINT8)((chunk >> 8) & 0xff);
        memcpy(msg + TLS_RECORD_HEADER_LENGTH, p, chunk);
        UINT8 *out;
        UINTN outlen;
        EFI_STATUS st = efitls_process(o->tls, msg, TLS_RECORD_HEADER_LENGTH + chunk,
            EfiTlsEncrypt, &out, &outlen);
        efi_free(msg);
        if (EFI_ERROR(st)) {
            o->last_error = MP_EIO;
            *errcode = MP_EIO;
            return MP_STREAM_ERROR;
        }
        bb_append(&o->tx, out, outlen);
        efi_free(out);
        p += chunk;
        remaining -= chunk;
    }
    // Best-effort drain; any remainder stays in tx and flushes on the next call.
    efitls_flush(o);
    return size;
}

static mp_uint_t ssl_socket_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_ssl_socket_t *self = MP_OBJ_TO_PTR(o_in);
    mp_uint_t ret = 0;
    uintptr_t saved_arg = 0;
    mp_obj_t sock = self->sock;

    if (request == MP_STREAM_CLOSE) {
        if (sock == MP_OBJ_NULL) {
            return 0;
        }
        // Best-effort close_notify.
        if (self->established && self->tls != NULL) {
            EFI_TLS_SESSION_STATE closing = EfiTlsSessionClosing;
            self->tls->SetSessionData(self->tls, EfiTlsSessionState, &closing, sizeof(closing));
            if (efitls_build(self, NULL, 0) == EFI_SUCCESS) {
                efitls_flush(self);   // ignore would-block on the way out
            }
        }
        efitls_destroy_child(self);
        self->sock = MP_OBJ_NULL;
        // Fall through to delegate CLOSE to the underlying socket.
    } else if (request == MP_STREAM_POLL) {
        if (sock == MP_OBJ_NULL || self->last_error != 0) {
            return MP_STREAM_POLL_NVAL;
        }
        // Redirect to the direction the engine is waiting on (handshake/would-block).
        if (self->poll_mask && (arg & MP_STREAM_POLL_RDWR)) {
            saved_arg = arg & MP_STREAM_POLL_RDWR;
            arg = (arg & ~saved_arg) | self->poll_mask;
        }
        // Buffered plaintext is immediately readable.
        if ((arg & MP_STREAM_POLL_RD) && self->plain.len > self->plain_off) {
            ret |= MP_STREAM_POLL_RD;
            if (arg == MP_STREAM_POLL_RD) {
                return MP_STREAM_POLL_RD;
            }
        }
    } else {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }

    ret |= mp_get_stream(sock)->ioctl(sock, request, arg, errcode);
    if (request == MP_STREAM_POLL && (ret & self->poll_mask)) {
        ret |= saved_arg;
    }
    return ret;
}

static mp_obj_t ssl_socket_setblocking(mp_obj_t self_in, mp_obj_t flag_in) {
    mp_obj_ssl_socket_t *o = MP_OBJ_TO_PTR(self_in);
    if (o->sock == MP_OBJ_NULL) {
        mp_raise_OSError(MP_EBADF);
    }
    mp_obj_t dest[3];
    mp_load_method(o->sock, MP_QSTR_setblocking, dest);
    dest[2] = flag_in;
    return mp_call_method_n_kw(1, 0, dest);
}
static MP_DEFINE_CONST_FUN_OBJ_2(ssl_socket_setblocking_obj, ssl_socket_setblocking);

static const mp_rom_map_elem_t ssl_socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&mp_stream_read1_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv_into), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&mp_stream_write1_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendall), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&ssl_socket_setblocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&mp_stream_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(ssl_socket_locals_dict, ssl_socket_locals_dict_table);

static const mp_stream_p_t ssl_socket_stream_p = {
    .read = ssl_socket_read,
    .write = ssl_socket_write,
    .ioctl = ssl_socket_ioctl,
};

static MP_DEFINE_CONST_OBJ_TYPE(
    ssl_socket_type,
    MP_QSTR_SSLSocket,
    MP_TYPE_FLAG_NONE,
    protocol, &ssl_socket_stream_p,
    locals_dict, &ssl_socket_locals_dict
    );

/******************************************************************************/
// Module.

static const mp_rom_map_elem_t mp_module_tls_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tls) },
    { MP_ROM_QSTR(MP_QSTR_SSLContext), MP_ROM_PTR(&ssl_context_type) },
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_TLS_CLIENT), MP_ROM_INT(MP_PROTOCOL_TLS_CLIENT) },
    { MP_ROM_QSTR(MP_QSTR_PROTOCOL_TLS_SERVER), MP_ROM_INT(MP_PROTOCOL_TLS_SERVER) },
    { MP_ROM_QSTR(MP_QSTR_CERT_NONE), MP_ROM_INT(MP_CERT_NONE) },
    { MP_ROM_QSTR(MP_QSTR_CERT_OPTIONAL), MP_ROM_INT(MP_CERT_OPTIONAL) },
    { MP_ROM_QSTR(MP_QSTR_CERT_REQUIRED), MP_ROM_INT(MP_CERT_REQUIRED) },
};
static MP_DEFINE_CONST_DICT(mp_module_tls_globals, mp_module_tls_globals_table);

const mp_obj_module_t mp_module_tls = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_tls_globals,
};

MP_REGISTER_MODULE(MP_QSTR_tls, mp_module_tls);

#endif // MICROPY_PY_SSL && MICROPY_SSL_EFITLS
