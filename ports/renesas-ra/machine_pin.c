/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Damien P. George
 * Copyright (c) 2022 Renesas Electronics Corporation
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"
#include "shared/runtime/mpirq.h"
#include "modmachine.h"

#include "pin.h"
#include "extint.h"

void machine_pin_init(void) {
}

void machine_pin_deinit(void) {
}

// C API used to convert a user-supplied pin name into an ordinal pin number.
const machine_pin_obj_t *machine_pin_find(mp_obj_t user_obj) {
    const machine_pin_obj_t *pin_obj;

    // If a pin was provided, then use it
    if (mp_obj_is_type(user_obj, &machine_pin_type)) {
        pin_obj = MP_OBJ_TO_PTR(user_obj);
        return pin_obj;
    }

    // See if the pin name matches a board pin
    pin_obj = pin_find_named_pin(&machine_pin_board_pins_locals_dict, user_obj);
    if (pin_obj) {
        return pin_obj;
    }

    // See if the pin name matches a cpu pin
    pin_obj = pin_find_named_pin(&machine_pin_cpu_pins_locals_dict, user_obj);
    if (pin_obj) {
        return pin_obj;
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%s) doesn't exist"), mp_obj_str_get_str(user_obj));
}

/// Return a string describing the pin object.
static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // pin name
    mp_printf(print, "Pin(Pin.cpu.%q, mode=Pin.", self->name);

    uint32_t mode = pin_get_mode(self);

    if (mode == MP_HAL_PIN_MODE_ANALOG) {
        // analog
        mp_print_str(print, "ANALOG)");

    } else {
        // IO mode
        bool af = false;
        qstr mode_qst;
        if (mode == MP_HAL_PIN_MODE_INPUT) {
            mode_qst = MP_QSTR_IN;
        } else if (mode == MP_HAL_PIN_MODE_OUTPUT) {
            mode_qst = MP_QSTR_OUT;
        } else if (mode == MP_HAL_PIN_MODE_OPEN_DRAIN) {
            mode_qst = MP_QSTR_OPEN_DRAIN;
        } else {
            af = true;
            if (mode == MP_HAL_PIN_MODE_ALT) {
                mode_qst = MP_QSTR_ALT;
            } else {
                mode_qst = MP_QSTR_ALT_OPEN_DRAIN;
            }
        }
        mp_print_str(print, qstr_str(mode_qst));

        // pull mode
        qstr pull_qst = MP_QSTRnull;
        uint32_t pull = pin_get_pull(self);
        if (pull == MP_HAL_PIN_PULL_UP) {
            pull_qst = MP_QSTR_PULL_UP;
        } else if (pull == MP_HAL_PIN_PULL_NONE) {
            pull_qst = MP_QSTR_PULL_NONE;
        }
        if (pull_qst != MP_QSTRnull) {
            mp_printf(print, ", pull=Pin.%q", pull_qst);
        }

        // drive
        qstr drive_qst = MP_QSTRnull;
        uint32_t drive = pin_get_drive(self);
        if (drive == MP_HAL_PIN_DRIVE_3) {
            drive_qst = MP_QSTR_DRIVE_3;
        } else if (drive == MP_HAL_PIN_DRIVE_2) {
            drive_qst = MP_QSTR_DRIVE_2;
        } else if (drive == MP_HAL_PIN_DRIVE_1) {
            drive_qst = MP_QSTR_DRIVE_1;
        } else if (drive == MP_HAL_PIN_DRIVE_0) {
            drive_qst = MP_QSTR_DRIVE_0;
        }
        if (drive_qst != MP_QSTRnull) {
            mp_printf(print, ", drive=Pin.%q", drive_qst);
        }

        // AF mode
        if (af) {
            mp_uint_t af_idx = pin_get_af(self);
            mp_printf(print, ", alt=%d)", af_idx);
        } else {
            mp_print_str(print, ")");
        }
    }
}

// pin.init(mode=-1, pull=-1, *, value=None, drive=0, alt=-1)
static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value, ARG_drive, ARG_alt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_alt, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get io mode
    uint32_t mode;
    if (args[ARG_mode].u_obj != mp_const_none) {
        mode = mp_obj_get_int(args[ARG_mode].u_obj);
        if (!IS_GPIO_MODE(mode)) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin mode: %d"), mode);
        }
    } else {
        mode = ra_gpio_get_mode(self->pin);
    }

    // get pull mode
    uint32_t pull;
    if (args[ARG_pull].u_obj != mp_const_none) {
        pull = mp_obj_get_int(args[ARG_pull].u_obj);
        if (!IS_GPIO_PULL(pull)) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pull mode: %d"), pull);
        }
        if (pull == MP_HAL_PIN_PULL_DOWN) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%q is not supported"), MP_QSTR_PULL_DOWN);
        }
    } else {
        pull = ra_gpio_get_pull(self->pin);
    }

    // get drive
    uint32_t drive;
    if (args[ARG_drive].u_obj != mp_const_none) {
        drive = mp_obj_get_int(args[ARG_drive].u_obj);
        if (!IS_GPIO_DRIVE(drive)) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin drive: %d"), drive);
        }
    } else {
        drive = ra_gpio_get_drive(self->pin);
    }

    // get alt
    if (args[ARG_alt].u_int != (-1)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("alt is not supported"));
    }
    mp_hal_pin_config(self, mode, pull, drive, 0);
    // if given, set the pin value before initialising to prevent glitches
    if (args[ARG_value].u_obj != mp_const_none) {
        mp_hal_pin_write(self, mp_obj_is_true(args[ARG_value].u_obj));
    }
    return mp_const_none;
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Run an argument through the mapper and return the result.
    const machine_pin_obj_t *pin = machine_pin_find(args[0]);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(pin);
}

// fast method for getting/setting pin value
static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
    } else {
        // set pin
        mp_hal_pin_write(self, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

// pin.init(mode, pull)
static mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

// pin.value([value])
static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// pin.low()
static mp_obj_t machine_pin_low(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_write(self, false);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

// pin.high()
static mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_write(self, true);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING, hard=False)
static mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = MP_HAL_PIN_TRIGGER_RISING | MP_HAL_PIN_TRIGGER_FALLING} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_trigger].u_int & MP_HAL_PIN_TRIGGER_HIGHLEVEL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("%q is not supported"), MP_QSTR_IRQ_HIGH_LEVEL);
    }

    if (n_args > 1 || kw_args->used != 0) {
        // configure irq
        extint_register_pin(self, args[ARG_trigger].u_int,
            args[ARG_hard].u_bool, args[ARG_handler].u_obj);
    }

    // TODO should return an IRQ object
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_pin_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_low), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_high), MP_ROM_PTR(&machine_pin_high_obj) },

    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board),   MP_ROM_PTR(&machine_pin_board_pins_obj_type) },
    { MP_ROM_QSTR(MP_QSTR_cpu),     MP_ROM_PTR(&machine_pin_cpu_pins_obj_type) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(MP_HAL_PIN_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(MP_HAL_PIN_MODE_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(MP_HAL_PIN_MODE_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_ALT), MP_ROM_INT(MP_HAL_PIN_MODE_ALT) },
    { MP_ROM_QSTR(MP_QSTR_ALT_OPEN_DRAIN), MP_ROM_INT(MP_HAL_PIN_MODE_ALT_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_ANALOG), MP_ROM_INT(MP_HAL_PIN_MODE_ANALOG) },
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE), MP_ROM_INT(MP_HAL_PIN_PULL_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(MP_HAL_PIN_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(MP_HAL_PIN_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_0), MP_ROM_INT(MP_HAL_PIN_DRIVE_0) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_1), MP_ROM_INT(MP_HAL_PIN_DRIVE_1) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_2), MP_ROM_INT(MP_HAL_PIN_DRIVE_2) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_3), MP_ROM_INT(MP_HAL_PIN_DRIVE_3) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(MP_HAL_PIN_TRIGGER_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(MP_HAL_PIN_TRIGGER_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_LOW_LEVEL), MP_ROM_INT(MP_HAL_PIN_TRIGGER_LOWLEVEL) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_HIGH_LEVEL), MP_ROM_INT(MP_HAL_PIN_TRIGGER_HIGHLEVEL) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static mp_uint_t machine_pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    switch (request) {
        case MP_PIN_READ: {
            return mp_hal_pin_read(self);
        }
        case MP_PIN_WRITE: {
            mp_hal_pin_write(self, arg);
            return 0;
        }
    }
    return -1;
}

static const mp_pin_p_t machine_pin_pin_p = {
    .ioctl = machine_pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
    locals_dict, &machine_pin_locals_dict,
    print, machine_pin_print,
    call, machine_pin_call,
    protocol, &machine_pin_pin_p
    );

// Returns the pin mode. This value returned by this macro should be one of:
// GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, GPIO_MODE_OUTPUT_OD,
// GPIO_MODE_AF_PP, GPIO_MODE_AF_OD, or GPIO_MODE_ANALOG.

uint32_t pin_get_mode(const machine_pin_obj_t *pin) {
    return ra_gpio_get_mode(pin->pin);
}

// Returns the pin pullup/pulldown. The value returned by this macro should
// be one of GPIO_NOPULL, GPIO_PULLUP, or GPIO_PULLDOWN.

uint32_t pin_get_pull(const machine_pin_obj_t *pin) {
    return (uint32_t)ra_gpio_get_pull(pin->pin);
}

// Returns the pin drive. The value returned by this macro should
// be one of GPIO_HIGH_POWER, GPIO_MID_FAST_POWER, GPIO_MID_POWER,
// or GPIO_LOW_POWER.

uint32_t pin_get_drive(const machine_pin_obj_t *pin) {
    return (uint32_t)ra_gpio_get_drive(pin->pin);
}

// Returns the af (alternate function) index currently set for a pin.

uint32_t pin_get_af(const machine_pin_obj_t *pin) {
    return (uint32_t)ra_gpio_get_af(pin->pin);
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_cpu_pins_obj_type,
    MP_QSTR_cpu,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_pin_cpu_pins_locals_dict
    );

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_board_pins_obj_type,
    MP_QSTR_board,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_pin_board_pins_locals_dict
    );

const machine_pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    const mp_map_t *named_map = &named_pins->map;
    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != MP_OBJ_NULL) {
        return MP_OBJ_TO_PTR(named_elem->value);
    }
    return NULL;
}
