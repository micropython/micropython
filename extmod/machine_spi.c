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
#include <string.h>

#include "py/runtime.h"
#include "extmod/machine_spi.h"

#if MICROPY_PY_MACHINE_SPI

void mp_machine_soft_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    mp_machine_soft_spi_obj_t *self = (mp_machine_soft_spi_obj_t*)self_in;
    uint32_t delay_half = self->delay_half;

    // only MSB transfer is implemented

    // If a port defines MICROPY_PY_MACHINE_SPI_MIN_DELAY, and the configured
    // delay_half is equal to this value, then the software SPI implementation
    // will run as fast as possible, limited only by CPU speed and GPIO time.
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (delay_half == MICROPY_PY_MACHINE_SPI_MIN_DELAY) {
        for (size_t i = 0; i < len; ++i) {
            uint8_t data_out = src[i];
            uint8_t data_in = 0;
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                mp_hal_pin_write(self->mosi, (data_out >> 7) & 1);
                mp_hal_pin_write(self->sck, 1 - self->polarity);
                data_in = (data_in << 1) | mp_hal_pin_read(self->miso);
                mp_hal_pin_write(self->sck, self->polarity);
            }
            if (dest != NULL) {
                dest[i] = data_in;
            }
        }
        return;
    }
    #endif

    for (size_t i = 0; i < len; ++i) {
        uint8_t data_out = src[i];
        uint8_t data_in = 0;
        for (int j = 0; j < 8; ++j, data_out <<= 1) {
            mp_hal_pin_write(self->mosi, (data_out >> 7) & 1);
            if (self->phase == 0) {
                mp_hal_delay_us_fast(delay_half);
                mp_hal_pin_write(self->sck, 1 - self->polarity);
            } else {
                mp_hal_pin_write(self->sck, 1 - self->polarity);
                mp_hal_delay_us_fast(delay_half);
            }
            data_in = (data_in << 1) | mp_hal_pin_read(self->miso);
            if (self->phase == 0) {
                mp_hal_delay_us_fast(delay_half);
                mp_hal_pin_write(self->sck, self->polarity);
            } else {
                mp_hal_pin_write(self->sck, self->polarity);
                mp_hal_delay_us_fast(delay_half);
            }
        }
        if (dest != NULL) {
            dest[i] = data_in;
        }

        // Some ports need a regular callback, but probably we don't need
        // to do this every byte, or even at all.
        #ifdef MICROPY_EVENT_POLL_HOOK
        MICROPY_EVENT_POLL_HOOK;
        #endif
    }
}

STATIC void mp_machine_spi_transfer(mp_obj_t self, size_t len, const void *src, void *dest) {
    mp_obj_base_t *s = (mp_obj_base_t*)MP_OBJ_TO_PTR(self);
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t*)s->type->protocol;
    spi_p->transfer(s, len, src, dest);
}

STATIC mp_obj_t mp_machine_spi_read(size_t n_args, const mp_obj_t *args) {
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(args[1]));
    memset(vstr.buf, n_args == 3 ? mp_obj_get_int(args[2]) : 0, vstr.len);
    mp_machine_spi_transfer(args[0], vstr.len, vstr.buf, vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_spi_read_obj, 2, 3, mp_machine_spi_read);

STATIC mp_obj_t mp_machine_spi_readinto(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, n_args == 3 ? mp_obj_get_int(args[2]) : 0, bufinfo.len);
    mp_machine_spi_transfer(args[0], bufinfo.len, bufinfo.buf, bufinfo.buf);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_spi_readinto_obj, 2, 3, mp_machine_spi_readinto);

STATIC mp_obj_t mp_machine_spi_write(mp_obj_t self, mp_obj_t wr_buf) {
    mp_buffer_info_t src;
    mp_get_buffer_raise(wr_buf, &src, MP_BUFFER_READ);
    mp_machine_spi_transfer(self, src.len, (const uint8_t*)src.buf, NULL);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_spi_write_obj, mp_machine_spi_write);

STATIC mp_obj_t mp_machine_spi_write_readinto(mp_obj_t self, mp_obj_t wr_buf, mp_obj_t rd_buf) {
    mp_buffer_info_t src;
    mp_get_buffer_raise(wr_buf, &src, MP_BUFFER_READ);
    mp_buffer_info_t dest;
    mp_get_buffer_raise(rd_buf, &dest, MP_BUFFER_WRITE);
    if (src.len != dest.len) {
        mp_raise_ValueError("buffers must be the same length");
    }
    mp_machine_spi_transfer(self, src.len, src.buf, dest.buf);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(mp_machine_spi_write_readinto_obj, mp_machine_spi_write_readinto);

#endif // MICROPY_PY_MACHINE_SPI
