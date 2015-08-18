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
///         print(pin.name())
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
STATIC void pin_obj_configure (const pin_obj_t *self);
STATIC void pin_get_hibernate_pin_and_idx (const pin_obj_t *self, uint *wake_pin, uint *idx);
STATIC void pin_extint_enable (mp_obj_t self_in);
STATIC void pin_extint_disable (mp_obj_t self_in);
STATIC void pin_extint_register(pin_obj_t *self, uint32_t intmode, uint32_t priority);
STATIC void pin_validate_mode (uint mode);
STATIC void pin_validate_pull (uint pull);
STATIC void pin_validate_drive (uint strength);
STATIC void GPIOA0IntHandler (void);
STATIC void GPIOA1IntHandler (void);
STATIC void GPIOA2IntHandler (void);
STATIC void GPIOA3IntHandler (void);
STATIC void EXTI_Handler(uint port);

/******************************************************************************
DEFINE CONSTANTS
******************************************************************************/
#define PYBPIN_NUM_WAKE_PINS            (6)
#define PYBPIN_WAKES_NOT                (-1)

#define GPIO_DIR_MODE_ALT               0x00000002  // Pin is NOT controlled by the PGIO module
#define GPIO_DIR_MODE_ALT_OD            0x00000003  // Pin is NOT controlled by the PGIO module and is in open drain mode

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
    // assign GP10 and GP11 to the GPIO peripheral (the default is I2C), so that the I2C bus can
    // be assigned safely to any other pins (as recomended by the SDK release notes). Make them
    // inputs with pull-downs enabled to ensure they are not floating during LDPS and hibernate.
    pin_config ((pin_obj_t *)&pin_GP10, PIN_MODE_0, GPIO_DIR_MODE_IN, PIN_TYPE_STD_PD, -1, PIN_STRENGTH_2MA);
    pin_config ((pin_obj_t *)&pin_GP11, PIN_MODE_0, GPIO_DIR_MODE_IN, PIN_TYPE_STD_PD, -1, PIN_STRENGTH_2MA);
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
    pin_obj = pin_find_named_pin(&pin_board_pins_locals_dict, user_obj);
    if (pin_obj) {
        return pin_obj;
    }

    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
}

void pin_config (pin_obj_t *self, int af, uint mode, uint pull, int value, uint strength) {
    self->mode = mode, self->pull = pull, self->strength = strength;
    // if af is -1, then we want to keep it as it is
    if (af != -1) {
        self->af = af;
    }
    // if value is -1, then we want to keep it as it is
    if (value != -1) {
        self->value = value;
    }
    pin_obj_configure ((const pin_obj_t *)self);
    // mark the pin as used
    self->isused = true;
    // register it with the sleep module
    pybsleep_add ((const mp_obj_t)self, (WakeUpCB_t)pin_obj_configure);
}

/******************************************************************************
DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void pin_obj_configure (const pin_obj_t *self) {
    uint32_t type;
    if (self->mode == PIN_TYPE_ANALOG) {
        type = PIN_TYPE_ANALOG;
    } else {
        type = self->pull;
        uint32_t direction = self->mode;
        if (direction == PIN_TYPE_OD || direction == GPIO_DIR_MODE_ALT_OD) {
            direction = GPIO_DIR_MODE_OUT;
            type |= PIN_TYPE_OD;
        }
        if (self->mode != GPIO_DIR_MODE_ALT && self->mode != GPIO_DIR_MODE_ALT_OD) {
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

            // set the pin value
            if (self->value) {
                MAP_GPIOPinWrite(self->port, self->bit, self->bit);
            } else {
                MAP_GPIOPinWrite(self->port, self->bit, 0);
            }

            // configure the direction
            MAP_GPIODirModeSet(self->port, self->bit, direction);
        }
        // now set the alternate function
        MAP_PinModeSet (self->pin_num, self->af);
    }
    MAP_PinConfigSet(self->pin_num, self->strength, type);
}

STATIC void pin_get_hibernate_pin_and_idx (const pin_obj_t *self, uint *hib_pin, uint *idx) {
    // pin_num is actually : (package_pin - 1)
    switch (self->pin_num) {
    case 56:    // GP2
        *hib_pin = PRCM_HIB_GPIO2;
        *idx = 0;
        break;
    case 58:    // GP4
        *hib_pin = PRCM_HIB_GPIO4;
        *idx = 1;
        break;
    case 3:     // GP13
        *hib_pin = PRCM_HIB_GPIO13;
        *idx = 2;
        break;
    case 7:     // GP17
        *hib_pin = PRCM_HIB_GPIO17;
        *idx = 3;
        break;
    case 1:     // GP11
        *hib_pin = PRCM_HIB_GPIO11;
        *idx = 4;
        break;
    case 16:    // GP24
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

STATIC void pin_extint_register(pin_obj_t *self, uint32_t intmode, uint32_t priority) {
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

STATIC void pin_validate_mode (uint mode) {
    if (mode != GPIO_DIR_MODE_IN && mode != GPIO_DIR_MODE_OUT && mode != PIN_TYPE_OD &&
        mode != GPIO_DIR_MODE_ALT && mode != GPIO_DIR_MODE_ALT_OD) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
}
STATIC void pin_validate_pull (uint pull) {
    if (pull != PIN_TYPE_STD && pull != PIN_TYPE_STD_PU && pull != PIN_TYPE_STD_PD) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
}

STATIC void pin_validate_drive(uint strength) {
    if (strength != PIN_STRENGTH_2MA && strength != PIN_STRENGTH_4MA && strength != PIN_STRENGTH_6MA) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
}

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
            pin_obj_t *self = (pin_obj_t *)pin_find_pin_by_port_bit(&pin_board_pins_locals_dict, port, bit);
            mp_obj_t _callback = mpcallback_find(self);
            mpcallback_handler(_callback);
        }
    }
}


/******************************************************************************/
// Micro Python bindings

/// \method init(mode, pull=Pin.PULL_NONE, af=-1)
/// Initialise the pin:
///
///   - `mode` can be one of:
///     - `Pin.IN`              - configure the pin for input
///     - `Pin.OUT`             - configure the pin for output
///     - `Pin.OPEN_DRAIN`      - open drain output
///   - `pull` can be one of:
///     - `Pin.PULL_UP`         - pull-up enabled
///     - `Pin.PULL_DOWN`       - pull-down enabled
///     - `Pin.PULL_NONE`       - no internal pull-up/down resistor
///   - `value` can take 1, 0, True or False to set the initial value of the pin
///   - `drive` can be one of:
///     - `Pin.LOW_POWER`       - 2ma drive strength
///     - `Pin.MED_POWER`       - 4ma drive strength
///     - `Pin.HIGH_POWER`      - 6ma drive strength
///
/// Returns: `None`.
STATIC const mp_arg_t pin_init_args[] = {
    { MP_QSTR_mode,     MP_ARG_REQUIRED |  MP_ARG_INT  },
    { MP_QSTR_pull,                        MP_ARG_INT, {.u_int = PIN_TYPE_STD} },
    { MP_QSTR_value,    MP_ARG_KW_ONLY  |  MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_drive,    MP_ARG_KW_ONLY  |  MP_ARG_INT, {.u_int = PIN_STRENGTH_4MA} },
};
#define pin_INIT_NUM_ARGS MP_ARRAY_SIZE(pin_init_args)

STATIC mp_obj_t pin_obj_init_helper(pin_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[pin_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, pos_args, kw_args, pin_INIT_NUM_ARGS, pin_init_args, args);

    // get the io mode
    uint mode = args[0].u_int;
    pin_validate_mode(mode);

    // get the pull type
    uint pull = args[1].u_int;
    pin_validate_pull(pull);

    // get the value
    int value = -1;
    if (args[2].u_obj != MP_OBJ_NULL) {
        if (mp_obj_is_true(args[2].u_obj)) {
            value = 1;
        } else {
            value = 0;
        }
    }

    // get the strenght
    uint strength = args[3].u_int;
    pin_validate_drive(strength);

    int af = (mode == GPIO_DIR_MODE_ALT || mode == GPIO_DIR_MODE_ALT_OD) ? -1 : PIN_MODE_0;

    // configure the pin as requested
    pin_config (self, af, mode, pull, value, strength);

    return mp_const_none;
}

/// \method print()
/// Return a string describing the pin object.
STATIC void pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_obj_t *self = self_in;
    uint32_t pull = self->pull;
    uint32_t drive = self->strength;

    // pin name
    mp_printf(print, "<Pin.board.%q", self->name);

    // pin mode
    qstr mode_qst;
    uint32_t mode = self->mode;
    if (mode == GPIO_DIR_MODE_IN) {
        mode_qst = MP_QSTR_IN;
    } else if (mode == GPIO_DIR_MODE_OUT) {
        mode_qst = MP_QSTR_OUT;
    } else if (mode == GPIO_DIR_MODE_ALT) {
        mode_qst = MP_QSTR_ALT;
    } else if (mode == GPIO_DIR_MODE_ALT_OD) {
        mode_qst = MP_QSTR_ALT_OPEN_DRAIN;
    } else {
        mode_qst = MP_QSTR_OPEN_DRAIN;
    }
    mp_printf(print, ", mode=Pin.%q", mode_qst);

    // pin pull
    qstr pull_qst;
    if (pull == PIN_TYPE_STD) {
        pull_qst = MP_QSTR_PULL_NONE;
    } else if (pull == PIN_TYPE_STD_PU) {
        pull_qst = MP_QSTR_PULL_UP;
    } else {
        pull_qst = MP_QSTR_PULL_DOWN;
    }
    mp_printf(print, ", pull=Pin.%q", pull_qst);

    // pin drive
    qstr drv_qst;
    if (drive == PIN_STRENGTH_2MA) {
        drv_qst = MP_QSTR_LOW_POWER;
    } else if (drive == PIN_STRENGTH_4MA) {
        drv_qst = MP_QSTR_MED_POWER;
    } else {
        drv_qst = MP_QSTR_HIGH_POWER;
    }
    mp_printf(print, ", drive=Pin.%q>", drv_qst);
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
            self->value = 1;
            MAP_GPIOPinWrite(self->port, self->bit, self->bit);
        } else {
            self->value = 0;
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

/// \method id()
/// Returns the qstr name of the pin
STATIC mp_obj_t pin_id(mp_obj_t self_in) {
    pin_obj_t *self = self_in;
    return MP_OBJ_NEW_QSTR(self->name);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_id_obj, pin_id);

/// \method mode()
/// Get or set the mode of the pin
STATIC mp_obj_t pin_mode(mp_uint_t n_args, const mp_obj_t *args) {
    pin_obj_t *self = args[0];
    if (n_args == 1) {
        return mp_obj_new_int(self->mode);
    } else {
        uint32_t mode = mp_obj_get_int(args[1]);
        pin_validate_mode (mode);
        self->mode = mode;
        pin_obj_configure(self);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_mode_obj, 1, 2, pin_mode);

/// \method pull()
/// Get or set the pull of the pin
STATIC mp_obj_t pin_pull(mp_uint_t n_args, const mp_obj_t *args) {
    pin_obj_t *self = args[0];
    if (n_args == 1) {
        return mp_obj_new_int(self->pull);
    } else {
        uint32_t pull = mp_obj_get_int(args[1]);
        pin_validate_pull (pull);
        self->pull = pull;
        pin_obj_configure(self);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_pull_obj, 1, 2, pin_pull);

/// \method drive()
/// Get or set the drive of the pin
STATIC mp_obj_t pin_drive(mp_uint_t n_args, const mp_obj_t *args) {
    pin_obj_t *self = args[0];
    if (n_args == 1) {
        return mp_obj_new_int(self->strength);
    } else {
        uint32_t strength = mp_obj_get_int(args[1]);
        pin_validate_drive (strength);
        self->strength = strength;
        pin_obj_configure(self);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_drive_obj, 1, 2, pin_drive);


/// \method callback(method, mode, priority, pwrmode)
/// Creates a callback object associated to a pin
/// min num of arguments is 1 (mode)
STATIC mp_obj_t pin_callback (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[mpcallback_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, mpcallback_INIT_NUM_ARGS, mpcallback_init_args, args);

    pin_obj_t *self = pos_args[0];
    // check if any parameters were passed
    mp_obj_t _callback = mpcallback_find(self);
    if (kw_args->used > 0) {
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
        _callback = mpcallback_new (self, args[1].u_obj, &pin_cb_methods, true);
        if (pwrmode & PYB_PWR_MODE_LPDS) {
            pybsleep_set_gpio_lpds_callback (_callback);
        }

        // enable the interrupt just before leaving
        pin_extint_enable(self);
    } else if (!_callback) {
        _callback = mpcallback_new (self, mp_const_none, &pin_cb_methods, false);
    }
    return _callback;

invalid_args:
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pin_callback_obj, 1, pin_callback);

/// \method \call()
/// Get or set the value of the pin
STATIC mp_obj_t pin_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_t _args[2] = {self_in, *args};
    return pin_value (n_args + 1, _args);
}

STATIC const mp_map_elem_t pin_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),                    (mp_obj_t)&pin_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_value),                   (mp_obj_t)&pin_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_low),                     (mp_obj_t)&pin_low_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_high),                    (mp_obj_t)&pin_high_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_toggle),                  (mp_obj_t)&pin_toggle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_id),                      (mp_obj_t)&pin_id_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mode),                    (mp_obj_t)&pin_mode_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pull),                    (mp_obj_t)&pin_pull_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_drive),                   (mp_obj_t)&pin_drive_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback),                (mp_obj_t)&pin_callback_obj },

    // class attributes
    { MP_OBJ_NEW_QSTR(MP_QSTR_board),                   (mp_obj_t)&pin_board_pins_obj_type },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_IN),                      MP_OBJ_NEW_SMALL_INT(GPIO_DIR_MODE_IN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OUT),                     MP_OBJ_NEW_SMALL_INT(GPIO_DIR_MODE_OUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OPEN_DRAIN),              MP_OBJ_NEW_SMALL_INT(PIN_TYPE_OD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ALT),                     MP_OBJ_NEW_SMALL_INT(GPIO_DIR_MODE_ALT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ALT_OPEN_DRAIN),          MP_OBJ_NEW_SMALL_INT(GPIO_DIR_MODE_ALT_OD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_NONE),               MP_OBJ_NEW_SMALL_INT(PIN_TYPE_STD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_UP),                 MP_OBJ_NEW_SMALL_INT(PIN_TYPE_STD_PU) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_DOWN),               MP_OBJ_NEW_SMALL_INT(PIN_TYPE_STD_PD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LOW_POWER),               MP_OBJ_NEW_SMALL_INT(PIN_STRENGTH_2MA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MED_POWER),               MP_OBJ_NEW_SMALL_INT(PIN_STRENGTH_4MA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HIGH_POWER),              MP_OBJ_NEW_SMALL_INT(PIN_STRENGTH_6MA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_FALLING),             MP_OBJ_NEW_SMALL_INT(GPIO_FALLING_EDGE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_RISING),              MP_OBJ_NEW_SMALL_INT(GPIO_RISING_EDGE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_RISING_FALLING),      MP_OBJ_NEW_SMALL_INT(GPIO_BOTH_EDGES) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_LOW_LEVEL),           MP_OBJ_NEW_SMALL_INT(GPIO_LOW_LEVEL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_INT_HIGH_LEVEL),          MP_OBJ_NEW_SMALL_INT(GPIO_HIGH_LEVEL) },
};

STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

const mp_obj_type_t pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pin_print,
    .make_new = pin_make_new,
    .call = pin_call,
    .locals_dict = (mp_obj_t)&pin_locals_dict,
};

STATIC const mp_cb_methods_t pin_cb_methods = {
    .init = pin_callback,
    .enable = pin_extint_enable,
    .disable = pin_extint_disable,
};

