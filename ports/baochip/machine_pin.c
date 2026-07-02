/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython contributors
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

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "hardware/gpio.h"

#include "machine_pin.h"
#include "modmachine.h"

// Pin modes (values exposed as Pin.IN / Pin.OUT / Pin.OPEN_DRAIN).
#define PIN_MODE_IN          (0)
#define PIN_MODE_OUT         (1)
#define PIN_MODE_OPEN_DRAIN  (2)

// Pull configuration. Baochip-1x supports pull-up only.
#define PIN_PULL_NONE        (0)
#define PIN_PULL_UP          (1)

// Open-drain emulation state: one bit per pin, one uint16_t per port.
uint16_t machine_pin_open_drain_mask[BAOCHIP_NUM_PORTS];

// --- Lookup ---

static const machine_pin_obj_t *machine_pin_find_named(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    const mp_map_t *map = &named_pins->map;
    mp_map_elem_t *elem = mp_map_lookup((mp_map_t *)map, name, MP_MAP_LOOKUP);
    if (elem != NULL && elem->value != NULL) {
        return MP_OBJ_TO_PTR(elem->value);
    }
    return NULL;
}

const machine_pin_obj_t *machine_pin_find(mp_obj_t pin) {
    if (mp_obj_is_type(pin, &machine_pin_type)) {
        return MP_OBJ_TO_PTR(pin);
    }
    // TODO: add mp_obj_is_small_int branch for header-pin index lookup.
    if (mp_obj_is_str(pin)) {
        const machine_pin_obj_t *self =
            machine_pin_find_named(&machine_pin_board_pins_locals_dict, pin);
        if (self != NULL) {
            return self;
        }
        self = machine_pin_find_named(&machine_pin_cpu_pins_locals_dict, pin);
        if (self != NULL) {
            return self;
        }
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("unknown named pin \"%s\""), mp_obj_str_get_str(pin));
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
}

// --- Hardware configuration ---

static void pin_apply(const machine_pin_obj_t *self, int mode, int pull, int initial_value) {
    // Validate inputs before touching any hardware state.
    if (mode != PIN_MODE_IN && mode != PIN_MODE_OUT && mode != PIN_MODE_OPEN_DRAIN) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin mode"));
    }
    if (pull != PIN_PULL_UP && pull != PIN_PULL_NONE) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pull"));
    }

    // TODO: tear down any active alternate function before switching to GPIO.
    gpio_init(self->port, self->pin);

    if (pull == PIN_PULL_UP) {
        gpio_pull_up(self->port, self->pin);
    } else {
        gpio_disable_pulls(self->port, self->pin);
    }

    if (mode == PIN_MODE_IN) {
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        PIN_DISABLE_OPEN_DRAIN(self->port, self->pin);
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        gpio_set_dir(self->port, self->pin, false);
    } else if (mode == PIN_MODE_OUT) {
        if (initial_value >= 0) {
            gpio_put(self->port, self->pin, initial_value != 0);
        }
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        PIN_DISABLE_OPEN_DRAIN(self->port, self->pin);
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        gpio_set_dir(self->port, self->pin, true);
    } else { // PIN_MODE_OPEN_DRAIN
        // OD emulation: DATA latch is always 0; direction selects low/float.
        // Stage data/dir before flipping the OD mask so a preempting
        // caller never sees direction=output with PIN_IS_OPEN_DRAIN clear.
        gpio_put(self->port, self->pin, false);
        gpio_set_dir(self->port, self->pin, initial_value == 0);
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        PIN_ENABLE_OPEN_DRAIN(self->port, self->pin);
        MICROPY_END_ATOMIC_SECTION(atomic_state);
    }
}

// --- Python methods ---

static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed[] = {
        { MP_QSTR_mode,  MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_pull,  MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed), allowed, args);

    int pull = PIN_PULL_NONE;
    if (args[ARG_pull].u_obj != mp_const_none) {
        pull = mp_obj_get_int(args[ARG_pull].u_obj);
    }
    int value = -1;
    if (args[ARG_value].u_obj != mp_const_none) {
        value = mp_obj_is_true(args[ARG_value].u_obj) ? 1 : 0;
    }

    if (args[ARG_mode].u_obj != mp_const_none) {
        int mode = mp_obj_get_int(args[ARG_mode].u_obj);
        pin_apply(self, mode, pull, value);
    } else {
        // Mode not specified: apply pull and/or value independently.
        if (args[ARG_pull].u_obj != mp_const_none) {
            if (pull == PIN_PULL_UP) {
                gpio_pull_up(self->port, self->pin);
            } else if (pull == PIN_PULL_NONE) {
                gpio_disable_pulls(self->port, self->pin);
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid pull"));
            }
        }
        if (value >= 0) {
            if (PIN_IS_OPEN_DRAIN(self->port, self->pin)) {
                gpio_set_dir(self->port, self->pin, !value);
            } else {
                gpio_put(self->port, self->pin, value != 0);
            }
        }
    }
    return mp_const_none;
}

// machine.Pin(id, mode=None, pull=None, *, value=None)
static mp_obj_t machine_pin_make_new(const mp_obj_type_t *type_in,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    const machine_pin_obj_t *self = machine_pin_find(args[0]);
    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }
    return MP_OBJ_FROM_PTR(self);
}

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool is_od = PIN_IS_OPEN_DRAIN(self->port, self->pin);
    bool is_out = gpio_get_dir(self->port, self->pin);
    qstr mode_qst;
    if (is_od) {
        mode_qst = MP_QSTR_OPEN_DRAIN;
    } else if (is_out) {
        mode_qst = MP_QSTR_OUT;
    } else {
        mode_qst = MP_QSTR_IN;
    }
    mp_printf(print, "Pin(%q, mode=%q", self->name, mode_qst);
    if (gpio_get_pull(self->port, self->pin)) {
        mp_printf(print, ", pull=PULL_UP");
    }
    if (is_out || is_od) {
        mp_printf(print, ", value=%d", gpio_get(self->port, self->pin) ? 1 : 0);
    }
    mp_printf(print, ")");
}

// pin.value([v])
static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(gpio_get(self->port, self->pin) ? 1 : 0);
    }
    bool val = mp_obj_is_true(args[1]);
    if (PIN_IS_OPEN_DRAIN(self->port, self->pin)) {
        gpio_set_dir(self->port, self->pin, !val);
    } else {
        gpio_put(self->port, self->pin, val);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (PIN_IS_OPEN_DRAIN(self->port, self->pin)) {
        gpio_set_dir(self->port, self->pin, false); // float = high
    } else {
        gpio_put(self->port, self->pin, true);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (PIN_IS_OPEN_DRAIN(self->port, self->pin)) {
        gpio_set_dir(self->port, self->pin, true); // drive low = off
    } else {
        gpio_put(self->port, self->pin, false);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static mp_obj_t machine_pin_toggle(mp_obj_t self_in) {
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (PIN_IS_OPEN_DRAIN(self->port, self->pin)) {
        bool currently_out = gpio_get_dir(self->port, self->pin);
        gpio_set_dir(self->port, self->pin, !currently_out);
    } else {
        gpio_toggle(self->port, self->pin);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_toggle_obj, machine_pin_toggle);

// pin.init(mode=None, pull=None, *, value=None)
static mp_obj_t machine_pin_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_init);

// --- board / cpu sub-namespaces ---

static MP_DEFINE_CONST_OBJ_TYPE(
    pin_cpu_pins_obj_type,
    MP_QSTR_cpu,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_pin_cpu_pins_locals_dict
    );

static MP_DEFINE_CONST_OBJ_TYPE(
    pin_board_pins_obj_type,
    MP_QSTR_board,
    MP_TYPE_FLAG_NONE,
    locals_dict, &machine_pin_board_pins_locals_dict
    );

// --- locals dict ---

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value),  MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),     MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),    MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&machine_pin_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_init),   MP_ROM_PTR(&machine_pin_init_obj) },

    { MP_ROM_QSTR(MP_QSTR_board),      MP_ROM_PTR(&pin_board_pins_obj_type) },
    { MP_ROM_QSTR(MP_QSTR_cpu),        MP_ROM_PTR(&pin_cpu_pins_obj_type) },

    { MP_ROM_QSTR(MP_QSTR_IN),         MP_ROM_INT(PIN_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT),        MP_ROM_INT(PIN_MODE_OUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(PIN_MODE_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE),  MP_ROM_INT(PIN_PULL_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP),    MP_ROM_INT(PIN_PULL_UP) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_make_new,
    print, machine_pin_print,
    locals_dict, &machine_pin_locals_dict
    );

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj) {
    return machine_pin_find(obj);
}
