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
#include "peripherals/pins.h"
#include "peripherals/periph.h"

// I2C

I2C_TypeDef *mcu_i2c_banks[I2C_BANK_ARRAY_LEN] = {I2C1, I2C2, I2C3};

const mcu_periph_obj_t mcu_i2c_sda_list[I2C_SDA_ARRAY_LEN] = {
    PERIPH(1, 4, &pin_PB07),
    PERIPH(1, 4, &pin_PB09),
    PERIPH(2, 9, &pin_PB03),
    PERIPH(2, 9, &pin_PB09),
    PERIPH(3, 9, &pin_PB04),
    PERIPH(3, 9, &pin_PB08)
};

const mcu_periph_obj_t mcu_i2c_scl_list[I2C_SCL_ARRAY_LEN] = {
    PERIPH(1, 4, &pin_PB06),
    PERIPH(1, 4, &pin_PB08),
    PERIPH(2, 4, &pin_PB10),
    PERIPH(3, 4, &pin_PA08)
};

// SPI

SPI_TypeDef *mcu_spi_banks[SPI_BANK_ARRAY_LEN] = {SPI1, SPI2, SPI3, SPI4, SPI5};

const mcu_periph_obj_t mcu_spi_sck_list[SPI_SCK_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA05),
    PERIPH(1, 5, &pin_PB03),
    PERIPH(2, 5, &pin_PB10),
    PERIPH(2, 5, &pin_PB13),
    PERIPH(3, 6, &pin_PB03),
    PERIPH(3, 7, &pin_PB12),
    PERIPH(4, 6, &pin_PB13),
    PERIPH(5, 6, &pin_PB00)
};

const mcu_periph_obj_t mcu_spi_mosi_list[SPI_MOSI_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA07),
    PERIPH(1, 5, &pin_PB05),
    PERIPH(2, 5, &pin_PB15),
    PERIPH(3, 6, &pin_PB05),
    PERIPH(4, 5, &pin_PA01),
    PERIPH(5, 6, &pin_PA10),
    PERIPH(5, 6, &pin_PB08)
};

const mcu_periph_obj_t mcu_spi_miso_list[SPI_MISO_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA06),
    PERIPH(1, 5, &pin_PB04),
    PERIPH(2, 5, &pin_PB14),
    PERIPH(3, 6, &pin_PB04),
    PERIPH(4, 6, &pin_PA11),
    PERIPH(5, 6, &pin_PA12)
};

const mcu_periph_obj_t mcu_spi_nss_list[SPI_NSS_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA04),
    PERIPH(1, 5, &pin_PA15),
    PERIPH(2, 5, &pin_PB09),
    PERIPH(2, 5, &pin_PB12),
    PERIPH(3, 6, &pin_PA04),
    PERIPH(3, 6, &pin_PA15),
    PERIPH(4, 6, &pin_PB12),
    PERIPH(5, 6, &pin_PB01)
};

// UART

USART_TypeDef *mcu_uart_banks[MAX_UART] = {USART1, USART2, USART3, NULL, NULL, USART6};
bool mcu_uart_has_usart[MAX_UART] = {true, true, true, false, false, true};

const mcu_periph_obj_t mcu_uart_tx_list[UART_TX_ARRAY_LEN] = {
    PERIPH(1, 7, &pin_PA09),
    PERIPH(1, 7, &pin_PA15),
    PERIPH(1, 7, &pin_PB06),
    PERIPH(2, 7, &pin_PA02),
    PERIPH(3, 7, &pin_PB10),
    PERIPH(6, 8, &pin_PA11)
};

const mcu_periph_obj_t mcu_uart_rx_list[UART_RX_ARRAY_LEN] = {
    PERIPH(1, 7, &pin_PA10),
    PERIPH(1, 7, &pin_PB03),
    PERIPH(1, 7, &pin_PB07),
    PERIPH(2, 7, &pin_PA03),
    PERIPH(6, 8, &pin_PA12)
};

// Timers
// TIM6 and TIM7 are basic timers that are only used by DAC, and don't have pins
TIM_TypeDef *mcu_tim_banks[TIM_BANK_ARRAY_LEN] = {TIM1, TIM2, TIM3, TIM4, TIM5, NULL, NULL, TIM8, TIM9, TIM10,
                                                  TIM11, TIM12, TIM13, TIM14};

// TIM8
const mcu_tim_pin_obj_t mcu_tim_pin_list[TIM_PIN_ARRAY_LEN] = {
    TIM(1, 1, 1, &pin_PA08),
    TIM(1, 1, 2, &pin_PA09),
    TIM(1, 1, 3, &pin_PA10),
    TIM(1, 1, 4, &pin_PA11),
    TIM(2, 1, 1, &pin_PA00),
    TIM(2, 1, 1, &pin_PA05),
    TIM(2, 1, 1, &pin_PA15),
    TIM(2, 1, 2, &pin_PA01),
    TIM(2, 1, 2, &pin_PB03),
    TIM(2, 1, 3, &pin_PA02),
    TIM(2, 1, 3, &pin_PB10),
    TIM(2, 1, 4, &pin_PA03),
    TIM(3, 2, 1, &pin_PA06),
    TIM(3, 2, 1, &pin_PB04),
    TIM(3, 2, 2, &pin_PA07),
    TIM(3, 2, 2, &pin_PB05),
    TIM(3, 2, 3, &pin_PB00),
    TIM(3, 2, 4, &pin_PB01),
    TIM(4, 2, 1, &pin_PB06),
    TIM(4, 2, 2, &pin_PB07),
    TIM(4, 2, 3, &pin_PB08),
    TIM(4, 2, 4, &pin_PB09),
    TIM(5, 2, 1, &pin_PA00),
    TIM(5, 2, 2, &pin_PA01),
    TIM(5, 2, 3, &pin_PA02),
    TIM(5, 2, 4, &pin_PA03),
    TIM(9, 3, 1, &pin_PA02),
    TIM(9, 3, 2, &pin_PA03),
    TIM(10, 3, 1, &pin_PB08),
    TIM(11, 3, 1, &pin_PB09),
    TIM(12, 9, 1, &pin_PB14),
    TIM(12, 9, 2, &pin_PB15),
    TIM(13, 9, 1, &pin_PA06),
    TIM(14, 9, 1, &pin_PA07)
};
