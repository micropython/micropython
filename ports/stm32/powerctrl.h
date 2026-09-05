/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_POWERCTRL_H
#define MICROPY_INCLUDED_STM32_POWERCTRL_H

#include <stdbool.h>
#include <stdint.h>

#if defined(STM32WB)
void stm32_system_init(void);
#else
static inline void stm32_system_init(void) {
    SystemInit();

    #if defined(STM32N6)
    // The ROM bootloader uses PLL1 to set the CPU to 400MHz, so update
    // the value of SystemCoreClock to reflect the hardware state.
    SystemCoreClockUpdate();
    #endif
}
#endif

int SystemClock_Config(void);

// Bounded spin-wait for clock/power flag transitions.  Timeout values
// should match the STM32 HAL for the relevant family: HSE 100ms, PLL 2ms,
// MSI 2ms, clock switch 5000ms, HSEM 500ms.
// Sets result to 1 on timeout, 0 on success.  Evaluates cond at most once
// per iteration (no post-loop re-evaluation) so it is safe with
// side-effecting conditions like LL_HSEM_1StepLock.
#define RCC_WAIT(cond, timeout_ms, result) \
    do { \
        uint32_t _t0 = mp_hal_ticks_ms(); \
        (result) = 1; \
        while (mp_hal_ticks_ms() - _t0 < (timeout_ms)) { \
            if (!(cond)) { (result) = 0; break; } \
        } \
    } while (0)

MP_NORETURN void powerctrl_mcu_reset(void);
MP_NORETURN void powerctrl_enter_bootloader(uint32_t r0, uint32_t bl_addr);
void powerctrl_check_enter_bootloader(void);

void powerctrl_config_systick(void);
int powerctrl_rcc_clock_config_pll(RCC_ClkInitTypeDef *rcc_init, uint32_t sysclk_mhz, bool need_pllsai);
int powerctrl_set_sysclk(uint32_t sysclk, uint32_t ahb, uint32_t apb1, uint32_t apb2);
void powerctrl_enter_stop_mode(void);
MP_NORETURN void powerctrl_enter_standby_mode(void);

#endif // MICROPY_INCLUDED_STM32_POWERCTRL_H
