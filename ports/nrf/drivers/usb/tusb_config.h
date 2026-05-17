/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_NRF_TUSB_CONFIG_H
#define MICROPY_INCLUDED_NRF_TUSB_CONFIG_H

#include "shared/tinyusb/tusb_config.h"

// Device configuration
#define CFG_TUSB_MCU                OPT_MCU_NRF5X

// TinyUSB uses newer style errata functions, but we currently don't have the
// latest nrfx component.  So provide macros mapping the new name to the old.
#include "nrfx_usbd_errata.h"
#define nrf52_errata_166 nrfx_usbd_errata_166
#define nrf52_errata_171 nrfx_usbd_errata_171
#define nrf52_errata_187 nrfx_usbd_errata_187

#endif // MICROPY_INCLUDED_NRF_TUSB_CONFIG_H
