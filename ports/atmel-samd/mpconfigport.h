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

#ifndef __INCLUDED_MPCONFIGPORT_H
#define __INCLUDED_MPCONFIGPORT_H

// Definitions for which SAMD chip we're using.
#include "include/sam.h"

#ifdef SAMD21
#define CIRCUITPY_MCU_FAMILY                        samd21
#define MICROPY_PY_SYS_PLATFORM                     "Atmel SAMD21"
#define SPI_FLASH_MAX_BAUDRATE 8000000
#define CIRCUITPY_DEFAULT_STACK_SIZE                4096
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED          (0)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT          (0)
#define MICROPY_PY_FUNCTION_ATTRS                   (0)
// MICROPY_PY_UJSON depends on MICROPY_PY_IO
#define MICROPY_PY_IO                               (0)
#define MICROPY_PY_UJSON                            (0)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS          (0)
#define MICROPY_PY_UERRNO_LIST \
    X(EPERM) \
    X(ENOENT) \
    X(EIO) \
    X(EAGAIN) \
    X(ENOMEM) \
    X(EACCES) \
    X(EEXIST) \
    X(ENODEV) \
    X(EISDIR) \
    X(EINVAL) \

#endif

#ifdef SAMD51
#define CIRCUITPY_MCU_FAMILY                        samd51
#define MICROPY_PY_SYS_PLATFORM                     "MicroChip SAMD51"
#define SPI_FLASH_MAX_BAUDRATE 24000000
// 24kiB stack
#define CIRCUITPY_DEFAULT_STACK_SIZE                0x6000
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED          (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT          (1)
#define MICROPY_PY_FUNCTION_ATTRS                   (1)
#define MICROPY_PY_IO                               (1)
#define MICROPY_PY_UJSON                            (1)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS          (1)
//      MICROPY_PY_UERRNO_LIST - Use the default
#endif

// Turning off audioio, audiobusio, and touchio as necessary
// due to limitations of chips is handled in mpconfigboard.mk

#include "peripherals/samd/dma.h"

#include "py/circuitpy_mpconfig.h"

#define MICROPY_PORT_ROOT_POINTERS \
    CIRCUITPY_COMMON_ROOT_POINTERS \
    mp_obj_t playing_audio[AUDIO_DMA_CHANNEL_COUNT];

#endif  // __INCLUDED_MPCONFIGPORT_H
