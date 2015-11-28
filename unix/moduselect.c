/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 * Copyright (c) 2015 Paul Sokolovsky
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

#include <stdio.h>
#include <errno.h>
#include <poll.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/objlist.h"
#include "py/objtuple.h"
#include "py/mphal.h"

/// \class Poll - poll class

typedef struct _mp_obj_poll_t {
    mp_obj_base_t base;
    unsigned short alloc;
    unsigned short len;
    struct pollfd *entries;
} mp_obj_poll_t;

/// \method register(obj[, eventmask])
STATIC mp_obj_t poll_register(uint n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = args[0];
    mp_uint_t flags;
    if (n_args == 3) {
        flags = mp_obj_get_int(args[2]);
    } else {
        flags = POLLIN | POLLOUT;
    }

    int i = 0;
    if (self->len < self->alloc) {
        i = self->len++;
    } else {
        struct pollfd *entries = self->entries;
        for (i = 0; i < self->len; i++, entries++) {
            if (entries->fd == -1) {
                break;
            }
        }
        if (entries->fd != -1) {
            assert(0);
        }
    }

    self->entries[i].fd = mp_obj_get_int(args[1]);
    self->entries[i].events = flags;
    self->entries[i].revents = 0;

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_register_obj, 2, 3, poll_register);

/// \method unregister(obj)
STATIC mp_obj_t poll_unregister(mp_obj_t self_in, mp_obj_t obj_in) {
    mp_obj_poll_t *self = self_in;
    struct pollfd *entries = self->entries;
    int fd = mp_obj_get_int(obj_in);
    for (int i = self->len - 1; i >= 0; i--) {
        if (entries->fd == fd) {
            entries->fd = -1;
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
    mp_obj_poll_t *self = self_in;
    struct pollfd *entries = self->entries;
    int fd = mp_obj_get_int(obj_in);
    for (int i = self->len - 1; i >= 0; i--) {
        if (entries->fd == fd) {
            entries->events = mp_obj_get_int(eventmask_in);
            break;
        }
        entries++;
    }

    // TODO raise KeyError if obj didn't exist in map
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(poll_modify_obj, poll_modify);

/// \method poll([timeout])
/// Timeout is in milliseconds.
STATIC mp_obj_t poll_poll(uint n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = args[0];

    // work out timeout (its given already in ms)
    int timeout = -1;
    if (n_args == 2) {
        if (args[1] != mp_const_none) {
            mp_int_t timeout_i = mp_obj_get_int(args[1]);
            if (timeout_i >= 0) {
                timeout = timeout_i;
            }
        }
    }

    int n_ready = poll(self->entries, self->len, timeout);
    RAISE_ERRNO(n_ready, errno);
    if (n_ready == 0) {
        return mp_const_empty_tuple;
    }

    mp_obj_list_t *ret_list = mp_obj_new_list(n_ready, NULL);
    int ret_i = 0;
    struct pollfd *entries = self->entries;
    for (int i = 0; i < self->len; i++, entries++) {
        if (entries->revents != 0) {
            mp_obj_tuple_t *t = mp_obj_new_tuple(2, NULL);
            t->items[0] = MP_OBJ_NEW_SMALL_INT(entries->fd);
            t->items[1] = MP_OBJ_NEW_SMALL_INT(entries->revents);
            ret_list->items[ret_i++] = t;
        }
    }

    return ret_list;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_poll_obj, 1, 2, poll_poll);

STATIC const mp_map_elem_t poll_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_register), (mp_obj_t)&poll_register_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unregister), (mp_obj_t)&poll_unregister_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_modify), (mp_obj_t)&poll_modify_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_poll), (mp_obj_t)&poll_poll_obj },
};
STATIC MP_DEFINE_CONST_DICT(poll_locals_dict, poll_locals_dict_table);

STATIC const mp_obj_type_t mp_type_poll = {
    { &mp_type_type },
    .name = MP_QSTR_poll,
    .locals_dict = (mp_obj_t)&poll_locals_dict,
};

STATIC mp_obj_t select_poll(mp_uint_t n_args, const mp_obj_t *args) {
    int alloc = 4;
    if (n_args > 0) {
        alloc = mp_obj_get_int(args[0]);
    }
    mp_obj_poll_t *poll = m_new_obj(mp_obj_poll_t);
    poll->base.type = &mp_type_poll;
    poll->entries = m_new(struct pollfd, alloc);
    poll->alloc = alloc;
    poll->len = 0;
    return poll;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_select_poll_obj, 0, 1, select_poll);

STATIC const mp_map_elem_t mp_module_select_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_uselect) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_poll), (mp_obj_t)&mp_select_poll_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLIN), MP_OBJ_NEW_SMALL_INT(POLLIN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLOUT), MP_OBJ_NEW_SMALL_INT(POLLOUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLERR), MP_OBJ_NEW_SMALL_INT(POLLERR) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLHUP), MP_OBJ_NEW_SMALL_INT(POLLHUP) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_select_globals, mp_module_select_globals_table);

const mp_obj_module_t mp_module_uselect = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uselect,
    .globals = (mp_obj_dict_t*)&mp_module_select_globals,
};
