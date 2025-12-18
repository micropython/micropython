
/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2025 Damien P. George
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
#ifndef MICROPY_INCLUDED_ESP32_MACHINE_I2C_H
#define MICROPY_INCLUDED_ESP32_MACHINE_I2C_H

// Configure default I2C0 pins.
#ifndef MICROPY_HW_I2C0_SCL
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
#define MICROPY_HW_I2C0_SCL (GPIO_NUM_9)
#define MICROPY_HW_I2C0_SDA (GPIO_NUM_8)
#else
#define MICROPY_HW_I2C0_SCL (GPIO_NUM_18)
#define MICROPY_HW_I2C0_SDA (GPIO_NUM_19)
#endif
#endif

// Configure default I2C1 pins.
#ifndef MICROPY_HW_I2C1_SCL
#if CONFIG_IDF_TARGET_ESP32
#define MICROPY_HW_I2C1_SCL (GPIO_NUM_25)
#define MICROPY_HW_I2C1_SDA (GPIO_NUM_26)
#else
#define MICROPY_HW_I2C1_SCL (GPIO_NUM_9)
#define MICROPY_HW_I2C1_SDA (GPIO_NUM_8)
#endif
#endif

#endif // MICROPY_INCLUDED_ESP32_MACHINE_I2C_H
