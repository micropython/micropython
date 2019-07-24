/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* IAR includes. */
#ifdef __ICCARM__

	#include <intrinsics.h>

	#ifdef __cplusplus
		extern "C" {
	#endif

	/*-----------------------------------------------------------
	 * Port specific definitions.
	 *
	 * The settings in this file configure FreeRTOS correctly for the given hardware
	 * and compiler.
	 *
	 * These settings should not be altered.
	 *-----------------------------------------------------------
	 */

	/* Type definitions. */
	#define portCHAR		char
	#define portFLOAT		float
	#define portDOUBLE		double
	#define portLONG		long
	#define portSHORT		short
	#define portSTACK_TYPE	uint32_t
	#define portBASE_TYPE	long

	typedef portSTACK_TYPE StackType_t;
	typedef long BaseType_t;
	typedef unsigned long UBaseType_t;

	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

	/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
	not need to be guarded with a critical section. */
	#define portTICK_TYPE_IS_ATOMIC 1

	/*-----------------------------------------------------------*/

	/* Hardware specifics. */
	#define portSTACK_GROWTH			( -1 )
	#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
	#define portBYTE_ALIGNMENT			8

	/*-----------------------------------------------------------*/

	/* Task utilities. */

	/* Called at the end of an ISR that can cause a context switch. */
	#define portEND_SWITCHING_ISR( xSwitchRequired )\
	{												\
	extern uint32_t ulPortYieldRequired;			\
													\
		if( xSwitchRequired != pdFALSE )			\
		{											\
			ulPortYieldRequired = pdTRUE;			\
		}											\
	}

	#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )
	#define portYIELD() __asm( "SWI 0" );


	/*-----------------------------------------------------------
	 * Critical section control
	 *----------------------------------------------------------*/

	extern void vPortEnterCritical( void );
	extern void vPortExitCritical( void );
	extern uint32_t ulPortSetInterruptMask( void );
	extern void vPortClearInterruptMask( uint32_t ulNewMaskValue );

	/* These macros do not globally disable/enable interrupts.  They do mask off
	interrupts that have a priority below configMAX_API_CALL_INTERRUPT_PRIORITY. */
	#define portENTER_CRITICAL()		vPortEnterCritical();
	#define portEXIT_CRITICAL()			vPortExitCritical();
	#define portDISABLE_INTERRUPTS()	ulPortSetInterruptMask()
	#define portENABLE_INTERRUPTS()		vPortClearInterruptMask( 0 )
	#define portSET_INTERRUPT_MASK_FROM_ISR()		ulPortSetInterruptMask()
	#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	vPortClearInterruptMask(x)

	/*-----------------------------------------------------------*/

	/* Task function macros as described on the FreeRTOS.org WEB site.  These are
	not required for this port but included in case common demo code that uses these
	macros is used. */
	#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )	void vFunction( void *pvParameters )
	#define portTASK_FUNCTION( vFunction, pvParameters )	void vFunction( void *pvParameters )

	/* Prototype of the FreeRTOS tick handler.  This must be installed as the
	handler for whichever peripheral is used to generate the RTOS tick. */
	void FreeRTOS_Tick_Handler( void );

	/* Any task that uses the floating point unit MUST call vPortTaskUsesFPU()
	before any floating point instructions are executed. */
	void vPortTaskUsesFPU( void );
	#define portTASK_USES_FLOATING_POINT() vPortTaskUsesFPU()

	#define portLOWEST_INTERRUPT_PRIORITY ( ( ( uint32_t ) configUNIQUE_INTERRUPT_PRIORITIES ) - 1UL )
	#define portLOWEST_USABLE_INTERRUPT_PRIORITY ( portLOWEST_INTERRUPT_PRIORITY - 1UL )

	/* Architecture specific optimisations. */
	#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
		#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
	#endif

	#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

		/* Store/clear the ready priorities in a bit map. */
		#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
		#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

		/*-----------------------------------------------------------*/

		#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31 - __CLZ( uxReadyPriorities ) )

	#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

	#ifdef configASSERT
		void vPortValidateInterruptPriority( void );
		#define portASSERT_IF_INTERRUPT_PRIORITY_INVALID() 	vPortValidateInterruptPriority()
	#endif /* configASSERT */

	#define portNOP() __asm volatile( "NOP" )


	#ifdef __cplusplus
		} /* extern C */
	#endif

	/* Suppress warnings that are generated by the IAR tools, but cannot be
	fixed in the source code because to do so would cause other compilers to
	generate warnings. */
	#pragma diag_suppress=Pe191
	#pragma diag_suppress=Pa082

#endif /* __ICCARM__ */


/* The number of bits to shift for an interrupt priority is dependent on the
number of bits implemented by the interrupt controller. */
#if configUNIQUE_INTERRUPT_PRIORITIES == 16
	#define portPRIORITY_SHIFT 4
	#define portMAX_BINARY_POINT_VALUE	3
#elif configUNIQUE_INTERRUPT_PRIORITIES == 32
	#define portPRIORITY_SHIFT 3
	#define portMAX_BINARY_POINT_VALUE	2
#elif configUNIQUE_INTERRUPT_PRIORITIES == 64
	#define portPRIORITY_SHIFT 2
	#define portMAX_BINARY_POINT_VALUE	1
#elif configUNIQUE_INTERRUPT_PRIORITIES == 128
	#define portPRIORITY_SHIFT 1
	#define portMAX_BINARY_POINT_VALUE	0
#elif configUNIQUE_INTERRUPT_PRIORITIES == 256
	#define portPRIORITY_SHIFT 0
	#define portMAX_BINARY_POINT_VALUE	0
#else
	#error Invalid configUNIQUE_INTERRUPT_PRIORITIES setting.  configUNIQUE_INTERRUPT_PRIORITIES must be set to the number of unique priorities implemented by the target hardware
#endif

/* Interrupt controller access addresses. */
#define portICCPMR_PRIORITY_MASK_OFFSET  						( 0x04 )
#define portICCIAR_INTERRUPT_ACKNOWLEDGE_OFFSET 				( 0x0C )
#define portICCEOIR_END_OF_INTERRUPT_OFFSET 					( 0x10 )
#define portICCBPR_BINARY_POINT_OFFSET							( 0x08 )
#define portICCRPR_RUNNING_PRIORITY_OFFSET						( 0x14 )

#define portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS 		( configINTERRUPT_CONTROLLER_BASE_ADDRESS + configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET )
#define portICCPMR_PRIORITY_MASK_REGISTER 					( *( ( volatile uint32_t * ) ( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCPMR_PRIORITY_MASK_OFFSET ) ) )
#define portICCIAR_INTERRUPT_ACKNOWLEDGE_REGISTER_ADDRESS 	( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCIAR_INTERRUPT_ACKNOWLEDGE_OFFSET )
#define portICCEOIR_END_OF_INTERRUPT_REGISTER_ADDRESS 		( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCEOIR_END_OF_INTERRUPT_OFFSET )
#define portICCPMR_PRIORITY_MASK_REGISTER_ADDRESS 			( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCPMR_PRIORITY_MASK_OFFSET )
#define portICCBPR_BINARY_POINT_REGISTER 					( *( ( const volatile uint32_t * ) ( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCBPR_BINARY_POINT_OFFSET ) ) )
#define portICCRPR_RUNNING_PRIORITY_REGISTER 				( *( ( const volatile uint32_t * ) ( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCRPR_RUNNING_PRIORITY_OFFSET ) ) )

#endif /* PORTMACRO_H */

