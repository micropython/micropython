/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#include "py/stream.h"
#include "board.h"
#include "modpyb.h"
#include "ciaanxp_mphal.h"

typedef struct _pyb_spi_obj_t {
    mp_obj_base_t base;
} pyb_spi_obj_t;

STATIC pyb_spi_obj_t pyb_spi_obj[] = {
    {{&pyb_spi_type}}
};

#define SPI_ID(obj) ((obj) - &pyb_spi_obj[0]) + 1

void pyb_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_spi_obj_t *self = self_in;
    mp_printf(print, "SPI (%u)", SPI_ID(self) );
}

STATIC mp_obj_t pyb_spi_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 3, 3, false);

    mp_int_t bits = mp_obj_get_int(args[0]);
    mp_int_t mode = mp_obj_get_int(args[1]);
    mp_int_t bitrate = mp_obj_get_int(args[2]);

    mp_hal_configSPI(bits, mode, bitrate);

    return (mp_obj_t)&pyb_spi_obj[0];
}



// functions used by streams
STATIC mp_uint_t pyb_spi_write_m(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {

    const byte *buf = buf_in;

    // write the data
    mp_hal_writeSPI(buf, size);
    return size+1;
}


STATIC mp_uint_t pyb_spi_read_m(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    byte *buf = buf_in;

    if (size == 0) {
        return 0;
    }

    return mp_hal_readSPI(buf, size);
}




STATIC const mp_map_elem_t pyb_spi_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);


STATIC const mp_stream_p_t spi_stream_p = {
    .read = pyb_spi_read_m,
    .write = pyb_spi_write_m,
    .is_text = false,
};

const mp_obj_type_t pyb_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = pyb_spi_print,
    .make_new = pyb_spi_make_new,
    .stream_p = &spi_stream_p,
    .locals_dict = (mp_obj_t)&pyb_spi_locals_dict,
};
