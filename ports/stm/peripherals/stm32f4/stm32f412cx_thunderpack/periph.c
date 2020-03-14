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

I2C_TypeDef * mcu_i2c_banks[3] = {I2C1, NULL, NULL};

const mcu_i2c_sda_obj_t mcu_i2c_sda_list[8] = {
    I2C_SDA(1, 4, &pin_PB07),
};

const mcu_i2c_scl_obj_t mcu_i2c_scl_list[4] = {
    I2C_SCL(1, 4, &pin_PB06),
};

// SPI

SPI_TypeDef * mcu_spi_banks[5] = {SPI1, NULL, NULL, NULL, NULL};

const mcu_spi_sck_obj_t mcu_spi_sck_list[15] = {
    SPI(1, 5, &pin_PA05),
};

const mcu_spi_mosi_obj_t mcu_spi_mosi_list[14] = {
    SPI(1, 5, &pin_PA07),
    SPI(1, 5, &pin_PB05),
};

const mcu_spi_miso_obj_t mcu_spi_miso_list[12] = {
    SPI(1, 5, &pin_PA06),
};

const mcu_spi_nss_obj_t mcu_spi_nss_list[12] = {
    SPI(1, 5, &pin_PA04),
};

//UART

USART_TypeDef * mcu_uart_banks[MAX_UART] = {USART1, NULL, NULL, NULL, NULL, NULL};
bool mcu_uart_has_usart[MAX_UART] = {true, false, false, false, false, false};

const mcu_uart_tx_obj_t mcu_uart_tx_list[11] = {
    UART(1, 7, &pin_PA09),
    UART(1, 7, &pin_PB06),
};

const mcu_uart_rx_obj_t mcu_uart_rx_list[12] = {
    UART(1, 7, &pin_PA10),
    UART(1, 7, &pin_PB07),
};

//Timers
//TIM6 and TIM7 are basic timers that are only used by DAC, and don't have pins
TIM_TypeDef * mcu_tim_banks[14] = {TIM1, TIM2, TIM3, TIM4, TIM5, NULL, NULL, NULL, TIM9, NULL,
                                    NULL, NULL, TIM13, TIM14};

const mcu_tim_pin_obj_t mcu_tim_pin_list[60] = {
    TIM(2,1,1,&pin_PA00),
    TIM(5,2,1,&pin_PA00),
    TIM(2,1,2,&pin_PA01),
    TIM(5,2,2,&pin_PA01),
    TIM(2,1,3,&pin_PA02),
    TIM(5,2,3,&pin_PA02),
    TIM(2,1,4,&pin_PA03),
    TIM(5,2,4,&pin_PA03),
    TIM(9,3,1,&pin_PA02),
    TIM(9,3,2,&pin_PA03),
    TIM(3,2,1,&pin_PA06),
    TIM(13,9,1,&pin_PA06),
    TIM(3,2,2,&pin_PA07),
    TIM(14,9,1,&pin_PA07),
    TIM(1,1,1,&pin_PA08),
    TIM(1,1,2,&pin_PA09),
    TIM(1,1,3,&pin_PA10),
    TIM(1,1,4,&pin_PA11),
    TIM(3,2,2,&pin_PB05),
    TIM(4,2,1,&pin_PB06),
    TIM(4,2,2,&pin_PB07),
};
