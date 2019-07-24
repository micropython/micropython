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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM4F port.
 *----------------------------------------------------------*/

/* IAR includes. */
#include <intrinsics.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_tickless_generic.h"

extern uint32_t SystemCoreClock; /* in Kinetis SDK, this contains the system core clock speed */

#ifndef __ARMVFP__
	#error This port can only be used when the project options are configured to enable hardware floating point support.
#endif

#if( configMAX_SYSCALL_INTERRUPT_PRIORITY == 0 )
	#error configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.  See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html
#endif

/* Constants used to detect a Cortex-M7 r0p1 core, which should use the ARM_CM7
r0p1 port. */
#define portCPUID							( * ( ( volatile uint32_t * ) 0xE000ed00 ) )
#define portCORTEX_M7_r0p1_ID				( 0x410FC271UL )
#define portCORTEX_M7_r0p0_ID				( 0x410FC270UL )

#define portNVIC_PENDSV_PRI					( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 16UL )
#define portNVIC_SYSTICK_PRI				( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 24UL )

/* Constants required to check the validity of an interrupt priority. */
#define portFIRST_USER_INTERRUPT_NUMBER		( 16 )
#define portNVIC_IP_REGISTERS_OFFSET_16 	( 0xE000E3F0 )
#define portAIRCR_REG						( * ( ( volatile uint32_t * ) 0xE000ED0C ) )
#define portMAX_8_BIT_VALUE					( ( uint8_t ) 0xff )
#define portTOP_BIT_OF_BYTE					( ( uint8_t ) 0x80 )
#define portMAX_PRIGROUP_BITS				( ( uint8_t ) 7 )
#define portPRIORITY_GROUP_MASK				( 0x07UL << 8UL )
#define portPRIGROUP_SHIFT					( 8UL )

/* Masks off all bits but the VECTACTIVE bits in the ICSR register. */
#define portVECTACTIVE_MASK					( 0xFFUL )

/* Constants required to manipulate the VFP. */
#define portFPCCR							( ( volatile uint32_t * ) 0xe000ef34 ) /* Floating point context control register. */
#define portASPEN_AND_LSPEN_BITS			( 0x3UL << 30UL )

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR					( 0x01000000 )
#define portINITIAL_EXC_RETURN				( 0xfffffffd )



/* For strict compliance with the Cortex-M spec the task start address should
have bit-0 clear, as it is loaded into the PC on exit from an ISR. */
#define portSTART_ADDRESS_MASK				( ( StackType_t ) 0xfffffffeUL )


/*
 * Exception handlers.
 */
void xPortSysTickHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
extern void vPortStartFirstTask( void );

/*
 * Turn the VFP on.
 */
extern void vPortEnableVFP( void );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

/*
 * Used by the portASSERT_IF_INTERRUPT_PRIORITY_INVALID() macro to ensure
 * FreeRTOS API functions are not called from interrupts that have been assigned
 * a priority above configMAX_SYSCALL_INTERRUPT_PRIORITY.
 */
#if( configASSERT_DEFINED == 1 )
	 static uint8_t ucMaxSysCallPriority = 0;
	 static uint32_t ulMaxPRIGROUPValue = 0;
	 static const volatile uint8_t * const pcInterruptPriorityRegisters = ( const volatile uint8_t * const ) portNVIC_IP_REGISTERS_OFFSET_16;
#endif /* configASSERT_DEFINED */

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */

	/* Offset added to account for the way the MCU uses the stack on entry/exit
	of interrupts, and to ensure alignment. */
	pxTopOfStack--;

	*pxTopOfStack = portINITIAL_XPSR;	/* xPSR */
	pxTopOfStack--;
	*pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;	/* PC */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) prvTaskExitError;	/* LR */

	/* Save code space by skipping register initialisation. */
	pxTopOfStack -= 5;	/* R12, R3, R2 and R1. */
	*pxTopOfStack = ( StackType_t ) pvParameters;	/* R0 */

	/* A save method is being used that requires each task to maintain its
	own exec return value. */
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_EXC_RETURN;

	pxTopOfStack -= 8;	/* R11, R10, R9, R8, R7, R6, R5 and R4. */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT( uxCriticalNesting == ~0UL );
	portDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
	/* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.
	See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html */
	configASSERT( configMAX_SYSCALL_INTERRUPT_PRIORITY );

	/* This port can be used on all revisions of the Cortex-M7 core other than
	the r0p1 parts.  r0p1 parts should use the port from the
	/source/portable/GCC/ARM_CM7/r0p1 directory. */
	configASSERT( portCPUID != portCORTEX_M7_r0p1_ID );
	configASSERT( portCPUID != portCORTEX_M7_r0p0_ID );

	#if( configASSERT_DEFINED == 1 )
	{
		volatile uint32_t ulOriginalPriority;
		volatile uint8_t * const pucFirstUserPriorityRegister = ( volatile uint8_t * const ) ( portNVIC_IP_REGISTERS_OFFSET_16 + portFIRST_USER_INTERRUPT_NUMBER );
		volatile uint8_t ucMaxPriorityValue;

		/* Determine the maximum priority from which ISR safe FreeRTOS API
		functions can be called.  ISR safe functions are those that end in
		"FromISR".  FreeRTOS maintains separate thread and ISR API functions to
		ensure interrupt entry is as fast and simple as possible.

		Save the interrupt priority value that is about to be clobbered. */
		ulOriginalPriority = *pucFirstUserPriorityRegister;

		/* Determine the number of priority bits available.  First write to all
		possible bits. */
		*pucFirstUserPriorityRegister = portMAX_8_BIT_VALUE;

		/* Read the value back to see how many bits stuck. */
		ucMaxPriorityValue = *pucFirstUserPriorityRegister;

		/* Use the same mask on the maximum system call priority. */
		ucMaxSysCallPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY & ucMaxPriorityValue;

		/* Calculate the maximum acceptable priority group value for the number
		of bits read back. */
		ulMaxPRIGROUPValue = portMAX_PRIGROUP_BITS;
		while( ( ucMaxPriorityValue & portTOP_BIT_OF_BYTE ) == portTOP_BIT_OF_BYTE )
		{
			ulMaxPRIGROUPValue--;
			ucMaxPriorityValue <<= ( uint8_t ) 0x01;
		}

		#ifdef __NVIC_PRIO_BITS
		{
			/* Check the CMSIS configuration that defines the number of
			priority bits matches the number of priority bits actually queried
			from the hardware. */
			configASSERT( ( portMAX_PRIGROUP_BITS - ulMaxPRIGROUPValue ) == __NVIC_PRIO_BITS );
		}
		#endif

		#ifdef configPRIO_BITS
		{
			/* Check the FreeRTOS configuration that defines the number of
			priority bits matches the number of priority bits actually queried
			from the hardware. */
			configASSERT( ( portMAX_PRIGROUP_BITS - ulMaxPRIGROUPValue ) == configPRIO_BITS );
		}
		#endif

		/* Shift the priority group value back to its position within the AIRCR
		register. */
		ulMaxPRIGROUPValue <<= portPRIGROUP_SHIFT;
		ulMaxPRIGROUPValue &= portPRIORITY_GROUP_MASK;

		/* Restore the clobbered interrupt priority register to its original
		value. */
		*pucFirstUserPriorityRegister = ulOriginalPriority;
	}
	#endif /* conifgASSERT_DEFINED */

	/* Make PendSV and SysTick the lowest priority interrupts. */
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	vPortSetupTimerInterrupt();

	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

	/* Ensure the VFP is enabled - it should be anyway. */
	vPortEnableVFP();

	/* Lazy save always. */
	*( portFPCCR ) |= portASPEN_AND_LSPEN_BITS;

	/* Start the first task. */
	vPortStartFirstTask();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( uxCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;

	/* This is not the interrupt safe version of the enter critical function so
	assert() if it is being called from an interrupt context.  Only API
	functions that end in "FromISR" can be used in an interrupt.  Only assert if
	the critical nesting count is 1 to protect against recursive calls if the
	assert function also uses a critical section. */
	if( uxCriticalNesting == 1 )
	{
		configASSERT( ( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK ) == 0 );
	}
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	configASSERT( uxCriticalNesting );
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
	/* The SysTick runs at the lowest interrupt priority, so when this interrupt
	executes all interrupts must be unmasked.  There is therefore no need to
	save and then restore the interrupt mask value as its value is already
	known. */
	portDISABLE_INTERRUPTS();
	{
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* A context switch is required.  Context switching is performed in
			the PendSV interrupt.  Pend the PendSV interrupt. */
			portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
		}
	}
	portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

#if( configASSERT_DEFINED == 1 )

	void vPortValidateInterruptPriority( void )
	{
	uint32_t ulCurrentInterrupt;
	uint8_t ucCurrentPriority;

		/* Obtain the number of the currently executing interrupt. */
		__asm volatile( "mrs %0, ipsr" : "=r"( ulCurrentInterrupt ) :: "memory" );

		/* Is the interrupt number a user defined interrupt? */
		if( ulCurrentInterrupt >= portFIRST_USER_INTERRUPT_NUMBER )
		{
			/* Look up the interrupt's priority. */
			ucCurrentPriority = pcInterruptPriorityRegisters[ ulCurrentInterrupt ];

			/* The following assertion will fail if a service routine (ISR) for
			an interrupt that has been assigned a priority above
			configMAX_SYSCALL_INTERRUPT_PRIORITY calls an ISR safe FreeRTOS API
			function.  ISR safe FreeRTOS API functions must *only* be called
			from interrupts that have been assigned a priority at or below
			configMAX_SYSCALL_INTERRUPT_PRIORITY.

			Numerically low interrupt priority numbers represent logically high
			interrupt priorities, therefore the priority of the interrupt must
			be set to a value equal to or numerically *higher* than
			configMAX_SYSCALL_INTERRUPT_PRIORITY.

			Interrupts that	use the FreeRTOS API must not be left at their
			default priority of	zero as that is the highest possible priority,
			which is guaranteed to be above configMAX_SYSCALL_INTERRUPT_PRIORITY,
			and	therefore also guaranteed to be invalid.

			FreeRTOS maintains separate thread and ISR API functions to ensure
			interrupt entry is as fast and simple as possible.

			The following links provide detailed information:
			http://www.freertos.org/RTOS-Cortex-M3-M4.html
			http://www.freertos.org/FAQHelp.html */
			configASSERT( ucCurrentPriority >= ucMaxSysCallPriority );
		}

		/* Priority grouping:  The interrupt controller (NVIC) allows the bits
		that define each interrupt's priority to be split between bits that
		define the interrupt's pre-emption priority bits and bits that define
		the interrupt's sub-priority.  For simplicity all bits must be defined
		to be pre-emption priority bits.  The following assertion will fail if
		this is not the case (if some bits represent a sub-priority).

		If the application only uses CMSIS libraries for interrupt
		configuration then the correct setting can be achieved on all Cortex-M
		devices by calling NVIC_SetPriorityGrouping( 0 ); before starting the
		scheduler.  Note however that some vendor specific peripheral libraries
		assume a non-zero priority group setting, in which cases using a value
		of zero will result in unpredictable behaviour. */
		configASSERT( ( portAIRCR_REG & portPRIORITY_GROUP_MASK ) <= ulMaxPRIGROUPValue );
	}

#endif /* configASSERT_DEFINED */
