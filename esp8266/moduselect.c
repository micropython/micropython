/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include "py/obj.h"
#include "py/nlr.h"
#include "py/mphal.h"
#include "py/objlist.h"

extern const mp_obj_type_t lwip_socket_type;
// FIXME: move to an LWIP header
typedef struct _lwip_socket_obj_t lwip_socket_obj_t;
bool lwip_has_incoming(lwip_socket_obj_t *);
bool lwip_is_closed(lwip_socket_obj_t *);


typedef enum
_uselect_poll_flags {
    POLL_IN   = 1 << 0,
    POLL_OUT  = 1 << 1,
    POLL_ERR    = 1 << 2,
    POLL_HUP    = 1 << 3,
} uselect_poll_flags;


/* \class Poll - poll class */
typedef struct
_uselect_poll_t {
    mp_obj_base_t base;
    mp_map_t poll_map;
} uselect_poll_t;


typedef struct
_poll_obj_t {
    mp_obj_t obj;
    uselect_poll_flags flags;
} poll_obj_t;


/* \method register(obj[, eventmask]) */
STATIC mp_obj_t
poll_register(uint n_args, const mp_obj_t *args) {
    uselect_poll_t *self = args[0];
    mp_obj_t *obj = args[1];
    mp_uint_t flags;
    mp_map_elem_t *elem;

    if (n_args == 3) {
        flags = mp_obj_get_int(args[2]);
    } else {
        flags = POLL_IN | POLL_OUT;
    }

    elem = mp_map_lookup(&self->poll_map,
                         mp_obj_id(obj),
                         MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);

    if (elem->value == NULL) {
        /* Check the type of the object.
         * Currently we only support lwip sockets */
        if (!mp_obj_is_subclass_fast(mp_obj_get_type(obj),
                                     &lwip_socket_type)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError,
                        "Only socket supported"));
        }

        /* Create a new map entry */
        poll_obj_t *poll_obj = elem->value = m_new_obj(poll_obj_t);
        poll_obj->obj = obj;
    }

    ((poll_obj_t *) elem->value)->flags = flags;

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_register_obj, 2, 3, poll_register);


/* \method unregister(obj) */
STATIC mp_obj_t
poll_unregister(mp_obj_t self_in, mp_obj_t obj_in) {
    uselect_poll_t *self = self_in;
    mp_map_lookup(&self->poll_map,
                  mp_obj_id(obj_in),
                  MP_MAP_LOOKUP_REMOVE_IF_FOUND);

    // FIXME raise KeyError if obj didn't exist in map
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(poll_unregister_obj, poll_unregister);


/* \method poll([timeout])
 *
 * Timeout is in milliseconds.
 */
STATIC mp_obj_t
poll_poll(uint n_args, const mp_obj_t *args) {
    uselect_poll_t *self = args[0];
    mp_uint_t timeout = -1;

    /* Get the timeout */
    if (n_args >= 2 && args[1] != mp_const_none) {
        mp_int_t timeout_i = mp_obj_get_int(args[1]);

        if (timeout_i >= 0) {
            timeout = timeout_i;
        }
    }

    mp_obj_list_t *ret_list = mp_obj_new_list(0, NULL);
    mp_uint_t start_tick = mp_hal_ticks_ms();

    while (timeout == -1 || mp_hal_ticks_ms() <= start_tick + timeout) {
        /* Check all the objects */
        for (mp_uint_t i = 0; i < self->poll_map.alloc; ++i) {
            if (!MP_MAP_SLOT_IS_FILLED(&self->poll_map, i)) {
                continue;
            }

            poll_obj_t *elem = (poll_obj_t *) self->poll_map.table[i].value;
            mp_obj_t type = mp_obj_get_type(elem->obj);
            uselect_poll_flags flags = 0; /* Output  flags */

            if (mp_obj_is_subclass_fast(type, &lwip_socket_type)) {
                /* Object is a socket */
                lwip_socket_obj_t *socket = elem->obj;

                if (elem->flags & POLL_IN && lwip_has_incoming(socket)) {
                    flags |= POLL_IN;
                }

                if (elem->flags & POLL_HUP && lwip_is_closed(socket)) {
                    flags |= POLL_HUP;
                }

            } else {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError,
                            "Only socket supported"));
            }

            /* Append it to the list */
            if (flags) {
                mp_obj_t tuple[] = {elem->obj, MP_OBJ_NEW_SMALL_INT(flags)};
                mp_obj_list_append(ret_list, mp_obj_new_tuple(2, tuple));
            }
        }

        /* All sockets checked */
        if (ret_list->len > 0) {
            break;
        } else {
            /* Gate the clock until something happens */
            MICROPY_EVENT_POLL_HOOK;
        }
    }

    return ret_list;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(poll_poll_obj, 1, 3, poll_poll);


STATIC const mp_map_elem_t
poll_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_register), (mp_obj_t) &poll_register_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unregister), (mp_obj_t) &poll_unregister_obj },
    /* Modify is an alias for register, they're basically the same */
    { MP_OBJ_NEW_QSTR(MP_QSTR_modify), (mp_obj_t) &poll_register_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_poll), (mp_obj_t) &poll_poll_obj },
};

STATIC MP_DEFINE_CONST_DICT(poll_locals_dict, poll_locals_dict_table);


STATIC const
mp_obj_type_t uselect_type_poll = {
    { &mp_type_type },
    .name = MP_QSTR_poll,
    .locals_dict = (mp_obj_t) &poll_locals_dict,
};


/* \function poll() */
STATIC mp_obj_t
uselect_poll(void) {
    uselect_poll_t *self = m_new_obj(uselect_poll_t);

    self->base.type = &uselect_type_poll;
    mp_map_init(&self->poll_map, 0);

    return self;
}

MP_DEFINE_CONST_FUN_OBJ_0(uselect_poll_obj, uselect_poll);


/* Module-level attributes */
STATIC const mp_map_elem_t
uselect_globals_table[] = {
    /* Module */
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_uselect) },
    /* Functions */
    // FIXME: placeholder
    { MP_OBJ_NEW_QSTR(MP_QSTR_select), MP_OBJ_NEW_QSTR(MP_QSTR_select) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_poll), (mp_obj_t) &uselect_poll_obj },
    /* Constants */
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLIN), MP_OBJ_NEW_SMALL_INT(POLL_IN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLOUT), MP_OBJ_NEW_SMALL_INT(POLL_OUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLERR), MP_OBJ_NEW_SMALL_INT(POLL_ERR) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_POLLHUP), MP_OBJ_NEW_SMALL_INT(POLL_HUP) },
};

STATIC MP_DEFINE_CONST_DICT(uselect_globals, uselect_globals_table);


const mp_obj_module_t
uselect_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uselect,
    .globals = (mp_obj_dict_t*) &uselect_globals,
};
