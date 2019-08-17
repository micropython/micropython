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
#include "powerctrl.h"

void __fatal_error(const char *msg);

const uint8_t AHBPrescTable[16U] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8U] =  {0, 0, 0, 0, 1, 2, 3, 4};

/************************* Miscellaneous Configuration ************************/

/*!< user need to use external SRAM  */
#ifdef DATA_IN_ExtSRAM
    #undef DATA_IN_ExtSRAM
#endif

#if (defined(STM32F100xE) || defined(STM32F101xE) || defined(STM32F101xG) || defined(STM32F103xE) || defined(STM32F103xG)) && \
    defined(MICROPY_HW_EXT_SRAM_ENABLE) && MICROPY_HW_EXT_SRAM_ENABLE
    #define DATA_IN_ExtSRAM
    static void SystemInit_ExtMemCtl(void);
#endif

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET 0x00 /*!< Vector Table base offset field. \
								  This value must be a multiple of 0x200. */
/******************************************************************************/

/* This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
uint32_t SystemCoreClock = 72000000U;

/**
  * @brief  Setup the microcontroller system
  *         Initialize the FPU setting, vector table location and External memory
  *         configuration.
  * @param  None
  * @retval None
  */
void SystemInit(void) {
	/* Set configured startup clk source */
    BIT_BAND_SET(RCC->CR, RCC_CR_HSION_Pos);

	/* Reset CFGR register */
	RCC->CFGR &= 0xF8FF0000U;

	/* Reset HSxON, CSSON and PLLON bits */
	RCC->CR &= ~(MICROPY_HW_RCC_CR_HSxON | RCC_CR_CSSON | RCC_CR_PLLON);

	/* Reset HSEBYP bit */
	RCC->CR &= 0xFFFBFFFFU;

	/* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
	RCC->CFGR &= 0xFF80FFFFU;

	/* Disable all interrupts */
	RCC->CIR = 0x009F0000U;

	/* Configure the Vector Table location add offset address ------------------*/
#ifdef MICROPY_HW_VTOR
	SCB->VTOR = MICROPY_HW_VTOR;
#elif defined(VECT_TAB_SRAM)
	SCB->VTOR = SRAM1_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
	SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif

#ifdef DATA_IN_ExtSRAM
    SystemInit_ExtMemCtl();
#endif

	/* dpgeorge: enable 8-byte stack alignment for IRQ handlers, in accord with EABI */
	SCB->CCR |= SCB_CCR_STKALIGN_Msk;
}

/**
  * @brief  System Clock Configuration
  *
  * Output clocks:
  *
  * CPU             SYSCLK      max 72MHz
  * USB,SDIO        PLL48CK     must be 48MHz for USB
  * AHB             HCLK        max 72MHz
  * APB1            PCLK1       max 36MHz
  * APB2            PCLK2       max 72MHz
  *
  * Timers run from APBx if APBx_PRESC=1, else 2x APBx
  */
void SystemClock_Config(void) {
	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	RCC_ClkInitTypeDef RCC_ClkInitStruct = {
		.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2),
		.AHBCLKDivider = RCC_SYSCLK_DIV1,
		.APB1CLKDivider = RCC_HCLK_DIV2,
		.APB2CLKDivider = RCC_HCLK_DIV1,
	};

	RCC_OscInitTypeDef RCC_OscInitStruct = {
		.OscillatorType = MICROPY_HW_RCC_OSCILLATOR_TYPE,
		.HSEState = MICROPY_HW_RCC_HSE_STATE,
		.HSEPredivValue = MICROPY_HW_HSE_PREDIV,
		.HSIState = MICROPY_HW_RCC_HSI_STATE,
		.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
		.PLL.PLLSource = MICROPY_HW_RCC_PLL_SRC,
		.PLL.PLLState = RCC_PLL_ON,
		.PLL.PLLMUL = MICROPY_HW_CLK_PLLMUL,
	};

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		__fatal_error("HAL_RCC_OscConfig");
	}

	if (powerctrl_rcc_clock_config_pll(&RCC_ClkInitStruct) != 0) {
		__fatal_error("HAL_RCC_ClockConfig");
	}
}

#ifdef DATA_IN_ExtSRAM
#define MPY_GPIO_LOCK_PIN(GPIOx, PinIdx, tmp) do{ \
        tmp = (GPIO_LCKR_LCKK | (PinIdx)); \
        GPIOx->LCKR = tmp;                 \
        GPIOx->LCKR = (PinIdx);            \
        GPIOx->LCKR = tmp;                 \
        tmp = GPIOx->LCKR;                 \
        (void)tmp;                         \
    }while(0)

void SystemInit_ExtMemCtl(void) {
    __IO uint32_t tmpreg;
    // __IO uint32_t tmp = GPIO_LCKR_LCKK;

    /* Enable FSMC clock */
    RCC->AHBENR = 0x00000114U;

    /* Delay after an RCC peripheral clock enabling */
    tmpreg = READ_BIT(RCC->AHBENR, RCC_AHBENR_FSMCEN);

    /* Enable GPIOD, GPIOE, GPIOF and GPIOG clocks */
    RCC->APB2ENR = 0x000001E0U;

    /* Delay after an RCC peripheral clock enabling */
    tmpreg = READ_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPDEN);
    (void)(tmpreg);

    /* ---------------  SRAM Data lines, NOE and NWE configuration ---------------*/
    /*----------------  SRAM Address lines configuration -------------------------*/
    /*----------------  NOE and NWE configuration --------------------------------*/
    /*----------------  NE3 configuration ----------------------------------------*/
    /*----------------  NBL0, NBL1 configuration ---------------------------------*/

    GPIOD->CRL = 0x44BB44BBU; /* PD0, PD1, PD4, PD5*/
    GPIOD->CRH = 0xBBBBBBBBU; /* PD8 ~ PD15 */
    // MPY_GPIO_LOCK_PIN(GPIOD, 0xFF33U, tmp);

    GPIOE->CRL = 0xB44444BBU; /* PE0, PE1, PE7 */
    GPIOE->CRH = 0xBBBBBBBBU; /* PE8 ~ PE15 */
    // MPY_GPIO_LOCK_PIN(GPIOE, 0xFF83U, tmp);

    GPIOF->CRL = 0x44BBBBBBU; /* PF0 ~ PF5 */
    GPIOF->CRH = 0xBBBB4444U; /* PF12 ~ PF15 */
    // MPY_GPIO_LOCK_PIN(GPIOF, 0xF03F, tmp);

    GPIOG->CRL = 0x44BBBBBBU; /* PG0 ~ PG5  */
    GPIOG->CRH = 0x44444B44U; /* PG10 (NE3) */
    // MPY_GPIO_LOCK_PIN(GPIOG, 0x043F, tmp);

    /*----------------  Enable FSMC Bank1_3 (for NE3) Bank (0x6800 0000)-----------------------*/
    FSMC_Bank1->BTCR[4U] = 0x00001091U; /* control register, [ WREN(b12), MWID(b4~b5), MBKEN(b0)]*/
    FSMC_Bank1->BTCR[5U] = 0x00110212U; /* timing register, [CLKDIV=1, BUSTURN=1, DATAST=2, ADDHLD=1, ADDSET=2] */
}
#endif