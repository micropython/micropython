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

// set the UART IOMUX for MIMXRT1021
// Input is the HW UART number

// D3/D5   LPUART1  Not usable, Since D3 is blocked.
// D0/D1   LPUART2 -> 1
// D6/D9   LPUART3 -> 2
// D10/D12 LPUART5 -> 3
// D14/D15 LPUART8 -> 4
// A0/A1   LPUART4 -> 5

bool lpuart_set_iomux(int8_t uart) {

    switch (uart) {
        case 1:  // LPUART1
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_06_LPUART1_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_06_LPUART1_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_07_LPUART1_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_07_LPUART1_RX, 0x10B0u);
            break;

        case 2:  // LPUART2
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_08_LPUART2_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_08_LPUART2_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_09_LPUART2_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_09_LPUART2_RX, 0x10B0u);
            break;

        case 3:  // LPUART3
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_14_LPUART3_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_14_LPUART3_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_15_LPUART3_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_15_LPUART3_RX, 0x10B0u);
            break;

        case 4:  // LPUART4
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_10_LPUART4_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_10_LPUART4_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_11_LPUART4_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_11_LPUART4_RX, 0x10B0u);
            break;

        case 5:  // LPUART5
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_10_LPUART5_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_10_LPUART5_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_11_LPUART5_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_11_LPUART5_RX, 0x10B0u);
            break;

        case 8:  // LPUART8
            IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_02_LPUART8_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_02_LPUART8_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_03_LPUART8_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_03_LPUART8_RX, 0x10B0u);
            break;
        default:
            return false;
    }
    return true;
}