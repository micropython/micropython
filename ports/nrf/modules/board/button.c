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
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "mphalport.h"
#include "button.h"
#include "mpconfigboard.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#if MICROPY_HW_HAS_BUTTON


typedef struct _board_button_obj_t {
    mp_obj_base_t base;
    mp_uint_t button_id;
    mp_uint_t hw_pin;
    uint8_t hw_pin_port;
} board_button_obj_t;

STATIC const board_button_obj_t board_button_obj[] = {
#if (MICROPY_HW_BUTTON_COUNT == 1)
    {{&board_button_type}, BOARD_BUTTON1, MICROPY_HW_BUTTON1},
#elif (MICROPY_HW_BUTTON_COUNT == 2)
    {{&board_button_type}, BOARD_BUTTON1, MICROPY_HW_BUTTON1},
    {{&board_button_type}, BOARD_BUTTON2, MICROPY_HW_BUTTON2},
#else
    {{&board_button_type}, BOARD_BUTTON1, MICROPY_HW_BUTTON1},
    {{&board_button_type}, BOARD_BUTTON2, MICROPY_HW_BUTTON2},
    {{&board_button_type}, BOARD_BUTTON3, MICROPY_HW_BUTTON3},
    {{&board_button_type}, BOARD_BUTTON4, MICROPY_HW_BUTTON4},
#endif
};

#define NUM_BUTTONS MP_ARRAY_SIZE(board_button_obj)

void button_init(void) {
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        nrf_gpio_cfg_input(board_button_obj[i].hw_pin,NRF_GPIO_PIN_PULLUP);
    }
}

/******************************************************************************/
/* MicroPython bindings                                                      */

void button_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    board_button_obj_t *self = self_in;
    mp_printf(print, "BUTTON(%lu)", self->button_id);
}

/// \classmethod \constructor(id)
/// Create an BUTTON object associated with the given BUTTON:
///
///   - `id` is the BUTTON number, 1-4.
STATIC mp_obj_t button_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get button number
    mp_int_t button_id = mp_obj_get_int(args[0]);

    // check button number
    if (!(1 <= button_id && button_id <= NUM_BUTTONS)) {
        mp_raise_ValueError("BUTTON doesn't exist");
    }

    // return static button object
    return (mp_obj_t)&board_button_obj[button_id - 1];
}

/// \method isPressed()
/// return the button state.
mp_obj_t button_obj_isPressed(mp_obj_t self_in) {
    board_button_obj_t *self = self_in;
    // get pin
    return mp_obj_new_bool(!(nrf_gpio_pin_read(self->hw_pin)));
}

/// \method getPresses()
/// return the button state.
mp_obj_t button_obj_getPresses(mp_obj_t self_in) {
    board_button_obj_t *self = self_in;
    // get pin
    return MP_OBJ_NEW_SMALL_INT(nrf_gpio_pin_read(self->hw_pin));
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(button_obj_isPressed_obj, button_obj_isPressed);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(button_obj_getPresses_obj, button_obj_getPresses);

STATIC const mp_rom_map_elem_t button_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_isPressed), MP_ROM_PTR(&button_obj_isPressed_obj) },
    { MP_ROM_QSTR(MP_QSTR_getPresses), MP_ROM_PTR(&button_obj_getPresses_obj) },
};

STATIC MP_DEFINE_CONST_DICT(button_locals_dict, button_locals_dict_table);

const mp_obj_type_t board_button_type = {
    { &mp_type_type },
    .name = MP_QSTR_BUTTON,
    .print = button_obj_print,
    .make_new = button_obj_make_new,
    .locals_dict = (mp_obj_dict_t*)&button_locals_dict,
};

#endif // MICROPY_HW_HAS_BUTTON
