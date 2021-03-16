/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

#include "stm32f7xx_hal.h"
#include "supervisor/shared/safe_mode.h"
#include <stdbool.h>

// F7 Series
#ifdef STM32F746xx
#include "stm32f7/stm32f746xx/clocks.h"
#endif
#ifdef STM32F767xx
#include "stm32f7/stm32f767xx/clocks.h"
#endif

void stm32_peripherals_clocks_init(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    // Configure LSE Drive
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    // Set voltage scaling in accordance with system clock speed
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(CPY_CLK_VSCALE);

    // Set up primary PLL and HSE clocks
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    #if (BOARD_HAS_LOW_SPEED_CRYSTAL)
    RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    #else
    RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    #endif
    RCC_OscInitStruct.HSEState = BOARD_HSE_SOURCE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = HSE_VALUE / 1000000;
    RCC_OscInitStruct.PLL.PLLN = CPY_CLK_PLLN;
    RCC_OscInitStruct.PLL.PLLP = CPY_CLK_PLLP;
    RCC_OscInitStruct.PLL.PLLQ = CPY_CLK_PLLQ;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        // Clock issues are too problematic to even attempt recovery.
        // If you end up here, check whether your LSE settings match your board.
        while (1) {
            ;
        }
    }

    /* Activate the OverDrive to reach the 216 MHz Frequency */
    HAL_PWREx_EnableOverDrive();

    // Configure bus clock sources and divisors
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = CPY_CLK_AHBDIV;
    RCC_ClkInitStruct.APB1CLKDivider = CPY_CLK_APB1DIV;
    RCC_ClkInitStruct.APB2CLKDivider = CPY_CLK_APB2DIV;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, CPY_CLK_FLASH_LATENCY);

    // Set up non-bus peripherals
    // TODO: I2S settings go here
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    #if (BOARD_HAS_LOW_SPEED_CRYSTAL)
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    #else
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    #endif
    #if (CPY_CLK_USB_USES_AUDIOPLL)
    // Should always result in 48M.
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;
    PeriphClkInitStruct.PLLSAIDivQ = 1;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_CK48;
    PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
    #endif

    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}
