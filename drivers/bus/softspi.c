/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Damien P. George
 * Copyright (c) 2019 "Eric Poulsen" <eric@zyxod.com>
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

#include "drivers/bus/spi.h"
#include "py/runtime.h"

int mp_soft_spi_ioctl(void *self_in, uint32_t cmd) {
    mp_soft_spi_obj_t *self = (mp_soft_spi_obj_t*)self_in;

    switch (cmd) {
        case MP_SPI_IOCTL_INIT:
            mp_hal_pin_write(self->sck, self->polarity);
            mp_hal_pin_output(self->sck);
            mp_hal_pin_output(self->mosi);
            mp_hal_pin_input(self->miso);
            break;

        case MP_SPI_IOCTL_DEINIT:
            break;
    }

    return 0;
}

void mp_soft_spi_transfer(void *self_in, size_t len, const uint8_t *src, uint8_t *dest, uint8_t bits) {
    mp_soft_spi_obj_t *self = (mp_soft_spi_obj_t*)self_in;
    uint32_t delay_half = self->delay_half;

    bits = bits ? bits : self->bits;
    if (bits == 0) {
        // Shoudln't be possible with soft SPI, but
        // just in case.
        mp_raise_ValueError(MP_ERROR_TEXT("bits cannot be 0"));
    }

    int bytesPerChunk = (bits + 7) / 8;
    // round length down as needed (possibly to zero)
    len = len / bytesPerChunk * bytesPerChunk;
    if(len == 0) {
        return;
    }

    uint8_t remBits = bits;

    // only MSB transfer is implemented

    // If a port defines MICROPY_HW_SOFTSPI_MIN_DELAY, and the configured
    // delay_half is equal to this value, then the software SPI implementation
    // will run as fast as possible, limited only by CPU speed and GPIO time.
    #ifdef MICROPY_HW_SOFTSPI_MIN_DELAY
    if (delay_half == MICROPY_HW_SOFTSPI_MIN_DELAY)
    {
        while (len--) {
            uint8_t bitsThisByte = remBits < 8 ? remBits : 8;
            uint8_t data_out = *src++;
            uint8_t data_in = 0;

            remBits -= bitsThisByte;
            for (; bitsThisByte; --bitsThisByte, data_out <<= 1) {
                mp_hal_pin_write(self->mosi, (data_out >> 7) & 1);
                mp_hal_pin_write(self->sck, 1 - self->polarity);
                data_in = (data_in << 1) | mp_hal_pin_read(self->miso);
                mp_hal_pin_write(self->sck, self->polarity);
            }
            remBits = remBits ? remBits : bits;
            if (dest != NULL) {
                *dest++ = data_in;
            }
        }
        return;
    }
#endif


    while(len--) {
        uint8_t bitsThisByte = remBits < 8 ? remBits : 8;
        uint8_t data_out = *src++;
        uint8_t data_in = 0;

        remBits -= bitsThisByte;
        for(;bitsThisByte; --bitsThisByte, data_out <<= 1) {
            mp_hal_pin_write(self->mosi, (data_out >> 7) & 1);
            if (self->phase == 0) {
                mp_hal_delay_us_fast(delay_half);
                mp_hal_pin_write(self->sck, 1 - self->polarity);
            } else {
                mp_hal_pin_write(self->sck, 1 - self->polarity);
                mp_hal_delay_us_fast(delay_half);
            }
            data_in = (data_in << 1) | mp_hal_pin_read(self->miso);
            if (self->phase == 0) {
                mp_hal_delay_us_fast(delay_half);
                mp_hal_pin_write(self->sck, self->polarity);
            } else {
                mp_hal_pin_write(self->sck, self->polarity);
                mp_hal_delay_us_fast(delay_half);
            }
        }
        remBits = remBits ? remBits : bits;
        if (dest != NULL) {
            *dest++ = data_in;
        }
    }
}

const mp_spi_proto_t mp_soft_spi_proto = {
    .ioctl = mp_soft_spi_ioctl,
    .transfer = mp_soft_spi_transfer,
};
