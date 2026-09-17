// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// uefi.Event: the pollable UEFI EVT_NOTIFY_SIGNAL wrapper.
//
// uefi.Event wraps a UEFI EVT_NOTIFY_SIGNAL event. When the event is signalled
// (by Python via .signal(), or later by a protocol/firmware that holds it), our
// notify runs hard-ISR (TPL_CALLBACK): it sets a sticky flag and wakes the idle
// loop. The object is pollable (ioctl(MP_STREAM_POLL) reports the flag), so it
// drops straight into select.poll and asyncio; .wait() blocks on it via the WFE.
// This is the uniform "an external thing signalled us" path.
//
// The type is defined here but registers no module of its own: moduefi_raw.c
// (the `_uefi` module) folds it into its globals table as `_uefi.Event`, the
// same way modmachine.c folds in machine_timer.c's Timer and machine_rtc.c's RTC.

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/stream.h"
#include "uefi_port.h"
#include "efi.h"

typedef struct _uefi_event_obj_t {
    mp_obj_base_t base;
    EFI_EVENT event;                // UEFI event, NULL once closed
    volatile int signaled;          // sticky flag, set by the notify
    struct _uefi_event_obj_t *next; // registry link while open
} uefi_event_obj_t;

const mp_obj_type_t uefi_event_type;

// Registry of open events: like machine.Timer, an open event holds a live UEFI
// event whose notify references it, so it must be kept alive (GC root) and closed
// at teardown. Only touched from foreground (TPL_APPLICATION), never a notify.
MP_REGISTER_ROOT_POINTER(struct _uefi_event_obj_t *uefi_event_obj_head);

static void uefi_event_link(uefi_event_obj_t *self) {
    self->next = MP_STATE_PORT(uefi_event_obj_head);
    MP_STATE_PORT(uefi_event_obj_head) = self;
}

static void uefi_event_unlink(uefi_event_obj_t *self) {
    uefi_event_obj_t **p = &MP_STATE_PORT(uefi_event_obj_head);
    while (*p != NULL) {
        if (*p == self) {
            *p = self->next;
            self->next = NULL;
            return;
        }
        p = &(*p)->next;
    }
}

// notify: hard-ISR context (TPL_CALLBACK). Set the flag and wake the idle loop;
// never touch the VM/heap. (EVT_NOTIFY_SIGNAL auto-resets the UEFI event after the
// notify runs, so our `signaled` flag is the durable state.)
static void EFIAPI uefi_event_notify(EFI_EVENT event, void *context) {
    (void)event;
    uefi_event_obj_t *self = context;
    self->signaled = 1;
    mp_uefi_wake();
}

static void uefi_event_do_close(uefi_event_obj_t *self) {
    if (self->event != NULL) {
        mp_uefi_close_event(self->event);
        self->event = NULL;
        uefi_event_unlink(self);
    }
}

// Teardown hook (before mp_deinit): close every open event so no notify fires
// into a torn-down interpreter.
void uefi_event_obj_deinit_all(void) {
    uefi_event_obj_t *e = MP_STATE_PORT(uefi_event_obj_head);
    while (e != NULL) {
        uefi_event_obj_t *next = e->next;
        if (e->event != NULL) {
            mp_uefi_close_event(e->event);
            e->event = NULL;
        }
        e->next = NULL;
        e = next;
    }
    MP_STATE_PORT(uefi_event_obj_head) = NULL;
}

static mp_obj_t uefi_event_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    // Event(timer=False): a plain EVT_NOTIFY_SIGNAL event by default; timer=True
    // creates an EVT_TIMER|EVT_NOTIFY_SIGNAL event so raw.set_timer works on it.
    enum { ARG_timer };
    static const mp_arg_t allowed[] = {
        { MP_QSTR_timer, MP_ARG_BOOL, { .u_bool = false } },
    };
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed), allowed, vals);

    uefi_event_obj_t *self = mp_obj_malloc_with_finaliser(uefi_event_obj_t, &uefi_event_type);
    self->event = NULL;
    self->signaled = 0;
    self->next = NULL;
    EFI_STATUS st = vals[ARG_timer].u_bool
        ? mp_uefi_create_timer(uefi_event_notify, self, &self->event)
        : mp_uefi_create_signal_event(uefi_event_notify, self, &self->event);
    if (EFI_ERROR(st)) {
        self->event = NULL;
        mp_raise_OSError(MP_ENOMEM);
    }
    uefi_event_link(self);
    return MP_OBJ_FROM_PTR(self);
}

// `.ptr` (the .ptr contract): the raw EFI_EVENT, for raw.set_timer /
// register_protocol_notify. Other attributes fall through to the methods.
static void uefi_event_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        if (attr == MP_QSTR_ptr) {
            uefi_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
            dest[0] = mp_obj_new_int_from_uint((uintptr_t)self->event);
        } else {
            dest[1] = MP_OBJ_SENTINEL;  // continue lookup in locals_dict
        }
    }
}

static mp_obj_t uefi_event_signal(mp_obj_t self_in) {
    uefi_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->event == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("closed"));
    }
    mp_uefi_signal_event(self->event); // fires the notify -> flag + wake
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_event_signal_obj, uefi_event_signal);

static mp_obj_t uefi_event_is_signaled(mp_obj_t self_in) {
    uefi_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->signaled);
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_event_is_signaled_obj, uefi_event_is_signaled);

static mp_obj_t uefi_event_clear(mp_obj_t self_in) {
    uefi_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->signaled = 0;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_event_clear_obj, uefi_event_clear);

// wait(timeout_ms=None) -> True if signalled, False on timeout. Rides the WFE so
// timers/callbacks keep flowing while blocked.
static mp_obj_t uefi_event_wait(size_t n_args, const mp_obj_t *args) {
    uefi_event_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    bool has_timeout = (n_args > 1 && args[1] != mp_const_none);
    mp_uint_t timeout = has_timeout ? (mp_uint_t)mp_obj_get_int(args[1]) : 0;
    mp_uint_t start = mp_hal_ticks_ms();
    while (!self->signaled) {
        if (has_timeout) {
            mp_uint_t elapsed = mp_hal_ticks_ms() - start;
            if (elapsed >= timeout) {
                return mp_const_false;
            }
            mp_event_wait_ms(timeout - elapsed);
        } else {
            mp_event_wait_indefinite();
        }
    }
    return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uefi_event_wait_obj, 1, 2, uefi_event_wait);

static mp_obj_t uefi_event_close(mp_obj_t self_in) {
    uefi_event_do_close(MP_OBJ_TO_PTR(self_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(uefi_event_close_obj, uefi_event_close);

// Stream protocol: report MP_STREAM_POLL_RD when signalled, so select.poll and
// asyncio can wait on the event like any other readable stream.
static mp_uint_t uefi_event_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    uefi_event_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (request == MP_STREAM_POLL) {
        mp_uint_t ret = 0;
        if ((arg & MP_STREAM_POLL_RD) && self->signaled) {
            ret |= MP_STREAM_POLL_RD;
        }
        return ret;
    }
    *errcode = MP_EINVAL;
    return MP_STREAM_ERROR;
}

static const mp_stream_p_t uefi_event_stream_p = {
    .ioctl = uefi_event_ioctl,
};

static const mp_rom_map_elem_t uefi_event_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&uefi_event_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_signal), MP_ROM_PTR(&uefi_event_signal_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_signaled), MP_ROM_PTR(&uefi_event_is_signaled_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&uefi_event_clear_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait), MP_ROM_PTR(&uefi_event_wait_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&uefi_event_close_obj) },
};
static MP_DEFINE_CONST_DICT(uefi_event_locals_dict, uefi_event_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    uefi_event_type,
    MP_QSTR_Event,
    MP_TYPE_FLAG_NONE,
    make_new, uefi_event_make_new,
    attr, uefi_event_attr,
    protocol, &uefi_event_stream_p,
    locals_dict, &uefi_event_locals_dict
    );
