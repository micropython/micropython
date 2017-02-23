/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "temp.h"
#include "hal_temp.h"

#if MICROPY_PY_MACHINE_TEMP

typedef struct _machine_temp_obj_t {
    mp_obj_base_t base;
    TEMP_HandleTypeDef *temp;
} machine_temp_obj_t;

TEMP_HandleTypeDef TEMPHandle0 = {.instance = NULL };


STATIC const machine_temp_obj_t machine_temp_obj = {
    {&machine_temp_type}, &TEMPHandle0
};

/// \method __str__()
/// Return a string describing the ADC object.
STATIC void machine_temp_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_temp_obj_t *self = o;

    (void)self;

    mp_printf(print, "TEMP");
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

STATIC mp_obj_t machine_temp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    memset(&TEMPHandle0, 0, sizeof(TEMP_HandleTypeDef));
    TEMPHandle0.instance = TEMP_BASE;

    const machine_temp_obj_t *self = &machine_temp_obj;

    return MP_OBJ_FROM_PTR(self);
}

/// \method read()
/// Get temperature.
mp_obj_t machine_temp_read(void) {
    int32_t volatile temp;

    NRF_TEMP->TASKS_START = 1; /** Start the temperature measurement. */

    /* Busy wait while temperature measurement is not finished, you can skip waiting if you enable interrupt for DATARDY event and read the result in the interrupt. */
    /*lint -e{845} // A zero has been given as right argument to operator '|'" */
    while (NRF_TEMP->EVENTS_DATARDY == 0)
    {
        // Do nothing.
    }
    NRF_TEMP->EVENTS_DATARDY = 0;

    /**@note Workaround for PAN_028 rev2.0A anomaly 29 - TEMP: Stop task clears the TEMP register. */
    temp = (hal_temp_read() / 4);

    /**@note Workaround for PAN_028 rev2.0A anomaly 30 - TEMP: Temp module analog front end does not power down when DATARDY event occurs. */
    NRF_TEMP->TASKS_STOP = 1; /** Stop the temperature measurement. */

    return MP_OBJ_NEW_SMALL_INT(temp);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_machine_temp_read_obj, machine_temp_read);

STATIC const mp_map_elem_t machine_temp_locals_dict_table[] = {
    // instance methods
    // class methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_machine_temp_read_obj },
};

STATIC MP_DEFINE_CONST_DICT(machine_temp_locals_dict, machine_temp_locals_dict_table);

const mp_obj_type_t machine_temp_type = {
    { &mp_type_type },
    .name = MP_QSTR_TEMP,
    .make_new = machine_temp_make_new,
    .locals_dict = (mp_obj_t)&machine_temp_locals_dict,
    .print = machine_temp_print,
};

#endif // MICROPY_PY_MACHINE_TEMP
