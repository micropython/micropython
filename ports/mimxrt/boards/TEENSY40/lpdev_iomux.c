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

// set the UART IOMUX for TEENSY 4.0
// Input is the HW UART number

bool lpuart_set_iomux(int8_t uart) {

    switch (uart) {
        case 1:  // LPUART1
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_12_LPUART1_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_12_LPUART1_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_13_LPUART1_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_13_LPUART1_RX, 0x10B0u);
            break;

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

       case 4:  // LPUART4
            IOMUXC_SetPinMux(IOMUXC_GPIO_B1_00_LPUART4_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_00_LPUART4_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_B1_01_LPUART4_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_01_LPUART4_RX, 0x10B0u);
            break;

        case 6:  // LPUART6
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_02_LPUART6_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_02_LPUART6_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_03_LPUART6_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_03_LPUART6_RX, 0x10B0u);
            break;

        case 7:  // LPUART7
            IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_31_LPUART7_TX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_31_LPUART7_TX, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_32_LPUART7_RX,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_32_LPUART7_RX, 0x10B0u);
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