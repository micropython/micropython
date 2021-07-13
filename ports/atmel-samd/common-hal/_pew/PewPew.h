/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Radomir Dopieralski
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

#ifndef MICROPY_INCLUDED_PEW_PEWPEW_H
#define MICROPY_INCLUDED_PEW_PEWPEW_H

#include <stdint.h>
#include "shared-bindings/digitalio/DigitalInOut.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t *buffer;
    mp_obj_t *rows;
    mp_obj_t *cols;
    digitalio_digitalinout_obj_t *buttons;
    uint8_t rows_size;
    uint8_t cols_size;
    uint8_t pressed;
} pew_obj_t;

void pew_init(void);
void pewpew_interrupt_handler(uint8_t index);
void pew_reset(void);
uint16_t pew_get_ticks(void);

#endif  // MICROPY_INCLUDED_PEW_PEWPEW_H
