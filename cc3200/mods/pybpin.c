/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "py/runtime.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "gpio.h"
#include "pybpin.h"
#include "mpexception.h"


/// \moduleref pyb
/// \class Pin - control I/O pins
///
/// A pin is the basic object to control I/O pins.  It has methods to set
/// the mode of the pin (input or output) and methods to get and set the
/// digital logic level.  For analog control of a pin, see the ADC class.
///
/// Usage Model:
///
/// All CPU Pins are predefined as pyb.Pin.cpu.Name
///
///     GPIO9_pin = pyb.Pin.cpu.GPIO9
///
///     g = pyb.Pin(pyb.Pin.cpu.GPIO9, 0, pyb.Pin.IN)
///
/// CPU pins which correspond to the board pins are available
/// as `pyb.cpu.Name`.
///
/// You can also use strings:
///
///     g = pyb.Pin('GPIO9', 0)
///
/// And finally, you can also pass a pin number directly:
///
///     g = pyb.Pin(64, 0)
///
/// To summarise, the following order determines how things get mapped into
/// an ordinal pin number:
///
/// 1. Directly specify a Pin object
/// 2. Supply a string which matches a CPU pin name
/// 3. Provide a pin number

void pin_init0(void) {
}

// C API used to convert a user-supplied pin name into an ordinal pin number.
const pin_obj_t *pin_find(mp_obj_t user_obj) {
    const pin_obj_t *pin_obj;

    // If a pin was provided, then use it
    if (MP_OBJ_IS_TYPE(user_obj, &pin_type)) {
        pin_obj = user_obj;
        return pin_obj;
    }

    // See if the pin name matches a cpu pin
    pin_obj = pin_find_named_pin(&pin_cpu_pins_locals_dict, user_obj);
    if (pin_obj) {
        return pin_obj;
    }

    // See if the pin number matches a cpu pin
    mp_int_t pin_num;
    if (mp_obj_get_int_maybe(user_obj, &pin_num)) {
        // The Pins dictionary has pin indexes, so we must substract one from the value passed
        pin_obj = pin_find_pin(&pin_cpu_pins_locals_dict, (pin_num - 1));
        if (pin_obj) {
            return pin_obj;
        }
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
}

void pin_verify_af (uint af) {
    if (af > PIN_MODE_15) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
}

void pin_config(const pin_obj_t *self, uint af, uint mode, uint type, uint strength) {
    // Skip all this if the pin is to be used in analog mode
    if (type != PIN_TYPE_ANALOG) {
        // PIN_MODE_0 means it stays as a Pin, else, another peripheral will take control of it
        if (af == PIN_MODE_0) {
            // enable the peripheral clock for the GPIO port of this pin
            switch (self->port) {
            case PORT_A0:
                MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
                break;
            case PORT_A1:
                MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
                break;
            case PORT_A2:
                MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
                break;
            case PORT_A3:
                MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
                break;
            default:
                break;
            }
            // configure the direction
            MAP_GPIODirModeSet(self->port, self->bit, mode);
        }
        // verify the alternate function
        pin_verify_af (af);
        // now set the alternate function, strenght and type
        MAP_PinModeSet (self->pin_num, af);
    }
    MAP_PinConfigSet(self->pin_num, strength, type);
}

/// \method print()
/// Return a string describing the pin object.
STATIC void pin_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_obj_t *self = self_in;
    uint32_t af = MAP_PinModeGet(self->pin_num);
    uint32_t type = pin_get_type(self);
    uint32_t strength = pin_get_strenght(self);

    // pin name
    print(env, "Pin(Pin.cpu.%s, af=%u", qstr_str(self->name), af);

    if (af == PIN_MODE_0) {
        // IO mode
        qstr mode_qst;
        uint32_t mode = pin_get_mode(self);
        if (mode == GPIO_DIR_MODE_IN) {
            mode_qst = MP_QSTR_IN;
        } else {
            mode_qst = MP_QSTR_OUT;
        }
        print(env, ", mode=Pin.%s", qstr_str(mode_qst)); // safe because mode_qst has no formatting chars
    }

    // pin type
    qstr type_qst;
    if (type == PIN_TYPE_STD) {
        type_qst = MP_QSTR_STD;
    } else if (type == PIN_TYPE_STD_PU) {
        type_qst = MP_QSTR_STD_PU;
    } else if (type == PIN_TYPE_STD_PD) {
        type_qst = MP_QSTR_STD_PD;
    } else if (type == PIN_TYPE_OD) {
        type_qst = MP_QSTR_OD;
    } else if (type == PIN_TYPE_OD_PU) {
        type_qst = MP_QSTR_OD_PU;
    } else {
        type_qst = MP_QSTR_OD_PD;
    }
    print(env, ", pull=Pin.%s", qstr_str(type_qst));

    // Strength
    qstr str_qst;
    if (strength == PIN_STRENGTH_2MA) {
        str_qst = MP_QSTR_S2MA;
    } else if (strength == PIN_STRENGTH_4MA) {
        str_qst = MP_QSTR_S4MA;
    } else {
        str_qst = MP_QSTR_S6MA;
    }
    print(env, ", strength=Pin.%s)", qstr_str(str_qst));
}

STATIC mp_obj_t pin_obj_init_helper(const pin_obj_t *pin, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args);

/// \classmethod \constructor(id, ...)
/// Create a new Pin object associated with the id.  If additional arguments are given,
/// they are used to initialise the pin.  See `init`.
STATIC mp_obj_t pin_make_new(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Run an argument through the mapper and return the result.
    const pin_obj_t *pin = pin_find(args[0]);

    if (n_args > 1) {
        // pin af given, so configure it
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

/// \method init(mode, pull=Pin.PULL_NONE, af=-1)
/// Initialise the pin:
///
///   - `af` can be in range 0-15, please check the CC3200 datasheet
///     for the details on the AFs availables on each pin (af=0, keeps it as a gpio pin).
///   - `mode` can be one of:
///     - `Pin.IN`     - configure the pin for input;
///     - `Pin.OUT`    - configure the pin for output;
///   - `type` can be one of:
///     - `Pin.STD`    - standard without pull-up or pull-down;
///     - `Pin.STD_PU` - standard with pull-up resistor;
///     - `Pin.STD_PD` - standard with pull-down resistor.
///     - `Pin.OD`     - standard without pull up or pull down;
///     - `Pin.OD_PU`  - open drain with pull-up resistor;
///     - `Pin.OD_PD`  - open drain with pull-down resistor.
///     - `Pin.ANALOG` - configured in analog (adc) mode
///   - `strength` can be one of:
///     - `Pin.S2MA`    - 2ma drive strength;
///     - `Pin.S4MA`    - 4ma drive strength;
///     - `Pin.S6MA`    - 6ma drive strength;
///
/// Returns: `None`.
STATIC const mp_arg_t pin_init_args[] = {
    { MP_QSTR_af,   MP_ARG_REQUIRED | MP_ARG_INT  },
    { MP_QSTR_mode,                   MP_ARG_INT, {.u_int = GPIO_DIR_MODE_OUT} },
    { MP_QSTR_type,                   MP_ARG_INT, {.u_int = PIN_TYPE_STD} },
    { MP_QSTR_str,                    MP_ARG_INT, {.u_int = PIN_STRENGTH_4MA} },
};
#define pin_INIT_NUM_ARGS MP_ARRAY_SIZE(pin_init_args)

STATIC mp_obj_t pin_obj_init_helper(const pin_obj_t *self, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[pin_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, pin_INIT_NUM_ARGS, pin_init_args, vals);

    // get the af
    uint af = vals[0].u_int;
    if (af < PIN_MODE_0 || af > PIN_MODE_15) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
    // get the io mode
    uint mode = vals[1].u_int;
    if (mode != GPIO_DIR_MODE_IN && mode != GPIO_DIR_MODE_OUT) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
    // get the type
    uint type = vals[2].u_int;
    if (type != PIN_TYPE_STD && type != PIN_TYPE_STD_PU && type != PIN_TYPE_STD_PD &&
        type != PIN_TYPE_OD  && type != PIN_TYPE_OD_PU  && type != PIN_TYPE_OD_PD) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
    // get the strenght
    uint strength = vals[3].u_int;
    if (strength != PIN_STRENGTH_2MA && strength != PIN_STRENGTH_4MA && strength != PIN_STRENGTH_6MA) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    // configure the pin as requested
    pin_config (self, af, mode, type, strength);

    return mp_const_none;
}

STATIC mp_obj_t pin_obj_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pin_init_obj, 1, pin_obj_init);

/// \method value([value])
/// Get or set the digital logic level of the pin:
///
///   - With no argument, return 0 or 1 depending on the logic level of the pin.
///   - With `value` given, set the logic level of the pin.  `value` can be
///   anything that converts to a boolean.  If it converts to `True`, the pin
///   is set high, otherwise it is set low.
STATIC mp_obj_t pin_value(mp_uint_t n_args, const mp_obj_t *args) {
    pin_obj_t *self = args[0];
    if (n_args == 1) {
        // get the pin value
        return MP_OBJ_NEW_SMALL_INT(MAP_GPIOPinRead(self->port, self->bit) ? 1 : 0);
    } else {
        // set the pin value
        if (mp_obj_is_true(args[1])) {
            MAP_GPIOPinWrite(self->port, self->bit, self->bit);
        } else {
            MAP_GPIOPinWrite(self->port, self->bit, 0);
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_value_obj, 1, 2, pin_value);

/// \method low()
/// Set the pin to a low logic level.
STATIC mp_obj_t pin_low(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    MAP_GPIOPinWrite(self->port, self->bit, 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_low_obj, pin_low);

/// \method high()
/// Set the pin to a high logic level.
STATIC mp_obj_t pin_high(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    MAP_GPIOPinWrite(self->port, self->bit, self->bit);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_high_obj, pin_high);

/// \method toggle()
/// Toggles the value of the pin
STATIC mp_obj_t pin_toggle(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    MAP_GPIOPinWrite(self->port, self->bit, ~MAP_GPIOPinRead(self->port, self->bit));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_toggle_obj, pin_toggle);

/// \method name()
/// Get the pin name.
STATIC mp_obj_t pin_name(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_QSTR(self->name);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_name_obj, pin_name);

/// \method port()
/// Get the pin port.
STATIC mp_obj_t pin_port(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return mp_obj_new_int(self->port);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_port_obj, pin_port);

/// \method pin()
/// Get the pin number.
STATIC mp_obj_t pin_pin(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(self->pin_num);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_pin_obj, pin_pin);

/// \method mode()
/// Returns the currently configured mode of the gpio pin. The integer returned
/// will match one of the allowed constants for the mode argument to the init
/// function.
STATIC mp_obj_t pin_mode(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(pin_get_mode(self_in));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_mode_obj, pin_mode);

/// \method type()
/// Returns the currently configured type of the pin. The integer returned
/// will match one of the allowed constants for the type argument to the init
/// function.
STATIC mp_obj_t pin_type_get(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(pin_get_type(self_in));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_type_obj, pin_type_get);

/// \method strength()
/// Returns the currently configured drive strength of the pin. The integer returned
/// will match one of the allowed constants for the strength argument to the init
/// function.
STATIC mp_obj_t pin_strength(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(pin_get_strenght(self_in));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_strenght_obj, pin_strength);

/// \method af()
/// Returns the currently configured alternate function of the gpio pin. The integer returned
/// will match one of the allowed constants for the af argument to the init function.
STATIC mp_obj_t pin_af(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(MAP_PinModeGet(self->pin_num));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_af_obj, pin_af);

STATIC const mp_map_elem_t pin_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),        (mp_obj_t)&pin_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_value),       (mp_obj_t)&pin_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_low),         (mp_obj_t)&pin_low_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_high),        (mp_obj_t)&pin_high_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_toggle),      (mp_obj_t)&pin_toggle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_name),        (mp_obj_t)&pin_name_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_port),        (mp_obj_t)&pin_port_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin),         (mp_obj_t)&pin_pin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mode),        (mp_obj_t)&pin_mode_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_type),        (mp_obj_t)&pin_type_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_strength),    (mp_obj_t)&pin_strenght_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_af),          (mp_obj_t)&pin_af_obj },

    // class attributes
    { MP_OBJ_NEW_QSTR(MP_QSTR_cpu),         (mp_obj_t)&pin_cpu_pins_obj_type },

    // class constants
    /// \constant IN        - set the pin to input mode
    /// \constant OUT       - set the pin to output mode
    /// \constant STD       - set the pin to standard mode without pull-up or pull-down
    /// \constant STD_PU    - set the pin to standard mode with pull-up
    /// \constant STD_PD    - set the pin to standard mode with pull-down
    /// \constant OD        - set the pin to open drain mode without pull-up or pull-down
    /// \constant OD_PU     - set the pin to open drain mode with pull-up
    /// \constant OD_PD     - set the pin to open drain mode with pull-down
    /// \constant 2MA       - set the drive strength to 2ma
    /// \constant 4MA       - set the drive strength to 4ma
    /// \constant 6MA       - set the drive strength to 6ma
    { MP_OBJ_NEW_QSTR(MP_QSTR_IN),          MP_OBJ_NEW_SMALL_INT(GPIO_DIR_MODE_IN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OUT),         MP_OBJ_NEW_SMALL_INT(GPIO_DIR_MODE_OUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STD),         MP_OBJ_NEW_SMALL_INT(PIN_TYPE_STD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STD_PU),      MP_OBJ_NEW_SMALL_INT(PIN_TYPE_STD_PU) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STD_PD),      MP_OBJ_NEW_SMALL_INT(PIN_TYPE_STD_PD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OD),          MP_OBJ_NEW_SMALL_INT(PIN_TYPE_OD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OD_PU),       MP_OBJ_NEW_SMALL_INT(PIN_TYPE_OD_PU) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OD_PD),       MP_OBJ_NEW_SMALL_INT(PIN_TYPE_OD_PD) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_S2MA),        MP_OBJ_NEW_SMALL_INT(PIN_STRENGTH_2MA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_S4MA),        MP_OBJ_NEW_SMALL_INT(PIN_STRENGTH_4MA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_S6MA),        MP_OBJ_NEW_SMALL_INT(PIN_STRENGTH_6MA) },
};

STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

const mp_obj_type_t pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pin_print,
    .make_new = pin_make_new,
    .locals_dict = (mp_obj_t)&pin_locals_dict,
};
