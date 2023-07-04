/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Taken from ST Cube library and modified.  See below for original header.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

/**
  ******************************************************************************
  * @file    system_stm32.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   CMSIS Cortex-M4/M7 Device Peripheral Access Layer System Source File.
  *
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "py/mphal.h"
#include "boardctrl.h"
#include "powerctrl.h"

#if defined(STM32F4) || defined(STM32F7) || defined(STM32G4) || defined(STM32H7) || defined(STM32L4)

/**
  * @brief  System Clock Configuration
  *
  *         The system Clock is configured for F4/F7 as follows:
  *         (HSx should be read as HSE or HSI depending on the value of MICROPY_HW_CLK_USE_HSI)
  *            System Clock source            = PLL (HSx)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSx Frequency(Hz)              = HSx_VALUE
  *            PLL_M                          = HSx_VALUE/1000000
  *            PLL_N                          = 336
  *            PLL_P                          = 4
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  *
  *         The system Clock is configured for L4 as follows:
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = MSI_VALUE (4000000)
  *            LSE Frequency(Hz)              = 32768
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_P                          = 7
  *            PLL_Q                          = 2
  *            PLL_R                          = 2 <= This is the source for SysClk, not as on F4/7 PLL_P
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  *
  * PLL is configured as follows:
  *
  *     VCO_IN
  *         F4/F7 = HSx / M
  *         L4    = MSI / M
  *     VCO_OUT
  *         F4/F7 = HSx / M * N
  *         L4    = MSI / M * N
  *     PLLCLK
  *         F4/F7 = HSx / M * N / P
  *         L4    = MSI / M * N / R
  *     PLL48CK
  *         F4/F7 = HSx / M * N / Q
  *         L4    = MSI / M * N / Q  USB Clock is obtained over PLLSAI1
  *
  *     SYSCLK = PLLCLK
  *     HCLK   = SYSCLK / AHB_PRESC
  *     PCLKx  = HCLK / APBx_PRESC
  *
  * Constraints on parameters:
  *
  *     VCO_IN between 1MHz and 2MHz (2MHz recommended)
  *     VCO_OUT between 192MHz and 432MHz
  *     HSE = 8MHz
  *     HSI = 16MHz
  *     M = 2 .. 63 (inclusive)
  *     N = 192 ... 432 (inclusive)
  *     P = 2, 4, 6, 8
  *     Q = 2 .. 15 (inclusive)
  *
  *     AHB_PRESC=1,2,4,8,16,64,128,256,512
  *     APBx_PRESC=1,2,4,8,16
  *
  * Output clocks:
  *
  * CPU             SYSCLK      max 168MHz
  * USB,RNG,SDIO    PLL48CK     must be 48MHz for USB
  * AHB             HCLK        max 168MHz
  * APB1            PCLK1       max 42MHz
  * APB2            PCLK2       max 84MHz
  *
  * Timers run from APBx if APBx_PRESC=1, else 2x APBx
  */
MP_WEAK void SystemClock_Config(void) {
    #if defined(STM32F7)
    // The DFU bootloader changes the clocksource register from its default power
    // on reset value, so we set it back here, so the clocksources are the same
    // whether we were started from DFU or from a power on reset.
    RCC->DCKCFGR2 = 0;
    #endif

    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    #if defined(STM32G4) || defined(STM32H7)
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    #endif

    #if defined(STM32F4) || defined(STM32F7) || defined(STM32H7)

    #if defined(STM32H7) && defined(SMPS)
    // H7 MCUs with SMPS must provide a power supply configuration.
    MODIFY_REG(PWR->CR3, PWR_SUPPLY_CONFIG_MASK, MICROPY_HW_PWR_SMPS_CONFIG);
    #elif defined(STM32H7)
    // H7 MCUs without SMPS, lock the power supply configuration update.
    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);
    #else
    // other MCUs, enable power control clock.
    __PWR_CLK_ENABLE();
    #endif

    #if defined(STM32H7)
    // Wait until the voltage levels are valid.
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_ACTVOSRDY)) {
    }

    #if defined(MICROPY_HW_PWR_SMPS_CONFIG)
    // If the SMPS supplies external circuitry, wait for the external supply ready flag.
    if (MICROPY_HW_PWR_SMPS_CONFIG & PWR_CR3_SMPSEXTHP) {
        while (!__HAL_PWR_GET_FLAG(PWR_FLAG_SMPSEXTRDY)) {
        }
    }
    #endif // defined(MICROPY_HW_PWR_SMPS_CONFIG)

    #endif // defined(STM32H7)

    /* The voltage scaling allows optimizing the power consumption when the device is
       clocked below the maximum system frequency, to update the voltage scaling value
       regarding system frequency refer to product datasheet.  */
    #if defined(STM32H7)
    if (HAL_GetREVID() >= 0x2003) {
        // Enable VSCALE0 for revision V devices.
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
    } else
    #endif
    {
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    }

    #elif defined(STM32G4)
    // Configure the main internal regulator output voltage
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
    #elif defined(STM32L4)
    // Configure LSE Drive Capability
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
    #endif

    #if defined(STM32H7)
    // Wait until core supply reaches the required voltage level.
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }
    #endif

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    #if defined(STM32F4) || defined(STM32F7) || defined(STM32G4) || defined(STM32H7)
    RCC_OscInitStruct.OscillatorType = MICROPY_HW_RCC_OSCILLATOR_TYPE;
    RCC_OscInitStruct.HSEState = MICROPY_HW_RCC_HSE_STATE;
    RCC_OscInitStruct.HSIState = MICROPY_HW_RCC_HSI_STATE;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    #if defined(MICROPY_HW_RCC_CSI_STATE)
    RCC_OscInitStruct.CSIState = MICROPY_HW_RCC_CSI_STATE;
    RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_CSI;
    #endif

    #if defined(MICROPY_HW_RCC_HSI48_STATE)
    RCC_OscInitStruct.HSI48State = MICROPY_HW_RCC_HSI48_STATE;
    RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_HSI48;
    #endif

    RCC_OscInitStruct.PLL.PLLSource = MICROPY_HW_RCC_PLL_SRC;

    #elif defined(STM32L4)

    #if MICROPY_HW_CLK_USE_HSE
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = MICROPY_HW_CLK_PLLM;
    RCC_OscInitStruct.PLL.PLLN = MICROPY_HW_CLK_PLLN;
    RCC_OscInitStruct.PLL.PLLP = MICROPY_HW_CLK_PLLP;
    RCC_OscInitStruct.PLL.PLLQ = MICROPY_HW_CLK_PLLQ;
    RCC_OscInitStruct.PLL.PLLR = MICROPY_HW_CLK_PLLR;
    RCC_OscInitStruct.MSIState = RCC_MSI_OFF;
    #else
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    #endif

    #if MICROPY_HW_RTC_USE_LSE
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    #else
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    #endif

    #endif

    #if defined(STM32G4)
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    #if MICROPY_HW_CLK_USE_HSI && MICROPY_HW_CLK_USE_HSI48
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    #else
    RCC_OscInitStruct.OscillatorType = MICROPY_HW_RCC_OSCILLATOR_TYPE;
    RCC_OscInitStruct.HSEState = MICROPY_HW_RCC_HSE_STATE;
    RCC_OscInitStruct.HSIState = MICROPY_HW_RCC_HSI_STATE;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    #endif
    RCC_OscInitStruct.PLL.PLLM = MICROPY_HW_CLK_PLLM;
    RCC_OscInitStruct.PLL.PLLN = MICROPY_HW_CLK_PLLN;
    RCC_OscInitStruct.PLL.PLLP = MICROPY_HW_CLK_PLLP;
    RCC_OscInitStruct.PLL.PLLQ = MICROPY_HW_CLK_PLLQ;
    RCC_OscInitStruct.PLL.PLLR = MICROPY_HW_CLK_PLLR;
    #endif

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    #if defined(STM32H7)
    RCC_ClkInitStruct.ClockType |= (RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1);
    #endif

    #if defined(MICROPY_HW_CLK_LAST_FREQ) && MICROPY_HW_CLK_LAST_FREQ
    #if defined(STM32F7)
    #define FREQ_BKP BKP31R
    #elif defined(STM32L4)
    #error Unsupported Processor
    #else
    #define FREQ_BKP BKP19R
    #endif
    uint32_t m = RTC->FREQ_BKP;
    uint32_t n;
    uint32_t p;
    uint32_t q;

    // 222111HH HHQQQQPP nNNNNNNN NNMMMMMM
    uint32_t h = (m >> 22) & 0xf;
    uint32_t b1 = (m >> 26) & 0x7;
    uint32_t b2 = (m >> 29) & 0x7;
    q = (m >> 18) & 0xf;
    p = (((m >> 16) & 0x03) + 1) * 2;
    n = (m >> 6) & 0x3ff;
    m &= 0x3f;
    if ((q < 2) || (q > 15) || (p > 8) || (p < 2) || (n < 192) || (n >= 433) || (m < 2)) {
        m = MICROPY_HW_CLK_PLLM;
        n = MICROPY_HW_CLK_PLLN;
        p = MICROPY_HW_CLK_PLLP;
        q = MICROPY_HW_CLK_PLLQ;
        h = MICROPY_HW_CLK_AHB_DIV;
        b1 = MICROPY_HW_CLK_APB1_DIV;
        b2 = MICROPY_HW_CLK_APB2_DIV;
    } else {
        h <<= 4;
        b1 <<= 10;
        b2 <<= 10;
    }
    RCC_OscInitStruct.PLL.PLLM = m; // MICROPY_HW_CLK_PLLM;
    RCC_OscInitStruct.PLL.PLLN = n; // MICROPY_HW_CLK_PLLN;
    RCC_OscInitStruct.PLL.PLLP = p; // MICROPY_HW_CLK_PLLP;
    RCC_OscInitStruct.PLL.PLLQ = q; // MICROPY_HW_CLK_PLLQ;

    RCC_ClkInitStruct.AHBCLKDivider = h;
    RCC_ClkInitStruct.APB1CLKDivider = b1;
    RCC_ClkInitStruct.APB2CLKDivider = b2;
    #else // defined(MICROPY_HW_CLK_LAST_FREQ) && MICROPY_HW_CLK_LAST_FREQ
    RCC_OscInitStruct.PLL.PLLM = MICROPY_HW_CLK_PLLM;
    RCC_OscInitStruct.PLL.PLLN = MICROPY_HW_CLK_PLLN;
    RCC_OscInitStruct.PLL.PLLP = MICROPY_HW_CLK_PLLP;
    RCC_OscInitStruct.PLL.PLLQ = MICROPY_HW_CLK_PLLQ;
    #if defined(STM32G4) || defined(STM32H7) || defined(STM32L4)
    RCC_OscInitStruct.PLL.PLLR = MICROPY_HW_CLK_PLLR;
    #endif

    #if defined(STM32H7)
    RCC_OscInitStruct.PLL.PLLRGE = MICROPY_HW_CLK_PLLVCI;
    RCC_OscInitStruct.PLL.PLLVCOSEL = MICROPY_HW_CLK_PLLVCO;
    RCC_OscInitStruct.PLL.PLLFRACN = MICROPY_HW_CLK_PLLFRAC;
    #endif

    #if defined(STM32F4) || defined(STM32F7)
    RCC_ClkInitStruct.AHBCLKDivider = MICROPY_HW_CLK_AHB_DIV;
    RCC_ClkInitStruct.APB1CLKDivider = MICROPY_HW_CLK_APB1_DIV;
    RCC_ClkInitStruct.APB2CLKDivider = MICROPY_HW_CLK_APB2_DIV;
    #elif defined(STM32G4)
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = MICROPY_HW_CLK_AHB_DIV;
    RCC_ClkInitStruct.APB1CLKDivider = MICROPY_HW_CLK_APB1_DIV;
    RCC_ClkInitStruct.APB2CLKDivider = MICROPY_HW_CLK_APB2_DIV;
    #elif defined(STM32L4)
    RCC_ClkInitStruct.AHBCLKDivider = MICROPY_HW_CLK_AHB_DIV;
    RCC_ClkInitStruct.APB1CLKDivider = MICROPY_HW_CLK_APB1_DIV;
    RCC_ClkInitStruct.APB2CLKDivider = MICROPY_HW_CLK_APB2_DIV;
    #elif defined(STM32H7)
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = MICROPY_HW_CLK_AHB_DIV;
    RCC_ClkInitStruct.APB3CLKDivider = MICROPY_HW_CLK_APB3_DIV;
    RCC_ClkInitStruct.APB1CLKDivider = MICROPY_HW_CLK_APB1_DIV;
    RCC_ClkInitStruct.APB2CLKDivider = MICROPY_HW_CLK_APB2_DIV;
    RCC_ClkInitStruct.APB4CLKDivider = MICROPY_HW_CLK_APB4_DIV;
    #endif
    #endif

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        MICROPY_BOARD_FATAL_ERROR("HAL_RCC_OscConfig");
    }

    #if defined(MICROPY_HW_CLK_PLL2M)
    // PLL2 configuration.
    PeriphClkInitStruct.PLL2.PLL2M = MICROPY_HW_CLK_PLL2M;
    PeriphClkInitStruct.PLL2.PLL2N = MICROPY_HW_CLK_PLL2N;
    PeriphClkInitStruct.PLL2.PLL2P = MICROPY_HW_CLK_PLL2P;
    PeriphClkInitStruct.PLL2.PLL2Q = MICROPY_HW_CLK_PLL2Q;
    PeriphClkInitStruct.PLL2.PLL2R = MICROPY_HW_CLK_PLL2R;
    PeriphClkInitStruct.PLL2.PLL2RGE = MICROPY_HW_CLK_PLL2VCI;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = MICROPY_HW_CLK_PLL2VCO;
    PeriphClkInitStruct.PLL2.PLL2FRACN = MICROPY_HW_CLK_PLL2FRAC;
    #endif

    #if defined(MICROPY_HW_CLK_PLL3M)
    // PLL3 configuration.
    PeriphClkInitStruct.PLL3.PLL3M = MICROPY_HW_CLK_PLL3M;
    PeriphClkInitStruct.PLL3.PLL3N = MICROPY_HW_CLK_PLL3N;
    PeriphClkInitStruct.PLL3.PLL3P = MICROPY_HW_CLK_PLL3P;
    PeriphClkInitStruct.PLL3.PLL3Q = MICROPY_HW_CLK_PLL3Q;
    PeriphClkInitStruct.PLL3.PLL3R = MICROPY_HW_CLK_PLL3R;
    PeriphClkInitStruct.PLL3.PLL3RGE = MICROPY_HW_CLK_PLL3VCI;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = MICROPY_HW_CLK_PLL3VCO;
    PeriphClkInitStruct.PLL3.PLL3FRACN = MICROPY_HW_CLK_PLL3FRAC;
    #endif

    #if defined(STM32H7)

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
    #if defined(MICROPY_HW_RCC_USB_CLKSOURCE)
    PeriphClkInitStruct.UsbClockSelection = MICROPY_HW_RCC_USB_CLKSOURCE;
    #else
    // Use PLL3 for USB clock source by default.
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
    #endif

    #if defined(MICROPY_HW_RCC_RTC_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = MICROPY_HW_RCC_RTC_CLKSOURCE;
    #endif

    #if defined(MICROPY_HW_RCC_RNG_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_RNG;
    PeriphClkInitStruct.RngClockSelection = MICROPY_HW_RCC_RNG_CLKSOURCE;
    #endif

    #if defined(MICROPY_HW_RCC_FMC_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_FMC;
    PeriphClkInitStruct.FmcClockSelection = MICROPY_HW_RCC_FMC_CLKSOURCE;
    #endif

    #if defined(MICROPY_HW_RCC_ADC_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.AdcClockSelection = MICROPY_HW_RCC_ADC_CLKSOURCE;
    #endif

    #if defined(MICROPY_HW_RCC_SDMMC_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_SDMMC;
    PeriphClkInitStruct.SdmmcClockSelection = MICROPY_HW_RCC_SDMMC_CLKSOURCE;
    #endif

    #if defined(MICROPY_HW_RCC_FDCAN_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_FDCAN;
    PeriphClkInitStruct.FdcanClockSelection = MICROPY_HW_RCC_FDCAN_CLKSOURCE;
    #endif

    #if defined(MICROPY_HW_RCC_QSPI_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_QSPI;
    PeriphClkInitStruct.QspiClockSelection = MICROPY_HW_RCC_QSPI_CLKSOURCE;
    #endif

    #if defined(MICROPY_HW_RCC_SPI123_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_SPI123;
    PeriphClkInitStruct.Spi123ClockSelection = MICROPY_HW_RCC_SPI123_CLKSOURCE;
    #endif

    #if defined(MICROPY_HW_RCC_I2C123_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_I2C123;
    PeriphClkInitStruct.I2c123ClockSelection = MICROPY_HW_RCC_I2C123_CLKSOURCE;
    #endif

    #if defined(MICROPY_HW_RCC_SPI45_CLKSOURCE)
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_SPI45;
    PeriphClkInitStruct.Spi45ClockSelection = MICROPY_HW_RCC_SPI45_CLKSOURCE;
    #endif

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        MICROPY_BOARD_FATAL_ERROR("HAL_RCCEx_PeriphCLKConfig");
    }
    #endif  // defined(STM32H7)

    #if defined(STM32F7)
    /* Activate the OverDrive to reach the 200 MHz Frequency */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        MICROPY_BOARD_FATAL_ERROR("HAL_PWREx_EnableOverDrive");
    }
    #endif

    #if defined(STM32G4)
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8) != HAL_OK) {
        MICROPY_BOARD_FATAL_ERROR("HAL_RCC_ClockConfig");
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_LPUART1
        | RCC_PERIPHCLK_RNG | RCC_PERIPHCLK_ADC12
        | RCC_PERIPHCLK_FDCAN | RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    PeriphClkInitStruct.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
    PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_HSE;
    PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_HSI48;
    PeriphClkInitStruct.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        MICROPY_BOARD_FATAL_ERROR("HAL_RCCEx_PeriphCLKConfig");
    }
    #else
    uint32_t vco_out = RCC_OscInitStruct.PLL.PLLN * (MICROPY_HW_CLK_VALUE / 1000000) / RCC_OscInitStruct.PLL.PLLM;
    uint32_t sysclk_mhz = vco_out / RCC_OscInitStruct.PLL.PLLP;
    bool need_pll48 = vco_out % 48 != 0;
    if (powerctrl_rcc_clock_config_pll(&RCC_ClkInitStruct, sysclk_mhz, need_pll48) != 0) {
        MICROPY_BOARD_FATAL_ERROR("HAL_RCC_ClockConfig");
    }
    #endif

    #if defined(STM32H7)
    /* Activate CSI clock mandatory for I/O Compensation Cell*/
    __HAL_RCC_CSI_ENABLE();

    /* Enable SYSCFG clock mandatory for I/O Compensation Cell */
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    /* Enable the I/O Compensation Cell */
    HAL_EnableCompensationCell();

    /* Enable the USB voltage level detector */
    HAL_PWREx_EnableUSBVoltageDetector();
    #endif

    #if defined(STM32L4)
    // Enable MSI-Hardware auto calibration mode with LSE
    HAL_RCCEx_EnableMSIPLLMode();

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1 | RCC_PERIPHCLK_I2C1
        | RCC_PERIPHCLK_USB | RCC_PERIPHCLK_ADC
        | RCC_PERIPHCLK_RNG | RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;

    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
    PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLLSAI1;

    #if MICROPY_HW_RTC_USE_LSE
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    #else
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    #endif

    #if MICROPY_HW_CLK_USE_HSE
    PeriphClkInitStruct.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1M = 1; // MICROPY_HW_CLK_PLLSAIM;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1N = MICROPY_HW_CLK_PLLSAIN;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1P = MICROPY_HW_CLK_PLLSAIP;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1Q = MICROPY_HW_CLK_PLLSAIQ;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1R = MICROPY_HW_CLK_PLLSAIR;
    #else
    /* PLLSAI is used to clock USB, ADC, I2C1 and RNG. The frequency is
       MSI(4MHz)/PLLM(1)*PLLSAI1N(24)/PLLSAIQ(2) = 48MHz. See the STM32CubeMx
       application or the reference manual. */
    PeriphClkInitStruct.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1N = 24;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    #endif
    PeriphClkInitStruct.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK
        | RCC_PLLSAI1_48M2CLK
        | RCC_PLLSAI1_ADC1CLK;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        MICROPY_BOARD_FATAL_ERROR("HAL_RCCEx_PeriphCLKConfig");
    }

    __PWR_CLK_ENABLE();

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, TICK_INT_PRIORITY, 0));
    #endif

    #if defined(STM32H7) && !defined(NDEBUG)
    // Enable the Debug Module in low-power modes.
    DBGMCU->CR |= (DBGMCU_CR_DBG_SLEEPD1 | DBGMCU_CR_DBG_STOPD1 | DBGMCU_CR_DBG_STANDBYD1);
    #endif
}

#endif
