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

I2C_TypeDef *mcu_i2c_banks[I2C_BANK_ARRAY_LEN] = {I2C1, I2C2, I2C3};

const mcu_periph_obj_t mcu_i2c_sda_list[I2C_SDA_ARRAY_LEN] = {
    PERIPH(1, 4, &pin_PB07),
    PERIPH(1, 4, &pin_PB09),
    PERIPH(2, 4, &pin_PB11),
    PERIPH(3, 4, &pin_PC09),
    PERIPH(2, 4, &pin_PF00),
    PERIPH(2, 4, &pin_PH05),
    PERIPH(3, 4, &pin_PH08),
};
const mcu_periph_obj_t mcu_i2c_scl_list[I2C_SCL_ARRAY_LEN] = {
    PERIPH(3, 4, &pin_PA08),
    PERIPH(1, 4, &pin_PB06),
    PERIPH(1, 4, &pin_PB08),
    PERIPH(2, 4, &pin_PB10),
    PERIPH(2, 4, &pin_PF01),
    PERIPH(2, 4, &pin_PH04),
    PERIPH(3, 4, &pin_PH07),
};

SPI_TypeDef *mcu_spi_banks[SPI_BANK_ARRAY_LEN] = {SPI1, SPI2, SPI3};

const mcu_periph_obj_t mcu_spi_sck_list[SPI_SCK_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA05),
    PERIPH(1, 5, &pin_PB03),
    PERIPH(3, 6, &pin_PB03),
    PERIPH(2, 5, &pin_PB10),
    PERIPH(2, 5, &pin_PB13),
    PERIPH(3, 6, &pin_PC10),
    PERIPH(2, 5, &pin_PI01),
};
const mcu_periph_obj_t mcu_spi_mosi_list[SPI_MOSI_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA07),
    PERIPH(1, 5, &pin_PB05),
    PERIPH(3, 6, &pin_PB05),
    PERIPH(2, 5, &pin_PB15),
    PERIPH(2, 5, &pin_PC03),
    PERIPH(3, 6, &pin_PC12),
    PERIPH(2, 5, &pin_PI03),
};
const mcu_periph_obj_t mcu_spi_miso_list[SPI_MISO_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA06),
    PERIPH(1, 5, &pin_PB04),
    PERIPH(3, 6, &pin_PB04),
    PERIPH(2, 5, &pin_PB14),
    PERIPH(2, 5, &pin_PC02),
    PERIPH(3, 6, &pin_PC11),
    PERIPH(2, 5, &pin_PI02),
};
const mcu_periph_obj_t mcu_spi_nss_list[SPI_NSS_ARRAY_LEN] = {
    PERIPH(1, 5, &pin_PA04),
    PERIPH(1, 5, &pin_PA15),
    PERIPH(2, 5, &pin_PB09),
    PERIPH(2, 5, &pin_PB12),
    PERIPH(3, 6, &pin_PA04),
    PERIPH(3, 6, &pin_PA15),
};

USART_TypeDef *mcu_uart_banks[MAX_UART] = {USART1, USART2, USART3, UART4, UART5, USART6};
bool mcu_uart_has_usart[MAX_UART] = {true, true, true, false, false, true};

const mcu_periph_obj_t mcu_uart_tx_list[UART_TX_ARRAY_LEN] = {
    PERIPH(4, 8, &pin_PA00),
    PERIPH(2, 7, &pin_PA02),
    PERIPH(1, 7, &pin_PA09),
    PERIPH(1, 7, &pin_PB06),
    PERIPH(3, 7, &pin_PB10),
    PERIPH(6, 8, &pin_PC06),
    PERIPH(3, 7, &pin_PC10),
    PERIPH(4, 8, &pin_PC10),
    PERIPH(5, 8, &pin_PC12),
    PERIPH(2, 7, &pin_PD05),
    PERIPH(3, 7, &pin_PD08),
    PERIPH(6, 8, &pin_PG14),
};
const mcu_periph_obj_t mcu_uart_rx_list[UART_RX_ARRAY_LEN] = {
    PERIPH(4, 8, &pin_PA01),
    PERIPH(2, 7, &pin_PA03),
    PERIPH(1, 7, &pin_PA10),
    PERIPH(1, 7, &pin_PB07),
    PERIPH(3, 7, &pin_PB11),
    PERIPH(6, 8, &pin_PC07),
    PERIPH(3, 7, &pin_PC11),
    PERIPH(4, 8, &pin_PC11),
    PERIPH(5, 8, &pin_PD02),
    PERIPH(2, 7, &pin_PD06),
    PERIPH(3, 7, &pin_PD09),
    PERIPH(6, 8, &pin_PG09),
};

// Timers
// TIM6 and TIM7 are basic timers that are only used by DAC, and don't have pins
TIM_TypeDef *mcu_tim_banks[TIM_BANK_ARRAY_LEN] = {TIM1, TIM2, TIM3, TIM4, TIM5, NULL, NULL, TIM8, TIM9, TIM10,
                                                  TIM11, TIM12, TIM13, TIM14};

const mcu_tim_pin_obj_t mcu_tim_pin_list[TIM_PIN_ARRAY_LEN] = {
    TIM(2, 1, 1, &pin_PA00),
    TIM(5, 2, 1, &pin_PA00),
    TIM(2, 1, 2, &pin_PA01),
    TIM(5, 2, 2, &pin_PA01),
    TIM(2, 1, 3, &pin_PA02),
    TIM(5, 2, 3, &pin_PA02),
    TIM(9, 3, 1, &pin_PA02),
    TIM(2, 1, 4, &pin_PA03),
    TIM(5, 2, 4, &pin_PA03),
    TIM(9, 3, 2, &pin_PA03),
    TIM(2, 1, 1, &pin_PA05),
    TIM(3, 2, 1, &pin_PA06),
    TIM(13, 9, 1, &pin_PA06),
    TIM(3, 2, 2, &pin_PA07),
    TIM(14, 9, 1, &pin_PA07),
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
    TIM(10, 3, 1, &pin_PB08),
    TIM(4, 2, 4, &pin_PB09),
    TIM(11, 3, 1, &pin_PB09),
    TIM(2, 1, 3, &pin_PB10),
    TIM(2, 1, 4, &pin_PB11),
    TIM(12, 9, 1, &pin_PB14),
    TIM(12, 9, 2, &pin_PB15),
    TIM(3, 2, 1, &pin_PC06),
    TIM(8, 3, 1, &pin_PC06),
    TIM(3, 2, 2, &pin_PC07),
    TIM(8, 3, 2, &pin_PC07),
    TIM(3, 2, 3, &pin_PC08),
    TIM(8, 3, 3, &pin_PC08),
    TIM(3, 2, 4, &pin_PC09),
    TIM(8, 3, 4, &pin_PC09),
    TIM(4, 2, 1, &pin_PD12),
    TIM(4, 2, 2, &pin_PD13),
    TIM(4, 2, 3, &pin_PD14),
    TIM(4, 2, 4, &pin_PD15),
    TIM(9, 3, 1, &pin_PE05),
    TIM(9, 3, 2, &pin_PE06),
    TIM(1, 1, 1, &pin_PE09),
    TIM(1, 1, 2, &pin_PE11),
    TIM(1, 1, 3, &pin_PE13),
    TIM(1, 1, 4, &pin_PE14),
    TIM(10, 3, 1, &pin_PF06),
    TIM(11, 3, 1, &pin_PF07),
    TIM(13, 9, 1, &pin_PF08),
    TIM(14, 9, 1, &pin_PF09),
    TIM(12, 9, 1, &pin_PH06),
    TIM(12, 9, 2, &pin_PH09),
    TIM(5, 2, 1, &pin_PH10),
    TIM(5, 2, 2, &pin_PH11),
    TIM(5, 2, 3, &pin_PH12),
    TIM(5, 2, 4, &pin_PI00),
    TIM(8, 3, 4, &pin_PI02),
    TIM(8, 3, 1, &pin_PI05),
    TIM(8, 3, 2, &pin_PI06),
    TIM(8, 3, 3, &pin_PI07),
};

// SDIO
SDIO_TypeDef *mcu_sdio_banks[1] = {SDIO};

const mcu_periph_obj_t mcu_sdio_clock_list[1] = {
    PERIPH(1, 12, &pin_PC12),
};
const mcu_periph_obj_t mcu_sdio_command_list[1] = {
    PERIPH(1, 12, &pin_PD02),
};
const mcu_periph_obj_t mcu_sdio_data0_list[1] = {
    PERIPH(1, 12, &pin_PC08),
};
const mcu_periph_obj_t mcu_sdio_data1_list[1] = {
    PERIPH(1, 12, &pin_PC09),
};
const mcu_periph_obj_t mcu_sdio_data2_list[1] = {
    PERIPH(1, 12, &pin_PC10),
};
const mcu_periph_obj_t mcu_sdio_data3_list[1] = {
    PERIPH(1, 12, &pin_PC11),
};

// CAN
CAN_TypeDef *mcu_can_banks[2] = {CAN1, CAN2};

const mcu_periph_obj_t mcu_can_tx_list[6] = {
    PERIPH(1, 9, &pin_PA11),
    PERIPH(1, 9, &pin_PB08),
    PERIPH(1, 9, &pin_PD00),
    PERIPH(1, 9, &pin_PI09),

    PERIPH(2, 9, &pin_PB12),
    PERIPH(2, 9, &pin_PB05),
};

const mcu_periph_obj_t mcu_can_rx_list[6] = {
    PERIPH(1, 9, &pin_PA12),
    PERIPH(1, 9, &pin_PB09),
    PERIPH(1, 9, &pin_PD01),
    PERIPH(1, 9, &pin_PH13),

    PERIPH(2, 9, &pin_PB13),
    PERIPH(2, 9, &pin_PB06),
};
