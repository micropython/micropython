/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Pavel Revak
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
#ifndef MICROPY_INCLUDED_ESP32_MACHINE_SDCARD_H
#define MICROPY_INCLUDED_ESP32_MACHINE_SDCARD_H

// Default pins for the primary SPI-mode SD card bus (slot 2). SPI mode is
// available on every ESP32 variant, so a board may override these in its
// mpconfigboard.h to make machine.SDCard(slot=2) work without explicit pins.
// SDMMC-mode pins (slots 0/1) are not affected.
#ifndef MICROPY_HW_SDCARD_SPI_SCK
#if CONFIG_IDF_TARGET_ESP32
#define MICROPY_HW_SDCARD_SPI_SCK (GPIO_NUM_18)
#define MICROPY_HW_SDCARD_SPI_MOSI (GPIO_NUM_23)
#define MICROPY_HW_SDCARD_SPI_MISO (GPIO_NUM_19)
#define MICROPY_HW_SDCARD_SPI_CS (GPIO_NUM_5)
#elif CONFIG_IDF_TARGET_ESP32S3
#define MICROPY_HW_SDCARD_SPI_SCK (GPIO_NUM_37)
#define MICROPY_HW_SDCARD_SPI_MOSI (GPIO_NUM_35)
#define MICROPY_HW_SDCARD_SPI_MISO (GPIO_NUM_36)
#define MICROPY_HW_SDCARD_SPI_CS (GPIO_NUM_34)
#else
// Other chips have no historical default; a board or the runtime must assign.
#define MICROPY_HW_SDCARD_SPI_SCK (GPIO_NUM_NC)
#define MICROPY_HW_SDCARD_SPI_MOSI (GPIO_NUM_NC)
#define MICROPY_HW_SDCARD_SPI_MISO (GPIO_NUM_NC)
#define MICROPY_HW_SDCARD_SPI_CS (GPIO_NUM_NC)
#endif
#endif

#endif // MICROPY_INCLUDED_ESP32_MACHINE_SDCARD_H
