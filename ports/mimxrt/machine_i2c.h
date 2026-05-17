/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
 * Copyright (c) 2025 Robert Hammelrath
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

#define DEFAULT_I2C_ID          (0)
#define DEFAULT_I2C_FREQ        (400000)
#define DEFAULT_I2C_DRIVE       (6)
#define DEFAULT_I2C_TIMEOUT     (50000)
#define DEFAULT_I2C_FILTER_NS   (200)
#define MICROPY_HW_I2C_NUM      ARRAY_SIZE(i2c_index_table)

extern const uint8_t i2c_index_table[];
extern LPI2C_Type *i2c_base_ptr_table[];
extern bool lpi2c_set_iomux(int8_t hw_i2c, uint8_t drive);
extern const uint8_t micropy_hw_i2c_num;
