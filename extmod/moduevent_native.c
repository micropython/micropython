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
#include "extmod/moduevent.h"

#if MICROPY_PY_UEVENT == MICROPY_PY_UEVENT_IMPL_NATIVE

struct _mp_obj_poll_t {
    mp_obj_base_t base;
    unsigned short alloc;
    unsigned short len;
    struct _mp_obj_poll_entry_t *entries;
    struct _mp_obj_poll_entry_t *entry_ready;
    struct _mp_obj_poll_entry_t *entry_free;
};

typedef struct _mp_obj_poll_entry_t {
    mp_obj_poll_entry_base_t base;
    struct _mp_obj_poll_entry_t *next;
    mp_uint_t (*ioctl)(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode);
    uint16_t flags_waiting;
    uint16_t flags_ready;
} mp_obj_poll_entry_t;

STATIC mp_obj_poll_entry_t *mp_uevent_poll_entry_new(mp_obj_poll_t *poller) {
    if (poller->len >= poller->alloc) {
        size_t new_alloc = poller->alloc + MICROPY_ALLOC_UEVENT_POLL_INC;
        poller->entries = m_renew(mp_obj_poll_entry_t, poller->entries, poller->alloc, new_alloc);
        poller->alloc = new_alloc;
    }
    mp_obj_poll_entry_t *self = &poller->entries[poller->len++];
    self->base.base.type = &mp_type_poll_entry;
    self->base.poller = poller;
    self->base.user_method_name = MP_QSTR_NULL;
    self->base.user_data = mp_const_none;
    self->next = NULL;
    return self;
}

STATIC void mp_uevent_poll_entry_init(mp_obj_poll_entry_t *self, mp_obj_t obj) {
    self->base.obj = obj;
    self->base.user_flags = 0;
    self->ioctl = mp_get_stream_raise(obj, MP_STREAM_OP_IOCTL)->ioctl;
    self->flags_waiting = 0;
    self->flags_ready = 0;
}

STATIC void mp_uevent_poll_entry_process_event(mp_obj_poll_entry_t *entry, uint16_t flags) {
    if (!(entry->flags_waiting & flags)) {
        return;
    }
    uint16_t old_flags_ready = entry->flags_ready;
    entry->flags_ready |= entry->flags_waiting & flags;
    entry->flags_waiting &= ~flags;
    if (old_flags_ready == 0 && entry->flags_ready) {
        // Put entry on the ready linked list.
        entry->next = entry->base.poller->entry_ready;
        entry->base.poller->entry_ready = entry;
    }
}

STATIC void mp_uevent_entry_clear_events(mp_obj_poll_t *self, mp_obj_poll_entry_t *entry, uint16_t flags) {
    entry->flags_waiting &= ~flags;
    if (entry->flags_waiting == 0) {
        // Move entry to free list.
        entry->base.obj = mp_const_none;
        entry->next = self->entry_free;
        self->entry_free = entry;
    }
}

mp_obj_t mp_uevent_poll_iternext(mp_obj_t self_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->entry_ready != NULL) {
        mp_obj_poll_entry_t *entry = self->entry_ready;
        self->entry_ready = entry->next;
        entry->base.user_flags = entry->flags_ready;
        entry->flags_ready = 0;
        mp_uevent_entry_clear_events(self, entry, 0);
        return MP_OBJ_FROM_PTR(entry);
    }

    return MP_OBJ_STOP_ITERATION;
}

mp_obj_t mp_uevent_poll_register(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t flags;
    if (n_args == 3) {
        flags = mp_obj_get_int(args[2]);
        // flags must be non-zero.
    } else {
        flags = 0xffff;
    }
    mp_obj_poll_entry_t *entry = NULL;
    for (size_t i = 0; i < self->len; ++i) {
        if (self->entries[i].base.obj == args[1]) {
            entry = &self->entries[i];
            break;
        }
    }
    if (entry == NULL) {
        if (self->entry_free != NULL) {
            entry = self->entry_free;
            self->entry_free = entry->next;
        } else {
            entry = mp_uevent_poll_entry_new(self);
        }
        mp_uevent_poll_entry_init(entry, args[1]);
    }

    entry->flags_waiting |= flags;

    return MP_OBJ_FROM_PTR(entry);
}

mp_obj_t mp_uevent_poll_unregister(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_entry_t *entry = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t flags = 0xffff;
    if (n_args > 1) {
        flags = mp_obj_get_int(args[1]);
    }
    mp_uevent_entry_clear_events(entry->base.poller, entry, flags);
    return MP_OBJ_FROM_PTR(entry);
}

STATIC void mp_uevent_poll_check_waiting(mp_obj_poll_t *self) {
    for (size_t i = 0; i < self->len; ++i) {
        mp_obj_poll_entry_t *entry = &self->entries[i];
        if (entry->flags_waiting == 0) {
            continue;
        }

        uintptr_t poll_flags = 0;
        if (entry->flags_waiting & UEVENT_READ) {
            poll_flags |= MP_STREAM_POLL_RD;
        }
        if (entry->flags_waiting & UEVENT_WRITE) {
            poll_flags |= MP_STREAM_POLL_WR;
        }

        int errcode;
        mp_int_t ret = entry->ioctl(entry->base.obj, MP_STREAM_POLL, poll_flags, &errcode);

        if (ret == -1) {
            // error doing ioctl
            mp_raise_OSError(errcode);
        }

        if (ret != 0) {
            uint16_t flags_ready = 0;
            if (ret & ~MP_STREAM_POLL_WR) {
                flags_ready |= UEVENT_READ;
            }
            if (ret & ~MP_STREAM_POLL_RD) {
                flags_ready |= UEVENT_WRITE;
            }
            mp_uevent_poll_entry_process_event(entry, flags_ready);
        }
    }
}

mp_obj_t mp_uevent_poll_poll_ms(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);

    // Determine timeout, in milliseconds.
    mp_uint_t timeout_ms = -1;
    if (n_args >= 2 && args[1] != mp_const_none) {
        mp_int_t timeout_i = mp_obj_get_int(args[1]);
        if (timeout_i >= 0) {
            timeout_ms = timeout_i;
        }
    }

    // Wait for event or timeout.
    mp_uint_t start_tick = mp_hal_ticks_ms();
    for (;;) {
        mp_uevent_poll_check_waiting(self);
        if (self->entry_ready != NULL
            || (timeout_ms != -1 && mp_hal_ticks_ms() - start_tick >= timeout_ms)) {
            break;
        }
        MICROPY_EVENT_POLL_HOOK
    }

    return args[0];
}

/*
STATIC mp_obj_t poll_dump(mp_obj_t self_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);
    printf("all:");
    for (size_t i = 0; i < self->len; ++i) {
        mp_obj_poll_entry_t *entry = &self->entries[i];
        printf(" (%p %x:%x)", MP_OBJ_TO_PTR(entry->obj), entry->flags_waiting, entry->flags_ready);
    }
    printf("\nready:");
    for (volatile mp_obj_poll_entry_t *entry = self->entry_ready; entry != NULL; entry = entry->next) {
        printf(" (%p %x:%x)", MP_OBJ_TO_PTR(entry->obj), entry->flags_waiting, entry->flags_ready);
    }
    printf("\n");
    return mp_const_none;
}
*/

mp_obj_t mp_uevent_poll(void) {
    mp_obj_poll_t *self = m_new_obj(mp_obj_poll_t);
    self->base.type = &mp_type_poll;
    self->alloc = MICROPY_ALLOC_UEVENT_POLL_INIT;
    self->len = 0;
    self->entries = m_new(mp_obj_poll_entry_t, self->alloc);
    self->entry_ready = NULL;
    self->entry_free = NULL;
    return MP_OBJ_FROM_PTR(self);
}

#endif // MICROPY_PY_UEVENT == MICROPY_PY_UEVENT_IMPL_NATIVE
