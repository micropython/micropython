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

typedef struct _pyb_spi_obj_t {
    mp_obj_base_t base;
    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t mosi;
    mp_hal_pin_obj_t miso;
} pyb_spi_obj_t;

STATIC void mp_hal_spi_transfer(pyb_spi_obj_t *self, size_t src_len, const uint8_t *src_buf, size_t dest_len, uint8_t *dest_buf) {
    // only MSB transfer is implemented
    uint32_t delay_half = 500000 / self->baudrate + 1;
    for (size_t i = 0; i < src_len || i < dest_len; ++i) {
        uint8_t data_out;
        if (src_len == 1) {
            data_out = src_buf[0];
        } else {
            data_out = src_buf[i];
        }
        uint8_t data_in = 0;
        for (int j = 0; j < 8; ++j, data_out <<= 1) {
            mp_hal_pin_write(self->mosi, (data_out >> 7) & 1);
            if (self->phase == 0) {
                ets_delay_us(delay_half);
                mp_hal_pin_write(self->sck, 1 - self->polarity);
            } else {
                mp_hal_pin_write(self->sck, 1 - self->polarity);
                ets_delay_us(delay_half);
            }
            data_in = (data_in << 1) | mp_hal_pin_read(self->miso);
            if (self->phase == 0) {
                ets_delay_us(delay_half);
                mp_hal_pin_write(self->sck, self->polarity);
            } else {
                mp_hal_pin_write(self->sck, self->polarity);
                ets_delay_us(delay_half);
            }
        }
        if (dest_len != 0) {
            dest_buf[i] = data_in;
        }
        // make sure pending tasks have a chance to run
        ets_loop_iter();
    }
}

/******************************************************************************/
// MicroPython bindings for SPI

STATIC void pyb_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(baudrate=%u, polarity=%u, phase=%u, sck=%u, mosi=%u, miso=%u)",
        self->baudrate, self->polarity, self->phase, self->sck, self->mosi, self->miso);
}

STATIC void pyb_spi_init_helper(pyb_spi_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_baudrate].u_int != -1) {
        self->baudrate = args[ARG_baudrate].u_int;
    }
    if (args[ARG_polarity].u_int != -1) {
        self->polarity = args[ARG_polarity].u_int;
    }
    if (args[ARG_phase].u_int != -1) {
        self->phase = args[ARG_phase].u_int;
    }
    if (args[ARG_sck].u_obj != MP_OBJ_NULL) {
        self->sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    }
    if (args[ARG_mosi].u_obj != MP_OBJ_NULL) {
        self->mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);
    }
    if (args[ARG_miso].u_obj != MP_OBJ_NULL) {
        self->miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);
    }

    // configure pins
    mp_hal_pin_write(self->sck, self->polarity);
    mp_hal_pin_output(self->sck);
    mp_hal_pin_output(self->mosi);
    mp_hal_pin_input(self->miso);
}

STATIC mp_obj_t pyb_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    pyb_spi_obj_t *self = m_new_obj(pyb_spi_obj_t);
    self->base.type = &pyb_spi_type;
    // set defaults
    self->baudrate = 500000;
    self->polarity = 0;
    self->phase = 0;
    self->sck = 14;
    self->mosi = 13;
    self->miso = 12;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    pyb_spi_init_helper(self, n_args, args, &kw_args);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_spi_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_spi_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_init_obj, 1, pyb_spi_init);

STATIC mp_obj_t pyb_spi_read(size_t n_args, const mp_obj_t *args) {
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint8_t write_byte = 0;
    if (n_args == 3) {
        write_byte = mp_obj_get_int(args[2]);
    }
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(args[1]));
    mp_hal_spi_transfer(self, 1, &write_byte, vstr.len, (uint8_t*)vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_spi_read_obj, 2, 3, pyb_spi_read);

STATIC mp_obj_t pyb_spi_readinto(size_t n_args, const mp_obj_t *args) {
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    uint8_t write_byte = 0;
    if (n_args == 3) {
        write_byte = mp_obj_get_int(args[2]);
    }
    mp_hal_spi_transfer(self, 1, &write_byte, bufinfo.len, (uint8_t*)bufinfo.buf);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_spi_readinto_obj, 2, 3, pyb_spi_readinto);

STATIC mp_obj_t pyb_spi_write(mp_obj_t self_in, mp_obj_t wr_buf_in) {
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t src_buf;
    mp_get_buffer_raise(wr_buf_in, &src_buf, MP_BUFFER_READ);
    mp_hal_spi_transfer(self, src_buf.len, (const uint8_t*)src_buf.buf, 0, NULL);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pyb_spi_write_obj, pyb_spi_write);

STATIC mp_obj_t pyb_spi_write_readinto(mp_obj_t self_in, mp_obj_t wr_buf_in, mp_obj_t rd_buf_in) {
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t src_buf;
    mp_get_buffer_raise(wr_buf_in, &src_buf, MP_BUFFER_READ);
    mp_buffer_info_t dest_buf;
    mp_get_buffer_raise(rd_buf_in, &dest_buf, MP_BUFFER_WRITE);
    if (src_buf.len != dest_buf.len) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "buffers must be the same length"));
    }
    mp_hal_spi_transfer(self, src_buf.len, (const uint8_t*)src_buf.buf, dest_buf.len, (uint8_t*)dest_buf.buf);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(pyb_spi_write_readinto_obj, pyb_spi_write_readinto);

STATIC const mp_rom_map_elem_t pyb_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_spi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&pyb_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&pyb_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&pyb_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&pyb_spi_write_readinto_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);

const mp_obj_type_t pyb_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = pyb_spi_print,
    .make_new = pyb_spi_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_spi_locals_dict,
};
