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
#include "shared-bindings/nativeio/SPI.h"
#include "py/nlr.h"

#include "eagle_soc.h"

extern const mcu_pin_obj_t pin_MTMS;
extern const mcu_pin_obj_t pin_MTCK;
extern const mcu_pin_obj_t pin_MTDI;

void common_hal_nativeio_spi_construct(nativeio_spi_obj_t *self,
        const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi,
        const mcu_pin_obj_t * miso, uint32_t baudrate) {
    if (clock != &pin_MTMS || mosi != &pin_MTCK || miso != &pin_MTDI) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "Pins not valid for SPI"));
    }
}

void common_hal_nativeio_spi_deinit(nativeio_spi_obj_t *self) {
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 0);
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
        uint8_t * data, size_t len) {
    // Process data in chunks, let the pending tasks run in between
    size_t chunk_size = 1024; // TODO this should depend on baudrate
    size_t count = len / chunk_size;
    size_t i = 0;
    for (size_t j = 0; j < count; ++j) {
        for (size_t k = 0; k < chunk_size; ++k) {
            data[i] = spi_rx8(HSPI);
            ++i;
        }
        ets_loop_iter();
    }
    while (i < len) {
        data[i] = spi_rx8(HSPI);
        ++i;
    }
    return true;
}
