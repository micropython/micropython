// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 flom84
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "peripherals/pins.h"
#include "peripherals/periph.h"

// I2C
I2C_TypeDef *mcu_i2c_banks[3] = {I2C1, I2C2, I2C3};

const mcu_periph_obj_t mcu_i2c_sda_list[3] = {
    PERIPH(1, 4, &pin_PB07),
    PERIPH(2, 4, &pin_PB03),
    PERIPH(3, 4, &pin_PB04),
};

const mcu_periph_obj_t mcu_i2c_scl_list[3] = {
    PERIPH(1, 4, &pin_PB06),
    PERIPH(2, 4, &pin_PB10),
    PERIPH(3, 4, &pin_PA08),
};

// SPI
SPI_TypeDef *mcu_spi_banks[3] = {SPI1, SPI2, SPI3};

const mcu_periph_obj_t mcu_spi_sck_list[3] = {
    PERIPH(1, 5, &pin_PA05),
    PERIPH(2, 5, &pin_PB13),
    PERIPH(3, 6, &pin_PB03),
};

const mcu_periph_obj_t mcu_spi_mosi_list[3] = {
    PERIPH(1, 5, &pin_PA07),
    PERIPH(2, 5, &pin_PB15),
    PERIPH(3, 6, &pin_PB05),

};

const mcu_periph_obj_t mcu_spi_miso_list[3] = {
    PERIPH(1, 5, &pin_PA06),
    PERIPH(2, 5, &pin_PB14),
    PERIPH(3, 6, &pin_PB04),
};

const mcu_periph_obj_t mcu_spi_nss_list[3] = {
    PERIPH(1, 5, &pin_PA04),
    PERIPH(2, 5, &pin_PB12),
    PERIPH(3, 6, &pin_PA15),
};

USART_TypeDef *mcu_uart_banks[MAX_UART] = {USART1, USART2, USART3, NULL, NULL, NULL};
bool mcu_uart_has_usart[MAX_UART] = {false, false, false, true, true, false};

const mcu_periph_obj_t mcu_uart_tx_list[3] = {
    PERIPH(1, 7, &pin_PB06),
    PERIPH(2, 7, &pin_PA02),
    PERIPH(3, 7, &pin_PC10),

};

const mcu_periph_obj_t mcu_uart_rx_list[3] = {
    PERIPH(1, 7, &pin_PB07),
    PERIPH(2, 7, &pin_PA03),
    PERIPH(3, 7, &pin_PC11),
};

// Timers
TIM_TypeDef *mcu_tim_banks[14] = {TIM1, TIM2, TIM3, TIM4, TIM5, NULL, NULL, NULL, TIM9, TIM10,
                                  TIM11, NULL, NULL, NULL};

const mcu_tim_pin_obj_t mcu_tim_pin_list[34] = {
    TIM(2, 1, 1, &pin_PA00),
    TIM(5, 2, 1, &pin_PA00),
    TIM(2, 1, 2, &pin_PA01),
    TIM(5, 2, 2, &pin_PA01),
    TIM(2, 1, 3, &pin_PA02),
    TIM(5, 2, 3, &pin_PA02),
    TIM(2, 1, 4, &pin_PA03),
    TIM(5, 2, 4, &pin_PA03),
    TIM(9, 3, 1, &pin_PA02),
    TIM(9, 3, 2, &pin_PA03),
    TIM(3, 2, 1, &pin_PA06),
    TIM(3, 2, 2, &pin_PA07),
    TIM(1, 1, 1, &pin_PA08),
    TIM(1, 1, 2, &pin_PA09),
    TIM(1, 1, 3, &pin_PA10),
    TIM(1, 1, 4, &pin_PA11),
    TIM(2, 1, 1, &pin_PA15),
    TIM(3, 2, 3, &pin_PB00),
    TIM(3, 2, 4, &pin_PB01),
    TIM(2, 1, 2, &pin_PB03),
    TIM(3, 2, 1, &pin_PB04),
    TIM(3, 2, 2, &pin_PB05),
    TIM(4, 2, 1, &pin_PB06),
    TIM(4, 2, 2, &pin_PB07),
    TIM(4, 2, 3, &pin_PB08),
    TIM(10, 2, 1, &pin_PB08),
    TIM(4, 2, 4, &pin_PB09),
    TIM(11, 2, 1, &pin_PB09),
    TIM(2, 1, 3, &pin_PB10),
    TIM(3, 2, 1, &pin_PC06),
    TIM(3, 2, 2, &pin_PC07),
    TIM(3, 2, 3, &pin_PC08),
    TIM(3, 2, 4, &pin_PC09),
};
