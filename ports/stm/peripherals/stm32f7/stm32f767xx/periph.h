// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// I2C
extern I2C_TypeDef *mcu_i2c_banks[4];

extern const mcu_periph_obj_t mcu_i2c_sda_list[12];
extern const mcu_periph_obj_t mcu_i2c_scl_list[12];

// SPI
extern SPI_TypeDef *mcu_spi_banks[6];

extern const mcu_periph_obj_t mcu_spi_sck_list[18];
extern const mcu_periph_obj_t mcu_spi_mosi_list[18];
extern const mcu_periph_obj_t mcu_spi_miso_list[15];

// UART
extern USART_TypeDef *mcu_uart_banks[MAX_UART];
extern bool mcu_uart_has_usart[MAX_UART];

extern const mcu_periph_obj_t mcu_uart_tx_list[24];
extern const mcu_periph_obj_t mcu_uart_rx_list[25];

// Timers
#define TIM_BANK_ARRAY_LEN 14
#define TIM_PIN_ARRAY_LEN 55
extern TIM_TypeDef *mcu_tim_banks[TIM_BANK_ARRAY_LEN];

extern const mcu_tim_pin_obj_t mcu_tim_pin_list[TIM_PIN_ARRAY_LEN];
