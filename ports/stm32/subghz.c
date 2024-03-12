/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Angus Gratton
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

#include "py/gc.h"
#include "py/runtime.h"
#include "subghz.h"
#include "irq.h"
#include "spi.h"

#if defined(STM32WL)

// Interface to the STM32WL series "SUBGHZ Radio" module

static void handle_radio_irq() {
    // Level-triggered interrupts means the interrupt has to be cleared before
    // this function returns.
    //
    // Rather than writing to SUBGHZ SPI in Interrupt Context to clear the
    // interrupt, disable the IRQ and rely on Python code to call
    // subghz_irq(handler) to re-enable when needed.
    HAL_NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);

    mp_obj_t callback = MP_STATE_PORT(subghz_callback);
    if (callback != mp_const_none) {
        mp_sched_lock();
        gc_lock();
        // Passing dummy 'pin' argument of None, to keep
        // compatibility with machine.Pin.isr() handlers
        mp_call_function_1_protected(callback, mp_const_none);
        gc_unlock();
        mp_sched_unlock();
    }
}

void SUBGHZ_Radio_IRQHandler(void) {
    IRQ_ENTER(SUBGHZ_Radio_IRQn);
    handle_radio_irq();
    IRQ_EXIT(SUBGHZ_Radio_IRQn);
}

void subghz_init(void) {
    __HAL_RCC_SUBGHZ_RADIO_FORCE_RESET();

    #if !MICROPY_HW_CLK_USE_HSE && MICROPY_HW_CLK_USE_BYPASS
    // SUBGHZ clock source is HSE oscillator.
    //
    // If this is not already enabled for the system clock, and we're depending
    // on the VDDTCXO pin to power the HSE ("bypass mode"), then enable it.
    __HAL_RCC_HSE_CONFIG(RCC_HSE_BYPASS_PWR);
    #endif

    NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);
    NVIC_SetPriority(SUBGHZ_Radio_IRQn, IRQ_PRI_SUBGHZ_RADIO);

    __HAL_RCC_SUBGHZ_RADIO_RELEASE_RESET();

    while (__HAL_RCC_GET_FLAG(RCC_FLAG_RFRST) != 0) {
    }

    MP_STATE_PORT(subghz_callback) = mp_const_none;
}

void subghz_deinit(void) {
    MP_STATE_PORT(subghz_callback) = mp_const_none;
    NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);
    __HAL_RCC_SUBGHZ_RADIO_FORCE_RESET();
    __HAL_RCC_SUBGHZ_RADIO_RELEASE_RESET();
}

static mp_obj_t subghz_cs(mp_obj_t value) {
    // Treat the same as normal SPI - truthy is "unselected",
    //   falsey is active low "selected",
    if (mp_obj_is_true(value)) {
        LL_PWR_UnselectSUBGHZSPI_NSS();
    } else {
        LL_PWR_SelectSUBGHZSPI_NSS();
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(subghz_cs_obj, subghz_cs);

static mp_obj_t subghz_irq(mp_obj_t handler) {
    MP_STATE_PORT(subghz_callback) = handler;

    if (mp_obj_is_true(handler)) {
        HAL_NVIC_ClearPendingIRQ(SUBGHZ_Radio_IRQn);
        HAL_NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);
    } else {
        HAL_NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(subghz_irq_obj, subghz_irq);

static mp_obj_t subghz_is_busy(void) {
    // Read the raw unmasked busy signal. This should be checked before driving
    // CS low to start a command.
    //
    // Reads the raw RFBUSYS not the masked RFBUSYMS, in contradiction to EM0453
    // 6.3 "Radio busy management". This is because the RFBUSYMS signal doesn't
    // seem to match the reference manual. Observed behaviour matches this bug
    // report instead: https://community.st.com/s/question/0D53W000014zFx9SAE
    //
    // Reading RFBUSYS won't cause any problems here provided a new SPI command
    // isn't immediately after the previous command, which shouldn't be possible
    // with MicroPython.
    return mp_obj_new_bool(LL_PWR_IsActiveFlag_RFBUSYS());

}
MP_DEFINE_CONST_FUN_OBJ_0(subghz_is_busy_obj, subghz_is_busy);

MP_REGISTER_ROOT_POINTER(mp_obj_t subghz_callback);

#endif // STM32WL
