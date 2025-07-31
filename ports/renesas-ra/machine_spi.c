/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"
#include "pin.h"
#include "spi.h"
#include "ra/ra_spi.h"

typedef struct _machine_hard_spi_obj_t {
    mp_obj_base_t base;
    uint8_t spi_id;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
    uint32_t baudrate;
    mp_hal_pin_obj_t sck;
    mp_hal_pin_obj_t mosi;
    mp_hal_pin_obj_t miso;
} machine_hard_spi_obj_t;

#define DEFAULT_SPI_BAUDRATE      (500000)
#define DEFAULT_SPI_POLARITY      (0)
#define DEFAULT_SPI_PHASE         (0)
#define DEFAULT_SPI_BITS          (8)
#define DEFAULT_SPI_FIRSTBIT      (MICROPY_PY_MACHINE_SPI_MSB)

#define IS_VALID_SCK(obj_pin, arg_pin)      ((obj_pin) == (arg_pin))
#define IS_VALID_MOSI(obj_pin, arg_pin)     ((obj_pin) == (arg_pin))
#define IS_VALID_MISO(obj_pin, arg_pin)     ((obj_pin) == (arg_pin))
#define IS_VALID_POLARITY(n)                (((n) == 0) || ((n) == 1))
#define IS_VALID_PHASE(n)                   (((n) == 0) || ((n) == 1))
#define IS_VALID_BITS(n)                    (((n) == 8) || ((n) == 16) || ((n) == 32))
#define IS_VALID_FIRSTBIT(n)                ((n) == MICROPY_PY_MACHINE_SPI_LSB)

/******************************************************************************/
// Implementation of hard SPI for machine module

static machine_hard_spi_obj_t machine_hard_spi_obj[] = {
    #if defined(MICROPY_HW_SPI0_RSPCK)
    {
        {&machine_spi_type}, 0,
        DEFAULT_SPI_POLARITY, DEFAULT_SPI_PHASE, DEFAULT_SPI_BITS,
        DEFAULT_SPI_FIRSTBIT, DEFAULT_SPI_BAUDRATE,
        MICROPY_HW_SPI0_RSPCK, MICROPY_HW_SPI0_MOSI, MICROPY_HW_SPI0_MISO,
    },
    #endif
    #if defined(MICROPY_HW_SPI1_RSPCK)
    {
        {&machine_spi_type}, 1,
        DEFAULT_SPI_POLARITY, DEFAULT_SPI_PHASE, DEFAULT_SPI_BITS,
        DEFAULT_SPI_FIRSTBIT, DEFAULT_SPI_BAUDRATE,
        MICROPY_HW_SPI1_RSPCK, MICROPY_HW_SPI1_MOSI, MICROPY_HW_SPI1_MISO,
    },
    #endif
};

static void spi_init(machine_hard_spi_obj_t *self) {
    const machine_pin_obj_t *pins[4] = { NULL, NULL, NULL, NULL };

    if (0) {
    #if defined(MICROPY_HW_SPI0_RSPCK)
    } else if (self->spi_id == 0) {
        #if defined(MICROPY_HW_SPI0_SSL)
        pins[0] = MICROPY_HW_SPI0_SSL;
        #endif
        #if defined(MICROPY_HW_SPI0_RSPCK)
        pins[1] = MICROPY_HW_SPI0_RSPCK;
        #endif
        #if defined(MICROPY_HW_SPI0_MISO)
        pins[2] = MICROPY_HW_SPI0_MISO;
        #endif
        #if defined(MICROPY_HW_SPI0_MOSI)
        pins[3] = MICROPY_HW_SPI0_MOSI;
        #endif
    #endif
    #if defined(MICROPY_HW_SPI1_RSPCK)
    } else if (self->spi_id == 1) {
        #if defined(MICROPY_HW_SPI1_SSL)
        pins[0] = MICROPY_HW_SPI1_SSL;
        #endif
        #if defined(MICROPY_HW_SPI1_RSPCK)
        pins[1] = MICROPY_HW_SPI1_RSPCK;
        #endif
        #if defined(MICROPY_HW_SPI1_MISO)
        pins[2] = MICROPY_HW_SPI1_MISO;
        #endif
        #if defined(MICROPY_HW_SPI1_MOSI)
        pins[3] = MICROPY_HW_SPI1_MOSI;
        #endif
    #endif
    } else {
        // SPI does not exist for this board (shouldn't get here, should be checked by caller)
        return;
    }
    ra_spi_init(self->spi_id, pins[3]->pin, pins[2]->pin, pins[1]->pin, pins[0]->pin, self->baudrate, self->bits, self->polarity, self->phase, self->firstbit);
}

static void machine_hard_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%u, sck=%q, mosi=%q, miso=%q)",
        self->spi_id, self->baudrate, self->polarity, self->phase, self->bits,
        self->firstbit, self->sck->name, self->mosi->name, self->miso->name);
}

mp_obj_t machine_hard_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    MP_MACHINE_SPI_CHECK_FOR_LEGACY_SOFTSPI_CONSTRUCTION(n_args, n_kw, all_args);

    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the SPI bus id.
    bool found = false;
    int spi_id = mp_obj_get_int(args[ARG_id].u_obj);
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)&machine_hard_spi_obj[0];
    for (int i = 0; i < MP_ARRAY_SIZE(machine_hard_spi_obj); i++) {
        if (spi_id == self->spi_id) {
            found = true;
            break;
        }
        ++self;
    }
    if (found != true) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesno't exist"), spi_id);
    }

    if (args[ARG_baudrate].u_int != -1) {
        self->baudrate = args[ARG_baudrate].u_int;
    }
    if (args[ARG_polarity].u_int != -1) {
        if (IS_VALID_POLARITY(args[ARG_polarity].u_int)) {
            self->polarity = args[ARG_polarity].u_int;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("bad polarity"));
        }
    }
    if (args[ARG_phase].u_int != -1) {
        if (IS_VALID_PHASE(args[ARG_phase].u_int)) {
            self->phase = args[ARG_phase].u_int;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("bad phase"));
        }
    }
    if (args[ARG_bits].u_int != -1) {
        if (IS_VALID_BITS(args[ARG_bits].u_int)) {
            self->bits = args[ARG_bits].u_int;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("bad bits"));
        }
    }
    if (args[ARG_firstbit].u_int != -1) {
        if (IS_VALID_FIRSTBIT(args[ARG_firstbit].u_int)) {
            self->firstbit = args[ARG_firstbit].u_int;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("bad firstbit"));
        }
    }
    // Set SCK/MOSI/MISO pins if configured.
    // currently pins are fixed, can not be changed.
    uint8_t sck, mosi, miso;

    if (args[ARG_sck].u_obj == MP_OBJ_NULL) {
        sck = self->sck->pin;
    } else {
        const machine_pin_obj_t *arg_sck = machine_pin_find(args[ARG_sck].u_obj);
        sck = arg_sck->pin;
        if (!IS_VALID_SCK(self->sck->pin, sck)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SCK pin"));
        }
    }
    if (args[ARG_mosi].u_obj == MP_OBJ_NULL) {
        mosi = self->mosi->pin;
    } else {
        const machine_pin_obj_t *arg_mosi = machine_pin_find(args[ARG_mosi].u_obj);
        mosi = arg_mosi->pin;
        if (!IS_VALID_MOSI(self->mosi->pin, mosi)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad MOSI pin"));
        }
    }
    if (args[ARG_miso].u_obj == MP_OBJ_NULL) {
        miso = self->miso->pin;
    } else {
        const machine_pin_obj_t *arg_miso = machine_pin_find(args[ARG_miso].u_obj);
        miso = arg_miso->pin;
        if (!IS_VALID_MISO(self->miso->pin, miso)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad MISO pin"));
        }
    }
    // init the SPI bus
    spi_init(self);
    return MP_OBJ_FROM_PTR(self);
}

static void machine_hard_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)self_in;

    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Set SCK/MOSI/MISO pins if configured.
    // currently pins are fixed, can not be changed.
    uint8_t sck, mosi, miso;

    if (args[ARG_baudrate].u_int != -1) {
        self->baudrate = args[ARG_baudrate].u_int;
    }
    if (args[ARG_polarity].u_int != -1) {
        if (IS_VALID_POLARITY(args[ARG_polarity].u_int)) {
            self->polarity = args[ARG_polarity].u_int;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("bad polarity"));
        }
    }
    if (args[ARG_phase].u_int != -1) {
        if (IS_VALID_PHASE(args[ARG_phase].u_int)) {
            self->phase = args[ARG_phase].u_int;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("bad phase"));
        }
    }
    if (args[ARG_bits].u_int != -1) {
        if (IS_VALID_BITS(args[ARG_bits].u_int)) {
            self->bits = args[ARG_bits].u_int;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("bad bits"));
        }
    }
    if (args[ARG_firstbit].u_int != -1) {
        if (IS_VALID_FIRSTBIT(args[ARG_firstbit].u_int)) {
            self->firstbit = args[ARG_firstbit].u_int;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("bad firstbit"));
        }
    }
    if (args[ARG_sck].u_obj != MP_OBJ_NULL) {
        const machine_pin_obj_t *arg_sck = machine_pin_find(args[ARG_sck].u_obj);
        sck = arg_sck->pin;
        if (!IS_VALID_SCK(self->sck->pin, sck)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SCK pin"));
        }
    }
    if (args[ARG_mosi].u_obj != MP_OBJ_NULL) {
        const machine_pin_obj_t *arg_mosi = machine_pin_find(args[ARG_mosi].u_obj);
        mosi = arg_mosi->pin;
        if (!IS_VALID_MOSI(self->mosi->pin, mosi)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad MOSI pin"));
        }
    }
    if (args[ARG_miso].u_obj != MP_OBJ_NULL) {
        const machine_pin_obj_t *arg_miso = machine_pin_find(args[ARG_miso].u_obj);
        miso = arg_miso->pin;
        if (!IS_VALID_MISO(self->miso->pin, miso)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad MISO pin"));
        }
    }

    if (self->firstbit == MICROPY_PY_MACHINE_SPI_LSB) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("LSB"));
    }

    // init the SPI bus
    spi_init(self);
}

static void machine_hard_spi_deinit(mp_obj_base_t *self_in) {
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)self_in;
    spi_deinit(self->spi_id);
}

static void machine_hard_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)self_in;
    spi_transfer(self->spi_id, self->bits, len, src, dest, SPI_TRANSFER_TIMEOUT(len));
}

static const mp_machine_spi_p_t machine_hard_spi_p = {
    .init = machine_hard_spi_init,
    .deinit = machine_hard_spi_deinit,
    .transfer = machine_hard_spi_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_hard_spi_make_new,
    locals_dict, &mp_machine_spi_locals_dict,
    print, machine_hard_spi_print,
    protocol, &machine_hard_spi_p
    );

void spi_init0(void) {
}

void spi_deinit(uint32_t ch) {
    if (0) {
    #if defined(MICROPY_HW_SPI0_RSPCK)
    } else if (ch == 0) {
        ra_spi_deinit(ch, 0);
    #endif
    #if defined(MICROPY_HW_SPI1_RSPCK)
    } else if (ch == 1) {
        ra_spi_deinit(ch, 0);
    #endif
    }
}

void spi_transfer(uint32_t ch, uint32_t bits, size_t len, const uint8_t *src, uint8_t *dest, uint32_t timeout) {
    ra_spi_transfer(ch, bits, dest, (uint8_t *)src, (uint32_t)len, timeout);
}
