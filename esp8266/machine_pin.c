/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014, 2015 Damien P. George
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

#include "etshal.h"
#include "c_types.h"
#include "user_interface.h"
#include "gpio.h"

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "extmod/virtpin.h"
#include "modmachine.h"

#define GET_TRIGGER(phys_port) \
    GPIO_PIN_INT_TYPE_GET(GPIO_REG_READ(GPIO_PIN_ADDR(phys_port)))
#define SET_TRIGGER(phys_port, trig) \
    (GPIO_REG_WRITE(GPIO_PIN_ADDR(phys_port), \
        (GPIO_REG_READ(GPIO_PIN_ADDR(phys_port)) & ~GPIO_PIN_INT_TYPE_MASK) \
        | GPIO_PIN_INT_TYPE_SET(trig))) \

#define GPIO_MODE_INPUT (0)
#define GPIO_MODE_OUTPUT (1)
#define GPIO_MODE_OPEN_DRAIN (2) // synthesised
#define GPIO_PULL_NONE (0)
#define GPIO_PULL_UP (1)
// Removed in SDK 1.1.0
//#define GPIO_PULL_DOWN (2)

typedef struct _pin_irq_obj_t {
    mp_obj_base_t base;
    uint16_t phys_port;
} pin_irq_obj_t;

const pyb_pin_obj_t pyb_pin_obj[16 + 1] = {
    {{&pyb_pin_type}, 0, FUNC_GPIO0, PERIPHS_IO_MUX_GPIO0_U},
    {{&pyb_pin_type}, 1, FUNC_GPIO1, PERIPHS_IO_MUX_U0TXD_U},
    {{&pyb_pin_type}, 2, FUNC_GPIO2, PERIPHS_IO_MUX_GPIO2_U},
    {{&pyb_pin_type}, 3, FUNC_GPIO3, PERIPHS_IO_MUX_U0RXD_U},
    {{&pyb_pin_type}, 4, FUNC_GPIO4, PERIPHS_IO_MUX_GPIO4_U},
    {{&pyb_pin_type}, 5, FUNC_GPIO5, PERIPHS_IO_MUX_GPIO5_U},
    {{NULL}, 0, 0, 0},
    {{NULL}, 0, 0, 0},
    {{NULL}, 0, 0, 0},
    {{&pyb_pin_type}, 9, FUNC_GPIO9, PERIPHS_IO_MUX_SD_DATA2_U},
    {{&pyb_pin_type}, 10, FUNC_GPIO10, PERIPHS_IO_MUX_SD_DATA3_U},
    {{NULL}, 0, 0, 0},
    {{&pyb_pin_type}, 12, FUNC_GPIO12, PERIPHS_IO_MUX_MTDI_U},
    {{&pyb_pin_type}, 13, FUNC_GPIO13, PERIPHS_IO_MUX_MTCK_U},
    {{&pyb_pin_type}, 14, FUNC_GPIO14, PERIPHS_IO_MUX_MTMS_U},
    {{&pyb_pin_type}, 15, FUNC_GPIO15, PERIPHS_IO_MUX_MTDO_U},
    // GPIO16 is special, belongs to different register set, and
    // otherwise handled specially.
    {{&pyb_pin_type}, 16, -1, -1},
};

STATIC uint8_t pin_mode[16 + 1];

// forward declaration
STATIC const pin_irq_obj_t pin_irq_obj[16];

// whether the irq is hard or soft
STATIC bool pin_irq_is_hard[16];

void pin_init0(void) {
    ETS_GPIO_INTR_DISABLE();
    ETS_GPIO_INTR_ATTACH(pin_intr_handler_iram, NULL);
    // disable all interrupts
    memset(&MP_STATE_PORT(pin_irq_handler)[0], 0, 16 * sizeof(mp_obj_t));
    memset(pin_irq_is_hard, 0, sizeof(pin_irq_is_hard));
    for (int p = 0; p < 16; ++p) {
        GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, 1 << p);
        SET_TRIGGER(p, 0);
    }
    ETS_GPIO_INTR_ENABLE();
}

void pin_intr_handler(uint32_t status) {
    mp_sched_lock();
    gc_lock();
    status &= 0xffff;
    for (int p = 0; status; ++p, status >>= 1) {
        if (status & 1) {
            mp_obj_t handler = MP_STATE_PORT(pin_irq_handler)[p];
            if (handler != MP_OBJ_NULL) {
                if (pin_irq_is_hard[p]) {
                    mp_call_function_1_protected(handler, MP_OBJ_FROM_PTR(&pyb_pin_obj[p]));
                } else {
                    mp_sched_schedule(handler, MP_OBJ_FROM_PTR(&pyb_pin_obj[p]));
                }
            }
        }
    }
    gc_unlock();
    mp_sched_unlock();
}

pyb_pin_obj_t *mp_obj_get_pin_obj(mp_obj_t pin_in) {
    if (mp_obj_get_type(pin_in) != &pyb_pin_type) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "expecting a pin"));
    }
    pyb_pin_obj_t *self = pin_in;
    return self;
}

uint mp_obj_get_pin(mp_obj_t pin_in) {
    return mp_obj_get_pin_obj(pin_in)->phys_port;
}

void mp_hal_pin_input(mp_hal_pin_obj_t pin_id) {
    pin_mode[pin_id] = GPIO_MODE_INPUT;
    if (pin_id == 16) {
        WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | 1);
        WRITE_PERI_REG(RTC_GPIO_CONF, READ_PERI_REG(RTC_GPIO_CONF) & ~1);
        WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & ~1)); // input
    } else {
        const pyb_pin_obj_t *self = &pyb_pin_obj[pin_id];
        PIN_FUNC_SELECT(self->periph, self->func);
        PIN_PULLUP_DIS(self->periph);
        gpio_output_set(0, 0, 0, 1 << self->phys_port);
    }
}

void mp_hal_pin_output(mp_hal_pin_obj_t pin_id) {
    pin_mode[pin_id] = GPIO_MODE_OUTPUT;
    if (pin_id == 16) {
        WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | 1);
        WRITE_PERI_REG(RTC_GPIO_CONF, READ_PERI_REG(RTC_GPIO_CONF) & ~1);
        WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & ~1) | 1); // output
    } else {
        const pyb_pin_obj_t *self = &pyb_pin_obj[pin_id];
        PIN_FUNC_SELECT(self->periph, self->func);
        PIN_PULLUP_DIS(self->periph);
        gpio_output_set(0, 0, 1 << self->phys_port, 0);
    }
}

int pin_get(uint pin) {
    if (pin == 16) {
        return READ_PERI_REG(RTC_GPIO_IN_DATA) & 1;
    }
    return GPIO_INPUT_GET(pin);
}

void pin_set(uint pin, int value) {
    if (pin == 16) {
        int out_en = (pin_mode[pin] == GPIO_MODE_OUTPUT);
        WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | 1);
        WRITE_PERI_REG(RTC_GPIO_CONF, READ_PERI_REG(RTC_GPIO_CONF) & ~1);
        WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & ~1) | out_en);
        WRITE_PERI_REG(RTC_GPIO_OUT, (READ_PERI_REG(RTC_GPIO_OUT) & ~1) | value);
        return;
    }

    uint32_t enable = 0;
    uint32_t disable = 0;
    switch (pin_mode[pin]) {
        case GPIO_MODE_INPUT:
            value = -1;
            disable = 1;
            break;

        case GPIO_MODE_OUTPUT:
            enable = 1;
            break;

        case GPIO_MODE_OPEN_DRAIN:
            if (value == -1) {
                return;
            } else if (value == 0) {
                enable = 1;
            } else {
                value = -1;
                disable = 1;
            }
            break;
    }

    enable <<= pin;
    disable <<= pin;
    if (value == -1) {
        gpio_output_set(0, 0, enable, disable);
    } else {
        gpio_output_set(value << pin, (1 - value) << pin, enable, disable);
    }
}

STATIC void pyb_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_pin_obj_t *self = self_in;

    // pin name
    mp_printf(print, "Pin(%u)", self->phys_port);
}

// pin.init(mode, pull=None, *, value)
STATIC mp_obj_t pyb_pin_obj_init_helper(pyb_pin_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get io mode
    uint mode = args[ARG_mode].u_int;

    // get pull mode
    uint pull = GPIO_PULL_NONE;
    if (args[ARG_pull].u_obj != mp_const_none) {
        pull = mp_obj_get_int(args[ARG_pull].u_obj);
    }

    // get initial value
    int value;
    if (args[ARG_value].u_obj == MP_OBJ_NULL) {
        value = -1;
    } else {
        value = mp_obj_is_true(args[ARG_value].u_obj);
    }

    // save the mode
    pin_mode[self->phys_port] = mode;

    // configure the GPIO as requested
    if (self->phys_port == 16) {
        // only pull-down seems to be supported by the hardware, and
        // we only expose pull-up behaviour in software
        if (pull != GPIO_PULL_NONE) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Pin(16) doesn't support pull"));
        }
    } else {
        PIN_FUNC_SELECT(self->periph, self->func);
        #if 0
        // Removed in SDK 1.1.0
        if ((pull & GPIO_PULL_DOWN) == 0) {
            PIN_PULLDWN_DIS(self->periph);
        }
        #endif
        if ((pull & GPIO_PULL_UP) == 0) {
            PIN_PULLUP_DIS(self->periph);
        }
        #if 0
        if ((pull & GPIO_PULL_DOWN) != 0) {
            PIN_PULLDWN_EN(self->periph);
        }
        #endif
        if ((pull & GPIO_PULL_UP) != 0) {
            PIN_PULLUP_EN(self->periph);
        }
    }

    pin_set(self->phys_port, value);

    return mp_const_none;
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get the wanted pin object
    int wanted_pin = mp_obj_get_int(args[0]);
    pyb_pin_obj_t *pin = NULL;
    if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(pyb_pin_obj)) {
        pin = (pyb_pin_obj_t*)&pyb_pin_obj[wanted_pin];
    }
    if (pin == NULL || pin->base.type == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid pin"));
    }

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

// fast method for getting/setting pin value
STATIC mp_obj_t pyb_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    pyb_pin_obj_t *self = self_in;
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(pin_get(self->phys_port));
    } else {
        // set pin
        pin_set(self->phys_port, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

// pin.init(mode, pull)
STATIC mp_obj_t pyb_pin_obj_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_pin_init_obj, 1, pyb_pin_obj_init);

// pin.value([value])
STATIC mp_obj_t pyb_pin_value(mp_uint_t n_args, const mp_obj_t *args) {
    return pyb_pin_call(args[0], n_args - 1, 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pin_value_obj, 1, 2, pyb_pin_value);

STATIC mp_obj_t pyb_pin_off(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    pin_set(self->phys_port, 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_off_obj, pyb_pin_off);

STATIC mp_obj_t pyb_pin_on(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    pin_set(self->phys_port, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_on_obj, pyb_pin_on);

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING, hard=False)
STATIC mp_obj_t pyb_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = GPIO_PIN_INTR_POSEDGE | GPIO_PIN_INTR_NEGEDGE} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    pyb_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (self->phys_port >= 16) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "pin does not have IRQ capabilities"));
    }

    if (n_args > 1 || kw_args->used != 0) {
        // configure irq
        mp_obj_t handler = args[ARG_handler].u_obj;
        uint32_t trigger = args[ARG_trigger].u_int;
        if (handler == mp_const_none) {
            handler = MP_OBJ_NULL;
            trigger = 0;
        }
        ETS_GPIO_INTR_DISABLE();
        MP_STATE_PORT(pin_irq_handler)[self->phys_port] = handler;
        pin_irq_is_hard[self->phys_port] = args[ARG_hard].u_bool;
        SET_TRIGGER(self->phys_port, trigger);
        GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, 1 << self->phys_port);
        ETS_GPIO_INTR_ENABLE();
    }

    // return the irq object
    return MP_OBJ_FROM_PTR(&pin_irq_obj[self->phys_port]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_pin_irq_obj, 1, pyb_pin_irq);

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode);
STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    pyb_pin_obj_t *self = self_in;

    switch (request) {
        case MP_PIN_READ: {
            return pin_get(self->phys_port);
        }
        case MP_PIN_WRITE: {
            pin_set(self->phys_port, arg);
            return 0;
        }
    }
    return -1;
}

STATIC const mp_map_elem_t pyb_pin_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),    (mp_obj_t)&pyb_pin_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_value),   (mp_obj_t)&pyb_pin_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_off),     (mp_obj_t)&pyb_pin_off_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_on),      (mp_obj_t)&pyb_pin_on_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_irq),     (mp_obj_t)&pyb_pin_irq_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_IN),        MP_OBJ_NEW_SMALL_INT(GPIO_MODE_INPUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OUT),       MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OUTPUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OPEN_DRAIN), MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OPEN_DRAIN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_UP),   MP_OBJ_NEW_SMALL_INT(GPIO_PULL_UP) },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_PULL_DOWN), MP_OBJ_NEW_SMALL_INT(GPIO_PULL_DOWN) },

    // IRG triggers, can be or'd together
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_RISING), MP_OBJ_NEW_SMALL_INT(GPIO_PIN_INTR_POSEDGE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_FALLING), MP_OBJ_NEW_SMALL_INT(GPIO_PIN_INTR_NEGEDGE) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_pin_locals_dict, pyb_pin_locals_dict_table);

STATIC const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

const mp_obj_type_t pyb_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pyb_pin_print,
    .make_new = mp_pin_make_new,
    .call = pyb_pin_call,
    .protocol = &pin_pin_p,
    .locals_dict = (mp_obj_t)&pyb_pin_locals_dict,
};

/******************************************************************************/
// Pin IRQ object

STATIC const mp_obj_type_t pin_irq_type;

STATIC const pin_irq_obj_t pin_irq_obj[16] = {
    {{&pin_irq_type}, 0},
    {{&pin_irq_type}, 1},
    {{&pin_irq_type}, 2},
    {{&pin_irq_type}, 3},
    {{&pin_irq_type}, 4},
    {{&pin_irq_type}, 5},
    {{&pin_irq_type}, 6},
    {{&pin_irq_type}, 7},
    {{&pin_irq_type}, 8},
    {{&pin_irq_type}, 9},
    {{&pin_irq_type}, 10},
    {{&pin_irq_type}, 11},
    {{&pin_irq_type}, 12},
    {{&pin_irq_type}, 13},
    {{&pin_irq_type}, 14},
    {{&pin_irq_type}, 15},
};

STATIC mp_obj_t pin_irq_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    pin_irq_obj_t *self = self_in;
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    pin_intr_handler(1 << self->phys_port);
    return mp_const_none;
}

STATIC mp_obj_t pin_irq_trigger(size_t n_args, const mp_obj_t *args) {
    pin_irq_obj_t *self = args[0];
    uint32_t orig_trig = GET_TRIGGER(self->phys_port);
    if (n_args == 2) {
        // set trigger
        SET_TRIGGER(self->phys_port, mp_obj_get_int(args[1]));
    }
    // return original trigger value
    return MP_OBJ_NEW_SMALL_INT(orig_trig);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_irq_trigger_obj, 1, 2, pin_irq_trigger);

STATIC const mp_rom_map_elem_t pin_irq_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_trigger),  MP_ROM_PTR(&pin_irq_trigger_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pin_irq_locals_dict, pin_irq_locals_dict_table);

STATIC const mp_obj_type_t pin_irq_type = {
    { &mp_type_type },
    .name = MP_QSTR_IRQ,
    .call = pin_irq_call,
    .locals_dict = (mp_obj_dict_t*)&pin_irq_locals_dict,
};
