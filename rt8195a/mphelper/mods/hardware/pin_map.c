/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
 * Copyright (c) 2016 Chester Tseng
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

#include "py/obj.h"

// Ameba sdk
#include "PinNames.h"

STATIC const mp_map_elem_t pin_board_pins_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA_0),        MP_OBJ_NEW_SMALL_INT(PA_0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA_1),        MP_OBJ_NEW_SMALL_INT(PA_1) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA_2),        MP_OBJ_NEW_SMALL_INT(PA_2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA_3),        MP_OBJ_NEW_SMALL_INT(PA_3) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA_4),        MP_OBJ_NEW_SMALL_INT(PA_4) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA_5),        MP_OBJ_NEW_SMALL_INT(PA_5) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA_6),        MP_OBJ_NEW_SMALL_INT(PA_6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PA_7),        MP_OBJ_NEW_SMALL_INT(PA_7) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_PB_0),        MP_OBJ_NEW_SMALL_INT(PB_0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB_1),        MP_OBJ_NEW_SMALL_INT(PB_1) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB_2),        MP_OBJ_NEW_SMALL_INT(PB_2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB_3),        MP_OBJ_NEW_SMALL_INT(PB_3) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB_4),        MP_OBJ_NEW_SMALL_INT(PB_4) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB_5),        MP_OBJ_NEW_SMALL_INT(PB_5) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB_6),        MP_OBJ_NEW_SMALL_INT(PB_6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PB_7),        MP_OBJ_NEW_SMALL_INT(PB_7) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_PC_0),        MP_OBJ_NEW_SMALL_INT(PC_0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC_1),        MP_OBJ_NEW_SMALL_INT(PC_1) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC_2),        MP_OBJ_NEW_SMALL_INT(PC_2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC_3),        MP_OBJ_NEW_SMALL_INT(PC_3) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC_4),        MP_OBJ_NEW_SMALL_INT(PC_4) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC_5),        MP_OBJ_NEW_SMALL_INT(PC_5) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC_6),        MP_OBJ_NEW_SMALL_INT(PC_6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PC_7),        MP_OBJ_NEW_SMALL_INT(PC_7) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_PD_0),        MP_OBJ_NEW_SMALL_INT(PD_0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD_1),        MP_OBJ_NEW_SMALL_INT(PD_1) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD_2),        MP_OBJ_NEW_SMALL_INT(PD_2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD_3),        MP_OBJ_NEW_SMALL_INT(PD_3) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD_4),        MP_OBJ_NEW_SMALL_INT(PD_4) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD_5),        MP_OBJ_NEW_SMALL_INT(PD_5) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD_6),        MP_OBJ_NEW_SMALL_INT(PD_6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PD_7),        MP_OBJ_NEW_SMALL_INT(PD_7) },
};

MP_DEFINE_CONST_DICT(pin_board_pins_locals_dict, pin_board_pins_locals_dict_table);
