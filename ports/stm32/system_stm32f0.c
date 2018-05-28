/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Taken from ST Cube library and modified.  See below for original header.
 */

/**
  ******************************************************************************
  * @file    system_stm32f0xx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer System Source File.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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

#include STM32_HAL_H

#ifndef HSE_VALUE
#define HSE_VALUE (8000000)
#endif

#ifndef HSI_VALUE
#define HSI_VALUE (8000000)
#endif

#ifndef HSI48_VALUE
#define HSI48_VALUE (48000000)
#endif

/* This variable is updated in three ways:
  1) by calling CMSIS function SystemCoreClockUpdate()
  2) by calling HAL API function HAL_RCC_GetHCLKFreq()
  3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
     Note: If you use this function to configure the system clock there is no need to
           call the 2 first functions listed above, since SystemCoreClock variable is
           updated automatically.
*/
uint32_t SystemCoreClock = 8000000;

const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};

void SystemInit(void) {
    // Set HSION bit
    RCC->CR |= (uint32_t)0x00000001U;

    #if defined(STM32F051x8) || defined(STM32F058x8)
    // Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE and MCOSEL[2:0] bits
    RCC->CFGR &= (uint32_t)0xF8FFB80CU;
    #else
    // Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits
    RCC->CFGR &= (uint32_t)0x08FFB80CU;
    #endif

    // Reset HSEON, CSSON and PLLON bits
    RCC->CR &= (uint32_t)0xFEF6FFFFU;

    // Reset HSEBYP bit
    RCC->CR &= (uint32_t)0xFFFBFFFFU;

    // Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits
    RCC->CFGR &= (uint32_t)0xFFC0FFFFU;

    // Reset PREDIV[3:0] bits
    RCC->CFGR2 &= (uint32_t)0xFFFFFFF0U;

    #if defined(STM32F072xB) || defined(STM32F078xx)
    // Reset USART2SW[1:0], USART1SW[1:0], I2C1SW, CECSW, USBSW and ADCSW bits
    RCC->CFGR3 &= (uint32_t)0xFFFCFE2CU;
    #elif defined(STM32F071xB)
    // Reset USART2SW[1:0], USART1SW[1:0], I2C1SW, CECSW and ADCSW bits
    RCC->CFGR3 &= (uint32_t)0xFFFFCEACU;
    #elif defined(STM32F091xC) || defined(STM32F098xx)
    // Reset USART3SW[1:0], USART2SW[1:0], USART1SW[1:0], I2C1SW, CECSW and ADCSW bits
    RCC->CFGR3 &= (uint32_t)0xFFF0FEACU;
    #elif defined(STM32F030x6) || defined(STM32F030x8) || defined(STM32F031x6) || defined(STM32F038xx) || defined(STM32F030xC)
    // Reset USART1SW[1:0], I2C1SW and ADCSW bits
    RCC->CFGR3 &= (uint32_t)0xFFFFFEECU;
    #elif defined(STM32F051x8) || defined(STM32F058xx)
    // Reset USART1SW[1:0], I2C1SW, CECSW and ADCSW bits
    RCC->CFGR3 &= (uint32_t)0xFFFFFEACU;
    #elif defined(STM32F042x6) || defined(STM32F048xx)
    // Reset USART1SW[1:0], I2C1SW, CECSW, USBSW and ADCSW bits
    RCC->CFGR3 &= (uint32_t)0xFFFFFE2CU;
    #elif defined(STM32F070x6) || defined(STM32F070xB)
    // Reset USART1SW[1:0], I2C1SW, USBSW and ADCSW bits
    RCC->CFGR3 &= (uint32_t)0xFFFFFE6CU;
    // Set default USB clock to PLLCLK, since there is no HSI48
    RCC->CFGR3 |= (uint32_t)0x00000080U;
    #else
    #warning "No target selected"
    #endif

    // Reset HSI14 bit
    RCC->CR2 &= (uint32_t)0xFFFFFFFEU;

    // Disable all interrupts
    RCC->CIR = 0x00000000U;

    // dpgeorge: enable 8-byte stack alignment for IRQ handlers, in accord with EABI
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
}

void SystemClock_Config(void) {
    // Set flash latency to 1 because SYSCLK > 24MHz
    FLASH->ACR = (FLASH->ACR & ~0x7) | 0x1;

    // Use the 48MHz internal oscillator
    RCC->CR2 |= RCC_CR2_HSI48ON;
    while ((RCC->CR2 & RCC_CR2_HSI48RDY) == 0) {
    }
    RCC->CFGR |= 3 << RCC_CFGR_SW_Pos;
    while (((RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x3) != 0x03) {
        // Wait for SYSCLK source to change
    }

    SystemCoreClockUpdate();

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void SystemCoreClockUpdate(void) {
    // Get SYSCLK source
    uint32_t tmp = RCC->CFGR & RCC_CFGR_SWS;

    switch (tmp) {
        case RCC_CFGR_SWS_HSI:
            SystemCoreClock = HSI_VALUE;
            break;
        case RCC_CFGR_SWS_HSE:
            SystemCoreClock = HSE_VALUE;
            break;
        case RCC_CFGR_SWS_PLL: {
            /* Get PLL clock source and multiplication factor */
            uint32_t pllmull = RCC->CFGR & RCC_CFGR_PLLMUL;
            uint32_t pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
            pllmull = (pllmull >> 18) + 2;
            uint32_t predivfactor = (RCC->CFGR2 & RCC_CFGR2_PREDIV) + 1;

            if (pllsource == RCC_CFGR_PLLSRC_HSE_PREDIV) {
                /* HSE used as PLL clock source : SystemCoreClock = HSE/PREDIV * PLLMUL */
                SystemCoreClock = (HSE_VALUE/predivfactor) * pllmull;
            #if defined(STM32F042x6) || defined(STM32F048xx) || defined(STM32F072xB) \
                || defined(STM32F078xx) || defined(STM32F091xC) || defined(STM32F098xx)
            } else if (pllsource == RCC_CFGR_PLLSRC_HSI48_PREDIV) {
                /* HSI48 used as PLL clock source : SystemCoreClock = HSI48/PREDIV * PLLMUL */
                SystemCoreClock = (HSI48_VALUE/predivfactor) * pllmull;
            #endif
            } else {
                #if defined(STM32F042x6) || defined(STM32F048xx)  || defined(STM32F070x6) \
                    || defined(STM32F078xx) || defined(STM32F071xB)  || defined(STM32F072xB) \
                    || defined(STM32F070xB) || defined(STM32F091xC) || defined(STM32F098xx)  || defined(STM32F030xC)
                /* HSI used as PLL clock source : SystemCoreClock = HSI/PREDIV * PLLMUL */
                SystemCoreClock = (HSI_VALUE / predivfactor) * pllmull;
                #else
                /* HSI used as PLL clock source : SystemCoreClock = HSI/2 * PLLMUL */
                SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
                #endif
            }
            break;
        }
        case RCC_CFGR_SWS_HSI48:
            SystemCoreClock = HSI48_VALUE;
            break;
        default:
            SystemCoreClock = HSI_VALUE;
            break;
    }

    // Compute HCLK clock frequency
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    SystemCoreClock >>= tmp;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
