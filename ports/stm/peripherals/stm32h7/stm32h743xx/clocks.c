
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

#include "py/mpconfig.h"

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
  RCC_OscInitStruct.PLL.PLLM = HSE_VALUE/1000000;
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
  
    // RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    // RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    // RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    // /** Supply configuration update enable 
    // */
    // HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    // /** Configure the main internal regulator output voltage 
    // */
    // __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
    // /** Macro to configure the PLL clock source 
    // */
    // __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
    // /** Initializes the CPU, AHB and APB busses clocks 
    // */
    // RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    // RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    // RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    // RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    // RCC_OscInitStruct.PLL.PLLM = 4;
    // RCC_OscInitStruct.PLL.PLLN = 400;
    // RCC_OscInitStruct.PLL.PLLP = 2;
    // RCC_OscInitStruct.PLL.PLLQ = 4;
    // RCC_OscInitStruct.PLL.PLLR = 2;
    // RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
    // RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    // RCC_OscInitStruct.PLL.PLLFRACN = 0;
    // if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    // {

    // }
    // /** Initializes the CPU, AHB and APB busses clocks 
    // */
    // RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
    //                           |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
    //                           |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    // RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    // RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    // RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    // RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    // RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    // RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    // RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    // if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    // {

    // }
    // PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB; /*RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_UART4
    //                           |RCC_PERIPHCLK_SPI1|RCC_PERIPHCLK_ADC
    //                           |RCC_PERIPHCLK_I2C1| */
    // // PeriphClkInitStruct.PLL2.PLL2M = 1;
    // // PeriphClkInitStruct.PLL2.PLL2N = 19;
    // // PeriphClkInitStruct.PLL2.PLL2P = 2;
    // // PeriphClkInitStruct.PLL2.PLL2Q = 2;
    // // PeriphClkInitStruct.PLL2.PLL2R = 2;
    // // PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
    // // PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
    // // PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    // PeriphClkInitStruct.PLL3.PLL3M = 4;
    // PeriphClkInitStruct.PLL3.PLL3N = 120;
    // PeriphClkInitStruct.PLL3.PLL3P = 2;
    // PeriphClkInitStruct.PLL3.PLL3Q = 5;
    // PeriphClkInitStruct.PLL3.PLL3R = 2;
    // PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_1;
    // PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    // PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    // PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    // PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    // PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
    // PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
    // PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
    // if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    // {

    // }

}
