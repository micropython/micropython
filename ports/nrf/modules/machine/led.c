/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
 * Copyright (c) 2015 Glenn Ruben Bakke
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

#include "py/runtime.h"

#include "mphalport.h"
#include "led.h"
#include "mpconfigboard.h"

#if MICROPY_HW_HAS_LED

#define LED_OFF(pin) {(MICROPY_HW_LED_PULLUP) ? nrf_gpio_pin_set(pin) : nrf_gpio_pin_clear(pin); }
#define LED_ON(pin) {(MICROPY_HW_LED_PULLUP) ? nrf_gpio_pin_clear(pin) : nrf_gpio_pin_set(pin); }

typedef struct _pyb_led_obj_t {
    mp_obj_base_t base;
    mp_uint_t led_id;
    mp_uint_t hw_pin;
    uint8_t hw_pin_port;
} pyb_led_obj_t;

STATIC const pyb_led_obj_t pyb_led_obj[] = {
#if MICROPY_HW_LED_TRICOLOR
    {{&pyb_led_type}, PYB_LED_RED, MICROPY_HW_LED_RED},
    {{&pyb_led_type}, PYB_LED_GREEN, MICROPY_HW_LED_GREEN},
    {{&pyb_led_type}, PYB_LED_BLUE, MICROPY_HW_LED_BLUE},
#elif (MICROPY_HW_LED_COUNT == 1)
    {{&pyb_led_type}, PYB_LED1, MICROPY_HW_LED1},
#elif (MICROPY_HW_LED_COUNT == 2)
    {{&pyb_led_type}, PYB_LED1, MICROPY_HW_LED1},
    {{&pyb_led_type}, PYB_LED2, MICROPY_HW_LED2},
#else
    {{&pyb_led_type}, PYB_LED1, MICROPY_HW_LED1},
    {{&pyb_led_type}, PYB_LED2, MICROPY_HW_LED2},
    {{&pyb_led_type}, PYB_LED3, MICROPY_HW_LED3},
    {{&pyb_led_type}, PYB_LED4, MICROPY_HW_LED4},
#endif
};

#define NUM_LEDS MP_ARRAY_SIZE(pyb_led_obj)

void led_init(void) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        LED_OFF(pyb_led_obj[i].hw_pin);
        nrf_gpio_cfg_output(pyb_led_obj[i].hw_pin);
    }
}

void led_state(pyb_led_obj_t * led_obj, int state) {
    if (state == 1) {
        LED_ON(led_obj->hw_pin);
    } else {
        LED_OFF(led_obj->hw_pin);
    }
}

void led_toggle(pyb_led_obj_t * led_obj) {
    nrf_gpio_pin_toggle(led_obj->hw_pin);
}



/******************************************************************************/
/* MicroPython bindings                                                      */

void led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_led_obj_t *self = self_in;
    mp_printf(print, "LED(%lu)", self->led_id);
}

/// \classmethod \constructor(id)
/// Create an LED object associated with the given LED:
///
///   - `id` is the LED number, 1-4.
STATIC mp_obj_t led_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get led number
    mp_int_t led_id = mp_obj_get_int(args[0]);

    // check led number
    if (!(1 <= led_id && led_id <= NUM_LEDS)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "LED(%d) does not exist", led_id));
    }

    // return static led object
    return (mp_obj_t)&pyb_led_obj[led_id - 1];
}

/// \method on()
/// Turn the LED on.
mp_obj_t led_obj_on(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_state(self, 1);
    return mp_const_none;
}

/// \method off()
/// Turn the LED off.
mp_obj_t led_obj_off(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_state(self, 0);
    return mp_const_none;
}

/// \method toggle()
/// Toggle the LED between on and off.
mp_obj_t led_obj_toggle(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_toggle(self);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_toggle_obj, led_obj_toggle);

STATIC const mp_rom_map_elem_t led_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&led_obj_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&led_obj_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&led_obj_toggle_obj) },
};

STATIC MP_DEFINE_CONST_DICT(led_locals_dict, led_locals_dict_table);

const mp_obj_type_t pyb_led_type = {
    { &mp_type_type },
    .name = MP_QSTR_LED,
    .print = led_obj_print,
    .make_new = led_obj_make_new,
    .locals_dict = (mp_obj_dict_t*)&led_locals_dict,
};

#endif // MICROPY_HW_HAS_LED
