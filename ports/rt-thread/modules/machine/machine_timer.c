/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 ChenYong (chenyong@rt-thread.com)
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
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"

#ifdef MICROPYTHON_USING_MACHINE_TIMER

#include <rtthread.h>
#include <drivers/hwtimer.h>
#include "machine_timer.h"

#define  MAX_TIMER  17

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    rt_device_t timer_device;
    char dev_name[RT_NAME_MAX];
    mp_obj_t timeout_cb;
    int8_t timerid;
    uint32_t timeout;
    rt_bool_t is_repeat;
    rt_bool_t is_init;
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;

STATIC void error_check(bool status, const char *msg) {
    if (!status) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, msg));
    }
}

STATIC void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = self_in;

    mp_printf(print, "Timer(%p; ", self);

    if (self->timerid >= 0) {
        mp_printf(print, "timer_id=%d, ", self->timerid);
    } else {
        mp_printf(print, "timer_name=%s, ", self->dev_name);
    }
    mp_printf(print, "period=%d, ", self->timeout);
    mp_printf(print, "auto_reload=%d)", self->is_repeat);
}

STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    machine_timer_obj_t *self = m_new_obj(machine_timer_obj_t);
    char timer_dev_name[RT_NAME_MAX] = {0};

    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, true);

    // check input timer device name or ID
    if (mp_obj_is_small_int(args[0])) {
        int device_id = mp_obj_get_int(args[0]);
        self->timerid = device_id;
        self->timer_device->device_id = device_id;
        rt_snprintf(timer_dev_name, sizeof(timer_dev_name), "timer%d", mp_obj_get_int(args[0]));
    } else if (mp_obj_is_qstr(args[0])) {
        static int device_id = 0;
        self->timerid = -1;
        self->timer_device->device_id = device_id++;
        rt_strncpy(self->dev_name, mp_obj_str_get_str(args[0]), RT_NAME_MAX);
        rt_strncpy(timer_dev_name, self->dev_name, RT_NAME_MAX);
    } else {
        error_check(0, "Input ADC device name or ID error.");
    }

    // find timer device
    self->timer_device = rt_device_find(timer_dev_name);
    if (self->timer_device == RT_NULL || self->timer_device->type != RT_Device_Class_Timer) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Timer(%s) don't exist", timer_dev_name));
    }

    // initialize timer device
    self->base.type = &machine_timer_type;
    self->timeout = 0;
    self->timeout_cb = RT_NULL;
    self->is_repeat = RT_TRUE;
    self->is_init = RT_FALSE;

    // return constant object
    return MP_OBJ_FROM_PTR(self);
}

static machine_timer_obj_t *timer_self[MAX_TIMER] = {RT_NULL};

STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;
    rt_err_t result = RT_EOK;

    if (self->is_init == RT_TRUE) {
        result = rt_device_close(self->timer_device);
        error_check(result == RT_EOK, "Timer device close error");
        self->is_init = RT_FALSE;
        timer_self[self->timer_device->device_id] = RT_NULL;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

STATIC rt_err_t timer_event_handler(rt_device_t dev, rt_size_t size) {
    machine_timer_obj_t *self = timer_self[dev->device_id];

    mp_sched_schedule(self->timeout_cb, MP_OBJ_FROM_PTR(self));
    return RT_EOK;
}

STATIC mp_obj_t machine_timer_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_obj_t *self = (machine_timer_obj_t *)args[0];
    rt_bool_t result = RT_EOK;
    int mode = 0;

    enum {
        ARG_mode,
        ARG_period,
        ARG_callback,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_period,       MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_callback,     MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t dargs[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, dargs);

    if (2 == n_args) {
        self->timeout = dargs[0].u_int;
    } else if (3 == n_args) {
        self->is_repeat = dargs[ARG_mode].u_int;
        self->timeout = dargs[ARG_period].u_int;
    } else if (4 == n_args) {
        self->is_repeat = dargs[ARG_mode].u_int;
        self->timeout = dargs[ARG_period].u_int;
        self->timeout_cb = dargs[ARG_callback].u_obj;
    } else {
        mp_raise_ValueError("invalid format");
    }

    error_check(self->timeout > 0, "Set timeout value error");

    if (self->is_init == RT_FALSE)
    {
        // open timer device
        result = rt_device_open(self->timer_device, RT_DEVICE_OFLAG_RDWR);
        error_check(result == RT_EOK, "Timer device open error");
    }

    if (self->timeout_cb != RT_NULL) {
        // set callback timer
        if (timer_self[self->timer_device->device_id] && timer_self[self->timer_device->device_id] != self) {
            error_check(result == RT_EOK, "Timer device callback function already exists");
        } else {
            timer_self[self->timer_device->device_id] = self;
        }
        result = rt_device_set_rx_indicate(self->timer_device, timer_event_handler);
        error_check(result == RT_EOK, "Timer set timout callback error");
    }

    // set timer mode
    mode = self->is_repeat ? HWTIMER_MODE_PERIOD : HWTIMER_MODE_ONESHOT;
    result = rt_device_control(self->timer_device, HWTIMER_CTRL_MODE_SET, &mode);
    error_check(result == RT_EOK, "Timer set mode error");

    if (self->timeout) {
        rt_hwtimerval_t timeout_s;
        rt_size_t len;

        timeout_s.sec = self->timeout / 1000;      // second
        timeout_s.usec = self->timeout % 1000;     // microsecond

        len = rt_device_write(self->timer_device, 0, &timeout_s, sizeof(timeout_s));
        error_check(len == sizeof(timeout_s), "Timer set timout error");
    }

    self->is_init = RT_TRUE;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);


STATIC mp_obj_t machine_timer_callback(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_obj_t *self = (machine_timer_obj_t *)args[0];
    rt_bool_t result = RT_EOK;

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_callback,     MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t dargs[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, dargs);

    self->timeout_cb = dargs[0].u_obj;

    if(n_args == 1)
    {
        self->timeout_cb = RT_NULL;
        self->timer_device->rx_indicate = RT_NULL;//Log-off callback function
    }
    else if(n_args == 2)
    {
        if(self->timeout_cb != mp_const_none)
        {
            timer_self[self->timer_device->device_id] = self;
            result = rt_device_set_rx_indicate(self->timer_device, timer_event_handler); //set callback timer
            error_check(result == RT_EOK, "Timer set timout callback error");
        }
        else
        {
            self->timeout_cb = RT_NULL;
            self->timer_device->rx_indicate = RT_NULL;//Log-off callback function
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_callback_obj, 0,machine_timer_callback);

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&machine_timer_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(RT_FALSE) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(RT_TRUE) },
};
STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

const mp_obj_type_t machine_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = machine_timer_print,
    .make_new = machine_timer_make_new,
    .locals_dict = (mp_obj_t) &machine_timer_locals_dict,
};

#endif // MICROPYTHON_USING_MACHINE_TIMER

