/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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

#include "boards/board.h"
#include "mpconfigboard.h"
#include "fsl_iomuxc.h"

void board_init(void) {
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_06_FLEXSPI_A_SS0_B, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_07_FLEXSPI_A_DATA1,1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_08_FLEXSPI_A_DATA2, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_09_FLEXSPI_A_DATA0, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_10_FLEXSPI_A_SCLK, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_11_FLEXSPI_A_DATA3, 1U);
    IOMUXC_SetPinMux(IOMUXC_GPIO_SD_12_FLEXSPI_A_DQS, 1U);

    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_06_FLEXSPI_A_SS0_B,0x10E1U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_07_FLEXSPI_A_DATA1, 0x10E1U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_08_FLEXSPI_A_DATA2, 0x10E1U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_09_FLEXSPI_A_DATA0, 0x10E1U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_10_FLEXSPI_A_SCLK, 0x10E1U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_11_FLEXSPI_A_DATA3, 0x10E1U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_12_FLEXSPI_A_DQS, 0x10E1U);
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
}
