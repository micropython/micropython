/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "genhdr/mpversion.h"
#include "py/mpprint.h"
#include "shared-bindings/help.h"

extern const mp_map_t mp_builtin_module_map;

void shared_module_help(void) {
    mp_printf(&mp_plat_print,
        "Welcome to Adafruit CircuitPython " MICROPY_GIT_TAG "!\r\n"
        "\r\n"
        "Please visit learn.adafruit.com/category/circuitpython for project guides.\r\n"
        "\r\n"
        "Built in modules:\r\n");
    for (int i = 0; i < mp_builtin_module_map.used; i++) {
        mp_printf(&mp_plat_print, "\t%q\r\n", MP_OBJ_QSTR_VALUE(mp_builtin_module_map.table[i].key));
    }
}
