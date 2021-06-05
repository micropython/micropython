/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Philipp Ebensberger
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

#include "fsl_gpio.h"
#include "fsl_iomuxc.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "pin.h"
#include "mphalport.h"

// Local functions
STATIC mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

// Class Methods
STATIC void machine_pin_obj_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind);
STATIC mp_obj_t machine_pin_obj_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args);
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

// Instance Methods
STATIC mp_obj_t machine_pin_off(mp_obj_t self_in);
STATIC mp_obj_t machine_pin_on(mp_obj_t self_in);
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args);
STATIC mp_obj_t machine_pin_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);

// Local data
enum {
    PIN_INIT_ARG_MODE = 0,
    PIN_INIT_ARG_PULL,
    PIN_INIT_ARG_VALUE,
    PIN_INIT_ARG_DRIVE,
};

// Pin mapping dictionaries
const mp_obj_type_t machine_pin_cpu_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_cpu,
    .locals_dict = (mp_obj_t)&machine_pin_cpu_pins_locals_dict,
};

const mp_obj_type_t machine_pin_board_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_board,
    .locals_dict = (mp_obj_t)&machine_pin_board_pins_locals_dict,
};

// Simplified mode setting used by the extmod modules
void machine_pin_set_mode(const machine_pin_obj_t *self, uint8_t mode) {
    gpio_pin_config_t pin_config = {kGPIO_DigitalInput, 1, kGPIO_NoIntmode};

    pin_config.direction = (mode == PIN_MODE_IN ? kGPIO_DigitalInput : kGPIO_DigitalOutput);
    GPIO_PinInit(self->gpio, self->pin, &pin_config);
    if (mode == PIN_MODE_OPEN_DRAIN) {
        uint32_t pad_config = *(uint32_t *)self->configRegister;
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_ODE(0b1) | IOMUXC_SW_PAD_CTL_PAD_DSE(0b110);
        IOMUXC_SetPinMux(self->muxRegister, PIN_AF_MODE_ALT5, 0, 0, self->configRegister, 1U);  // Software Input On Field: Input Path is determined by functionality
        IOMUXC_SetPinConfig(self->muxRegister, PIN_AF_MODE_ALT5, 0, 0, self->configRegister, pad_config);
    }
}

STATIC mp_obj_t machine_pin_obj_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;

    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
    } else {
        mp_hal_pin_write(self, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

STATIC mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        [PIN_INIT_ARG_MODE] { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        [PIN_INIT_ARG_PULL] { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        [PIN_INIT_ARG_VALUE] { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        [PIN_INIT_ARG_DRIVE] { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PIN_DRIVE_POWER_3}},
        // TODO: Implement additional arguments
        /*
        { MP_QSTR_af, MP_ARG_INT, {.u_int = -1}}, // legacy
        { MP_QSTR_alt, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},*/
    };

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get io mode
    uint mode = args[PIN_INIT_ARG_MODE].u_int;
    if (!IS_GPIO_MODE(mode)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin mode: %d"), mode);
    }

    // Handle configuration for GPIO
    if ((mode == PIN_MODE_IN) || (mode == PIN_MODE_OUT) || (mode == PIN_MODE_OPEN_DRAIN)) {
        gpio_pin_config_t pin_config;
        const machine_pin_af_obj_t *af_obj;
        uint32_t pad_config = 0UL;
        uint8_t pull = PIN_PULL_DISABLED;

        // Generate pin configuration
        if ((args[PIN_INIT_ARG_VALUE].u_obj != MP_OBJ_NULL) && (mp_obj_is_true(args[PIN_INIT_ARG_VALUE].u_obj))) {
            pin_config.outputLogic = 1U;
        } else {
            pin_config.outputLogic = 0U;
        }
        pin_config.direction = mode == PIN_MODE_IN ? kGPIO_DigitalInput : kGPIO_DigitalOutput;
        pin_config.interruptMode = kGPIO_NoIntmode;

        af_obj = pin_find_af(self, PIN_AF_MODE_ALT5);  // GPIO is always ALT5
        if (af_obj == NULL) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("requested AF %d not available for pin %d"), PIN_AF_MODE_ALT5, mode);
        }

        // Generate pad configuration
        if (args[PIN_INIT_ARG_PULL].u_obj != mp_const_none) {
            pull = (uint8_t)mp_obj_get_int(args[PIN_INIT_ARG_PULL].u_obj);
        }

        pad_config |= IOMUXC_SW_PAD_CTL_PAD_SRE(0U);  // Slow Slew Rate
        pad_config |= IOMUXC_SW_PAD_CTL_PAD_SPEED(0b01);  // medium(100MHz)

        if (mode == PIN_MODE_OPEN_DRAIN) {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_ODE(0b1);  // Open Drain Enabled
        } else {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_ODE(0b0);  // Open Drain Disabled
        }

        if (pull == PIN_PULL_DISABLED) {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_PKE(0); // Pull/Keeper Disabled
        } else if (pull == PIN_PULL_HOLD) {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_PKE(1) | // Pull/Keeper Enabled
                IOMUXC_SW_PAD_CTL_PAD_PUE(0);            // Keeper selected
        } else {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_PKE(1) |  // Pull/Keeper Enabled
                IOMUXC_SW_PAD_CTL_PAD_PUE(1) |            // Pull selected
                IOMUXC_SW_PAD_CTL_PAD_PUS(pull);
        }

        if (mode == PIN_MODE_IN) {
            pad_config |= IOMUXC_SW_PAD_CTL_PAD_DSE(0b000) |  // output driver disabled
                IOMUXC_SW_PAD_CTL_PAD_HYS(1U);  // Hysteresis enabled
        } else {
            uint drive = args[PIN_INIT_ARG_DRIVE].u_int;
            if (!IS_GPIO_DRIVE(drive)) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid drive strength: %d"), drive);
            }

            pad_config |= IOMUXC_SW_PAD_CTL_PAD_DSE(drive) |
                IOMUXC_SW_PAD_CTL_PAD_HYS(0U);  // Hysteresis disabled
        }

        // Configure PAD as GPIO
        IOMUXC_SetPinMux(self->muxRegister, af_obj->af_mode, 0, 0, self->configRegister, 1U);  // Software Input On Field: Input Path is determined by functionality
        IOMUXC_SetPinConfig(self->muxRegister, af_obj->af_mode, 0, 0, self->configRegister, pad_config);
        GPIO_PinInit(self->gpio, self->pin, &pin_config);
    }

    return mp_const_none;
}

STATIC void machine_pin_obj_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    (void)kind;
    const machine_pin_obj_t *self = MP_OBJ_TO_PTR(o);
    mp_printf(print, "Pin(%s)", qstr_str(self->name));
}

// pin(id, mode, pull, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    const machine_pin_obj_t *pin = pin_find(args[0]);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

// pin.off()
STATIC mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    mp_hal_pin_low(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

// pin.on()
STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    mp_hal_pin_high(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

// pin.value([value])
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_obj_call(args[0], (n_args - 1), 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// pin.init(mode, pull, [kwargs])
STATIC mp_obj_t machine_pin_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_init);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_low),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),    MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_pin_init_obj) },
    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board),   MP_ROM_PTR(&machine_pin_board_pins_obj_type) },
    { MP_ROM_QSTR(MP_QSTR_cpu),     MP_ROM_PTR(&machine_pin_cpu_pins_obj_type) },
    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN),      MP_ROM_INT(PIN_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT),     MP_ROM_INT(PIN_MODE_OUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(PIN_MODE_OPEN_DRAIN) },

    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(PIN_PULL_UP_100K) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP_47K), MP_ROM_INT(PIN_PULL_UP_47K) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP_22K), MP_ROM_INT(PIN_PULL_UP_22K) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(PIN_PULL_DOWN_100K) },
    { MP_ROM_QSTR(MP_QSTR_PULL_HOLD), MP_ROM_INT(PIN_PULL_HOLD) },

    { MP_ROM_QSTR(MP_QSTR_DRIVER_OFF), MP_ROM_INT(PIN_DRIVE_OFF) },
    { MP_ROM_QSTR(MP_QSTR_POWER_0),    MP_ROM_INT(PIN_DRIVE_POWER_0) }, // R0 (150 Ohm @3.3V / 260 Ohm @ 1.8V)
    { MP_ROM_QSTR(MP_QSTR_POWER_1),    MP_ROM_INT(PIN_DRIVE_POWER_1) }, // R0/2
    { MP_ROM_QSTR(MP_QSTR_POWER_2),    MP_ROM_INT(PIN_DRIVE_POWER_2) }, // R0/3
    { MP_ROM_QSTR(MP_QSTR_POWER_3),    MP_ROM_INT(PIN_DRIVE_POWER_3) }, // R0/4
    { MP_ROM_QSTR(MP_QSTR_POWER_4),    MP_ROM_INT(PIN_DRIVE_POWER_4) }, // R0/5
    { MP_ROM_QSTR(MP_QSTR_POWER_5),    MP_ROM_INT(PIN_DRIVE_POWER_5) }, // R0/6
    { MP_ROM_QSTR(MP_QSTR_POWER_6),    MP_ROM_INT(PIN_DRIVE_POWER_6) }, // R0/7

};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

const mp_obj_type_t machine_pin_type = {
    {&mp_type_type},
    .name = MP_QSTR_Pin,
    .print = machine_pin_obj_print,
    .call = machine_pin_obj_call,
    .make_new = mp_pin_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_pin_locals_dict,
};

// FIXME: Create actual pin_af type!!!
const mp_obj_type_t machine_pin_af_type = {
    {&mp_type_type},
    .name = MP_QSTR_PinAF,
    .print = machine_pin_obj_print,
    .make_new = mp_pin_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_pin_locals_dict,
};
