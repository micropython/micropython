/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef __SECURE_PORT_MACROS_H__
#define __SECURE_PORT_MACROS_H__

/**
 * @brief Byte alignment requirements.
 */
#define secureportBYTE_ALIGNMENT					8
#define secureportBYTE_ALIGNMENT_MASK				( 0x0007 )

/**
 * @brief Macro to declare a function as non-secure callable.
 */
#if defined( __IAR_SYSTEMS_ICC__ )
	#define secureportNON_SECURE_CALLABLE			__cmse_nonsecure_entry
#else
	#define secureportNON_SECURE_CALLABLE			__attribute__((cmse_nonsecure_entry))
#endif

/**
 * @brief Set the secure PRIMASK value.
 */
#define secureportSET_SECURE_PRIMASK( ulPrimaskValue ) \
	__asm volatile ( "msr primask, %0" : : "r" ( ulPrimaskValue ) : "memory" )

/**
 * @brief Set the non-secure PRIMASK value.
 */
#define secureportSET_NON_SECURE_PRIMASK( ulPrimaskValue ) \
	__asm volatile ( "msr primask_ns, %0" : : "r" ( ulPrimaskValue ) : "memory" )

/**
 * @brief Read the PSP value in the given variable.
 */
#define secureportREAD_PSP( pucOutCurrentStackPointer ) \
	__asm volatile ( "mrs %0, psp"  : "=r" ( pucOutCurrentStackPointer ) )

/**
 * @brief Set the PSP to the given value.
 */
#define secureportSET_PSP( pucCurrentStackPointer ) \
	__asm volatile ( "msr psp, %0" : : "r" ( pucCurrentStackPointer ) )

/**
 * @brief Set the PSPLIM to the given value.
 */
#define secureportSET_PSPLIM( pucStackLimit ) \
	__asm volatile ( "msr psplim, %0" : : "r" ( pucStackLimit ) )

/**
 * @brief Set the NonSecure MSP to the given value.
 */
#define secureportSET_MSP_NS( pucMainStackPointer ) \
	__asm volatile ( "msr msp_ns, %0" : : "r" ( pucMainStackPointer ) )

/**
 * @brief Set the CONTROL register to the given value.
 */
#define secureportSET_CONTROL( ulControl ) \
	__asm volatile ( "msr control, %0" : : "r" ( ulControl ) : "memory" )

/**
 * @brief Read the Interrupt Program Status Register (IPSR) value in the given
 * variable.
 */
#define secureportREAD_IPSR( ulIPSR ) \
	__asm volatile ( "mrs %0, ipsr"  : "=r" ( ulIPSR ) )

/**
 * @brief PRIMASK value to enable interrupts.
 */
#define secureportPRIMASK_ENABLE_INTERRUPTS_VAL		0

/**
 * @brief PRIMASK value to disable interrupts.
 */
#define secureportPRIMASK_DISABLE_INTERRUPTS_VAL	1

/**
 * @brief Disable secure interrupts.
 */
#define secureportDISABLE_SECURE_INTERRUPTS()		secureportSET_SECURE_PRIMASK( secureportPRIMASK_DISABLE_INTERRUPTS_VAL )

/**
 * @brief Disable non-secure interrupts.
 *
 * This effectively disables context switches.
 */
#define secureportDISABLE_NON_SECURE_INTERRUPTS()	secureportSET_NON_SECURE_PRIMASK( secureportPRIMASK_DISABLE_INTERRUPTS_VAL )

/**
 * @brief Enable non-secure interrupts.
 */
#define secureportENABLE_NON_SECURE_INTERRUPTS()	secureportSET_NON_SECURE_PRIMASK( secureportPRIMASK_ENABLE_INTERRUPTS_VAL )

/**
 * @brief Assert definition.
 */
#define secureportASSERT( x )						\
	if( ( x ) == 0 )								\
	{												\
		secureportDISABLE_SECURE_INTERRUPTS();		\
		secureportDISABLE_NON_SECURE_INTERRUPTS();	\
		for( ;; );									\
	}

#endif /* __SECURE_PORT_MACROS_H__ */
