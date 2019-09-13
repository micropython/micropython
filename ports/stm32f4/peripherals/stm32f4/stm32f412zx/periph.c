 /*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include "py/obj.h"
#include "py/mphal.h"
#include "stm32f4/pins.h"
#include "stm32f4/periph.h"

//const mcu_i2c_periph_obj_t periph_I2C1_0 = I2C(1, 0, SDA(PB,7), SCL(PB,6));
//const mcu_i2c_periph_obj_t periph_I2C1_1 = I2C(1, 1, SDA(PB,9), SCL(PB,8));
//const mcu_i2c_periph_obj_t periph_I2C2 = I2C(1, NO_ALT, SDA(PB,11), SCL(PB,10));
//const mcu_i2c_periph_obj_t periph_I2C3 = I2C(1, NO_ALT, SDA(PB,9), SCL(PB,8));

//WARNING: The STM32F412 has a bunch of repeated ones on AF9 so BEWARE

const mcu_i2c_periph_obj_t periph_I2C1_0 = I2C(1, 0, &pin_PB07, &pin_PB06);
const mcu_i2c_periph_obj_t periph_I2C1_1 = I2C(1, 1, &pin_PB09, &pin_PB08);
const mcu_i2c_periph_obj_t periph_I2C2 = I2C(1, NO_ALT, &pin_PB11, &pin_PB10); //not on LQFP100
const mcu_i2c_periph_obj_t periph_I2C3 = I2C(1, NO_ALT, &pin_PB09, &pin_PB08);

const mcu_i2c_periph_obj_t* mcu_i2c_list[4] = {&periph_I2C1_0,&periph_I2C1_1,&periph_I2C3};
