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

// if a port didn't define MSB/LSB constants then provide them
#ifndef MICROPY_PY_MACHINE_SPI_MSB
#define MICROPY_PY_MACHINE_SPI_MSB (0)
#define MICROPY_PY_MACHINE_SPI_LSB (1)
#endif

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
    }
}

/******************************************************************************/
// MicroPython bindings for generic machine.SPI

STATIC mp_obj_t mp_machine_soft_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

mp_obj_t mp_machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check the id argument, if given
    if (n_args > 0) {
        if (args[0] != MP_OBJ_NEW_SMALL_INT(-1)) {
            #if defined(MICROPY_PY_MACHINE_SPI_MAKE_NEW)
            // dispatch to port-specific constructor
            extern mp_obj_t MICROPY_PY_MACHINE_SPI_MAKE_NEW(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
            return MICROPY_PY_MACHINE_SPI_MAKE_NEW(type, n_args, n_kw, args);
            #else
            mp_raise_ValueError("invalid SPI peripheral");
            #endif
        }
        --n_args;
        ++args;
    }

    // software SPI
    return mp_machine_soft_spi_make_new(type, n_args, n_kw, args);
}

STATIC mp_obj_t machine_spi_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_obj_base_t *s = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t*)s->type->protocol;
    spi_p->init(s, n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_init_obj, 1, machine_spi_init);

STATIC mp_obj_t machine_spi_deinit(mp_obj_t self) {
    mp_obj_base_t *s = (mp_obj_base_t*)MP_OBJ_TO_PTR(self);
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t*)s->type->protocol;
    if (spi_p->deinit != NULL) {
        spi_p->deinit(s);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_spi_deinit_obj, machine_spi_deinit);

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

STATIC const mp_rom_map_elem_t machine_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_spi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_machine_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_machine_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_machine_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&mp_machine_spi_write_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_MSB), MP_ROM_INT(MICROPY_PY_MACHINE_SPI_MSB) },
    { MP_ROM_QSTR(MP_QSTR_LSB), MP_ROM_INT(MICROPY_PY_MACHINE_SPI_LSB) },
};

MP_DEFINE_CONST_DICT(mp_machine_spi_locals_dict, machine_spi_locals_dict_table);

/******************************************************************************/
// Implementation of soft SPI

STATIC uint32_t baudrate_from_delay_half(uint32_t delay_half) {
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (delay_half == MICROPY_PY_MACHINE_SPI_MIN_DELAY) {
        return MICROPY_PY_MACHINE_SPI_MAX_BAUDRATE;
    } else
    #endif
    {
        return 500000 / delay_half;
    }
}

STATIC uint32_t baudrate_to_delay_half(uint32_t baudrate) {
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (baudrate >= MICROPY_PY_MACHINE_SPI_MAX_BAUDRATE) {
        return MICROPY_PY_MACHINE_SPI_MIN_DELAY;
    } else
    #endif
    {
        uint32_t delay_half = 500000 / baudrate;
        // round delay_half up so that: actual_baudrate <= requested_baudrate
        if (500000 % baudrate != 0) {
            delay_half += 1;
        }
        return delay_half;
    }
}

STATIC void mp_machine_soft_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_machine_soft_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SoftSPI(baudrate=%u, polarity=%u, phase=%u,"
        " sck=" MP_HAL_PIN_FMT ", mosi=" MP_HAL_PIN_FMT ", miso=" MP_HAL_PIN_FMT ")",
        baudrate_from_delay_half(self->delay_half), self->polarity, self->phase,
        mp_hal_pin_name(self->sck), mp_hal_pin_name(self->mosi), mp_hal_pin_name(self->miso));
}

STATIC mp_obj_t mp_machine_soft_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 500000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // create new object
    mp_machine_soft_spi_obj_t *self = m_new_obj(mp_machine_soft_spi_obj_t);
    self->base.type = &mp_machine_soft_spi_type;

    // set parameters
    self->delay_half = baudrate_to_delay_half(args[ARG_baudrate].u_int);
    self->polarity = args[ARG_polarity].u_int;
    self->phase = args[ARG_phase].u_int;
    if (args[ARG_bits].u_int != 8) {
        mp_raise_ValueError("bits must be 8");
    }
    if (args[ARG_firstbit].u_int != MICROPY_PY_MACHINE_SPI_MSB) {
        mp_raise_ValueError("firstbit must be MSB");
    }
    if (args[ARG_sck].u_obj == MP_OBJ_NULL
        || args[ARG_mosi].u_obj == MP_OBJ_NULL
        || args[ARG_miso].u_obj == MP_OBJ_NULL) {
        mp_raise_ValueError("must specify all of sck/mosi/miso");
    }
    self->sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    self->mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);
    self->miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);

    // configure pins
    mp_hal_pin_write(self->sck, self->polarity);
    mp_hal_pin_output(self->sck);
    mp_hal_pin_output(self->mosi);
    mp_hal_pin_input(self->miso);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void mp_machine_soft_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_machine_soft_spi_obj_t *self = (mp_machine_soft_spi_obj_t*)self_in;

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
        self->delay_half = baudrate_to_delay_half(args[ARG_baudrate].u_int);
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

const mp_machine_spi_p_t mp_machine_soft_spi_p = {
    .init = mp_machine_soft_spi_init,
    .deinit = NULL,
    .transfer = mp_machine_soft_spi_transfer,
};

const mp_obj_type_t mp_machine_soft_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SoftSPI,
    .print = mp_machine_soft_spi_print,
    .make_new = mp_machine_spi_make_new, // delegate to master constructor
    .protocol = &mp_machine_soft_spi_p,
    .locals_dict = (mp_obj_dict_t*)&mp_machine_spi_locals_dict,
};

#endif // MICROPY_PY_MACHINE_SPI
