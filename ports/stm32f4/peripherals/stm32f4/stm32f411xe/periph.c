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

// I2C

I2C_TypeDef * mcu_i2c_banks[3] = {I2C1, I2C2, I2C3};

//SDA Pins
const mcu_i2c_sda_obj_t per_SDA_1B07 = I2C_SDA(1, 4, &pin_PB07);
const mcu_i2c_sda_obj_t per_SDA_1B09 = I2C_SDA(1, 4, &pin_PB09);
// const mcu_i2c_sda_obj_t per_SDA_2B11 = I2C_SDA(2, 4, &pin_PB11); //not on LQFP100
const mcu_i2c_sda_obj_t per_SDA_2B09 = I2C_SDA(2, 9, &pin_PB09); 
const mcu_i2c_sda_obj_t per_SDA_2B03 = I2C_SDA(2, 9, &pin_PB03);
const mcu_i2c_sda_obj_t per_SDA_3C09 = I2C_SDA(3, 4, &pin_PC09);
const mcu_i2c_sda_obj_t per_SDA_3B04 = I2C_SDA(3, 9, &pin_PB04);
const mcu_i2c_sda_obj_t per_SDA_3B08 = I2C_SDA(3, 9, &pin_PB08);

//SCL Pins
const mcu_i2c_scl_obj_t per_SCL_1B06 = I2C_SCL(1, 4, &pin_PB06);
const mcu_i2c_scl_obj_t per_SCL_1B08 = I2C_SCL(1, 4, &pin_PB08);
const mcu_i2c_scl_obj_t per_SCL_2B10 = I2C_SCL(2, 4, &pin_PB10);
const mcu_i2c_scl_obj_t per_SCL_3B08 = I2C_SCL(3, 4, &pin_PA08);

const mcu_i2c_sda_obj_t* mcu_i2c_sda_list[7] = {
	&per_SDA_1B07,
	&per_SDA_1B09,
	&per_SDA_2B09,
	&per_SDA_2B03,
	&per_SDA_3C09,
	&per_SDA_3B04,
	&per_SDA_3B08
};

const mcu_i2c_scl_obj_t* mcu_i2c_scl_list[4] = {
	&per_SCL_1B06,
	&per_SCL_1B08,
	&per_SCL_2B10,
	&per_SCL_3B08
};

//SPI, UART, Etc
