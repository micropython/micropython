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

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32fxxx_system
  * @{
  */

/** @addtogroup STM32Fxxx_System_Private_Includes
  * @{
  */

#include "py/mphal.h"

void __fatal_error(const char *msg);

/**
  * @}
  */

/** @addtogroup STM32Fxxx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32Fxxx_System_Private_Defines
  * @{
  */

#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)

#define CONFIG_RCC_CR_1ST (RCC_CR_HSION)
#define CONFIG_RCC_CR_2ND (RCC_CR_HSEON || RCC_CR_CSSON || RCC_CR_PLLON)
#define CONFIG_RCC_PLLCFGR (0x24003010)

#if defined(MCU_SERIES_F7)
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};
#endif

#elif defined(MCU_SERIES_L4)

#define CONFIG_RCC_CR_1ST (RCC_CR_MSION)
#define CONFIG_RCC_CR_2ND (RCC_CR_HSEON || RCC_CR_CSSON || RCC_CR_HSION || RCC_CR_PLLON)
#define CONFIG_RCC_PLLCFGR (0x00001000)
/*
 * FIXME Do not know why I have to define these arrays here! they should be defined in the
 * hal_rcc-file!!
 *
 */
const uint8_t  AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t  APBPrescTable[8] =  {0, 0, 0, 0, 1, 2, 3, 4};
const uint32_t MSIRangeTable[12] = {100000, 200000, 400000, 800000, 1000000, 2000000, \
                                  4000000, 8000000, 16000000, 24000000, 32000000, 48000000};
#else
#error Unknown processor
#endif

/************************* Miscellaneous Configuration ************************/

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field.
                                   This value must be a multiple of 0x200. */
/******************************************************************************/

/**
  * @}
  */

/** @addtogroup STM32Fxxx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32Fxxx_System_Private_Variables
  * @{
  */
  /* This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
  uint32_t SystemCoreClock = 16000000;

/**
  * @}
  */

/** @addtogroup STM32Fxxx_System_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32Fxxx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system
  *         Initialize the FPU setting, vector table location and External memory
  *         configuration.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif
  /* Reset the RCC clock configuration to the default reset state ------------*/

  /* Set HSION bit */
  RCC->CR |= CONFIG_RCC_CR_1ST;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= ~ CONFIG_RCC_CR_2ND;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = CONFIG_RCC_PLLCFGR;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
  RCC->CIR = 0x00000000;
  #elif defined(MCU_SERIES_L4)
  RCC->CIER = 0x00000000;
  #endif

  /* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
  SCB->VTOR = SRAM1_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif

  /* dpgeorge: enable 8-byte stack alignment for IRQ handlers, in accord with EABI */
  SCB->CCR |= SCB_CCR_STKALIGN_Msk;
}


/**
  * @brief  System Clock Configuration
  *
  *         The system Clock is configured for F4/F7 as follows:
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = HSE_VALUE
  *            PLL_M                          = HSE_VALUE/1000000
  *            PLL_N                          = 336
  *            PLL_P                          = 2
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
  *         F4/F7 = HSE / M
  *         L4    = MSI / M
  *     VCO_OUT
  *         F4/F7 = HSE / M * N
  *         L4    = MSI / M * N
  *     PLLCLK
  *         F4/F7 = HSE / M * N / P
  *         L4    = MSI / M * N / R
  *     PLL48CK
  *         F4/F7 = HSE / M * N / Q
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
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

    #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    #elif defined(MCU_SERIES_L4)
    /* Enable the LSE Oscillator */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        __fatal_error("HAL_RCC_OscConfig");
    }
    #endif

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    #elif defined(MCU_SERIES_L4)
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    #endif
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

#if defined(MICROPY_HW_CLK_LAST_FREQ) && MICROPY_HW_CLK_LAST_FREQ
    #if defined(MCU_SERIES_F7)
    #define FREQ_BKP BKP31R
    #elif defined(MCU_SERIES_L4)
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
    p = (((m >> 16) & 0x03)+1)*2;
    n = (m >> 6) & 0x3ff;
    m &= 0x3f;
    if ((q < 2) || (q > 15) || (p > 8) || (p < 2) || (n < 192) || (n >= 433) || (m < 2)) {
        m = MICROPY_HW_CLK_PLLM;
        n = MICROPY_HW_CLK_PLLN;
        p = MICROPY_HW_CLK_PLLP;
        q = MICROPY_HW_CLK_PLLQ;
        h = RCC_SYSCLK_DIV1;
        b1 = RCC_HCLK_DIV4;
        b2 = RCC_HCLK_DIV2;
    } else {
        h <<= 4;
        b1 <<= 10;
        b2 <<= 10;
    }
    RCC_OscInitStruct.PLL.PLLM = m; //MICROPY_HW_CLK_PLLM;
    RCC_OscInitStruct.PLL.PLLN = n; //MICROPY_HW_CLK_PLLN;
    RCC_OscInitStruct.PLL.PLLP = p; //MICROPY_HW_CLK_PLLP;
    RCC_OscInitStruct.PLL.PLLQ = q; //MICROPY_HW_CLK_PLLQ;

    RCC_ClkInitStruct.AHBCLKDivider = h;  //RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = b1; //RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = b2; //RCC_HCLK_DIV2;
#else // defined(MICROPY_HW_CLK_LAST_FREQ) && MICROPY_HW_CLK_LAST_FREQ
    RCC_OscInitStruct.PLL.PLLM = MICROPY_HW_CLK_PLLM;
    RCC_OscInitStruct.PLL.PLLN = MICROPY_HW_CLK_PLLN;
    RCC_OscInitStruct.PLL.PLLP = MICROPY_HW_CLK_PLLP;
    RCC_OscInitStruct.PLL.PLLQ = MICROPY_HW_CLK_PLLQ;
    #if defined(MCU_SERIES_L4)
    RCC_OscInitStruct.PLL.PLLR = MICROPY_HW_CLK_PLLR;
    #endif

    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    #elif defined(MCU_SERIES_L4)
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    #endif
#endif
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    __fatal_error("HAL_RCC_OscConfig");
  }

#if defined(MCU_SERIES_F7)
  /* Activate the OverDrive to reach the 200 MHz Frequency */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    __fatal_error("HAL_PWREx_EnableOverDrive");
  }
#endif

#if !defined(MICROPY_HW_FLASH_LATENCY)
#define MICROPY_HW_FLASH_LATENCY FLASH_LATENCY_5
#endif

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, MICROPY_HW_FLASH_LATENCY) != HAL_OK)
  {
    __fatal_error("HAL_RCC_ClockConfig");
  }

#if defined(MCU_SERIES_F7)
  // The DFU bootloader changes the clocksource register from its default power
  // on reset value, so we set it back here, so the clocksources are the same
  // whether we were started from DFU or from a power on reset.

  RCC->DCKCFGR2 = 0;
#endif
#if defined(MCU_SERIES_L4)
    // Enable MSI-Hardware auto calibration mode with LSE
    HAL_RCCEx_EnableMSIPLLMode();

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1|RCC_PERIPHCLK_I2C1
                                              |RCC_PERIPHCLK_USB |RCC_PERIPHCLK_ADC
                                              |RCC_PERIPHCLK_RNG |RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    /* PLLSAI is used to clock USB, ADC, I2C1 and RNG. The frequency is
       HSE(8MHz)/PLLM(2)*PLLSAI1N(24)/PLLSAIQ(2) = 48MHz. See the STM32CubeMx
       application or the reference manual. */
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLLSAI1;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1N = 24;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInitStruct.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK
                                                 |RCC_PLLSAI1_48M2CLK
                                                 |RCC_PLLSAI1_ADC1CLK;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        __fatal_error("HAL_RCCEx_PeriphCLKConfig");
    }

    __PWR_CLK_ENABLE();

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
#endif
}

void HAL_MspInit(void) {
#if defined(MCU_SERIES_F7)
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
#endif
}
