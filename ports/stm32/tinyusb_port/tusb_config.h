/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Andrew Leech
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
#ifndef MICROPY_INCLUDED_STM32_TINYUSB_PORT_TUSB_CONFIG_H
#define MICROPY_INCLUDED_STM32_TINYUSB_PORT_TUSB_CONFIG_H

#include "py/mpconfig.h"

// STM32F4/F7/H7 boards with USB_HS use OTG_HS on RHPORT1, not RHPORT0.
// Disable RHPORT0 and put RHPORT1 in device mode (HS, or FS when the
// HS controller uses the internal FS PHY via MICROPY_HW_USB_HS_IN_FS).
// Other configs are handled either by the board (e.g. N6 sets RHPORT0
// to HS in mpconfigboard_common.h) or by the shared default in
// shared/tinyusb/tusb_config.h (RHPORT0 in FS device mode).

// These families place OTG_HS on RHPORT1. Extend the list if a new family
// also uses OTG_HS on RHPORT1 rather than RHPORT0.
#if MICROPY_HW_USB_HS && (defined(STM32F4) || defined(STM32F7) || defined(STM32H7))
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_NONE)
#if MICROPY_HW_USB_HS_IN_FS
#define CFG_TUSB_RHPORT1_MODE (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
#else
#define CFG_TUSB_RHPORT1_MODE (OPT_MODE_DEVICE | OPT_MODE_HIGH_SPEED)
#endif
#endif

#include "shared/tinyusb/tusb_config.h"

#endif // MICROPY_INCLUDED_STM32_TINYUSB_PORT_TUSB_CONFIG_H
