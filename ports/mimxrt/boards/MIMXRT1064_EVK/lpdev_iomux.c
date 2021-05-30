/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Robert Hammelrath
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

#include "fsl_iomuxc.h"

// set the UART IOMUX for MIMXRT1060-EVK
// Input is the HW UART number. 4 UARTS
// LPUART3 on D0/D1  -> 1
// LPUART2 on D7/D6  -> 2
// LPUART6 on D8/D9  -> 3
// LPUART8 on A1/A0  -> 4

bool lpuart_set_iomux(int8_t uart) {

    switch (uart) {
        case 2:  // LPUART2
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_02_LPUART2_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_02_LPUART2_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_03_LPUART2_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_03_LPUART2_RX, 0x10B0u);
            break;

        case 3:  // LPUART3
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_06_LPUART3_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_06_LPUART3_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_07_LPUART3_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_07_LPUART3_RX, 0x10B0u);
            break;

        case 6:  // LPUART6
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_02_LPUART6_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_02_LPUART6_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_03_LPUART6_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_03_LPUART6_RX, 0x10B0u);
            break;

        case 8:  // LPUART8
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_10_LPUART8_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_10_LPUART8_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_11_LPUART8_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_11_LPUART8_RX, 0x10B0u);
            break;
        default:
            return false;
    }
    return true;
}