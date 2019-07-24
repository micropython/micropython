/*
 * FreeRTOS Kernel V10.0.0
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
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
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

/* This file was separated from original port.c by SDK team to enable efective tickless idle mode implementation. */

#include "FreeRTOS.h"
#include "task.h"
#include "fsl_tickless_generic.h"

extern uint32_t SystemCoreClock; /* in Kinetis SDK, this contains the system core clock speed */

/*-----------------------------------------------------------*/

/*
 * The number of SysTick increments that make up one tick period.
 */
#if configUSE_TICKLESS_IDLE == 1
	static uint32_t ulTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if configUSE_TICKLESS_IDLE == 1
	static uint32_t xMaximumPossibleSuppressedTicks = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
#if configUSE_TICKLESS_IDLE == 1
	static uint32_t ulStoppedTimerCompensation = 0;
#endif /* configUSE_TICKLESS_IDLE */


/*-----------------------------------------------------------*/

#if( configUSE_TICKLESS_IDLE == 1 )

	__attribute__((weak)) void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
	{
	uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements;
	TickType_t xModifiableIdleTime;

		/* Make sure the SysTick reload value does not overflow the counter. */
		if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
		{
			xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
		}

		/* Stop the SysTick momentarily.  The time the SysTick is stopped for
		is accounted for as best it can be, but using the tickless mode will
		inevitably result in some tiny drift of the time maintained by the
		kernel with respect to calendar time. */
		portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

		/* Calculate the reload value required to wait xExpectedIdleTime
		tick periods.  -1 is used because this code will execute part way
		through one of the tick periods. */
		ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE_REG + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );
		if( ulReloadValue > ulStoppedTimerCompensation )
		{
			ulReloadValue -= ulStoppedTimerCompensation;
		}

		/* Enter a critical section but don't use the taskENTER_CRITICAL()
		method as that will mask interrupts that should exit sleep mode. */
		__asm volatile( "cpsid i" ::: "memory" );
		__asm volatile( "dsb" );
		__asm volatile( "isb" );

		/* If a context switch is pending or a task is waiting for the scheduler
		to be unsuspended then abandon the low power entry. */
		if( eTaskConfirmSleepModeStatus() == eAbortSleep )
		{
			/* Restart from whatever is left in the count register to complete
			this tick period. */
			portNVIC_SYSTICK_LOAD_REG = portNVIC_SYSTICK_CURRENT_VALUE_REG;

			/* Restart SysTick. */
			portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

			/* Reset the reload register to the value required for normal tick
			periods. */
			portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

			/* Re-enable interrupts - see comments above the cpsid instruction()
			above. */
			__asm volatile( "cpsie i" ::: "memory" );
		}
		else
		{
			/* Set the new reload value. */
			portNVIC_SYSTICK_LOAD_REG = ulReloadValue;

			/* Clear the SysTick count flag and set the count value back to
			zero. */
			portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

			/* Restart SysTick. */
			portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

			/* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
			set its parameter to 0 to indicate that its implementation contains
			its own wait for interrupt or wait for event instruction, and so wfi
			should not be executed again.  However, the original expected idle
			time variable must remain unmodified, so a copy is taken. */
			xModifiableIdleTime = xExpectedIdleTime;
			configPRE_SLEEP_PROCESSING( xModifiableIdleTime );
			if( xModifiableIdleTime > 0 )
			{
				__asm volatile( "dsb" ::: "memory" );
				__asm volatile( "wfi" );
				__asm volatile( "isb" );
			}
			configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

			/* Re-enable interrupts to allow the interrupt that brought the MCU
			out of sleep mode to execute immediately.  see comments above
			__disable_interrupt() call above. */
			__asm volatile( "cpsie i" ::: "memory" );
			__asm volatile( "dsb" );
			__asm volatile( "isb" );

			/* Disable interrupts again because the clock is about to be stopped
			and interrupts that execute while the clock is stopped will increase
			any slippage between the time maintained by the RTOS and calendar
			time. */
			__asm volatile( "cpsid i" ::: "memory" );
			__asm volatile( "dsb" );
			__asm volatile( "isb" );

			/* Disable the SysTick clock without reading the
			portNVIC_SYSTICK_CTRL_REG register to ensure the
			portNVIC_SYSTICK_COUNT_FLAG_BIT is not cleared if it is set.  Again,
			the time the SysTick is stopped for is accounted for as best it can
			be, but using the tickless mode will inevitably result in some tiny
			drift of the time maintained by the kernel with respect to calendar
			time*/
			portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT );

			/* Determine if the SysTick clock has already counted to zero and
			been set back to the current reload value (the reload back being
			correct for the entire expected idle time) or if the SysTick is yet
			to count to zero (in which case an interrupt other than the SysTick
			must have brought the system out of sleep mode). */
			if( ( portNVIC_SYSTICK_CTRL_REG & portNVIC_SYSTICK_COUNT_FLAG_BIT ) != 0 )
			{
				uint32_t ulCalculatedLoadValue;

				/* The tick interrupt is already pending, and the SysTick count
				reloaded with ulReloadValue.  Reset the
				portNVIC_SYSTICK_LOAD_REG with whatever remains of this tick
				period. */
				ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL ) - ( ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE_REG );

				/* Don't allow a tiny value, or values that have somehow
				underflowed because the post sleep hook did something
				that took too long. */
				if( ( ulCalculatedLoadValue < ulStoppedTimerCompensation ) || ( ulCalculatedLoadValue > ulTimerCountsForOneTick ) )
				{
					ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL );
				}

				portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;

				/* As the pending tick will be processed as soon as this
				function exits, the tick value maintained by the tick is stepped
				forward by one less than the time spent waiting. */
				ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
			}
			else
			{
				/* Something other than the tick interrupt ended the sleep.
				Work out how long the sleep lasted rounded to complete tick
				periods (not the ulReload value which accounted for part
				ticks). */
				ulCompletedSysTickDecrements = ( xExpectedIdleTime * ulTimerCountsForOneTick ) - portNVIC_SYSTICK_CURRENT_VALUE_REG;

				/* How many complete tick periods passed while the processor
				was waiting? */
				ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

				/* The reload value is set to whatever fraction of a single tick
				period remains. */
				portNVIC_SYSTICK_LOAD_REG = ( ( ulCompleteTickPeriods + 1UL ) * ulTimerCountsForOneTick ) - ulCompletedSysTickDecrements;
			}

			/* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
			again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
			value. */
			portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
			portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
			vTaskStepTick( ulCompleteTickPeriods );
			portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

			/* Exit with interrpts enabled. */
			__asm volatile( "cpsie i" ::: "memory" );
		}
	}

#endif /* #if configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
__attribute__(( weak )) void vPortSetupTimerInterrupt( void )
{
	/* Calculate the constants required to configure the tick interrupt. */
	#if( configUSE_TICKLESS_IDLE == 1 )
	{
		ulTimerCountsForOneTick = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ );
		xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
		ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / ( configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ );
	}
	#endif /* configUSE_TICKLESS_IDLE */

	/* Stop and clear the SysTick. */
	portNVIC_SYSTICK_CTRL_REG = 0UL;
	portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

	/* Configure SysTick to interrupt at the requested rate. */
	portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
	portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );
}
/*-----------------------------------------------------------*/
