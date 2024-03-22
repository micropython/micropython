/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022 Robert Hammelrath
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

#if MICROPY_PY_MACHINE_SPI

#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "samd_soc.h"
#include "pin_af.h"
#include "clock_config.h"

#define DEFAULT_SPI_BAUDRATE    (1000000)
#define DEFAULT_SPI_POLARITY    (0)
#define DEFAULT_SPI_PHASE       (0)
#define DEFAULT_SPI_BITS        (8)
#define DEFAULT_SPI_FIRSTBIT    (0)

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    uint8_t polarity;
    uint8_t phase;
    uint8_t firstbit;
    uint8_t sck;
    uint8_t mosi;
    uint8_t miso;
    uint8_t new;
    uint32_t baudrate;
    sercom_pad_config_t sck_pad_config;
    sercom_pad_config_t mosi_pad_config;
    sercom_pad_config_t miso_pad_config;
    uint8_t *dest;
    size_t rxlen;
} machine_spi_obj_t;

void common_spi_irq_handler(int spi_id) {
    // handle Sercom IRQ RXC
    machine_spi_obj_t *self = MP_STATE_PORT(sercom_table[spi_id]);
    // Handle IRQ
    if (self != NULL) {
        Sercom *spi = sercom_instance[self->id];
        if (spi->SPI.INTFLAG.bit.RXC != 0) {
            if (self->rxlen > 0) {
                *(self->dest)++ = spi->SPI.DATA.bit.DATA;
                self->rxlen--;
            } else {
                // Just in the unlikely case there is data but no space in the buffer
                // discard the data and clear the intflag
                uint32_t temp;
                (void)temp;
                temp = spi->SPI.DATA.bit.DATA;
            }
        }
    }
}

static void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(%u, baudrate=%u, firstbit=%u, polarity=%u, phase=%u, bits=8)",
        self->id, self->baudrate, self->firstbit, self->polarity, self->phase);
}

static void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_firstbit,
           ARG_sck, ARG_mosi, ARG_miso};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mosi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_miso, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Set baudrate if configured.
    if (args[ARG_baudrate].u_int >= 0) {
        self->baudrate = args[ARG_baudrate].u_int;
    }

    // Set polarity if configured.
    if (args[ARG_polarity].u_int >= 0) {
        self->polarity = args[ARG_polarity].u_int;
    }

    // Set phase if configured.
    if (args[ARG_phase].u_int >= 0) {
        self->phase = args[ARG_phase].u_int;
    }

    // Set firstbit if configured.
    if (args[ARG_firstbit].u_int >= 0) {
        self->firstbit = args[ARG_firstbit].u_int;
    }

    // Set SCK/MOSI/MISO pins if configured.
    if (args[ARG_sck].u_obj != mp_const_none) {
        self->sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    }
    if (args[ARG_mosi].u_obj != mp_const_none) {
        self->mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);
    }
    if (args[ARG_miso].u_obj != mp_const_none) {
        self->miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);
    }

    // Initialise the SPI peripheral if any arguments given, or it was not initialised previously.
    if (n_args > 0 || kw_args->used > 0 || self->new) {
        self->new = false;

        // Get the pad and alt-fct numbers.
        self->sck_pad_config = get_sercom_config(self->sck, self->id);
        self->mosi_pad_config = get_sercom_config(self->mosi, self->id);

        uint8_t dopo = 0;
        #if defined(MCU_SAMD21)
        if (self->mosi_pad_config.pad_nr == 0 && self->sck_pad_config.pad_nr == 1) {
            dopo = 0;
        } else if (self->mosi_pad_config.pad_nr == 2 && self->sck_pad_config.pad_nr == 3) {
            dopo = 1;
        } else if (self->mosi_pad_config.pad_nr == 3 && self->sck_pad_config.pad_nr == 1) {
            dopo = 2;
        } else if (self->mosi_pad_config.pad_nr == 0 && self->sck_pad_config.pad_nr == 3) {
            dopo = 3;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid pin for sck or mosi"));
        }
        #elif defined(MCU_SAMD51)
        if (self->mosi_pad_config.pad_nr == 0 && self->sck_pad_config.pad_nr == 1) {
            dopo = 0;
        } else if (self->mosi_pad_config.pad_nr == 3 && self->sck_pad_config.pad_nr == 1) {
            dopo = 2;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid pin for sck or mosi"));
        }
        #endif

        if (self->miso != 0xff) { // Miso may be undefined
            self->miso_pad_config = get_sercom_config(self->miso, self->id);
            mp_hal_set_pin_mux(self->miso, self->miso_pad_config.alt_fct);
        }
        // Configure the Pin mux.
        mp_hal_set_pin_mux(self->sck, self->sck_pad_config.alt_fct);
        mp_hal_set_pin_mux(self->mosi, self->mosi_pad_config.alt_fct);

        // Set up the clocks
        enable_sercom_clock(self->id);

        // Configure the SPI
        Sercom *spi = sercom_instance[self->id];
        // Reset (clear) the peripheral registers.
        while (spi->SPI.SYNCBUSY.bit.SWRST) {
        }
        spi->SPI.CTRLA.bit.SWRST = 1;
        while (spi->SPI.SYNCBUSY.bit.SWRST) {
        }

        // Set the registers
        spi->SPI.CTRLA.bit.MODE = 0x03; // SPI master mode
        spi->SPI.CTRLA.bit.CPOL = self->polarity;
        spi->SPI.CTRLA.bit.CPHA = self->phase;
        spi->SPI.CTRLA.bit.DIPO = self->miso_pad_config.pad_nr;
        spi->SPI.CTRLA.bit.DOPO = dopo;
        spi->SPI.CTRLA.bit.DORD = self->firstbit;

        // Enable receive only if miso is defined
        if (self->miso != 0xff) {
            spi->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;
            while (spi->SPI.SYNCBUSY.bit.CTRLB) {
            }
        }

        #if defined(MCU_SAMD51)
        spi->SPI.CTRLC.reg = 1; // 1 clock cycle character spacing
        #endif

        // SPI is driven by the clock of GCLK Generator 2, freq by get_peripheral_freq()
        // baud = bus_freq / (2 * baudrate) - 1
        uint32_t baud = get_peripheral_freq() / (2 * self->baudrate);
        if (baud > 0) {  // Avoid underflow
            baud -= 1;
        }
        if (baud > 255) { // Avoid overflow
            baud = 255;
        }
        spi->SPI.BAUD.reg = baud; // Set Baud

        // Enable RXC interrupt only if miso is defined
        if (self->miso != 0xff) {
            #if defined(MCU_SAMD21)
            NVIC_EnableIRQ(SERCOM0_IRQn + self->id);
            #elif defined(MCU_SAMD51)
            NVIC_EnableIRQ(SERCOM0_0_IRQn + 4 * self->id + 2);
            #endif
            sercom_register_irq(self->id, &common_spi_irq_handler);
        }

        sercom_enable(spi, 1);
    }
}

static mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get SPI bus.
    int spi_id = mp_obj_get_int(args[0]);
    if (spi_id < 0 || spi_id > SERCOM_INST_NUM) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI(%d) doesn't exist"), spi_id);
    }

    // Create the SPI object and fill it with defaults.
    machine_spi_obj_t *self = mp_obj_malloc(machine_spi_obj_t, &machine_spi_type);
    self->id = spi_id;
    self->baudrate = DEFAULT_SPI_BAUDRATE;
    self->polarity = DEFAULT_SPI_POLARITY;
    self->phase = DEFAULT_SPI_PHASE;
    self->firstbit = DEFAULT_SPI_FIRSTBIT;
    self->mosi = 0xff; // 0xff: pin not defined (yet)
    self->miso = 0xff;
    self->sck = 0xff;

    self->new = true;
    MP_STATE_PORT(sercom_table[spi_id]) = self;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_spi_init((mp_obj_base_t *)self, n_args - 1, args + 1, &kw_args);
    return self;
}

static void machine_sercom_deinit(mp_obj_base_t *self_in) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    Sercom *spi = sercom_instance[self->id];
    // Disable interrupts (if any)
    spi->SPI.INTENCLR.reg = 0xff;
    sercom_enable(spi, 0);
    // clear table entry of spi
    MP_STATE_PORT(sercom_table[self->id]) = NULL;
}

static void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;

    Sercom *spi = sercom_instance[self->id];
    size_t txlen = len;
    // Clear the input queue, if needed
    while (dest && spi->SPI.INTFLAG.bit.RXC) {
        uint32_t temp;
        (void)temp;
        temp = spi->SPI.DATA.bit.DATA;
    }
    // Set up the irq data pointers and enable IRQ
    if (dest) {
        if (self->miso == 0xff) {
            mp_raise_ValueError(MP_ERROR_TEXT("read is not enabled"));
        }
        spi->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_RXC;
        self->dest = dest;
        self->rxlen = len;
    }

    // Send by polling & receive by IRQ
    while (txlen) {
        if (spi->SPI.INTFLAG.bit.DRE) {
            spi->SPI.DATA.bit.DATA = *src;
            src += 1;
            txlen--;
        }
    }
    // Receive the remaining data, if any and clear IRQ
    // Do no wait forever.
    if (dest) {
        int32_t timeout = 1000;
        while (self->rxlen > 0 && timeout) {
            timeout--;
            MICROPY_EVENT_POLL_HOOK
        }
        spi->SPI.INTENCLR.reg = SERCOM_SPI_INTENCLR_RXC;
    } else {
        // Wait for the data being shifted out.
        while (!spi->SPI.INTFLAG.bit.TXC) {
        }
    }
}


static const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .deinit = machine_sercom_deinit,
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

#endif
