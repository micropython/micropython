/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

static bool stm_timer_reserved[MP_ARRAY_SIZE(mcu_tim_banks)];
static bool stm_timer_never_reset[MP_ARRAY_SIZE(mcu_tim_banks)];
static void *stm_timer_callback[MP_ARRAY_SIZE(mcu_tim_banks)](void);

STATIC void tim_clock_enable(uint16_t mask);
STATIC void tim_clock_disable(uint16_t mask);

// Get the frequency (in Hz) of the source clock for the given timer.
// On STM32F405/407/415/417 there are 2 cases for how the clock freq is set.
// If the APB prescaler is 1, then the timer clock is equal to its respective
// APB clock.  Otherwise (APB prescaler > 1) the timer clock is twice its
// respective APB clock.  See DM00031020 Rev 4, page 115.
STATIC uint32_t timer_get_source_freq(uint32_t tim_id) {
    uint32_t source, clk_div;
    if (tim_id == 1 || (8 <= tim_id && tim_id <= 11)) {
        // TIM{1,8,9,10,11} are on APB2
        source = HAL_RCC_GetPCLK2Freq();
        clk_div = RCC->CFGR & RCC_CFGR_PPRE2;
    } else {
        // TIM{2,3,4,5,6,7,12,13,14} are on APB1
        source = HAL_RCC_GetPCLK1Freq();
        clk_div = RCC->CFGR & RCC_CFGR_PPRE1;
    }
    if (clk_div != 0) {
        // APB prescaler for this timer is > 1
        source *= 2;
    }
    return source;
}

TIM_TypeDef * stm_peripherals_find_timer(void) {
    // TODO: check for unreserved timers from already claimed pins

    // Work backwards - higher index timers have fewer pin allocations
    for (size_t i = MP_ARRAY_SIZE(stm_timer_reserved); i>=0; i--) {
        if ((!stm_timer_reserved[i]) && (mcu_tim_banks[i] != NULL)) {
            return mcu_tim_banks[i];
        }
    }
    mp_raise_RuntimeError(translate("All timers in use"));
    return NULL;
}

void stm_peripherals_timer_set_callback(void(*callback)(void), TIM_TypeDef * timer) {
    stm_timer_callback[stm_peripherals_timer_get_index]
}

void stm_peripherals_timer_free(TIM_TypeDef * instance) {

}
void stm_peripherals_timer_never_reset(TIM_TypeDef * instance);
void stm_peripherals_timer_reset_ok(TIM_TypeDef * instance);
void stm_peripherals_timer_is_never_reset(TIM_TypeDef * instance);
void stm_peripherals_timer_is_reserved(TIM_TypeDef * instance);
size_t stm_peripherals_timer_get_index(TIM_TypeDef * instance);

STATIC void tim_clock_enable(uint16_t mask) {
    #ifdef TIM1
    if (mask & (1 << 0)) {
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
    #endif
    #ifdef TIM2
    if (mask & (1 << 1)) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    #endif
    #ifdef TIM3
    if (mask & (1 << 2)) {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
    #endif
    #ifdef TIM4
    if (mask & (1 << 3)) {
        __HAL_RCC_TIM4_CLK_ENABLE();
    }
    #endif
    #ifdef TIM5
    if (mask & (1 << 4)) {
        __HAL_RCC_TIM5_CLK_ENABLE();
    }
    #endif
    //6 and 7 are reserved ADC timers
    #ifdef TIM8
    if (mask & (1 << 7)) {
        __HAL_RCC_TIM8_CLK_ENABLE();
    }
    #endif
    #ifdef TIM9
    if (mask & (1 << 8)) {
        __HAL_RCC_TIM9_CLK_ENABLE();
    }
    #endif
    #ifdef TIM10
    if (mask & (1 << 9)) {
        __HAL_RCC_TIM10_CLK_ENABLE();
    }
    #endif
    #ifdef TIM11
    if (mask & (1 << 10)) {
        __HAL_RCC_TIM11_CLK_ENABLE();
    }
    #endif
    #ifdef TIM12
    if (mask & (1 << 11)) {
        __HAL_RCC_TIM12_CLK_ENABLE();
    }
    #endif
    #ifdef TIM13
    if (mask & (1 << 12)) {
        __HAL_RCC_TIM13_CLK_ENABLE();
    }
    #endif
    #ifdef TIM14
    if (mask & (1 << 13)) {
        __HAL_RCC_TIM14_CLK_ENABLE();
    }
    #endif
}

STATIC void tim_clock_disable(uint16_t mask) {
    #ifdef TIM1
    if (mask & (1 << 0)) {
        __HAL_RCC_TIM1_CLK_DISABLE();
    }
    #endif
    #ifdef TIM2
    if (mask & (1 << 1)) {
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
    #endif
    #ifdef TIM3
    if (mask & (1 << 2)) {
        __HAL_RCC_TIM3_CLK_DISABLE();
    }
    #endif
    #ifdef TIM4
    if (mask & (1 << 3)) {
        __HAL_RCC_TIM4_CLK_DISABLE();
    }
    #endif
    #ifdef TIM5
    if (mask & (1 << 4)) {
        __HAL_RCC_TIM5_CLK_DISABLE();
    }
    #endif
    //6 and 7 are reserved ADC timers
    #ifdef TIM8
    if (mask & (1 << 7)) {
        __HAL_RCC_TIM8_CLK_DISABLE();
    }
    #endif
    #ifdef TIM9
    if (mask & (1 << 8)) {
        __HAL_RCC_TIM9_CLK_DISABLE();
    }
    #endif
    #ifdef TIM10
    if (mask & (1 << 9)) {
        __HAL_RCC_TIM10_CLK_DISABLE();
    }
    #endif
    #ifdef TIM11
    if (mask & (1 << 10)) {
        __HAL_RCC_TIM11_CLK_DISABLE();
    }
    #endif
    #ifdef TIM12
    if (mask & (1 << 11)) {
        __HAL_RCC_TIM12_CLK_DISABLE();
    }
    #endif
    #ifdef TIM13
    if (mask & (1 << 12)) {
        __HAL_RCC_TIM13_CLK_DISABLE();
    }
    #endif
    #ifdef TIM14
    if (mask & (1 << 13)) {
        __HAL_RCC_TIM14_CLK_DISABLE();
    }
    #endif
}

STATIC void callback_router(size_t index) {
    if (stm_timer_callback[index]) {
        (*stm_timer_callback[index])();
    }
}

void TIM1_CC_IRQHandler(void) {     // Advanced timer
    callback_router(1);
}
void TIM2_IRQHandler(void) {
    callback_router(2);
}
void TIM3_IRQHandler(void) {
    callback_router(3);
}
void TIM4_IRQHandler(void) {
    callback_router(4);
}
void TIM5_IRQHandler(void) {
    callback_router(5);
}
void TIM6_DAC_IRQHandler(void) {    // Basic timer (DAC)
    callback_router(6);
}
void TIM7_IRQHandler(void) {        // Basic timer
    callback_router(7);
}
void TIM8_CC_IRQHandler(void) {     // Advanced timer
    callback_router(8);
}

// Advanced timer interrupts are currently unused.
void TIM1_BRK_TIM9_IRQHandler(void) {
    callback_router(9);
}
void TIM1_UP_TIM10_IRQHandler(void) {
    callback_router(10);
}
void TIM1_TRG_COM_TIM11_IRQHandler(void) {
    callback_router(11);
}
void TIM8_BRK_TIM12_IRQHandler(void) {
    callback_router(12);
}
void TIM8_UP_TIM13_IRQHandler(void) {
    callback_router(13);
}
void TIM8_TRG_COM_TIM14_IRQHandler(void) {
    callback_router(14);
}

#if (CPY_STM32H7)
void TIM15_IRQHandler(void) {
    callback_router(15);
}
void TIM16_IRQHandler(void) {
    callback_router(16);
}
void TIM17_IRQHandler(void) {
    callback_router(17);
}
#endif
