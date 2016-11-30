/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "mpconfigport.h"

#include "py/nlr.h"
#include "py/obj.h"

#include "common-hal/microcontroller/types.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/nativeio/DigitalInOut.h"
#include "shared-module/bitbangio/types.h"

#define MAX_BAUDRATE (common_hal_mcu_get_clock_frequency() / 48)

extern void shared_module_bitbangio_spi_construct(bitbangio_spi_obj_t *self,
        const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi,
        const mcu_pin_obj_t * miso, uint32_t baudrate) {
    digitalinout_result_t result = common_hal_nativeio_digitalinout_construct(&self->clock, clock);
    if (result != DIGITALINOUT_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
            "Clock pin init failed."));
    }
    if (mosi != mp_const_none) {
        result = common_hal_nativeio_digitalinout_construct(&self->mosi, mosi);
        if (result != DIGITALINOUT_OK) {
            common_hal_nativeio_digitalinout_deinit(&self->clock);
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                "MOSI pin init failed."));
        }
        self->has_mosi = true;
    }
    if (miso != mp_const_none) {
        result = common_hal_nativeio_digitalinout_construct(&self->miso, miso);
        if (result != DIGITALINOUT_OK) {
            common_hal_nativeio_digitalinout_deinit(&self->clock);
            if (mosi != mp_const_none) {
                common_hal_nativeio_digitalinout_deinit(&self->mosi);
            }
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                "MISO pin init failed."));
        }
        self->has_miso = true;
    }

    self->delay_half = 500000 / baudrate;
    // round delay_half up so that: actual_baudrate <= requested_baudrate
    if (500000 % baudrate != 0) {
        self->delay_half += 1;
    }

    self->polarity = 0;
    self->phase = 0;
}

extern void shared_module_bitbangio_spi_deinit(bitbangio_spi_obj_t *self) {
    common_hal_nativeio_digitalinout_deinit(&self->clock);
    if (self->has_mosi) {
        common_hal_nativeio_digitalinout_deinit(&self->mosi);
    }
    if (self->has_miso) {
        common_hal_nativeio_digitalinout_deinit(&self->miso);
    }
}

bool shared_module_bitbangio_spi_transfer(bitbangio_spi_obj_t *self,
        const uint8_t *write_buffer, size_t write_buffer_len,
        uint8_t *read_buffer, size_t read_buffer_len) {
    if (write_buffer_len > 0 && !self->has_mosi) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
            "Cannot write without MOSI pin."));
    }
    if (read_buffer_len > 0 && !self->has_miso) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
            "Cannot read without MISO pin."));
    }
    uint32_t delay_half = self->delay_half;

    // only MSB transfer is implemented

    // If a port defines MICROPY_PY_MACHINE_SPI_MIN_DELAY, and the configured
    // delay_half is equal to this value, then the software SPI implementation
    // will run as fast as possible, limited only by CPU speed and GPIO time.
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (delay_half <= MICROPY_PY_MACHINE_SPI_MIN_DELAY) {
        for (size_t i = 0; i < write_buffer_len; ++i) {
            uint8_t data_out = write_buffer[i];
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                common_hal_nativeio_digitalinout_set_value(&self->mosi, (data_out >> 7) & 1);
                common_hal_nativeio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                common_hal_nativeio_digitalinout_set_value(&self->clock, self->polarity);
            }
            if (dest != NULL) {
                dest[i] = data_in;
            }
        }

        // Clock out zeroes while we read.
        if (self->has_mosi) {
            common_hal_nativeio_digitalinout_set_value(&self->mosi, false);
        }
        for (size_t i = 0; i < read_buffer_len; ++i) {
            uint8_t data_in = 0;
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                common_hal_nativeio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                data_in = (data_in << 1) | common_hal_nativeio_digitalinout_get_value(&self->miso);
                common_hal_nativeio_digitalinout_set_value(&self->clock, self->polarity);
            }
            read_buffer[i] = data_in;
        }
        return true;
    }
    #endif

    for (size_t i = 0; i < write_buffer_len; ++i) {
        uint8_t data_out = write_buffer[i];
        for (int j = 0; j < 8; ++j, data_out <<= 1) {
            common_hal_nativeio_digitalinout_set_value(&self->mosi, (data_out >> 7) & 1);
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                common_hal_nativeio_digitalinout_set_value(&self->clock, 1 - self->polarity);
            } else {
                common_hal_nativeio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                common_hal_nativeio_digitalinout_set_value(&self->clock, self->polarity);
            } else {
                common_hal_nativeio_digitalinout_set_value(&self->clock, self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
        }

        // Some ports need a regular callback, but probably we don't need
        // to do this every byte, or even at all.
        #ifdef MICROPY_EVENT_POLL_HOOK
        MICROPY_EVENT_POLL_HOOK;
        #endif
    }
    if (self->has_mosi) {
        common_hal_nativeio_digitalinout_set_value(&self->mosi, false);
    }
    for (size_t i = 0; i < read_buffer_len; ++i) {
        uint8_t data_in = 0;
        for (int j = 0; j < 8; ++j) {
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                common_hal_nativeio_digitalinout_set_value(&self->clock, 1 - self->polarity);
            } else {
                common_hal_nativeio_digitalinout_set_value(&self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
            data_in = (data_in << 1) | common_hal_nativeio_digitalinout_get_value(&self->miso);
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                common_hal_nativeio_digitalinout_set_value(&self->clock, self->polarity);
            } else {
                common_hal_nativeio_digitalinout_set_value(&self->clock, self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
        }
        read_buffer[i] = data_in;

        // Some ports need a regular callback, but probably we don't need
        // to do this every byte, or even at all.
        #ifdef MICROPY_EVENT_POLL_HOOK
        MICROPY_EVENT_POLL_HOOK;
        #endif
    }
    return true;
}
