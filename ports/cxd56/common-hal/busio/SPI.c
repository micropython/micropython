/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#include <arch/chip/pin.h>
#include <cxd56_spi.h>
#include <cxd56_pinconfig.h>

#include "py/runtime.h"

#include "shared-bindings/busio/SPI.h"

void common_hal_busio_spi_construct(busio_spi_obj_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    int port = -1;

    if (clock->number == PIN_SPI4_SCK &&
        (mosi == NULL || mosi->number == PIN_SPI4_MOSI) &&
        (miso == NULL || miso->number == PIN_SPI4_MISO)) {
        port = 4;
        CXD56_PIN_CONFIGS(PINCONFS_SPI4);
    } else if (clock->number == PIN_EMMC_CLK &&
               (mosi == NULL || mosi->number == PIN_EMMC_DATA0) &&
               (miso == NULL || miso->number == PIN_EMMC_DATA1)) {
        port = 5;
        CXD56_PIN_CONFIGS(PINCONFS_EMMCA_SPI5);
    }

    if (port < 0) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    claim_pin(clock);
    claim_pin(mosi);
    claim_pin(miso);

    self->clock_pin = clock;
    self->mosi_pin = mosi;
    self->miso_pin = miso;
    self->spi_dev = cxd56_spibus_initialize(port);
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }

    self->spi_dev = NULL;

    reset_pin_number(self->clock_pin->number);
    reset_pin_number(self->mosi_pin->number);
    reset_pin_number(self->miso_pin->number);
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->spi_dev == NULL;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    uint8_t mode;

    self->frequency = baudrate;
    SPI_SETFREQUENCY(self->spi_dev, baudrate);

    if (polarity == 0) {
        if (phase == 0) {
            mode = SPIDEV_MODE0;
        } else {
            mode = SPIDEV_MODE1;
        }
    } else {
        if (phase == 0) {
            mode = SPIDEV_MODE2;
        } else {
            mode = SPIDEV_MODE3;
        }
    }

    self->polarity = polarity;
    self->phase = phase;
    SPI_SETMODE(self->spi_dev, mode);

    self->bits = bits;
    SPI_SETBITS(self->spi_dev, bits);

    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self, const uint8_t *data, size_t len) {
    SPI_EXCHANGE(self->spi_dev, data, NULL, len);

    return true;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value) {
    SPI_EXCHANGE(self->spi_dev, NULL, data, len);

    return true;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len) {
    SPI_EXCHANGE(self->spi_dev, data_out, data_in, len);

    return true;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->frequency;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    never_reset_pin_number(self->clock_pin->number);
    never_reset_pin_number(self->mosi_pin->number);
    never_reset_pin_number(self->miso_pin->number);
}
