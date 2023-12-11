/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"
#include "shared/runtime/mpirq.h"
#include "pinconf.h"

const machine_pin_obj_t machine_pin_obj_table[16 * 8] = {
    {{&machine_pin_type}, (GPIO_Type *)GPIO0_BASE, PORT_0, PIN_0, MP_QSTR_GPIO0_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO0_BASE, PORT_0, PIN_1, MP_QSTR_GPIO0_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO0_BASE, PORT_0, PIN_2, MP_QSTR_GPIO0_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO0_BASE, PORT_0, PIN_3, MP_QSTR_GPIO0_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO0_BASE, PORT_0, PIN_4, MP_QSTR_GPIO0_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO0_BASE, PORT_0, PIN_5, MP_QSTR_GPIO0_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO0_BASE, PORT_0, PIN_6, MP_QSTR_GPIO0_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO0_BASE, PORT_0, PIN_7, MP_QSTR_GPIO0_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO1_BASE, PORT_1, PIN_0, MP_QSTR_GPIO1_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO1_BASE, PORT_1, PIN_1, MP_QSTR_GPIO1_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO1_BASE, PORT_1, PIN_2, MP_QSTR_GPIO1_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO1_BASE, PORT_1, PIN_3, MP_QSTR_GPIO1_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO1_BASE, PORT_1, PIN_4, MP_QSTR_GPIO1_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO1_BASE, PORT_1, PIN_5, MP_QSTR_GPIO1_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO1_BASE, PORT_1, PIN_6, MP_QSTR_GPIO1_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO1_BASE, PORT_1, PIN_7, MP_QSTR_GPIO1_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO2_BASE, PORT_2, PIN_0, MP_QSTR_GPIO2_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO2_BASE, PORT_2, PIN_1, MP_QSTR_GPIO2_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO2_BASE, PORT_2, PIN_2, MP_QSTR_GPIO2_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO2_BASE, PORT_2, PIN_3, MP_QSTR_GPIO2_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO2_BASE, PORT_2, PIN_4, MP_QSTR_GPIO2_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO2_BASE, PORT_2, PIN_5, MP_QSTR_GPIO2_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO2_BASE, PORT_2, PIN_6, MP_QSTR_GPIO2_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO2_BASE, PORT_2, PIN_7, MP_QSTR_GPIO2_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO3_BASE, PORT_3, PIN_0, MP_QSTR_GPIO3_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO3_BASE, PORT_3, PIN_1, MP_QSTR_GPIO3_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO3_BASE, PORT_3, PIN_2, MP_QSTR_GPIO3_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO3_BASE, PORT_3, PIN_3, MP_QSTR_GPIO3_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO3_BASE, PORT_3, PIN_4, MP_QSTR_GPIO3_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO3_BASE, PORT_3, PIN_5, MP_QSTR_GPIO3_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO3_BASE, PORT_3, PIN_6, MP_QSTR_GPIO3_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO3_BASE, PORT_3, PIN_7, MP_QSTR_GPIO3_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO4_BASE, PORT_4, PIN_0, MP_QSTR_GPIO4_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO4_BASE, PORT_4, PIN_1, MP_QSTR_GPIO4_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO4_BASE, PORT_4, PIN_2, MP_QSTR_GPIO4_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO4_BASE, PORT_4, PIN_3, MP_QSTR_GPIO4_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO4_BASE, PORT_4, PIN_4, MP_QSTR_GPIO4_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO4_BASE, PORT_4, PIN_5, MP_QSTR_GPIO4_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO4_BASE, PORT_4, PIN_6, MP_QSTR_GPIO4_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO4_BASE, PORT_4, PIN_7, MP_QSTR_GPIO4_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO5_BASE, PORT_5, PIN_0, MP_QSTR_GPIO5_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO5_BASE, PORT_5, PIN_1, MP_QSTR_GPIO5_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO5_BASE, PORT_5, PIN_2, MP_QSTR_GPIO5_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO5_BASE, PORT_5, PIN_3, MP_QSTR_GPIO5_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO5_BASE, PORT_5, PIN_4, MP_QSTR_GPIO5_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO5_BASE, PORT_5, PIN_5, MP_QSTR_GPIO5_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO5_BASE, PORT_5, PIN_6, MP_QSTR_GPIO5_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO5_BASE, PORT_5, PIN_7, MP_QSTR_GPIO5_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO6_BASE, PORT_6, PIN_0, MP_QSTR_GPIO6_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO6_BASE, PORT_6, PIN_1, MP_QSTR_GPIO6_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO6_BASE, PORT_6, PIN_2, MP_QSTR_GPIO6_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO6_BASE, PORT_6, PIN_3, MP_QSTR_GPIO6_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO6_BASE, PORT_6, PIN_4, MP_QSTR_GPIO6_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO6_BASE, PORT_6, PIN_5, MP_QSTR_GPIO6_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO6_BASE, PORT_6, PIN_6, MP_QSTR_GPIO6_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO6_BASE, PORT_6, PIN_7, MP_QSTR_GPIO6_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO7_BASE, PORT_7, PIN_0, MP_QSTR_GPIO7_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO7_BASE, PORT_7, PIN_1, MP_QSTR_GPIO7_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO7_BASE, PORT_7, PIN_2, MP_QSTR_GPIO7_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO7_BASE, PORT_7, PIN_3, MP_QSTR_GPIO7_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO7_BASE, PORT_7, PIN_4, MP_QSTR_GPIO7_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO7_BASE, PORT_7, PIN_5, MP_QSTR_GPIO7_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO7_BASE, PORT_7, PIN_6, MP_QSTR_GPIO7_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO7_BASE, PORT_7, PIN_7, MP_QSTR_GPIO7_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO8_BASE, PORT_8, PIN_0, MP_QSTR_GPIO8_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO8_BASE, PORT_8, PIN_1, MP_QSTR_GPIO8_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO8_BASE, PORT_8, PIN_2, MP_QSTR_GPIO8_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO8_BASE, PORT_8, PIN_3, MP_QSTR_GPIO8_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO8_BASE, PORT_8, PIN_4, MP_QSTR_GPIO8_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO8_BASE, PORT_8, PIN_5, MP_QSTR_GPIO8_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO8_BASE, PORT_8, PIN_6, MP_QSTR_GPIO8_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO8_BASE, PORT_8, PIN_7, MP_QSTR_GPIO8_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO9_BASE, PORT_9, PIN_0, MP_QSTR_GPIO9_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO9_BASE, PORT_9, PIN_1, MP_QSTR_GPIO9_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO9_BASE, PORT_9, PIN_2, MP_QSTR_GPIO9_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO9_BASE, PORT_9, PIN_3, MP_QSTR_GPIO9_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO9_BASE, PORT_9, PIN_4, MP_QSTR_GPIO9_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO9_BASE, PORT_9, PIN_5, MP_QSTR_GPIO9_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO9_BASE, PORT_9, PIN_6, MP_QSTR_GPIO9_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO9_BASE, PORT_9, PIN_7, MP_QSTR_GPIO9_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO10_BASE, PORT_10, PIN_0, MP_QSTR_GPIO10_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO10_BASE, PORT_10, PIN_1, MP_QSTR_GPIO10_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO10_BASE, PORT_10, PIN_2, MP_QSTR_GPIO10_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO10_BASE, PORT_10, PIN_3, MP_QSTR_GPIO10_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO10_BASE, PORT_10, PIN_4, MP_QSTR_GPIO10_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO10_BASE, PORT_10, PIN_5, MP_QSTR_GPIO10_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO10_BASE, PORT_10, PIN_6, MP_QSTR_GPIO10_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO10_BASE, PORT_10, PIN_7, MP_QSTR_GPIO10_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO11_BASE, PORT_11, PIN_0, MP_QSTR_GPIO11_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO11_BASE, PORT_11, PIN_1, MP_QSTR_GPIO11_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO11_BASE, PORT_11, PIN_2, MP_QSTR_GPIO11_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO11_BASE, PORT_11, PIN_3, MP_QSTR_GPIO11_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO11_BASE, PORT_11, PIN_4, MP_QSTR_GPIO11_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO11_BASE, PORT_11, PIN_5, MP_QSTR_GPIO11_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO11_BASE, PORT_11, PIN_6, MP_QSTR_GPIO11_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO11_BASE, PORT_11, PIN_7, MP_QSTR_GPIO11_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO12_BASE, PORT_12, PIN_0, MP_QSTR_GPIO12_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO12_BASE, PORT_12, PIN_1, MP_QSTR_GPIO12_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO12_BASE, PORT_12, PIN_2, MP_QSTR_GPIO12_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO12_BASE, PORT_12, PIN_3, MP_QSTR_GPIO12_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO12_BASE, PORT_12, PIN_4, MP_QSTR_GPIO12_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO12_BASE, PORT_12, PIN_5, MP_QSTR_GPIO12_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO12_BASE, PORT_12, PIN_6, MP_QSTR_GPIO12_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO12_BASE, PORT_12, PIN_7, MP_QSTR_GPIO12_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO13_BASE, PORT_13, PIN_0, MP_QSTR_GPIO13_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO13_BASE, PORT_13, PIN_1, MP_QSTR_GPIO13_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO13_BASE, PORT_13, PIN_2, MP_QSTR_GPIO13_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO13_BASE, PORT_13, PIN_3, MP_QSTR_GPIO13_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO13_BASE, PORT_13, PIN_4, MP_QSTR_GPIO13_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO13_BASE, PORT_13, PIN_5, MP_QSTR_GPIO13_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO13_BASE, PORT_13, PIN_6, MP_QSTR_GPIO13_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO13_BASE, PORT_13, PIN_7, MP_QSTR_GPIO13_7},

    {{&machine_pin_type}, (GPIO_Type *)GPIO14_BASE, PORT_14, PIN_0, MP_QSTR_GPIO14_0},
    {{&machine_pin_type}, (GPIO_Type *)GPIO14_BASE, PORT_14, PIN_1, MP_QSTR_GPIO14_1},
    {{&machine_pin_type}, (GPIO_Type *)GPIO14_BASE, PORT_14, PIN_2, MP_QSTR_GPIO14_2},
    {{&machine_pin_type}, (GPIO_Type *)GPIO14_BASE, PORT_14, PIN_3, MP_QSTR_GPIO14_3},
    {{&machine_pin_type}, (GPIO_Type *)GPIO14_BASE, PORT_14, PIN_4, MP_QSTR_GPIO14_4},
    {{&machine_pin_type}, (GPIO_Type *)GPIO14_BASE, PORT_14, PIN_5, MP_QSTR_GPIO14_5},
    {{&machine_pin_type}, (GPIO_Type *)GPIO14_BASE, PORT_14, PIN_6, MP_QSTR_GPIO14_6},
    {{&machine_pin_type}, (GPIO_Type *)GPIO14_BASE, PORT_14, PIN_7, MP_QSTR_GPIO14_7},

    {{&machine_pin_type}, (GPIO_Type *)LPGPIO_BASE, PORT_15, PIN_0, MP_QSTR_GPIO15_0},
    {{&machine_pin_type}, (GPIO_Type *)LPGPIO_BASE, PORT_15, PIN_1, MP_QSTR_GPIO15_1},
    {{&machine_pin_type}, (GPIO_Type *)LPGPIO_BASE, PORT_15, PIN_2, MP_QSTR_GPIO15_2},
    {{&machine_pin_type}, (GPIO_Type *)LPGPIO_BASE, PORT_15, PIN_3, MP_QSTR_GPIO15_3},
    {{&machine_pin_type}, (GPIO_Type *)LPGPIO_BASE, PORT_15, PIN_4, MP_QSTR_GPIO15_4},
    {{&machine_pin_type}, (GPIO_Type *)LPGPIO_BASE, PORT_15, PIN_5, MP_QSTR_GPIO15_5},
    {{&machine_pin_type}, (GPIO_Type *)LPGPIO_BASE, PORT_15, PIN_6, MP_QSTR_GPIO15_6},
    {{&machine_pin_type}, (GPIO_Type *)LPGPIO_BASE, PORT_15, PIN_7, MP_QSTR_GPIO15_7},
};

const machine_pin_obj_t *machine_pin_find(mp_obj_t pin) {
    // Is already a object of the proper type
    if (mp_obj_is_type(pin, &machine_pin_type)) {
        return MP_OBJ_TO_PTR(pin);
    }
    if (mp_obj_is_str(pin)) {
        qstr name = mp_obj_str_get_qstr(pin);
        for (size_t i = 0; i < MP_ARRAY_SIZE(machine_pin_obj_table); ++i) {
            if (name == machine_pin_obj_table[i].name) {
                return &machine_pin_obj_table[i];
            }
        }
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("unknown named pin \"%s\""), name);
    } else if (mp_obj_is_int(pin)) {
        // get the wanted pin object
        int wanted_pin = mp_obj_get_int(pin);
        if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj_table)) {
            return &machine_pin_obj_table[wanted_pin];
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
}

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    qstr mode_qst;
    if (gpio_get_direction(self->gpio, self->pin) == GPIO_PIN_DIR_INPUT) {
        mode_qst = MP_QSTR_IN;
    } else {
        mode_qst = MP_QSTR_OUT;
    }
    mp_printf(print, "Pin(%q, mode=%q", self->name, mode_qst);
    bool pull_up = false;
    uint8_t alt_func, pad_ctrl;
    pinconf_get(self->port, self->pin, &alt_func, &pad_ctrl);
    if (pad_ctrl & PADCTRL_DRIVER_DISABLED_PULL_UP) {
        mp_printf(print, ", pull=%q", MP_QSTR_PULL_UP);
        pull_up = true;
    }
    if (pad_ctrl & PADCTRL_DRIVER_DISABLED_PULL_DOWN) {
        if (pull_up) {
            mp_printf(print, "|%q", MP_QSTR_PULL_DOWN);
        } else {
            mp_printf(print, ", pull=%q", MP_QSTR_PULL_DOWN);
        }
    }
    if (alt_func != PINMUX_ALTERNATE_FUNCTION_0) {
        mp_printf(print, ", alt=%u", alt_func);
    }
    mp_printf(print, ")");
}

enum {
    ARG_mode, ARG_pull, ARG_value, ARG_alt
};
static const mp_arg_t allowed_args[] = {
    {MP_QSTR_mode,  MP_ARG_OBJ,                  {.u_rom_obj = MP_ROM_NONE}},
    {MP_QSTR_pull,  MP_ARG_OBJ,                  {.u_rom_obj = MP_ROM_NONE}},
    {MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
    {MP_QSTR_alt,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
};

static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get initial value of pin (only valid for OUT and OPEN_DRAIN modes)
    int value = -1;
    if (args[ARG_value].u_obj != mp_const_none) {
        value = mp_obj_is_true(args[ARG_value].u_obj);
    }

    // configure mode
    if (args[ARG_mode].u_obj != mp_const_none) {
        mp_int_t mode = mp_obj_get_int(args[ARG_mode].u_obj);
        if (mode == MP_HAL_PIN_MODE_INPUT) {
            mp_hal_pin_input(self);
        } else if (mode == MP_HAL_PIN_MODE_OUTPUT) {
            if (value != -1) {
                // set initial output value before configuring mode
                mp_hal_pin_write(self, value);
            }
            mp_hal_pin_output(self);
        }
    }

    // Configure pull (unconditionally because None means no-pull).
    uint32_t pull = 0;
    if (args[ARG_pull].u_obj != mp_const_none) {
        pull = mp_obj_get_int(args[ARG_pull].u_obj);
    }
    uint8_t alt_func = PINMUX_ALTERNATE_FUNCTION_0;
    uint8_t pad_ctrl = PADCTRL_READ_ENABLE;
    if (pull & MP_HAL_PIN_PULL_UP) {
        pad_ctrl |= PADCTRL_DRIVER_DISABLED_PULL_UP;
    }
    if (pull & MP_HAL_PIN_PULL_DOWN) {
        pad_ctrl |= PADCTRL_DRIVER_DISABLED_PULL_DOWN;
    }
    pinconf_set(self->port, self->pin, alt_func, pad_ctrl);

    return mp_const_none;
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    const machine_pin_obj_t *self = machine_pin_find(args[0]);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }
    return MP_OBJ_FROM_PTR(self);
}

// fast method for getting/setting pin value
static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
    } else {
        // set pin
        bool value = mp_obj_is_true(args[0]);
        mp_hal_pin_write(self, value);
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
    mp_hal_pin_low(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

// pin.high()
static mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_high(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

// pin.toggle()
static mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_toggle_value(self->gpio, self->pin);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_low), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_high), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&machine_pin_toggle_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(MP_HAL_PIN_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(MP_HAL_PIN_MODE_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(MP_HAL_PIN_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(MP_HAL_PIN_PULL_DOWN) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

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

static const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
    print, machine_pin_print,
    call, machine_pin_call,
    protocol, &pin_pin_p,
    locals_dict, &machine_pin_locals_dict
    );

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj) {
    return machine_pin_find(obj);
}
