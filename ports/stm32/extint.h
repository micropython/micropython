/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_STM32_EXTINT_H
#define MICROPY_INCLUDED_STM32_EXTINT_H

#include "py/mphal.h"

// Vectors 0-15 are for regular pins
// Vectors 16-22 are for internal sources.
//
// Use the following constants for the internal sources:

#define EXTI_PVD_OUTPUT         (16)
#if defined(STM32L4)
#define EXTI_RTC_ALARM          (18)
#define EXTI_USB_OTG_FS_WAKEUP  (17)
#else
#define EXTI_RTC_ALARM          (17)
#define EXTI_USB_OTG_FS_WAKEUP  (18)
#endif
#define EXTI_ETH_WAKEUP         (19)
#define EXTI_USB_OTG_HS_WAKEUP  (20)
#if defined(STM32F0) || defined(STM32G4) || defined(STM32L1) || defined(STM32L4) || defined(STM32WL)
#define EXTI_RTC_TIMESTAMP      (19)
#define EXTI_RTC_WAKEUP         (20)
#elif defined(STM32H7) || defined(STM32WB)
#define EXTI_RTC_TIMESTAMP      (18)
#define EXTI_RTC_WAKEUP         (19)
#elif defined(STM32G0)
#define EXTI_RTC_WAKEUP         (19)
#define EXTI_RTC_TIMESTAMP      (21)
#else
#define EXTI_RTC_TIMESTAMP      (21)
#define EXTI_RTC_WAKEUP         (22)
#endif
#if defined(STM32F7)
#define EXTI_LPTIM1_ASYNC_EVENT (23)
#endif

#define EXTI_NUM_VECTORS        (PYB_EXTI_NUM_VECTORS)

void extint_init0(void);

uint extint_register(mp_obj_t pin_obj, uint32_t mode, uint32_t pull, mp_obj_t callback_obj, bool override_callback_obj);
void extint_register_pin(const pin_obj_t *pin, uint32_t mode, bool hard_irq, mp_obj_t callback_obj);
void extint_set(const pin_obj_t *pin, uint32_t mode);

void extint_enable(uint line);
void extint_disable(uint line);
void extint_swint(uint line);
void extint_trigger_mode(uint line, uint32_t mode);

void Handle_EXTI_Irq(uint32_t line);

extern const mp_obj_type_t extint_type;

#endif // MICROPY_INCLUDED_STM32_EXTINT_H
