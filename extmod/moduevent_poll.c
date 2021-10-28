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
#include "extmod/moduevent.h"

#if MICROPY_PY_UEVENT == MICROPY_PY_UEVENT_IMPL_POLL

#include <poll.h>

typedef mp_obj_poll_entry_base_t mp_obj_poll_entry_t;

struct _mp_obj_poll_t {
    mp_obj_base_t base;
    unsigned short alloc;
    unsigned short len;
    unsigned short iter_idx;
    struct pollfd *pollfds;
    mp_obj_poll_entry_t *entries;
};

STATIC void mp_uevent_poll_entry_init(mp_obj_poll_entry_t *self, mp_obj_poll_t *poller, mp_obj_t obj) {
    self->base.type = &mp_type_poll_entry;
    self->poller = poller;
    self->obj = obj;
    self->user_flags = 0;
    self->user_method_name = MP_QSTR_NULL;
    self->user_data = mp_const_none;
}

STATIC void mp_uevent_entry_clear_events(struct pollfd *pollfd, mp_obj_poll_entry_t *entry, size_t events) {
    pollfd->events &= ~events;
    if (pollfd->events == 0) {
        // Free this pollfd/entry.
        pollfd->fd = -1;
        entry->obj = mp_const_none;
    }
}

mp_obj_t mp_uevent_poll_iternext(mp_obj_t self_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);

    for (; self->iter_idx < self->len; ++self->iter_idx) {
        struct pollfd *pollfd = &self->pollfds[self->iter_idx];
        mp_obj_poll_entry_t *entry = &self->entries[self->iter_idx];
        if (pollfd->revents != 0) {
            entry->user_flags = 0;
            if (pollfd->revents & ~POLLOUT) {
                entry->user_flags |= UEVENT_READ;
            }
            if (pollfd->revents & ~POLLIN) {
                entry->user_flags |= UEVENT_WRITE;
            }
            mp_uevent_entry_clear_events(pollfd, entry, pollfd->revents);
            pollfd->revents = 0;
            return MP_OBJ_FROM_PTR(entry);
        }
    }

    return MP_OBJ_STOP_ITERATION;
}

mp_obj_t mp_uevent_poll_register(size_t n_args, const mp_obj_t *args) {
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
        flags = 0xffff;
    }

    struct pollfd *pollfd = NULL;
    struct pollfd *pollfd_unused = NULL;
    for (size_t i = 0; i < self->len; ++i) {
        if (self->entries[i].obj == args[1]) {
            pollfd = &self->pollfds[i];
            break;
        }
        if (self->entries[i].obj == mp_const_none) {
            pollfd_unused = &self->pollfds[i];
        }
    }

    if (pollfd == NULL) {
        if (pollfd_unused == NULL) {
            if (self->len >= self->alloc) {
                size_t new_alloc = self->alloc + MICROPY_ALLOC_UEVENT_POLL_INC;
                self->pollfds = m_renew(struct pollfd, self->pollfds, self->alloc, new_alloc);
                self->entries = m_renew(mp_obj_poll_entry_t, self->entries, self->alloc, new_alloc);
                self->alloc = new_alloc;
            }
            pollfd = &self->pollfds[self->len++];
        } else {
            pollfd = pollfd_unused;
        }

        pollfd->fd = fd;
        mp_obj_poll_entry_t *entry = &self->entries[pollfd - &self->pollfds[0]];
        mp_uevent_poll_entry_init(entry, self, args[1]);
    }

    if (flags & UEVENT_READ) {
        pollfd->events |= POLLIN;
    }
    if (flags & UEVENT_WRITE) {
        pollfd->events |= POLLOUT;
    }

    return MP_OBJ_FROM_PTR(&self->entries[pollfd - &self->pollfds[0]]);
}

mp_obj_t mp_uevent_poll_unregister(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_entry_t *entry = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t flags = 0xffff;
    if (n_args > 1) {
        flags = mp_obj_get_int(args[1]);
    }
    size_t to_clear = 0;
    if (flags & UEVENT_READ) {
        to_clear |= POLLIN;
    }
    if (flags & UEVENT_WRITE) {
        to_clear |= POLLOUT;
    }
    struct pollfd *pollfd = &entry->poller->pollfds[entry - &entry->poller->entries[0]];
    mp_uevent_entry_clear_events(pollfd, entry, to_clear);
    return MP_OBJ_FROM_PTR(entry);
}

mp_obj_t mp_uevent_poll_poll_ms(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);

    // work out timeout
    mp_uint_t timeout_ms = -1;
    if (n_args >= 2 && args[1] != mp_const_none) {
        mp_int_t timeout_i = mp_obj_get_int(args[1]);
        if (timeout_i >= 0) {
            timeout_ms = timeout_i;
        }
    }

    for (;;) {
        MP_THREAD_GIL_EXIT();
        int ret = poll(self->pollfds, self->len, timeout_ms);
        MP_THREAD_GIL_ENTER();
        if (ret != -1) {
            break;
        }
        if (errno == EINTR) {
            mp_handle_pending(true);
        } else {
            mp_raise_OSError(errno);
        }
    }

    self->iter_idx = 0;

    return args[0];
}

mp_obj_t mp_uevent_poll(void) {
    mp_obj_poll_t *self = m_new_obj(mp_obj_poll_t);
    self->base.type = &mp_type_poll;
    self->alloc = MICROPY_ALLOC_UEVENT_POLL_INIT;
    self->len = 0;
    self->pollfds = m_new(struct pollfd, self->alloc);
    self->entries = m_new(mp_obj_poll_entry_t, self->alloc);
    return MP_OBJ_FROM_PTR(self);
}

#endif // MICROPY_PY_UEVENT == MICROPY_PY_UEVENT_IMPL_POLL
