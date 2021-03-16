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

#ifndef MICROPY_INCLUDED_STM32_PERIPHERALS_STM32F411XE_PERIPH_H
#define MICROPY_INCLUDED_STM32_PERIPHERALS_STM32F411XE_PERIPH_H

// I2C
extern I2C_TypeDef *mcu_i2c_banks[3];

extern const mcu_periph_obj_t mcu_i2c_sda_list[7];
extern const mcu_periph_obj_t mcu_i2c_scl_list[4];

// SPI
extern SPI_TypeDef *mcu_spi_banks[5];

extern const mcu_periph_obj_t mcu_spi_sck_list[15];
extern const mcu_periph_obj_t mcu_spi_mosi_list[14];
extern const mcu_periph_obj_t mcu_spi_miso_list[12];
extern const mcu_periph_obj_t mcu_spi_nss_list[12];

// UART
extern USART_TypeDef *mcu_uart_banks[MAX_UART];
extern bool mcu_uart_has_usart[MAX_UART];

extern const mcu_periph_obj_t mcu_uart_tx_list[7];
extern const mcu_periph_obj_t mcu_uart_rx_list[7];

// Timers
#define TIM_BANK_ARRAY_LEN 14
#define TIM_PIN_ARRAY_LEN 44
extern TIM_TypeDef *mcu_tim_banks[TIM_BANK_ARRAY_LEN];
extern const mcu_tim_pin_obj_t mcu_tim_pin_list[TIM_PIN_ARRAY_LEN];

#endif // MICROPY_INCLUDED_STM32_PERIPHERALS_STM32F411XE_PERIPH_H
