/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#include <string.h>

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "nrfx_spim.h"
#include "nrf_gpio.h"

#ifndef NRFX_SPIM3_ENABLED
#define NRFX_SPIM3_ENABLED (0)
#endif

#ifndef NRFX_SPIM2_ENABLED
#define NRFX_SPIM2_ENABLED (0)
#endif

#ifndef NRFX_SPIM1_ENABLED
#define NRFX_SPIM1_ENABLED (0)
#endif

#ifndef NRFX_SPIM0_ENABLED
#define NRFX_SPIM0_ENABLED (0)
#endif

// These are in order from highest available frequency to lowest (32MHz first, then 8MHz).
STATIC const spim_peripheral_t spim_peripherals[] = {
    #if NRFX_CHECK(NRFX_SPIM3_ENABLED)
    // SPIM3 exists only on nRF52840 and supports 32MHz max. All other SPIM's are only 8MHz max.
    // Allocate SPIM3 first.
    { .spim = NRFX_SPIM_INSTANCE(3),
      .max_frequency = 32000000,
      .max_xfer_size = MIN(SPIM3_BUFFER_RAM_SIZE, (1UL << SPIM3_EASYDMA_MAXCNT_SIZE) - 1)},
    #endif
    #if NRFX_CHECK(NRFX_SPIM2_ENABLED)
    // SPIM2 is not shared with a TWIM, so allocate before the shared ones.
    { .spim = NRFX_SPIM_INSTANCE(2),
      .max_frequency = 8000000,
      .max_xfer_size = (1UL << SPIM2_EASYDMA_MAXCNT_SIZE) - 1},
    #endif
    #if NRFX_CHECK(NRFX_SPIM1_ENABLED)
    // SPIM1 and TWIM1 share an address.
    { .spim = NRFX_SPIM_INSTANCE(1),
      .max_frequency = 8000000,
      .max_xfer_size = (1UL << SPIM1_EASYDMA_MAXCNT_SIZE) - 1},
    #endif
    #if NRFX_CHECK(NRFX_SPIM0_ENABLED)
    // SPIM0 and TWIM0 share an address.
    { .spim = NRFX_SPIM_INSTANCE(0),
      .max_frequency = 8000000,
      .max_xfer_size = (1UL << SPIM0_EASYDMA_MAXCNT_SIZE) - 1},
    #endif
};

STATIC bool never_reset[MP_ARRAY_SIZE(spim_peripherals)];

// Separate RAM area for SPIM3 transmit buffer to avoid SPIM3 hardware errata.
// https://infocenter.nordicsemi.com/index.jsp?topic=%2Ferrata_nRF52840_Rev2%2FERR%2FnRF52840%2FRev2%2Flatest%2Fanomaly_840_198.html
STATIC uint8_t *spim3_transmit_buffer = (uint8_t *)SPIM3_BUFFER_RAM_START_ADDR;

void spi_reset(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(spim_peripherals); i++) {
        if (never_reset[i]) {
            continue;
        }
        nrfx_spim_uninit(&spim_peripherals[i].spim);
    }
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(spim_peripherals); i++) {
        if (self->spim_peripheral == &spim_peripherals[i]) {
            never_reset[i] = true;

            never_reset_pin_number(self->clock_pin_number);
            never_reset_pin_number(self->MOSI_pin_number);
            never_reset_pin_number(self->MISO_pin_number);
            break;
        }
    }
}

// Convert frequency to clock-speed-dependent value. Choose the next lower baudrate if in between
// available baudrates.
static nrf_spim_frequency_t baudrate_to_spim_frequency(const uint32_t baudrate) {

    static const struct {
        const uint32_t boundary;
        nrf_spim_frequency_t spim_frequency;
    } baudrate_map[] = {
        #ifdef SPIM_FREQUENCY_FREQUENCY_M32
        { 32000000, NRF_SPIM_FREQ_32M },
        #endif
        #ifdef SPIM_FREQUENCY_FREQUENCY_M16
        { 16000000, NRF_SPIM_FREQ_16M },
        #endif
        {  8000000, NRF_SPIM_FREQ_8M },
        {  4000000, NRF_SPIM_FREQ_4M },
        {  2000000, NRF_SPIM_FREQ_2M },
        {  1000000, NRF_SPIM_FREQ_1M },
        {   500000, NRF_SPIM_FREQ_500K },
        {   250000, NRF_SPIM_FREQ_250K },
        {        0, NRF_SPIM_FREQ_125K },
    };

    size_t i = 0;
    uint32_t boundary;
    do {
        boundary = baudrate_map[i].boundary;
        if (baudrate >= boundary) {
            return baudrate_map[i].spim_frequency;
        }
        i++;
    } while (boundary != 0);
    // Should not get here.
    return 0;
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self, const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    // Find a free instance, with most desirable (highest freq and not shared) allocated first.
    self->spim_peripheral = NULL;
    for (size_t i = 0; i < MP_ARRAY_SIZE(spim_peripherals); i++) {
        if ((spim_peripherals[i].spim.p_reg->ENABLE & SPIM_ENABLE_ENABLE_Msk) == 0) {
            self->spim_peripheral = &spim_peripherals[i];
            break;
        }
    }

    if (self->spim_peripheral == NULL) {
        mp_raise_ValueError(translate("All SPI peripherals are in use"));
    }

    nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG(NRFX_SPIM_PIN_NOT_USED, NRFX_SPIM_PIN_NOT_USED,
        NRFX_SPIM_PIN_NOT_USED, NRFX_SPIM_PIN_NOT_USED);

    config.frequency = baudrate_to_spim_frequency(self->spim_peripheral->max_frequency);

    config.sck_pin = clock->number;
    self->clock_pin_number = clock->number;
    claim_pin(clock);

    if (mosi != NULL) {
        config.mosi_pin = mosi->number;
        self->MOSI_pin_number = mosi->number;
        claim_pin(mosi);
    } else {
        self->MOSI_pin_number = NO_PIN;
    }

    if (miso != NULL) {
        config.miso_pin = miso->number;
        self->MISO_pin_number = miso->number;
        claim_pin(miso);
    } else {
        self->MISO_pin_number = NO_PIN;
    }

    nrfx_err_t err = nrfx_spim_init(&self->spim_peripheral->spim, &config, NULL, NULL);
    if (err != NRFX_SUCCESS) {
        common_hal_busio_spi_deinit(self);
        mp_raise_OSError(MP_EIO);
    }
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->clock_pin_number == NO_PIN;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }

    nrfx_spim_uninit(&self->spim_peripheral->spim);

    reset_pin_number(self->clock_pin_number);
    reset_pin_number(self->MOSI_pin_number);
    reset_pin_number(self->MISO_pin_number);
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    // nrf52 does not support 16 bit
    if (bits != 8) {
        return false;
    }

    // Set desired frequency, rounding down, and don't go above available frequency for this SPIM.
    nrf_spim_frequency_set(self->spim_peripheral->spim.p_reg,
        baudrate_to_spim_frequency(MIN(baudrate, self->spim_peripheral->max_frequency)));

    nrf_spim_mode_t mode = NRF_SPIM_MODE_0;
    if (polarity) {
        mode = (phase) ? NRF_SPIM_MODE_3 : NRF_SPIM_MODE_2;
    } else {
        mode = (phase) ? NRF_SPIM_MODE_1 : NRF_SPIM_MODE_0;
    }

    nrf_spim_configure(self->spim_peripheral->spim.p_reg, mode, NRF_SPIM_BIT_ORDER_MSB_FIRST);

    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
    // NRFX_CRITICAL_SECTION_ENTER();
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    // NRFX_CRITICAL_SECTION_EXIT();
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self, const uint8_t *data, size_t len) {
    const bool is_spim3 = self->spim_peripheral->spim.p_reg == NRF_SPIM3;
    uint8_t *next_chunk = (uint8_t *)data;

    while (len > 0) {
        size_t chunk_size = MIN(len, self->spim_peripheral->max_xfer_size);
        uint8_t *chunk = next_chunk;
        if (is_spim3) {
            // If SPIM3, copy into unused RAM block, and do DMA from there.
            memcpy(spim3_transmit_buffer, chunk, chunk_size);
            chunk = spim3_transmit_buffer;
        }
        const nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_XFER_TX(chunk, chunk_size);
        if (nrfx_spim_xfer(&self->spim_peripheral->spim, &xfer, 0) != NRFX_SUCCESS) {
            return false;
        }
        next_chunk += chunk_size;
        len -= chunk_size;
    }
    return true;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value) {
    uint8_t *next_chunk = data;

    while (len > 0) {
        size_t chunk_size = MIN(len, self->spim_peripheral->max_xfer_size);
        const nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_XFER_RX(next_chunk, chunk_size);
        if (nrfx_spim_xfer(&self->spim_peripheral->spim, &xfer, 0) != NRFX_SUCCESS) {
            return false;
        }
        next_chunk += chunk_size;
        len -= chunk_size;
    }
    return true;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len) {
    const bool is_spim3 = self->spim_peripheral->spim.p_reg == NRF_SPIM3;
    const uint8_t *next_chunk_out = data_out;
    uint8_t *next_chunk_in = data_in;

    while (len > 0) {
        const uint8_t *chunk_out = next_chunk_out;
        size_t chunk_size = MIN(len, self->spim_peripheral->max_xfer_size);
        if (is_spim3) {
            // If SPIM3, copy into unused RAM block, and do DMA from there.
            memcpy(spim3_transmit_buffer, chunk_out, chunk_size);
            chunk_out = spim3_transmit_buffer;
        }
        const nrfx_spim_xfer_desc_t xfer =
            NRFX_SPIM_SINGLE_XFER(next_chunk_out, chunk_size,
                next_chunk_in, chunk_size);
        if (nrfx_spim_xfer(&self->spim_peripheral->spim, &xfer, 0) != NRFX_SUCCESS) {
            return false;
        }

        next_chunk_out += chunk_size;
        next_chunk_in += chunk_size;
        len -= chunk_size;
    }
    return true;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    switch (self->spim_peripheral->spim.p_reg->FREQUENCY) {
        case NRF_SPIM_FREQ_125K:
            return 125000;
        case NRF_SPIM_FREQ_250K:
            return 250000;
        case NRF_SPIM_FREQ_500K:
            return 500000;
        case NRF_SPIM_FREQ_1M:
            return 1000000;
        case NRF_SPIM_FREQ_2M:
            return 2000000;
        case NRF_SPIM_FREQ_4M:
            return 4000000;
        case NRF_SPIM_FREQ_8M:
            return 8000000;
        #ifdef SPIM_FREQUENCY_FREQUENCY_M16
        case NRF_SPIM_FREQ_16M:
            return 16000000;
        #endif
        #ifdef SPIM_FREQUENCY_FREQUENCY_M32
        case NRF_SPIM_FREQ_32M:
            return 32000000;
        #endif
        default:
            return 0;
    }
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return (self->spim_peripheral->spim.p_reg->CONFIG & SPIM_CONFIG_CPHA_Msk) >> SPIM_CONFIG_CPHA_Pos;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return (self->spim_peripheral->spim.p_reg->CONFIG & SPIM_CONFIG_CPOL_Msk) >> SPIM_CONFIG_CPOL_Pos;
}
