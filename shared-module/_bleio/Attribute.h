/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_BLEIO_ATTRIBUTE_H
#define MICROPY_INCLUDED_SHARED_MODULE_BLEIO_ATTRIBUTE_H

// BLE security modes: 0x<level><mode>
typedef enum {
    SECURITY_MODE_NO_ACCESS = 0x00,
    SECURITY_MODE_OPEN = 0x11,
    SECURITY_MODE_ENC_NO_MITM = 0x21,
    SECURITY_MODE_ENC_WITH_MITM = 0x31,
    SECURITY_MODE_LESC_ENC_WITH_MITM = 0x41,
    SECURITY_MODE_SIGNED_NO_MITM = 0x12,
    SECURITY_MODE_SIGNED_WITH_MITM = 0x22,
} bleio_attribute_security_mode_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_BLEIO_ATTRIBUTE_H
