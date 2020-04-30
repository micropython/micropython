
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

#include "stm32h7xx_hal.h"

void stm32_peripherals_clocks_init(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* The PWR block is always enabled on the H7 series- there is no clock
     enable. For now, use the default VOS3 scale mode (lowest) and limit clock
     frequencies to avoid potential current draw problems from bus
     power when using the max clock speeds throughout the chip. */

    /* Enable HSE Oscillator and activate PLL1 with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = BOARD_OSC_DIV;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_0;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
            RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | \
            RCC_CLOCKTYPE_D3PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;

    /* Unlike on the STM32F4 family, it appears the maximum APB frequencies are
     device-dependent- 120 MHz for this board according to Figure 2 of
     the datasheet. Dividing by half will be safe for now. */
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    /* 4 wait states required for 168MHz and VOS3. */
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);

    /* Like on F4, on H7, USB's actual peripheral clock and bus clock are
     separate. However, the main system PLL (PLL1) doesn't have a direct
     connection to the USB peripheral clock to generate 48 MHz, so we do this
     dance. This will connect PLL1's Q output to the USB peripheral clock. */
    RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct;

    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
    RCC_PeriphCLKInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

}
