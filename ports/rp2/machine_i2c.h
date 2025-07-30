/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2025 Damien P. George
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
#ifndef MICROPY_INCLUDED_RP2_MACHINE_I2C_H
#define MICROPY_INCLUDED_RP2_MACHINE_I2C_H

#ifdef MICROPY_HW_I2C_NO_DEFAULT_PINS

// With no default I2C, need to require the pin args.
#define MICROPY_HW_I2C0_SCL (0)
#define MICROPY_HW_I2C0_SDA (0)
#define MICROPY_HW_I2C1_SCL (0)
#define MICROPY_HW_I2C1_SDA (0)
#define MICROPY_I2C_PINS_ARG_OPTS MP_ARG_REQUIRED

#else

// Most boards do not require pin args.
#define MICROPY_I2C_PINS_ARG_OPTS 0

#ifndef MICROPY_HW_I2C0_SCL
#if PICO_DEFAULT_I2C == 0
#define MICROPY_HW_I2C0_SCL (PICO_DEFAULT_I2C_SCL_PIN)
#define MICROPY_HW_I2C0_SDA (PICO_DEFAULT_I2C_SDA_PIN)
#else
#define MICROPY_HW_I2C0_SCL (9)
#define MICROPY_HW_I2C0_SDA (8)
#endif
#endif

#ifndef MICROPY_HW_I2C1_SCL
#if PICO_DEFAULT_I2C == 1
#define MICROPY_HW_I2C1_SCL (PICO_DEFAULT_I2C_SCL_PIN)
#define MICROPY_HW_I2C1_SDA (PICO_DEFAULT_I2C_SDA_PIN)
#else
#define MICROPY_HW_I2C1_SCL (7)
#define MICROPY_HW_I2C1_SDA (6)
#endif
#endif
#endif

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))

#endif // MICROPY_INCLUDED_RP2_MACHINE_I2C_H
