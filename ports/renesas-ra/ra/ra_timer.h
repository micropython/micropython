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

#ifndef RA_RA_TIMER_H_
#define RA_RA_TIMER_H_

#include <stdint.h>

void SysTick_Handler(void);
uint32_t HAL_GetTick(void);

#define DEF_CLKDEV    2
#define TENUSEC_COUNT (PCLK / DEF_CLKDEV / 100000)
#define MSEC_COUNT    (PCLK / DEF_CLKDEV / 100)

typedef void (*AGT_TIMER_CB)(void *);

void ra_agt_timer_set_callback(uint32_t ch, AGT_TIMER_CB cb, void *param);
void ra_agt_int_isr0(void);
void ra_agt_int_isr1(void);
void ra_agt_timer_start(uint32_t ch);
void ra_agt_timer_stop(uint32_t ch);
void ra_agt_timer_set_freq(uint32_t ch, float freq);
float ra_agt_timer_get_freq(uint32_t ch);
void ra_agt_timer_init(uint32_t ch, float freq);
void ra_agt_timer_deinit(uint32_t ch);
__WEAK void agt_int_isr(void);
uint32_t mtick();

#endif /* RA_RA_TIMER_H_ */
