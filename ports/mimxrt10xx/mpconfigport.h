/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

#include <stdint.h>

#define MICROPY_PY_SYS_PLATFORM                     "NXP IMXRT10XX"
#define SPI_FLASH_MAX_BAUDRATE 24000000

extern uint8_t _ld_filesystem_start;
extern uint8_t _ld_filesystem_end;
extern uint8_t _ld_default_stack_size;

// 20kiB stack
#define CIRCUITPY_DEFAULT_STACK_SIZE                ((uint32_t)&_ld_default_stack_size)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED          (0)
#define MICROPY_PY_FUNCTION_ATTRS                   (0)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS          (1)


#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR ((uint32_t)&_ld_filesystem_start)
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE ((uint32_t)(&_ld_filesystem_end - &_ld_filesystem_start))

#include "py/circuitpy_mpconfig.h"

#define MICROPY_PORT_ROOT_POINTERS \
    CIRCUITPY_COMMON_ROOT_POINTERS \

// TODO:
//    mp_obj_t playing_audio[AUDIO_DMA_CHANNEL_COUNT];

#endif  // __INCLUDED_MPCONFIGPORT_H
