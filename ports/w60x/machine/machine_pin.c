/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
#include <string.h>

#include "wm_include.h"

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/virtpin.h"

#include "mphalport.h"
#include "modmachine.h"

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    enum tls_io_name id;
} machine_pin_obj_t;

typedef struct _machine_pin_irq_obj_t {
    mp_obj_base_t base;
    enum tls_io_name id;
} machine_pin_irq_obj_t;

STATIC const machine_pin_obj_t machine_pin_obj[] = {
    {{&machine_pin_type}, WM_IO_PA_00},
    {{&machine_pin_type}, WM_IO_PA_01},
    {{&machine_pin_type}, WM_IO_PA_02},
    {{&machine_pin_type}, WM_IO_PA_03},
    {{&machine_pin_type}, WM_IO_PA_04},
    {{&machine_pin_type}, WM_IO_PA_05},

    {{&machine_pin_type}, WM_IO_PA_06},
    {{&machine_pin_type}, WM_IO_PA_07},
    {{&machine_pin_type}, WM_IO_PA_08},
    {{&machine_pin_type}, WM_IO_PA_09},
    {{&machine_pin_type}, WM_IO_PA_10},
    {{&machine_pin_type}, WM_IO_PA_11},

    {{&machine_pin_type}, WM_IO_PA_12},
    {{&machine_pin_type}, WM_IO_PA_13},
    {{&machine_pin_type}, WM_IO_PA_14},
    {{&machine_pin_type}, WM_IO_PA_15},


    {{&machine_pin_type}, WM_IO_PB_00},
    {{&machine_pin_type}, WM_IO_PB_01},
    {{&machine_pin_type}, WM_IO_PB_02},
    {{&machine_pin_type}, WM_IO_PB_03},
    {{&machine_pin_type}, WM_IO_PB_04},
    {{&machine_pin_type}, WM_IO_PB_05},

    {{&machine_pin_type}, WM_IO_PB_06},
    {{&machine_pin_type}, WM_IO_PB_07},
    {{&machine_pin_type}, WM_IO_PB_08},
    {{&machine_pin_type}, WM_IO_PB_09},
    {{&machine_pin_type}, WM_IO_PB_10},
    {{&machine_pin_type}, WM_IO_PB_11},

    {{&machine_pin_type}, WM_IO_PB_12},
    {{&machine_pin_type}, WM_IO_PB_13},
    {{&machine_pin_type}, WM_IO_PB_14},
    {{&machine_pin_type}, WM_IO_PB_15},
    {{&machine_pin_type}, WM_IO_PB_16},
    {{&machine_pin_type}, WM_IO_PB_17},

    {{&machine_pin_type}, WM_IO_PB_18},
    {{&machine_pin_type}, WM_IO_PB_19},
    {{&machine_pin_type}, WM_IO_PB_20},
    {{&machine_pin_type}, WM_IO_PB_21},
    {{&machine_pin_type}, WM_IO_PB_22},
    {{&machine_pin_type}, WM_IO_PB_23},

    {{&machine_pin_type}, WM_IO_PB_24},
    {{&machine_pin_type}, WM_IO_PB_25},
    {{&machine_pin_type}, WM_IO_PB_26},
    {{&machine_pin_type}, WM_IO_PB_27},
    {{&machine_pin_type}, WM_IO_PB_28},
    {{&machine_pin_type}, WM_IO_PB_29},

    {{&machine_pin_type}, WM_IO_PB_30},
    {{&machine_pin_type}, WM_IO_PB_31},
};

// forward declaration
STATIC const machine_pin_irq_obj_t machine_pin_irq_object[];

void machine_pins_init(void) {
    memset(&MP_STATE_PORT(machine_pin_irq_handler[0]), 0, sizeof(MP_STATE_PORT(machine_pin_irq_handler)));
}

STATIC void machine_pin_isr_handler(void *arg) {
    machine_pin_obj_t *self = arg;
    mp_obj_t handler = MP_STATE_PORT(machine_pin_irq_handler)[self->id];
    mp_sched_schedule(handler, MP_OBJ_FROM_PTR(self));
}

mp_hal_pin_obj_t machine_pin_get_id(mp_obj_t pin_in) {
    if (mp_obj_get_type(pin_in) != &machine_pin_type) {
        mp_raise_ValueError("expecting a pin");
    }
    machine_pin_obj_t *self = pin_in;
    return self->id;
}

STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    mp_printf(print, "Pin(%u)", self->id);
}

// pin.init(direction, attribute, value)
STATIC mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = WM_GPIO_DIR_INPUT} },
        { MP_QSTR_pull, MP_ARG_INT, {.u_int = WM_GPIO_ATTR_FLOATING} },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    tls_gpio_cfg(self->id, args[ARG_mode].u_int, args[ARG_pull].u_int);

    if (args[ARG_value].u_obj != MP_OBJ_NULL)
        tls_gpio_write(self->id,  mp_obj_is_true(args[ARG_value].u_obj));

    return mp_const_none;
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get the wanted pin object
    int wanted_pin = mp_obj_get_int(args[0]);
    const machine_pin_obj_t *self = NULL;
    if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj)) {
        self = (machine_pin_obj_t *)&machine_pin_obj[wanted_pin];
    }
    if (self == NULL || self->base.type == NULL) {
        mp_raise_ValueError("invalid pin");
    }

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

// fast method for getting/setting pin value
STATIC mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(tls_gpio_read(self->id));
    } else {
        // set pin
        tls_gpio_write(self->id, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

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

// pin.off()
STATIC mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    tls_gpio_write(self->id,  0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

// pin.on()
STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    tls_gpio_write(self->id, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

STATIC void machine_pin_irq_callback(void *p) {
    machine_pin_obj_t *self = p;
    tls_clr_gpio_irq_status(self->id);
    machine_pin_isr_handler(self);
}


// pin.irq(trigger_mode)
STATIC mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = 0} },
    };
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (n_args > 1 || kw_args->used != 0) {
        // configure irq
        mp_obj_t handler = args[ARG_handler].u_obj;
        uint32_t trigger = args[ARG_trigger].u_int;
        if (handler == mp_const_none) {
            handler = MP_OBJ_NULL;
        }

        MP_STATE_PORT(machine_pin_irq_handler)[self->id] = handler;
        tls_gpio_isr_register(self->id, machine_pin_irq_callback, self);
        tls_gpio_irq_enable(self->id, trigger);
    }

    // return the irq object
    return MP_OBJ_FROM_PTR(&machine_pin_irq_object[self->id]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_pin_irq_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(WM_GPIO_DIR_INPUT) },

    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(WM_GPIO_DIR_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(WM_GPIO_DIR_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(WM_GPIO_ATTR_PULLHIGH) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(WM_GPIO_ATTR_PULLLOW) },
    { MP_ROM_QSTR(MP_QSTR_PULL_FLOATING), MP_ROM_INT(WM_GPIO_ATTR_FLOATING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_DOUBLE_EDGE), MP_ROM_INT(WM_GPIO_IRQ_TRIG_DOUBLE_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(WM_GPIO_IRQ_TRIG_RISING_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(WM_GPIO_IRQ_TRIG_FALLING_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_HIGH_LEVEL), MP_ROM_INT(WM_GPIO_IRQ_TRIG_HIGH_LEVEL) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_LOW_LEVEL), MP_ROM_INT(WM_GPIO_IRQ_TRIG_LOW_LEVEL) },

    { MP_ROM_QSTR(MP_QSTR_PA_00), MP_ROM_INT(WM_IO_PA_00) },
    { MP_ROM_QSTR(MP_QSTR_PA_01), MP_ROM_INT(WM_IO_PA_01) },
    { MP_ROM_QSTR(MP_QSTR_PA_02), MP_ROM_INT(WM_IO_PA_02) },
    { MP_ROM_QSTR(MP_QSTR_PA_03), MP_ROM_INT(WM_IO_PA_03) },
    { MP_ROM_QSTR(MP_QSTR_PA_04), MP_ROM_INT(WM_IO_PA_04) },

    { MP_ROM_QSTR(MP_QSTR_PA_05), MP_ROM_INT(WM_IO_PA_05) },
    { MP_ROM_QSTR(MP_QSTR_PA_06), MP_ROM_INT(WM_IO_PA_06) },
    { MP_ROM_QSTR(MP_QSTR_PA_07), MP_ROM_INT(WM_IO_PA_07) },
    { MP_ROM_QSTR(MP_QSTR_PA_08), MP_ROM_INT(WM_IO_PA_08) },
    { MP_ROM_QSTR(MP_QSTR_PA_09), MP_ROM_INT(WM_IO_PA_09) },

    { MP_ROM_QSTR(MP_QSTR_PA_10), MP_ROM_INT(WM_IO_PA_10) },
    { MP_ROM_QSTR(MP_QSTR_PA_11), MP_ROM_INT(WM_IO_PA_11) },
    { MP_ROM_QSTR(MP_QSTR_PA_12), MP_ROM_INT(WM_IO_PA_12) },
    { MP_ROM_QSTR(MP_QSTR_PA_13), MP_ROM_INT(WM_IO_PA_13) },
    { MP_ROM_QSTR(MP_QSTR_PA_14), MP_ROM_INT(WM_IO_PA_14) },

    { MP_ROM_QSTR(MP_QSTR_PA_15), MP_ROM_INT(WM_IO_PA_15) },


    { MP_ROM_QSTR(MP_QSTR_PB_00), MP_ROM_INT(WM_IO_PB_00) },
    { MP_ROM_QSTR(MP_QSTR_PB_01), MP_ROM_INT(WM_IO_PB_01) },
    { MP_ROM_QSTR(MP_QSTR_PB_02), MP_ROM_INT(WM_IO_PB_02) },
    { MP_ROM_QSTR(MP_QSTR_PB_03), MP_ROM_INT(WM_IO_PB_03) },
    { MP_ROM_QSTR(MP_QSTR_PB_04), MP_ROM_INT(WM_IO_PB_04) },

    { MP_ROM_QSTR(MP_QSTR_PB_05), MP_ROM_INT(WM_IO_PB_05) },
    { MP_ROM_QSTR(MP_QSTR_PB_06), MP_ROM_INT(WM_IO_PB_06) },
    { MP_ROM_QSTR(MP_QSTR_PB_07), MP_ROM_INT(WM_IO_PB_07) },
    { MP_ROM_QSTR(MP_QSTR_PB_08), MP_ROM_INT(WM_IO_PB_08) },
    { MP_ROM_QSTR(MP_QSTR_PB_09), MP_ROM_INT(WM_IO_PB_09) },

    { MP_ROM_QSTR(MP_QSTR_PB_10), MP_ROM_INT(WM_IO_PB_10) },
    { MP_ROM_QSTR(MP_QSTR_PB_11), MP_ROM_INT(WM_IO_PB_11) },
    { MP_ROM_QSTR(MP_QSTR_PB_12), MP_ROM_INT(WM_IO_PB_12) },
    { MP_ROM_QSTR(MP_QSTR_PB_13), MP_ROM_INT(WM_IO_PB_13) },
    { MP_ROM_QSTR(MP_QSTR_PB_14), MP_ROM_INT(WM_IO_PB_14) },

    { MP_ROM_QSTR(MP_QSTR_PB_15), MP_ROM_INT(WM_IO_PB_15) },
    { MP_ROM_QSTR(MP_QSTR_PB_16), MP_ROM_INT(WM_IO_PB_16) },
    { MP_ROM_QSTR(MP_QSTR_PB_17), MP_ROM_INT(WM_IO_PB_17) },
    { MP_ROM_QSTR(MP_QSTR_PB_18), MP_ROM_INT(WM_IO_PB_18) },
    { MP_ROM_QSTR(MP_QSTR_PB_19), MP_ROM_INT(WM_IO_PB_19) },

    { MP_ROM_QSTR(MP_QSTR_PB_20), MP_ROM_INT(WM_IO_PB_20) },
    { MP_ROM_QSTR(MP_QSTR_PB_21), MP_ROM_INT(WM_IO_PB_21) },
    { MP_ROM_QSTR(MP_QSTR_PB_22), MP_ROM_INT(WM_IO_PB_22) },
    { MP_ROM_QSTR(MP_QSTR_PB_23), MP_ROM_INT(WM_IO_PB_23) },
    { MP_ROM_QSTR(MP_QSTR_PB_24), MP_ROM_INT(WM_IO_PB_24) },

    { MP_ROM_QSTR(MP_QSTR_PB_25), MP_ROM_INT(WM_IO_PB_25) },
    { MP_ROM_QSTR(MP_QSTR_PB_26), MP_ROM_INT(WM_IO_PB_26) },
    { MP_ROM_QSTR(MP_QSTR_PB_27), MP_ROM_INT(WM_IO_PB_27) },
    { MP_ROM_QSTR(MP_QSTR_PB_28), MP_ROM_INT(WM_IO_PB_28) },
    { MP_ROM_QSTR(MP_QSTR_PB_29), MP_ROM_INT(WM_IO_PB_29) },

    { MP_ROM_QSTR(MP_QSTR_PB_30), MP_ROM_INT(WM_IO_PB_30) },
    { MP_ROM_QSTR(MP_QSTR_PB_31), MP_ROM_INT(WM_IO_PB_31) },
};

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    switch (request) {
    case MP_PIN_READ: {
        return tls_gpio_read(self->id);
    }
    case MP_PIN_WRITE: {
        tls_gpio_write(self->id, arg);
        return 0;
    }
    }
    return -1;
}

STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

const mp_obj_type_t machine_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = machine_pin_print,
    .make_new = mp_pin_make_new,
    .call = machine_pin_call,
    .protocol = &pin_pin_p,
    .locals_dict = (mp_obj_t) &machine_pin_locals_dict,
};

/******************************************************************************/
// Pin IRQ object

STATIC const mp_obj_type_t machine_pin_irq_type;

STATIC const machine_pin_irq_obj_t machine_pin_irq_object[] = {
    {{&machine_pin_irq_type}, WM_IO_PA_00},
    {{&machine_pin_irq_type}, WM_IO_PA_01},
    {{&machine_pin_irq_type}, WM_IO_PA_02},
    {{&machine_pin_irq_type}, WM_IO_PA_03},
    {{&machine_pin_irq_type}, WM_IO_PA_04},
    {{&machine_pin_irq_type}, WM_IO_PA_05},

    {{&machine_pin_irq_type}, WM_IO_PA_06},
    {{&machine_pin_irq_type}, WM_IO_PA_07},
    {{&machine_pin_irq_type}, WM_IO_PA_08},
    {{&machine_pin_irq_type}, WM_IO_PA_09},
    {{&machine_pin_irq_type}, WM_IO_PA_10},
    {{&machine_pin_irq_type}, WM_IO_PA_11},

    {{&machine_pin_irq_type}, WM_IO_PA_12},
    {{&machine_pin_irq_type}, WM_IO_PA_13},
    {{&machine_pin_irq_type}, WM_IO_PA_14},
    {{&machine_pin_irq_type}, WM_IO_PA_15},


    {{&machine_pin_irq_type}, WM_IO_PB_00},
    {{&machine_pin_irq_type}, WM_IO_PB_01},
    {{&machine_pin_irq_type}, WM_IO_PB_02},
    {{&machine_pin_irq_type}, WM_IO_PB_03},
    {{&machine_pin_irq_type}, WM_IO_PB_04},
    {{&machine_pin_irq_type}, WM_IO_PB_05},

    {{&machine_pin_irq_type}, WM_IO_PB_06},
    {{&machine_pin_irq_type}, WM_IO_PB_07},
    {{&machine_pin_irq_type}, WM_IO_PB_08},
    {{&machine_pin_irq_type}, WM_IO_PB_09},
    {{&machine_pin_irq_type}, WM_IO_PB_10},
    {{&machine_pin_irq_type}, WM_IO_PB_11},

    {{&machine_pin_irq_type}, WM_IO_PB_12},
    {{&machine_pin_irq_type}, WM_IO_PB_13},
    {{&machine_pin_irq_type}, WM_IO_PB_14},
    {{&machine_pin_irq_type}, WM_IO_PB_15},
    {{&machine_pin_irq_type}, WM_IO_PB_16},
    {{&machine_pin_irq_type}, WM_IO_PB_17},

    {{&machine_pin_irq_type}, WM_IO_PB_18},
    {{&machine_pin_irq_type}, WM_IO_PB_19},
    {{&machine_pin_irq_type}, WM_IO_PB_20},
    {{&machine_pin_irq_type}, WM_IO_PB_21},
    {{&machine_pin_irq_type}, WM_IO_PB_22},
    {{&machine_pin_irq_type}, WM_IO_PB_23},

    {{&machine_pin_irq_type}, WM_IO_PB_24},
    {{&machine_pin_irq_type}, WM_IO_PB_25},
    {{&machine_pin_irq_type}, WM_IO_PB_26},
    {{&machine_pin_irq_type}, WM_IO_PB_27},
    {{&machine_pin_irq_type}, WM_IO_PB_28},
    {{&machine_pin_irq_type}, WM_IO_PB_29},

    {{&machine_pin_irq_type}, WM_IO_PB_30},
    {{&machine_pin_irq_type}, WM_IO_PB_31},
};

STATIC mp_obj_t machine_pin_irq_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    machine_pin_irq_obj_t *self = self_in;
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    machine_pin_isr_handler(self);
    return mp_const_none;
}

STATIC mp_obj_t machine_pin_irq_trigger(size_t n_args, const mp_obj_t *args) {
    machine_pin_irq_obj_t *self = args[0];

    if (n_args == 2) {
        // set trigger
        tls_gpio_irq_enable(self->id, mp_obj_get_int(args[1]));
    }
    // return irq status
    return MP_OBJ_NEW_SMALL_INT(tls_get_gpio_irq_status(self->id));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_irq_trigger_obj, 1, 2, machine_pin_irq_trigger);

STATIC const mp_rom_map_elem_t machine_pin_irq_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_trigger), MP_ROM_PTR(&machine_pin_irq_trigger_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_pin_irq_locals_dict, machine_pin_irq_locals_dict_table);

STATIC const mp_obj_type_t machine_pin_irq_type = {
    { &mp_type_type },
    .name = MP_QSTR_GPIO_IRQ,
    .call = machine_pin_irq_call,
    .locals_dict = (mp_obj_dict_t *) &machine_pin_irq_locals_dict,
};

