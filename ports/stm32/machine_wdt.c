/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2026 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.

#include "py/mphal.h"

#if defined(STM32H7)
#define IWDG (IWDG1)
#define WWDG (WWDG1)
#if defined(DUAL_CORE)
#define DUAL_WDG (1)
#endif
#endif

#ifndef DUAL_WDG
#define DUAL_WDG (0)
#endif

#if defined(WWDG_CFR_WDGTB_2)
#define WWDG_PRESCALER_MAX (7U)
#else
#define WWDG_PRESCALER_MAX (3U)
#endif
#define WWDG_COUNTER_MAX (64U)

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    __IO uint32_t *feed_register;
    uint32_t feed_value;
} machine_wdt_obj_t;

static const machine_wdt_obj_t machine_iwdt = {{&machine_wdt_type}, &IWDG->KR, 0xaaaa};
static machine_wdt_obj_t machine_wwdt = {{&machine_wdt_type}, &WWDG->CR, 0};

#if DUAL_WDG
static const machine_wdt_obj_t machine_iwdt2 = {{&machine_wdt_type}, &IWDG2->KR, 0xaaaa};
static machine_wdt_obj_t machine_wwdt2 = {{&machine_wdt_type}, &WWDG2->CR, 0};
#endif

#if defined(STM32H7)
// This is not provided by the HAL, so define it here.
static uint32_t HAL_RCC_GetPCLK3Freq(void) {
    return LL_RCC_CALC_PCLK3_FREQ(HAL_RCC_GetHCLKFreq(), LL_RCC_GetAPB3Prescaler());
}
#endif

static machine_wdt_obj_t *make_wwdt(machine_wdt_obj_t *self, WWDG_TypeDef *wwdg, mp_int_t timeout_ms) {
    // WWDG is clocked from PCLKx divided by 4096.
    uint32_t pclk;
    #if defined(STM32H7)
    #if DUAL_WDG
    if (wwdg == WWDG2) {
        pclk = HAL_RCC_GetPCLK1Freq();
    } else
    #endif
    {
        pclk = HAL_RCC_GetPCLK3Freq();
    }
    #else
    pclk = HAL_RCC_GetPCLK1Freq();
    #endif

    // Compute the number of ticks corresponding to the requested millisecond timeout.
    uint64_t timeout_ticks = (uint64_t)timeout_ms * (uint64_t)(pclk / 1000U) / 4096ULL;

    // Increase the prescaler to try and get timeout_ticks below its maximum value.
    uint32_t prescaler = 0;
    while (prescaler < WWDG_PRESCALER_MAX && timeout_ticks > WWDG_COUNTER_MAX) {
        ++prescaler;
        timeout_ticks = (timeout_ticks + 1) / 2;
    }

    // Check that the timeout is within range of the peripheral limits.
    if (timeout_ticks <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too short"));
    } else if (timeout_ticks > WWDG_COUNTER_MAX) {
        unsigned int max_timeout_ms = WWDG_COUNTER_MAX * 4096U * (1U << WWDG_PRESCALER_MAX) / (HAL_RCC_GetPCLK1Freq() / 1000U);
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT timeout too long, max %ums"), max_timeout_ms);
    }

    // Compute the value that will feed the WWDG.
    self->feed_value = WWDG_CR_WDGA | (0x3f + timeout_ticks) << WWDG_CR_T_Pos;

    // Enable WWDG clock.
    #if defined(STM32H7)
    #if DUAL_WDG
    if (wwdg == WWDG2) {
        LL_APB3_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG2);
        LL_RCC_WWDG2_EnableSystemReset();
    } else
    #endif
    {
        LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_WWDG1);
        #if defined(RCC_GCR_WW1RSC)
        LL_RCC_WWDG1_EnableSystemReset();
        #endif
    }
    #else
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);
    #endif

    // Initialise and start the WWDG.
    wwdg->CFR = prescaler << WWDG_CFR_WDGTB_Pos | 0x7f << WWDG_CFR_W_Pos;
    wwdg->CR = self->feed_value;

    return self;
}

static machine_wdt_obj_t *make_iwdt(const machine_wdt_obj_t *self, IWDG_TypeDef *iwdg, mp_int_t timeout_ms) {
    // compute prescaler
    int32_t timeout = timeout_ms;
    uint32_t prescaler;
    for (prescaler = 0; prescaler < 6 && timeout >= 512; ++prescaler, timeout /= 2) {
    }

    // convert milliseconds to ticks
    timeout *= 8; // 32kHz / 4 = 8 ticks per millisecond (approx)
    if (timeout <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too short"));
    } else if (timeout > 0xfff) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too long"));
    }
    timeout -= 1;

    // set the reload register
    while (iwdg->SR & 2) {
    }
    iwdg->KR = 0x5555;
    iwdg->RLR = timeout;

    // set the prescaler
    while (iwdg->SR & 1) {
    }
    iwdg->KR = 0x5555;
    iwdg->PR = prescaler;

    // start the watch dog
    iwdg->KR = 0xcccc;

    return (machine_wdt_obj_t *)self;
}

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_obj_t id_obj, mp_int_t timeout_ms) {
    if (mp_obj_is_str(id_obj)) {
        qstr qst = mp_obj_str_get_qstr(id_obj);
        if (qst == MP_QSTR_IWDG) {
            return make_iwdt(&machine_iwdt, IWDG, timeout_ms);
        } else if (qst == MP_QSTR_WWDG) {
            return make_wwdt(&machine_wwdt, WWDG, timeout_ms);
        #if DUAL_WDG
        } else if (qst == MP_QSTR_IWDG2) {
            return make_iwdt(&machine_iwdt2, IWDG2, timeout_ms);
        } else if (qst == MP_QSTR_WWDG2) {
            return make_wwdt(&machine_wwdt2, WWDG2, timeout_ms);
        #endif
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT(%q) doesn't exist"), qst);
        }
    } else {
        mp_int_t id = mp_obj_get_int(id_obj);
        if (id == 0) {
            return make_iwdt(&machine_iwdt, IWDG, timeout_ms);
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT(%d) doesn't exist"), id);
        }
    }
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    *self->feed_register = self->feed_value;
}
