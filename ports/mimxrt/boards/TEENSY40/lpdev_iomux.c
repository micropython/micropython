/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 NXP
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

// set the SPI IOMUX for Teensy 4.1
// Input is the HW SPI number

bool lpspi_set_iomux(int8_t spi) {
    CLOCK_EnableClock(kCLOCK_Iomuxc);           /* iomuxc clock (iomuxc_clk_enable): 0x03U */

    switch (spi) {
        case 3:  // LPSPI3
            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_15_LPSPI3_SCK,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_15_LPSPI3_SCK, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_03_LPSPI3_PCS0,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_03_LPSPI3_PCS0, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_14_LPSPI3_SDO,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_14_LPSPI3_SDO, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B0_02_LPSPI3_SDI,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B0_02_LPSPI3_SDI, 0x10B0u);
            break;

        case 4:  // LPSPI4
            IOMUXC_SetPinMux(IOMUXC_GPIO_B0_03_LPSPI4_SCK,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_03_LPSPI4_SCK, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_B0_00_LPSPI4_PCS0,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_00_LPSPI4_PCS0, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_B0_02_LPSPI4_SDO,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_02_LPSPI4_SDO, 0x10B0u);

            IOMUXC_SetPinMux(IOMUXC_GPIO_B0_01_LPSPI4_SDI,0U);
            IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_01_LPSPI4_SDI, 0x10B0u);
            break;

        default:
            return false;
    }
    return true;
}
