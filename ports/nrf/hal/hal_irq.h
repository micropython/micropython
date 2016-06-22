/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#ifndef HAL_IRQ_H__
#define HAL_IRQ_H__

#include <stdint.h>

#include "nrf.h"

#if BLUETOOTH_SD
#include "py/nlr.h"
#include "ble_drv.h"

#define BLUETOOTH_STACK_ENABLED() (ble_drv_stack_enabled())

#ifdef NRF51
  #include "nrf_soc.h"
#elif defined(NRF52)
  #include "nrf_nvic.h"
#endif
#endif // BLUETOOTH_SD

static inline void hal_irq_clear(uint32_t irq_num) {
#if BLUETOOTH_SD
    if (BLUETOOTH_STACK_ENABLED() == 1) {
        if (sd_nvic_ClearPendingIRQ(irq_num) != 0) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                      "IRQ (%d) clear error", irq_num));
        }
    } else
#endif // BLUETOOTH_SD
    {
        NVIC_ClearPendingIRQ(irq_num);
    }
}

static inline void hal_irq_enable(uint32_t irq_num) {
    hal_irq_clear(irq_num);

#if BLUETOOTH_SD
    if (BLUETOOTH_STACK_ENABLED() == 1) {
        if (sd_nvic_EnableIRQ(irq_num) != 0) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                      "IRQ (%d) enable error", irq_num));
        }
    } else
#endif // BLUETOOTH_SD
    {
        NVIC_EnableIRQ(irq_num);
    }
}

static inline void hal_irq_disable(uint32_t irq_num) {
#if BLUETOOTH_SD
    if (BLUETOOTH_STACK_ENABLED() == 1) {
        if (sd_nvic_DisableIRQ(irq_num) != 0) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                      "IRQ (%d) disable error", irq_num));
        }
    } else
#endif // BLUETOOTH_SD
    {
        NVIC_DisableIRQ(irq_num);
    }
}

static inline void hal_irq_priority(uint32_t irq_num, uint8_t priority) {
#if BLUETOOTH_SD
    if (BLUETOOTH_STACK_ENABLED() == 1) {
        if (sd_nvic_SetPriority(irq_num, priority) != 0) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                      "IRQ (%d) priority error", irq_num, priority));
        }
    } else
#endif // BLUETOOTH_SD
    {
        NVIC_SetPriority(irq_num, priority);
    }
}

static inline void hal_irq_pending(uint32_t irq_num) {
#if BLUETOOTH_SD
    if (BLUETOOTH_STACK_ENABLED() == 1) {
        if (sd_nvic_SetPendingIRQ(irq_num) != 0) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                      "IRQ (%d) pending error", irq_num));
        }
    } else
#endif // BLUETOOTH_SD
    {
        NVIC_SetPendingIRQ(irq_num);
    }
}

#endif // HAL_IRQ_H__
