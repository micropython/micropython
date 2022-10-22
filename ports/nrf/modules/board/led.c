/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
 * Copyright (c) 2015 - 2018 Glenn Ruben Bakke
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

typedef struct _board_led_obj_t {
    mp_obj_base_t base;
    mp_uint_t led_id;
    mp_uint_t hw_pin;
    uint8_t hw_pin_port;
    bool pullup;
} board_led_obj_t;

static inline void led_off(board_led_obj_t * const led_obj) {
    if (led_obj->pullup) {
        nrf_gpio_pin_set(led_obj->hw_pin);
    }
    else {
        nrf_gpio_pin_clear(led_obj->hw_pin);
    }
}

static inline void led_on(board_led_obj_t * const led_obj) {
    if (led_obj->pullup) {
        nrf_gpio_pin_clear(led_obj->hw_pin);
    }
    else {
        nrf_gpio_pin_set(led_obj->hw_pin);
    }
}


static const board_led_obj_t board_led_obj[] = {

#if MICROPY_HW_LED_TRICOLOR

    {{&board_led_type}, BOARD_LED_RED, MICROPY_HW_LED_RED, 0, MICROPY_HW_LED_PULLUP},
    {{&board_led_type}, BOARD_LED_GREEN, MICROPY_HW_LED_GREEN, 0, MICROPY_HW_LED_PULLUP},
    {{&board_led_type}, BOARD_LED_BLUE, MICROPY_HW_LED_BLUE, 0, MICROPY_HW_LED_PULLUP},
#endif
#if (MICROPY_HW_LED_COUNT >= 1)
    {{&board_led_type}, BOARD_LED1, MICROPY_HW_LED1, 0,
    #ifdef MICROPY_HW_LED1_PULLUP
        MICROPY_HW_LED1_PULLUP
    #else
        MICROPY_HW_LED_PULLUP
    #endif
    },
#endif
#if (MICROPY_HW_LED_COUNT >= 2)
    {{&board_led_type}, BOARD_LED2, MICROPY_HW_LED2, 0,
    #ifdef MICROPY_HW_LED2_PULLUP
        MICROPY_HW_LED2_PULLUP
    #else
        MICROPY_HW_LED_PULLUP
    #endif
    },
#endif
#if (MICROPY_HW_LED_COUNT >= 3)
    {{&board_led_type}, BOARD_LED3, MICROPY_HW_LED3, 0,
    #ifdef MICROPY_HW_LED3_PULLUP
        MICROPY_HW_LED3_PULLUP
    #else
        MICROPY_HW_LED_PULLUP
    #endif
    },
#endif
#if (MICROPY_HW_LED_COUNT == 4)
    {{&board_led_type}, BOARD_LED4, MICROPY_HW_LED4, 0,
    #ifdef MICROPY_HW_LED4_PULLUP
        MICROPY_HW_LED4_PULLUP
    #else
        MICROPY_HW_LED_PULLUP
    #endif
    },
#endif
};

#define NUM_LEDS MP_ARRAY_SIZE(board_led_obj)

void led_init(void) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        led_off((board_led_obj_t*)&board_led_obj[i]);
        nrf_gpio_cfg_output(board_led_obj[i].hw_pin);
    }
}

void led_state(board_led_t led, int state) {
    if (state == 1) {
        led_on((board_led_obj_t*)&board_led_obj[led-1]);

    } else {
        led_off((board_led_obj_t*)&board_led_obj[led-1]);
    }
}

void led_toggle(board_led_t led) {
    nrf_gpio_pin_toggle(board_led_obj[led-1].hw_pin);
}



/******************************************************************************/
/* MicroPython bindings                                                      */

void led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    board_led_obj_t *self = self_in;
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
        mp_raise_ValueError(MP_ERROR_TEXT("LED doesn't exist"));
    }

    // return static led object
    return (mp_obj_t)&board_led_obj[led_id - 1];
}

/// \method on()
/// Turn the LED on.
mp_obj_t led_obj_on(mp_obj_t self_in) {
    board_led_obj_t *self = self_in;
    led_state(self->led_id, 1);
    return mp_const_none;
}

/// \method off()
/// Turn the LED off.
mp_obj_t led_obj_off(mp_obj_t self_in) {
    board_led_obj_t *self = self_in;
    led_state(self->led_id, 0);
    return mp_const_none;
}

/// \method toggle()
/// Toggle the LED between on and off.
mp_obj_t led_obj_toggle(mp_obj_t self_in) {
    board_led_obj_t *self = self_in;
    led_toggle(self->led_id);
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

MP_DEFINE_CONST_OBJ_TYPE(
    board_led_type,
    MP_QSTR_LED,
    MP_TYPE_FLAG_NONE,
    make_new, led_obj_make_new,
    print, led_obj_print,
    locals_dict, &led_locals_dict
    );

#else
// For boards with no LEDs, we leave an empty function here so that we don't
// have to put conditionals everywhere.
void led_init(void) {
}
void led_state(board_led_t led, int state) {
}
void led_toggle(board_led_t led) {
}
#endif // MICROPY_HW_HAS_LED
