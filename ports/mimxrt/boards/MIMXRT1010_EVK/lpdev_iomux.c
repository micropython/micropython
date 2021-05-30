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

// set the UART IOMUX for MIMXRT1010-EVK
// Input is the HW UART number
// LPUART1 on D0/D1 -> 1
// LPUART3 on A0/D4 -> 3
// LPUART4 on D6/D7 -> 2

bool lpuart_set_iomux(int8_t uart) {

    switch (uart) {
        case 1:  // LPUART1
            IOMUXC_SetPinMux(IOMUXC_GPIO_10_LPUART1_TXD, 0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_10_LPUART1_TXD, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_09_LPUART1_RXD, 0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_09_LPUART1_RXD, 0x10B0u);
            break;

        case 3:  // LPUART3
            IOMUXC_SetPinMux(IOMUXC_GPIO_08_LPUART3_TXD, 0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_08_LPUART3_TXD, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_07_LPUART3_RXD, 0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_07_LPUART3_RXD, 0x10B0u);
            break;

       case 4:  // LPUART4
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_02_LPUART4_TXD, 0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_02_LPUART4_TXD, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_01_LPUART4_RXD, 0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_01_LPUART4_RXD, 0x10B0u);
            break;

       default:
            return false;
    }
    return true;
}