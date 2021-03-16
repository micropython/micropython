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

#ifndef MICROPY_INCLUDED_STM32_PERIPHERALS_STM32F407XX_PERIPH_H
#define MICROPY_INCLUDED_STM32_PERIPHERALS_STM32F407XX_PERIPH_H

// I2C
#define I2C_BANK_ARRAY_LEN 3
#define I2C_SDA_ARRAY_LEN 7
#define I2C_SCL_ARRAY_LEN 7
extern I2C_TypeDef *mcu_i2c_banks[I2C_BANK_ARRAY_LEN];
extern const mcu_periph_obj_t mcu_i2c_sda_list[I2C_SDA_ARRAY_LEN];
extern const mcu_periph_obj_t mcu_i2c_scl_list[I2C_SCL_ARRAY_LEN];

// SPI
#define SPI_BANK_ARRAY_LEN 3
#define SPI_SCK_ARRAY_LEN 7
#define SPI_MOSI_ARRAY_LEN 7
#define SPI_MISO_ARRAY_LEN 7
#define SPI_NSS_ARRAY_LEN 6
extern SPI_TypeDef *mcu_spi_banks[SPI_BANK_ARRAY_LEN];
extern const mcu_periph_obj_t mcu_spi_sck_list[SPI_SCK_ARRAY_LEN];
extern const mcu_periph_obj_t mcu_spi_mosi_list[SPI_MOSI_ARRAY_LEN];
extern const mcu_periph_obj_t mcu_spi_miso_list[SPI_MISO_ARRAY_LEN];
extern const mcu_periph_obj_t mcu_spi_nss_list[SPI_NSS_ARRAY_LEN];

// UART
#define UART_TX_ARRAY_LEN 12
#define UART_RX_ARRAY_LEN 12
extern USART_TypeDef *mcu_uart_banks[MAX_UART];
extern bool mcu_uart_has_usart[MAX_UART];
extern const mcu_periph_obj_t mcu_uart_tx_list[UART_TX_ARRAY_LEN];
extern const mcu_periph_obj_t mcu_uart_rx_list[UART_RX_ARRAY_LEN];

// Timers
#define TIM_BANK_ARRAY_LEN 14
#define TIM_PIN_ARRAY_LEN 67
extern TIM_TypeDef *mcu_tim_banks[TIM_BANK_ARRAY_LEN];
extern const mcu_tim_pin_obj_t mcu_tim_pin_list[TIM_PIN_ARRAY_LEN];

// SDIO
extern SDIO_TypeDef *mcu_sdio_banks[1];

extern const mcu_periph_obj_t mcu_sdio_clock_list[1];
extern const mcu_periph_obj_t mcu_sdio_command_list[1];
extern const mcu_periph_obj_t mcu_sdio_data0_list[1];
extern const mcu_periph_obj_t mcu_sdio_data1_list[1];
extern const mcu_periph_obj_t mcu_sdio_data2_list[1];
extern const mcu_periph_obj_t mcu_sdio_data3_list[1];

// CAN
extern CAN_TypeDef *mcu_can_banks[2];

extern const mcu_periph_obj_t mcu_can_tx_list[6];
extern const mcu_periph_obj_t mcu_can_rx_list[6];


#endif // MICROPY_INCLUDED_STM32_PERIPHERALS_STM32F407XX_PERIPH_H
