// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// uefi.GUID: the EFI_GUID type, as a buffer.
//
// A GUID's whole job is to be 16 packed bytes handed to firmware, so the type
// implements the buffer protocol rather than hiding the bytes behind an
// attribute. That makes a GUID directly usable everywhere a 16-byte buffer is:
// `bytes(g)`, `memoryview(g)`, `uctypes.addressof(g)`, `struct`, and every
// GUID parameter of the raw C surface — no `.bytes` at the call site. (A pure
// Python class cannot supply the buffer protocol, which is why this type is
// here in C and not in uefi/guid.py; that module keeps the well-known-GUID
// registry built on top of it.)
//
// The buffer is READ-only on purpose: the registry hands out shared instances
// (uefi.guid.BLOCK_IO_PROTOCOL and friends), so letting firmware write through
// one would corrupt every later lookup.
//
// Storage is the packed mixed-endian EFI_GUID: Data1 (u32) and Data2/Data3
// (u16) little-endian, Data4 (the last 8 bytes) in order. The canonical text
// form is "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
//
// Like uefi.Event, the type registers no module of its own: moduefi_raw.c folds
// it into the `_uefi` globals table as `_uefi.GUID`.

#include <string.h>
#include "py/runtime.h"
#include "py/objstr.h"
#include "uefi_port.h"
#include "efi.h"

typedef struct _uefi_guid_obj_t {
    mp_obj_base_t base;
    uint8_t data[16];
} uefi_guid_obj_t;

const mp_obj_type_t uefi_guid_type;

static int hexval(unsigned c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

// Parse the canonical hyphenated form, a bare 32-hex-digit string, or either
// wrapped in {braces}; case-insensitive, surrounding whitespace ignored.
// Returns false if `s` is not a GUID string. Writes the packed 16 bytes.
static bool guid_parse(const char *s, size_t len, uint8_t *out) {
    // Trim whitespace, then one optional layer of braces.
    while (len > 0 && (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r')) {
        s++;
        len--;
    }
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' || s[len - 1] == '\n' || s[len - 1] == '\r')) {
        len--;
    }
    if (len >= 2 && s[0] == '{' && s[len - 1] == '}') {
        s++;
        len -= 2;
    }
    // Collect exactly 32 hex digits, skipping hyphens.
    uint8_t raw[16];
    size_t n = 0;
    for (size_t i = 0; i < len; i++) {
        if (s[i] == '-') {
            continue;
        }
        int hi = hexval((unsigned char)s[i]);
        if (hi < 0 || i + 1 >= len) {
            return false;
        }
        int lo = hexval((unsigned char)s[i + 1]);
        if (lo < 0) {
            return false;
        }
        if (n >= 16) {
            return false;
        }
        raw[n++] = (uint8_t)((hi << 4) | lo);
        i++;
    }
    if (n != 16) {
        return false;
    }
    // Byte-swap Data1/Data2/Data3 into the packed mixed-endian layout; Data4
    // (the final 8 bytes) is already in order.
    out[0] = raw[3];
    out[1] = raw[2];
    out[2] = raw[1];
    out[3] = raw[0];
    out[4] = raw[5];
    out[5] = raw[4];
    out[6] = raw[7];
    out[7] = raw[6];
    memcpy(out + 8, raw + 8, 8);
    return true;
}

// Fill `out` (16 bytes) from any accepted GUID spelling: a GUID, a 16-byte
// buffer, or a GUID string. Returns false if the object is none of those.
bool mp_uefi_guid_from_obj(mp_obj_t obj, uint8_t *out) {
    if (mp_obj_is_type(obj, &uefi_guid_type)) {
        memcpy(out, ((uefi_guid_obj_t *)MP_OBJ_TO_PTR(obj))->data, 16);
        return true;
    }
    if (mp_obj_is_str(obj)) {
        size_t len;
        const char *s = mp_obj_str_get_data(obj, &len);
        return guid_parse(s, len, out);
    }
    mp_buffer_info_t bi;
    if (mp_get_buffer(obj, &bi, MP_BUFFER_READ)) {
        if (bi.len != 16) {
            return false;
        }
        memcpy(out, bi.buf, 16);
        return true;
    }
    return false;
}

static mp_obj_t uefi_guid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    // A GUID is immutable, so re-wrapping one can hand back the same object.
    if (mp_obj_is_type(args[0], &uefi_guid_type)) {
        return args[0];
    }
    uefi_guid_obj_t *self = mp_obj_malloc(uefi_guid_obj_t, &uefi_guid_type);
    if (!mp_uefi_guid_from_obj(args[0], self->data)) {
        if (mp_obj_is_str(args[0])) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad GUID string"));
        }
        mp_buffer_info_t bi;
        if (mp_get_buffer(args[0], &bi, MP_BUFFER_READ)) {
            mp_raise_ValueError(MP_ERROR_TEXT("GUID needs 16 bytes"));
        }
        mp_raise_TypeError(MP_ERROR_TEXT("GUID expects a GUID, 16 bytes, or a string"));
    }
    return MP_OBJ_FROM_PTR(self);
}

static void uefi_guid_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    const uint8_t *b = ((uefi_guid_obj_t *)MP_OBJ_TO_PTR(self_in))->data;
    if (kind == PRINT_REPR) {
        mp_print_str(print, "GUID('");
    }
    mp_printf(print, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        b[3], b[2], b[1], b[0], b[5], b[4], b[7], b[6],
        b[8], b[9], b[10], b[11], b[12], b[13], b[14], b[15]);
    if (kind == PRINT_REPR) {
        mp_print_str(print, "')");
    }
}

// The point of the type: 16 packed bytes, READ-only (see the header comment).
static mp_int_t uefi_guid_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    uefi_guid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (flags & MP_BUFFER_WRITE) {
        return 1;
    }
    bufinfo->buf = self->data;
    bufinfo->len = 16;
    bufinfo->typecode = 'B';
    return 0;
}

static mp_obj_t uefi_guid_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    uefi_guid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_HASH:
            return MP_OBJ_NEW_SMALL_INT(qstr_compute_hash(self->data, 16));
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(16);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

// Equality is GUID-to-GUID only: comparing against loose bytes would make a
// GUID and its packed form interchangeable as dict keys, which they are not.
static mp_obj_t uefi_guid_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    if (op != MP_BINARY_OP_EQUAL) {
        return MP_OBJ_NULL;
    }
    if (!mp_obj_is_type(rhs_in, &uefi_guid_type)) {
        return mp_const_false;
    }
    uefi_guid_obj_t *lhs = MP_OBJ_TO_PTR(lhs_in);
    uefi_guid_obj_t *rhs = MP_OBJ_TO_PTR(rhs_in);
    return mp_obj_new_bool(memcmp(lhs->data, rhs->data, 16) == 0);
}

// `.bytes`: the packed form as a bytes object. The buffer protocol makes this
// redundant for passing a GUID onwards, but it stays for code that wants an
// owned copy (and for the pre-buffer-protocol spelling).
static void uefi_guid_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL && attr == MP_QSTR_bytes) {
        uefi_guid_obj_t *self = MP_OBJ_TO_PTR(self_in);
        dest[0] = mp_obj_new_bytes(self->data, 16);
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    uefi_guid_type,
    MP_QSTR_GUID,
    MP_TYPE_FLAG_NONE,
    make_new, uefi_guid_make_new,
    print, uefi_guid_print,
    attr, uefi_guid_attr,
    unary_op, uefi_guid_unary_op,
    binary_op, uefi_guid_binary_op,
    buffer, uefi_guid_get_buffer
    );
