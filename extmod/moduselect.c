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
#if MICROPY_PY_USELECT

#include <stdio.h>

#include "py/runtime.h"
#include "py/obj.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"

// Flags for poll()
#define FLAG_ONESHOT (1)

typedef struct _poll_obj_t {
    mp_obj_t obj;
    mp_uint_t (*ioctl)(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode);
    mp_uint_t flags;
    mp_uint_t flags_ret;
} poll_obj_t;

STATIC void poll_map_add(mp_map_t *poll_map, const mp_obj_t *obj, mp_uint_t obj_len, mp_uint_t flags, bool or_flags) {
    for (mp_uint_t i = 0; i < obj_len; i++) {
        mp_map_elem_t *elem = mp_map_lookup(poll_map, mp_obj_id(obj[i]), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
        if (elem->value == MP_OBJ_NULL) {
            // object not found; get its ioctl and add it to the poll list
            const mp_stream_p_t *stream_p = mp_get_stream_raise(obj[i], MP_STREAM_OP_IOCTL);
            poll_obj_t *poll_obj = m_new_obj(poll_obj_t);
            poll_obj->obj = obj[i];
            poll_obj->ioctl = stream_p->ioctl;
            poll_obj->flags = flags;
            poll_obj->flags_ret = 0;
            elem->value = MP_OBJ_FROM_PTR(poll_obj);
        } else {
            // object exists; update its flags
            if (or_flags) {
                ((poll_obj_t *)MP_OBJ_TO_PTR(elem->value))->flags |= flags;
            } else {
                ((poll_obj_t *)MP_OBJ_TO_PTR(elem->value))->flags = flags;
            }
        }
    }
}

// poll each object in the map
STATIC mp_uint_t poll_map_poll(mp_map_t *poll_map, size_t *rwx_num) {
    mp_uint_t n_ready = 0;
    for (mp_uint_t i = 0; i < poll_map->alloc; ++i) {
        if (!mp_map_slot_is_filled(poll_map, i)) {
            continue;
        }

        poll_obj_t *poll_obj = MP_OBJ_TO_PTR(poll_map->table[i].value);
        int errcode;
        mp_int_t ret = poll_obj->ioctl(poll_obj->obj, MP_STREAM_POLL, poll_obj->flags, &errcode);
        poll_obj->flags_ret = ret;

        if (ret == -1) {
            // error doing ioctl
            mp_raise_OSError(errcode);
        }

        if (ret != 0) {
            // object is ready
            n_ready += 1;
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
        }
    }
    return n_ready;
}

#if MICROPY_PY_USELECT_SELECT
// select(rlist, wlist, xlist[, timeout])
STATIC mp_obj_t select_select(size_t n_args, const mp_obj_t *args) {
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
    mp_map_t poll_map;
    mp_map_init(&poll_map, rwx_len[0] + rwx_len[1] + rwx_len[2]);
    poll_map_add(&poll_map, r_array, rwx_len[0], MP_STREAM_POLL_RD, true);
    poll_map_add(&poll_map, w_array, rwx_len[1], MP_STREAM_POLL_WR, true);
    poll_map_add(&poll_map, x_array, rwx_len[2], MP_STREAM_POLL_ERR | MP_STREAM_POLL_HUP, true);

    mp_uint_t start_tick = mp_hal_ticks_ms();
    rwx_len[0] = rwx_len[1] = rwx_len[2] = 0;
    for (;;) {
        // poll the objects
        mp_uint_t n_ready = poll_map_poll(&poll_map, rwx_len);

        if (n_ready > 0 || (timeout != (mp_uint_t)-1 && mp_hal_ticks_ms() - start_tick >= timeout)) {
            // one or more objects are ready, or we had a timeout
            mp_obj_t list_array[3];
            list_array[0] = mp_obj_new_list(rwx_len[0], NULL);
            list_array[1] = mp_obj_new_list(rwx_len[1], NULL);
            list_array[2] = mp_obj_new_list(rwx_len[2], NULL);
            rwx_len[0] = rwx_len[1] = rwx_len[2] = 0;
            for (mp_uint_t i = 0; i < poll_map.alloc; ++i) {
                if (!mp_map_slot_is_filled(&poll_map, i)) {
                    continue;
                }
                poll_obj_t *poll_obj = MP_OBJ_TO_PTR(poll_map.table[i].value);
                if (poll_obj->flags_ret & MP_STREAM_POLL_RD) {
                    ((mp_obj_list_t *)MP_OBJ_TO_PTR(list_array[0]))->items[rwx_len[0]++] = poll_obj->obj;
                }
                if (poll_obj->flags_ret & MP_STREAM_POLL_WR) {
                    ((mp_obj_list_t *)MP_OBJ_TO_PTR(list_array[1]))->items[rwx_len[1]++] = poll_obj->obj;
                }
                if ((poll_obj->flags_ret & ~(MP_STREAM_POLL_RD | MP_STREAM_POLL_WR)) != 0) {
                    ((mp_obj_list_t *)MP_OBJ_TO_PTR(list_array[2]))->items[rwx_len[2]++] = poll_obj->obj;
                }
            }
            mp_map_deinit(&poll_map);
            return mp_obj_new_tuple(3, list_array);
        }
        MICROPY_EVENT_POLL_HOOK
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_select_select_obj, 3, 4, select_select);
#endif // MICROPY_PY_USELECT_SELECT

typedef struct _mp_obj_poll_t {
    mp_obj_base_t base;
    mp_map_t poll_map;
    short iter_cnt;
    short iter_idx;
    int flags;
    // callee-owned tuple
    mp_obj_t ret_tuple;
} mp_obj_poll_t;

// register(obj[, eventmask])
STATIC mp_obj_t poll_register(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t flags;
    if (n_args == 3) {
        flags = mp_obj_get_int(args[2]);
    } else {
        flags = MP_STREAM_POLL_RD | MP_STREAM_POLL_WR;
    }
    poll_map_add(&self->poll_map, &args[1], 1, flags, false);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_register_obj, 2, 3, poll_register);

// unregister(obj)
STATIC mp_obj_t poll_unregister(mp_obj_t self_in, mp_obj_t obj_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);
    mp_map_lookup(&self->poll_map, mp_obj_id(obj_in), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
    // TODO raise KeyError if obj didn't exist in map
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(poll_unregister_obj, poll_unregister);

// modify(obj, eventmask)
STATIC mp_obj_t poll_modify(mp_obj_t self_in, mp_obj_t obj_in, mp_obj_t eventmask_in) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(self_in);
    mp_map_elem_t *elem = mp_map_lookup(&self->poll_map, mp_obj_id(obj_in), MP_MAP_LOOKUP);
    if (elem == NULL) {
        mp_raise_OSError(MP_ENOENT);
    }
    ((poll_obj_t *)MP_OBJ_TO_PTR(elem->value))->flags = mp_obj_get_int(eventmask_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(poll_modify_obj, poll_modify);

STATIC mp_uint_t poll_poll_internal(uint n_args, const mp_obj_t *args) {
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

    mp_uint_t start_tick = mp_hal_ticks_ms();
    mp_uint_t n_ready;
    for (;;) {
        // poll the objects
        n_ready = poll_map_poll(&self->poll_map, NULL);
        if (n_ready > 0 || (timeout != (mp_uint_t)-1 && mp_hal_ticks_ms() - start_tick >= timeout)) {
            break;
        }
        MICROPY_EVENT_POLL_HOOK
    }

    return n_ready;
}

STATIC mp_obj_t poll_poll(size_t n_args, const mp_obj_t *args) {
    mp_obj_poll_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_uint_t n_ready = poll_poll_internal(n_args, args);

    // one or more objects are ready, or we had a timeout
    mp_obj_list_t *ret_list = MP_OBJ_TO_PTR(mp_obj_new_list(n_ready, NULL));
    n_ready = 0;
    for (mp_uint_t i = 0; i < self->poll_map.alloc; ++i) {
        if (!mp_map_slot_is_filled(&self->poll_map, i)) {
            continue;
        }
        poll_obj_t *poll_obj = MP_OBJ_TO_PTR(self->poll_map.table[i].value);
        if (poll_obj->flags_ret != 0) {
            mp_obj_t tuple[2] = {poll_obj->obj, MP_OBJ_NEW_SMALL_INT(poll_obj->flags_ret)};
            ret_list->items[n_ready++] = mp_obj_new_tuple(2, tuple);
            if (self->flags & FLAG_ONESHOT) {
                // Don't poll next time, until new event flags will be set explicitly
                poll_obj->flags = 0;
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

    for (mp_uint_t i = self->iter_idx; i < self->poll_map.alloc; ++i) {
        self->iter_idx++;
        if (!mp_map_slot_is_filled(&self->poll_map, i)) {
            continue;
        }
        poll_obj_t *poll_obj = MP_OBJ_TO_PTR(self->poll_map.table[i].value);
        if (poll_obj->flags_ret != 0) {
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(self->ret_tuple);
            t->items[0] = poll_obj->obj;
            t->items[1] = MP_OBJ_NEW_SMALL_INT(poll_obj->flags_ret);
            if (self->flags & FLAG_ONESHOT) {
                // Don't poll next time, until new event flags will be set explicitly
                poll_obj->flags = 0;
            }
            return MP_OBJ_FROM_PTR(t);
        }
    }

    assert(!"inconsistent number of poll active entries");
    self->iter_cnt = 0;
    return MP_OBJ_STOP_ITERATION;
}

STATIC const mp_rom_map_elem_t poll_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&poll_register_obj) },
    { MP_ROM_QSTR(MP_QSTR_unregister), MP_ROM_PTR(&poll_unregister_obj) },
    { MP_ROM_QSTR(MP_QSTR_modify), MP_ROM_PTR(&poll_modify_obj) },
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&poll_poll_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipoll), MP_ROM_PTR(&poll_ipoll_obj) },
};
STATIC MP_DEFINE_CONST_DICT(poll_locals_dict, poll_locals_dict_table);

STATIC const mp_obj_type_t mp_type_poll = {
    { &mp_type_type },
    .name = MP_QSTR_poll,
    .getiter = mp_identity_getiter,
    .iternext = poll_iternext,
    .locals_dict = (void *)&poll_locals_dict,
};

// poll()
STATIC mp_obj_t select_poll(void) {
    mp_obj_poll_t *poll = mp_obj_malloc(mp_obj_poll_t, &mp_type_poll);
    mp_map_init(&poll->poll_map, 0);
    poll->iter_cnt = 0;
    poll->ret_tuple = MP_OBJ_NULL;
    return MP_OBJ_FROM_PTR(poll);
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_select_poll_obj, select_poll);

STATIC const mp_rom_map_elem_t mp_module_select_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uselect) },
    #if MICROPY_PY_USELECT_SELECT
    { MP_ROM_QSTR(MP_QSTR_select), MP_ROM_PTR(&mp_select_select_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&mp_select_poll_obj) },
    { MP_ROM_QSTR(MP_QSTR_POLLIN), MP_ROM_INT(MP_STREAM_POLL_RD) },
    { MP_ROM_QSTR(MP_QSTR_POLLOUT), MP_ROM_INT(MP_STREAM_POLL_WR) },
    { MP_ROM_QSTR(MP_QSTR_POLLERR), MP_ROM_INT(MP_STREAM_POLL_ERR) },
    { MP_ROM_QSTR(MP_QSTR_POLLHUP), MP_ROM_INT(MP_STREAM_POLL_HUP) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_select_globals, mp_module_select_globals_table);

const mp_obj_module_t mp_module_uselect = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_select_globals,
};

#endif // MICROPY_PY_USELECT
