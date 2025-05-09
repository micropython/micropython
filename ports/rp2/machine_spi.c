/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"

#include "hardware/spi.h"
#include "hardware/dma.h"

#define DEFAULT_SPI_BAUDRATE    (1000000)
#define DEFAULT_SPI_POLARITY    (0)
#define DEFAULT_SPI_PHASE       (0)
#define DEFAULT_SPI_BITS        (8)
#define DEFAULT_SPI_FIRSTBIT    (SPI_MSB_FIRST)

#ifdef MICROPY_HW_SPI_NO_DEFAULT_PINS

// With no default SPI, need to require the pin args.
#define MICROPY_HW_SPI0_SCK     (0)
#define MICROPY_HW_SPI0_MOSI    (0)
#define MICROPY_HW_SPI0_MISO    (0)
#define MICROPY_HW_SPI1_SCK     (0)
#define MICROPY_HW_SPI1_MOSI    (0)
#define MICROPY_HW_SPI1_MISO    (0)
#define MICROPY_SPI_PINS_ARG_OPTS MP_ARG_REQUIRED

#else

// Most boards do not require pin args.
#define MICROPY_SPI_PINS_ARG_OPTS 0

#ifndef MICROPY_HW_SPI0_SCK
#if PICO_DEFAULT_SPI == 0
#define MICROPY_HW_SPI0_SCK     (PICO_DEFAULT_SPI_SCK_PIN)
#define MICROPY_HW_SPI0_MOSI    (PICO_DEFAULT_SPI_TX_PIN)
#define MICROPY_HW_SPI0_MISO    (PICO_DEFAULT_SPI_RX_PIN)
#else
#define MICROPY_HW_SPI0_SCK     (6)
#define MICROPY_HW_SPI0_MOSI    (7)
#define MICROPY_HW_SPI0_MISO    (4)
#endif
#endif

#ifndef MICROPY_HW_SPI1_SCK
#if PICO_DEFAULT_SPI == 1
#define MICROPY_HW_SPI1_SCK     (PICO_DEFAULT_SPI_SCK_PIN)
#define MICROPY_HW_SPI1_MOSI    (PICO_DEFAULT_SPI_TX_PIN)
#define MICROPY_HW_SPI1_MISO    (PICO_DEFAULT_SPI_RX_PIN)
#else
#define MICROPY_HW_SPI1_SCK     (10)
#define MICROPY_HW_SPI1_MOSI    (11)
#define MICROPY_HW_SPI1_MISO    (8)
#endif
#endif

#endif

// Assume we won't get an RP2-compatible with 255 GPIO pins
#define MICROPY_HW_SPI_PIN_UNUSED UINT8_MAX

// SPI0 can be GP{0..7,16..23}, SPI1 can be GP{8..15,24..29}.
#define IS_VALID_PERIPH(spi, pin)   ((((pin) & 8) >> 3) == (spi))
// GP{2,6,10,14,...}
#define IS_VALID_SCK(spi, pin)      (((pin) & 3) == 2 && IS_VALID_PERIPH(spi, pin))
// GP{3,7,11,15,...}
#define IS_VALID_MOSI(spi, pin)     (((pin) & 3) == 3 && IS_VALID_PERIPH(spi, pin))
// GP{0,4,8,10,...}
#define IS_VALID_MISO(spi, pin)     (((pin) & 3) == 0 && IS_VALID_PERIPH(spi, pin))

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    spi_inst_t *const spi_inst;
    uint8_t spi_id;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
    uint8_t sck;
    uint8_t mosi;
    uint8_t miso;
    uint32_t baudrate;
} machine_spi_obj_t;

static machine_spi_obj_t machine_spi_obj[] = {
    {
        {&machine_spi_type}, spi0, 0,
        DEFAULT_SPI_POLARITY, DEFAULT_SPI_PHASE, DEFAULT_SPI_BITS, DEFAULT_SPI_FIRSTBIT,
        MICROPY_HW_SPI0_SCK, MICROPY_HW_SPI0_MOSI, MICROPY_HW_SPI0_MISO,
        0,
    },
    {
        {&machine_spi_type}, spi1, 1,
        DEFAULT_SPI_POLARITY, DEFAULT_SPI_PHASE, DEFAULT_SPI_BITS, DEFAULT_SPI_FIRSTBIT,
        MICROPY_HW_SPI1_SCK, MICROPY_HW_SPI1_MOSI, MICROPY_HW_SPI1_MISO,
        0,
    },
};

static void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, sck=%u, mosi=%u, miso=",
        self->spi_id, self->baudrate, self->polarity, self->phase, self->bits,
        self->sck, self->mosi);
    if (self->miso == MICROPY_HW_SPI_PIN_UNUSED) {
        mp_printf(print, "None)");
    } else {
        mp_printf(print, "%u)", self->miso);
    }
}

mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        #ifdef PICO_DEFAULT_SPI
        { MP_QSTR_id,       MP_ARG_INT, {.u_int = PICO_DEFAULT_SPI} },
        #else
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT, },
        #endif
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = DEFAULT_SPI_BAUDRATE} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_POLARITY} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_PHASE} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_BITS} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_FIRSTBIT} },
        { MP_QSTR_sck,      MICROPY_SPI_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mosi,     MICROPY_SPI_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_miso,     MICROPY_SPI_PINS_ARG_OPTS | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get the SPI bus id.
    int spi_id = args[ARG_id].u_int;
    if (spi_id < 0 || spi_id >= MP_ARRAY_SIZE(machine_spi_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesn't exist"), spi_id);
    }

    // Get static peripheral object.
    machine_spi_obj_t *self = (machine_spi_obj_t *)&machine_spi_obj[spi_id];

    // Set SCK/MOSI/MISO pins if configured.
    if (args[ARG_sck].u_obj != mp_const_none) {
        int sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
        if (!IS_VALID_SCK(self->spi_id, sck)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SCK pin"));
        }
        self->sck = sck;
    }
    if (args[ARG_mosi].u_obj != mp_const_none) {
        int mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);
        if (!IS_VALID_MOSI(self->spi_id, mosi)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad MOSI pin"));
        }
        self->mosi = mosi;
    }

    if (args[ARG_miso].u_obj == mp_const_none) {
        self->miso = MICROPY_HW_SPI_PIN_UNUSED;
    } else if (args[ARG_miso].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        int miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);
        if (!IS_VALID_MISO(self->spi_id, miso)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad MISO pin"));
        }
        self->miso = miso;
    }

    // Initialise the SPI peripheral if any arguments given, or it was not initialised previously.
    if (n_args > 1 || n_kw > 0 || self->baudrate == 0) {
        self->baudrate = args[ARG_baudrate].u_int;
        self->polarity = args[ARG_polarity].u_int;
        self->phase = args[ARG_phase].u_int;
        self->bits = args[ARG_bits].u_int;
        self->firstbit = args[ARG_firstbit].u_int;
        if (self->firstbit == SPI_LSB_FIRST) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("LSB"));
        }

        spi_init(self->spi_inst, self->baudrate);
        self->baudrate = spi_set_baudrate(self->spi_inst, self->baudrate);
        spi_set_format(self->spi_inst, self->bits, self->polarity, self->phase, self->firstbit);
        gpio_set_function(self->sck, GPIO_FUNC_SPI);
        if (self->miso != MICROPY_HW_SPI_PIN_UNUSED) {
            gpio_set_function(self->miso, GPIO_FUNC_SPI);
        }
        gpio_set_function(self->mosi, GPIO_FUNC_SPI);
    }

    return MP_OBJ_FROM_PTR(self);
}

static void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse the arguments.
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Reconfigure the baudrate if requested.
    if (args[ARG_baudrate].u_int != -1) {
        self->baudrate = spi_set_baudrate(self->spi_inst, args[ARG_baudrate].u_int);
    }

    // Reconfigure the format if requested.
    bool set_format = false;
    if (args[ARG_polarity].u_int != -1) {
        self->polarity = args[ARG_polarity].u_int;
        set_format = true;
    }
    if (args[ARG_phase].u_int != -1) {
        self->phase = args[ARG_phase].u_int;
        set_format = true;
    }
    if (args[ARG_bits].u_int != -1) {
        self->bits = args[ARG_bits].u_int;
        set_format = true;
    }
    if (args[ARG_firstbit].u_int != -1) {
        self->firstbit = args[ARG_firstbit].u_int;
        if (self->firstbit == SPI_LSB_FIRST) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("LSB"));
        }
    }
    if (set_format) {
        spi_set_format(self->spi_inst, self->bits, self->polarity, self->phase, self->firstbit);
    }
}

static void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    // Use DMA for large transfers if channels are available
    const size_t dma_min_size_threshold = 32;
    int chan_tx = -1;
    int chan_rx = -1;
    if (len >= dma_min_size_threshold) {
        // Use two DMA channels to service the two FIFOs
        chan_tx = dma_claim_unused_channel(false);
        chan_rx = dma_claim_unused_channel(false);
    }
    bool use_dma = chan_rx >= 0 && chan_tx >= 0;
    // note src is guaranteed to be non-NULL
    bool write_only = dest == NULL;

    if (use_dma) {
        uint8_t dev_null;
        dma_channel_config c = dma_channel_get_default_config(chan_tx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->spi_inst) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
        dma_channel_configure(chan_tx, &c,
            &spi_get_hw(self->spi_inst)->dr,
            src,
            len,
            false);

        c = dma_channel_get_default_config(chan_rx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_index(self->spi_inst) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, !write_only);
        dma_channel_configure(chan_rx, &c,
            write_only ? &dev_null : dest,
            &spi_get_hw(self->spi_inst)->dr,
            len,
            false);

        dma_start_channel_mask((1u << chan_rx) | (1u << chan_tx));
        dma_channel_wait_for_finish_blocking(chan_rx);
        dma_channel_wait_for_finish_blocking(chan_tx);
    }

    // If we have claimed only one channel successfully, we should release immediately
    if (chan_rx >= 0) {
        dma_channel_unclaim(chan_rx);
    }
    if (chan_tx >= 0) {
        dma_channel_unclaim(chan_tx);
    }

    if (!use_dma) {
        // Use software for small transfers, or if couldn't claim two DMA channels
        if (write_only) {
            spi_write_blocking(self->spi_inst, src, len);
        } else {
            spi_write_read_blocking(self->spi_inst, src, dest, len);
        }
    }
}

// Buffer protocol implementation for SPI.
// The buffer represents the SPI data FIFO.
static mp_int_t machine_spi_get_buffer(mp_obj_t o_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(o_in);

    bufinfo->len = 4;
    bufinfo->typecode = 'I';
    bufinfo->buf = (void *)&spi_get_hw(self->spi_inst)->dr;

    return 0;
}

static const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .transfer = machine_spi_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    print, machine_spi_print,
    protocol, &machine_spi_p,
    buffer, machine_spi_get_buffer,
    locals_dict, &mp_machine_spi_locals_dict
    );

mp_obj_base_t *mp_hal_get_spi_obj(mp_obj_t o) {
    if (mp_obj_is_type(o, &machine_spi_type)) {
        return MP_OBJ_TO_PTR(o);
    }
    #if MICROPY_PY_MACHINE_SOFTSPI
    else if (mp_obj_is_type(o, &mp_machine_soft_spi_type)) {
        return MP_OBJ_TO_PTR(o);
    }
    #endif
    else {
        mp_raise_TypeError(MP_ERROR_TEXT("expecting an SPI object"));
    }
}
