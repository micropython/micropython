/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* This file was separated prom original port.c by SDK team to enable efective tickless idle mode implementation. */

#ifndef FSL_TICKLESS_GENERIC_H
#define FSL_TICKLESS_GENERIC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef configSYSTICK_CLOCK_HZ
#define configSYSTICK_CLOCK_HZ CLOCK_GetFreq(kCLOCK_IpgClk)
#endif
/* Constants required to manipulate the core.  Registers first... */
#define portGIC_SYSTICK_CTRL_REG (GPT1->CR)
#define portGIC_SYSTICK_LOAD_REG (GPT1->OCR[0])
#define portGIC_SYSTICK_CURRENT_VALUE_REG (GPT1->CNT)

/* ...then bits in the registers. */
#define portGIC_SYSTICK_ENABLE_BIT (GPT_CR_EN_MASK)

/* The systick is a 32-bit counter. */
#define portMAX_32_BIT_NUMBER (0xffffffffUL)

/* Low power Tickless idle support. Low power timer input clocks. */
#define configLPTMR_CLOCK_HZ    (1000)
#define configRTC_CLOCK_HZ      (0x8000UL)
#if defined(CPU_MIMX8QM5AVUDD_cm4_core0)
#define configLPIT_CLOCK_HZ     CLOCK_GetFreq(kCLOCK_M4_0_Lpit)
#elif defined(CPU_MIMX8QM5AVUDD_cm4_core1)
#define configLPIT_CLOCK_HZ     CLOCK_GetFreq(kCLOCK_M4_1_Lpit)
#elif defined(CPU_MIMX8QXP_M4AVVJP_M4_0)
#define configLPIT_CLOCK_HZ     CLOCK_GetFreq(kCLOCK_M4_0_Lpit)
#endif
#define configEPIT_CLOCK_HZ     CLOCK_GetFreq(kCLOCK_IpgClk)

#ifdef __cplusplus
}
#endif

#endif /* FSL_TICKLESS_GENERIC_H */
