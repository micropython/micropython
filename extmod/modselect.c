/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2023 Damien P. George
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
#include "py/runtime.h"
#include "py/obj.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_SELECT

#if MICROPY_PY_SELECT_SELECT && MICROPY_PY_SELECT_POSIX_OPTIMISATIONS
#error "select.select is not supported with MICROPY_PY_SELECT_POSIX_OPTIMISATIONS"
#endif

#if MICROPY_PY_SELECT_POSIX_OPTIMISATIONS

#include <string.h>
#include <poll.h>

#if !((MP_STREAM_POLL_RD) == (POLLIN) && \
    (MP_STREAM_POLL_WR) == (POLLOUT) && \
    (MP_STREAM_POLL_ERR) == (POLLERR) && \
    (MP_STREAM_POLL_HUP) == (POLLHUP) && \
    (MP_STREAM_POLL_NVAL) == (POLLNVAL))
#error "With MICROPY_PY_SELECT_POSIX_OPTIMISATIONS enabled, POLL constants must match"
#endif

// When non-file-descriptor objects are on the list to be polled (the polling of
// which involves repeatedly calling ioctl(MP_STREAM_POLL)), this variable sets
// the period between polling these objects.
#define MICROPY_PY_SELECT_IOCTL_CALL_PERIOD_MS (1)

#endif

// Flags for ipoll()
#define FLAG_ONESHOT (1)

// A single pollable object.
typedef struct _poll_obj_t {
    mp_obj_t obj;
    mp_uint_t (*ioctl)(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode);
    #if MICROPY_PY_SELECT_POSIX_OPTIMISATIONS
    // If the pollable object has an associated file descriptor, then pollfd points to an entry
    // in poll_set_t::pollfds, and the events/revents fields for this object are stored in the
    // pollfd entry (and the nonfd_* members are unused).
    // Otherwise the object is a non-file-descriptor object and pollfd==NULL, and the events/
    // revents fields are stored in the nonfd_* members (which are named as such so that code
    // doesn't accidentally mix the use of these members when this optimisation is used).
    struct pollfd *pollfd;
    uint16_t nonfd_events;
    uint16_t nonfd_revents;
    #else
    mp_uint_t events;
    mp_uint_t revents;
    #endif
} poll_obj_t;

// A set of pollable objects.
typedef struct _poll_set_t {
    // Map containing a dict with key=object to poll, value=its corresponding poll_obj_t.
    mp_map_t map;

    #if MICROPY_PY_SELECT_POSIX_OPTIMISATIONS
    // Array of pollfd entries for objects that have a file descriptor.
    unsigned short alloc; // memory allocated for pollfds
    unsigned short max_used; // maximum number of used entries in pollfds
    unsigned short used; // actual number of used entries in pollfds
    struct pollfd *pollfds;
    #endif
} poll_set_t;

static void poll_set_init(poll_set_t *poll_set, size_t n) {
    mp_map_init(&poll_set->map, n);
    #if MICROPY_PY_SELECT_POSIX_OPTIMISATIONS
    poll_set->alloc = 0;
    poll_set->max_used = 0;
    poll_set->used = 0;
    poll_set->pollfds = NULL;
    #endif
}

#if MICROPY_PY_SELECT_SELECT
static void poll_set_deinit(poll_set_t *poll_set) {
    mp_map_deinit(&poll_set->map);
}
#endif

#if MICROPY_PY_SELECT_POSIX_OPTIMISATIONS

static mp_uint_t poll_obj_get_events(poll_obj_t *poll_obj) {
    assert(poll_obj->pollfd == NULL);
    return poll_obj->nonfd_events;
}

static void poll_obj_set_events(poll_obj_t *poll_obj, mp_uint_t events) {
    if (poll_obj->pollfd != NULL) {
        poll_obj->pollfd->events = events;
    } else {
        poll_obj->nonfd_events = events;
    }
}

static mp_uint_t poll_obj_get_revents(poll_obj_t *poll_obj) {
    if (poll_obj->pollfd != NULL) {
        return poll_obj->pollfd->revents;
    } else {
        return poll_obj->nonfd_revents;
    }
}

static void poll_obj_set_revents(poll_obj_t *poll_obj, mp_uint_t revents) {
    if (poll_obj->pollfd != NULL) {
        poll_obj->pollfd->revents = revents;
    } else {
        poll_obj->nonfd_revents = revents;
    }
}

// How much (in pollfds) to grow the allocation for poll_set->pollfds by.
#define POLL_SET_ALLOC_INCREMENT (4)

static struct pollfd *poll_set_add_fd(poll_set_t *poll_set, int fd) {
    struct pollfd *free_slot = NULL;

    if (poll_set->used == poll_set->max_used) {
        // No free slots below max_used, so expand max_used (and possibly allocate).
        if (poll_set->max_used >= poll_set->alloc) {
            size_t new_alloc = poll_set->alloc + POLL_SET_ALLOC_INCREMENT;
            // Try to grow in-place.
            struct pollfd *new_fds = m_renew_maybe(struct pollfd, poll_set->pollfds, poll_set->alloc, new_alloc, false);
            if (!new_fds) {
                // Failed to grow in-place. Do a new allocation and copy over the pollfd values.
                new_fds = m_new(struct pollfd, new_alloc);
                memcpy(new_fds, poll_set->pollfds, sizeof(struct pollfd) * poll_set->alloc);

                // Update existing poll_obj_t to update their pollfd field to
                // point to the same offset inside the new allocation.
                for (mp_uint_t i = 0; i < poll_set->map.alloc; ++i) {
                    if (!mp_map_slot_is_filled(&poll_set->map, i)) {
                        continue;
                    }

                    poll_obj_t *poll_obj = MP_OBJ_TO_PTR(poll_set->map.table[i].value);
                    if (!poll_obj) {
                        // This is the one we're currently adding,
                        // poll_set_add_obj doesn't assign elem->value until
                        // afterwards.
                        continue;
                    }

                    poll_obj->pollfd = new_fds + (poll_obj->pollfd - poll_set->pollfds);
                }

                // Delete the old allocation.
                m_del(struct pollfd, poll_set->pollfds, poll_set->alloc);
            }

            poll_set->pollfds = new_fds;
            poll_set->alloc = new_alloc;
        }
        free_slot = &poll_set->pollfds[poll_set->max_used++];
    } else {
        // There should be a free slot below max_used.
        for (unsigned int i = 0; i < poll_set->max_used; ++i) {
            struct pollfd *slot = &poll_set->pollfds[i];
            if (slot->fd == -1) {
                free_slot = slot;
                break;
            }
        }
        assert(free_slot != NULL);
    }

    free_slot->fd = fd;
    ++poll_set->used;

    return free_slot;
}

static inline bool poll_set_all_are_fds(poll_set_t *poll_set) {
    return poll_set->map.used == poll_set->used;
}

#else

static inline mp_uint_t poll_obj_get_events(poll_obj_t *poll_obj) {
    return poll_obj->events;
}

static inline void poll_obj_set_events(poll_obj_t *poll_obj, mp_uint_t events) {
    poll_obj->events = events;
}

static inline mp_uint_t poll_obj_get_revents(poll_obj_t *poll_obj) {
    return poll_obj->revents;
}

static inline void poll_obj_set_revents(poll_obj_t *poll_obj, mp_uint_t revents) {
    poll_obj->revents = revents;
}

#endif

static void poll_set_add_obj(poll_set_t *poll_set, const mp_obj_t *obj, mp_uint_t obj_len, mp_uint_t events, bool or_events) {
    for (mp_uint_t i = 0; i < obj_len; i++) {
        mp_map_elem_t *elem = mp_map_lookup(&poll_set->map, mp_obj_id(obj[i]), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
        if (elem->value == MP_OBJ_NULL) {
            // object not found; get its ioctl and add it to the poll list

            // If an exception is raised below when adding the new object then the map entry for that
            // object remains unpopulated, and methods like poll() may crash.  This case is not handled.

            poll_obj_t *poll_obj = m_new_obj(poll_obj_t);
            poll_obj->obj = obj[i];

            #if MICROPY_PY_SELECT_POSIX_OPTIMISATIONS
            int fd = -1;
            if (mp_obj_is_int(obj[i])) {
                // A file descriptor integer passed in as the object, so use it directly.
                fd = mp_obj_get_int(obj[i]);
                if (fd < 0) {
                    mp_raise_ValueError(NULL);
                }
                poll_obj->ioctl = NULL;
            } else {
                // An object passed in.  Check if it has a file descriptor.
                const mp_stream_p_t *stream_p = mp_get_stream_raise(obj[i], MP_STREAM_OP_IOCTL);
                poll_obj->ioctl = stream_p->ioctl;
                int err;
                mp_uint_t res = stream_p->ioctl(obj[i], MP_STREAM_GET_FILENO, 0, &err);
                if (res != MP_STREAM_ERROR) {
                    fd = res;
                }
            }
            if (fd >= 0) {
                // Object has a file descriptor so add it to pollfds.
                poll_obj->pollfd = poll_set_add_fd(poll_set, fd);
            } else {
                // Object doesn't have a file descriptor.
                poll_obj->pollfd = NULL;
            }
            #else
            const mp_stream_p_t *stream_p = mp_get_stream_raise(obj[i], MP_STREAM_OP_IOCTL);
            poll_obj->ioctl = stream_p->ioctl;
            #endif

            poll_obj_set_events(poll_obj, events);
            poll_obj_set_revents(poll_obj, 0);
            elem->value = MP_OBJ_FROM_PTR(poll_obj);
        } else {
            // object exists; update its events
            poll_obj_t *poll_obj = (poll_obj_t *)MP_OBJ_TO_PTR(elem->value);
            #if MICROPY_PY_SELECT_SELECT
            if (or_events) {
                events |= poll_obj_get_events(poll_obj);
            }
            #else
            (void)or_events;
            #endif
            poll_obj_set_events(poll_obj, events);
        }
    }
}

// For each object in the poll set, poll it once.
static mp_uint_t poll_set_poll_once(poll_set_t *poll_set, size_t *rwx_num) {
    mp_uint_t n_ready = 0;
    for (mp_uint_t i = 0; i < poll_set->map.alloc; ++i) {
        if (!mp_map_slot_is_filled(&poll_set->map, i)) {
            continue;
        }

        poll_obj_t *poll_obj = MP_OBJ_TO_PTR(poll_set->map.table[i].value);

        #if MICROPY_PY_SELECT_POSIX_OPTIMISATIONS
        if (poll_obj->pollfd != NULL) {
            // Object has file descriptor so will be polled separately by poll().
            continue;
        }
        #endif

        int errcode;
        mp_int_t ret = poll_obj->ioctl(poll_obj->obj, MP_STREAM_POLL, poll_obj_get_events(poll_obj), &errcode);
        poll_obj_set_revents(poll_obj, ret);

        if (ret == -1) {
            // error doing ioctl
            mp_raise_OSError(errcode);
        }

        if (ret != 0) {
            // object is ready
            n_ready += 1;
            #if MICROPY_PY_SELECT_SELECT
            if (rwx_num != NULL) {
                if (ret & MP_STREAM_POLL_RD) {
                    rwx_num[0] += 1;
                }
                if (ret & MP_STREAM_POLL_WR) {
                    rwx_num[1] += 1;
                }
                if ((ret & ~(MP_STREAM_POLL_RD | MP_STREAM_POLL_WR)) != 0) {
                    rwx_num[2] += 1;
                }
            }
            #else
            (void)rwx_num;
            #endif
        }
    }
    return n_ready;
}

static mp_uint_t poll_set_poll_until_ready_or_timeout(poll_set_t *poll_set, size_t *rwx_num, mp_uint_t timeout) {
    mp_uint_t start_ticks = mp_hal_ticks_ms();
    bool has_timeout = timeout != (mp_uint_t)-1;

    #if MICROPY_PY_SELECT_POSIX_OPTIMISATIONS

    for (;;) {
        MP_THREAD_GIL_EXIT();

        // Compute the timeout.
        int t = MICROPY_PY_SELECT_IOCTL_CALL_PERIOD_MS;
        if (poll_set_all_are_fds(poll_set)) {
            // All our pollables are file descriptors, so we can use a blocking
            // poll and let it (the underlying system) handle the timeout.
            if (timeout == (mp_uint_t)-1) {
                t = -1;
            } else {
                mp_uint_t delta = mp_hal_ticks_ms() - start_ticks;
                if (delta >= timeout) {
                    t = 0;
                } else {
                    t = timeout - delta;
                }
            }
        }

        // Call system poll for those objects that have a file descriptor.
        int n_ready = poll(poll_set->pollfds, poll_set->max_used, t);

        MP_THREAD_GIL_ENTER();

        // The call to poll() may have been interrupted, but per PEP 475 we must retry if the
        // signal is EINTR (this implements a special case of calling MP_HAL_RETRY_SYSCALL()).
        if (n_ready == -1) {
            int err = errno;
            if (err != EINTR) {
                mp_raise_OSError(err);
            }
            n_ready = 0;
        }

        // Explicitly poll any objects that do not have a file descriptor.
        if (!poll_set_all_are_fds(poll_set)) {
            n_ready += poll_set_poll_once(poll_set, rwx_num);
        }

        // Return if an object is ready, or if the timeout expired.
        if (n_ready > 0 || (has_timeout && mp_hal_ticks_ms() - start_ticks >= timeout)) {
            return n_ready;
        }

        // This would be mp_event_wait_ms() but the call to poll() above already includes a delay.
        mp_event_handle_nowait();
    }

    #else

    for (;;) {
        // poll the objects
        mp_uint_t n_ready = poll_set_poll_once(poll_set, rwx_num);
        uint32_t elapsed = mp_hal_ticks_ms() - start_ticks;
        if (n_ready > 0 || (has_timeout && elapsed >= timeout)) {
            return n_ready;
        }
        if (has_timeout) {
            mp_event_wait_ms(timeout - elapsed);
        } else {
            mp_event_wait_indefinite();
        }
    }

    #endif
}

#if MICROPY_PY_SELECT_SELECT
// select(rlist, wlist, xlist[, timeout])
static mp_obj_t select_select(size_t n_args, const mp_obj_t *args) {
    // get array data from tuple/list arguments
    size_t rwx_len[3];
    mp_obj_t *r_array, *w_array, *x_array;
    mp_obj_get_array(args[0], &rwx_len[0], &r_array);
    mp_obj_get_array(args[1], &rwx_len[1], &w_array);
    mp_obj_get_array(args[2], &rwx_len[2], &x_array);

    // get timeout
    mp_uint_t timeout = -1;
    if (n_args == 4) {
        if (args[3] != mp_const_none) {
            #if MICROPY_PY_BUILTINS_FLOAT
            float timeout_f = mp_obj_get_float_to_f(args[3]);
            if (timeout_f >= 0) {
                timeout = (mp_uint_t)(timeout_f * 1000);
            }
            #else
            timeout = mp_obj_get_int(args[3]) * 1000;
            #endif
        }
    }

    // merge separate lists and get the ioctl function for each object
    poll_set_t poll_set;
    poll_set_init(&poll_set, rwx_len[0] + rwx_len[1] + rwx_len[2]);
    poll_set_add_obj(&poll_set, r_array, rwx_len[0], MP_STREAM_POLL_RD, true);
    poll_set_add_obj(&poll_set, w_array, rwx_len[1], MP_STREAM_POLL_WR, true);
    poll_set_add_obj(&poll_set, x_array, rwx_len[2], MP_STREAM_POLL_ERR | MP_STREAM_POLL_HUP, true);

    // poll all objects
    rwx_len[0] = rwx_len[1] = rwx_len[2] = 0;
    poll_set_poll_until_ready_or_timeout(&poll_set, rwx_len, timeout);

    // one or more objects are ready, or we had a timeout
    mp_obj_t list_array[3];
    list_array[0] = mp_obj_new_list(rwx_len[0], NULL);
    list_array[1] = mp_obj_new_list(rwx_len[1], NULL);
    list_array[2] = mp_obj_new_list(rwx_len[2], NULL);
    rwx_len[0] = rwx_len[1] = rwx_len[2] = 0;
    for (mp_uint_t i = 0; i < poll_set.map.alloc; ++i) {
        if (!mp_map_slot_is_filled(&poll_set.map, i)) {
            continue;
        }
        poll_obj_t *poll_obj = MP_OBJ_TO_PTR(poll_set.map.table[i].value);
        if (poll_obj->revents & MP_STREAM_POLL_RD) {
            ((mp_obj_list_t *)MP_OBJ_TO_PTR(list_array[0]))->items[rwx_len[0]++] = poll_obj->obj;
        }
        if (poll_obj->revents & MP_STREAM_POLL_WR) {
            ((mp_obj_list_t *)MP_OBJ_TO_PTR(list_array[1]))->items[rwx_len[1]++] = poll_obj->obj;
        }
        if ((poll_obj->revents & ~(MP_STREAM_POLL_RD | MP_STREAM_POLL_WR)) != 0) {
            ((mp_obj_list_t *)MP_OBJ_TO_PTR(list_array[2]))->items[rwx_len[2]++] = poll_obj->obj;
        }
    }
    poll_set_deinit(&poll_set);
    return mp_obj_new_tuple(3, list_array);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_select_select_obj, 3, 4, select_select);
#endif // MICROPY_PY_SELECT_SELECT

typedef struct _mp_obj_poll_t {
    mp_obj_base_t base;
    poll_set_t poll_set;
    short iter_cnt;
    short iter_idx;
    int flags;
    // callee-owned tuple
    mp_obj_t ret_tuple;
} mp_obj_poll_t;

// register(obj[, eventmask])
static mp_obj_t poll_register(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t events;
    if (n_args == 3) {
        events = mp_obj_get_int(args[2]);
    } else {
        events = MP_STREAM_POLL_RD | MP_STREAM_POLL_WR;
    }
    poll_set_add_obj(&self->poll_set, &args[1], 1, events, false);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_register_obj, 2, 3, poll_register);

// unregister(obj)
static mp_obj_t poll_unregister(mp_obj_t self_in, mp_obj_t obj_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);
    mp_map_elem_t *elem = mp_map_lookup(&self->poll_set.map, mp_obj_id(obj_in), MP_MAP_LOOKUP_REMOVE_IF_FOUND);

    #if MICROPY_PY_SELECT_POSIX_OPTIMISATIONS
    if (elem != NULL) {
        poll_obj_t *poll_obj = (poll_obj_t *)MP_OBJ_TO_PTR(elem->value);
        if (poll_obj->pollfd != NULL) {
            poll_obj->pollfd->fd = -1;
            --self->poll_set.used;
        }
        elem->value = MP_OBJ_NULL;
    }
    #else
    (void)elem;
    #endif

    // TODO raise KeyError if obj didn't exist in map
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(poll_unregister_obj, poll_unregister);

// modify(obj, eventmask)
static mp_obj_t poll_modify(mp_obj_t self_in, mp_obj_t obj_in, mp_obj_t eventmask_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);
    mp_map_elem_t *elem = mp_map_lookup(&self->poll_set.map, mp_obj_id(obj_in), MP_MAP_LOOKUP);
    if (elem == NULL) {
        mp_raise_OSError(MP_ENOENT);
    }
    poll_obj_set_events((poll_obj_t *)MP_OBJ_TO_PTR(elem->value), mp_obj_get_int(eventmask_in));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(poll_modify_obj, poll_modify);

static mp_uint_t poll_poll_internal(uint n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);

    // work out timeout (its given already in ms)
    mp_uint_t timeout = -1;
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

    return poll_set_poll_until_ready_or_timeout(&self->poll_set, NULL, timeout);
}

static mp_obj_t poll_poll(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t n_ready = poll_poll_internal(n_args, args);

    // one or more objects are ready, or we had a timeout
    mp_obj_list_t *ret_list = MP_OBJ_TO_PTR(mp_obj_new_list(n_ready, NULL));
    n_ready = 0;
    for (mp_uint_t i = 0; i < self->poll_set.map.alloc; ++i) {
        if (!mp_map_slot_is_filled(&self->poll_set.map, i)) {
            continue;
        }
        poll_obj_t *poll_obj = MP_OBJ_TO_PTR(self->poll_set.map.table[i].value);
        if (poll_obj_get_revents(poll_obj) != 0) {
            mp_obj_t tuple[2] = {poll_obj->obj, MP_OBJ_NEW_SMALL_INT(poll_obj_get_revents(poll_obj))};
            ret_list->items[n_ready++] = mp_obj_new_tuple(2, tuple);
        }
    }
    return MP_OBJ_FROM_PTR(ret_list);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_poll_obj, 1, 2, poll_poll);

static mp_obj_t poll_ipoll(size_t n_args, const mp_obj_t *args) {
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

static mp_obj_t poll_iternext(mp_obj_t self_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->iter_cnt == 0) {
        return MP_OBJ_STOP_ITERATION;
    }

    self->iter_cnt--;

    for (mp_uint_t i = self->iter_idx; i < self->poll_set.map.alloc; ++i) {
        self->iter_idx++;
        if (!mp_map_slot_is_filled(&self->poll_set.map, i)) {
            continue;
        }
        poll_obj_t *poll_obj = MP_OBJ_TO_PTR(self->poll_set.map.table[i].value);
        if (poll_obj_get_revents(poll_obj) != 0) {
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(self->ret_tuple);
            t->items[0] = poll_obj->obj;
            t->items[1] = MP_OBJ_NEW_SMALL_INT(poll_obj_get_revents(poll_obj));
            if (self->flags & FLAG_ONESHOT) {
                // Don't poll next time, until new event mask will be set explicitly
                poll_obj_set_events(poll_obj, 0);
            }
            return MP_OBJ_FROM_PTR(t);
        }
    }

    assert(!"inconsistent number of poll active entries");
    self->iter_cnt = 0;
    return MP_OBJ_STOP_ITERATION;
}

static const mp_rom_map_elem_t poll_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&poll_register_obj) },
    { MP_ROM_QSTR(MP_QSTR_unregister), MP_ROM_PTR(&poll_unregister_obj) },
    { MP_ROM_QSTR(MP_QSTR_modify), MP_ROM_PTR(&poll_modify_obj) },
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&poll_poll_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipoll), MP_ROM_PTR(&poll_ipoll_obj) },
};
static MP_DEFINE_CONST_DICT(poll_locals_dict, poll_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_poll,
    MP_QSTR_poll,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, poll_iternext,
    locals_dict, &poll_locals_dict
    );

// poll()
static mp_obj_t select_poll(void) {
    mp_obj_poll_t *poll = mp_obj_malloc(mp_obj_poll_t, &mp_type_poll);
    poll_set_init(&poll->poll_set, 0);
    poll->iter_cnt = 0;
    poll->ret_tuple = MP_OBJ_NULL;
    return MP_OBJ_FROM_PTR(poll);
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_select_poll_obj, select_poll);

static const mp_rom_map_elem_t mp_module_select_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_select) },
    #if MICROPY_PY_SELECT_SELECT
    { MP_ROM_QSTR(MP_QSTR_select), MP_ROM_PTR(&mp_select_select_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&mp_select_poll_obj) },
    { MP_ROM_QSTR(MP_QSTR_POLLIN), MP_ROM_INT(MP_STREAM_POLL_RD) },
    { MP_ROM_QSTR(MP_QSTR_POLLOUT), MP_ROM_INT(MP_STREAM_POLL_WR) },
    { MP_ROM_QSTR(MP_QSTR_POLLERR), MP_ROM_INT(MP_STREAM_POLL_ERR) },
    { MP_ROM_QSTR(MP_QSTR_POLLHUP), MP_ROM_INT(MP_STREAM_POLL_HUP) },
};

static MP_DEFINE_CONST_DICT(mp_module_select_globals, mp_module_select_globals_table);

const mp_obj_module_t mp_module_select = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_select_globals,
};

MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_select, mp_module_select);

#endif // MICROPY_PY_SELECT
