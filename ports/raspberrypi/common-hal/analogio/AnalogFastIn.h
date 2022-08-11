/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_ANALOGIO_ANALOGFASTIN_H
#define MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_ANALOGIO_ANALOGFASTIN_H

#include "common-hal/microcontroller/Pin.h"
#include "src/rp2_common/hardware_dma/include/hardware/dma.h"

#include "py/obj.h"

// We can extend the struct without impact to existing code
typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint8_t *buffer;
    uint32_t len;
    uint8_t bytes_per_sample;
    bool samples_signed;
    mp_float_t sample_rate;
    uint8_t chan;
    uint dma_chan;
    dma_channel_config cfg;
    // data_size       = DMA_SIZE_8;  //                                  - default DMA_SIZE_8
    // data_size       = DMA_SIZE_16;  //                                 - default DMA_SIZE_16
    // Either 12 bits in 16 or 12 over 2 bytes or truncate 12 to 8 in 8
    // Either B or H, default array.array("h", [0]*length) "h"==short (16 bits signed)
} analogio_analogfastin_obj_t;

// void common_hal_analogio_analogfastin_construct(analogio_analogfastin_obj_t *self, const mcu_pin_obj_t *pin, uint8_t *buffer, uint32_t len, uint8_t bytes_per_sample, bool samples_signed, uint32_t sample_rate)
/*
void common_hal_analogio_analogfastin_construct(analogio_analogfastin_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint8_t *buffer,
    uint32_t len,
    uint8_t bytes_per_sample,
    bool samples_signed,
    uint32_t sample_rate) {
    self->pin = pin;
    self->buffer = buffer;
    self->len = len;
    //self->bits_per_sample = bytes_per_sample * 8;
    self->samples_signed = samples_signed;
    self->sample_rate = sample_rate;
}
*/

void analogfastin_init(void);

#endif // MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_ANALOGIO_ANALOGFASTIN_H
