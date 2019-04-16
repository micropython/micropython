/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
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

#ifndef NRF5_MPCONFIGPORT_H__
#define NRF5_MPCONFIGPORT_H__

#include "ble_drv.h"

#define MICROPY_PY_COLLECTIONS_ORDEREDDICT       (1)
#define MICROPY_PY_FUNCTION_ATTRS                (1)
#define MICROPY_PY_IO                            (1)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS       (1)
#define MICROPY_PY_SYS_STDIO_BUFFER              (1)
#define MICROPY_PY_UBINASCII                     (1)
#define MICROPY_PY_UJSON                         (1)

// TODO this is old BLE stuff
#if BLUETOOTH_SD
    #define MICROPY_PY_BLEIO                     (1)
    #define MICROPY_PY_BLE_NUS                   (0)
#else
    #ifndef MICROPY_PY_BLEIO
        #define MICROPY_PY_BLEIO                 (0)
    #endif
#endif

// 24kiB stack
#define CIRCUITPY_DEFAULT_STACK_SIZE            0x6000

#include "py/circuitpy_mpconfig.h"

#define MICROPY_PORT_ROOT_POINTERS \
    CIRCUITPY_COMMON_ROOT_POINTERS \
    ble_drv_evt_handler_entry_t* ble_drv_evt_handler_entries; \

#endif  // NRF5_MPCONFIGPORT_H__
