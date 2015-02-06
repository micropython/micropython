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

#include "py/mpstate.h"
#include "mpconfig.h"
#include MICROPY_HAL_H
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "gpio.h"
#include "pybgpio.h"
#include "mpexception.h"


/// \moduleref pyb
/// \class GPIO - control I/O pins
///
/// A pin is the basic object to control I/O pins.  It has methods to set
/// the mode of the pin (input or output) and methods to get and set the
/// digital logic level.  For analog control of a pin, see the ADC class.
///
/// Usage Model:
///
/// All CPU Pins are predefined as pyb.GPIO.cpu.Name
///
///     GPIO9_pin = pyb.GPIO.cpu.GPIO9
///
///     g = pyb.GPIO(pyb.GPIO.cpu.GPIO9, 0, pyb.GPIO.IN)
///
/// CPU pins which correspond to the board pins are available
/// as `pyb.cpu.Name`.
///
/// You can also use strings:
///
///     g = pyb.GPIO('GPIO9', 0)
///
/// Users can add their own names:
///
///     MyMapperDict = { 'LeftMotorDir' : pyb.GPIO.cpu.GPIO11 }
///     pyb.GPIO.dict(MyMapperDict)
///     g = pyb.GPIO("LeftMotorDir", 0)
///
/// and can query mappings
///
///     g = pyb.GPIO("LeftMotorDir")
///
/// Users can also add their own mapping function:
///
///     def MyMapper(gpio_name):
///        if gpio_name == "LeftMotorDir":
///            return pyb.GPIO.cpu.GPIO11
///
///     pyb.GPIO.mapper(MyMapper)
///
/// So, if you were to call: `pyb.GPIO("LeftMotorDir", pyb.GPIO.OUT)`
/// then `"LeftMotorDir"` is passed directly to the mapper function.
///
/// To summarise, the following order determines how things get mapped into
/// an ordinal pin number:
///
/// 1. Directly specify a GPIO object
/// 2. User supplied mapping function
/// 3. User supplied mapping (object must be usable as a dictionary key)
/// 4. Supply a string which matches a CPU port/pin

void gpio_init0(void) {
    MP_STATE_PORT(gpio_class_mapper) = mp_const_none;
    MP_STATE_PORT(gpio_class_map_dict) = mp_const_none;
}

// C API used to convert a user-supplied pin name into an ordinal pin number.
const gpio_obj_t *gpio_find(mp_obj_t user_obj) {
    const gpio_obj_t *gpio_obj;

    // If a pin was provided, then use it
    if (MP_OBJ_IS_TYPE(user_obj, &gpio_type)) {
        gpio_obj = user_obj;
        return gpio_obj;
    }

    if (MP_STATE_PORT(gpio_class_mapper) != mp_const_none) {
        gpio_obj = mp_call_function_1(MP_STATE_PORT(gpio_class_mapper), user_obj);
        if (gpio_obj != mp_const_none) {
            if (!MP_OBJ_IS_TYPE(gpio_obj, &gpio_type)) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_resource_not_avaliable));
            }
            return gpio_obj;
        }
        // The pin mapping function returned mp_const_none, fall through to
        // other lookup methods.
    }

    if (MP_STATE_PORT(gpio_class_map_dict) != mp_const_none) {
        mp_map_t *gpio_map_map = mp_obj_dict_get_map(MP_STATE_PORT(gpio_class_map_dict));
        mp_map_elem_t *elem = mp_map_lookup(gpio_map_map, user_obj, MP_MAP_LOOKUP);
        if (elem != NULL && elem->value != NULL) {
            gpio_obj = elem->value;
            return gpio_obj;
        }
    }

    // See if the pin name matches a cpu pin
    gpio_obj = gpio_find_named_pin(&gpio_cpu_pins_locals_dict, user_obj);
    if (gpio_obj) {
        return gpio_obj;
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
}

void gpio_config(const gpio_obj_t *self, uint af, uint mode, uint type, uint strength) {
    // PIN_MODE_0 means it stays as a GPIO, else, another peripheral will take control of it
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

    // now set the alternate function, strenght and type
    MAP_PinModeSet (self->pin_num, af);
    MAP_PinConfigSet(self->pin_num, strength, type);
}

/// \method print()
/// Return a string describing the pin object.
STATIC void gpio_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    gpio_obj_t *self = self_in;
    uint32_t af = MAP_PinModeGet(self->pin_num);
    uint32_t type = gpio_get_type(self);
    uint32_t strength = gpio_get_strenght(self);

    // pin name
    print(env, "GPIO(GPIO.cpu.%s, af=%u", qstr_str(self->name), af);

    if (af == PIN_MODE_0) {
        // IO mode
        qstr mode_qst;
        uint32_t mode = gpio_get_mode(self);
        if (mode == GPIO_DIR_MODE_IN) {
            mode_qst = MP_QSTR_IN;
        } else {
            mode_qst = MP_QSTR_OUT;
        }
        print(env, ", mode=GPIO.%s", qstr_str(mode_qst)); // safe because mode_qst has no formatting chars
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
    print(env, ", pull=GPIO.%s", qstr_str(type_qst));

    // Strength
    qstr str_qst;
    if (strength == PIN_STRENGTH_2MA) {
        str_qst = MP_QSTR_S2MA;
    } else if (strength == PIN_STRENGTH_4MA) {
        str_qst = MP_QSTR_S4MA;
    } else {
        str_qst = MP_QSTR_S6MA;
    }
    print(env, ", strength=GPIO.%s)", qstr_str(str_qst));
}

STATIC mp_obj_t gpio_obj_init_helper(const gpio_obj_t *pin, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args);

/// \classmethod \constructor(id, ...)
/// Create a new GPIO object associated with the id.  If additional arguments are given,
/// they are used to initialise the pin.  See `init`.
STATIC mp_obj_t gpio_make_new(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Run an argument through the mapper and return the result.
    const gpio_obj_t *pin = gpio_find(args[0]);

    if (n_args > 1) {
        // pin af given, so configure it
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        gpio_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

/// \classmethod mapper([fun])
/// Get or set the pin mapper function.
STATIC mp_obj_t gpio_mapper(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        MP_STATE_PORT(gpio_class_mapper) = args[1];
        return mp_const_none;
    }
    return MP_STATE_PORT(gpio_class_mapper);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gpio_mapper_fun_obj, 1, 2, gpio_mapper);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(gpio_mapper_obj, (mp_obj_t)&gpio_mapper_fun_obj);

/// \classmethod dict([dict])
/// Get or set the pin mapper dictionary.
STATIC mp_obj_t gpio_map_dict(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        MP_STATE_PORT(gpio_class_map_dict) = args[1];
        return mp_const_none;
    }
    return MP_STATE_PORT(gpio_class_map_dict);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gpio_map_dict_fun_obj, 1, 2, gpio_map_dict);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(gpio_map_dict_obj, (mp_obj_t)&gpio_map_dict_fun_obj);

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
///     - `Pin.2MA`    - 2ma drive strength;
///     - `Pin.4MA`    - 4ma drive strength;
///     - `Pin.6MA`    - 6ma drive strength;
///
/// Returns: `None`.
STATIC const mp_arg_t gpio_init_args[] = {
    { MP_QSTR_af,   MP_ARG_REQUIRED | MP_ARG_INT  },
    { MP_QSTR_mode,                   MP_ARG_INT, {.u_int = GPIO_DIR_MODE_OUT} },
    { MP_QSTR_type,                   MP_ARG_INT, {.u_int = PIN_TYPE_STD} },
    { MP_QSTR_str,                    MP_ARG_INT, {.u_int = PIN_STRENGTH_4MA} },
};
#define gpio_INIT_NUM_ARGS MP_ARRAY_SIZE(gpio_init_args)

STATIC mp_obj_t gpio_obj_init_helper(const gpio_obj_t *self, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[gpio_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, gpio_INIT_NUM_ARGS, gpio_init_args, vals);

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
    gpio_config (self, af, mode, type, strength);

    return mp_const_none;
}

STATIC mp_obj_t gpio_obj_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return gpio_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(gpio_init_obj, 1, gpio_obj_init);

/// \method value([value])
/// Get or set the digital logic level of the pin:
///
///   - With no argument, return 0 or 1 depending on the logic level of the pin.
///   - With `value` given, set the logic level of the pin.  `value` can be
///   anything that converts to a boolean.  If it converts to `True`, the pin
///   is set high, otherwise it is set low.
STATIC mp_obj_t gpio_value(mp_uint_t n_args, const mp_obj_t *args) {
    gpio_obj_t *self = args[0];
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
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gpio_value_obj, 1, 2, gpio_value);

/// \method low()
/// Set the pin to a low logic level.
STATIC mp_obj_t gpio_low(mp_obj_t self_in) {
    gpio_obj_t *self = self_in;
    MAP_GPIOPinWrite(self->port, self->bit, 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_low_obj, gpio_low);

/// \method high()
/// Set the pin to a high logic level.
STATIC mp_obj_t gpio_high(mp_obj_t self_in) {
    gpio_obj_t *self = self_in;
    MAP_GPIOPinWrite(self->port, self->bit, self->bit);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_high_obj, gpio_high);

/// \method toggle()
/// Toggles the value of the pin
STATIC mp_obj_t gpio_toggle(mp_obj_t self_in) {
    gpio_obj_t *self = self_in;
    MAP_GPIOPinWrite(self->port, self->bit, ~MAP_GPIOPinRead(self->port, self->bit));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_toggle_obj, gpio_toggle);

/// \method name()
/// Get the pin name.
STATIC mp_obj_t gpio_name(mp_obj_t self_in) {
    gpio_obj_t *self = self_in;
    return MP_OBJ_NEW_QSTR(self->name);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_name_obj, gpio_name);

/// \method name()
/// Returns the cpu name for this pin.
STATIC mp_obj_t gpio_cpu_name(mp_obj_t self_in) {
    gpio_obj_t *self = self_in;
    mp_obj_t result = mp_obj_new_list(0, NULL);
    mp_obj_list_append(result, MP_OBJ_NEW_QSTR(self->name));

    mp_map_t *map = mp_obj_dict_get_map((mp_obj_t)&gpio_cpu_pins_locals_dict);
    mp_map_elem_t *elem = map->table;

    for (mp_uint_t i = 0; i < map->used; i++, elem++) {
        if (elem->value == self) {
            mp_obj_list_append(result, elem->key);
        }
    }
    return result;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_cpu_name_obj, gpio_cpu_name);

/// \method port()
/// Get the pin port.
STATIC mp_obj_t gpio_port(mp_obj_t self_in) {
    gpio_obj_t *self = self_in;
    return mp_obj_new_int(self->port);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_port_obj, gpio_port);

/// \method pin()
/// Get the pin number.
STATIC mp_obj_t gpio_pin(mp_obj_t self_in) {
    gpio_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(self->pin_num);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_pin_obj, gpio_pin);

/// \method mode()
/// Returns the currently configured mode of the gpio pin. The integer returned
/// will match one of the allowed constants for the mode argument to the init
/// function.
STATIC mp_obj_t gpio_mode(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(gpio_get_mode(self_in));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_mode_obj, gpio_mode);

/// \method type()
/// Returns the currently configured type of the pin. The integer returned
/// will match one of the allowed constants for the type argument to the init
/// function.
STATIC mp_obj_t gpio_type_get(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(gpio_get_type(self_in));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_type_obj, gpio_type_get);

/// \method strength()
/// Returns the currently configured drive strength of the pin. The integer returned
/// will match one of the allowed constants for the strength argument to the init
/// function.
STATIC mp_obj_t gpio_strength(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(gpio_get_strenght(self_in));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_strenght_obj, gpio_strength);

/// \method af()
/// Returns the currently configured alternate function of the gpio pin. The integer returned
/// will match one of the allowed constants for the af argument to the init function.
STATIC mp_obj_t gpio_af(mp_obj_t self_in) {
    gpio_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(MAP_PinModeGet(self->pin_num));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_af_obj, gpio_af);

STATIC const mp_map_elem_t gpio_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),        (mp_obj_t)&gpio_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_value),       (mp_obj_t)&gpio_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_low),         (mp_obj_t)&gpio_low_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_high),        (mp_obj_t)&gpio_high_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_toggle),      (mp_obj_t)&gpio_toggle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_name),        (mp_obj_t)&gpio_name_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_names),       (mp_obj_t)&gpio_cpu_name_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_port),        (mp_obj_t)&gpio_port_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin),         (mp_obj_t)&gpio_pin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mode),        (mp_obj_t)&gpio_mode_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_type),        (mp_obj_t)&gpio_type_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_strength),    (mp_obj_t)&gpio_strenght_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_af),          (mp_obj_t)&gpio_af_obj },

    // class methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_mapper),      (mp_obj_t)&gpio_mapper_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_dict),        (mp_obj_t)&gpio_map_dict_obj },

    // class attributes
    { MP_OBJ_NEW_QSTR(MP_QSTR_cpu),         (mp_obj_t)&gpio_cpu_pins_obj_type },

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

STATIC MP_DEFINE_CONST_DICT(gpio_locals_dict, gpio_locals_dict_table);

const mp_obj_type_t gpio_type = {
    { &mp_type_type },
    .name = MP_QSTR_GPIO,
    .print = gpio_print,
    .make_new = gpio_make_new,
    .locals_dict = (mp_obj_t)&gpio_locals_dict,
};
