// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// machine.Timer for the UEFI port: MicroPython's idiomatic timer API over a UEFI
// timer event. Compiled directly (so its MP_REGISTER_ROOT_POINTER is collected);
// the machine module glue in modmachine.c references machine_timer_type.
//
// Lifecycle (Model B, like the mainline ports): an armed timer holds a live UEFI
// event whose notify references it, so it is kept alive by a GC-root registry —
// dropping the Python reference does not stop an active timer; deinit() does, and
// machine_timer_deinit_all() stops everything at interpreter teardown. Callbacks may be
// soft (scheduled to run later at TPL_APPLICATION) or hard (`hard=True` — run directly in
// the notify's TPL_CALLBACK context with the heap locked); see machine_timer_notify.

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/runtime/mpirq.h"
#include "uefi_port.h"
#include "efi.h"

#define TIMER_MODE_ONE_SHOT (0)
#define TIMER_MODE_PERIODIC (1)

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    EFI_EVENT event;     // UEFI timer event, or NULL when inactive
    uint32_t mode;
    uint64_t period_us;  // expiry interval
    mp_obj_t callback;   // Python callable(timer), or mp_const_none
    bool ishard;         // hard IRQ: run callback in notify context (heap locked)
    struct _machine_timer_obj_t *next; // registry link while armed
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;

// Registry of armed timers. An armed timer must not be collected while its notify
// can still fire, so linking it here (a GC root) keeps it alive. Only ever touched
// from foreground (TPL_APPLICATION), never from a notify.
MP_REGISTER_ROOT_POINTER(struct _machine_timer_obj_t *machine_timer_obj_head);

static void machine_timer_link(machine_timer_obj_t *self) {
    self->next = MP_STATE_PORT(machine_timer_obj_head);
    MP_STATE_PORT(machine_timer_obj_head) = self;
}

static void machine_timer_unlink(machine_timer_obj_t *self) {
    machine_timer_obj_t **p = &MP_STATE_PORT(machine_timer_obj_head);
    while (*p != NULL) {
        if (*p == self) {
            *p = self->next;
            self->next = NULL;
            return;
        }
        p = &(*p)->next;
    }
}

// Timer-event notify: runs in hard-ISR context (TPL_CALLBACK), on the foreground stack
// (UEFI dispatches CALLBACK notifies at a TPL transition, so the interpreter's stack
// limit still applies). mp_irq_dispatch does the right thing for each kind: a soft IRQ
// (ishard=false) is only scheduled, to run later at TPL_APPLICATION with the heap live;
// a hard IRQ (ishard=true) runs the callback right here with the GC and scheduler locked
// (so it must not allocate — the usual hard-IRQ caveat), nlr-guarded.
static void EFIAPI machine_timer_notify(EFI_EVENT event, void *context) {
    (void)event;
    machine_timer_obj_t *self = context;
    if (mp_irq_dispatch(self->callback, MP_OBJ_FROM_PTR(self), self->ishard) < 0) {
        // Uncaught exception in a hard-IRQ callback: disable it so it can't refire.
        self->callback = mp_const_none;
    }
}

static void machine_timer_disarm(machine_timer_obj_t *self) {
    if (self->event != NULL) {
        mp_uefi_cancel_timer(self->event);
        mp_uefi_close_event(self->event);
        self->event = NULL;
        machine_timer_unlink(self);
    }
}

// Teardown hook (called before mp_deinit): stop every armed timer so no notify can
// fire into a torn-down interpreter. Events are closed; the objects are left for GC.
void machine_timer_deinit_all(void) {
    machine_timer_obj_t *t = MP_STATE_PORT(machine_timer_obj_head);
    while (t != NULL) {
        machine_timer_obj_t *next = t->next;
        if (t->event != NULL) {
            mp_uefi_cancel_timer(t->event);
            mp_uefi_close_event(t->event);
            t->event = NULL;
        }
        t->next = NULL;
        t = next;
    }
    MP_STATE_PORT(machine_timer_obj_head) = NULL;
}

static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    qstr mode = self->mode == TIMER_MODE_ONE_SHOT ? MP_QSTR_ONE_SHOT : MP_QSTR_PERIODIC;
    mp_printf(print, "Timer(mode=%q, period=%u us)", mode, (unsigned int)self->period_us);
}

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_callback, ARG_period, ARG_tick_hz, ARG_freq, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_PERIODIC} },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_period,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_tick_hz,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_hard,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->mode = args[ARG_mode].u_int;
    if (args[ARG_freq].u_obj != mp_const_none) {
        #if MICROPY_PY_BUILTINS_FLOAT
        self->period_us = (uint64_t)(MICROPY_FLOAT_CONST(1000000.0) / mp_obj_get_float(args[ARG_freq].u_obj));
        #else
        self->period_us = 1000000 / mp_obj_get_int(args[ARG_freq].u_obj);
        #endif
    } else {
        self->period_us = (uint64_t)args[ARG_period].u_int * 1000000 / args[ARG_tick_hz].u_int;
    }
    if (self->period_us < 1) {
        self->period_us = 1;
    }
    self->callback = args[ARG_callback].u_obj;
    self->ishard = args[ARG_hard].u_bool;

    if (self->event == NULL) {
        if (EFI_ERROR(mp_uefi_create_timer(machine_timer_notify, self, &self->event))) {
            self->event = NULL;
            mp_raise_OSError(MP_ENOMEM);
        }
        machine_timer_link(self); // armed -> keep alive in the registry
    }
    mp_uefi_arm_timer(self->event, self->mode == TIMER_MODE_PERIODIC, self->period_us);
    return mp_const_none;
}

static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    machine_timer_obj_t *self = mp_obj_malloc_with_finaliser(machine_timer_obj_t, &machine_timer_type);
    self->event = NULL;
    self->callback = mp_const_none;
    self->ishard = false;
    self->next = NULL;

    // Optional leading timer id; only the virtual timer (-1) is supported.
    mp_int_t id = -1;
    if (n_args > 0) {
        id = mp_obj_get_int(args[0]);
        --n_args;
        ++args;
    }
    if (id != -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("Timer doesn't exist"));
    }

    if (n_args > 0 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
    }
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (self->event != NULL) {
        mp_uefi_cancel_timer(self->event); // re-arm cleanly; keep the event handle
    }
    return machine_timer_init_helper(self, n_args - 1, args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

static mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_disarm(MP_OBJ_TO_PTR(self_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

static const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_MODE_ONE_SHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};
static MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
    print, machine_timer_print,
    locals_dict, &machine_timer_locals_dict
    );
