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
#include "py/gc.h"
#include "py/mpstate.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "gpio.h"
#include "interrupt.h"
#include "pybpin.h"
#include "pins.h"
#include "pybsleep.h"
#include "mpcallback.h"
#include "mpexception.h"
#include "mperror.h"


/// \moduleref pyb
/// \class Pin - control I/O pins
///
/// A pin is the basic object to control I/O pins.  It has methods to set
/// the mode of the pin (input or output) and methods to get and set the
/// digital logic level.  For analog control of a pin, see the ADC class.
///
/// Usage Model:
///
///     g = pyb.Pin('GPIO9', af=0, mode=pyb.Pin.IN, type=pyb.Pin.STD, strength=pyb.Pin.S2MA)
///
/// \Interrupts:
//// You can also configure the Pin to generate interrupts
///
/// Example callback:
///
///     def pincb(pin):
///         print(pin.get_config().name)
///
///     extint = pyb.Pin('GPIO10', 0, pyb.Pin.INT_RISING, pyb.GPIO.STD_PD, pyb.S2MA)
///     extint.callback (mode=pyb.Pin.INT_RISING, handler=pincb)
///     # the callback can be triggered manually
///     extint.callback()()
///     # to disable the callback
///     extint.callback().disable()
///
/// Now every time a falling edge is seen on the gpio pin, the callback will be
/// called. Caution: mechanical pushbuttons have "bounce" and pushing or
/// releasing a switch will often generate multiple edges.
/// See: http://www.eng.utah.edu/~cs5780/debouncing.pdf for a detailed
/// explanation, along with various techniques for debouncing.
///
/// All pin objects go through the pin mapper to come up with one of the
/// gpio pins.
///
/// There is also a C API, so that drivers which require Pin interrupts
/// can also use this code. See pybextint.h for the available functions.
/******************************************************************************
DECLARE PRIVATE FUNCTIONS
******************************************************************************/
STATIC void GPIOA0IntHandler (void);
STATIC void GPIOA1IntHandler (void);
STATIC void GPIOA2IntHandler (void);
STATIC void GPIOA3IntHandler (void);
STATIC void EXTI_Handler(uint port);
STATIC void pin_obj_configure (const pin_obj_t *self);
STATIC void pin_get_hibernate_pin_and_idx (const pin_obj_t *self, uint *wake_pin, uint *idx);
STATIC void pin_extint_enable (mp_obj_t self_in);
STATIC void pin_extint_disable (mp_obj_t self_in);

/******************************************************************************
DEFINE CONSTANTS
******************************************************************************/
#define PYBPIN_NUM_WAKE_PINS            (6)
#define PYBPIN_WAKES_NOT                (-1)

/******************************************************************************
DEFINE TYPES
******************************************************************************/
typedef struct {
    bool       active;
    int8_t     lpds;
    int8_t     hib;
} pybpin_wake_pin_t;

/******************************************************************************
DECLARE PRIVATE DATA
******************************************************************************/
STATIC const mp_cb_methods_t pin_cb_methods;
STATIC pybpin_wake_pin_t pybpin_wake_pin[PYBPIN_NUM_WAKE_PINS] =
                                    { {.active = false, .lpds = PYBPIN_WAKES_NOT, .hib = PYBPIN_WAKES_NOT},
                                      {.active = false, .lpds = PYBPIN_WAKES_NOT, .hib = PYBPIN_WAKES_NOT},
                                      {.active = false, .lpds = PYBPIN_WAKES_NOT, .hib = PYBPIN_WAKES_NOT},
                                      {.active = false, .lpds = PYBPIN_WAKES_NOT, .hib = PYBPIN_WAKES_NOT},
                                      {.active = false, .lpds = PYBPIN_WAKES_NOT, .hib = PYBPIN_WAKES_NOT},
                                      {.active = false, .lpds = PYBPIN_WAKES_NOT, .hib = PYBPIN_WAKES_NOT} } ;

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void pin_init0(void) {
    // assign GPIO10 and GPIO11 to the GPIO peripheral (the default is I2C), so that the I2C bus can
    // be assigned safely to any other pins (as recomended by the SDK release notes). Make them
    // inputs with pull-downs enabled to ensure they are not floating during LDPS and hibernate.
    pin_config ((pin_obj_t *)&pin_GPIO10, PIN_MODE_0, GPIO_DIR_MODE_IN, PIN_TYPE_STD_PD, PIN_STRENGTH_2MA);
    pin_config ((pin_obj_t *)&pin_GPIO11, PIN_MODE_0, GPIO_DIR_MODE_IN, PIN_TYPE_STD_PD, PIN_STRENGTH_2MA);
}

// C API used to convert a user-supplied pin name into an ordinal pin number.
pin_obj_t *pin_find(mp_obj_t user_obj) {
    pin_obj_t *pin_obj;

    // if a pin was provided, use it
    if (MP_OBJ_IS_TYPE(user_obj, &pin_type)) {
        pin_obj = user_obj;
        return pin_obj;
    }

    // otherwise see if the pin name matches a cpu pin
    pin_obj = pin_find_named_pin(&pin_cpu_pins_locals_dict, user_obj);
    if (pin_obj) {
        return pin_obj;
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
}

void pin_verify_af (uint af) {
    if (af > PIN_MODE_15) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
}

void pin_config (pin_obj_t *self, uint af, uint mode, uint type, uint strength) {
    // configure the pin in analog mode
    self->af = af, self->mode = mode, self->type = type, self->strength = strength;
    pin_obj_configure ((const pin_obj_t *)self);
    // mark the pin as used
    self->isused = true;
    // register it with the sleep module
    pybsleep_add ((const mp_obj_t)self, (WakeUpCB_t)pin_obj_configure);
}

void pin_extint_register(pin_obj_t *self, uint32_t intmode, uint32_t priority) {
    void *handler;
    uint32_t intnum;

    // configure the interrupt type
    MAP_GPIOIntTypeSet(self->port, self->bit, intmode);
    switch (self->port) {
    case GPIOA0_BASE:
        handler = GPIOA0IntHandler;
        intnum = INT_GPIOA0;
        break;
    case GPIOA1_BASE:
        handler = GPIOA1IntHandler;
        intnum = INT_GPIOA1;
        break;
    case GPIOA2_BASE:
        handler = GPIOA2IntHandler;
        intnum = INT_GPIOA2;
        break;
    case GPIOA3_BASE:
    default:
        handler = GPIOA3IntHandler;
        intnum = INT_GPIOA3;
        break;
    }
    MAP_GPIOIntRegister(self->port, handler);
    // set the interrupt to the lowest priority, to make sure that
    // no other ISRs will be preemted by this one
    MAP_IntPrioritySet(intnum, priority);
}

/******************************************************************************
DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void pin_obj_configure (const pin_obj_t *self) {
    // Skip all this if the pin is to be used in analog mode
    if (self->type != PYBPIN_ANALOG_TYPE) {
        // verify the alternate function
        pin_verify_af (self->af);
        // PIN_MODE_0 means it stays as a pin, else, another peripheral will take control of it
        if (self->af == PIN_MODE_0) {
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
            MAP_GPIODirModeSet(self->port, self->bit, self->mode);
        }
        // now set the alternate function, strenght and type
        MAP_PinModeSet (self->pin_num, self->af);
    }
    MAP_PinConfigSet(self->pin_num, self->strength, self->type);
}

STATIC void pin_get_hibernate_pin_and_idx (const pin_obj_t *self, uint *hib_pin, uint *idx) {
    // pin_num is actually : (package_pin - 1)
    switch (self->pin_num) {
    case 56:    // GPIO2
        *hib_pin = PRCM_HIB_GPIO2;
        *idx = 0;
        break;
    case 58:    // GPIO4
        *hib_pin = PRCM_HIB_GPIO4;
        *idx = 1;
        break;
    case 3:     // GPIO13
        *hib_pin = PRCM_HIB_GPIO13;
        *idx = 2;
        break;
    case 7:     // GPIO17
        *hib_pin = PRCM_HIB_GPIO17;
        *idx = 3;
        break;
    case 1:     // GPIO11
        *hib_pin = PRCM_HIB_GPIO11;
        *idx = 4;
        break;
    case 16:    // GPIO24
        *hib_pin = PRCM_HIB_GPIO24;
        *idx = 5;
        break;
    default:
        *idx = 0xFF;
        break;
    }
}

STATIC void pin_extint_enable (mp_obj_t self_in) {
    const pin_obj_t *self = self_in;
    uint hib_pin, idx;

    pin_get_hibernate_pin_and_idx (self, &hib_pin, &idx);
    if (idx < PYBPIN_NUM_WAKE_PINS) {
        if (pybpin_wake_pin[idx].lpds != PYBPIN_WAKES_NOT) {
            // enable GPIO as a wake source during LPDS
            MAP_PRCMLPDSWakeUpGPIOSelect(idx, pybpin_wake_pin[idx].lpds);
            MAP_PRCMLPDSWakeupSourceEnable(PRCM_LPDS_GPIO);
        }

        if (pybpin_wake_pin[idx].hib != PYBPIN_WAKES_NOT) {
            // enable GPIO as a wake source during hibernate
            MAP_PRCMHibernateWakeUpGPIOSelect(hib_pin, pybpin_wake_pin[idx].hib);
            MAP_PRCMHibernateWakeupSourceEnable(hib_pin);
        }
        else {
            MAP_PRCMHibernateWakeupSourceDisable(hib_pin);
        }
    }
    // if idx is invalid, the pin supports active interrupts for sure
    if (idx >= PYBPIN_NUM_WAKE_PINS || pybpin_wake_pin[idx].active) {
        MAP_GPIOIntClear(self->port, self->bit);
        MAP_GPIOIntEnable(self->port, self->bit);
    }
    // in case it was enabled before
    else if (idx < PYBPIN_NUM_WAKE_PINS && !pybpin_wake_pin[idx].active) {
        MAP_GPIOIntDisable(self->port, self->bit);
    }
}

STATIC void pin_extint_disable (mp_obj_t self_in) {
    const pin_obj_t *self = self_in;
    uint hib_pin, idx;

    pin_get_hibernate_pin_and_idx (self, &hib_pin, &idx);
    if (idx < PYBPIN_NUM_WAKE_PINS) {
        if (pybpin_wake_pin[idx].lpds != PYBPIN_WAKES_NOT) {
            // disable GPIO as a wake source during LPDS
            MAP_PRCMLPDSWakeupSourceDisable(PRCM_LPDS_GPIO);
        }
        if (pybpin_wake_pin[idx].hib != PYBPIN_WAKES_NOT) {
            // disable GPIO as a wake source during hibernate
            MAP_PRCMHibernateWakeupSourceDisable(hib_pin);
        }
    }
    // not need to check for the active flag, it's safe to disable it anyway
    MAP_GPIOIntDisable(self->port, self->bit);
}

/******************************************************************************/
// Micro Python bindings

/// \method init(mode, pull=Pin.PULL_NONE, af=-1)
/// Initialise the pin:
///
///   - `af` can be in range 0-15, please check the CC3200 datasheet
///          for the details on the AFs availables on each pin (af=0 keeps it as a gpio pin).
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
///   - `strength` can be one of:
///     - `Pin.S2MA`    - 2ma drive strength;
///     - `Pin.S4MA`    - 4ma drive strength;
///     - `Pin.S6MA`    - 6ma drive strength;
///
/// Returns: `None`.
STATIC const mp_arg_t pin_init_args[] = {
    { MP_QSTR_af,       MP_ARG_REQUIRED | MP_ARG_INT  },
    { MP_QSTR_mode,                       MP_ARG_INT, {.u_int = GPIO_DIR_MODE_OUT} },
    { MP_QSTR_type,                       MP_ARG_INT, {.u_int = PIN_TYPE_STD} },
    { MP_QSTR_strength,                   MP_ARG_INT, {.u_int = PIN_STRENGTH_4MA} },
};
#define pin_INIT_NUM_ARGS MP_ARRAY_SIZE(pin_init_args)

STATIC mp_obj_t pin_obj_init_helper(pin_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[pin_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, pos_args, kw_args, pin_INIT_NUM_ARGS, pin_init_args, args);

    // get the af
    uint af = args[0].u_int;
    if (af < PIN_MODE_0 || af > PIN_MODE_15) {
        goto invalid_args;
    }
    // get the io mode
    uint mode = args[1].u_int;
    // checking the mode only makes sense if af == GPIO
    if (af == PIN_MODE_0) {
        if (mode != GPIO_DIR_MODE_IN && mode != GPIO_DIR_MODE_OUT) {
            goto invalid_args;
        }
    }
    // get the type
    uint type = args[2].u_int;
    if (type != PIN_TYPE_STD && type != PIN_TYPE_STD_PU && type != PIN_TYPE_STD_PD &&
            type != PIN_TYPE_OD && type != PIN_TYPE_OD_PU && type != PIN_TYPE_OD_PD) {
        goto invalid_args;
    }
    // get the strenght
    uint strength = args[3].u_int;
    if (strength != PIN_STRENGTH_2MA && strength != PIN_STRENGTH_4MA && strength != PIN_STRENGTH_6MA) {
        goto invalid_args;
    }

    // configure the pin as requested
    pin_config (self, af, mode, type, strength);

    return mp_const_none;

invalid_args:
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
}

/// \method print()
/// Return a string describing the pin object.
STATIC void pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_obj_t *self = self_in;
    uint32_t af = self->af;
    uint32_t type = self->type;
    uint32_t strength = self->strength;

    // pin name
    mp_printf(print, "<Pin.cpu.%q, af=%u", self->name, af);

    // pin mode
    if (af == PIN_MODE_0) {
        // IO mode
        qstr mode_qst;
        uint32_t mode = self->mode;
        if (mode == GPIO_DIR_MODE_IN) {
            mode_qst = MP_QSTR_IN;
        } else {
            mode_qst = MP_QSTR_OUT;
        }
        mp_printf(print, ", mode=Pin.%q", mode_qst);
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
    mp_printf(print, ", type=Pin.%q", type_qst);

    // pin strength
    qstr str_qst;
    if (strength == PIN_STRENGTH_2MA) {
        str_qst = MP_QSTR_S2MA;
    } else if (strength == PIN_STRENGTH_4MA) {
        str_qst = MP_QSTR_S4MA;
    } else {
        str_qst = MP_QSTR_S6MA;
    }
    mp_printf(print, ", strength=Pin.%q>", str_qst);
}

/// \classmethod \constructor(id, ...)
/// Create a new Pin object associated with the id.  If additional arguments are given,
/// they are used to initialise the pin.  See `init`.
STATIC mp_obj_t pin_make_new(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Run an argument through the mapper and return the result.
    pin_obj_t *pin = (pin_obj_t *)pin_find(args[0]);

    if (n_args > 1 || n_kw > 0) {
        // pin af given, so configure it
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

STATIC mp_obj_t pin_obj_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pin_init_obj, 1, pin_obj_init);

/// \method value([value])
/// Get or set the digital logic level of the pin:
///
///   - With no arguments, return 0 or 1 depending on the logic level of the pin.
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

/// \method info()
/// Returns a named tupple with the current configuration of the gpio pin
STATIC mp_obj_t pin_info(mp_obj_t self_in) {
    STATIC const qstr pin_info_fields[] = {
        MP_QSTR_name, MP_QSTR_af, MP_QSTR_mode,
        MP_QSTR_type, MP_QSTR_strength
    };

    pin_obj_t *self = self_in;
    mp_obj_t pin_config[5];
    pin_config[0] = MP_OBJ_NEW_QSTR(self->name);
    pin_config[1] = mp_obj_new_int(self->af);
    pin_config[2] = mp_obj_new_int(self->mode);
    pin_config[3] = mp_obj_new_int(self->type);
    pin_config[4] = mp_obj_new_int(self->strength);

    return mp_obj_new_attrtuple(pin_info_fields, 5, pin_config);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_info_obj, pin_info);

/// \method callback(method, mode, priority, pwrmode)
/// Creates a callback object associated to a pin
/// min num of arguments is 1 (mode)
STATIC mp_obj_t pin_callback (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[mpcallback_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, mpcallback_INIT_NUM_ARGS, mpcallback_init_args, args);

    pin_obj_t *self = pos_args[0];
    // check if any parameters were passed
    mp_obj_t _callback = mpcallback_find(self);
    if (kw_args->used > 0 || !_callback) {
        // convert the priority to the correct value
        uint priority = mpcallback_translate_priority (args[2].u_int);
        // verify the interrupt mode
        uint intmode = args[0].u_int;
        if (intmode != GPIO_FALLING_EDGE && intmode != GPIO_RISING_EDGE && intmode != GPIO_BOTH_EDGES &&
            intmode != GPIO_LOW_LEVEL && intmode != GPIO_HIGH_LEVEL) {
            goto invalid_args;
        }

        uint pwrmode = args[4].u_int;
        if (pwrmode > (PYB_PWR_MODE_ACTIVE | PYB_PWR_MODE_LPDS | PYB_PWR_MODE_HIBERNATE)) {
            goto invalid_args;
        }

        // get the wake info from this pin
        uint hib_pin, idx;
        pin_get_hibernate_pin_and_idx ((const pin_obj_t *)self, &hib_pin, &idx);
        if (pwrmode & PYB_PWR_MODE_LPDS) {
            if (idx >= PYBPIN_NUM_WAKE_PINS) {
                goto invalid_args;
            }
            // wake modes are different in LDPS
            uint wake_mode;
            switch (intmode) {
            case GPIO_FALLING_EDGE:
                wake_mode = PRCM_LPDS_FALL_EDGE;
                break;
            case GPIO_RISING_EDGE:
                wake_mode = PRCM_LPDS_RISE_EDGE;
                break;
            case GPIO_LOW_LEVEL:
                wake_mode = PRCM_LPDS_LOW_LEVEL;
                break;
            case GPIO_HIGH_LEVEL:
                wake_mode = PRCM_LPDS_HIGH_LEVEL;
                break;
            default:
                goto invalid_args;
                break;
            }

            // first clear the lpds value from all wake-able pins
            for (uint i = 0; i < PYBPIN_NUM_WAKE_PINS; i++) {
                pybpin_wake_pin[i].lpds = PYBPIN_WAKES_NOT;
            }

            // enable this pin as a wake-up source during LPDS
            pybpin_wake_pin[idx].lpds = wake_mode;
        }
        else {
            // this pin was the previous LPDS wake source, so disable it completely
            if (pybpin_wake_pin[idx].lpds != PYBPIN_WAKES_NOT) {
                MAP_PRCMLPDSWakeupSourceDisable(PRCM_LPDS_GPIO);
            }
            pybpin_wake_pin[idx].lpds = PYBPIN_WAKES_NOT;
        }

        if (pwrmode & PYB_PWR_MODE_HIBERNATE) {
            if (idx >= PYBPIN_NUM_WAKE_PINS) {
                goto invalid_args;
            }
            // wake modes are different in hibernate
            uint wake_mode;
            switch (intmode) {
            case GPIO_FALLING_EDGE:
                wake_mode = PRCM_HIB_FALL_EDGE;
                break;
            case GPIO_RISING_EDGE:
                wake_mode = PRCM_HIB_RISE_EDGE;
                break;
            case GPIO_LOW_LEVEL:
                wake_mode = PRCM_HIB_LOW_LEVEL;
                break;
            case GPIO_HIGH_LEVEL:
                wake_mode = PRCM_HIB_HIGH_LEVEL;
                break;
            default:
                goto invalid_args;
                break;
            }

            // enable this pin as wake-up source during hibernate
            pybpin_wake_pin[idx].hib = wake_mode;
        }
        else {
            pybpin_wake_pin[idx].hib = PYBPIN_WAKES_NOT;
        }

        // we need to update the callback atomically, so we disable the
        // interrupt before we update anything.
        pin_extint_disable(self);
        if (pwrmode & PYB_PWR_MODE_ACTIVE) {
            // register the interrupt
            pin_extint_register((pin_obj_t *)self, intmode, priority);
            if (idx < PYBPIN_NUM_WAKE_PINS) {
                pybpin_wake_pin[idx].active = true;
            }
        }
        else if (idx < PYBPIN_NUM_WAKE_PINS) {
            pybpin_wake_pin[idx].active = false;
        }

        // all checks have passed, now we can create the callback
        _callback = mpcallback_new (self, args[1].u_obj, &pin_cb_methods);
        if (pwrmode & PYB_PWR_MODE_LPDS) {
            pybsleep_set_gpio_lpds_callback (_callback);
        }

        // enable the interrupt just before leaving
        pin_extint_enable(self);
    }
    return _callback;

invalid_args:
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pin_callback_obj, 1, pin_callback);

STATIC const mp_map_elem_t pin_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),                    (mp_obj_t)&pin_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_value),                   (mp_obj_t)&pin_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_low),                     (mp_obj_t)&pin_low_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_high),                    (mp_obj_t)&pin_high_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_toggle),                  (mp_obj_t)&pin_toggle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_info),                    (mp_obj_t)&pin_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback),                (mp_obj_t)&pin_callback_obj },

    // class constants
    /// \constant IN                                    - set the pin to input mode
    /// \constant OUT                                   - set the pin to output mode
    /// \constant STD                                   - set the pin to standard mode without pull-up or pull-down
    /// \constant STD_PU                                - set the pin to standard mode with pull-up
    /// \constant STD_PD                                - set the pin to standard mode with pull-down
    /// \constant OD                                    - set the pin to open drain mode without pull-up or pull-down
    /// \constant OD_PU                                 - set the pin to open drain mode with pull-up
    /// \constant OD_PD                                 - set the pin to open drain mode with pull-down
    /// \constant IRQ_RISING                            - interrupt on a rising edge
    /// \constant IRQ_FALLING                           - interrupt on a falling edge
    /// \constant IRQ_RISING_FALLING                    - interrupt on a rising or falling edge
    /// \constant IRQ_LOW_LEVEL                         - interrupt on a low level
    /// \constant IRQ_HIGH_LEVEL                        - interrupt on a high level
    /// \constant 2MA                                   - set the drive strength to 2ma
    /// \constant 4MA                                   - set the drive strength to 4ma
    /// \constant 6MA                                   - set the drive strength to 6ma
    { MP_OBJ_NEW_QSTR(MP_QSTR_IN),                      MP_OBJ_NEW_SMALL_INT(GPIO_DIR_MODE_IN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OUT),                     MP_OBJ_NEW_SMALL_INT(GPIO_DIR_MODE_OUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STD),                     MP_OBJ_NEW_SMALL_INT(PIN_TYPE_STD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STD_PU),                  MP_OBJ_NEW_SMALL_INT(PIN_TYPE_STD_PU) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_STD_PD),                  MP_OBJ_NEW_SMALL_INT(PIN_TYPE_STD_PD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OD),                      MP_OBJ_NEW_SMALL_INT(PIN_TYPE_OD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OD_PU),                   MP_OBJ_NEW_SMALL_INT(PIN_TYPE_OD_PU) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OD_PD),                   MP_OBJ_NEW_SMALL_INT(PIN_TYPE_OD_PD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_FALLING),             MP_OBJ_NEW_SMALL_INT(GPIO_FALLING_EDGE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_RISING),              MP_OBJ_NEW_SMALL_INT(GPIO_RISING_EDGE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_RISING_FALLING),      MP_OBJ_NEW_SMALL_INT(GPIO_BOTH_EDGES) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_LOW_LEVEL),           MP_OBJ_NEW_SMALL_INT(GPIO_LOW_LEVEL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_HIGH_LEVEL),          MP_OBJ_NEW_SMALL_INT(GPIO_HIGH_LEVEL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_S2MA),                    MP_OBJ_NEW_SMALL_INT(PIN_STRENGTH_2MA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_S4MA),                    MP_OBJ_NEW_SMALL_INT(PIN_STRENGTH_4MA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_S6MA),                    MP_OBJ_NEW_SMALL_INT(PIN_STRENGTH_6MA) },
};

STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

const mp_obj_type_t pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pin_print,
    .make_new = pin_make_new,
    .locals_dict = (mp_obj_t)&pin_locals_dict,
};

STATIC const mp_cb_methods_t pin_cb_methods = {
    .init = pin_callback,
    .enable = pin_extint_enable,
    .disable = pin_extint_disable,
};

STATIC void GPIOA0IntHandler (void) {
    EXTI_Handler(GPIOA0_BASE);
}

STATIC void GPIOA1IntHandler (void) {
    EXTI_Handler(GPIOA1_BASE);
}

STATIC void GPIOA2IntHandler (void) {
    EXTI_Handler(GPIOA2_BASE);
}

STATIC void GPIOA3IntHandler (void) {
    EXTI_Handler(GPIOA3_BASE);
}

// common interrupt handler
STATIC void EXTI_Handler(uint port) {
    uint32_t bits = MAP_GPIOIntStatus(port, true);
    MAP_GPIOIntClear(port, bits);

    // might be that we have more than one Pin interrupt pending
    // therefore we must loop through all of the 8 possible bits
    for (int i = 0; i < 8; i++) {
        uint32_t bit = (1 << i);
        if (bit & bits) {
            pin_obj_t *self = (pin_obj_t *)pin_find_pin_by_port_bit(&pin_cpu_pins_locals_dict, port, bit);
            mp_obj_t _callback = mpcallback_find(self);
            mpcallback_handler(_callback);
        }
    }
}

