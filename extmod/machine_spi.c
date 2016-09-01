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

#include "py/runtime.h"
#include "extmod/machine_spi.h"

#if MICROPY_PY_MACHINE_SPI

STATIC void mp_machine_spi_transfer(mp_obj_t self, size_t slen, const uint8_t *src, size_t dlen, uint8_t *dest) {
    mp_obj_base_t *s = (mp_obj_base_t*)MP_OBJ_TO_PTR(self);
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t*)s->type->protocol;
    spi_p->transfer(s, slen, src, dlen, dest);
}

STATIC mp_obj_t mp_machine_spi_read(size_t n_args, const mp_obj_t *args) {
    uint8_t write_byte = 0;
    if (n_args == 3) {
        write_byte = mp_obj_get_int(args[2]);
    }
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(args[1]));
    mp_machine_spi_transfer(args[0], 1, &write_byte, vstr.len, (uint8_t*)vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_spi_read_obj, 2, 3, mp_machine_spi_read);

STATIC mp_obj_t mp_machine_spi_readinto(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    uint8_t write_byte = 0;
    if (n_args == 3) {
        write_byte = mp_obj_get_int(args[2]);
    }
    mp_machine_spi_transfer(args[0], 1, &write_byte, bufinfo.len, (uint8_t*)bufinfo.buf);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_spi_readinto_obj, 2, 3, mp_machine_spi_readinto);

STATIC mp_obj_t mp_machine_spi_write(mp_obj_t self, mp_obj_t wr_buf) {
    mp_buffer_info_t src;
    mp_get_buffer_raise(wr_buf, &src, MP_BUFFER_READ);
    mp_machine_spi_transfer(self, src.len, (const uint8_t*)src.buf, 0, NULL);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_spi_write_obj, mp_machine_spi_write);

STATIC mp_obj_t mp_machine_spi_write_readinto(mp_obj_t self, mp_obj_t wr_buf, mp_obj_t rd_buf) {
    mp_buffer_info_t src;
    mp_get_buffer_raise(wr_buf, &src, MP_BUFFER_READ);
    mp_buffer_info_t dest;
    mp_get_buffer_raise(rd_buf, &dest, MP_BUFFER_WRITE);
    if (src.len != dest.len) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "buffers must be the same length"));
    }
    mp_machine_spi_transfer(self, src.len, (const uint8_t*)src.buf, dest.len, (uint8_t*)dest.buf);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(mp_machine_spi_write_readinto_obj, mp_machine_spi_write_readinto);

#endif // MICROPY_PY_MACHINE_SPI
