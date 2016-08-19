/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include <stdint.h>
#include <string.h>

#include "ets_sys.h"
#include "etshal.h"
#include "ets_alt_task.h"

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"


mp_obj_t pyb_spi_make_new(const mp_obj_type_t *type, size_t n_args,
                          size_t n_kw, const mp_obj_t *args);
mp_obj_t pyb_hspi_make_new(const mp_obj_type_t *type, size_t n_args,
                           size_t n_kw, const mp_obj_t *args);


STATIC mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args,
                                     size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    switch (mp_obj_get_int(args[0])) {
        case -1:
            return pyb_spi_make_new(type, n_args - 1, n_kw, args + 1);
        case 0:
            return pyb_hspi_make_new(type, n_args - 1, n_kw, args + 1);
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(
                &mp_type_ValueError, "no such SPI peripheral"));
    }
}


STATIC const mp_rom_map_elem_t machine_spi_locals_dict_table[] = {};

STATIC MP_DEFINE_CONST_DICT(machine_spi_locals_dict,
                            machine_spi_locals_dict_table);

const mp_obj_type_t machine_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .make_new = machine_spi_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_spi_locals_dict,
};
