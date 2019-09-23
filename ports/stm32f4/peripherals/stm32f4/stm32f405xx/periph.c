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

// I2C

I2C_TypeDef * mcu_i2c_banks[3] = {I2C1, I2C2, I2C3};

const mcu_i2c_sda_obj_t mcu_i2c_sda_list[4] = {
	I2C_SDA(1, 4, &pin_PB07),
	I2C_SDA(1, 4, &pin_PB09),
	I2C_SDA(2, 4, &pin_PB11),
	I2C_SDA(3, 4, &pin_PC09),
};

const mcu_i2c_scl_obj_t mcu_i2c_scl_list[4] = {
	I2C_SCL(1, 4, &pin_PB06),
	I2C_SCL(1, 4, &pin_PB08),
	I2C_SCL(2, 4, &pin_PB10),
	I2C_SCL(3, 4, &pin_PA08)
};
//SPI, UART, Etc
