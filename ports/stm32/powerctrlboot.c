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
#include "irq.h"
#include "powerctrl.h"

#if defined(STM32WB)
void stm32_system_init(void) {
    if (RCC->CR == 0x00000560 && RCC->CFGR == 0x00070005) {
        // Wake from STANDBY with HSI enabled as system clock.  The second core likely
        // also needs HSI to remain enabled, so do as little as possible here.
        #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
        // set CP10 and CP11 Full Access.
        SCB->CPACR |= (3 << (10 * 2)) | (3 << (11 * 2));
        #endif
        // Disable all interrupts.
        RCC->CIER = 0x00000000;
    } else {
        // Other start-up (eg POR), use standard system init code.
        SystemInit();
    }
}
#endif

void powerctrl_config_systick(void) {
    #if defined(STM32N6)
    uint32_t systick_source_freq = HAL_RCC_GetCpuClockFreq();
    #else
    uint32_t systick_source_freq = HAL_RCC_GetHCLKFreq();
    #endif

    // Configure SYSTICK to run at 1kHz (1ms interval)
    SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
    SysTick_Config(systick_source_freq / 1000);
    NVIC_SetPriority(SysTick_IRQn, IRQ_PRI_SYSTICK);

    #if !BUILDING_MBOOT && (defined(STM32H5) || defined(STM32H7) || defined(STM32L4) || defined(STM32WB))
    // Set SysTick IRQ priority variable in case the HAL needs to use it
    uwTickPrio = IRQ_PRI_SYSTICK;
    #endif
}

#if defined(STM32F0)

void SystemClock_Config(void) {
    // Enable power control peripheral
    __HAL_RCC_PWR_CLK_ENABLE();

    // Set flash latency to 1 because SYSCLK > 24MHz
    FLASH->ACR = (FLASH->ACR & ~0x7) | 0x1;

    #if MICROPY_HW_CLK_USE_HSI48
    // Use the 48MHz internal oscillator
    // HAL does not support RCC CFGR SW=3 (HSI48 direct to SYSCLK)
    // so use HSI48 -> PREDIV(divide by 2) -> PLL (mult by 2) -> SYSCLK.

    RCC->CR2 |= RCC_CR2_HSI48ON;
    while ((RCC->CR2 & RCC_CR2_HSI48RDY) == 0) {
        // Wait for HSI48 to be ready
    }
    RCC->CFGR = 0 << RCC_CFGR_PLLMUL_Pos | 3 << RCC_CFGR_PLLSRC_Pos; // PLL mult by 2, src = HSI48/PREDIV
    RCC->CFGR2 = 1; // Input clock divided by 2

    #elif MICROPY_HW_CLK_USE_HSE
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

    #elif MICROPY_HW_CLK_USE_HSI
    // Use the 8MHz internal oscillator and the PLL to get a 48MHz SYSCLK

    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {
        // Wait for HSI to be ready
    }
    RCC->CFGR = 4 << RCC_CFGR_PLLMUL_Pos | 1 << RCC_CFGR_PLLSRC_Pos; // PLL mult by 6, src = HSI
    RCC->CFGR2 = 0; // Input clock not divided

    #else
    #error System clock not specified
    #endif

    RCC->CR |= RCC_CR_PLLON; // Turn PLL on
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
        // Wait for PLL to lock
    }
    const uint32_t sysclk_src = 2;

    // Select SYSCLK source
    RCC->CFGR |= sysclk_src << RCC_CFGR_SW_Pos;
    while (((RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x3) != sysclk_src) {
        // Wait for SYSCLK source to change
    }

    SystemCoreClockUpdate();
    powerctrl_config_systick();
}

#elif defined(STM32G0)

void SystemClock_Config(void) {
    // Enable power control peripheral
    __HAL_RCC_PWR_CLK_ENABLE();

    // Set flash latency to 2 because SYSCLK > 48MHz
    FLASH->ACR = (FLASH->ACR & ~0x7) | 0x2;

    #if MICROPY_HW_CLK_USE_HSI
    // Enable the 16MHz internal oscillator and the PLL to get a 64MHz SYSCLK
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {
        // Wait for HSI to be ready
    }

    // Use the PLL to get a 64MHz SYSCLK
    #define PLLM (HSI_VALUE / 16000000) // input is 8MHz
    #define PLLN (8) // 8*16MHz = 128MHz
    #define PLLP (2) // f_P = 64MHz
    #define PLLQ (2) // f_Q = 64MHz
    #define PLLR (2) // f_R = 64MHz
    RCC->PLLCFGR =
        (PLLP - 1) << RCC_PLLCFGR_PLLP_Pos | RCC_PLLCFGR_PLLPEN
            | (PLLQ - 1) << RCC_PLLCFGR_PLLQ_Pos | RCC_PLLCFGR_PLLQEN
            | (PLLR - 1) << RCC_PLLCFGR_PLLR_Pos | RCC_PLLCFGR_PLLREN
            | PLLN << RCC_PLLCFGR_PLLN_Pos
            | (PLLM - 1) << RCC_PLLCFGR_PLLM_Pos
            | RCC_PLLCFGR_PLLSRC_HSI;

    #else
    #error System clock not specified
    #endif

    RCC->CR |= RCC_CR_PLLON; // Turn PLL on
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
        // Wait for PLL to lock
    }
    const uint32_t sysclk_src = 2; // 2 = PLLRCLK

    // Select SYSCLK source
    RCC->CFGR |= sysclk_src << RCC_CFGR_SW_Pos;
    while (((RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x7) != sysclk_src) {
        // Wait for SYSCLK source to change
    }

    SystemCoreClockUpdate();
    powerctrl_config_systick();

    #if MICROPY_HW_ENABLE_RNG || MICROPY_HW_ENABLE_USB
    // Enable the 48MHz internal oscillator
    RCC->CR |= RCC_CR_HSI48ON;
    RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;
    while (!(RCC->CR & RCC_CR_HSI48RDY)) {
        // Wait for HSI48 to be ready
    }

    // Select HSI48 for USB
    RCC->CCIPR2 &= ~(3 << RCC_CCIPR2_USBSEL_Pos);

    #if MICROPY_HW_ENABLE_USB
    // Synchronise HSI48 with 1kHz USB SoF
    __HAL_RCC_CRS_CLK_ENABLE();
    CRS->CR = 0x20 << CRS_CR_TRIM_Pos;
    CRS->CFGR = 2 << CRS_CFGR_SYNCSRC_Pos | 0x22 << CRS_CFGR_FELIM_Pos
        | __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000) << CRS_CFGR_RELOAD_Pos;
    #endif
    #endif
}

#elif defined(STM32H5)

void SystemClock_Config(void) {
    // Set power voltage scaling.
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }

    #if MICROPY_HW_CLK_USE_HSI
    LL_RCC_HSI_Enable();
    while (!LL_RCC_HSI_IsReady()) {
    }
    const uint32_t pll1_source = LL_RCC_PLL1SOURCE_HSI;
    #else
    // Enable HSE.
    #if MICROPY_HW_CLK_USE_BYPASS
    LL_RCC_HSE_EnableBypass();
    #endif
    LL_RCC_HSE_Enable();
    while (!LL_RCC_HSE_IsReady()) {
    }
    const uint32_t pll1_source = LL_RCC_PLL1SOURCE_HSE;
    #endif

    // Configure PLL1 for use as system clock.
    LL_RCC_PLL1_ConfigDomain_SYS(pll1_source, MICROPY_HW_CLK_PLLM, MICROPY_HW_CLK_PLLN, MICROPY_HW_CLK_PLLP);
    LL_RCC_PLL1_SetFRACN(MICROPY_HW_CLK_PLLFRAC);
    LL_RCC_PLL1_SetVCOInputRange(MICROPY_HW_CLK_PLLVCI_LL);
    LL_RCC_PLL1_SetVCOOutputRange(MICROPY_HW_CLK_PLLVCO_LL);
    LL_RCC_PLL1P_Enable();

    #if defined(MICROPY_HW_CLK_PLLQ)
    LL_RCC_PLL1_SetQ(MICROPY_HW_CLK_PLLQ);
    LL_RCC_PLL1Q_Enable();
    #endif

    #if defined(MICROPY_HW_CLK_PLLR)
    LL_RCC_PLL1_SetR(MICROPY_HW_CLK_PLLR);
    LL_RCC_PLL1R_Enable();
    #endif

    // Enable PLL1.
    LL_RCC_PLL1_Enable();
    while (!LL_RCC_PLL1_IsReady()) {
    }

    // Configure bus dividers.
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

    // Configure the flash latency before switching the system clock source.
    __HAL_FLASH_SET_LATENCY(MICROPY_HW_FLASH_LATENCY);
    while (__HAL_FLASH_GET_LATENCY() != MICROPY_HW_FLASH_LATENCY) {
    }

    // Switch the system clock source to PLL1P.
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1) {
    }

    // Reconfigure clock state and SysTick.
    SystemCoreClockUpdate();
    powerctrl_config_systick();

    // USB clock configuration, either HSI48 or PLL3.
    #if MICROPY_HW_ENABLE_USB && !MICROPY_HW_CLK_USE_PLL3_FOR_USB

    // Enable HSI48.
    LL_RCC_HSI48_Enable();
    while (!LL_RCC_HSI48_IsReady()) {
    }

    // Select HSI48 for USB clock source
    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_HSI48);

    // Synchronise HSI48 with 1kHz USB SoF
    __HAL_RCC_CRS_CLK_ENABLE();
    CRS->CFGR = 2 << CRS_CFGR_SYNCSRC_Pos | 0x22 << CRS_CFGR_FELIM_Pos
        | __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000) << CRS_CFGR_RELOAD_Pos;
    CRS->CR = 0x20 << CRS_CR_TRIM_Pos | CRS_CR_AUTOTRIMEN | CRS_CR_CEN;

    #elif MICROPY_HW_ENABLE_USB && MICROPY_HW_CLK_USE_PLL3_FOR_USB

    // Configure PLL3 for use by USB at Q=48MHz.
    LL_RCC_PLL3_SetSource(LL_RCC_PLL3SOURCE_HSE);
    LL_RCC_PLL3_SetM(MICROPY_HW_CLK_PLL3M);
    LL_RCC_PLL3_SetN(MICROPY_HW_CLK_PLL3N);
    LL_RCC_PLL3_SetP(MICROPY_HW_CLK_PLL3P);
    LL_RCC_PLL3_SetQ(MICROPY_HW_CLK_PLL3Q);
    LL_RCC_PLL3_SetR(MICROPY_HW_CLK_PLL3R);
    LL_RCC_PLL3_SetFRACN(MICROPY_HW_CLK_PLL3FRAC);
    LL_RCC_PLL3_SetVCOInputRange(MICROPY_HW_CLK_PLL3VCI_LL);
    LL_RCC_PLL3_SetVCOOutputRange(MICROPY_HW_CLK_PLL3VCO_LL);
    LL_RCC_PLL3Q_Enable();

    // Enable PLL3.
    LL_RCC_PLL3_Enable();
    while (!LL_RCC_PLL3_IsReady()) {
    }

    // Select PLL3-Q for USB clock source
    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL3Q);

    #endif

    #ifdef NDEBUG
    DBGMCU->CR = 0;
    #endif
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
    powerctrl_config_systick();

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

#elif defined(STM32L1)

void SystemClock_Config(void) {
    // Enable power control peripheral
    __HAL_RCC_PWR_CLK_ENABLE();

    // Set power voltage scaling
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Enable the FLASH 64-bit access
    FLASH->ACR = FLASH_ACR_ACC64;
    // Set flash latency to 1 because SYSCLK > 16MHz
    FLASH->ACR |= MICROPY_HW_FLASH_LATENCY;

    #if MICROPY_HW_CLK_USE_HSI
    // Enable the 16MHz internal oscillator
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {
    }
    RCC->CFGR = RCC_CFGR_PLLSRC_HSI;
    #else
    // Enable the 8MHz external oscillator
    RCC->CR |= RCC_CR_HSEBYP;
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {
    }
    RCC->CFGR = RCC_CFGR_PLLSRC_HSE;
    #endif
    // Use HSI16 and the PLL to get a 32MHz SYSCLK
    RCC->CFGR |= MICROPY_HW_CLK_PLLMUL | MICROPY_HW_CLK_PLLDIV;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {
        // Wait for PLL to lock
    }
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL) {
        // Wait for SYSCLK source to change
    }

    SystemCoreClockUpdate();
    powerctrl_config_systick();

    #if MICROPY_HW_ENABLE_USB
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    #endif

    // Disable the Debug Module in low-power mode due to prevent
    // unexpected HardFault after __WFI().
    #if !defined(NDEBUG)
    DBGMCU->CR &= ~(DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STANDBY);
    #endif
}

#elif defined(STM32N6)

void SystemClock_Config(void) {
    // Enable HSI.
    LL_RCC_HSI_Enable();
    while (!LL_RCC_HSI_IsReady()) {
    }

    // Switch the CPU clock source to HSI.
    LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_HSI);
    while (LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_HSI) {
    }

    // Switch the system clock source to HSI.
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {
    }

    // Disable all ICx clocks.
    RCC->DIVENCR = 0x000fffff;

    // This doesn't work, VOSRDY never becomes active.
    #if 0
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
    while (!LL_PWR_IsActiveFlag_VOSRDY()) {
    }
    #endif

    // Enable HSE.
    LL_RCC_HSE_Enable();
    while (!LL_RCC_HSE_IsReady()) {
    }

    // Disable PLL1.
    LL_RCC_PLL1_Disable();
    while (LL_RCC_PLL1_IsReady()) {
    }

    // Configure PLL1 for use as system clock.
    LL_RCC_PLL1_SetSource(LL_RCC_PLLSOURCE_HSE);
    LL_RCC_PLL1_DisableBypass();
    LL_RCC_PLL1_DisableFractionalModulationSpreadSpectrum();
    LL_RCC_PLL1_SetM(MICROPY_HW_CLK_PLLM);
    LL_RCC_PLL1_SetN(MICROPY_HW_CLK_PLLN);
    LL_RCC_PLL1_SetP1(MICROPY_HW_CLK_PLLP1);
    LL_RCC_PLL1_SetP2(MICROPY_HW_CLK_PLLP2);
    LL_RCC_PLL1_SetFRACN(MICROPY_HW_CLK_PLLFRAC);
    LL_RCC_PLL1P_Enable();

    // Enable PLL1.
    LL_RCC_PLL1_Enable();
    while (!LL_RCC_PLL1_IsReady()) {
    }

    // Configure IC1, IC2, IC6, IC11.
    LL_RCC_IC1_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC1_SetDivider(1);
    LL_RCC_IC1_Enable();
    LL_RCC_IC2_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC2_SetDivider(2);
    LL_RCC_IC2_Enable();
    LL_RCC_IC6_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC6_SetDivider(1);
    LL_RCC_IC6_Enable();
    LL_RCC_IC11_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC11_SetDivider(1);
    LL_RCC_IC11_Enable();

    // Configure IC14 at 100MHz for slower peripherals.
    LL_RCC_IC14_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC14_SetDivider(8);
    LL_RCC_IC14_Enable();

    // Enable buses.
    LL_BUS_EnableClock(LL_APB5 | LL_APB4 | LL_APB3 | LL_APB2 | LL_APB1 | LL_AHB5 | LL_AHB4 | LL_AHB3 | LL_AHB2 | LL_AHB1);
    LL_MISC_EnableClock(LL_PER);

    // Configure bus dividers.
    LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_1);
    LL_RCC_SetAPB5Prescaler(LL_RCC_APB5_DIV_1);

    // Switch the CPU clock source to IC1 (connected to PLL1).
    LL_RCC_SetCpuClkSource(LL_RCC_CPU_CLKSOURCE_IC1);
    while (LL_RCC_GetCpuClkSource() != LL_RCC_CPU_CLKSOURCE_STATUS_IC1) {
    }

    // Switch the system clock source to IC2/IC6/IC11 (connected to PLL1).
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_IC2_IC6_IC11);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_IC2_IC6_IC11) {
    }

    // ADC clock configuration, HCLK/2.
    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_HCLK);
    LL_RCC_SetADCPrescaler(2 - 1);

    // USB clock configuration.
    #if MICROPY_HW_ENABLE_USB

    // Select HSE/2 as output of direct HSE signal.
    LL_RCC_HSE_SelectHSEDiv2AsDiv2Clock();

    // Select HSE/2 for OTG1 clock source.
    LL_RCC_SetClockSource(LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC);
    LL_RCC_SetClockSource(LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC);
    LL_RCC_SetOTGPHYClockSource(LL_RCC_OTGPHY1_CLKSOURCE_HSE_DIV_2_OSC);
    LL_RCC_SetOTGPHYCKREFClockSource(LL_RCC_OTGPHY1CKREF_CLKSOURCE_HSE_DIV_2_OSC);

    #endif

    // Reconfigure clock state and SysTick.
    SystemCoreClockUpdate();
    powerctrl_config_systick();
}

#elif defined(STM32WB)

void SystemClock_Config(void) {
    while (LL_HSEM_1StepLock(HSEM, CFG_HW_RCC_SEMID)) {
    }

    // Enable the 32MHz external oscillator
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {
    }

    // Prevent CPU2 from disabling CLK48.
    // This semaphore protected access to the CLK48 configuration.
    // CPU1 should hold this semaphore while the USB peripheral is in use.
    // See AN5289 and https://github.com/micropython/micropython/issues/6316.
    while (LL_HSEM_1StepLock(HSEM, CFG_HW_CLK48_CONFIG_SEMID)) {
    }

    // Use HSE and the PLL to get a 64MHz SYSCLK
    #define PLLM (HSE_VALUE / 8000000) // VCO input is 8MHz
    #define PLLN (24) // 24*8MHz = 192MHz
    #define PLLQ (4) // f_Q = 48MHz
    #define PLLR (3) // f_R = 64MHz
    RCC->PLLCFGR =
        (PLLR - 1) << RCC_PLLCFGR_PLLR_Pos | RCC_PLLCFGR_PLLREN
            | (PLLQ - 1) << RCC_PLLCFGR_PLLQ_Pos | RCC_PLLCFGR_PLLQEN
            | PLLN << RCC_PLLCFGR_PLLN_Pos
            | (PLLM - 1) << RCC_PLLCFGR_PLLM_Pos
            | 3 << RCC_PLLCFGR_PLLSRC_Pos;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {
        // Wait for PLL to lock
    }
    const uint32_t sysclk_src = 3;

    // Set divider for HCLK2 to 2 so f_HCLK2 = 32MHz
    RCC->EXTCFGR = 8 << RCC_EXTCFGR_C2HPRE_Pos;

    // Set flash latency to 3 because SYSCLK > 54MHz
    FLASH->ACR |= 3 << FLASH_ACR_LATENCY_Pos;

    // Select SYSCLK source
    RCC->CFGR |= sysclk_src << RCC_CFGR_SW_Pos;
    while (((RCC->CFGR >> RCC_CFGR_SWS_Pos) & 0x3) != sysclk_src) {
        // Wait for SYSCLK source to change
    }

    // Select PLLQ as 48MHz source for USB and RNG
    RCC->CCIPR = 2 << RCC_CCIPR_CLK48SEL_Pos;

    SystemCoreClockUpdate();
    powerctrl_config_systick();

    // Release RCC semaphore
    LL_HSEM_ReleaseLock(HSEM, CFG_HW_RCC_SEMID, 0);
}

#elif defined(STM32WL)

#include "stm32wlxx_ll_utils.h"

void SystemClock_Config(void) {
    // Set flash latency (2 wait states, sysclk > 36MHz)
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2) {
    }

    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    #if MICROPY_HW_CLK_USE_HSE
    // Enable the 32MHz external oscillator and 48MHZ SYSCLK via PLL

    #if MICROPY_HW_CLK_USE_BYPASS
    // Use "bypass power" option, port PB0_VDDTCXO supplies TCXO
    // (STM32WL5x has no other HSE bypass mode.)

    // "PB0 must be configured in analog mode prior enabling the HSE"
    //
    // Note: PB0 analog mode muxes PB0_VDDTCXO pin to the VDDTCXO regulator, set
    // to default voltage of 1.7V.  Changing this voltage requires initializing
    // the SUBGHZ radio and sending a Set_Tcxo command to it.
    //
    // For the Nucelo-WL55 board, ST uses the NDK "NT2016SF-32M-END5875A" TCXO
    // which has no publicly available datasheet. However, the ST code for this
    // board always keeps the pin at the default 1.7V voltage level so changing
    // the level would only be needed if a different TCXO is used.
    //
    // (Note also that setting pin PB0 as a push-pull GPIO output is technically
    // possible too, but 3.3V will be too high for many TCXOs.)
    mp_hal_pin_config(pin_B0, MP_HAL_PIN_MODE_ANALOG, MP_HAL_PIN_PULL_NONE, 0);

    LL_RCC_HSE_EnableTcxo();

    #endif // MICROPY_HW_CLK_USE_BYPASS

    LL_RCC_HSE_Enable();
    while (!LL_RCC_HSE_IsReady()) {
        // Wait for HSE Ready signal
    }

    // Configure PLL for a 48MHz SYSCLK
    #define PLLM (HSE_VALUE / 16000000) // VCO input 16MHz (recommended in ST docs)
    #define PLLN (6) // 7*8MHz = 96MHz
    #define PLLP (2) // f_P = 48MHz
    #define PLLQ (2) // f_Q = 48MHz
    #define PLLR (2) // f_R = 48MHz
    RCC->PLLCFGR =
        (PLLR - 1) << RCC_PLLCFGR_PLLR_Pos | RCC_PLLCFGR_PLLREN
            | (PLLQ - 1) << RCC_PLLCFGR_PLLQ_Pos | RCC_PLLCFGR_PLLQEN
            | (PLLP - 1) << RCC_PLLCFGR_PLLP_Pos | RCC_PLLCFGR_PLLPEN
            | PLLN << RCC_PLLCFGR_PLLN_Pos
            | (PLLM - 1) << RCC_PLLCFGR_PLLM_Pos
            | LL_RCC_PLLSOURCE_HSE;

    LL_RCC_PLL_Enable();
    LL_RCC_PLL_EnableDomain_SYS();
    while (!LL_RCC_PLL_IsReady()) {
        // Wait for PLL to lock
    }

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
        // Wait for system clock source to switch
    }

    #else // Use MSI as 48MHz source for SYSCLK

    // Enable MSI
    LL_RCC_MSI_Enable();
    while (!LL_RCC_MSI_IsReady()) {
    }

    // Configure MSI
    LL_RCC_MSI_EnableRangeSelection();
    LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_11);
    LL_RCC_MSI_SetCalibTrimming(0);

    // Select SYSCLK source
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_MSI);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_MSI) {
    }

    #endif // MICROPY_HW_CLK_USE_HSE

    // Set bus dividers
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAHB3Prescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    SystemCoreClockUpdate();
    powerctrl_config_systick();
}

#endif
