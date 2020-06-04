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

#if MICROPY_PY_UEVENT && defined(__unix__)

#include <poll.h>

typedef struct _mp_obj_poll_t {
    mp_obj_base_t base;
    unsigned short alloc;
    unsigned short len;
    unsigned short iter_idx;
    struct pollfd *entries;
    struct _mp_obj_poll_entry_t *poll_entries;
} mp_obj_poll_t;

typedef struct _mp_obj_poll_entry_t {
    mp_obj_base_t base;
    mp_obj_t obj;
    uint16_t flags;
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
            dest[0] = MP_OBJ_NEW_SMALL_INT(self->flags);
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

STATIC void poll_entry_init(mp_obj_poll_entry_t *self, mp_obj_t obj) {
    self->base.type = &mp_type_poll_entry;
    self->obj = obj;
    self->flags = 0;
    self->user_data = mp_const_none;
    self->user_method_name = MP_QSTR_NULL;
}

STATIC mp_obj_t poll_iternext(mp_obj_t self_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);

    struct pollfd *entries = self->entries + self->iter_idx;
    for (int i = self->iter_idx; i < self->len; i++, entries++) {
        self->iter_idx++;
        if (entries->revents != 0) {
            mp_obj_poll_entry_t *poll_entry = &self->poll_entries[i];
            poll_entry->flags = 0;
            if (entries->revents & POLLIN) {
                poll_entry->flags |= 1;
            }
            if (entries->revents & POLLOUT) {
                poll_entry->flags |= 2;
            }
            entries->events &= ~entries->revents;
            entries->revents = 0;
            return MP_OBJ_FROM_PTR(poll_entry);
        }
    }

    return MP_OBJ_STOP_ITERATION;
}

// register(obj[, eventmask])
STATIC mp_obj_t poll_register(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);

    const mp_stream_p_t *stream_p = mp_get_stream_raise(args[1], MP_STREAM_OP_IOCTL);
    int err;
    mp_uint_t res = stream_p->ioctl(args[1], MP_STREAM_GET_FILENO, 0, &err);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(err);
    }
    int fd = res;

    mp_uint_t flags;
    if (n_args == 3) {
        flags = mp_obj_get_int(args[2]);
    } else {
        flags = 0xff;
    }

    struct pollfd *free_slot = NULL;

    struct pollfd *entry = self->entries;
    int idx = -1;
    for (int i = 0; i < self->len; i++, entry++) {
        int entry_fd = entry->fd;
        if (entry_fd == fd) {
            free_slot = entry;
            idx = i;
            break;
        }
        if (entry_fd == -1) {
            free_slot = entry;
        }
    }

    if (idx == -1) {
        if (free_slot == NULL) {
            if (self->len >= self->alloc) {
                self->entries = m_renew(struct pollfd, self->entries, self->alloc, self->alloc + 4);
                self->poll_entries = m_renew(mp_obj_poll_entry_t, self->poll_entries, self->alloc, self->alloc + 4);
                self->alloc += 4;
            }
            free_slot = &self->entries[self->len++];
        }

        idx = free_slot - self->entries;
        poll_entry_init(&self->poll_entries[idx], args[1]);
        free_slot->fd = fd;
    }

    if (flags & 1) {
        free_slot->events |= POLLIN;
    }
    if (flags & 2) {
        free_slot->events |= POLLOUT;
    }

    return MP_OBJ_FROM_PTR(&self->poll_entries[idx]);
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

    /*
    printf("pollfd:");
    struct pollfd *entry = self->entries;
    for (int i = 0; i < self->len; i++, entry++) {
        printf("(%d %d %d)", entry->fd, entry->events, entry->revents);
    }
    printf("\n");
    */

    for (;;) {
        MP_THREAD_GIL_EXIT();
        int n_ready = poll(self->entries, self->len, timeout_ms);
        MP_THREAD_GIL_ENTER();
        if (ret != -1) {
            break;
        }
        if (errno == EINTR) {
            mp_handle_pending(true);
            /*
            if (self->notify) {
                break;
            }
            */
        } else {
            mp_raise_OSError(errno);
        }
    }

    self->iter_idx = 0;

    return args[0];
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_wait_ms_obj, 1, 2, poll_wait_ms);

STATIC const mp_rom_map_elem_t poll_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&poll_register_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_ms), MP_ROM_PTR(&poll_wait_ms_obj) },
};
STATIC MP_DEFINE_CONST_DICT(poll_locals_dict, poll_locals_dict_table);

STATIC const mp_obj_type_t mp_type_poll = {
    { &mp_type_type },
    .name = MP_QSTR_poll,
    .getiter = mp_identity_getiter,
    .iternext = poll_iternext,
    .locals_dict = (mp_obj_dict_t *)&poll_locals_dict,
};

// poll()
STATIC mp_obj_t mp_uevent_poll(void) {
    mp_obj_poll_t *self = m_new_obj(mp_obj_poll_t);
    self->base.type = &mp_type_poll;
    self->alloc = 4;
    self->len = 0;
    self->entries = m_new(struct pollfd, self->alloc);
    self->poll_entries = m_new(mp_obj_poll_entry_t, self->alloc);
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
