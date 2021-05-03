/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dominique Meurisse - MCHobby.be
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

// #include "driver/gpio.h"
// #include "driver/rtc_io.h"
#include "samd21.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "mphalport.h"
#include "modmachine.h"
// Should not be used-- #include "modmachine.h"
// #include "extmod/virtpin.h"
// #include "machine_rtc.h"
// #include "modesp32.h"q
#include "machine_pin.h"
// include "sam.h"

// Used to implement a range of pull capabilities
#define GPIO_PULL_DOWN (1)
#define GPIO_PULL_UP   (2)
#define GPIO_PULL_HOLD (4)

#define GPIO_MODE_INPUT       (1) 
#define GPIO_MODE_OUTPUT      (2)
#define GPIO_MODE_OPEN_DRAIN  (4) 

#define GPIO_PIN_INTR_HILEVEL     (1) // interruption
#define GPIO_PIN_INTR_POSEDGE     (2)
#define GPIO_PIN_INTR_LOLEVEL     (4)
#define GPIO_PIN_INTR_NEGEDGE     (8)


// --- hal_gpio routines -------------------------------------------------------------

int  GPIO_IS_VALID_OUTPUT_GPIO( gpio_num_t p ){ return 0; } //TODO: from ESP32 API, change to SAMD API (on ESP32 some GPIO cannot be used as output)
int  rtc_gpio_is_valid_gpio( gpio_num_t p ){ return 0; } // TODO: from ESP32 API, change to SAMD API


void gpio_pad_select( machine_pin_obj_t *pin_obj ){
    // Deactivate MultiPlexing 
    PORT->Group[PORTPA_NR(pin_obj->port_shift)].PINCFG[PORTPA_SHIFT(pin_obj->port_shift)].reg &= ~PORT_PINCFG_PMUXEN;
} 

void gpio_set_level( machine_pin_obj_t *pin_obj, uint8_t state ){
    mp_printf( MP_PYTHON_PRINTER, "DBG: gpio_set_level %u \n", PORT_PA(pin_obj) ); 
    mp_printf( MP_PYTHON_PRINTER, "DBG: state %u \n", state );
    if( state != 0 ) {
        REG_PORT_OUTSET0 = PORT_PA(pin_obj);
    }
    else {
        REG_PORT_OUTCLR0 = PORT_PA(pin_obj);
    }
}

uint8_t gpio_get_level( machine_pin_obj_t *pin_obj ) {
    if( (REG_PORT_DIR0 & PORT_PA(pin_obj)) == 0){ // DIR = Input ?
        if ((REG_PORT_IN0 &  PORT_PA(pin_obj)) != 0)
            return 1;
        else
            return 0;
    }
    else { // DIR = output
        if ((REG_PORT_OUT0 &  PORT_PA(pin_obj)) != 0)
            return 1;
        else
            return 0;
    }
}


void gpio_set_direction( machine_pin_obj_t *pin_obj, uint8_t io_mode ){ 
    if( io_mode & GPIO_MODE_OUTPUT ) {
        mp_printf( MP_PYTHON_PRINTER, "DBG: gpio_set_direction OUT %u \n", PORT_PA(pin_obj) );
        REG_PORT_DIRSET0 = PORT_PA(pin_obj);
    }
    else if ( io_mode & GPIO_MODE_INPUT ) {
        REG_PORT_DIRCLR0 = PORT_PA(pin_obj);
    }
    else {
        mp_raise_ValueError(MP_ERROR_TEXT("OD to implement"));
    }

} 

void gpio_pulldown_en( gpio_num_t p ){ } // TODO: from ESP32 API, change to SAMD API
void gpio_pulldown_dis( gpio_num_t p ){ } // TODO: from ESP32 API, change to SAMD API
void gpio_pullup_en( gpio_num_t p ){ } // TODO: from ESP32 API, change to SAMD API
void gpio_pullup_dis( gpio_num_t p ){ } // TODO: from ESP32 API, change to SAMD API
void gpio_hold_en( gpio_num_t p ){ } // TODO: from ESP32 API, change to SAMD API
void gpio_hold_dis( gpio_num_t p ){ } // TODO: from ESP32 API, change to SAMD API
void rtc_gpio_deinit( gpio_num_t p ){ } // TODO: from ESP32 API, change to SAMD API



// in.h: const mp_obj_type_t machine_pin_type;

// in.h: typedef struct _machine_pin_obj_t {
// in.h:     mp_obj_base_t base;
// in.h:     gpio_num_t id;
// in.h: } machine_pin_obj_t;

//typedef struct _machine_pin_irq_obj_t {
//    mp_obj_base_t base;
//    gpio_num_t id;
//} machine_pin_irq_obj_t;


// forward declaration
// TODO: STATIC const machine_pin_irq_obj_t machine_pin_irq_object[];

// --- machine_pin routines -------------------------------------------------------------

void machine_pins_init(void) {
    //TODO: static bool did_install = false;
    //if (!did_install) {
    //    gpio_install_isr_service(0);
    //    did_install = true;
    //}
    //memset(&MP_STATE_PORT(machine_pin_irq_handler[0]), 0, sizeof(MP_STATE_PORT(machine_pin_irq_handler)));
}

void machine_pins_deinit(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(machine_pin_obj); ++i) {
        if (machine_pin_obj[i].id != (gpio_num_t)-1) {
            //TODO: gpio_isr_handler_remove(machine_pin_obj[i].id);
        }
    }
}

//TODO: STATIC void machine_pin_isr_handler(void *arg) {
//    machine_pin_obj_t *self = arg;
//    mp_obj_t handler = MP_STATE_PORT(machine_pin_irq_handler)[self->id];
//    mp_sched_schedule(handler, MP_OBJ_FROM_PTR(self));
//    mp_hal_wake_main_task_from_isr();
//}

gpio_num_t machine_pin_get_id(mp_obj_t pin_in) {
    if (mp_obj_get_type(pin_in) != &machine_pin_type) {
        mp_raise_ValueError(MP_ERROR_TEXT("expecting a pin"));
    }
    machine_pin_obj_t *self = pin_in;
    return self->id;
}

STATIC void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    mp_printf(print, "Pin(cpu=%u)", self->id);
}

// pin.init(mode, pull=None, *, value)
STATIC mp_obj_t machine_pin_obj_init_helper(const machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // reset the pin to digital if this is a mode-setting init (grab it back from ADC)
    if (args[ARG_mode].u_obj != mp_const_none) {
        if (rtc_gpio_is_valid_gpio(self->id)) {
            rtc_gpio_deinit(self->id);
        }
    }

    // configure the pin for gpio
    gpio_pad_select((machine_pin_obj_t *)self);

    // set initial value (do this before configuring mode/pull)
    if (args[ARG_value].u_obj != MP_OBJ_NULL) {
        gpio_set_level((machine_pin_obj_t *)self, mp_obj_is_true(args[ARG_value].u_obj)); // TODO: check casting
    }

    // configure mode
    if (args[ARG_mode].u_obj != mp_const_none) {
        mp_int_t pin_io_mode = mp_obj_get_int(args[ARG_mode].u_obj);
        //TODO: if (self->id >= 34 && (pin_io_mode & GPIO_MODE_DEF_OUTPUT)) {
        //    mp_raise_ValueError(MP_ERROR_TEXT("pin can only be input"));
        //} else {
        //    gpio_set_direction(self, pin_io_mode);
        //}
        gpio_set_direction((machine_pin_obj_t *)self, pin_io_mode);
    }

    // configure pull
    if (args[ARG_pull].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        int mode = 0;
        if (args[ARG_pull].u_obj != mp_const_none) {
            mode = mp_obj_get_int(args[ARG_pull].u_obj);
        }
        if (mode & GPIO_PULL_DOWN) {
            gpio_pulldown_en(self->id);
        } else {
            gpio_pulldown_dis(self->id);
        }
        if (mode & GPIO_PULL_UP) {
            gpio_pullup_en(self->id);
        } else {
            gpio_pullup_dis(self->id);
        }
        if (mode & GPIO_PULL_HOLD) {
            //TODO: should be checked
            gpio_hold_en(self->id);
        } else if (GPIO_IS_VALID_OUTPUT_GPIO(self->id)) {
            gpio_hold_dis(self->id);
        }
    }

    return mp_const_none;
}

// constructor(id, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // TODO: support pin name with pind_find(). See /ports/stm32/pin.c for implementation

    // get the wanted pin object
    int wanted_pin = mp_obj_get_int(args[0]);
    const machine_pin_obj_t *self = NULL;
    if (0 <= wanted_pin && wanted_pin < MP_ARRAY_SIZE(machine_pin_obj)) {
        self = (machine_pin_obj_t *)&machine_pin_obj[wanted_pin];
    }
    if (self == NULL || self->base.type == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
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
        return MP_OBJ_NEW_SMALL_INT(gpio_get_level(self)); // gpio_get_level(self->id)
    } else {
        // set pin
        gpio_set_level(self, mp_obj_is_true(args[0]));
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
    gpio_set_level(self, 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

// pin.on()
STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_set_level(self, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

// pin.debug()
STATIC mp_obj_t machine_pin_debug(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf( MP_PYTHON_PRINTER, "DBG: PORT_PA %u \n", PORT_PA(self) );
    mp_printf( MP_PYTHON_PRINTER, "DBG: PORT_DIR0 %u \n", REG_PORT_DIR0 );
    mp_printf( MP_PYTHON_PRINTER, "DBG: PORT_OUT0 %u \n", REG_PORT_OUT0 );
    mp_printf( MP_PYTHON_PRINTER, "DBG: PORT_IN0 %u \n", REG_PORT_IN0 );
    //mp_printf( MP_PYTHON_PRINTER, "DBG: PORT_INEN0 %u \n", REG_PORT_INEN0 );
    //mp_printf( MP_PYTHON_PRINTER, "DBG: PORT_PULLEN0 %u \n", REG_PORT_PULLEN0 );

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_debug_obj, machine_pin_debug);

// pin.irq(handler=None, trigger=IRQ_FALLING|IRQ_RISING)
//TODO: STATIC mp_obj_t machine_pin_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
//    enum { ARG_handler, ARG_trigger, ARG_wake };
//    static const mp_arg_t allowed_args[] = {
//        { MP_QSTR_handler, MP_ARG_OBJ, {.u_obj = mp_const_none} },
//        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = GPIO_PIN_INTR_POSEDGE | GPIO_PIN_INTR_NEGEDGE} },
//        { MP_QSTR_wake, MP_ARG_OBJ, {.u_obj = mp_const_none} },
//    };
//    machine_pin_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
//    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
//    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
//
//    if (n_args > 1 || kw_args->used != 0) {
//        // configure irq
//        mp_obj_t handler = args[ARG_handler].u_obj;
//        uint32_t trigger = args[ARG_trigger].u_int;
//        mp_obj_t wake_obj = args[ARG_wake].u_obj;
//
//        if ((trigger == GPIO_PIN_INTR_LOLEVEL || trigger == GPIO_PIN_INTR_HILEVEL) && wake_obj != mp_const_none) {
//            mp_int_t wake;
//            if (mp_obj_get_int_maybe(wake_obj, &wake)) {
//                if (wake < 2 || wake > 7) {
//                    mp_raise_ValueError(MP_ERROR_TEXT("bad wake value"));
//                }
//            } else {
//                mp_raise_ValueError(MP_ERROR_TEXT("bad wake value"));
//            }
//
//            if (machine_rtc_config.wake_on_touch) { // not compatible
//               mp_raise_ValueError(MP_ERROR_TEXT("no resources"));
//            }
//
//            if (!RTC_IS_VALID_EXT_PIN(self->id)) {
//                mp_raise_ValueError(MP_ERROR_TEXT("invalid pin for wake"));
//            }
//
//            if (machine_rtc_config.ext0_pin == -1) {
//                machine_rtc_config.ext0_pin = self->id;
//            } else if (machine_rtc_config.ext0_pin != self->id) {
//                mp_raise_ValueError(MP_ERROR_TEXT("no resources"));
//            }
//
//            machine_rtc_config.ext0_level = trigger == GPIO_PIN_INTR_LOLEVEL ? 0 : 1;
//            machine_rtc_config.ext0_wake_types = wake;
//        } else {
//            if (machine_rtc_config.ext0_pin == self->id) {
//                machine_rtc_config.ext0_pin = -1;
//            }
//
//            if (handler == mp_const_none) {
//                handler = MP_OBJ_NULL;
//                trigger = 0;
//            }
//            gpio_isr_handler_remove(self->id);
//            MP_STATE_PORT(machine_pin_irq_handler)[self->id] = handler;
//            gpio_set_intr_type(self->id, trigger);
//            gpio_isr_handler_add(self->id, machine_pin_isr_handler, (void *)self);
//        }
//    }
//
//    // return the irq object
//    return MP_OBJ_FROM_PTR(&machine_pin_irq_object[self->id]);
//}
//STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_irq_obj, 1, machine_pin_irq);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_debug), MP_ROM_PTR(&machine_pin_debug_obj) },
    //TODO: { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_pin_irq_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(GPIO_MODE_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(GPIO_MODE_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(GPIO_MODE_OPEN_DRAIN) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(GPIO_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(GPIO_PULL_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_PULL_HOLD), MP_ROM_INT(GPIO_PULL_HOLD) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING), MP_ROM_INT(GPIO_PIN_INTR_POSEDGE) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING), MP_ROM_INT(GPIO_PIN_INTR_NEGEDGE) },
    { MP_ROM_QSTR(MP_QSTR_WAKE_LOW), MP_ROM_INT(GPIO_PIN_INTR_LOLEVEL) },
    { MP_ROM_QSTR(MP_QSTR_WAKE_HIGH), MP_ROM_INT(GPIO_PIN_INTR_HILEVEL) },
};

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    switch (request) {
        case MP_PIN_READ: {
            return gpio_get_level(self);
        }
        case MP_PIN_WRITE: {
            gpio_set_level(self, arg);
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
    .locals_dict = (mp_obj_t)&machine_pin_locals_dict,
};

/******************************************************************************/
// Pin IRQ object

// not copied yet
