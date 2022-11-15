/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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

#ifndef RA_RA_ICU_H_
#define RA_RA_ICU_H_

#include <stdint.h>
#include <stdbool.h>

// #define IRQ_MAX            (16)
#define VECTOR_NUMBER_NONE (0xff)

typedef struct ra_irq_pin {
    uint8_t irq_no;
    uint32_t pin;
} ra_icu_pin_t;

typedef void (*ICU_CB)(void *);

bool ra_icu_find_irq_no(uint32_t pin, uint8_t *irq_no);
void ra_icu_set_pin(uint32_t pin, bool irq_enable, bool pullup);
void ra_icu_enable_irq_no(uint8_t irq_no);
void ra_icu_disable_irq_no(uint8_t irq_no);
void ra_icu_enable_pin(uint32_t pin);
void ra_icu_disable_pin(uint32_t pin);
void ra_icu_priority_irq_no(uint8_t irq_no, uint32_t ipl);
void ra_icu_priority_pin(uint32_t pin, uint32_t ipl);
void ra_icu_set_callback(uint8_t irq_no, ICU_CB func, void *param);
void ra_icu_trigger_irq_no(uint8_t irq_no, uint32_t cond);
void ra_icu_trigger_pin(uint32_t pin, uint32_t cond);
void ra_icu_set_bounce(uint8_t irq_no, uint32_t bounce);
void ra_icu_init(void);
void ra_icu_deinit(void);
void ra_icu_swint(uint8_t irq_no);
void r_icu_isr(void);

#endif /* RA_RA_ICU_H_ */
