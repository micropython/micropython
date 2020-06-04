/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014, 2015 Damien P. George
 * Copyright (c) 2016 Linaro Limited
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
#include <stdint.h>
#include <string.h>

#include <zephyr.h>
#include <drivers/gpio.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "modmachine.h"

typedef struct _machine_pin_irq_obj_t {
    machine_pin_obj_t *pin;
    struct _machine_pin_irq_obj_t *next;
    struct gpio_callback gpio_callback;
    uint32_t timestamp_us;
    atomic_t event_count;
    mp_stream_event_t stream_event;
} machine_pin_irq_obj_t;

const mp_obj_base_t machine_pin_obj_template = {&machine_pin_type};

void machine_pin_deinit(void) {
    for (machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_list); irq != NULL; irq = irq->next) {
        machine_pin_obj_t *pin = MP_OBJ_TO_PTR(irq->pin);
        gpio_pin_interrupt_configure(pin->port, pin->pin, GPIO_INT_DISABLE);
        gpio_remove_callback(pin->port, &irq->gpio_callback);
    }
    MP_STATE_PORT(machine_pin_irq_list) = NULL;
}

STATIC void gpio_callback_handler(struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins) {
    machine_pin_irq_obj_t *irq = (void *)((uintptr_t)cb - offsetof(machine_pin_irq_obj_t, gpio_callback));
    if (atomic_inc(&irq->event_count) == 0) {
        irq->timestamp_us = mp_hal_ticks_us();
    }
    if (irq->stream_event.callback != NULL) {
        irq->stream_event.callback(irq->stream_event.arg, 1);
    }
}

STATIC void machine_pin_init_irq(machine_pin_obj_t *self) {
    if (self->irq != NULL) {
        return;
    }

    machine_pin_irq_obj_t *irq;
    for (irq = MP_STATE_PORT(machine_pin_irq_list); irq != NULL; irq = irq->next) {
        machine_pin_obj_t *irq_pin = MP_OBJ_TO_PTR(irq->pin);
        if (irq_pin->port == self->port && irq_pin->pin == self->pin) {
            break;
        }
    }

    if (irq == NULL) {
        irq = m_new_obj(machine_pin_irq_obj_t);
        irq->next = MP_STATE_PORT(machine_pin_irq_list);
        gpio_init_callback(&irq->gpio_callback, gpio_callback_handler, 0);
        int ret = gpio_add_callback(self->port, &irq->gpio_callback);
        if (ret != 0) {
            mp_raise_OSError(-ret);
        }
        MP_STATE_PORT(machine_pin_irq_list) = irq;
    }

    self->irq = irq;
}

STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    mp_printf(print, "<Pin %p %d>", self->port, self->pin);
}

// pin.init(mode, pull=None, *, value)
STATIC mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get io mode
    uint mode = args[ARG_mode].u_int;

    // get pull mode
    uint pull = 0;
    if (args[ARG_pull].u_obj != mp_const_none) {
        pull = mp_obj_get_int(args[ARG_pull].u_obj);
    }

    // get initial value
    uint init = 0;
    if (args[ARG_value].u_obj != MP_OBJ_NULL) {
        init = mp_obj_is_true(args[ARG_value].u_obj) ? GPIO_OUTPUT_INIT_HIGH : GPIO_OUTPUT_INIT_LOW;
    }

    int ret = gpio_pin_configure(self->port, self->pin, mode | pull | init);
    if (ret) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }

    return mp_const_none;
}

// constructor(drv_name, pin, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get the wanted port
    if (!mp_obj_is_type(args[0], &mp_type_tuple)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Pin id must be tuple of (\"GPIO_x\", pin#)"));
    }
    mp_obj_t *items;
    mp_obj_get_array_fixed_n(args[0], 2, &items);
    const char *drv_name = mp_obj_str_get_str(items[0]);
    int wanted_pin = mp_obj_get_int(items[1]);
    struct device *wanted_port = device_get_binding(drv_name);
    if (!wanted_port) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid port"));
    }

    machine_pin_obj_t *pin = m_new_obj(machine_pin_obj_t);
    pin->base = machine_pin_obj_template;
    pin->port = wanted_port;
    pin->pin = wanted_pin;
    pin->irq = NULL;

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

// fast method for getting/setting pin value
STATIC mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;
    if (n_args == 0) {
        int pin_val = gpio_pin_get_raw(self->port, self->pin);
        return MP_OBJ_NEW_SMALL_INT(pin_val);
    } else {
        (void)gpio_pin_set_raw(self->port, self->pin, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

// pin.init(mode, pull)
STATIC mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

// pin.value([value])
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

STATIC mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    (void)gpio_pin_set_raw(self->port, self->pin, 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    (void)gpio_pin_set_raw(self->port, self->pin, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

STATIC mp_obj_t machine_pin_set_event(mp_obj_t self_in, mp_obj_t mode_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t mode = mp_obj_get_int_truncated(mode_in);
    machine_pin_init_irq(self);
    self->irq->gpio_callback.pin_mask &= ~BIT(self->pin);
    int ret;
    if (mode == 0) {
        ret = gpio_pin_interrupt_configure(self->port, self->pin, GPIO_INT_DISABLE);
    } else {
        ret = gpio_pin_interrupt_configure(self->port, self->pin, mode);
        self->irq->gpio_callback.pin_mask |= BIT(self->pin);
    }
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_pin_set_event_obj, machine_pin_set_event);

STATIC mp_obj_t machine_pin_get_event(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->irq == NULL) {
        mp_raise_OSError(MP_EIO);
    }
    unsigned int key = irq_lock();
    atomic_t event_count = atomic_clear(&self->irq->event_count);
    mp_obj_t args[2] = { MP_OBJ_NEW_SMALL_INT(self->irq->timestamp_us), MP_OBJ_NEW_SMALL_INT(event_count) };
    irq_unlock(key);
    return mp_obj_new_tuple(2, args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_get_event_obj, machine_pin_get_event);

STATIC mp_uint_t machine_pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    switch (request) {
        case MP_STREAM_SET_EVENT: {
            mp_stream_event_t *stream_event = (mp_stream_event_t *)arg;
            machine_pin_init_irq(self);
            self->irq->stream_event = *stream_event;
            mp_uint_t ret = 0;
            if (atomic_get(&self->irq->event_count) != 0) {
                ret |= 1;
            }
            return ret;
        }
        case MP_PIN_READ: {
            return gpio_pin_get_raw(self->port, self->pin);
        }
        case MP_PIN_WRITE: {
            return gpio_pin_set_raw(self->port, self->pin, arg);
        }
    }
    return -1;
}

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&machine_pin_on_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_event), MP_ROM_PTR(&machine_pin_set_event_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_event), MP_ROM_PTR(&machine_pin_get_event_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN),        MP_ROM_INT(GPIO_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT),       MP_ROM_INT(GPIO_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP),   MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(GPIO_INT_EDGE_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(GPIO_INT_EDGE_FALLING) },
};

STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC const mp_pin_p_t machine_pin_pin_p = {
    .ioctl = machine_pin_ioctl,
};

const mp_obj_type_t machine_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = machine_pin_print,
    .make_new = mp_pin_make_new,
    .call = machine_pin_call,
    .protocol = &machine_pin_pin_p,
    .locals_dict = (mp_obj_t)&machine_pin_locals_dict,
};
