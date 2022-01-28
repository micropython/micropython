/*
 * This is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Damien P. George
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
 *
 * Uses pins.h & pins.c to create board (MCU package) specific 'machine_pin_obj' array.
 */

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/virtpin.h"
#include "modmachine.h"
#include "samd_soc.h"
#include "pins.h" // boards/<BOARD>/

// ASF4 (MCU package specific pin defs in 'boards')
#include "hal_gpio.h"
#include "hpl_gpio.h"
#include "hal_atomic.h"

#define GPIO_MODE_IN (0)
#define GPIO_MODE_OUT (1)
// #define GPIO_MODE_ALT (3)

#define GPIO_STRENGTH_2MA (0)
#define GPIO_STRENGTH_8MA (1)

// asf4 hpl_gpio.h gpio_pull_mode

/*
typedef struct _machine_pin_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
} machine_pin_irq_obj_t;

STATIC const mp_irq_methods_t machine_pin_irq_methods;
*/

STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    mp_printf(print, "Pin(%u)", self->id);
}

STATIC void pin_validate_drive(bool strength) {
    if (strength != GPIO_STRENGTH_2MA && strength != GPIO_STRENGTH_8MA) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid argument(s) value"));
    }
}

// Pin.init(mode, pull=None, *, value=None, drive=0). No 'alt' yet.
STATIC mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value, ARG_drive, ARG_alt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = GPIO_STRENGTH_2MA} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // set initial value (do this before configuring mode/pull)
    if (args[ARG_value].u_obj != mp_const_none) {
        gpio_set_pin_level(self->id, mp_obj_is_true(args[ARG_value].u_obj));
    }

    // configure mode
    if (args[ARG_mode].u_obj != mp_const_none) {
        mp_int_t mode = mp_obj_get_int(args[ARG_mode].u_obj);
        if (mode == GPIO_MODE_IN) {
            gpio_set_pin_direction(self->id, GPIO_DIRECTION_IN);
        } else if (mode == GPIO_MODE_OUT) {
            gpio_set_pin_direction(self->id, GPIO_DIRECTION_OUT);
        } else {
            gpio_set_pin_direction(self->id, GPIO_DIRECTION_IN); // If no args are given, the Pin is 'input'.
        }
    }
    // configure pull. Only to be used with IN mode. The function sets the pin to INPUT.
    uint32_t pull = 0;
    mp_int_t mode = mp_obj_get_int(args[ARG_mode].u_obj);
    if (mode == GPIO_MODE_OUT && args[ARG_pull].u_obj != mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("OUT incompatible with pull"));
    } else if (args[ARG_pull].u_obj != mp_const_none) {
        pull = mp_obj_get_int(args[ARG_pull].u_obj);
        gpio_set_pin_pull_mode(self->id, pull); // hal_gpio.h
    }

    // get the strength
    bool strength = args[3].u_int;
    pin_validate_drive(strength);

    return mp_const_none;
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get the wanted pin object
    int wanted_pin = mp_obj_get_int(args[0]);

    const machine_pin_obj_t *self = NULL;
    if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj)) {
        self = (machine_pin_obj_t *)&machine_pin_obj[wanted_pin];
    }

    if (self == NULL || self->base.type == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }
    self = (machine_pin_obj_t *)&machine_pin_obj[wanted_pin];

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

// fast method for getting/setting pin value
STATIC mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(gpio_get_pin_level(self->id));
    } else {
        // set pin
        bool value = mp_obj_is_true(args[0]);
        gpio_set_pin_level(self->id, value);

        return mp_const_none;
    }
}

// Pin.init(mode, pull)
STATIC mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

// Pin.value([value])
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// Pin.disable(pin)
STATIC mp_obj_t machine_pin_disable(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_set_pin_direction(self->id, GPIO_DIRECTION_OFF); // Disables the pin (low power state)
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_disable_obj, machine_pin_disable);

// Pin.low() Totem-pole (push-pull)
STATIC mp_obj_t machine_pin_low(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_set_pin_direction(self->id, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(self->id, false);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

// Pin.high() Totem-pole (push-pull)
STATIC mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_set_pin_direction(self->id, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(self->id, true);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

// Pin.toggle(). Only TOGGLE pins set as OUTPUT.
STATIC mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Determine DIRECTION of PIN.
    bool pin_dir;

    pin_dir = (PORT->Group[(enum gpio_port)GPIO_PORT(self->id)].DIR.reg // Get PORT#
        & (1 << GPIO_PIN(self->id)))      // Isolate the Pin in question
        >> GPIO_PIN(self->id);            // Shift to LSB for binary result.

    if (pin_dir) {
        // Pin is OUTPUT
        gpio_set_pin_direction(self->id, GPIO_DIRECTION_OUT);
        gpio_toggle_pin_level(self->id);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Cannot TOGGLE INPUT pin!\n"));
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

// Pin.drive(). Normal (0) is 2mA, High (1) allows 8mA.
STATIC mp_obj_t machine_pin_drive(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = args[0]; // Pin
    if (n_args == 1) {
        return mp_const_none;
    } else {
        bool strength = mp_obj_get_int(args[1]); // 0 or 1
        pin_validate_drive(strength);
        // Set the DRVSTR bit (ASF hri/hri_port_dxx.h
        hri_port_write_PINCFG_DRVSTR_bit(PORT,
            (enum gpio_port)GPIO_PORT(self->id),
            GPIO_PIN(self->id),
            strength);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_drive_obj, 1, 2, machine_pin_drive);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_low), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_high), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&machine_pin_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&machine_pin_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_drive), MP_ROM_PTR(&machine_pin_drive_obj) },


    // { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_pin_irq_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(GPIO_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(GPIO_MODE_OUT) },
    // { MP_ROM_QSTR(MP_QSTR_ALT), MP_ROM_INT(GPIO_MODE_ALT) },
    { MP_ROM_QSTR(MP_QSTR_PULL_OFF), MP_ROM_INT(GPIO_PULL_OFF) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_LOW_POWER), MP_ROM_INT(GPIO_STRENGTH_2MA) },
    { MP_ROM_QSTR(MP_QSTR_HIGH_POWER), MP_ROM_INT(GPIO_STRENGTH_8MA) },
    // { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(GPIO_IRQ_EDGE_RISE) },
    // { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(GPIO_IRQ_EDGE_FALL) },
};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    switch (request) {
        case MP_PIN_READ: {
            return gpio_get_pin_level(self->id);
        }
        case MP_PIN_WRITE: {
            gpio_set_pin_level(self->id, arg);
            return 0;
        }
    }
    return -1;
}

STATIC const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

const mp_obj_type_t machine_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = machine_pin_print,
    .make_new = mp_pin_make_new,
    .call = machine_pin_call,
    .protocol = &pin_pin_p,
    .locals_dict = (mp_obj_t)&machine_pin_locals_dict,
};

/*
STATIC mp_uint_t machine_pin_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_obj[self->id]);
    gpio_set_irq_enabled(self->id, GPIO_IRQ_ALL, false);
    irq->flags = 0;
    irq->trigger = new_trigger;
    gpio_set_irq_enabled(self->id, new_trigger, true);
    return 0;
}

STATIC mp_uint_t machine_pin_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_pin_irq_obj_t *irq = MP_STATE_PORT(machine_pin_irq_obj[self->id]);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return irq->flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return irq->trigger;
    }
    return 0;
}

STATIC const mp_irq_methods_t machine_pin_irq_methods = {
    .trigger = machine_pin_irq_trigger,
    .info = machine_pin_irq_info,
};
*/

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj) {
    if (!mp_obj_is_type(obj, &machine_pin_type)) {
        mp_raise_ValueError(MP_ERROR_TEXT("expecting a Pin"));
    }
    machine_pin_obj_t *pin = MP_OBJ_TO_PTR(obj);
    return pin->id;
}
