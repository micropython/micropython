/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"

#if MICROPY_PY_UEVENT && !defined(__unix__)

// This class is similar to selectors but:
// - supports arbitrary event flags, determined by the underlying object
// - poll.register() will update flags, not overwrite
// - one-shot behaviour (hence the name "uevent")
// - key=poll_entry can have a user method set on it
// - everything is O(1) (would need a big hash table with selectors to get O(1))
// - TODO: add poll.notify() for async wake-up

// Hack to detect Zephyr build
#if defined(K_POLL_EVENT_INITIALIZER)
#define HAVE_ZEPHYR (1)
#else
#define HAVE_ZEPHYR (0)
#endif

typedef struct _mp_obj_poll_t {
    mp_obj_base_t base;
    void *entry_all;
    volatile void *entry_ready;
    #if HAVE_ZEPHYR
    struct k_sem sem;
    #endif
} mp_obj_poll_t;

typedef struct _mp_obj_poll_entry_t {
    mp_obj_base_t base;
    mp_obj_t obj;
    mp_uint_t (*ioctl)(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode);
    volatile uint16_t flags_waiting;
    volatile uint16_t flags_ready;
    uint16_t flags_user;
    mp_obj_poll_t *poller;
    void *next_all;
    volatile void *next_ready;
    // User stuff.
    mp_obj_t user_data;
    qstr user_method_name;
    mp_obj_t user_method_obj;
} mp_obj_poll_entry_t;

STATIC void poll_entry_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_poll_entry_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute.
        if (attr == MP_QSTR_data) {
            dest[0] = self->user_data;
        } else if (attr == MP_QSTR_flags) {
            dest[0] = MP_OBJ_NEW_SMALL_INT(self->flags_user);
        } else if (attr == self->user_method_name) {
            dest[0] = self->user_method_obj;
            dest[1] = self;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store attribute.
        if (attr == MP_QSTR_data) {
            self->user_data = dest[1];
            dest[0] = MP_OBJ_NULL; // success
        } else if (attr != MP_QSTR_flags) {
            self->user_method_name = attr;
            self->user_method_obj = dest[1];
            dest[0] = MP_OBJ_NULL; // success
        }
    }
}

STATIC const mp_obj_type_t mp_type_poll_entry = {
    { &mp_type_type },
    .name = MP_QSTR_poll_entry,
    .attr = poll_entry_attr,
};

STATIC void poll_event_callback(void *arg_in, mp_uint_t flags) {
    mp_obj_poll_entry_t *entry = arg_in;
    if (!(entry->flags_waiting & flags)) {
        //printf("CB0(%p %x %x %x)", arg_in, flags, entry->flags_waiting, entry->flags_ready);
        return;
    }
    //printf("CB1(%p %x %x %x)", arg_in, flags, entry->flags_waiting, entry->flags_ready);
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    uint16_t old_flags_ready = entry->flags_ready;
    entry->flags_ready |= entry->flags_waiting & flags;
    entry->flags_waiting &= ~flags;
    if (old_flags_ready == 0 && entry->flags_ready) {
        entry->next_ready = entry->poller->entry_ready;
        entry->poller->entry_ready = entry;
        #if HAVE_ZEPHYR
        k_sem_give(&entry->poller->sem);
        #endif
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

STATIC mp_obj_poll_entry_t *poll_entry_new(void) {
    mp_obj_poll_entry_t *self = m_new_obj(mp_obj_poll_entry_t);
    self->base.type = &mp_type_poll_entry;
    return self;
}

STATIC void poll_entry_init(mp_obj_poll_entry_t *self, mp_obj_t obj, mp_obj_poll_t *poller) {
    self->obj = obj;
    self->ioctl = mp_get_stream_raise(obj, MP_STREAM_OP_IOCTL)->ioctl;
    self->flags_waiting = 0;
    self->flags_ready = 0;
    self->flags_user = 0;
    self->poller = poller;
    self->user_data = mp_const_none; // TODO consider not resetting this to reuse prev value
    self->user_method_name = MP_QSTR_NULL;
}

STATIC void poll_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);
    size_t alloc = 0;
    size_t waiting = 0;
    size_t ready = 0;
    for (mp_obj_poll_entry_t *entry = self->entry_all; entry != NULL; entry = entry->next_all) {
        ++alloc;
        if (entry->flags_waiting != 0) {
            ++waiting;
        }
        if (entry->flags_ready != 0) {
            ++ready;
        }
    }
    mp_printf(print, "<poll alloc=%d waiting=%d ready=%d>", alloc, waiting, ready);
}

STATIC mp_obj_t poll_iternext(mp_obj_t self_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->entry_ready != NULL) {
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        mp_obj_poll_entry_t *entry = (mp_obj_poll_entry_t *)self->entry_ready;
        self->entry_ready = entry->next_ready;
        entry->flags_user = entry->flags_ready;
        entry->flags_ready = 0;
        // if flags_waiting == 0 then put on entry_free list?
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        return MP_OBJ_FROM_PTR(entry);
    }

    return MP_OBJ_STOP_ITERATION;
}

// register(obj[, eventmask])
STATIC mp_obj_t poll_register(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t flags;
    if (n_args == 3) {
        flags = mp_obj_get_int(args[2]);
    } else {
        flags = 0xff;
    }
    mp_obj_poll_entry_t *entry;
    if (mp_obj_get_type(args[1]) == &mp_type_poll_entry) {
        entry = MP_OBJ_TO_PTR(args[1]);
    } else {
        /*
        mp_stream_event_t stream_event;
        mp_int_t ret = entry->ioctl(entry->obj, MP_STREAM_GET_EVENT, (uintptr_t)&stream_event, &errcode);
        if (ret == 0 && stream_event.callback == poll_event_callback) {
            entry = stream_event.arg;
            check entry.poller == self and entry.obj = args[1]
        }
        */

        mp_obj_poll_entry_t *entry_free = NULL;
        for (entry = self->entry_all; entry != NULL; entry = entry->next_all) {
            if (entry->obj == args[1]) {
                break;
            }
            if (entry->flags_waiting == 0 && entry->flags_ready == 0) {
                // reuse memory
                entry_free = entry;
            }
        }
        if (entry == NULL) {
            if (entry_free == NULL) {
                entry = poll_entry_new();
                entry->next_all = self->entry_all;
                self->entry_all = entry;
            }
            poll_entry_init(entry, args[1], self);
        }
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    entry->flags_waiting |= flags;
    MICROPY_END_ATOMIC_SECTION(atomic_state);

    int errcode;
    mp_stream_event_t stream_event = { entry->flags_waiting == 0 ? NULL : poll_event_callback, entry };
    mp_int_t ret = entry->ioctl(entry->obj, MP_STREAM_SET_EVENT, (uintptr_t)&stream_event, &errcode);
    if (ret == MP_STREAM_ERROR) {
        mp_raise_OSError(errcode);
    }
    if (entry->flags_waiting & ret) {
        poll_event_callback(entry, ret);
    }

    return MP_OBJ_FROM_PTR(entry);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_register_obj, 2, 3, poll_register);

STATIC mp_obj_t poll_wait_ms(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);

    // work out timeout
    mp_uint_t timeout_ms = -1;
    if (n_args >= 2 && args[1] != mp_const_none) {
        mp_int_t timeout_i = mp_obj_get_int(args[1]);
        if (timeout_i >= 0) {
            timeout_ms = timeout_i;
        }
    }

    #if HAVE_ZEPHYR

    if (self->entry_ready == NULL) {
        k_sem_take(&self->sem, timeout_ms == -1 ? K_FOREVER : timeout_ms);
    }

    #else

    mp_uint_t start_tick = mp_hal_ticks_ms();
    for (;;) {
        if (self->entry_ready != NULL
            || (timeout_ms != -1 && mp_hal_ticks_ms() - start_tick >= timeout_ms)) {
            break;
        }
        MICROPY_EVENT_POLL_HOOK
    }

    #endif

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_wait_ms_obj, 1, 2, poll_wait_ms);

STATIC mp_obj_t poll_dump(mp_obj_t self_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);
    printf("all:");
    for (mp_obj_poll_entry_t *entry = self->entry_all; entry != NULL; entry = entry->next_all) {
        printf(" (%p %x:%x)", MP_OBJ_TO_PTR(entry->obj), entry->flags_waiting, entry->flags_ready);
    }
    printf("\nready:");
    for (volatile mp_obj_poll_entry_t *entry = self->entry_ready; entry != NULL; entry = entry->next_ready) {
        printf(" (%p %x:%x)", MP_OBJ_TO_PTR(entry->obj), entry->flags_waiting, entry->flags_ready);
    }
    printf("\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(poll_dump_obj, poll_dump);

STATIC const mp_rom_map_elem_t poll_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&poll_register_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_ms), MP_ROM_PTR(&poll_wait_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_dump), MP_ROM_PTR(&poll_dump_obj) },
};
STATIC MP_DEFINE_CONST_DICT(poll_locals_dict, poll_locals_dict_table);

STATIC const mp_obj_type_t mp_type_poll = {
    { &mp_type_type },
    .name = MP_QSTR_poll,
    .print = poll_print,
    .getiter = mp_identity_getiter,
    .iternext = poll_iternext,
    .locals_dict = (mp_obj_dict_t *)&poll_locals_dict,
};

// poll()
STATIC mp_obj_t mp_uevent_poll(void) {
    mp_obj_poll_t *self = m_new_obj(mp_obj_poll_t);
    self->base.type = &mp_type_poll;
    self->entry_all = NULL;
    self->entry_ready = NULL;
    #if HAVE_ZEPHYR
    k_sem_init(&self->sem, 0, 1);
    #endif
    return MP_OBJ_FROM_PTR(self);
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_uevent_poll_obj, mp_uevent_poll);

STATIC const mp_rom_map_elem_t mp_module_uevent_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uevent) },
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&mp_uevent_poll_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_uevent_globals, mp_module_uevent_globals_table);

const mp_obj_module_t mp_module_uevent = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_uevent_globals,
};

#endif // MICROPY_PY_UEVENT
