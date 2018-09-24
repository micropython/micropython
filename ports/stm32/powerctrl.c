/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
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

#include "py/mperrno.h"
#include "py/mphal.h"
#include "powerctrl.h"
#include "genhdr/pllfreqtable.h"

#if !(defined(STM32F0) || defined(STM32L4))

STATIC uint32_t calc_ahb_div(uint32_t wanted_div) {
    if (wanted_div <= 1) { return RCC_SYSCLK_DIV1; }
    else if (wanted_div <= 2) { return RCC_SYSCLK_DIV2; }
    else if (wanted_div <= 4) { return RCC_SYSCLK_DIV4; }
    else if (wanted_div <= 8) { return RCC_SYSCLK_DIV8; }
    else if (wanted_div <= 16) { return RCC_SYSCLK_DIV16; }
    else if (wanted_div <= 64) { return RCC_SYSCLK_DIV64; }
    else if (wanted_div <= 128) { return RCC_SYSCLK_DIV128; }
    else if (wanted_div <= 256) { return RCC_SYSCLK_DIV256; }
    else { return RCC_SYSCLK_DIV512; }
}

STATIC uint32_t calc_apb_div(uint32_t wanted_div) {
    if (wanted_div <= 1) { return RCC_HCLK_DIV1; }
    else if (wanted_div <= 2) { return RCC_HCLK_DIV2; }
    else if (wanted_div <= 4) { return RCC_HCLK_DIV4; }
    else if (wanted_div <= 8) { return RCC_HCLK_DIV8; }
    else { return RCC_SYSCLK_DIV16; }
}

int powerctrl_set_sysclk(uint32_t sysclk, uint32_t ahb, uint32_t apb1, uint32_t apb2) {
    // Default PLL parameters that give 48MHz on PLL48CK
    uint32_t m = HSE_VALUE / 1000000, n = 336, p = 2, q = 7;
    uint32_t sysclk_source;
    #if defined(STM32F7)
    bool need_pllsai = false;
    #endif

    // Search for a valid PLL configuration that keeps USB at 48MHz
    uint32_t sysclk_mhz = sysclk / 1000000;
    for (const uint16_t *pll = &pll_freq_table[MP_ARRAY_SIZE(pll_freq_table) - 1]; pll >= &pll_freq_table[0]; --pll) {
        uint32_t sys = *pll & 0xff;
        if (sys <= sysclk_mhz) {
            m = (*pll >> 10) & 0x3f;
            p = ((*pll >> 7) & 0x6) + 2;
            if (m == 0) {
                // special entry for using HSI directly
                sysclk_source = RCC_SYSCLKSOURCE_HSI;
            } else if (m == 1) {
                // special entry for using HSE directly
                sysclk_source = RCC_SYSCLKSOURCE_HSE;
            } else {
                // use PLL
                sysclk_source = RCC_SYSCLKSOURCE_PLLCLK;
                uint32_t vco_out = sys * p;
                n = vco_out * m / (HSE_VALUE / 1000000);
                q = vco_out / 48;
                #if defined(STM32F7)
                need_pllsai = vco_out % 48 != 0;
                #endif
            }
            goto set_clk;
        }
    }
    return -MP_EINVAL;

set_clk:
    // Let the USB CDC have a chance to process before we change the clock
    mp_hal_delay_ms(5);

    // Desired system clock source is in sysclk_source
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    if (sysclk_source == RCC_SYSCLKSOURCE_PLLCLK) {
        // Set HSE as system clock source to allow modification of the PLL configuration
        // We then change to PLL after re-configuring PLL
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    } else {
        // Directly set the system clock source as desired
        RCC_ClkInitStruct.SYSCLKSource = sysclk_source;
    }

    // Determine the bus clock dividers
    if (ahb != 0) {
        // Note: AHB freq required to be >= 14.2MHz for USB operation
        RCC_ClkInitStruct.AHBCLKDivider = calc_ahb_div(sysclk / ahb);
    } else {
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    }
    if (apb1 != 0) {
        RCC_ClkInitStruct.APB1CLKDivider = calc_apb_div(sysclk / apb1);
    } else {
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    }
    if (apb2 != 0) {
        RCC_ClkInitStruct.APB2CLKDivider = calc_apb_div(sysclk / apb2);
    } else {
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    }

    #if MICROPY_HW_CLK_LAST_FREQ
    // Save the bus dividers for use later
    uint32_t h = RCC_ClkInitStruct.AHBCLKDivider >> 4;
    uint32_t b1 = RCC_ClkInitStruct.APB1CLKDivider >> 10;
    uint32_t b2 = RCC_ClkInitStruct.APB2CLKDivider >> 10;
    #endif

    // Configure clock
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        return -MP_EIO;
    }

    #if defined(STM32F7)
    // Turn PLLSAI off because we are changing PLLM (which drives PLLSAI)
    RCC->CR &= ~RCC_CR_PLLSAION;
    #endif

    // Re-configure PLL
    // Even if we don't use the PLL for the system clock, we still need it for USB, RNG and SDIO
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = MICROPY_HW_CLK_HSE_STATE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = m;
    RCC_OscInitStruct.PLL.PLLN = n;
    RCC_OscInitStruct.PLL.PLLP = p;
    RCC_OscInitStruct.PLL.PLLQ = q;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        return -MP_EIO;
    }

    #if defined(STM32F7)
    if (need_pllsai) {
        // Configure PLLSAI at 48MHz for those peripherals that need this freq
        const uint32_t pllsain = 192;
        const uint32_t pllsaip = 4;
        const uint32_t pllsaiq = 2;
        RCC->PLLSAICFGR = pllsaiq << RCC_PLLSAICFGR_PLLSAIQ_Pos
            | (pllsaip / 2 - 1) << RCC_PLLSAICFGR_PLLSAIP_Pos
            | pllsain << RCC_PLLSAICFGR_PLLSAIN_Pos;
        RCC->CR |= RCC_CR_PLLSAION;
        uint32_t ticks = mp_hal_ticks_ms();
        while (!(RCC->CR & RCC_CR_PLLSAIRDY)) {
            if (mp_hal_ticks_ms() - ticks > 200) {
                return -MP_ETIMEDOUT;
            }
        }
        RCC->DCKCFGR2 |= RCC_DCKCFGR2_CK48MSEL;
    } else {
        RCC->DCKCFGR2 &= ~RCC_DCKCFGR2_CK48MSEL;
    }
    #endif

    // Set PLL as system clock source if wanted
    if (sysclk_source == RCC_SYSCLKSOURCE_PLLCLK) {
        uint32_t flash_latency;
        #if defined(STM32F7)
        // If possible, scale down the internal voltage regulator to save power
        // The flash_latency values assume a supply voltage between 2.7V and 3.6V
        uint32_t volt_scale;
        if (sysclk <= 90000000) {
            volt_scale = PWR_REGULATOR_VOLTAGE_SCALE3;
            flash_latency = FLASH_LATENCY_2;
        } else if (sysclk <= 120000000) {
            volt_scale = PWR_REGULATOR_VOLTAGE_SCALE3;
            flash_latency = FLASH_LATENCY_3;
        } else if (sysclk <= 144000000) {
            volt_scale = PWR_REGULATOR_VOLTAGE_SCALE3;
            flash_latency = FLASH_LATENCY_4;
        } else if (sysclk <= 180000000) {
            volt_scale = PWR_REGULATOR_VOLTAGE_SCALE2;
            flash_latency = FLASH_LATENCY_5;
        } else if (sysclk <= 210000000) {
            volt_scale = PWR_REGULATOR_VOLTAGE_SCALE1;
            flash_latency = FLASH_LATENCY_6;
        } else {
            volt_scale = PWR_REGULATOR_VOLTAGE_SCALE1;
            flash_latency = FLASH_LATENCY_7;
        }
        if (HAL_PWREx_ControlVoltageScaling(volt_scale) != HAL_OK) {
            return -MP_EIO;
        }
        #endif

        #if !defined(STM32F7)
        #if !defined(MICROPY_HW_FLASH_LATENCY)
        #define MICROPY_HW_FLASH_LATENCY FLASH_LATENCY_5
        #endif
        flash_latency = MICROPY_HW_FLASH_LATENCY;
        #endif
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, flash_latency) != HAL_OK) {
            return -MP_EIO;
        }
    }

    #if MICROPY_HW_CLK_LAST_FREQ
    // Save settings in RTC backup register to reconfigure clocks on hard-reset
    #if defined(STM32F7)
    #define FREQ_BKP BKP31R
    #else
    #define FREQ_BKP BKP19R
    #endif
    // qqqqqqqq pppppppp nnnnnnnn nnmmmmmm
    // qqqqQQQQ ppppppPP nNNNNNNN NNMMMMMM
    // 222111HH HHQQQQPP nNNNNNNN NNMMMMMM
    p = (p / 2) - 1;
    RTC->FREQ_BKP = m
        | (n << 6) | (p << 16) | (q << 18)
        | (h << 22)
        | (b1 << 26)
        | (b2 << 29);
    #endif

    return 0;
}

#endif
