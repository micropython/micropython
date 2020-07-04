/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 * Copyright (c) 2015-2017 Paul Sokolovsky
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

#if MICROPY_PY_USELECT_POSIX

#include <stdio.h>
#include <errno.h>
#include <poll.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/obj.h"
#include "py/objlist.h"
#include "py/objtuple.h"
#include "py/mphal.h"
#include "py/mpthread.h"

#define DEBUG 0

#if MICROPY_PY_SOCKET
extern const mp_obj_type_t mp_type_socket;
#endif

// Flags for poll()
#define FLAG_ONESHOT (1)

/// \class Poll - poll class

typedef struct _mp_obj_poll_t {
    mp_obj_base_t base;
    unsigned short alloc;
    unsigned short len;
    struct pollfd *entries;
    mp_obj_t *obj_map;
    short iter_cnt;
    short iter_idx;
    int flags;
    // callee-owned tuple
    mp_obj_t ret_tuple;
} mp_obj_poll_t;

STATIC int get_fd(mp_obj_t fdlike) {
    if (mp_obj_is_obj(fdlike)) {
        const mp_stream_p_t *stream_p = mp_get_stream_raise(fdlike, MP_STREAM_OP_IOCTL);
        int err;
        mp_uint_t res = stream_p->ioctl(fdlike, MP_STREAM_GET_FILENO, 0, &err);
        if (res != MP_STREAM_ERROR) {
            return res;
        }
    }
    return mp_obj_get_int(fdlike);
}

/// \method register(obj[, eventmask])
STATIC mp_obj_t poll_register(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);
    bool is_fd = mp_obj_is_int(args[1]);
    int fd = get_fd(args[1]);

    mp_uint_t flags;
    if (n_args == 3) {
        flags = mp_obj_get_int(args[2]);
    } else {
        flags = POLLIN | POLLOUT;
    }

    struct pollfd *free_slot = NULL;

    struct pollfd *entry = self->entries;
    for (int i = 0; i < self->len; i++, entry++) {
        int entry_fd = entry->fd;
        if (entry_fd == fd) {
            entry->events = flags;
            return mp_const_false;
        }
        if (entry_fd == -1) {
            free_slot = entry;
        }
    }

    if (free_slot == NULL) {
        if (self->len >= self->alloc) {
            self->entries = m_renew(struct pollfd, self->entries, self->alloc, self->alloc + 4);
            if (self->obj_map) {
                self->obj_map = m_renew(mp_obj_t, self->obj_map, self->alloc, self->alloc + 4);
            }
            self->alloc += 4;
        }
        free_slot = &self->entries[self->len++];
    }

    if (!is_fd) {
        if (self->obj_map == NULL) {
            self->obj_map = m_new0(mp_obj_t, self->alloc);
        }
        self->obj_map[free_slot - self->entries] = args[1];
    }

    free_slot->fd = fd;
    free_slot->events = flags;
    free_slot->revents = 0;
    return mp_const_true;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_register_obj, 2, 3, poll_register);

/// \method unregister(obj)
STATIC mp_obj_t poll_unregister(mp_obj_t self_in, mp_obj_t obj_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);
    struct pollfd *entries = self->entries;
    int fd = get_fd(obj_in);
    for (int i = self->len - 1; i >= 0; i--) {
        if (entries->fd == fd) {
            entries->fd = -1;
            if (self->obj_map) {
                self->obj_map[entries - self->entries] = MP_OBJ_NULL;
            }
            break;
        }
        entries++;
    }

    // TODO raise KeyError if obj didn't exist in map
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(poll_unregister_obj, poll_unregister);

/// \method modify(obj, eventmask)
STATIC mp_obj_t poll_modify(mp_obj_t self_in, mp_obj_t obj_in, mp_obj_t eventmask_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);
    struct pollfd *entries = self->entries;
    int fd = get_fd(obj_in);
    for (int i = self->len - 1; i >= 0; i--) {
        if (entries->fd == fd) {
            entries->events = mp_obj_get_int(eventmask_in);
            return mp_const_none;
        }
        entries++;
    }

    // obj doesn't exist in poller
    mp_raise_OSError(MP_ENOENT);
}
MP_DEFINE_CONST_FUN_OBJ_3(poll_modify_obj, poll_modify);

STATIC int poll_poll_internal(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);

    // work out timeout (it's given already in ms)
    int timeout = -1;
    int flags = 0;
    if (n_args >= 2) {
        if (args[1] != mp_const_none) {
            mp_int_t timeout_i = mp_obj_get_int(args[1]);
            if (timeout_i >= 0) {
                timeout = timeout_i;
            }
        }
        if (n_args >= 3) {
            flags = mp_obj_get_int(args[2]);
        }
    }

    self->flags = flags;

    int n_ready;
    MP_HAL_RETRY_SYSCALL(n_ready, poll(self->entries, self->len, timeout), mp_raise_OSError(err));
    return n_ready;
}

/// \method poll([timeout])
/// Timeout is in milliseconds.
STATIC mp_obj_t poll_poll(size_t n_args, const mp_obj_t *args) {
    int n_ready = poll_poll_internal(n_args, args);

    if (n_ready == 0) {
        return mp_const_empty_tuple;
    }

    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);

    mp_obj_list_t *ret_list = MP_OBJ_TO_PTR(mp_obj_new_list(n_ready, NULL));
    int ret_i = 0;
    struct pollfd *entries = self->entries;
    for (int i = 0; i < self->len; i++, entries++) {
        if (entries->revents != 0) {
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
            // If there's an object stored, return it, otherwise raw fd
            if (self->obj_map && self->obj_map[i] != MP_OBJ_NULL) {
                t->items[0] = self->obj_map[i];
            } else {
                t->items[0] = MP_OBJ_NEW_SMALL_INT(entries->fd);
            }
            t->items[1] = MP_OBJ_NEW_SMALL_INT(entries->revents);
            ret_list->items[ret_i++] = MP_OBJ_FROM_PTR(t);
            if (self->flags & FLAG_ONESHOT) {
                entries->events = 0;
            }
        }
    }

    return MP_OBJ_FROM_PTR(ret_list);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_poll_obj, 1, 3, poll_poll);

STATIC mp_obj_t poll_ipoll(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);

    if (self->ret_tuple == MP_OBJ_NULL) {
        self->ret_tuple = mp_obj_new_tuple(2, NULL);
    }

    int n_ready = poll_poll_internal(n_args, args);
    self->iter_cnt = n_ready;
    self->iter_idx = 0;

    return args[0];
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_ipoll_obj, 1, 3, poll_ipoll);

STATIC mp_obj_t poll_iternext(mp_obj_t self_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->iter_cnt == 0) {
        return MP_OBJ_STOP_ITERATION;
    }

    self->iter_cnt--;

    struct pollfd *entries = self->entries + self->iter_idx;
    for (int i = self->iter_idx; i < self->len; i++, entries++) {
        self->iter_idx++;
        if (entries->revents != 0) {
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(self->ret_tuple);
            // If there's an object stored, return it, otherwise raw fd
            if (self->obj_map && self->obj_map[i] != MP_OBJ_NULL) {
                t->items[0] = self->obj_map[i];
            } else {
                t->items[0] = MP_OBJ_NEW_SMALL_INT(entries->fd);
            }
            t->items[1] = MP_OBJ_NEW_SMALL_INT(entries->revents);
            if (self->flags & FLAG_ONESHOT) {
                entries->events = 0;
            }
            return MP_OBJ_FROM_PTR(t);
        }
    }

    assert(!"inconsistent number of poll active entries");
    self->iter_cnt = 0;
    return MP_OBJ_STOP_ITERATION;
}

#if DEBUG
STATIC mp_obj_t poll_dump(mp_obj_t self_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);

    struct pollfd *entries = self->entries;
    for (int i = self->len - 1; i >= 0; i--) {
        printf("fd: %d ev: %x rev: %x", entries->fd, entries->events, entries->revents);
        if (self->obj_map) {
            printf(" obj: %p", self->obj_map[entries - self->entries]);
        }
        printf("\n");
        entries++;
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(poll_dump_obj, poll_dump);
#endif

STATIC const mp_rom_map_elem_t poll_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&poll_register_obj) },
    { MP_ROM_QSTR(MP_QSTR_unregister), MP_ROM_PTR(&poll_unregister_obj) },
    { MP_ROM_QSTR(MP_QSTR_modify), MP_ROM_PTR(&poll_modify_obj) },
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&poll_poll_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipoll), MP_ROM_PTR(&poll_ipoll_obj) },
    #if DEBUG
    { MP_ROM_QSTR(MP_QSTR_dump), MP_ROM_PTR(&poll_dump_obj) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(poll_locals_dict, poll_locals_dict_table);

STATIC const mp_obj_type_t mp_type_poll = {
    { &mp_type_type },
    .name = MP_QSTR_poll,
    .getiter = mp_identity_getiter,
    .iternext = poll_iternext,
    .locals_dict = (void *)&poll_locals_dict,
};

STATIC mp_obj_t select_poll(size_t n_args, const mp_obj_t *args) {
    int alloc = 4;
    if (n_args > 0) {
        alloc = mp_obj_get_int(args[0]);
    }
    mp_obj_poll_t *poll = m_new_obj(mp_obj_poll_t);
    poll->base.type = &mp_type_poll;
    poll->entries = m_new(struct pollfd, alloc);
    poll->alloc = alloc;
    poll->len = 0;
    poll->obj_map = NULL;
    poll->iter_cnt = 0;
    poll->ret_tuple = MP_OBJ_NULL;
    return MP_OBJ_FROM_PTR(poll);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_select_poll_obj, 0, 1, select_poll);

STATIC const mp_rom_map_elem_t mp_module_select_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uselect) },
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&mp_select_poll_obj) },
    { MP_ROM_QSTR(MP_QSTR_POLLIN), MP_ROM_INT(POLLIN) },
    { MP_ROM_QSTR(MP_QSTR_POLLOUT), MP_ROM_INT(POLLOUT) },
    { MP_ROM_QSTR(MP_QSTR_POLLERR), MP_ROM_INT(POLLERR) },
    { MP_ROM_QSTR(MP_QSTR_POLLHUP), MP_ROM_INT(POLLHUP) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_select_globals, mp_module_select_globals_table);

const mp_obj_module_t mp_module_uselect = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_select_globals,
};

#endif // MICROPY_PY_USELECT_POSIX
