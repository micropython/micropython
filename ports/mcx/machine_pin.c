/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
 * Copyright (c) 2024 NXP
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

#include "extmod/modmachine.h"
#include "extmod/virtpin.h"

#include "machine_pin.h"
#include "machine_pin_defs.h"

static mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value, ARG_drive, ARG_alt };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE }},
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_alt, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    drv_pin_t pin = {
        .port = self->port,
        .pin = self->pin,
    };

    drv_pin_config_t pin_cfg;

    /* Retrieve current (default) configuration */
    drv_pin_config_get(&pin, &pin_cfg);
    /* TODO: check return value */

    /* Validate and set mode */
    if (args[ARG_mode].u_obj != mp_const_none) {
        pin_cfg.mode = (drv_pin_mode_t)mp_obj_get_int(args[ARG_mode].u_obj);
        if (pin_cfg.mode >= DRV_PIN_MODE_END) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin mode: %d"), pin_cfg.mode);
        }
    }

    /* Validate and set pull mode */
    if (args[ARG_pull].u_obj != mp_const_none) {
        pin_cfg.pull = (drv_pin_pull_type_t)mp_obj_get_int(args[ARG_pull].u_obj);
        if (pin_cfg.pull >= DRV_PIN_PULL_END) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pull mode: %d"), pin_cfg.pull);
        }
    }

    /* Validate and set drive strength */
    if (args[ARG_drive].u_obj != mp_const_none) {
        pin_cfg.drive = (drv_pin_drive_t)mp_obj_get_int(args[ARG_drive].u_obj);
        if (pin_cfg.drive >= DRV_PIN_DRIVE_END) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid drive strength settings: %d"), pin_cfg.drive);
        }
    }

    /* Validate and set alternative function */
    pin_cfg.af = args[ARG_alt].u_int;

    int ret = drv_pin_config_set(&pin, &pin_cfg);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to set pin mode: %d"), ret);
    }

    /* Validate and set initial value IF MODE IS GPIO OUTPUT OR OD */
    if ((pin_cfg.mode == DRV_PIN_MODE_OUTPUT_OD) || (pin_cfg.mode == DRV_PIN_MODE_OUTPUT_PP)) {
        if (args[ARG_value].u_obj != mp_const_none) {
            bool write_value = mp_obj_get_int(args[ARG_value].u_obj) ? true : false;
            drv_pin_write(&pin, write_value);
        }
    }

    return mp_const_none;
}

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    drv_pin_t pin = {
        .port = self->port,
        .pin = self->pin,
    };

    /* Note: a common hardfault reason is a direct print call from Pin.cpu._ or Pin.board._,
     * which is not initialized using a proper constructor. Currently it seems there's no way
     * finding out whether this pin is initialized or not (every obj is a const ROM obj).
     * WORKAROUND: Init the called pin (enable the PORT and GPIO clocks).
     */
    drv_pin_init(&pin);

    drv_pin_config_t pin_cfg;
    drv_pin_config_get(&pin, &pin_cfg);

    mp_printf(print, "Pin(Pin.cpu.%q, mode=Pin.", self->name);

    qstr mode_qst;

    switch (pin_cfg.mode) {
        case DRV_PIN_MODE_INPUT:
            mode_qst = MP_QSTR_IN;
            break;

        case DRV_PIN_MODE_OUTPUT_PP:
            mode_qst = MP_QSTR_OUTPUT;
            break;

        case DRV_PIN_MODE_OUTPUT_OD:
            mode_qst = MP_QSTR_OPEN_DRAIN;
            break;

        case DRV_PIN_MODE_AF_PP:
            mode_qst = MP_QSTR_ALT;
            break;

        case DRV_PIN_MODE_AF_OD:
            mode_qst = MP_QSTR_ALT_OPEN_DRAIN;
            break;

        case DRV_PIN_MODE_ANALOG:
        default:
            mode_qst = MP_QSTR_ANALOG;
            break;

    }

    mp_printf(print, "%s", qstr_str(mode_qst));

    qstr pull_qst;

    switch (pin_cfg.pull) {
        case DRV_PIN_PULL_UP:
            pull_qst = MP_QSTR_PULL_UP;
            break;

        case DRV_PIN_PULL_DOWN:
            pull_qst = MP_QSTR_PULL_DOWN;
            break;

        case DRV_PIN_PULL_NONE:
        default:
            pull_qst = MP_QSTR_PULL_NONE;
            break;
    }

    mp_printf(print, " pull=Pin.%s", qstr_str(pull_qst));

    qstr drive_qst;
    switch (pin_cfg.drive) {
        case DRV_PIN_DRIVE_HIGH:
            drive_qst = MP_QSTR_DRIVE_HIGH;
            break;

        case DRV_PIN_DRIVE_LOW:
        default:
            drive_qst = MP_QSTR_DRIVE_LOW;
            break;
    }

    mp_printf(print, " drive=Pin.%s", qstr_str(drive_qst));

    /* AF */
    mp_printf(print, " alt=%d", pin_cfg.af);

    /* TODO: Add other attributes */

    mp_printf(print, ")");
}

static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (n_args == 0) {
        int read_val = mp_hal_pin_read(self);
        /* TODO: Check negative read errors (does this even occur?). */
        return MP_OBJ_NEW_SMALL_INT(read_val);
    } else {
        mp_hal_pin_write(self, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

static mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_high(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_hal_pin_high(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_high_obj, machine_pin_high);

static mp_obj_t machine_pin_low(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_hal_pin_low(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_low_obj, machine_pin_low);

static mp_uint_t machine_pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    bool output_val = (int)arg ? true : false;

    switch (request) {
        case MP_PIN_READ:
            return mp_hal_pin_read(self);
            break;
        case MP_PIN_WRITE:
            mp_hal_pin_write(self, output_val);
            break;
        default:
            return -1;
            break;
    }

    return 0;
}

mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_obj_t pin_obj = args[0];

    const machine_pin_obj_t *pin;

    if (mp_obj_is_type(pin_obj, &machine_pin_type)) {
        pin = MP_OBJ_TO_PTR(pin_obj);
    } else {
        pin = mp_hal_pin_find_named_pin(&machine_pin_board_pins_locals_dict, pin_obj);
        if (pin == NULL) {
            pin = mp_hal_pin_find_named_pin(&machine_pin_cpu_pins_locals_dict, pin_obj);
        }

        if (pin == NULL) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%s) doesn't exist"), mp_obj_str_get_str(pin_obj));
        }
    }

    drv_pin_t drv_pin = {
        .port = pin->port,
        .pin = pin->pin,
    };

    /* TODO: check return value */
    drv_pin_init(&drv_pin);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(pin);
}

static const mp_pin_p_t machine_pin_pin_p = {
    .ioctl = machine_pin_ioctl,
};

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

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    /* Instance methods */
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_high), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_low), MP_ROM_PTR(&machine_pin_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_low_obj) },

    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board), MP_ROM_PTR(&machine_pin_board_pins_obj_type) },
    { MP_ROM_QSTR(MP_QSTR_cpu), MP_ROM_PTR(&machine_pin_cpu_pins_obj_type) },

    /* Class constants */
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(MP_HAL_PIN_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(MP_HAL_PIN_MODE_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(MP_HAL_PIN_MODE_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_ALT), MP_ROM_INT(MP_HAL_PIN_MODE_ALT) },
    { MP_ROM_QSTR(MP_QSTR_ALT_OPEN_DRAIN), MP_ROM_INT(MP_HAL_PIN_MODE_ALT_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_ANALOG), MP_ROM_INT(MP_HAL_PIN_MODE_ANALOG) },
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE), MP_ROM_INT(MP_HAL_PIN_PULL_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(MP_HAL_PIN_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(MP_HAL_PIN_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_HIGH), MP_ROM_INT(MP_HAL_PIN_DRIVE_0) },
    { MP_ROM_QSTR(MP_QSTR_DRIVE_LOW), MP_ROM_INT(MP_HAL_PIN_DRIVE_1) },
};

static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

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
