/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

// This file defines board specific functions.

#ifndef MICROPY_INCLUDED_MIMXRT10XX_BOARDS_FLASH_CONFIG_H
#define MICROPY_INCLUDED_MIMXRT10XX_BOARDS_FLASH_CONFIG_H

#include <stdbool.h>

#include "mpconfigboard.h"  // For flash size settings

#include "fsl_common.h"
#include "fsl_flexspi_nor_config.h"

#define SEQUENCE(first, second, third, fourth) first, second, third, fourth
#define TWO_EMPTY_STEPS 0x00000000
#define EMPTY_SEQUENCE SEQUENCE(TWO_EMPTY_STEPS, TWO_EMPTY_STEPS, TWO_EMPTY_STEPS, TWO_EMPTY_STEPS)

// FlexSPI configuration that stores command info.
extern const flexspi_nor_config_t qspiflash_config;

#endif  // MICROPY_INCLUDED_MIMXRT10XX_BOARDS_FLASH_CONFIG_H
