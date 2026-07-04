/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// The `_uefi` C module: the low-level (raw) surface for the `uefi` Python package.
//
// It exposes thin, policy-free wrappers around Boot/Runtime Services table calls
// plus the one thing that cannot be done in Python: `call`, an EFIAPI call-through
// trampoline that invokes a protocol's function pointers. The whole port is built
// with the UEFI/MS calling convention as the C default (EFIAPI is empty), so a
// plain indirect C call *is* an EFIAPI call — no assembly, no attribute needed.
//
// Everything here returns raw status ints (or tuples including a status); nothing
// raises. The pythonic layer (uefi.status.check) maps errors to uefi.Error. The
// table pointers come straight from the port globals (mp_uefi_st / mp_uefi_image),
// so there is no separate `_init` step.

#include <string.h>
#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/unicode.h"
#include "uefi_port.h"
#include "efi.h"

#define BS (mp_uefi_st->BootServices)
#define RS (mp_uefi_st->RuntimeServices)

// The pollable EVT_NOTIFY_SIGNAL event type, defined in modefi.c; folded into the
// `_uefi` module so the Python layer reaches it as uefi.Event.
extern const mp_obj_type_t efi_event_type;

// ── The trampoline ──────────────────────────────────────────────────────────
// A single fixed-arity prototype. We always call with the maximum arg count,
// zero-padding the unused tail: EFIAPI is caller-cleanup and the args are all
// integer/pointer, so surplus args are harmless (the callee ignores them).
#define EFI_CALL_MAX_ARGS 16
typedef UINTN (*efi_call16_t)(UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN,
    UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN, UINTN);

static mp_obj_t uefi_raw_call(size_t n_args, const mp_obj_t *args) {
    // args[0] = function pointer; args[1..] = up to 16 integer/pointer args.
    UINTN fn = (UINTN)mp_obj_get_int_truncated(args[0]);
    if (fn == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("null function pointer"));
    }
    UINTN a[EFI_CALL_MAX_ARGS];
    size_t n = n_args - 1;
    for (size_t i = 0; i < EFI_CALL_MAX_ARGS; i++) {
        a[i] = (i < n) ? (UINTN)mp_obj_get_int_truncated(args[1 + i]) : 0;
    }
    UINTN r = ((efi_call16_t)fn)(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7],
        a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]);
    return mp_obj_new_int_from_uint(r);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_call_obj, 1, 1 + EFI_CALL_MAX_ARGS, uefi_raw_call);

// ── Helpers ─────────────────────────────────────────────────────────────────
static mp_obj_t status_and_uint(EFI_STATUS st, UINTN value) {
    mp_obj_t items[2] = { mp_obj_new_int_from_uint(st), mp_obj_new_int_from_uint(value) };
    return mp_obj_new_tuple(2, items);
}

// Pull a 16-byte packed EFI_GUID out of a bytes-like argument.
static EFI_GUID *guid_from_obj(mp_obj_t obj) {
    mp_buffer_info_t bi;
    mp_get_buffer_raise(obj, &bi, MP_BUFFER_READ);
    if (bi.len != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("guid must be 16 bytes"));
    }
    return (EFI_GUID *)bi.buf;
}

// Convert a Python str (UTF-8) to a null-terminated CHAR16 (UTF-16LE) buffer in
// `out` (capacity `cap` units incl. terminator). Returns units written (excl.
// terminator); raises on overflow. Used for UEFI variable names (CHAR16*).
static size_t str_to_char16le(mp_obj_t str_in, CHAR16 *out, size_t cap) {
    size_t blen;
    const char *s = mp_obj_str_get_data(str_in, &blen);
    const byte *p = (const byte *)s;
    const byte *end = p + blen;
    size_t n = 0;
    while (p < end) {
        unichar c = utf8_get_char(p);
        p = utf8_next_char(p);
        if (c <= 0xFFFF) {
            if (n + 1 >= cap) {
                mp_raise_ValueError(MP_ERROR_TEXT("name too long"));
            }
            out[n++] = (CHAR16)c;
        } else {
            if (n + 2 >= cap) {
                mp_raise_ValueError(MP_ERROR_TEXT("name too long"));
            }
            c -= 0x10000;
            out[n++] = (CHAR16)(0xD800 + (c >> 10));
            out[n++] = (CHAR16)(0xDC00 + (c & 0x3FF));
        }
    }
    if (n >= cap) {
        mp_raise_ValueError(MP_ERROR_TEXT("name too long"));
    }
    out[n] = 0;
    return n;
}

static void vstr_add_unichar(vstr_t *v, uint32_t c) {
    if (c < 0x80) {
        vstr_add_byte(v, c);
    } else if (c < 0x800) {
        vstr_add_byte(v, 0xC0 | (c >> 6));
        vstr_add_byte(v, 0x80 | (c & 0x3F));
    } else if (c < 0x10000) {
        vstr_add_byte(v, 0xE0 | (c >> 12));
        vstr_add_byte(v, 0x80 | ((c >> 6) & 0x3F));
        vstr_add_byte(v, 0x80 | (c & 0x3F));
    } else {
        vstr_add_byte(v, 0xF0 | (c >> 18));
        vstr_add_byte(v, 0x80 | ((c >> 12) & 0x3F));
        vstr_add_byte(v, 0x80 | ((c >> 6) & 0x3F));
        vstr_add_byte(v, 0x80 | (c & 0x3F));
    }
}

// Decode a null-terminated CHAR16 (UTF-16LE) buffer into a Python str.
static mp_obj_t char16le_to_str(const CHAR16 *s) {
    vstr_t vstr;
    vstr_init(&vstr, 16);
    const CHAR16 *p = s;
    while (*p) {
        uint32_t c = *p++;
        if (c >= 0xD800 && c <= 0xDBFF && *p >= 0xDC00 && *p <= 0xDFFF) {
            uint32_t lo = *p++;
            c = 0x10000 + ((c - 0xD800) << 10) + (lo - 0xDC00);
        }
        vstr_add_unichar(&vstr, c);
    }
    mp_obj_t res = mp_obj_new_str(vstr.buf, vstr.len);
    vstr_clear(&vstr);
    return res;
}

// ── Table base pointers ─────────────────────────────────────────────────────
static mp_obj_t uefi_raw_system_table_ptr(void) {
    return mp_obj_new_int_from_uint((UINTN)(uintptr_t)mp_uefi_st);
}
static MP_DEFINE_CONST_FUN_OBJ_0(uefi_raw_system_table_ptr_obj, uefi_raw_system_table_ptr);

static mp_obj_t uefi_raw_boot_services_ptr(void) {
    return mp_obj_new_int_from_uint((UINTN)(uintptr_t)mp_uefi_st->BootServices);
}
static MP_DEFINE_CONST_FUN_OBJ_0(uefi_raw_boot_services_ptr_obj, uefi_raw_boot_services_ptr);

static mp_obj_t uefi_raw_runtime_services_ptr(void) {
    return mp_obj_new_int_from_uint((UINTN)(uintptr_t)mp_uefi_st->RuntimeServices);
}
static MP_DEFINE_CONST_FUN_OBJ_0(uefi_raw_runtime_services_ptr_obj, uefi_raw_runtime_services_ptr);

static mp_obj_t uefi_raw_image_handle(void) {
    return mp_obj_new_int_from_uint((UINTN)(uintptr_t)mp_uefi_image);
}
static MP_DEFINE_CONST_FUN_OBJ_0(uefi_raw_image_handle_obj, uefi_raw_image_handle);

static mp_obj_t uefi_raw_boot_services_active(void) {
    // The port is a pre-ExitBootServices application that leaves via reset, so
    // Boot Services are live for our whole lifetime.
    return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_0(uefi_raw_boot_services_active_obj, uefi_raw_boot_services_active);

// ── Memory ──────────────────────────────────────────────────────────────────
static mp_obj_t uefi_raw_allocate_pool(mp_obj_t type_in, mp_obj_t size_in) {
    EFI_MEMORY_TYPE mt = (EFI_MEMORY_TYPE)mp_obj_get_int(type_in);
    UINTN size = (UINTN)mp_obj_get_int_truncated(size_in);
    void *buf = NULL;
    EFI_STATUS st = BS->AllocatePool(mt, size, &buf);
    return status_and_uint(st, (UINTN)(uintptr_t)buf);
}
static MP_DEFINE_CONST_FUN_OBJ_2(uefi_raw_allocate_pool_obj, uefi_raw_allocate_pool);

static mp_obj_t uefi_raw_free_pool(mp_obj_t ptr_in) {
    EFI_STATUS st = BS->FreePool((void *)(uintptr_t)mp_obj_get_int_truncated(ptr_in));
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_raw_free_pool_obj, uefi_raw_free_pool);

// ── Handle database ─────────────────────────────────────────────────────────
static mp_obj_t uefi_raw_locate_handle_buffer(mp_obj_t search_type_in, mp_obj_t guid_in) {
    EFI_LOCATE_SEARCH_TYPE stype = (EFI_LOCATE_SEARCH_TYPE)mp_obj_get_int(search_type_in);
    EFI_GUID *guid = (guid_in == mp_const_none) ? NULL : guid_from_obj(guid_in);
    UINTN count = 0;
    EFI_HANDLE *buf = NULL;
    EFI_STATUS st = BS->LocateHandleBuffer(stype, guid, NULL, &count, &buf);
    mp_obj_t list = mp_obj_new_list(0, NULL);
    if (!EFI_ERROR(st) && buf != NULL) {
        for (UINTN i = 0; i < count; i++) {
            mp_obj_list_append(list, mp_obj_new_int_from_uint((UINTN)(uintptr_t)buf[i]));
        }
        BS->FreePool(buf);
    }
    mp_obj_t items[2] = { mp_obj_new_int_from_uint(st), list };
    return mp_obj_new_tuple(2, items);
}
static MP_DEFINE_CONST_FUN_OBJ_2(uefi_raw_locate_handle_buffer_obj, uefi_raw_locate_handle_buffer);

static mp_obj_t uefi_raw_open_protocol(size_t n_args, const mp_obj_t *args) {
    // (handle, guid, agent, controller, attributes) -> (status, interface_ptr)
    EFI_HANDLE handle = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[0]);
    EFI_GUID *guid = guid_from_obj(args[1]);
    EFI_HANDLE agent = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[2]);
    EFI_HANDLE controller = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[3]);
    UINT32 attributes = (UINT32)mp_obj_get_int_truncated(args[4]);
    void *iface = NULL;
    EFI_STATUS st = BS->OpenProtocol(handle, guid, &iface, agent, controller, attributes);
    return status_and_uint(st, (UINTN)(uintptr_t)iface);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_open_protocol_obj, 5, 5, uefi_raw_open_protocol);

static mp_obj_t uefi_raw_close_protocol(size_t n_args, const mp_obj_t *args) {
    // (handle, guid, agent, controller) -> status
    EFI_HANDLE handle = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[0]);
    EFI_GUID *guid = guid_from_obj(args[1]);
    EFI_HANDLE agent = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[2]);
    EFI_HANDLE controller = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[3]);
    EFI_STATUS st = BS->CloseProtocol(handle, guid, agent, controller);
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_close_protocol_obj, 4, 4, uefi_raw_close_protocol);

static mp_obj_t uefi_raw_protocols_per_handle(mp_obj_t handle_in) {
    EFI_HANDLE handle = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(handle_in);
    EFI_GUID **guids = NULL;
    UINTN count = 0;
    EFI_STATUS st = BS->ProtocolsPerHandle(handle, &guids, &count);
    mp_obj_t list = mp_obj_new_list(0, NULL);
    if (!EFI_ERROR(st) && guids != NULL) {
        for (UINTN i = 0; i < count; i++) {
            mp_obj_list_append(list, mp_obj_new_bytes((const byte *)guids[i], 16));
        }
        BS->FreePool(guids);
    }
    mp_obj_t items[2] = { mp_obj_new_int_from_uint(st), list };
    return mp_obj_new_tuple(2, items);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_raw_protocols_per_handle_obj, uefi_raw_protocols_per_handle);

// open_protocol_information(handle, guid) -> (status, [(agent, controller,
// attributes, open_count), ...]). The usage view behind `dh -d` / `devices`:
// which agents have a protocol open on a handle, and how (BY_DRIVER, etc.).
static mp_obj_t uefi_raw_open_protocol_information(mp_obj_t handle_in, mp_obj_t guid_in) {
    EFI_HANDLE handle = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(handle_in);
    EFI_GUID *guid = guid_from_obj(guid_in);
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *entries = NULL;
    UINTN count = 0;
    EFI_STATUS st = BS->OpenProtocolInformation(handle, guid, &entries, &count);
    mp_obj_t list = mp_obj_new_list(0, NULL);
    if (!EFI_ERROR(st) && entries != NULL) {
        for (UINTN i = 0; i < count; i++) {
            mp_obj_t item[4] = {
                mp_obj_new_int_from_uint((UINTN)(uintptr_t)entries[i].AgentHandle),
                mp_obj_new_int_from_uint((UINTN)(uintptr_t)entries[i].ControllerHandle),
                mp_obj_new_int_from_uint(entries[i].Attributes),
                mp_obj_new_int_from_uint(entries[i].OpenCount),
            };
            mp_obj_list_append(list, mp_obj_new_tuple(4, item));
        }
        BS->FreePool(entries);
    }
    mp_obj_t items[2] = { mp_obj_new_int_from_uint(st), list };
    return mp_obj_new_tuple(2, items);
}
static MP_DEFINE_CONST_FUN_OBJ_2(uefi_raw_open_protocol_information_obj, uefi_raw_open_protocol_information);

static mp_obj_t uefi_raw_locate_protocol(size_t n_args, const mp_obj_t *args) {
    EFI_GUID *guid = guid_from_obj(args[0]);
    void *registration = (n_args > 1) ? (void *)(uintptr_t)mp_obj_get_int_truncated(args[1]) : NULL;
    void *iface = NULL;
    EFI_STATUS st = BS->LocateProtocol(guid, registration, &iface);
    return status_and_uint(st, (UINTN)(uintptr_t)iface);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_locate_protocol_obj, 1, 2, uefi_raw_locate_protocol);

// ── Variables (Runtime Services) ────────────────────────────────────────────
// get_variable(name, guid) -> (status, attributes, data_bytes|None). Does the
// two-phase size probe internally; data is None on any error (e.g. NOT_FOUND).
static mp_obj_t uefi_raw_get_variable(mp_obj_t name_in, mp_obj_t guid_in) {
    CHAR16 name[256];
    str_to_char16le(name_in, name, MP_ARRAY_SIZE(name));
    EFI_GUID *guid = guid_from_obj(guid_in);
    UINT32 attrs = 0;
    UINTN size = 0;
    EFI_STATUS st = RS->GetVariable(name, guid, &attrs, &size, NULL);
    mp_obj_t data = mp_const_none;
    if (st == EFI_BUFFER_TOO_SMALL) {
        void *buf = NULL;
        EFI_STATUS as = BS->AllocatePool(EfiBootServicesData, size, &buf);
        if (EFI_ERROR(as)) {
            st = as;
        } else {
            st = RS->GetVariable(name, guid, &attrs, &size, buf);
            if (!EFI_ERROR(st)) {
                data = mp_obj_new_bytes((const byte *)buf, size);
            }
            BS->FreePool(buf);
        }
    } else if (!EFI_ERROR(st)) {
        // Zero-length variable: success with size 0.
        data = mp_obj_new_bytes((const byte *)"", 0);
    }
    mp_obj_t items[3] = { mp_obj_new_int_from_uint(st), mp_obj_new_int_from_uint(attrs), data };
    return mp_obj_new_tuple(3, items);
}
static MP_DEFINE_CONST_FUN_OBJ_2(uefi_raw_get_variable_obj, uefi_raw_get_variable);

// set_variable(name, guid, attributes, data|None) -> status. data None or empty
// with attributes 0 deletes the variable.
static mp_obj_t uefi_raw_set_variable(size_t n_args, const mp_obj_t *args) {
    CHAR16 name[256];
    str_to_char16le(args[0], name, MP_ARRAY_SIZE(name));
    EFI_GUID *guid = guid_from_obj(args[1]);
    UINT32 attrs = (UINT32)mp_obj_get_int_truncated(args[2]);
    void *data = NULL;
    UINTN size = 0;
    if (args[3] != mp_const_none) {
        mp_buffer_info_t bi;
        mp_get_buffer_raise(args[3], &bi, MP_BUFFER_READ);
        data = bi.buf;
        size = bi.len;
    }
    EFI_STATUS st = RS->SetVariable(name, guid, attrs, size, data);
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_set_variable_obj, 4, 4, uefi_raw_set_variable);

// get_next_variable_name(name, guid) -> (status, next_name, next_guid_bytes).
// Pass name="" and any guid to start; feed each result back to step the walk.
static mp_obj_t uefi_raw_get_next_variable_name(mp_obj_t name_in, mp_obj_t guid_in) {
    CHAR16 stackbuf[512];
    EFI_GUID g;
    memcpy(&g, guid_from_obj(guid_in), sizeof(EFI_GUID));
    size_t units = str_to_char16le(name_in, stackbuf, MP_ARRAY_SIZE(stackbuf));
    UINTN namebytes = (units + 1) * sizeof(CHAR16);

    CHAR16 *buf = stackbuf;
    void *alloc = NULL;
    UINTN size = sizeof(stackbuf);
    EFI_STATUS st = RS->GetNextVariableName(&size, buf, &g);
    if (st == EFI_BUFFER_TOO_SMALL) {
        // Name longer than our stack buffer: allocate the required size, re-seed
        // the current name (the failed call left our inputs untouched), retry.
        EFI_STATUS as = BS->AllocatePool(EfiBootServicesData, size, &alloc);
        if (EFI_ERROR(as)) {
            mp_obj_t e[3] = { mp_obj_new_int_from_uint(as), mp_const_none, mp_const_none };
            return mp_obj_new_tuple(3, e);
        }
        memcpy(alloc, stackbuf, namebytes);
        buf = (CHAR16 *)alloc;
        st = RS->GetNextVariableName(&size, buf, &g);
    }
    mp_obj_t name_obj = mp_const_none, guid_obj = mp_const_none;
    if (!EFI_ERROR(st)) {
        name_obj = char16le_to_str(buf);
        guid_obj = mp_obj_new_bytes((const byte *)&g, sizeof(EFI_GUID));
    }
    if (alloc) {
        BS->FreePool(alloc);
    }
    mp_obj_t items[3] = { mp_obj_new_int_from_uint(st), name_obj, guid_obj };
    return mp_obj_new_tuple(3, items);
}
static MP_DEFINE_CONST_FUN_OBJ_2(uefi_raw_get_next_variable_name_obj, uefi_raw_get_next_variable_name);

// ── Events / timers / protocol-notify (the C uefi.Event drives the rest) ────
// set_timer(event_ptr, type, trigger_100ns) -> status. type: 0=cancel, 1=periodic,
// 2=relative. The event must be a timer event (uefi.Event(timer=True)).
static mp_obj_t uefi_raw_set_timer(mp_obj_t event_in, mp_obj_t type_in, mp_obj_t time_in) {
    EFI_EVENT ev = (EFI_EVENT)(uintptr_t)mp_obj_get_int_truncated(event_in);
    EFI_TIMER_DELAY type = (EFI_TIMER_DELAY)mp_obj_get_int(type_in);
    UINT64 trigger = (UINT64)mp_obj_get_int_truncated(time_in);
    EFI_STATUS st = BS->SetTimer(ev, type, trigger);
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_3(uefi_raw_set_timer_obj, uefi_raw_set_timer);

// register_protocol_notify(guid, event_ptr) -> (status, registration_ptr).
// Firmware signals `event` whenever an instance of `guid` is installed (hot-plug).
static mp_obj_t uefi_raw_register_protocol_notify(mp_obj_t guid_in, mp_obj_t event_in) {
    EFI_GUID *guid = guid_from_obj(guid_in);
    EFI_EVENT ev = (EFI_EVENT)(uintptr_t)mp_obj_get_int_truncated(event_in);
    void *registration = NULL;
    EFI_STATUS st = BS->RegisterProtocolNotify(guid, ev, &registration);
    return status_and_uint(st, (UINTN)(uintptr_t)registration);
}
static MP_DEFINE_CONST_FUN_OBJ_2(uefi_raw_register_protocol_notify_obj, uefi_raw_register_protocol_notify);

// locate_handle_notify(registration) -> (status, handle). One handle per call
// (NOT_FOUND when drained) — the spec-clean way to walk RegisterProtocolNotify
// arrivals (LocateHandleBuffer double-advances the registration on ByRegisterNotify).
static mp_obj_t uefi_raw_locate_handle_notify(mp_obj_t reg_in) {
    void *registration = (void *)(uintptr_t)mp_obj_get_int_truncated(reg_in);
    EFI_HANDLE handle = NULL;
    UINTN size = sizeof(EFI_HANDLE);
    EFI_STATUS st = BS->LocateHandle(ByRegisterNotify, NULL, registration, &size, &handle);
    return status_and_uint(st, (UINTN)(uintptr_t)handle);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_raw_locate_handle_notify_obj, uefi_raw_locate_handle_notify);

// install_protocol_interface(handle_or_0, guid, iface_ptr) -> (status, handle).
// handle 0 creates a fresh handle. uninstall_protocol_interface(handle, guid, iface) -> status.
static mp_obj_t uefi_raw_install_protocol_interface(mp_obj_t handle_in, mp_obj_t guid_in, mp_obj_t iface_in) {
    EFI_HANDLE handle = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(handle_in);
    EFI_GUID *guid = guid_from_obj(guid_in);
    void *iface = (void *)(uintptr_t)mp_obj_get_int_truncated(iface_in);
    EFI_STATUS st = BS->InstallProtocolInterface(&handle, guid, EFI_NATIVE_INTERFACE, iface);
    return status_and_uint(st, (UINTN)(uintptr_t)handle);
}
static MP_DEFINE_CONST_FUN_OBJ_3(uefi_raw_install_protocol_interface_obj, uefi_raw_install_protocol_interface);

static mp_obj_t uefi_raw_uninstall_protocol_interface(mp_obj_t handle_in, mp_obj_t guid_in, mp_obj_t iface_in) {
    EFI_HANDLE handle = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(handle_in);
    EFI_GUID *guid = guid_from_obj(guid_in);
    void *iface = (void *)(uintptr_t)mp_obj_get_int_truncated(iface_in);
    EFI_STATUS st = BS->UninstallProtocolInterface(handle, guid, iface);
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_3(uefi_raw_uninstall_protocol_interface_obj, uefi_raw_uninstall_protocol_interface);

// ── Images / drivers ────────────────────────────────────────────────────────
// load_image(boot_policy, parent_handle, device_path_ptr, src_buffer_ptr, src_size)
//   -> (status, image_handle). Pass a source buffer for a memory-to-memory load
// (device_path_ptr=0), or a device path to load from a volume (src_buffer_ptr=0,
// src_size=0). LoadImage copies/relocates the source into fresh pages, so the
// source buffer need only live for the duration of this call.
static mp_obj_t uefi_raw_load_image(size_t n_args, const mp_obj_t *args) {
    BOOLEAN boot_policy = mp_obj_is_true(args[0]) ? 1 : 0;
    EFI_HANDLE parent = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[1]);
    void *dp = (void *)(uintptr_t)mp_obj_get_int_truncated(args[2]);
    void *src = (void *)(uintptr_t)mp_obj_get_int_truncated(args[3]);
    UINTN src_size = (UINTN)mp_obj_get_int_truncated(args[4]);
    EFI_HANDLE image = NULL;
    EFI_STATUS st = BS->LoadImage(boot_policy, parent, dp, src, src_size, &image);
    return status_and_uint(st, (UINTN)(uintptr_t)image);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_load_image_obj, 5, 5, uefi_raw_load_image);

// start_image(image_handle) -> (status, exit_data_ptr, exit_data_size). The exit
// data is a firmware AllocatePool buffer the caller must FreePool (the Python
// layer copies it out and frees it). status is the image's exit code.
static mp_obj_t uefi_raw_start_image(mp_obj_t image_in) {
    EFI_HANDLE image = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(image_in);
    UINTN exit_size = 0;
    CHAR16 *exit_data = NULL;
    EFI_STATUS st = BS->StartImage(image, &exit_size, &exit_data);
    mp_obj_t items[3] = {
        mp_obj_new_int_from_uint(st),
        mp_obj_new_int_from_uint((UINTN)(uintptr_t)exit_data),
        mp_obj_new_int_from_uint(exit_size),
    };
    return mp_obj_new_tuple(3, items);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_raw_start_image_obj, uefi_raw_start_image);

static mp_obj_t uefi_raw_unload_image(mp_obj_t image_in) {
    EFI_HANDLE image = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(image_in);
    EFI_STATUS st = BS->UnloadImage(image);
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_raw_unload_image_obj, uefi_raw_unload_image);

// ── EFI_LOAD_FILE(2) provider ───────────────────────────────────────────────
// Present an in-memory buffer to the firmware/OS as a "file", served on demand
// through EFI_LOAD_FILE2_PROTOCOL (or the v1 EFI_LOAD_FILE_PROTOCOL) at a device
// path. This is the one boot mechanism the consumer wrappers can't cover: here we
// *implement* a protocol, so the firmware calls back into us. An EFI-stub OS uses
// it to pull a file that isn't on a filesystem — most importantly the Linux initrd
// (looked up at the LINUX_EFI_INITRD_MEDIA device path). The buffer is served by
// this native shim (no interpreter re-entry mid-call); the Python layer keeps the
// source buffer alive. The record starts with the function pointer, so the EFIAPI
// `This` is exactly the record.
typedef EFI_STATUS (EFIAPI *EFI_LOAD_FILE_FN)(void *This, void *FilePath,
    BOOLEAN BootPolicy, UINTN *BufferSize, void *Buffer);

typedef struct {
    EFI_LOAD_FILE_FN LoadFile;   // MUST be first: this IS the EFI_LOAD_FILE(2)_PROTOCOL
    EFI_HANDLE handle;           // the handle we installed onto
    const void *data;            // borrowed file bytes (Python keeps them alive)
    UINTN size;
    void *dp;                    // device path bytes we own (firmware retains the ptr)
    UINT8 is_v2;                 // 1 = LOAD_FILE2 (rejects boot policy), 0 = LOAD_FILE
} lf_provider_t;

static const EFI_GUID g_load_file2 =
{ 0x4006c0c1, 0xfcb3, 0x403e, { 0x99, 0x6d, 0x4a, 0x6c, 0x87, 0x24, 0xe0, 0x6d } };
static const EFI_GUID g_load_file =
{ 0x56ec3091, 0x954c, 0x11d2, { 0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };
static const EFI_GUID g_device_path_proto =
{ 0x09576e91, 0x6d3f, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } };

// The LoadFile callback the firmware invokes (two-phase size probe, then copy).
static EFI_STATUS EFIAPI lf_load_file(void *This, void *FilePath,
    BOOLEAN BootPolicy, UINTN *BufferSize, void *Buffer) {
    (void)FilePath;
    lf_provider_t *p = (lf_provider_t *)This;
    if (BufferSize == NULL) {
        return EFIERR(2);                 // EFI_INVALID_PARAMETER
    }
    if (p->is_v2 && BootPolicy) {
        return EFIERR(3);                 // EFI_UNSUPPORTED: LoadFile2 forbids boot policy
    }
    if (Buffer == NULL || *BufferSize < p->size) {
        *BufferSize = p->size;
        return EFI_BUFFER_TOO_SMALL;
    }
    memcpy(Buffer, p->data, p->size);
    *BufferSize = p->size;
    return EFI_SUCCESS;
}

static mp_obj_t status_and_two(EFI_STATUS st, UINTN a, UINTN b) {
    mp_obj_t items[3] = {
        mp_obj_new_int_from_uint(st), mp_obj_new_int_from_uint(a), mp_obj_new_int_from_uint(b),
    };
    return mp_obj_new_tuple(3, items);
}

// load_file_install(dp_ptr, dp_len, data_ptr, data_len, is_v2) -> (status, handle, token)
// Installs a device path + LOAD_FILE(2) on a fresh handle. `token` is an opaque
// handle for load_file_uninstall(). `data` is borrowed (not copied): keep it alive.
static mp_obj_t uefi_raw_load_file_install(size_t n_args, const mp_obj_t *args) {
    const void *dp = (const void *)(uintptr_t)mp_obj_get_int_truncated(args[0]);
    UINTN dp_len = (UINTN)mp_obj_get_int_truncated(args[1]);
    const void *data = (const void *)(uintptr_t)mp_obj_get_int_truncated(args[2]);
    UINTN data_len = (UINTN)mp_obj_get_int_truncated(args[3]);
    UINT8 is_v2 = mp_obj_is_true(args[4]) ? 1 : 0;

    lf_provider_t *p = NULL;
    EFI_STATUS st = BS->AllocatePool(EfiBootServicesData, sizeof(*p), (void **)&p);
    if (EFI_ERROR(st)) {
        return status_and_two(st, 0, 0);
    }
    void *dpcopy = NULL;
    st = BS->AllocatePool(EfiBootServicesData, dp_len ? dp_len : 1, &dpcopy);
    if (EFI_ERROR(st)) {
        BS->FreePool(p);
        return status_and_two(st, 0, 0);
    }
    memcpy(dpcopy, dp, dp_len);
    p->LoadFile = lf_load_file;
    p->handle = NULL;
    p->data = data;
    p->size = data_len;
    p->dp = dpcopy;
    p->is_v2 = is_v2;

    EFI_HANDLE h = NULL;
    st = BS->InstallProtocolInterface(&h, (EFI_GUID *)&g_device_path_proto,
        EFI_NATIVE_INTERFACE, dpcopy);
    if (!EFI_ERROR(st)) {
        st = BS->InstallProtocolInterface(&h,
            (EFI_GUID *)(is_v2 ? &g_load_file2 : &g_load_file), EFI_NATIVE_INTERFACE, p);
        if (EFI_ERROR(st)) {
            BS->UninstallProtocolInterface(h, (EFI_GUID *)&g_device_path_proto, dpcopy);
        }
    }
    if (EFI_ERROR(st)) {
        BS->FreePool(dpcopy);
        BS->FreePool(p);
        return status_and_two(st, 0, 0);
    }
    p->handle = h;
    return status_and_two(st, (UINTN)(uintptr_t)h, (UINTN)(uintptr_t)p);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_load_file_install_obj, 5, 5, uefi_raw_load_file_install);

// load_file_uninstall(token) -> status. Removes both protocols and frees the record.
static mp_obj_t uefi_raw_load_file_uninstall(mp_obj_t token_in) {
    lf_provider_t *p = (lf_provider_t *)(uintptr_t)mp_obj_get_int_truncated(token_in);
    if (p == NULL) {
        return mp_obj_new_int_from_uint(EFI_SUCCESS);
    }
    const EFI_GUID *lfg = p->is_v2 ? &g_load_file2 : &g_load_file;
    EFI_STATUS st1 = BS->UninstallProtocolInterface(p->handle, (EFI_GUID *)lfg, p);
    EFI_STATUS st2 = BS->UninstallProtocolInterface(p->handle,
        (EFI_GUID *)&g_device_path_proto, p->dp);
    BS->FreePool(p->dp);
    BS->FreePool(p);
    return mp_obj_new_int_from_uint(EFI_ERROR(st1) ? st1 : st2);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_raw_load_file_uninstall_obj, uefi_raw_load_file_uninstall);

// connect_controller(handle, driver_image=0, remaining_dp=0, recursive=False) ->
// status. driver_image 0 lets the firmware pick drivers; otherwise it is a single
// driver image handle, wrapped in the NULL-terminated array LoadImage expects.
static mp_obj_t uefi_raw_connect_controller(size_t n_args, const mp_obj_t *args) {
    EFI_HANDLE ctrl = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[0]);
    EFI_HANDLE driver = (n_args > 1) ? (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[1]) : NULL;
    void *remaining = (n_args > 2) ? (void *)(uintptr_t)mp_obj_get_int_truncated(args[2]) : NULL;
    BOOLEAN recursive = (n_args > 3 && mp_obj_is_true(args[3])) ? 1 : 0;
    EFI_HANDLE drivers[2] = { driver, NULL };
    EFI_STATUS st = BS->ConnectController(ctrl, driver ? drivers : NULL, remaining, recursive);
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_connect_controller_obj, 1, 4, uefi_raw_connect_controller);

// disconnect_controller(handle, driver=0, child=0) -> status. driver 0 disconnects
// all drivers; child 0 disconnects all children of the (selected) driver.
static mp_obj_t uefi_raw_disconnect_controller(size_t n_args, const mp_obj_t *args) {
    EFI_HANDLE ctrl = (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[0]);
    EFI_HANDLE driver = (n_args > 1) ? (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[1]) : NULL;
    EFI_HANDLE child = (n_args > 2) ? (EFI_HANDLE)(uintptr_t)mp_obj_get_int_truncated(args[2]) : NULL;
    EFI_STATUS st = BS->DisconnectController(ctrl, driver, child);
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_disconnect_controller_obj, 1, 3, uefi_raw_disconnect_controller);

// ── Time (Runtime Services RTC) + monotonic count ───────────────────────────
// get_time() -> (status, (year,month,day,hour,minute,second,nanosecond,timezone,daylight))
// timezone is signed minutes-from-UTC (or 0x07FF = "unspecified/local"), daylight a bitfield.
static mp_obj_t uefi_raw_get_time(void) {
    EFI_TIME t;
    memset(&t, 0, sizeof(t));
    EFI_STATUS st = RS->GetTime(&t, NULL);
    mp_obj_t tup = mp_const_none;
    if (!EFI_ERROR(st)) {
        mp_obj_t f[9] = {
            mp_obj_new_int(t.Year), mp_obj_new_int(t.Month), mp_obj_new_int(t.Day),
            mp_obj_new_int(t.Hour), mp_obj_new_int(t.Minute), mp_obj_new_int(t.Second),
            mp_obj_new_int_from_uint(t.Nanosecond), mp_obj_new_int(t.TimeZone),
            mp_obj_new_int(t.Daylight),
        };
        tup = mp_obj_new_tuple(9, f);
    }
    mp_obj_t items[2] = { mp_obj_new_int_from_uint(st), tup };
    return mp_obj_new_tuple(2, items);
}
static MP_DEFINE_CONST_FUN_OBJ_0(uefi_raw_get_time_obj, uefi_raw_get_time);

// set_time(year, month, day, hour, minute, second, nanosecond=0, timezone=0x07FF,
// daylight=0) -> status. 0x07FF is EFI_UNSPECIFIED_TIMEZONE (store as local time).
static mp_obj_t uefi_raw_set_time(size_t n_args, const mp_obj_t *args) {
    EFI_TIME t;
    memset(&t, 0, sizeof(t));
    t.Year = (UINT16)mp_obj_get_int(args[0]);
    t.Month = (UINT8)mp_obj_get_int(args[1]);
    t.Day = (UINT8)mp_obj_get_int(args[2]);
    t.Hour = (UINT8)mp_obj_get_int(args[3]);
    t.Minute = (UINT8)mp_obj_get_int(args[4]);
    t.Second = (UINT8)mp_obj_get_int(args[5]);
    t.Nanosecond = (n_args > 6) ? (UINT32)mp_obj_get_int_truncated(args[6]) : 0;
    t.TimeZone = (n_args > 7) ? (INT16)mp_obj_get_int(args[7]) : 0x07FF;
    t.Daylight = (n_args > 8) ? (UINT8)mp_obj_get_int(args[8]) : 0;
    EFI_STATUS st = RS->SetTime(&t);
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_set_time_obj, 6, 9, uefi_raw_set_time);

// get_next_monotonic_count() -> (status, count). The platform's monotonic counter
// (never repeats, survives across calls) — the shell's `getmtc`.
static mp_obj_t uefi_raw_get_next_monotonic_count(void) {
    UINT64 c = 0;
    EFI_STATUS st = BS->GetNextMonotonicCount(&c);
    return status_and_uint(st, c);
}
static MP_DEFINE_CONST_FUN_OBJ_0(uefi_raw_get_next_monotonic_count_obj, uefi_raw_get_next_monotonic_count);

// wait_for_event([event, ...]) -> (status, signaled_index). Blocks in
// BS->WaitForEvent until one of the events fires. TPL_APPLICATION only, and NOT
// valid for EVT_NOTIFY_SIGNAL events (use the pollable uefi.Event for those). Used
// by the console to block on the ConIn WaitForKey event.
static mp_obj_t uefi_raw_wait_for_event(mp_obj_t events_in) {
    size_t n;
    mp_obj_t *items;
    mp_obj_get_array(events_in, &n, &items);
    if (n == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("no events"));
    }
    EFI_EVENT stackev[8];
    EFI_EVENT *ev = stackev;
    void *alloc = NULL;
    if (n > MP_ARRAY_SIZE(stackev)) {
        if (EFI_ERROR(BS->AllocatePool(EfiBootServicesData, n * sizeof(EFI_EVENT), &alloc))) {
            mp_raise_ValueError(MP_ERROR_TEXT("too many events"));
        }
        ev = (EFI_EVENT *)alloc;
    }
    for (size_t i = 0; i < n; i++) {
        ev[i] = (EFI_EVENT)(uintptr_t)mp_obj_get_int_truncated(items[i]);
    }
    UINTN index = 0;
    EFI_STATUS st = BS->WaitForEvent(n, ev, &index);
    if (alloc) {
        BS->FreePool(alloc);
    }
    return status_and_uint(st, index);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_raw_wait_for_event_obj, uefi_raw_wait_for_event);

// ── System / firmware tables (Runtime + Boot Services) ──────────────────────
// reset_system(type, status=0, data=None): RuntimeServices->ResetSystem. type is
// EFI_RESET_TYPE (0=cold, 1=warm, 2=shutdown, 3=platform-specific). Does NOT
// return for cold/warm/shutdown — the platform is reset before this call unwinds.
static mp_obj_t uefi_raw_reset_system(size_t n_args, const mp_obj_t *args) {
    EFI_RESET_TYPE type = (EFI_RESET_TYPE)mp_obj_get_int(args[0]);
    EFI_STATUS rst = (n_args > 1) ? (EFI_STATUS)mp_obj_get_int_truncated(args[1]) : 0;
    void *data = NULL;
    UINTN size = 0;
    if (n_args > 2 && args[2] != mp_const_none) {
        mp_buffer_info_t bi;
        mp_get_buffer_raise(args[2], &bi, MP_BUFFER_READ);
        data = bi.buf;
        size = bi.len;
    }
    RS->ResetSystem(type, rst, size, data);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_reset_system_obj, 1, 3, uefi_raw_reset_system);

// set_watchdog_timer(timeout_s, code=0) -> status. timeout_s=0 disables the
// watchdog (what a boot manager does before handing off to an OS loader).
static mp_obj_t uefi_raw_set_watchdog_timer(size_t n_args, const mp_obj_t *args) {
    UINTN timeout = (UINTN)mp_obj_get_int_truncated(args[0]);
    UINT64 code = (n_args > 1) ? (UINT64)mp_obj_get_int_truncated(args[1]) : 0;
    EFI_STATUS st = BS->SetWatchdogTimer(timeout, code, 0, NULL);
    return mp_obj_new_int_from_uint(st);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_raw_set_watchdog_timer_obj, 1, 2, uefi_raw_set_watchdog_timer);

// get_memory_map() -> (status, map_key, [(type, phys_start, num_pages, attribute), ...]).
// Two-phase probe internally; the firmware buffer is FreePool'd before returning.
static mp_obj_t uefi_raw_get_memory_map(void) {
    UINTN size = 0, map_key = 0, desc_size = 0;
    UINT32 desc_ver = 0;
    EFI_STATUS st = BS->GetMemoryMap(&size, NULL, &map_key, &desc_size, &desc_ver);
    mp_obj_t list = mp_obj_new_list(0, NULL);
    if (st == EFI_BUFFER_TOO_SMALL && desc_size > 0) {
        // Slack: allocating the buffer can itself add map entries.
        size += desc_size * 8;
        void *buf = NULL;
        EFI_STATUS as = BS->AllocatePool(EfiBootServicesData, size, &buf);
        if (EFI_ERROR(as)) {
            st = as;
        } else {
            st = BS->GetMemoryMap(&size, (EFI_MEMORY_DESCRIPTOR *)buf, &map_key, &desc_size, &desc_ver);
            if (!EFI_ERROR(st)) {
                for (UINTN off = 0; off + desc_size <= size; off += desc_size) {
                    EFI_MEMORY_DESCRIPTOR *d = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)buf + off);
                    mp_obj_t item[4] = {
                        mp_obj_new_int_from_uint(d->Type),
                        mp_obj_new_int_from_uint(d->PhysicalStart),
                        mp_obj_new_int_from_uint(d->NumberOfPages),
                        mp_obj_new_int_from_uint(d->Attribute),
                    };
                    mp_obj_list_append(list, mp_obj_new_tuple(4, item));
                }
            }
            BS->FreePool(buf);
        }
    }
    mp_obj_t items[3] = { mp_obj_new_int_from_uint(st), mp_obj_new_int_from_uint(map_key), list };
    return mp_obj_new_tuple(3, items);
}
static MP_DEFINE_CONST_FUN_OBJ_0(uefi_raw_get_memory_map_obj, uefi_raw_get_memory_map);

// configuration_tables() -> [(guid_bytes_16, table_ptr), ...]. The System Table's
// vendor configuration tables (ACPI RSDP, SMBIOS, device tree, ...) — read straight
// from the port's system-table global; no Boot Services call.
static mp_obj_t uefi_raw_configuration_tables(void) {
    mp_obj_t list = mp_obj_new_list(0, NULL);
    UINTN n = mp_uefi_st->NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE *t = mp_uefi_st->ConfigurationTable;
    for (UINTN i = 0; i < n; i++) {
        mp_obj_t item[2] = {
            mp_obj_new_bytes((const byte *)&t[i].VendorGuid, 16),
            mp_obj_new_int_from_uint((UINTN)(uintptr_t)t[i].VendorTable),
        };
        mp_obj_list_append(list, mp_obj_new_tuple(2, item));
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_0(uefi_raw_configuration_tables_obj, uefi_raw_configuration_tables);

// wstr_at(ptr) -> str: decode a null-terminated CHAR16 (UTF-16LE) string at `ptr`
// into a Python str (None for a null pointer). For firmware-returned CHAR16*, e.g.
// DevicePathToText / Component Name results.
static mp_obj_t uefi_raw_wstr_at(mp_obj_t ptr_in) {
    const CHAR16 *s = (const CHAR16 *)(uintptr_t)mp_obj_get_int_truncated(ptr_in);
    if (s == NULL) {
        return mp_const_none;
    }
    return char16le_to_str(s);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_raw_wstr_at_obj, uefi_raw_wstr_at);

// open_volume(fs_iface_ptr) -> vfs_object: build a mountable VFS from an
// EFI_SIMPLE_FILE_SYSTEM_PROTOCOL interface pointer (OpenVolume). The returned
// object is passed to os.mount() to make another volume reachable by path.
extern void *mp_uefi_vfs_from_simple_fs(void *fs_iface);
static mp_obj_t uefi_raw_open_volume(mp_obj_t fs_in) {
    void *fs = (void *)(uintptr_t)mp_obj_get_int_truncated(fs_in);
    if (fs == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("null filesystem pointer"));
    }
    return (mp_obj_t)mp_uefi_vfs_from_simple_fs(fs);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_raw_open_volume_obj, uefi_raw_open_volume);

// convert_rgb565(src_ptr, dst_ptr, count): expand `count` RGB565 pixels at src to
// 32-bit BGRx at dst — for pushing a framebuf.FrameBuffer(RGB565) to GOP via Blt.
// A C loop keeps this off the (slow, per-pixel) Python path for full-screen frames.
static mp_obj_t uefi_raw_convert_rgb565(mp_obj_t src_in, mp_obj_t dst_in, mp_obj_t count_in) {
    const UINT16 *src = (const UINT16 *)(uintptr_t)mp_obj_get_int_truncated(src_in);
    UINT32 *dst = (UINT32 *)(uintptr_t)mp_obj_get_int_truncated(dst_in);
    size_t count = (size_t)mp_obj_get_int_truncated(count_in);
    for (size_t i = 0; i < count; i++) {
        UINT16 p = src[i];
        UINT32 r = (p >> 11) & 0x1F, g = (p >> 5) & 0x3F, b = p & 0x1F;
        r = (r << 3) | (r >> 2);        // 5/6-bit -> 8-bit
        g = (g << 2) | (g >> 4);
        b = (b << 3) | (b >> 2);
        dst[i] = (r << 16) | (g << 8) | b;   // 0x00RRGGBB == BGRx little-endian
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(uefi_raw_convert_rgb565_obj, uefi_raw_convert_rgb565);

// ── Module table ────────────────────────────────────────────────────────────
static const mp_rom_map_elem_t uefi_raw_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__uefi) },
    { MP_ROM_QSTR(MP_QSTR_call), MP_ROM_PTR(&uefi_raw_call_obj) },
    { MP_ROM_QSTR(MP_QSTR_system_table_ptr), MP_ROM_PTR(&uefi_raw_system_table_ptr_obj) },
    { MP_ROM_QSTR(MP_QSTR_boot_services_ptr), MP_ROM_PTR(&uefi_raw_boot_services_ptr_obj) },
    { MP_ROM_QSTR(MP_QSTR_runtime_services_ptr), MP_ROM_PTR(&uefi_raw_runtime_services_ptr_obj) },
    { MP_ROM_QSTR(MP_QSTR_image_handle), MP_ROM_PTR(&uefi_raw_image_handle_obj) },
    { MP_ROM_QSTR(MP_QSTR_boot_services_active), MP_ROM_PTR(&uefi_raw_boot_services_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_allocate_pool), MP_ROM_PTR(&uefi_raw_allocate_pool_obj) },
    { MP_ROM_QSTR(MP_QSTR_free_pool), MP_ROM_PTR(&uefi_raw_free_pool_obj) },
    { MP_ROM_QSTR(MP_QSTR_locate_handle_buffer), MP_ROM_PTR(&uefi_raw_locate_handle_buffer_obj) },
    { MP_ROM_QSTR(MP_QSTR_open_protocol), MP_ROM_PTR(&uefi_raw_open_protocol_obj) },
    { MP_ROM_QSTR(MP_QSTR_close_protocol), MP_ROM_PTR(&uefi_raw_close_protocol_obj) },
    { MP_ROM_QSTR(MP_QSTR_protocols_per_handle), MP_ROM_PTR(&uefi_raw_protocols_per_handle_obj) },
    { MP_ROM_QSTR(MP_QSTR_open_protocol_information), MP_ROM_PTR(&uefi_raw_open_protocol_information_obj) },
    { MP_ROM_QSTR(MP_QSTR_locate_protocol), MP_ROM_PTR(&uefi_raw_locate_protocol_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_variable), MP_ROM_PTR(&uefi_raw_get_variable_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_variable), MP_ROM_PTR(&uefi_raw_set_variable_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_next_variable_name), MP_ROM_PTR(&uefi_raw_get_next_variable_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_timer), MP_ROM_PTR(&uefi_raw_set_timer_obj) },
    { MP_ROM_QSTR(MP_QSTR_register_protocol_notify), MP_ROM_PTR(&uefi_raw_register_protocol_notify_obj) },
    { MP_ROM_QSTR(MP_QSTR_locate_handle_notify), MP_ROM_PTR(&uefi_raw_locate_handle_notify_obj) },
    { MP_ROM_QSTR(MP_QSTR_install_protocol_interface), MP_ROM_PTR(&uefi_raw_install_protocol_interface_obj) },
    { MP_ROM_QSTR(MP_QSTR_uninstall_protocol_interface), MP_ROM_PTR(&uefi_raw_uninstall_protocol_interface_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_image), MP_ROM_PTR(&uefi_raw_load_image_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_image), MP_ROM_PTR(&uefi_raw_start_image_obj) },
    { MP_ROM_QSTR(MP_QSTR_unload_image), MP_ROM_PTR(&uefi_raw_unload_image_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_file_install), MP_ROM_PTR(&uefi_raw_load_file_install_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_file_uninstall), MP_ROM_PTR(&uefi_raw_load_file_uninstall_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect_controller), MP_ROM_PTR(&uefi_raw_connect_controller_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect_controller), MP_ROM_PTR(&uefi_raw_disconnect_controller_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_system), MP_ROM_PTR(&uefi_raw_reset_system_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_watchdog_timer), MP_ROM_PTR(&uefi_raw_set_watchdog_timer_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_memory_map), MP_ROM_PTR(&uefi_raw_get_memory_map_obj) },
    { MP_ROM_QSTR(MP_QSTR_configuration_tables), MP_ROM_PTR(&uefi_raw_configuration_tables_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_for_event), MP_ROM_PTR(&uefi_raw_wait_for_event_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_time), MP_ROM_PTR(&uefi_raw_get_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_time), MP_ROM_PTR(&uefi_raw_set_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_next_monotonic_count), MP_ROM_PTR(&uefi_raw_get_next_monotonic_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_wstr_at), MP_ROM_PTR(&uefi_raw_wstr_at_obj) },
    { MP_ROM_QSTR(MP_QSTR_open_volume), MP_ROM_PTR(&uefi_raw_open_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_convert_rgb565), MP_ROM_PTR(&uefi_raw_convert_rgb565_obj) },
    // The C uefi.Event type (defined in modefi.c) folded under this module.
    { MP_ROM_QSTR(MP_QSTR_Event), MP_ROM_PTR(&efi_event_type) },
};
static MP_DEFINE_CONST_DICT(uefi_raw_globals, uefi_raw_globals_table);

const mp_obj_module_t mp_module__uefi = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&uefi_raw_globals,
};
MP_REGISTER_MODULE(MP_QSTR__uefi, mp_module__uefi);
