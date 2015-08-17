/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "mpconfigboard.h"
#include STM32_HAL_H

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
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

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
  *         The system Clock is configured as follow :
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
  * @param  None
  * @retval None
  *
  * PLL is configured as follows:
  *
  *     VCO_IN  = HSE / M
  *     VCO_OUT = HSE / M * N
  *     PLLCLK  = HSE / M * N / P
  *     PLL48CK = HSE / M * N / Q
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

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = MICROPY_HW_CLK_PLLM;
  RCC_OscInitStruct.PLL.PLLN = MICROPY_HW_CLK_PLLN;
  RCC_OscInitStruct.PLL.PLLP = MICROPY_HW_CLK_PLLP;
  RCC_OscInitStruct.PLL.PLLQ = MICROPY_HW_CLK_PLLQ;
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

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

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
}

void HAL_MspInit(void) {
#if defined(MCU_SERIES_F7)
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
#endif
}
