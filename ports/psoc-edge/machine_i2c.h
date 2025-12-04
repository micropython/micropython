
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
#ifndef MICROPY_INCLUDED_PSOCEDGE_MACHINE_I2C_H
#define MICROPY_INCLUDED_PSOCEDGE_MACHINE_I2C_H

// Configure default I2C0 pins and SCB peripheral
#ifndef MICROPY_HW_I2C0_SCL
#define MICROPY_HW_I2C0_SCB (SCB5)
#define MICROPY_HW_I2C0_SCL (P17_0_NUM)
#define MICROPY_HW_I2C0_SDA (P17_1_NUM)
#define MAX_I2C               1
#endif

// Declare the I2C type for external use
extern const mp_obj_type_t machine_i2c_type;

#endif // MICROPY_INCLUDED_PSOCEDGE_MACHINE_I2C_H
