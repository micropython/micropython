/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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

#include "py/mphal.h"
#include "powerctrl.h"

#if defined(STM32F0)

void SystemClock_Config(void) {
    // Enable power control peripheral
    __HAL_RCC_PWR_CLK_ENABLE();

    // Set flash latency to 1 because SYSCLK > 24MHz
    FLASH->ACR = (FLASH->ACR & ~0x7) | 0x1;

    #if MICROPY_HW_CLK_USE_HSI48
    // Use the 48MHz internal oscillator

    RCC->CR2 |= RCC_CR2_HSI48ON;
    while ((RCC->CR2 & RCC_CR2_HSI48RDY) == 0) {
    }
    const uint32_t sysclk_src = 3;

    #else
    // Use HSE and the PLL to get a 48MHz SYSCLK

    #if MICROPY_HW_CLK_USE_BYPASS
    RCC->CR |= RCC_CR_HSEBYP;
    #endif
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0) {
        // Wait for HSE to be ready
    }
    RCC->CFGR = ((48000000 / HSE_VALUE) - 2) << RCC_CFGR_PLLMUL_Pos | 2 << RCC_CFGR_PLLSRC_Pos;
    RCC->CFGR2 = 0; // Input clock not divided
    RCC->CR |= RCC_CR_PLLON; // Turn PLL on
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
        // Wait for PLL to lock
    }
    const uint32_t sysclk_src = 2;

    #endif

    // Select SYSCLK source
    RCC->CFGR |= sysclk_src << RCC_CFGR_SW_Pos;
    while (((RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x3) != sysclk_src) {
        // Wait for SYSCLK source to change
    }

    SystemCoreClockUpdate();

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

#elif defined(STM32L0)

void SystemClock_Config(void) {
    // Enable power control peripheral
    __HAL_RCC_PWR_CLK_ENABLE();

    // Set flash latency to 1 because SYSCLK > 16MHz
    FLASH->ACR |= FLASH_ACR_LATENCY;

    // Enable the 16MHz internal oscillator
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {
    }

    // Use HSI16 and the PLL to get a 32MHz SYSCLK
    RCC->CFGR = 1 << RCC_CFGR_PLLDIV_Pos | 1 << RCC_CFGR_PLLMUL_Pos;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {
        // Wait for PLL to lock
    }
    const uint32_t sysclk_src = 3;

    // Select SYSCLK source
    RCC->CFGR |= sysclk_src << RCC_CFGR_SW_Pos;
    while (((RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x3) != sysclk_src) {
        // Wait for SYSCLK source to change
    }

    SystemCoreClockUpdate();

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    #if MICROPY_HW_ENABLE_RNG || MICROPY_HW_ENABLE_USB
    // Enable the 48MHz internal oscillator
    RCC->CRRCR |= RCC_CRRCR_HSI48ON;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->CFGR3 |= SYSCFG_CFGR3_ENREF_HSI48;
    while (!(RCC->CRRCR & RCC_CRRCR_HSI48RDY)) {
        // Wait for HSI48 to be ready
    }

    // Select RC48 as HSI48 for USB and RNG
    RCC->CCIPR |= RCC_CCIPR_HSI48SEL;

    #if MICROPY_HW_ENABLE_USB
    // Synchronise HSI48 with 1kHz USB SoF
    __HAL_RCC_CRS_CLK_ENABLE();
    CRS->CR = 0x20 << CRS_CR_TRIM_Pos;
    CRS->CFGR = 2 << CRS_CFGR_SYNCSRC_Pos | 0x22 << CRS_CFGR_FELIM_Pos
        | __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000) << CRS_CFGR_RELOAD_Pos;
    #endif
    #endif
}

#endif
