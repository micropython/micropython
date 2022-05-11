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
#include "shared/runtime/mpirq.h"
#include "modmachine.h"
#include "extmod/virtpin.h"

#include "pin.h"
#include "extint.h"

// Pin class variables
STATIC bool pin_class_debug;

void machine_pin_init(void) {
    MP_STATE_PORT(pin_class_mapper) = mp_const_none;
    MP_STATE_PORT(pin_class_map_dict) = mp_const_none;
    pin_class_debug = false;
}

void machine_pin_deinit(void) {
}

// C API used to convert a user-supplied pin name into an ordinal pin number.
const machine_pin_obj_t *machine_pin_find(mp_obj_t user_obj) {
    const machine_pin_obj_t *pin_obj;

    // If a pin was provided, then use it
    if (mp_obj_is_type(user_obj, &machine_pin_type)) {
        pin_obj = MP_OBJ_TO_PTR(user_obj);
        if (pin_class_debug) {
            printf("Pin map passed pin ");
            mp_obj_print(MP_OBJ_FROM_PTR(pin_obj), PRINT_STR);
            printf("\n");
        }
        return pin_obj;
    }

    if (MP_STATE_PORT(pin_class_mapper) != mp_const_none) {
        mp_obj_t o = mp_call_function_1(MP_STATE_PORT(pin_class_mapper), user_obj);
        if (o != mp_const_none) {
            if (!mp_obj_is_type(o, &machine_pin_type)) {
                mp_raise_ValueError(MP_ERROR_TEXT("Pin.mapper didn't return a Pin object"));
            }
            if (pin_class_debug) {
                printf("Pin.mapper maps ");
                mp_obj_print(user_obj, PRINT_REPR);
                printf(" to ");
                mp_obj_print(o, PRINT_STR);
                printf("\n");
            }
            return MP_OBJ_TO_PTR(o);
        }
        // The pin mapping function returned mp_const_none, fall through to
        // other lookup methods.
    }

    if (MP_STATE_PORT(pin_class_map_dict) != mp_const_none) {
        mp_map_t *pin_map_map = mp_obj_dict_get_map(MP_STATE_PORT(pin_class_map_dict));
        mp_map_elem_t *elem = mp_map_lookup(pin_map_map, user_obj, MP_MAP_LOOKUP);
        if (elem != NULL && elem->value != MP_OBJ_NULL) {
            mp_obj_t o = elem->value;
            if (pin_class_debug) {
                printf("Pin.map_dict maps ");
                mp_obj_print(user_obj, PRINT_REPR);
                printf(" to ");
                mp_obj_print(o, PRINT_STR);
                printf("\n");
            }
            return MP_OBJ_TO_PTR(o);
        }
    }

    // See if the pin name matches a board pin
    pin_obj = pin_find_named_pin(&pin_board_pins_locals_dict, user_obj);
    if (pin_obj) {
        if (pin_class_debug) {
            printf("Pin.board maps ");
            mp_obj_print(user_obj, PRINT_REPR);
            printf(" to ");
            mp_obj_print(MP_OBJ_FROM_PTR(pin_obj), PRINT_STR);
            printf("\n");
        }
        return pin_obj;
    }

    // See if the pin name matches a cpu pin
    pin_obj = pin_find_named_pin(&pin_cpu_pins_locals_dict, user_obj);
    if (pin_obj) {
        if (pin_class_debug) {
            printf("Pin.cpu maps ");
            mp_obj_print(user_obj, PRINT_REPR);
            printf(" to ");
            mp_obj_print(MP_OBJ_FROM_PTR(pin_obj), PRINT_STR);
            printf("\n");
        }
        return pin_obj;
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%s) doesn't exist"), mp_obj_str_get_str(user_obj));
}

/// Return a string describing the pin object.
STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // pin name
    mp_printf(print, "Pin(Pin.cpu.%q, mode=Pin.", self->name);

    uint32_t mode = pin_get_mode(self);

    if (mode == GPIO_MODE_ANALOG) {
        // analog
        mp_print_str(print, "ANALOG)");

    } else {
        // IO mode
        bool af = false;
        qstr mode_qst;
        if (mode == GPIO_MODE_INPUT) {
            mode_qst = MP_QSTR_IN;
        } else if (mode == GPIO_MODE_OUTPUT_PP) {
            mode_qst = MP_QSTR_OUT;
        } else if (mode == GPIO_MODE_OUTPUT_OD) {
            mode_qst = MP_QSTR_OPEN_DRAIN;
        } else {
            af = true;
            if (mode == GPIO_MODE_AF_PP) {
                mode_qst = MP_QSTR_ALT;
            } else {
                mode_qst = MP_QSTR_ALT_OPEN_DRAIN;
            }
        }
        mp_print_str(print, qstr_str(mode_qst));

        // pull mode
        qstr pull_qst = MP_QSTRnull;
        uint32_t pull = pin_get_pull(self);
        if (pull == GPIO_PULLUP) {
            pull_qst = MP_QSTR_PULL_UP;
        } else if (pull == GPIO_NOPULL) {
            pull_qst = MP_QSTR_PULL_NONE;
        }
        if (pull_qst != MP_QSTRnull) {
            mp_printf(print, ", pull=Pin.%q", pull_qst);
        }

        // drive
        qstr drive_qst = MP_QSTRnull;
        uint32_t drive = pin_get_drive(self);
        if (drive == GPIO_HIGH_POWER) {
            drive_qst = MP_QSTR_HIGH_POWER;
        } else if (drive == GPIO_MED_POWER) {
            drive_qst = MP_QSTR_MED_POWER;
        } else if (drive == GPIO_LOW_POWER) {
            drive_qst = MP_QSTR_LOW_POWER;
        }
        if (drive_qst != MP_QSTRnull) {
            mp_printf(print, ", drive=Pin.%q", drive_qst);
        }

        // AF mode
        if (af) {
            mp_uint_t af_idx = pin_get_af(self);
            const pin_af_obj_t *af_obj = pin_find_af_by_index(self, af_idx);
            if (af_obj == NULL) {
                mp_printf(print, ", alt=%d)", af_idx);
            } else {
                mp_printf(print, ", alt=Pin.%q)", af_obj->name);
            }
        } else {
            mp_print_str(print, ")");
        }
    }
}

// pin.init(mode, pull=None, *, value=None, driver=None, alt=FUNC_SIO)
STATIC mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_af, MP_ARG_INT, {.u_int = -1}}, // legacy
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = GPIO_LOW_POWER}},
        { MP_QSTR_alt, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get io mode
    uint mode = args[0].u_int;
    if (!IS_GPIO_MODE(mode)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin mode: %d"), mode);
    }

    // get pull mode
    uint pull = 0;
    if (args[1].u_obj != mp_const_none) {
        pull = mp_obj_get_int(args[1].u_obj);
    }

    // get drive
    uint drive = args[4].u_int;
    if (!IS_GPIO_DRIVE(drive)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin drive: %d"), drive);
    }

    mp_hal_pin_config(self, mode, pull, drive, -1);
    // if given, set the pin value before initialising to prevent glitches
    if (args[3].u_obj != MP_OBJ_NULL) {
        mp_hal_pin_write(self, mp_obj_is_true(args[3].u_obj));
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
STATIC mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
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

/// \classmethod mapper([fun])
/// Get or set the pin mapper function.
STATIC mp_obj_t pin_mapper(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        MP_STATE_PORT(pin_class_mapper) = args[1];
        return mp_const_none;
    }
    return MP_STATE_PORT(pin_class_mapper);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_mapper_fun_obj, 1, 2, pin_mapper);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_mapper_obj, MP_ROM_PTR(&pin_mapper_fun_obj));

/// \classmethod dict([dict])
/// Get or set the pin mapper dictionary.
STATIC mp_obj_t pin_map_dict(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        MP_STATE_PORT(pin_class_map_dict) = args[1];
        return mp_const_none;
    }
    return MP_STATE_PORT(pin_class_map_dict);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_map_dict_fun_obj, 1, 2, pin_map_dict);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_map_dict_obj, MP_ROM_PTR(&pin_map_dict_fun_obj));

#if 0
/// \classmethod af_list()
/// Returns an array of alternate functions available for this pin.
STATIC mp_obj_t pin_af_list(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t result = mp_obj_new_list(0, NULL);

    const pin_af_obj_t *af = self->af;
    for (mp_uint_t i = 0; i < self->num_af; i++, af++) {
        mp_obj_list_append(result, MP_OBJ_FROM_PTR(af));
    }
    return result;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_af_list_obj, pin_af_list);
#endif

/// \classmethod debug([state])
/// Get or set the debugging state (`True` or `False` for on or off).
STATIC mp_obj_t pin_debug(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        pin_class_debug = mp_obj_is_true(args[1]);
        return mp_const_none;
    }
    return mp_obj_new_bool(pin_class_debug);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_debug_fun_obj, 1, 2, pin_debug);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_debug_obj, MP_ROM_PTR(&pin_debug_fun_obj));

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

// pin.low()
STATIC mp_obj_t machine_pin_low(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_write(self, false);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

// pin.high()
STATIC mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_write(self, true);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

// pin.mode()
STATIC mp_obj_t machine_pin_mode(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(pin_get_mode(MP_OBJ_TO_PTR(self_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_mode_obj, machine_pin_mode);

// pin.pull()
STATIC mp_obj_t machine_pin_pull(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(pin_get_pull(MP_OBJ_TO_PTR(self_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_pull_obj, machine_pin_pull);

// pin.drive()
STATIC mp_obj_t machine_pin_drive(mp_obj_t self_in) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Pin.drive() is not supported yet"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_drive_obj, machine_pin_drive);

// pin.toggle()
STATIC mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_toggle(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING, hard=False)
STATIC mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = GPIO_MODE_IT_RISING | GPIO_MODE_IT_FALLING} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (n_args > 1 || kw_args->used != 0) {
        // configure irq
        extint_register_pin(self, args[ARG_trigger].u_int,
            args[ARG_hard].u_bool, args[ARG_handler].u_obj);
    }

    // TODO should return an IRQ object
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

/// \method af()
/// Returns the currently configured alternate-function of the pin. The
/// integer returned will match one of the allowed constants for the af
/// argument to the init function.
STATIC mp_obj_t machine_pin_af(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(pin_get_af(MP_OBJ_TO_PTR(self_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_af_obj, machine_pin_af);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_pin_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_low), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_high), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode), MP_ROM_PTR(&machine_pin_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_pull), MP_ROM_PTR(&machine_pin_pull_obj) },
    { MP_ROM_QSTR(MP_QSTR_drive), MP_ROM_PTR(&machine_pin_drive_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&machine_pin_toggle_obj) },
    #if 0
    { MP_ROM_QSTR(MP_QSTR_af_list), MP_ROM_PTR(&pin_af_list) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_af),      MP_ROM_PTR(&machine_pin_af_obj) },

    // class methods
    { MP_ROM_QSTR(MP_QSTR_mapper),  MP_ROM_PTR(&pin_mapper_obj) },
    { MP_ROM_QSTR(MP_QSTR_dict),    MP_ROM_PTR(&pin_map_dict_obj) },
    { MP_ROM_QSTR(MP_QSTR_debug),   MP_ROM_PTR(&pin_debug_obj) },

    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board),   MP_ROM_PTR(&pin_board_pins_obj_type) },
    { MP_ROM_QSTR(MP_QSTR_cpu),     MP_ROM_PTR(&pin_cpu_pins_obj_type) },


    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(GPIO_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(GPIO_MODE_OUTPUT_PP) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(GPIO_MODE_OUTPUT_OD) },
    { MP_ROM_QSTR(MP_QSTR_ALT), MP_ROM_INT(GPIO_MODE_AF_PP) },
    { MP_ROM_QSTR(MP_QSTR_ALT_OPEN_DRAIN), MP_ROM_INT(GPIO_MODE_AF_OD) },
    { MP_ROM_QSTR(MP_QSTR_ANALOG), MP_ROM_INT(GPIO_MODE_ANALOG) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(GPIO_PULLUP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULLDOWN) },
    { MP_ROM_QSTR(MP_QSTR_PULL_HOLD), MP_ROM_INT(GPIO_PULLHOLD) },
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE), MP_ROM_INT(GPIO_NOPULL) },
    { MP_ROM_QSTR(MP_QSTR_LOW_POWER), MP_ROM_INT(GPIO_LOW_POWER) },
    { MP_ROM_QSTR(MP_QSTR_MED_POWER), MP_ROM_INT(GPIO_MED_POWER) },
    { MP_ROM_QSTR(MP_QSTR_HIGH_POWER), MP_ROM_INT(GPIO_HIGH_POWER) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(GPIO_MODE_IT_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(GPIO_MODE_IT_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING_FALLING), MP_ROM_INT(GPIO_MODE_IT_RISING_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_EVT_RISING), MP_ROM_INT(GPIO_MODE_EVT_RISING) },
    { MP_ROM_QSTR(MP_QSTR_EVT_FALLING), MP_ROM_INT(GPIO_MODE_EVT_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_EVT_RISING_FALLING), MP_ROM_INT(GPIO_MODE_EVT_RISING_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_LOWLEVEL), MP_ROM_INT(GPIO_IRQ_LOWLEVEL) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_HIGHLEVEL), MP_ROM_INT(GPIO_IRQ_HIGHLEVEL) },
};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC mp_uint_t machine_pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
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
// be one of GPIO_HIGH_POWER, GPIO_MED_POWER, or GPIO_LOW_POWER.

uint32_t pin_get_drive(const machine_pin_obj_t *pin) {
    return (uint32_t)ra_gpio_get_drive(pin->pin);
}

// Returns the af (alternate function) index currently set for a pin.

uint32_t pin_get_af(const machine_pin_obj_t *pin) {
    return (uint32_t)ra_gpio_get_af(pin->pin);
}

const mp_obj_type_t pin_cpu_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_cpu,
    .locals_dict = (mp_obj_dict_t *)&pin_cpu_pins_locals_dict,
};

const mp_obj_type_t pin_board_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_board,
    .locals_dict = (mp_obj_dict_t *)&pin_board_pins_locals_dict,
};

const machine_pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    const mp_map_t *named_map = &named_pins->map;
    mp_map_elem_t *named_elem = mp_map_lookup((mp_map_t *)named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != MP_OBJ_NULL) {
        return MP_OBJ_TO_PTR(named_elem->value);
    }
    return NULL;
}

const pin_af_obj_t *pin_find_af(const machine_pin_obj_t *pin, uint8_t fn, uint8_t unit) {
    const pin_af_obj_t *af = pin->af;
    for (mp_uint_t i = 0; i < pin->num_af; i++, af++) {
        if (af->fn == fn && af->unit == unit) {
            return af;
        }
    }
    return NULL;
}

const pin_af_obj_t *pin_find_af_by_index(const machine_pin_obj_t *pin, mp_uint_t af_idx) {
    const pin_af_obj_t *af = pin->af;
    for (mp_uint_t i = 0; i < pin->num_af; i++, af++) {
        if (af->idx == af_idx) {
            return af;
        }
    }
    return NULL;
}

// ====================================================================
// PinAF
// ====================================================================

/// \moduleref pyb
/// \class PinAF - Pin Alternate Functions
///
/// A Pin represents a physical pin on the microcprocessor. Each pin
/// can have a variety of functions (GPIO, I2C SDA, etc). Each PinAF
/// object represents a particular function for a pin.
///
/// Usage Model:
///
///     x3 = pyb.Pin.board.X3
///     x3_af = x3.af_list()
///
/// x3_af will now contain an array of PinAF objects which are availble on
/// pin X3.
///
/// For the pyboard, x3_af would contain:
///     [Pin.AF1_TIM2, Pin.AF2_TIM5, Pin.AF3_TIM9, Pin.AF7_USART2]
///
/// Normally, each peripheral would configure the af automatically, but sometimes
/// the same function is available on multiple pins, and having more control
/// is desired.
///
/// To configure X3 to expose TIM2_CH3, you could use:
///    pin = pyb.Pin(pyb.Pin.board.X3, mode=pyb.Pin.AF_PP, af=pyb.Pin.AF1_TIM2)
/// or:
///    pin = pyb.Pin(pyb.Pin.board.X3, mode=pyb.Pin.AF_PP, af=1)

/// \method __str__()
/// Return a string describing the alternate function.
STATIC void pin_af_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_af_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Pin.%q", self->name);
}

/// \method index()
/// Return the alternate function index.
STATIC mp_obj_t pin_af_index(mp_obj_t self_in) {
    pin_af_obj_t *af = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(af->idx);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_af_index_obj, pin_af_index);

/// \method name()
/// Return the name of the alternate function.
STATIC mp_obj_t pin_af_name(mp_obj_t self_in) {
    pin_af_obj_t *af = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_QSTR(af->name);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_af_name_obj, pin_af_name);

STATIC mp_obj_t pin_af_reg(mp_obj_t self_in) {
    pin_af_obj_t *af = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT((uintptr_t)af->reg);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_af_reg_obj, pin_af_reg);

STATIC const mp_rom_map_elem_t pin_af_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_index), MP_ROM_PTR(&pin_af_index_obj) },
    { MP_ROM_QSTR(MP_QSTR_name), MP_ROM_PTR(&pin_af_name_obj) },
    { MP_ROM_QSTR(MP_QSTR_reg), MP_ROM_PTR(&pin_af_reg_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pin_af_locals_dict, pin_af_locals_dict_table);

const mp_obj_type_t pin_af_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinAF,
    .print = pin_af_obj_print,
    .locals_dict = (mp_obj_dict_t *)&pin_af_locals_dict,
};
