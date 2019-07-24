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
	#define configSYSTICK_CLOCK_HZ			configCPU_CLOCK_HZ
	/* Ensure the SysTick is clocked at the same frequency as the core. */
	#define portNVIC_SYSTICK_CLK_BIT		( 1UL << 2UL )
#else
	/* The way the SysTick is clocked is not modified in case it is not the same
	as the core. */
	#define portNVIC_SYSTICK_CLK_BIT		( 0 )
#endif

/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL				( ( volatile uint32_t *) 0xe000e010 )
#define portNVIC_SYSTICK_LOAD				( ( volatile uint32_t *) 0xe000e014 )
#define portNVIC_INT_CTRL					( ( volatile uint32_t *) 0xe000ed04 )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG	( * ( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SYSPRI2					( ( volatile uint32_t *) 0xe000ed20 )
#define portNVIC_SYSTICK_CLK				0x00000004
#define portNVIC_SYSTICK_INT				0x00000002
#define portNVIC_SYSTICK_ENABLE				0x00000001
#define portNVIC_PENDSVSET					0x10000000
#define portMIN_INTERRUPT_PRIORITY			( 255UL )
#define portNVIC_PENDSV_PRI					( portMIN_INTERRUPT_PRIORITY << 16UL )
#define portNVIC_SYSTICK_PRI				( portMIN_INTERRUPT_PRIORITY << 24UL )

/* ...then bits in the registers. */
#define portNVIC_SYSTICK_INT_BIT			( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT			( 1UL << 0UL )
#define portNVIC_SYSTICK_COUNT_FLAG_BIT		( 1UL << 16UL )
#define portNVIC_PENDSVCLEAR_BIT			( 1UL << 27UL )
#define portNVIC_PEND_SYSTICK_CLEAR_BIT		( 1UL << 25UL )

/* The systick is a 24-bit counter. */
#define portMAX_24_BIT_NUMBER				( 0xffffffUL )

/* The LPTMR is a 16-bit counter. */
#define portMAX_16_BIT_NUMBER				( 0xffffUL )

/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define portMISSED_COUNTS_FACTOR			( 45UL )

/* Constants used with memory barrier intrinsics. */
#define portSY_FULL_READ_WRITE				( 15 )

/* Adaptation for general low power timers */
#define vPortSetupTimerInterrupt prvSetupTimerInterrupt
#define portNVIC_SYSTICK_CTRL_REG (*(portNVIC_SYSTICK_CTRL))
#define portNVIC_SYSTICK_LOAD_REG (*(portNVIC_SYSTICK_LOAD))

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
/*
 * Setup the timer to generate the tick interrupts.
 */
void prvSetupTimerInterrupt( void );

#ifdef __cplusplus
}
#endif

#endif /* FSL_TICKLESS_GENERIC_H */
