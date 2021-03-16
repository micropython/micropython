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
#include "timers.h"

#include "py/mpconfig.h"
#include "py/gc.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#if !(CPY_STM32H7)

#define ALL_CLOCKS 0xFFFF
#define NULL_IRQ 0xFF

static bool stm_timer_reserved[MP_ARRAY_SIZE(mcu_tim_banks)];
static bool stm_timer_never_reset[MP_ARRAY_SIZE(mcu_tim_banks)];
static void (*stm_timer_callback[MP_ARRAY_SIZE (mcu_tim_banks)])(void);
    static size_t irq_map[] = {
    #ifdef TIM1
    TIM1_CC_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM2
    TIM2_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM3
    TIM3_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM4
    TIM4_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM5
    TIM5_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM6
    #if !defined(DAC_BASE) && !defined(DAC1_BASE)
    TIM6_IRQn,
    #else
    TIM6_DAC_IRQn,
    #endif
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM7
    TIM7_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM8
    TIM8_CC_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM9
    TIM1_BRK_TIM9_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM10
    TIM1_UP_TIM10_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM11
    TIM1_TRG_COM_TIM11_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM12
    TIM8_BRK_TIM12_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM13
    TIM8_UP_TIM13_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM14
    TIM8_TRG_COM_TIM14_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM15
    TIM15_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM16
    TIM16_IRQn,
    #else
    NULL_IRQ,
    #endif
    #ifdef TIM17
    TIM17_IRQn,
    #else
    NULL_IRQ,
    #endif
};

// Get the frequency (in Hz) of the source clock for the given timer.
// On STM32F405/407/415/417 there are 2 cases for how the clock freq is set.
// If the APB prescaler is 1, then the timer clock is equal to its respective
// APB clock.  Otherwise (APB prescaler > 1) the timer clock is twice its
// respective APB clock.  See DM00031020 Rev 4, page 115.
    uint32_t stm_peripherals_timer_get_source_freq(TIM_TypeDef *timer) {
    size_t tim_id = stm_peripherals_timer_get_index(timer);
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

    size_t stm_peripherals_timer_get_irqnum(TIM_TypeDef *instance) {
    size_t tim_id = stm_peripherals_timer_get_index(instance);
    return irq_map[tim_id];
}

    void timers_reset(void) {
    uint16_t never_reset_mask = 0x00;
    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_tim_banks); i++) {
        if (!stm_timer_never_reset[i]) {
            stm_timer_reserved[i] = false;
        } else {
            never_reset_mask |= 1 << i;
        }
    }
    tim_clock_disable(ALL_CLOCKS & ~(never_reset_mask));
}

    TIM_TypeDef *stm_peripherals_find_timer(void) {
    // Check for timers on pins outside the package size
    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_tim_banks); i++) {
        bool timer_in_package = false;
        // Find each timer instance on the given bank
        for (size_t j = 0; j < MP_ARRAY_SIZE(mcu_tim_pin_list); j++) {
            // If a pin is claimed, we skip it
            if ((mcu_tim_pin_list[j].tim_index == i + 1)
                && (common_hal_mcu_pin_is_free(mcu_tim_pin_list[j].pin) == true)) {
                // Search whether any pins in the package array match it
                for (size_t k = 0; k < mcu_pin_globals.map.alloc; k++) {
                    if ((mcu_tim_pin_list[j].pin == (mcu_pin_obj_t *)(mcu_pin_globals.map.table[k].value))) {
                        timer_in_package = true;
                    }
                }
            }
        }
        // If no results are found, no unclaimed pins with this timer are in this package,
        // and it is safe to pick
        if (timer_in_package == false && mcu_tim_banks[i] != NULL) {
            return mcu_tim_banks[i];
        }
    }
    // TODO: secondary search for timers outside the pins in the board profile

    // Work backwards - higher index timers have fewer pin allocations
    for (size_t i = (MP_ARRAY_SIZE(mcu_tim_banks) - 1); i >= 0; i--) {
        if ((!stm_timer_reserved[i]) && (mcu_tim_banks[i] != NULL)) {
            return mcu_tim_banks[i];
        }
    }
    mp_raise_RuntimeError(translate("All timers in use"));
    return NULL;
}

    void stm_peripherals_timer_preinit(TIM_TypeDef *instance, uint8_t prio, void (*callback)(void)) {
    size_t tim_idx = stm_peripherals_timer_get_index(instance);
    stm_timer_callback[tim_idx] = callback;
    tim_clock_enable(1 << tim_idx);
    HAL_NVIC_SetPriority(irq_map[tim_idx], prio, 0);
    HAL_NVIC_EnableIRQ(irq_map[tim_idx]);
}

    void stm_peripherals_timer_reserve(TIM_TypeDef *instance) {
    size_t tim_idx = stm_peripherals_timer_get_index(instance);
    stm_timer_reserved[tim_idx] = true;
}

    void stm_peripherals_timer_set_callback(void (*callback)(void), TIM_TypeDef *timer) {
    stm_timer_callback[stm_peripherals_timer_get_index(timer)] = callback;
}

    void stm_peripherals_timer_free(TIM_TypeDef *instance) {
    size_t tim_idx = stm_peripherals_timer_get_index(instance);
    HAL_NVIC_DisableIRQ(irq_map[tim_idx]);
    stm_timer_callback[tim_idx] = NULL;
    tim_clock_disable(1 << tim_idx);
    stm_timer_reserved[tim_idx] = false;
    stm_timer_never_reset[tim_idx] = false;
}

    void stm_peripherals_timer_never_reset(TIM_TypeDef *instance) {
    size_t tim_idx = stm_peripherals_timer_get_index(instance);
    stm_timer_never_reset[tim_idx] = true;
}
    void stm_peripherals_timer_reset_ok(TIM_TypeDef *instance) {
    size_t tim_idx = stm_peripherals_timer_get_index(instance);
    stm_timer_never_reset[tim_idx] = false;
}
    bool stm_peripherals_timer_is_never_reset(TIM_TypeDef *instance) {
    size_t tim_idx = stm_peripherals_timer_get_index(instance);
    return stm_timer_never_reset[tim_idx];
}
    bool stm_peripherals_timer_is_reserved(TIM_TypeDef *instance) {
    size_t tim_idx = stm_peripherals_timer_get_index(instance);
    return stm_timer_reserved[tim_idx];
}

    size_t stm_peripherals_timer_get_index(TIM_TypeDef *instance) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_tim_banks); i++) {
        if (instance == mcu_tim_banks[i]) {
            return i;
        }
    }
    return ~(size_t)0;
}

    void tim_clock_enable(uint16_t mask) {
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
    // 6 and 7 are reserved ADC timers
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

    void tim_clock_disable(uint16_t mask) {
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
    // 6 and 7 are reserved ADC timers
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
    if (stm_timer_callback[index - 1]) {
        (*stm_timer_callback[index - 1])();
    }
}

    void TIM1_CC_IRQHandler(void) { // Advanced timer
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
    void TIM6_DAC_IRQHandler(void) { // Basic timer (DAC)
    callback_router(6);
}
    void TIM7_IRQHandler(void) {    // Basic timer
    callback_router(7);
}
    void TIM8_CC_IRQHandler(void) { // Advanced timer
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

#endif
