/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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

#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "py/obj.h"
#include "py/runtime.h"

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;

    mp_uint_t index;

    mp_uint_t repeat;

    uint64_t period;

    mp_obj_t callback_mpy;

    HANDLE h_tmr;   // Timer attached to Q.T. timer handler

    void *arg;

    struct _machine_timer_obj_t *next;
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;

// global timer queue handler
static HANDLE _h_que_tmr = INVALID_HANDLE_VALUE;

STATIC void machine_timer_disable(machine_timer_obj_t *self);

void machine_timer_deinit_all(void) {
    // Disable, deallocate and remove all timers from list
    machine_timer_obj_t **t = &MP_STATE_PORT(machine_timer_obj_head);
    while (*t != NULL) {
        machine_timer_disable(*t);
        machine_timer_obj_t *next = (*t)->next;
        m_del_obj(machine_timer_obj_t, *t);
        *t = next;
    }
}

STATIC void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = self_in;

    mp_printf(print, "Timer(%p; ", self);
}

STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_uint_t index = mp_obj_get_int(args[0]) & 3; // four timers

    // Check whether the timer is already initialized, if so return it
    for (machine_timer_obj_t *t = MP_STATE_PORT(machine_timer_obj_head); t; t = t->next) {
        if (t->index == index) {
            return t;
        }
    }

    machine_timer_obj_t *self = m_new_obj(machine_timer_obj_t);
    self->base.type = &machine_timer_type;
    self->index = index;
    self->h_tmr = INVALID_HANDLE_VALUE;

    // Add the timer to the linked-list of timers
    self->next = MP_STATE_PORT(machine_timer_obj_head);
    MP_STATE_PORT(machine_timer_obj_head) = self;

    return self;
}

STATIC void machine_timer_disable(machine_timer_obj_t *self) {

    if (self->h_tmr != INVALID_HANDLE_VALUE) {
        DeleteTimerQueueTimer(NULL, self->h_tmr, NULL);
    }
}

void NTAPI machine_timer_callback_os(void *self_in, BOOLEAN timerCalled) {
    machine_timer_obj_t *self = (machine_timer_obj_t *)self_in;
    mp_sched_schedule(self->callback_mpy, self);
}

STATIC void machine_timer_enable(machine_timer_obj_t *self) {
    enum { ONE_SHOT, PERIODIC };
    BOOL bResult;

    if (_h_que_tmr == INVALID_HANDLE_VALUE) {
        mp_raise_OSError(errno);
        return;
    }

    if (self->repeat == ONE_SHOT) {
        bResult = CreateTimerQueueTimer(&self->h_tmr, _h_que_tmr, machine_timer_callback_os,
            self, self->period, 0, WT_EXECUTEDEFAULT);
    } else {
        bResult = CreateTimerQueueTimer(&self->h_tmr, _h_que_tmr, machine_timer_callback_os,
            self, 0, self->period, WT_EXECUTEDEFAULT);
    }

    if (!bResult) {
        mp_raise_OSError(errno);
        return;
    }

    self->arg = self;
}

STATIC mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_callback, ARG_period };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
    };

    machine_timer_disable(self);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->period = (uint64_t)args[ARG_period].u_int;
    self->repeat = args[ARG_mode].u_int;
    self->callback_mpy = args[ARG_callback].u_obj;

    machine_timer_enable(self);

    return mp_const_none;
}

STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_disable(self_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

STATIC mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {

    if (_h_que_tmr == INVALID_HANDLE_VALUE) {
        _h_que_tmr = CreateTimerQueue();
        if (_h_que_tmr == INVALID_HANDLE_VALUE) {
            mp_raise_OSError(errno);
        }
    }

    return machine_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

STATIC mp_obj_t machine_timer_value(mp_obj_t self_in) {

    return MP_OBJ_NEW_SMALL_INT((mp_uint_t)(0));  // value in ms
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_value_obj, machine_timer_value);

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_timer_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(false) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(true) },
};
STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

const mp_obj_type_t machine_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = machine_timer_print,
    .make_new = machine_timer_make_new,
    .locals_dict = (mp_obj_t)&machine_timer_locals_dict,
};

/*
package machine

class Timer

    PERIODIC = 0
    ONE_SHOT = 1

    def __init__(self, id, ...)
      """
      Construct a new timer object of the given id. id of -1 constructs a virtual timer (if supported by a board).
      id shall not be passed as a keyword argument.

      See init for parameters of initialization.
      '''

    def __init__(self, mode=Timer.PERIODIC, period=-1, callback=None):
        """
        Initialize the timer. Keyword arguments:

        mode - can be one of:
          Timer.ONE_SHOT - The timer runs once until the configured period of the channel expires.
          Timer.PERIODIC - The timer runs periodically at the configured frequency of the channel.

        period - The timer period, in milliseconds.

        callback - The callable to call upon expiration of the timer period. The callback must take
          one argument, which is passed the Timer object. The callback argument shall be specified.
          Otherwise an exception will occur upon timer expiration: TypeError: 'NoneType' object isn't callable
        """

    def deinit():
        """ De-initialize the timer. Stops the timer, and disables the timer peripheral. """

*/
