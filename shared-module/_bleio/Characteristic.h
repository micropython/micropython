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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_BLEIO_CHARACTERISTIC_H
#define MICROPY_INCLUDED_SHARED_MODULE_BLEIO_CHARACTERISTIC_H

typedef enum {
    CHAR_PROP_NONE =              0,
    CHAR_PROP_BROADCAST =         1u << 0,
    CHAR_PROP_INDICATE =          1u << 1,
    CHAR_PROP_NOTIFY =            1u << 2,
    CHAR_PROP_READ =              1u << 3,
    CHAR_PROP_WRITE =             1u << 4,
    CHAR_PROP_WRITE_NO_RESPONSE = 1u << 5,
    CHAR_PROP_ALL =               (CHAR_PROP_BROADCAST | CHAR_PROP_INDICATE | CHAR_PROP_NOTIFY |
                                   CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_NO_RESPONSE)
} bleio_characteristic_properties_enum_t;
typedef uint8_t bleio_characteristic_properties_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_BLEIO_CHARACTERISTIC_H
