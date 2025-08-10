/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2016 - 2018 Glenn Ruben Bakke
 * Copyright (c) 2018 Ayke van Laethem
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

#if MICROPY_PY_MACHINE_SPI

#include "py/nlr.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"
#if NRFX_SPI_ENABLED
#include "nrfx_spi.h"
#else
#include "nrfx_spim.h"
#endif

/// \moduleref machine
/// \class SPI - a controller-driven serial protocol
///
/// SPI is a serial protocol that is driven by a controller.  At the physical level
/// there are 3 lines: SCK, MOSI, MISO.
///
/// See usage model of I2C; SPI is very similar.  Main difference is
/// parameters to init the SPI bus:
///
///     from machine import SPI
///     spi = SPI(1, baudrate=600000, polarity=1, phase=0, crc=0x7)
///
/// Polarity can be
/// 0 or 1, and is the level the idle clock line sits at.  Phase can be 0 or 1
/// to sample data on the first or second clock edge respectively.  Crc can be
/// None for no CRC, or a polynomial specifier.
///
/// Additional method for SPI:
///
///     data = spi.send_recv(b'1234')        # send 4 bytes and receive 4 bytes
///     buf = bytearray(4)
///     spi.send_recv(b'1234', buf)          # send 4 bytes and receive 4 into buf
///     spi.send_recv(buf, buf)              # send/recv 4 bytes from/to buf

#if NRFX_SPIM_ENABLED

#define nrfx_spi_t                  nrfx_spim_t
#define nrfx_spi_config_t           nrfx_spim_config_t
#define nrfx_spi_xfer_desc_t        nrfx_spim_xfer_desc_t

#define NRFX_SPI_PIN_NOT_USED       NRFX_SPIM_PIN_NOT_USED
#define NRFX_SPI_INSTANCE           NRFX_SPIM_INSTANCE
#define NRF_SPI_BIT_ORDER_LSB_FIRST NRF_SPIM_BIT_ORDER_LSB_FIRST
#define NRF_SPI_BIT_ORDER_MSB_FIRST NRF_SPIM_BIT_ORDER_MSB_FIRST
#define NRF_SPI_MODE_0              NRF_SPIM_MODE_0
#define NRF_SPI_MODE_1              NRF_SPIM_MODE_1
#define NRF_SPI_MODE_2              NRF_SPIM_MODE_2
#define NRF_SPI_MODE_3              NRF_SPIM_MODE_3
#define NRF_SPI_FREQ_125K           NRF_SPIM_FREQ_125K
#define NRF_SPI_FREQ_250K           NRF_SPIM_FREQ_250K
#define NRF_SPI_FREQ_500K           NRF_SPIM_FREQ_500K
#define NRF_SPI_FREQ_1M             NRF_SPIM_FREQ_1M
#define NRF_SPI_FREQ_2M             NRF_SPIM_FREQ_2M
#define NRF_SPI_FREQ_4M             NRF_SPIM_FREQ_4M
#define NRF_SPI_FREQ_8M             NRF_SPIM_FREQ_8M

#define nrfx_spi_init               nrfx_spim_init
#define nrfx_spi_uninit             nrfx_spim_uninit
#define nrfx_spi_xfer               nrfx_spim_xfer

#endif // NRFX_SPIM_ENABLED

typedef struct _machine_hard_spi_obj_t {
    mp_obj_base_t base;
    const nrfx_spi_t *p_spi;       // Driver instance
    nrfx_spi_config_t *p_config;   // pointer to volatile part
} machine_hard_spi_obj_t;

static const nrfx_spi_t machine_spi_instances[] = {
    NRFX_SPI_INSTANCE(0),
    NRFX_SPI_INSTANCE(1),
    #if defined(NRF52_SERIES)
    NRFX_SPI_INSTANCE(2),
    #if defined(NRF52840_XXAA) && NRFX_SPIM_ENABLED
    NRFX_SPI_INSTANCE(3),
    #endif // NRF52840_XXAA && NRFX_SPIM_ENABLED
    #endif // NRF52_SERIES
};

static nrfx_spi_config_t configs[MP_ARRAY_SIZE(machine_spi_instances)];

static const machine_hard_spi_obj_t machine_hard_spi_obj[] = {
    {{&machine_spi_type}, .p_spi = &machine_spi_instances[0], .p_config = &configs[0]},
    {{&machine_spi_type}, .p_spi = &machine_spi_instances[1], .p_config = &configs[1]},
    #if defined(NRF52_SERIES)
    {{&machine_spi_type}, .p_spi = &machine_spi_instances[2], .p_config = &configs[2]},
    #if defined(NRF52840_XXAA) && NRFX_SPIM_ENABLED
    {{&machine_spi_type}, .p_spi = &machine_spi_instances[3], .p_config = &configs[3]},
    #endif // NRF52840_XXAA && NRFX_SPIM_ENABLED
    #endif // NRF52_SERIES
};

void spi_init0(void) {
}

static int spi_find(mp_obj_t id) {
    if (mp_obj_is_str(id)) {
        // given a string id
        const char *port = mp_obj_str_get_str(id);
        if (0) {
        #ifdef MICROPY_HW_SPI0_NAME
        } else if (strcmp(port, MICROPY_HW_SPI0_NAME) == 0) {
            return 1;
        #endif
        }
        mp_raise_ValueError(MP_ERROR_TEXT("SPI doesn't exist"));
    } else {
        // given an integer id
        int spi_id = mp_obj_get_int(id);
        if (spi_id >= 0 && spi_id < MP_ARRAY_SIZE(machine_hard_spi_obj)) {
            return spi_id;
        }
        mp_raise_ValueError(MP_ERROR_TEXT("SPI doesn't exist"));
    }
}

void spi_transfer(const machine_hard_spi_obj_t *self, size_t len, const void *src, void *dest) {
    nrfx_spi_xfer_desc_t xfer_desc = {
        .p_tx_buffer = src,
        .tx_length = len,
        .p_rx_buffer = dest,
        .rx_length = len
    };

    nrfx_spi_xfer(self->p_spi, &xfer_desc, 0);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

// for make_new
enum {
    ARG_NEW_id,
    ARG_NEW_baudrate,
    ARG_NEW_polarity,
    ARG_NEW_phase,
    ARG_NEW_bits,
    ARG_NEW_firstbit,
    ARG_NEW_sck,
    ARG_NEW_mosi,
    ARG_NEW_miso
};

// for init
enum {
    ARG_INIT_baudrate,
    ARG_INIT_polarity,
    ARG_INIT_phase,
    ARG_INIT_bits,
    ARG_INIT_firstbit
};

static inline uint32_t machine_hard_spi_get_baudrate(const machine_hard_spi_obj_t *self) {
    MP_STATIC_ASSERT(NRF_SPI_FREQ_125K == (2 << 24));
    MP_STATIC_ASSERT(NRF_SPI_FREQ_250K == (4 << 24));
    MP_STATIC_ASSERT(NRF_SPI_FREQ_500K == (8 << 24));
    MP_STATIC_ASSERT(NRF_SPI_FREQ_1M == (16 << 24));
    MP_STATIC_ASSERT(NRF_SPI_FREQ_2M == (32 << 24));
    MP_STATIC_ASSERT(NRF_SPI_FREQ_4M == (64 << 24));
    MP_STATIC_ASSERT(NRF_SPI_FREQ_8M == (128 << 24));
    #if defined(NRF52840_XXAA) && NRFX_SPIM_ENABLED
    if (self->p_config->frequency == NRF_SPIM_FREQ_16M) {
        return 16000000;
    }
    if (self->p_config->frequency == NRF_SPIM_FREQ_32M) {
        return 32000000;
    }
    #endif
    return 125000 * (self->p_config->frequency >> 25);
}

static void machine_hard_spi_init_helper(const machine_hard_spi_obj_t *self, mp_arg_val_t *args);

static void machine_hard_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_spi_obj_t *self = self_in;
    unsigned int polarity = self->p_config->mode == NRF_SPI_MODE_0 || self->p_config->mode == NRF_SPI_MODE_1 ? 0 : 1;
    unsigned int phase = self->p_config->mode == NRF_SPI_MODE_0 || self->p_config->mode == NRF_SPI_MODE_2 ? 0 : 1;
    mp_printf(print, "SPI(%u, baudrate=%u, polarity=%u, phase=%u)",
        self->p_spi->drv_inst_idx, machine_hard_spi_get_baudrate(self), polarity, phase);
}

static mp_obj_t machine_hard_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 1000000} },
        { MP_QSTR_polarity, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_INT, {.u_int = 0 /* SPI_FIRSTBIT_MSB */} },
        { MP_QSTR_sck,      MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get static peripheral object
    int spi_id = spi_find(args[ARG_NEW_id].u_obj);
    const machine_hard_spi_obj_t *self = &machine_hard_spi_obj[spi_id];

    // here we would check the sck/mosi/miso pins and configure them
    if (args[ARG_NEW_sck].u_obj != MP_OBJ_NULL
        && args[ARG_NEW_mosi].u_obj != MP_OBJ_NULL
        && args[ARG_NEW_miso].u_obj != MP_OBJ_NULL) {

        self->p_config->sck_pin = mp_hal_get_pin_obj(args[ARG_NEW_sck].u_obj)->pin;
        self->p_config->mosi_pin = mp_hal_get_pin_obj(args[ARG_NEW_mosi].u_obj)->pin;
        self->p_config->miso_pin = mp_hal_get_pin_obj(args[ARG_NEW_miso].u_obj)->pin;
    } else {
        self->p_config->sck_pin = MICROPY_HW_SPI0_SCK;
        self->p_config->mosi_pin = MICROPY_HW_SPI0_MOSI;
        self->p_config->miso_pin = MICROPY_HW_SPI0_MISO;
    }

    // Manually trigger slave select from upper layer.
    self->p_config->ss_pin = NRFX_SPI_PIN_NOT_USED;

    #ifdef NRF51
    self->p_config->irq_priority = 3;
    #else
    self->p_config->irq_priority = 6;
    #endif

    machine_hard_spi_init_helper(self, &args[1]); // Skip instance id param.

    return MP_OBJ_FROM_PTR(self);
}

static void machine_hard_spi_init_helper(const machine_hard_spi_obj_t *self, mp_arg_val_t *args) {
    int baudrate = args[ARG_INIT_baudrate].u_int;

    if (baudrate <= 125000) {
        self->p_config->frequency = NRF_SPI_FREQ_125K;
    } else if (baudrate <= 250000) {
        self->p_config->frequency = NRF_SPI_FREQ_250K;
    } else if (baudrate <= 500000) {
        self->p_config->frequency = NRF_SPI_FREQ_500K;
    } else if (baudrate <= 1000000) {
        self->p_config->frequency = NRF_SPI_FREQ_1M;
    } else if (baudrate <= 2000000) {
        self->p_config->frequency = NRF_SPI_FREQ_2M;
    } else if (baudrate <= 4000000) {
        self->p_config->frequency = NRF_SPI_FREQ_4M;
    } else if (baudrate <= 8000000) {
        self->p_config->frequency = NRF_SPI_FREQ_8M;
    #if defined(NRF52840_XXAA) && NRFX_SPIM_ENABLED
    } else if (baudrate <= 16000000) {
        self->p_config->frequency = NRF_SPIM_FREQ_16M;
    } else if (baudrate <= 32000000) {
        self->p_config->frequency = NRF_SPIM_FREQ_32M;
    #endif // NRF52840_XXAA && NRFX_SPIM_ENABLED
    } else { // Default
        self->p_config->frequency = NRF_SPI_FREQ_1M;
    }

    if (args[ARG_INIT_polarity].u_int == 0) {
        // Active high
        if (args[ARG_INIT_phase].u_int == 0) {
            // First clock edge
            self->p_config->mode = NRF_SPI_MODE_0;
        } else {
            // Second clock edge
            self->p_config->mode = NRF_SPI_MODE_1;
        }
    } else {
        // Active low
        if (args[ARG_INIT_phase].u_int == 0) {
            // First clock edge
            self->p_config->mode = NRF_SPI_MODE_2;
        } else {
            // Second clock edge
            self->p_config->mode = NRF_SPI_MODE_3;
        }
    }

    self->p_config->orc = 0xFF;  // Overrun character
    self->p_config->bit_order = (args[ARG_INIT_firstbit].u_int == 0) ? NRF_SPI_BIT_ORDER_MSB_FIRST : NRF_SPI_BIT_ORDER_LSB_FIRST;

    // Set context to this instance of SPI
    nrfx_err_t err_code = nrfx_spi_init(self->p_spi, self->p_config, NULL, (void *)self);

    if (err_code == NRFX_ERROR_INVALID_STATE) {
        // Instance already initialized, deinitialize first.
        nrfx_spi_uninit(self->p_spi);
        // Initialize again.
        nrfx_spi_init(self->p_spi, self->p_config, NULL, (void *)self);
    }
}

static void machine_hard_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // parse args
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)self_in;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_hard_spi_init_helper(self, args);
}

static void machine_hard_spi_deinit(mp_obj_base_t *self_in) {
    const machine_hard_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    nrfx_spi_uninit(self->p_spi);
}

static void machine_hard_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    const machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)self_in;
    spi_transfer(self, len, src, dest);
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
    print, machine_hard_spi_print,
    protocol, &machine_hard_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );

#endif // MICROPY_PY_MACHINE_SPI
