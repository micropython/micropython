/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_AESIO__INIT__H
#define MICROPY_INCLUDED_SHARED_MODULE_AESIO__INIT__H

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"

#include "shared-module/aesio/aes.h"

// These values were chosen to correspond with the values
// present in pycrypto.
enum AES_MODE {
    AES_MODE_ECB = 1,
    AES_MODE_CBC = 2,
    AES_MODE_CTR = 6,
};

typedef struct {
    mp_obj_base_t base;

    // The tinyaes context
    struct AES_ctx ctx;

    // Which AES mode this instance of the object is configured to use
    enum AES_MODE mode;

    // Counter for running in CTR mode
    uint32_t counter;
} aesio_aes_obj_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_AESIO__INIT__H
