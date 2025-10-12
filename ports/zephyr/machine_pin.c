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

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"
#include "shared/runtime/mpirq.h"
#include "modmachine.h"
#include "zephyr_device.h"

#if MICROPY_PY_MACHINE

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL

#include CONFIG_MICROPY_DYNAMIC_PINCTRL_HEADER_BUILDER

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
#include CONFIG_MICROPY_DYNAMIC_PINCTRL_HEADER
#include "generated_dynamic_pinctrl.h"
#define PINMUX_COUNT (ARRAY_SIZE(generated_dynamic_pinmuxes))
#endif

#endif

typedef struct _machine_pin_irq_obj_t {
    mp_irq_obj_t base;
    struct _machine_pin_irq_obj_t *next;
    struct gpio_callback callback;
} machine_pin_irq_obj_t;

static const mp_irq_methods_t machine_pin_irq_methods;
const mp_obj_base_t machine_pin_obj_template = {&machine_pin_type};

static inline void machine_pin_check_pinctrl(machine_pin_obj_t *self) {
    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL
    if (self->is_pinctrl) {
        mp_raise_ValueError(MP_ERROR_TEXT("This is a pinctrl only Pin"));
    }
    #endif
}

void machine_pin_deinit(void) {
    for (machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_list); irq != NULL; irq = irq->next) {
        machine_pin_obj_t *pin = MP_OBJ_TO_PTR(irq->base.parent);
        gpio_pin_interrupt_configure(pin->port, pin->pin, GPIO_INT_DISABLE);
        gpio_remove_callback(pin->port, &irq->callback);
    }
    MP_STATE_PORT(machine_pin_irq_list) = NULL;
}

static void gpio_callback_handler(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins) {
    machine_pin_irq_obj_t *irq = CONTAINER_OF(cb, machine_pin_irq_obj_t, callback);

    mp_irq_handler(&irq->base);
}

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL
    if (self->is_pinctrl) {
        mp_printf(print, "Pin(None, alt=%d)", self->pinctrl_data.pinmux);
        return;
    }
    #endif
    mp_printf(print, "<Pin %p %d>", self->port, self->pin);
}

// pin.init(mode, pull=None, *, value)
static mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
    if (ret == -ENOTSUP) {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported pin setup"));
    } else if (ret == -EINVAL) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    } else if (ret == -EIO) {
        mp_raise_ValueError(MP_ERROR_TEXT("I/O error"));
    } else if (ret) {
        mp_raise_ValueError(MP_ERROR_TEXT("couldn't configure pin"));
    }

    return mp_const_none;
}

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL
static mp_obj_t machine_pin_pinctrl_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_alt, ARG_mode, ARG_pull, ARG_drive, ARG_schmitt_enable,  ARG_slew };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_alt, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        { MP_QSTR_drive, MP_ARG_INT, {.u_int = 0 } },
        { MP_QSTR_schmitt_enable, MP_ARG_BOOL, {.u_bool = false } },
        { MP_QSTR_slew, MP_ARG_INT, {.u_int = 0 } },
    };
    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
    size_t index = 0;
    #endif

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (mp_obj_is_str(args[ARG_alt].u_obj)) {
        #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
        const char *name = mp_obj_str_get_str(args[ARG_alt].u_obj);
        for (index = 0; index < PINMUX_COUNT; index++) {
            size_t dummy_len;
            if (strcmp(qstr_data(MP_OBJ_QSTR_VALUE(generated_dynamic_pinmuxes[index].key), &dummy_len), name) == 0) {
                break;
            }
            (void)dummy_len;
        }
        if (index >= PINMUX_COUNT) {
            mp_raise_ValueError(MP_ERROR_TEXT("not a valid pinmux name"));
        }
        self->pinctrl_data.pinmux = mp_obj_get_uint(generated_dynamic_pinmuxes[index].value);
        #else
        mp_raise_ValueError(MP_ERROR_TEXT("pinmuxes not generated"));
        #endif
    } else if (mp_obj_is_int(args[ARG_alt].u_obj)) {
        self->pinctrl_data.pinmux = mp_obj_get_int(args[ARG_alt].u_obj);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pinmux alt"));
    }

    bool pull_up = args[ARG_mode].u_int & GPIO_PULL_UP;
    bool pull_down = args[ARG_mode].u_int & GPIO_PULL_DOWN;
    bool output_enable = args[ARG_mode].u_int & GPIO_OUTPUT;
    bool input_enable = args[ARG_mode].u_int & GPIO_INPUT;

    self->pinctrl_data.pinctrl = mp_dynamic_pinctrl_make_pinctrl_soc_pin_t(
        self->pinctrl_data.pinmux,
        pull_up,
        pull_down,
        args[ARG_schmitt_enable].u_bool,
        output_enable,
        input_enable,
        args[ARG_slew].u_int,
        args[ARG_drive].u_int);

    return mp_const_none;
}

#endif

// constructor(drv_name, pin, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    machine_pin_obj_t *pin;
    if (mp_obj_is_type(args[0], &machine_pin_type)) {
        // Already a Pin object, reuse it.
        pin = MP_OBJ_TO_PTR(args[0]);
    } else if (mp_obj_is_type(args[0], &mp_type_tuple)) {
        // Get the wanted (port, pin) values.
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[0], 2, &items);
        const struct device *wanted_port = zephyr_device_find(items[0]);
        int wanted_pin = mp_obj_get_int(items[1]);

        pin = m_new_obj(machine_pin_obj_t);
        pin->base = machine_pin_obj_template;
        pin->port = wanted_port;
        pin->pin = wanted_pin;
        pin->irq = NULL;
        #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL
        pin->is_pinctrl = false;
    } else if (args[0] == mp_const_none) {
        // Pinctrl Pin
        pin = m_new_obj(machine_pin_obj_t);
        pin->base = machine_pin_obj_template;
        pin->is_pinctrl = true;

        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_pinctrl_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
        #endif
    } else {
        // Unknown Pin.
        mp_raise_ValueError(MP_ERROR_TEXT("Pin id must be tuple of (\"GPIO_x\", pin#) or None"));
    }

    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL
    if ((n_args > 1 || n_kw > 0) && !pin->is_pinctrl) {
    #else
    if (n_args > 1 || n_kw > 0) {
        #endif
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

// fast method for getting/setting pin value
static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;

    machine_pin_check_pinctrl(self);

    if (n_args == 0) {
        int pin_val = gpio_pin_get_raw(self->port, self->pin);
        return MP_OBJ_NEW_SMALL_INT(pin_val);
    } else {
        (void)gpio_pin_set_raw(self->port, self->pin, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

// pin.init(mode, pull)
static mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {

    machine_pin_check_pinctrl((machine_pin_obj_t *)args[0]);

    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

// pin.value([value])
static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;

    machine_pin_check_pinctrl(self);

    (void)gpio_pin_set_raw(self->port, self->pin, 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;

    machine_pin_check_pinctrl(self);

    (void)gpio_pin_set_raw(self->port, self->pin, 1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING, hard=False)
static mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = GPIO_INT_EDGE_BOTH} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_pin_check_pinctrl(self);

    if (self->irq == NULL) {
        machine_pin_irq_obj_t *irq;
        for (irq = MP_STATE_PORT(machine_pin_irq_list); irq != NULL; irq = irq->next) {
            machine_pin_obj_t *irq_pin = MP_OBJ_TO_PTR(irq->base.parent);
            if (irq_pin->port == self->port && irq_pin->pin == self->pin) {
                break;
            }
        }
        if (irq == NULL) {
            irq = m_new_obj(machine_pin_irq_obj_t);
            mp_irq_init(&irq->base, &machine_pin_irq_methods, MP_OBJ_FROM_PTR(self));
            irq->next = MP_STATE_PORT(machine_pin_irq_list);
            gpio_init_callback(&irq->callback, gpio_callback_handler, BIT(self->pin));
            int ret = gpio_add_callback(self->port, &irq->callback);
            if (ret != 0) {
                mp_raise_OSError(-ret);
            }
            MP_STATE_PORT(machine_pin_irq_list) = irq;
        }
        self->irq = irq;
    }

    if (n_args > 1 || kw_args->used != 0) {
        // configure irq
        int ret = gpio_pin_interrupt_configure(self->port, self->pin, GPIO_INT_DISABLE);
        if (ret != 0) {
            mp_raise_OSError(-ret);
        }

        self->irq->base.handler = args[ARG_handler].u_obj;
        self->irq->base.ishard = args[ARG_hard].u_bool;

        if (args[ARG_handler].u_obj != mp_const_none) {
            ret = gpio_pin_interrupt_configure(self->port, self->pin, args[ARG_trigger].u_int);
            if (ret != 0) {
                mp_raise_OSError(-ret);
            }
        }
    }

    return MP_OBJ_FROM_PTR(self->irq);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

static mp_uint_t machine_pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    machine_pin_check_pinctrl(self);

    switch (request) {
        case MP_PIN_READ: {
            return gpio_pin_get_raw(self->port, self->pin);
        }
        case MP_PIN_WRITE: {
            return gpio_pin_set_raw(self->port, self->pin, arg);
        }
    }
    return -1;
}

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL

static mp_obj_t zephyr_pinmux_pin(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;

    if (self->is_pinctrl) {
        return mp_obj_new_int_from_uint(mp_dynamic_pinctrl_get_pin(self->pinctrl_data.pinmux));
    }
    return mp_obj_new_int_from_uint(self->pin);
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_pinmux_pin_obj, zephyr_pinmux_pin);

static mp_obj_t zephyr_pinmux_function(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;

    if (self->is_pinctrl) {
        return mp_obj_new_int_from_uint(mp_dynamic_pinctrl_get_function(self->pinctrl_data.pinmux));
    }
    return mp_obj_new_int_from_uint(MICROPY_DYNAMIC_PINCTRL_FUNCTION_GPIO);
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_pinmux_function_obj, zephyr_pinmux_function);

static mp_obj_t zephyr_pinmux_pinmux(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;

    if (self->is_pinctrl) {
        return mp_obj_new_int_from_uint(self->pinctrl_data.pinmux);
    }
    return mp_obj_new_int_from_uint(mp_dynamic_pinctrl_make_pinmux(self->pin, MICROPY_DYNAMIC_PINCTRL_FUNCTION_GPIO));
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_pinmux_pinmux_obj, zephyr_pinmux_pinmux);

static mp_obj_t zephyr_pinmux_genpinmux(mp_obj_t self_in, mp_obj_t pin_in, mp_obj_t function_in) {
    (void)self_in;
    return mp_obj_new_int_from_uint(mp_dynamic_pinctrl_make_pinmux(mp_obj_get_int(pin_in), mp_obj_get_int(function_in)));
}

static MP_DEFINE_CONST_FUN_OBJ_3(zephyr_pinmux_genpinmux_obj, zephyr_pinmux_genpinmux);

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX

MP_DEFINE_CONST_DICT(mp_zephyr_pinmux_dict, generated_dynamic_pinmuxes);

#endif

#endif

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),     MP_ROM_PTR(&machine_pin_irq_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN),        MP_ROM_INT(GPIO_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT),       MP_ROM_INT(GPIO_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_INOUT),       MP_ROM_INT(GPIO_INPUT | GPIO_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(GPIO_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP),   MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(GPIO_INT_EDGE_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(GPIO_INT_EDGE_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_LOW_LEVEL), MP_ROM_INT(GPIO_INT_LEVEL_LOW) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_HIGH_LEVEL), MP_ROM_INT(GPIO_INT_LEVEL_HIGH) },
    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL
    #if MICROPY_DYNAMIC_PINCTRL_FUNCTION_DRIVE
    { MP_ROM_QSTR(MP_QSTR_DRIVE_0), MP_ROM_INT(MICROPY_DYNAMIC_PINCTRL_FUNCTION_DRIVE_0) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_1), MP_ROM_INT(MICROPY_DYNAMIC_PINCTRL_FUNCTION_DRIVE_1) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_2), MP_ROM_INT(MICROPY_DYNAMIC_PINCTRL_FUNCTION_DRIVE_2) },
    #endif
    /* Additional to the normal Pin API */
    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
    { MP_ROM_QSTR(MP_QSTR_Pinmux), MP_ROM_PTR(&mp_zephyr_pinmux_dict) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&zephyr_pinmux_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_function), MP_ROM_PTR(&zephyr_pinmux_function_obj) },
    { MP_ROM_QSTR(MP_QSTR_pinmux), MP_ROM_PTR(&zephyr_pinmux_pinmux_obj) },
    { MP_ROM_QSTR(MP_QSTR_genpinmux), MP_ROM_PTR(&zephyr_pinmux_genpinmux_obj) },
    #endif
};

static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static const mp_pin_p_t machine_pin_pin_p = {
    .ioctl = machine_pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
    print, machine_pin_print,
    call, machine_pin_call,
    protocol, &machine_pin_pin_p,
    locals_dict, &machine_pin_locals_dict
    );

static mp_uint_t machine_pin_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (new_trigger == 0) {
        new_trigger = GPIO_INT_DISABLE;
    }
    int ret = gpio_pin_interrupt_configure(self->port, self->pin, new_trigger);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }
    return 0;
}

static mp_uint_t machine_pin_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return gpio_get_pending_int(self->port);
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return 0; // TODO
    }
    return 0;
}

static const mp_irq_methods_t machine_pin_irq_methods = {
    .trigger = machine_pin_irq_trigger,
    .info = machine_pin_irq_info,
};

/* Linked list of pin irq objects */
MP_REGISTER_ROOT_POINTER(void *machine_pin_irq_list);

#endif // MICROPY_PY_MACHINE
