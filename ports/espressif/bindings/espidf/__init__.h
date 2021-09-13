/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ESP32S2_BINDINGS_ESPIDF___INIT___H
#define MICROPY_INCLUDED_ESP32S2_BINDINGS_ESPIDF___INIT___H

#include "esp_err.h"
#include "py/mpconfig.h"
#include "py/obj.h"

extern const mp_obj_type_t mp_type_espidf_IDFError;
extern const mp_obj_type_t mp_type_espidf_MemoryError;

NORETURN void mp_raise_espidf_MemoryError(void);

void raise_esp_error(esp_err_t err) NORETURN;
#define CHECK_ESP_RESULT(x) do { int res = (x); if (res != ESP_OK) raise_esp_error(res); } while (0)

#endif  // MICROPY_INCLUDED_ESP32S2_BINDINGS_ESPIDF___INIT___H
