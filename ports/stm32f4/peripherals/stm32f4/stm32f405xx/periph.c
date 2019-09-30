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

SPI_TypeDef * mcu_spi_banks[3] = {SPI1, SPI2, SPI3};

const mcu_spi_sck_obj_t mcu_spi_sck_list[7] = {
    SPI(1, 5, &pin_PA05),
    SPI(1, 5, &pin_PB03),
    SPI(2, 5, &pin_PB10),
    SPI(2, 5, &pin_PB13),
    SPI(2, 5, &pin_PC07),
    SPI(3, 6, &pin_PB03),
    SPI(3, 6, &pin_PC10),
};

const mcu_spi_mosi_obj_t mcu_spi_mosi_list[6] = {
    SPI(1, 5, &pin_PA07),
    SPI(1, 5, &pin_PB05),
    SPI(2, 5, &pin_PB15),
    SPI(2, 5, &pin_PC03),
    SPI(3, 6, &pin_PB05),
    SPI(3, 6, &pin_PC12),
};

const mcu_spi_miso_obj_t mcu_spi_miso_list[6] = {
    SPI(1, 5, &pin_PA06),
    SPI(1, 5, &pin_PB04),
    SPI(2, 5, &pin_PB14),
    SPI(2, 5, &pin_PC02),
    SPI(3, 6, &pin_PB04),
    SPI(3, 6, &pin_PC11),
};

const mcu_spi_nss_obj_t mcu_spi_nss_list[6] = {
    SPI(1, 5, &pin_PA04),
    SPI(1, 5, &pin_PA15),
    SPI(2, 5, &pin_PB09),
    SPI(2, 5, &pin_PB12),
    SPI(3, 6, &pin_PA04),
    SPI(3, 6, &pin_PA15),
};
//UART, Etc
