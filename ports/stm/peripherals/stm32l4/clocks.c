// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Blues Wireless Contributors
//
// SPDX-License-Identifier: MIT

#include "stm32l4xx_hal.h"
#include "supervisor/shared/safe_mode.h"
#include <stdbool.h>

// L4 Series
#ifdef STM32L4R5xx
#include "stm32l4/stm32l4r5xx/clocks.h"
#else
#error Please add other MCUs here so that they are not silently ignored due to #define typos
#endif

#if BOARD_HAS_HIGH_SPEED_CRYSTAL
#error HSE support needs to be added for the L4 family.
#elif !BOARD_HAS_LOW_SPEED_CRYSTAL
  #error LSE clock source required
#elif !defined(BOARD_LSE_DRIVE_LEVEL)
  #error BOARD_LSE_DRIVE_LEVEL is not defined for this board. The board should define the drive strength of 32kHz external crystal in line with calculations specified in ST AN2867 sections 3.3, 3.4, and STM32L4 datasheet DS12023 Table 58, LSE oscillator characteristics.
#endif

void Error_Handler(void) {
    for (;;) {

    }
}

#define HAL_CHECK(x) if (x != HAL_OK) Error_Handler()

void stm32_peripherals_clocks_init(void) {

    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    // Configure LSE Drive
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(BOARD_LSE_DRIVE_LEVEL);
    __HAL_RCC_PWR_CLK_ENABLE();

    /** Configure the main internal regulator output voltage
  */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK) {
        Error_Handler();
    }


    /* Activate PLL with MSI , stabilizied via PLL by LSE */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 6;
    RCC_OscInitStruct.PLL.PLLN = 30;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV5;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    HAL_CHECK(HAL_RCC_OscConfig(&RCC_OscInitStruct));

    /* Enable MSI Auto-calibration through LSE */
    HAL_RCCEx_EnableMSIPLLMode();

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
    clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    // Avoid overshoot and start with HCLK 60 MHz
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_CHECK(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3));

    /* AHB prescaler divider at 1 as second step */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    HAL_CHECK(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5));

    /* Select MSI output as USB clock source */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_MSI;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
    HAL_CHECK(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct));

}
