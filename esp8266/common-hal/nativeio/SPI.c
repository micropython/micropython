/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#include "esp8266/ets_alt_task.h"
#include "esp8266/hspi.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/nativeio/SPI.h"
#include "py/nlr.h"

#include "eagle_soc.h"
#include "c_types.h"
#include "gpio.h"

extern const mcu_pin_obj_t pin_MTMS;
extern const mcu_pin_obj_t pin_MTCK;
extern const mcu_pin_obj_t pin_MTDI;

void common_hal_nativeio_spi_construct(nativeio_spi_obj_t *self,
        const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi,
        const mcu_pin_obj_t * miso) {
    if (clock != &pin_MTMS || mosi != &pin_MTCK || miso != &pin_MTDI) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "Pins not valid for SPI"));
    }
    spi_init(HSPI);
}

void common_hal_nativeio_spi_deinit(nativeio_spi_obj_t *self) {
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 0);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTDI_U);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 0);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTCK_U);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 0);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTMS_U);

    // Turn off outputs 12 - 14.
    gpio_output_set(0x0, 0x0, 0x0, 0x7 << 12);
}

bool common_hal_nativeio_spi_configure(nativeio_spi_obj_t *self,
        uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    if (bits != 8) {
        return false;
    }
    if (baudrate == 80000000L) {
        // Special case for full speed.
        spi_init_gpio(HSPI, SPI_CLK_80MHZ_NODIV);
        spi_clock(HSPI, 0, 0);
    } else if (baudrate > 40000000L) {
        return false;
    } else {
        uint32_t divider = 40000000L / baudrate;
        uint16_t prediv = MIN(divider, SPI_CLKDIV_PRE + 1);
        uint16_t cntdiv = (divider / prediv) * 2; // cntdiv has to be even
        if (cntdiv > SPI_CLKCNT_N + 1 || cntdiv == 0 || prediv == 0) {
            return false;
        }
        spi_init_gpio(HSPI, SPI_CLK_USE_DIV);
        spi_clock(HSPI, prediv, cntdiv);
    }
    spi_mode(HSPI, phase, polarity);
    return true;
}

bool common_hal_nativeio_spi_try_lock(nativeio_spi_obj_t *self) {
    bool success = false;
    common_hal_mcu_disable_interrupts();
    if (!self->locked) {
        self->locked = true;
        success = true;
    }
    common_hal_mcu_enable_interrupts();
    return success;
}

bool common_hal_nativeio_spi_has_lock(nativeio_spi_obj_t *self) {
    return self->locked;
}

void common_hal_nativeio_spi_unlock(nativeio_spi_obj_t *self) {
    self->locked = false;
}

bool common_hal_nativeio_spi_write(nativeio_spi_obj_t *self,
        const uint8_t * data, size_t len) {
    size_t chunk_size = 1024;
    size_t count = len / chunk_size;
    size_t i = 0;
    for (size_t j = 0; j < count; ++j) {
        for (size_t k = 0; k < chunk_size; ++k) {
            spi_tx8fast(HSPI, data[i]);
            ++i;
        }
        ets_loop_iter();
    }
    while (i < len) {
        spi_tx8fast(HSPI, data[i]);
        ++i;
    }
    return true;
}

bool common_hal_nativeio_spi_read(nativeio_spi_obj_t *self,
        uint8_t * data, size_t len, uint8_t write_value) {
    // Process data in chunks, let the pending tasks run in between
    size_t chunk_size = 1024; // TODO this should depend on baudrate
    size_t count = len / chunk_size;
    size_t i = 0;
    uint32_t long_write_value = ((uint32_t) write_value) << 24 |
                                write_value << 16 |
                                write_value << 8 |
                                write_value;
    for (size_t j = 0; j < count; ++j) {
        for (size_t k = 0; k < chunk_size; ++k) {
            data[i] = spi_transaction(HSPI, 0, 0, 0, 0, 0, 0, 8, long_write_value);
            ++i;
        }
        ets_loop_iter();
    }
    while (i < len) {
        data[i] = spi_transaction(HSPI, 0, 0, 0, 0, 0, 0, 8, long_write_value);
        ++i;
    }
    return true;
}
