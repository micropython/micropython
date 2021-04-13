/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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
#include "mimxrt10xx/periph.h"

LPI2C_Type *mcu_i2c_banks[2] = { LPI2C1, LPI2C2 };

const mcu_periph_obj_t mcu_i2c_sda_list[8] = {
    PERIPH_PIN(1, 0, kIOMUXC_LPI2C1_SDA_SELECT_INPUT, 0, &pin_GPIO_AD_13),
    PERIPH_PIN(1, 1, kIOMUXC_LPI2C1_SDA_SELECT_INPUT, 1, &pin_GPIO_SD_05),
    PERIPH_PIN(1, 1, kIOMUXC_LPI2C1_SDA_SELECT_INPUT, 2, &pin_GPIO_11),
    PERIPH_PIN(1, 3, kIOMUXC_LPI2C1_SDA_SELECT_INPUT, 3, &pin_GPIO_01),

    PERIPH_PIN(2, 0, kIOMUXC_LPI2C2_SDA_SELECT_INPUT, 0, &pin_GPIO_AD_07),
    PERIPH_PIN(2, 3, kIOMUXC_LPI2C2_SDA_SELECT_INPUT, 1, &pin_GPIO_AD_01),
    PERIPH_PIN(2, 1, kIOMUXC_LPI2C2_SDA_SELECT_INPUT, 2, &pin_GPIO_SD_07),
    PERIPH_PIN(2, 3, kIOMUXC_LPI2C2_SDA_SELECT_INPUT, 3, &pin_GPIO_09),
};

const mcu_periph_obj_t mcu_i2c_scl_list[8] = {
    PERIPH_PIN(1, 0, kIOMUXC_LPI2C1_SCL_SELECT_INPUT, 0, &pin_GPIO_AD_14),
    PERIPH_PIN(1, 1, kIOMUXC_LPI2C1_SCL_SELECT_INPUT, 1, &pin_GPIO_SD_06),
    PERIPH_PIN(1, 1, kIOMUXC_LPI2C1_SCL_SELECT_INPUT, 2, &pin_GPIO_12),
    PERIPH_PIN(1, 3, kIOMUXC_LPI2C1_SCL_SELECT_INPUT, 3, &pin_GPIO_02),

    PERIPH_PIN(2, 0, kIOMUXC_LPI2C2_SCL_SELECT_INPUT, 0, &pin_GPIO_AD_08),
    PERIPH_PIN(2, 3, kIOMUXC_LPI2C2_SCL_SELECT_INPUT, 1, &pin_GPIO_AD_02),
    PERIPH_PIN(2, 1, kIOMUXC_LPI2C2_SCL_SELECT_INPUT, 2, &pin_GPIO_SD_08),
    PERIPH_PIN(2, 3, kIOMUXC_LPI2C2_SCL_SELECT_INPUT, 3, &pin_GPIO_10),
};

LPSPI_Type *mcu_spi_banks[2] = { LPSPI1, LPSPI2 };

const mcu_periph_obj_t mcu_spi_sck_list[4] = {
    PERIPH_PIN(1, 0, kIOMUXC_LPSPI1_SCK_SELECT_INPUT, 0, &pin_GPIO_AD_06),
    PERIPH_PIN(1, 2, kIOMUXC_LPSPI1_SCK_SELECT_INPUT, 1, &pin_GPIO_SD_08),

    PERIPH_PIN(2, 0, kIOMUXC_LPSPI2_SCK_SELECT_INPUT, 0, &pin_GPIO_AD_12),
    PERIPH_PIN(2, 1, kIOMUXC_LPSPI2_SCK_SELECT_INPUT, 1, &pin_GPIO_SD_11),
};

const mcu_periph_obj_t mcu_spi_mosi_list[4] = {
    PERIPH_PIN(1, 0, kIOMUXC_LPSPI1_SDO_SELECT_INPUT, 0, &pin_GPIO_AD_04),
    PERIPH_PIN(1, 2, kIOMUXC_LPSPI1_SDO_SELECT_INPUT, 1, &pin_GPIO_SD_06),

    PERIPH_PIN(2, 0, kIOMUXC_LPSPI2_SDO_SELECT_INPUT, 0, &pin_GPIO_AD_10),
    PERIPH_PIN(2, 1, kIOMUXC_LPSPI2_SDO_SELECT_INPUT, 1, &pin_GPIO_SD_10),
};

const mcu_periph_obj_t mcu_spi_miso_list[4] = {
    PERIPH_PIN(1, 0, kIOMUXC_LPSPI1_SDI_SELECT_INPUT, 0, &pin_GPIO_AD_03),
    PERIPH_PIN(1, 2, kIOMUXC_LPSPI1_SDI_SELECT_INPUT, 1, &pin_GPIO_SD_05),

    PERIPH_PIN(2, 0, kIOMUXC_LPSPI2_SDI_SELECT_INPUT, 0, &pin_GPIO_AD_09),
    PERIPH_PIN(2, 1, kIOMUXC_LPSPI2_SDI_SELECT_INPUT, 1, &pin_GPIO_SD_09),
};

LPUART_Type *mcu_uart_banks[4] = { LPUART1, LPUART2, LPUART3, LPUART4 };

const mcu_periph_obj_t mcu_uart_rx_list[9] = {
    PERIPH_PIN(1, 2, kIOMUXC_LPUART1_RXD_SELECT_INPUT, 0, &pin_GPIO_SD_11),
    PERIPH_PIN(1, 0, kIOMUXC_LPUART1_RXD_SELECT_INPUT, 1, &pin_GPIO_09),

    PERIPH_PIN(2, 2, kIOMUXC_LPUART2_RXD_SELECT_INPUT, 0, &pin_GPIO_SD_09),
    PERIPH_PIN(2, 0, kIOMUXC_LPUART2_RXD_SELECT_INPUT, 1, &pin_GPIO_13),

    PERIPH_PIN(3, 1, kIOMUXC_LPUART3_RXD_SELECT_INPUT, 0, &pin_GPIO_AD_07),
    PERIPH_PIN(3, 0, kIOMUXC_LPUART3_RXD_SELECT_INPUT, 1, &pin_GPIO_11),
    PERIPH_PIN(3, 3, kIOMUXC_LPUART3_RXD_SELECT_INPUT, 2, &pin_GPIO_07),

    PERIPH_PIN(4, 0, kIOMUXC_LPUART4_RXD_SELECT_INPUT, 0, &pin_GPIO_AD_01),
    PERIPH_PIN(4, 3, kIOMUXC_LPUART4_RXD_SELECT_INPUT, 1, &pin_GPIO_05),
};

const mcu_periph_obj_t mcu_uart_tx_list[9] = {
    PERIPH_PIN(1, 2, kIOMUXC_LPUART1_TXD_SELECT_INPUT, 0, &pin_GPIO_SD_12),
    PERIPH_PIN(1, 0, kIOMUXC_LPUART1_TXD_SELECT_INPUT, 1, &pin_GPIO_10),

    PERIPH_PIN(2, 0, kIOMUXC_LPUART2_TXD_SELECT_INPUT, 0, &pin_GPIO_AD_00),
    PERIPH_PIN(2, 2, kIOMUXC_LPUART2_TXD_SELECT_INPUT, 1, &pin_GPIO_SD_10),

    PERIPH_PIN(3, 1, kIOMUXC_LPUART3_TXD_SELECT_INPUT, 0, &pin_GPIO_AD_08),
    PERIPH_PIN(3, 0, kIOMUXC_LPUART3_TXD_SELECT_INPUT, 1, &pin_GPIO_12),
    PERIPH_PIN(3, 3, kIOMUXC_LPUART3_TXD_SELECT_INPUT, 2, &pin_GPIO_08),

    PERIPH_PIN(4, 0, kIOMUXC_LPUART4_TXD_SELECT_INPUT, 0, &pin_GPIO_AD_02),
    PERIPH_PIN(4, 3, kIOMUXC_LPUART4_TXD_SELECT_INPUT, 1, &pin_GPIO_06),
};

const mcu_periph_obj_t mcu_uart_rts_list[4] = {
    PERIPH_PIN(1, 6, 0, 0, &pin_GPIO_07),

    PERIPH_PIN(2, 3, 0, 0, &pin_GPIO_AD_07),

    PERIPH_PIN(3, 1, 0, 0, &pin_GPIO_AD_13),

    PERIPH_PIN(4, 3, 0, 0, &pin_GPIO_AD_13)
};

const mcu_periph_obj_t mcu_uart_cts_list[4] = {
    PERIPH_PIN(1, 6, 0, 0, &pin_GPIO_08),

    PERIPH_PIN(2, 3, 0, 0, &pin_GPIO_AD_08),

    PERIPH_PIN(3, 1, 0, 0, &pin_GPIO_AD_14),

    PERIPH_PIN(4, 3, 0, 0, &pin_GPIO_AD_14),
};

const mcu_pwm_obj_t mcu_pwm_list[20] = {
    PWM_PIN(PWM1, kPWM_Module_0, kPWM_PwmA, IOMUXC_GPIO_02_FLEXPWM1_PWM0_A, &pin_GPIO_02),
    PWM_PIN(PWM1, kPWM_Module_0, kPWM_PwmA, IOMUXC_GPIO_SD_02_FLEXPWM1_PWM0_A, &pin_GPIO_SD_02),

    PWM_PIN(PWM1, kPWM_Module_0, kPWM_PwmB, IOMUXC_GPIO_01_FLEXPWM1_PWM0_B, &pin_GPIO_01),
    PWM_PIN(PWM1, kPWM_Module_0, kPWM_PwmB, IOMUXC_GPIO_SD_01_FLEXPWM1_PWM0_B, &pin_GPIO_SD_01),

    PWM_PIN(PWM1, kPWM_Module_0, kPWM_PwmX, IOMUXC_GPIO_AD_12_FLEXPWM1_PWM0_X, &pin_GPIO_AD_12),

    PWM_PIN(PWM1, kPWM_Module_1, kPWM_PwmA, IOMUXC_GPIO_04_FLEXPWM1_PWM1_A, &pin_GPIO_04),
    PWM_PIN(PWM1, kPWM_Module_1, kPWM_PwmA, IOMUXC_GPIO_SD_04_FLEXPWM1_PWM1_A, &pin_GPIO_SD_04),

    PWM_PIN(PWM1, kPWM_Module_1, kPWM_PwmB, IOMUXC_GPIO_03_FLEXPWM1_PWM1_B, &pin_GPIO_03),
    PWM_PIN(PWM1, kPWM_Module_1, kPWM_PwmB, IOMUXC_GPIO_SD_03_FLEXPWM1_PWM1_B, &pin_GPIO_SD_03),

    PWM_PIN(PWM1, kPWM_Module_1, kPWM_PwmX, IOMUXC_GPIO_AD_11_FLEXPWM1_PWM1_X, &pin_GPIO_AD_11),

    PWM_PIN(PWM1, kPWM_Module_2, kPWM_PwmA, IOMUXC_GPIO_06_FLEXPWM1_PWM2_A, &pin_GPIO_06),
    PWM_PIN(PWM1, kPWM_Module_2, kPWM_PwmA, IOMUXC_GPIO_AD_04_FLEXPWM1_PWM2_A, &pin_GPIO_AD_04),

    PWM_PIN(PWM1, kPWM_Module_2, kPWM_PwmB, IOMUXC_GPIO_05_FLEXPWM1_PWM2_B, &pin_GPIO_05),
    PWM_PIN(PWM1, kPWM_Module_2, kPWM_PwmB, IOMUXC_GPIO_AD_03_FLEXPWM1_PWM2_B, &pin_GPIO_AD_03),

    PWM_PIN(PWM1, kPWM_Module_2, kPWM_PwmX, IOMUXC_GPIO_AD_10_FLEXPWM1_PWM2_X, &pin_GPIO_AD_10),

    PWM_PIN(PWM1, kPWM_Module_3, kPWM_PwmA, IOMUXC_GPIO_08_FLEXPWM1_PWM3_A, &pin_GPIO_08),
    PWM_PIN(PWM1, kPWM_Module_3, kPWM_PwmA, IOMUXC_GPIO_AD_06_FLEXPWM1_PWM3_A, &pin_GPIO_AD_06),

    PWM_PIN(PWM1, kPWM_Module_3, kPWM_PwmB, IOMUXC_GPIO_07_FLEXPWM1_PWM3_B, &pin_GPIO_07),
    PWM_PIN(PWM1, kPWM_Module_3, kPWM_PwmB, IOMUXC_GPIO_AD_05_FLEXPWM1_PWM3_B, &pin_GPIO_AD_05),

    PWM_PIN(PWM1, kPWM_Module_3, kPWM_PwmX, IOMUXC_GPIO_AD_09_FLEXPWM1_PWM3_X, &pin_GPIO_AD_09),
};
