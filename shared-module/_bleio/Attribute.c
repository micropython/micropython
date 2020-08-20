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

#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/_bleio/Attribute.h"

void common_hal_bleio_attribute_security_mode_check_valid(bleio_attribute_security_mode_t security_mode) {
    switch (security_mode) {
        case SECURITY_MODE_NO_ACCESS:
        case SECURITY_MODE_OPEN:
        case SECURITY_MODE_ENC_NO_MITM:
        case SECURITY_MODE_ENC_WITH_MITM:
        case SECURITY_MODE_LESC_ENC_WITH_MITM:
        case SECURITY_MODE_SIGNED_NO_MITM:
        case SECURITY_MODE_SIGNED_WITH_MITM:
            break;
        default:
            mp_raise_ValueError(translate("Invalid security_mode"));
            break;
    }
}
