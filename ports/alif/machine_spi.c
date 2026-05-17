/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 OpenMV LLC.
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

#if MICROPY_PY_MACHINE_SPI
#include "clk.h"
#include "spi.h"
#include "sys_ctrl_spi.h"

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    SPI_Type *inst;
    bool is_lp;
    uint32_t bits;
} machine_spi_obj_t;

static machine_spi_obj_t machine_spi_obj[] = {
    #if defined(MICROPY_HW_SPI0_SCK)
    [0] = {{&machine_spi_type}, 0, (SPI_Type *)SPI0_BASE, false},
    #endif
    #if defined(MICROPY_HW_SPI1_SCK)
    [1] = {{&machine_spi_type}, 1, (SPI_Type *)SPI1_BASE, false},
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    [2] = {{&machine_spi_type}, 2, (SPI_Type *)SPI2_BASE, false},
    #endif
    #if defined(MICROPY_HW_SPI3_SCK)
    [3] = {{&machine_spi_type}, 3, (SPI_Type *)SPI3_BASE, false},
    #endif
    #if defined(MICROPY_HW_LPSPI0_SCK)
    [4] = {{&machine_spi_type}, 4, (SPI_Type *)LPSPI0_BASE, true},
    #endif

};

static const uint8_t spi_pin_alt[] = {
    MP_HAL_PIN_ALT_SPI_SCLK,
    MP_HAL_PIN_ALT_SPI_MISO,
    MP_HAL_PIN_ALT_SPI_MOSI,
    MP_HAL_PIN_ALT_SPI_SS0,
};

static const uint8_t lpspi_pin_alt[] = {
    MP_HAL_PIN_ALT_LPSPI_SCLK,
    MP_HAL_PIN_ALT_LPSPI_MISO,
    MP_HAL_PIN_ALT_LPSPI_MOSI,
    MP_HAL_PIN_ALT_LPSPI_SS,
};

static inline uint32_t spi_get_clk(machine_spi_obj_t *spi) {
    return spi->is_lp ? GetSystemCoreClock() : GetSystemAHBClock();
}

static void spi_init(machine_spi_obj_t *spi, int32_t baudrate,
    int32_t polarity, int32_t phase, int32_t bits, int32_t firstbit) {
    const machine_pin_obj_t *pins[4] = { NULL, NULL, NULL, NULL };
    switch (spi->id) {
        #if defined(MICROPY_HW_SPI0_SCK)
        case 0:
            pins[0] = MICROPY_HW_SPI0_SCK;
            pins[1] = MICROPY_HW_SPI0_MISO;
            pins[2] = MICROPY_HW_SPI0_MOSI;
            #if defined(MICROPY_HW_SPI0_NSS)
            pins[3] = MICROPY_HW_SPI0_NSS;
            #endif
            break;
        #endif
        #if defined(MICROPY_HW_SPI1_SCK)
        case 1:
            pins[0] = MICROPY_HW_SPI1_SCK;
            pins[1] = MICROPY_HW_SPI1_MISO;
            pins[2] = MICROPY_HW_SPI1_MOSI;
            #if defined(MICROPY_HW_SPI1_NSS)
            pins[3] = MICROPY_HW_SPI1_NSS;
            #endif
            break;
        #endif
        #if defined(MICROPY_HW_SPI2_SCK)
        case 2:
            pins[0] = MICROPY_HW_SPI2_SCK;
            pins[1] = MICROPY_HW_SPI2_MISO;
            pins[2] = MICROPY_HW_SPI2_MOSI;
            #if defined(MICROPY_HW_SPI2_NSS)
            pins[3] = MICROPY_HW_SPI2_NSS;
            #endif
            break;
        #endif
        #if defined(MICROPY_HW_SPI3_SCK)
        case 3:
            pins[0] = MICROPY_HW_SPI3_SCK;
            pins[1] = MICROPY_HW_SPI3_MISO;
            pins[2] = MICROPY_HW_SPI3_MOSI;
            #if defined(MICROPY_HW_SPI3_NSS)
            pins[3] = MICROPY_HW_SPI3_NSS;
            #endif
            break;
        #endif
        #if defined(MICROPY_HW_LPSPI0_SCK)
        case 4:  // LPSPI0
            pins[0] = MICROPY_HW_LPSPI0_SCK;
            pins[1] = MICROPY_HW_LPSPI0_MISO;
            pins[2] = MICROPY_HW_LPSPI0_MOSI;
            #if defined(MICROPY_HW_LPSPI0_NSS)
            pins[3] = MICROPY_HW_LPSPI0_NSS;
            #endif
            break;
        #endif
        default:
            return;
    }

    // Disable SPI.
    spi_disable(spi->inst);

    // Enable clocks.
    if (spi->is_lp) {
        enable_lpspi_clk();
    }

    // Configure SPI pins.
    const uint8_t *alt;
    if (spi->id <= 3) {
        alt = spi_pin_alt;
    } else {
        alt = lpspi_pin_alt;
    }
    for (size_t i = 0; i < MP_ARRAY_SIZE(pins) && pins[i]; i++) {
        mp_hal_pin_config(pins[i], MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
            MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_8MA, MP_HAL_PIN_ALT_MAKE(alt[i], spi->id), true);
    }

    // Disable all interrupts.
    spi_mask_interrupts(spi->inst);

    // Configure baudrate clock
    if (baudrate > 0) {
        spi_set_bus_speed(spi->inst, baudrate, spi_get_clk(spi));
    }

    // Configure FIFOs
    spi_set_tx_threshold(spi->inst, 0);
    spi_set_rx_threshold(spi->inst, 0);
    if (!spi->is_lp) {
        spi_set_rx_sample_delay(spi->inst, 0);
        spi_set_tx_fifo_start_level(spi->inst, 0);
    }

    // Configure SPI bus mode.
    if (!spi->is_lp) {
        if (polarity < 0) {
            polarity = (spi->inst->SPI_CTRLR0 & SPI_CTRLR0_SCPOL_HIGH) ? 1 : 0;
        }
        if (phase < 0) {
            phase = (spi->inst->SPI_CTRLR0 & SPI_CTRLR0_SCPH_HIGH) ? 1 : 0;
        }
    } else {
        if (polarity < 0) {
            polarity = (spi->inst->SPI_CTRLR0 & LPSPI_CTRLR0_SCPOL_HIGH) ? 1 : 0;
        }
        if (phase < 0) {
            phase = (spi->inst->SPI_CTRLR0 & LPSPI_CTRLR0_SCPH_HIGH) ? 1 : 0;
        }
    }
    uint32_t spi_mode = (polarity << 1) | phase;
    if (!spi->is_lp) {
        spi_set_mode(spi->inst, spi_mode);
    } else {
        lpspi_set_mode(spi->inst, spi_mode);
    }

    // Configure SPI bus protocol.
    uint32_t spi_proto = SPI_PROTO_SPI;
    if (!spi->is_lp) {
        spi_set_protocol(spi->inst, spi_proto);
    } else {
        lpspi_set_protocol(spi->inst, spi_proto);
    }

    // Configure SPI transfer mode.
    if (!spi->is_lp) {
        spi_mode_master(spi->inst);
    }

    // Configure frame size.
    if (bits > 0) {
        if (!spi->is_lp) {
            spi_set_dfs(spi->inst, bits);
        } else {
            lpspi_set_dfs(spi->inst, bits);
        }
    }

    // Configure slave select pin
    spi_control_ss(spi->inst, 0, true);
    if (!spi->is_lp) {
        spi_set_sste(spi->inst, false);
    } else {
        lpspi_set_sste(spi->inst, false);
    }

    // Clear IRQs.
    (void)spi->inst->SPI_ICR;

    // Enable SPI.
    spi_enable(spi->inst);
}

static void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t baudrate = spi_get_bus_speed(self->inst, spi_get_clk(self));
    mp_printf(print, "SPI(%u, baudrate=%u, lp=%u)", self->id, baudrate, self->is_lp);
}

mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 500000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get spi bus.
    int spi_id = mp_obj_get_int(args[ARG_id].u_obj);
    if (spi_id < 0 || spi_id >= MP_ARRAY_SIZE(machine_spi_obj) || !machine_spi_obj[spi_id].inst) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesn't exist"), spi_id);
    }

    // Get static peripheral object.
    machine_spi_obj_t *self = &machine_spi_obj[spi_id];

    // Set args
    self->bits = args[ARG_bits].u_int;

    // here we would check the sck/mosi/miso pins and configure them, but it's not implemented
    if (args[ARG_sck].u_obj != MP_OBJ_NULL ||
        args[ARG_mosi].u_obj != MP_OBJ_NULL ||
        args[ARG_miso].u_obj != MP_OBJ_NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("explicit choice of sck/mosi/miso is not implemented"));
    }

    // Initialize and configure SPI.
    spi_init(self, args[ARG_baudrate].u_int, args[ARG_polarity].u_int,
        args[ARG_phase].u_int, args[ARG_bits].u_int, args[ARG_firstbit].u_int);

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

    // Initialize and configure SPI.
    spi_init(self, args[ARG_baudrate].u_int, args[ARG_polarity].u_int,
        args[ARG_phase].u_int, args[ARG_bits].u_int, args[ARG_firstbit].u_int);
}

static void machine_spi_deinit(mp_obj_base_t *self_in) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    // Disable all interrupts.
    spi_mask_interrupts(self->inst);
    // Disable SS pin.
    spi_control_ss(self->inst, 0, 0);
    // Disable SPI.
    spi_disable(self->inst);
    // Deinitialize GPIOs and clocks.
    if (self->is_lp) {
        disable_lpspi_clk();
    }
}

static void machine_spi_poll_flag(SPI_Type *spi, uint32_t flag, uint32_t timeout) {
    mp_uint_t tick_start = mp_hal_ticks_ms();
    while (!(spi->SPI_SR & flag)) {
        if (mp_hal_ticks_ms() - tick_start >= timeout) {
            mp_raise_OSError(MP_ETIMEDOUT);
        }
        mp_event_handle_nowait();
    }
}

static void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;
    volatile uint32_t *dr = self->inst->SPI_DR;

    spi_set_tmod(self->inst, SPI_TMOD_TX_AND_RX);

    for (size_t i = 0; i < len; i++) {
        // Wait for space in the TX FIFO
        machine_spi_poll_flag(self->inst, SPI_SR_TFNF, 100);

        // Send data
        if (src == NULL) {
            *dr = 0xFFFFFFFFU;
        } else if (self->bits > 16) {
            *dr = ((uint32_t *)src)[i];
        } else if (self->bits > 8) {
            *dr = ((uint16_t *)src)[i];
        } else {
            *dr = ((uint8_t *)src)[i];
        }

        // Wait for data in the RX FIFO
        machine_spi_poll_flag(self->inst, SPI_SR_RFNE, 100);

        // Recv data
        if (dest == NULL) {
            (void)*dr;
        } else if (self->bits > 16) {
            ((uint32_t *)dest)[i] = *dr;
        } else if (self->bits > 8) {
            ((uint16_t *)dest)[i] = *dr;
        } else {
            ((uint8_t *)dest)[i] = *dr;
        }
    }
}

static const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .deinit = machine_spi_deinit,
    .transfer = machine_spi_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    print, machine_spi_print,
    protocol, &machine_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );

#endif // MICROPY_PY_MACHINE_SPI
