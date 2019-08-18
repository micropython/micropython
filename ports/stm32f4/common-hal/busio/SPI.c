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

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "boards/board.h"
#include "common-hal/microcontroller/Pin.h"

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
         const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi,
         const mcu_pin_obj_t * miso) {
    mp_raise_NotImplementedError(translate("SPI not yet supported"));
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return 0;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
         uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
     return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
        const uint8_t *data, size_t len) {
    return 0;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
        uint8_t *data, size_t len, uint8_t write_value) {
    return 0;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, uint8_t *data_out, uint8_t *data_in, size_t len) {
    return 0;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t* self) {
    return 0;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t* self) {
    return 0;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t* self) {
    return 0;
}