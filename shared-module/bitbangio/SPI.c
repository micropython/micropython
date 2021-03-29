/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/bitbangio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/shared/translate.h"

#define MAX_BAUDRATE (common_hal_mcu_get_clock_frequency() / 48)

void shared_module_bitbangio_spi_construct(bitbangio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso) {
    digitalinout_result_t result = common_hal_digitalio_digitalinout_construct(&self->clock, clock);
    if (result != DIGITALINOUT_OK) {
        mp_raise_ValueError(translate("Clock pin init failed."));
    }
    common_hal_digitalio_digitalinout_switch_to_output(&self->clock, self->polarity == 1, DRIVE_MODE_PUSH_PULL);

    if (mosi != NULL) {
        result = common_hal_digitalio_digitalinout_construct(&self->mosi, mosi);
        if (result != DIGITALINOUT_OK) {
            common_hal_digitalio_digitalinout_deinit(&self->clock);
            mp_raise_ValueError(translate("MOSI pin init failed."));
        }
        self->has_mosi = true;
        common_hal_digitalio_digitalinout_switch_to_output(&self->mosi, false, DRIVE_MODE_PUSH_PULL);
    }

    if (miso != NULL) {
        // Starts out as input by default, no need to change.
        result = common_hal_digitalio_digitalinout_construct(&self->miso, miso);
        if (result != DIGITALINOUT_OK) {
            common_hal_digitalio_digitalinout_deinit(&self->clock);
            if (mosi != NULL) {
                common_hal_digitalio_digitalinout_deinit(&self->mosi);
            }
            mp_raise_ValueError(translate("MISO pin init failed."));
        }
        self->has_miso = true;
    }
    self->delay_half = 5;
    self->polarity = 0;
    self->phase = 0;
}

bool shared_module_bitbangio_spi_deinited(bitbangio_spi_obj_t *self) {
    return common_hal_digitalio_digitalinout_deinited(&self->clock);
}

void shared_module_bitbangio_spi_deinit(bitbangio_spi_obj_t *self) {
    if (shared_module_bitbangio_spi_deinited(self)) {
        return;
    }
    common_hal_digitalio_digitalinout_deinit(&self->clock);
    if (self->has_mosi) {
        common_hal_digitalio_digitalinout_deinit(&self->mosi);
    }
    if (self->has_miso) {
        common_hal_digitalio_digitalinout_deinit(&self->miso);
    }
}

void shared_module_bitbangio_spi_configure(bitbangio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    self->delay_half = 500000 / baudrate;
    // round delay_half up so that: actual_baudrate <= requested_baudrate
    if (500000 % baudrate != 0) {
        self->delay_half += 1;
    }

    self->polarity = polarity;
    self->phase = phase;
}

bool shared_module_bitbangio_spi_try_lock(bitbangio_spi_obj_t *self) {
    bool success = false;
    common_hal_mcu_disable_interrupts();
    if (!self->locked) {
        self->locked = true;
        success = true;
    }
    common_hal_mcu_enable_interrupts();
    return success;
}

bool shared_module_bitbangio_spi_has_lock(bitbangio_spi_obj_t *self) {
    return self->locked;
}

void shared_module_bitbangio_spi_unlock(bitbangio_spi_obj_t *self) {
    self->locked = false;
}

// Writes out the given data.
bool shared_module_bitbangio_spi_write(bitbangio_spi_obj_t *self, const uint8_t *data, size_t len) {
    if (len > 0 && !self->has_mosi) {
        mp_raise_ValueError(translate("Cannot write without MOSI pin."));
    }
    uint32_t delay_half = self->delay_half;

    // only MSB transfer is implemented

    // If a port defines MICROPY_PY_MACHINE_SPI_MIN_DELAY, and the configured
    // delay_half is equal to this value, then the software SPI implementation
    // will run as fast as possible, limited only by CPU speed and GPIO time.
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (delay_half <= MICROPY_PY_MACHINE_SPI_MIN_DELAY) {
        for (size_t i = 0; i < len; ++i) {
            uint8_t data_out = data[i];
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                common_hal_digitalio_digitalinout_set_value(&self->mosi, (data_out >> 7) & 1);
                common_hal_digitalio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                common_hal_digitalio_digitalinout_set_value(&self->clock, self->polarity);
            }
            if (dest != NULL) {
                dest[i] = data_in;
            }
        }
        return true;
    }
    #endif

    for (size_t i = 0; i < len; ++i) {
        uint8_t data_out = data[i];
        for (int j = 0; j < 8; ++j, data_out <<= 1) {
            common_hal_digitalio_digitalinout_set_value(&self->mosi, (data_out >> 7) & 1);
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                common_hal_digitalio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
                common_hal_digitalio_digitalinout_set_value(&self->clock, self->polarity);
            } else {
                common_hal_digitalio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
                common_hal_digitalio_digitalinout_set_value(&self->clock, self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
        }

        // Some ports need a regular callback, but probably we don't need
        // to do this every byte, or even at all.
        #ifdef MICROPY_EVENT_POLL_HOOK
        MICROPY_EVENT_POLL_HOOK;
        #endif
    }
    return true;
}

// Reads in len bytes while outputting zeroes.
bool shared_module_bitbangio_spi_read(bitbangio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_data) {
    if (len > 0 && !self->has_miso) {
        mp_raise_ValueError(translate("Cannot read without MISO pin."));
    }

    uint32_t delay_half = self->delay_half;

    // only MSB transfer is implemented

    // If a port defines MICROPY_PY_MACHINE_SPI_MIN_DELAY, and the configured
    // delay_half is equal to this value, then the software SPI implementation
    // will run as fast as possible, limited only by CPU speed and GPIO time.
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (delay_half <= MICROPY_PY_MACHINE_SPI_MIN_DELAY) {
        // Clock out zeroes while we read.
        if (self->has_mosi) {
            common_hal_digitalio_digitalinout_set_value(&self->mosi, false);
        }
        for (size_t i = 0; i < len; ++i) {
            uint8_t data_in = 0;
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                common_hal_digitalio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                data_in = (data_in << 1) | common_hal_digitalio_digitalinout_get_value(&self->miso);
                common_hal_digitalio_digitalinout_set_value(&self->clock, self->polarity);
            }
            data[i] = data_in;
        }
        return true;
    }
    #endif
    if (self->has_mosi) {
        common_hal_digitalio_digitalinout_set_value(&self->mosi, false);
    }
    for (size_t i = 0; i < len; ++i) {
        uint8_t data_out = write_data;
        uint8_t data_in = 0;
        for (int j = 0; j < 8; ++j, data_out <<= 1) {
            if (self->has_mosi) {
                common_hal_digitalio_digitalinout_set_value(&self->mosi, (data_out >> 7) & 1);
            }
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                common_hal_digitalio_digitalinout_set_value(&self->clock, 1 - self->polarity);
            } else {
                common_hal_digitalio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
            data_in = (data_in << 1) | common_hal_digitalio_digitalinout_get_value(&self->miso);
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                common_hal_digitalio_digitalinout_set_value(&self->clock, self->polarity);
            } else {
                common_hal_digitalio_digitalinout_set_value(&self->clock, self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
        }
        data[i] = data_in;

        // Some ports need a regular callback, but probably we don't need
        // to do this every byte, or even at all.
        #ifdef MICROPY_EVENT_POLL_HOOK
        MICROPY_EVENT_POLL_HOOK;
        #endif
    }
    return true;
}

// transfer
bool shared_module_bitbangio_spi_transfer(bitbangio_spi_obj_t *self, const uint8_t *dout, uint8_t *din, size_t len) {
    if (len > 0 && (!self->has_mosi || !self->has_miso)) {
        mp_raise_ValueError(translate("Cannot transfer without MOSI and MISO pins."));
    }
    uint32_t delay_half = self->delay_half;

    // only MSB transfer is implemented

    // If a port defines MICROPY_PY_MACHINE_SPI_MIN_DELAY, and the configured
    // delay_half is equal to this value, then the software SPI implementation
    // will run as fast as possible, limited only by CPU speed and GPIO time.
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (delay_half <= MICROPY_PY_MACHINE_SPI_MIN_DELAY) {
        for (size_t i = 0; i < len; ++i) {
            uint8_t data_out = dout[i];
            uint8_t data_in = 0;
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                common_hal_digitalio_digitalinout_set_value(&self->mosi, (data_out >> 7) & 1);
                common_hal_digitalio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                data_in = (data_in << 1) | common_hal_digitalio_digitalinout_get_value(&self->miso);
                common_hal_digitalio_digitalinout_set_value(&self->clock, self->polarity);
            }
            din[i] = data_in;

            if (dest != NULL) {
                dest[i] = data_in;
            }
        }
        return true;
    }
    #endif

    for (size_t i = 0; i < len; ++i) {
        uint8_t data_out = dout[i];
        uint8_t data_in = 0;
        for (int j = 0; j < 8; ++j, data_out <<= 1) {
            common_hal_digitalio_digitalinout_set_value(&self->mosi, (data_out >> 7) & 1);
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                common_hal_digitalio_digitalinout_set_value(&self->clock, 1 - self->polarity);
            } else {
                common_hal_digitalio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
            data_in = (data_in << 1) | common_hal_digitalio_digitalinout_get_value(&self->miso);
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                common_hal_digitalio_digitalinout_set_value(&self->clock, self->polarity);
            } else {
                common_hal_digitalio_digitalinout_set_value(&self->clock, self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
        }
        din[i] = data_in;

        // Some ports need a regular callback, but probably we don't need
        // to do this every byte, or even at all.
        #ifdef MICROPY_EVENT_POLL_HOOK
        MICROPY_EVENT_POLL_HOOK;
        #endif
    }
    return true;
}
