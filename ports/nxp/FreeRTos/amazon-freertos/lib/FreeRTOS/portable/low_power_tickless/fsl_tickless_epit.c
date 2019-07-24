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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
/* Compiler includes. */
#if defined(__ICCARM__)
#include <intrinsics.h>
#endif

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_tickless_generic.h"

#if configUSE_TICKLESS_IDLE == 1
#include "fsl_epit.h"
#endif

extern uint32_t SystemCoreClock; /* in Kinetis SDK, this contains the system core clock speed */

/*
 * EPIT timer base address and interrupt number
 */

#if configUSE_TICKLESS_IDLE == 1
	extern EPIT_Type *vPortGetEpitBase(void);
	extern IRQn_Type vPortGetEpitIrqn(void);
#endif /* configUSE_TICKLESS_IDLE */

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

/*
 * The number of EPIT increments that make up one tick period.
 */
#if configUSE_TICKLESS_IDLE == 1
	static uint32_t ulEPITimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The flag of EPIT is occurs or not.
 */
#if configUSE_TICKLESS_IDLE == 1
	static volatile bool ulEPITimerInterruptFired = false;
#endif /* configUSE_TICKLESS_IDLE */

#if configUSE_TICKLESS_IDLE == 1
	void vPortEpitIsr(void)
	{
		ulEPITimerInterruptFired = true;
		EPIT_ClearStatusFlags(vPortGetEpitBase(), kEPIT_OutputCompareFlag);
	}

	void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
	{
		uint32_t ulReloadValue, ulCompleteTickPeriods;
		TickType_t xModifiableIdleTime;
		EPIT_Type *pxEpitBase;

		if (ulTimerCountsForOneTick == 0)
		{
			ulTimerCountsForOneTick = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ);
			ulStoppedTimerCompensation = 0;
			ulEPITimerCountsForOneTick = configEPIT_CLOCK_HZ / configTICK_RATE_HZ;
			xMaximumPossibleSuppressedTicks = portMAX_32_BIT_NUMBER / ulEPITimerCountsForOneTick;
		}

		pxEpitBase = vPortGetEpitBase();
		if (pxEpitBase == 0)
			return;
		/* Make sure the SysTick reload value does not overflow the counter. */
		if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks)
		{
			xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
		}
		if (xExpectedIdleTime == 0)
			return;

		/* Calculate the reload value required to wait xExpectedIdleTime
		tick periods.  -1 is used because this code will execute part way
		through one of the tick periods. */
		ulReloadValue = (ulEPITimerCountsForOneTick * (xExpectedIdleTime - 1UL));
		if (ulReloadValue > ulStoppedTimerCompensation)
		{
			ulReloadValue -= ulStoppedTimerCompensation;
		}

		/* Stop the EPIT and systick momentarily.  The time the EPIT and systick is stopped for
		is accounted for as best it can be, but using the tickless mode will
		inevitably result in some tiny drift of the time maintained by the
		kernel with respect to calendar time. */
		EPIT_StopTimer(pxEpitBase);
		portGIC_SYSTICK_CTRL_REG &= ~portGIC_SYSTICK_ENABLE_BIT;

		/* Enter a critical section but don't use the taskENTER_CRITICAL()
		method as that will mask interrupts that should exit sleep mode. */
		__disable_irq();
		__DSB();
		__ISB();

		/* If a context switch is pending or a task is waiting for the scheduler
		to be unsuspended then abandon the low power entry. */
		if (eTaskConfirmSleepModeStatus() == eAbortSleep)
		{
			/* Restart from whatever is left in the count register to complete
			this tick period. */
			portGIC_SYSTICK_LOAD_REG = portGIC_SYSTICK_CURRENT_VALUE_REG;

			/* Restart SysTick. */
			portGIC_SYSTICK_CTRL_REG |= portGIC_SYSTICK_ENABLE_BIT;

			/* Reset the reload register to the value required for normal tick
			periods. */
			portGIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

			/* Re-enable interrupts - see comments above __disable_irq()
			call above. */
			__enable_irq();
		}
		else
		{
			/* Set the new reload value. */
			EPIT_SetTimerPeriod(pxEpitBase, ulReloadValue);
			EPIT_SetOutputCompareValue(pxEpitBase, 0);

			/* Enable EPIT. */
			EPIT_StartTimer(pxEpitBase);

			/* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
			set its parameter to 0 to indicate that its implementation contains
			its own wait for interrupt or wait for event instruction, and so wfi
			should not be executed again.  However, the original expected idle
			time variable must remain unmodified, so a copy is taken. */
			xModifiableIdleTime = xExpectedIdleTime;
			configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
			if (xModifiableIdleTime > 0)
			{
				__DSB();
				__WFI();
				__ISB();
			}
			configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

			ulEPITimerInterruptFired = false;

			/* Re-enable interrupts - see comments above __disable_irq()
			call above. */
			__enable_irq();
			__NOP();
			if (ulEPITimerInterruptFired)
			{
				/* The tick interrupt handler will already have pended the tick
				processing in the kernel.  As the pending tick will be
				processed as soon as this function exits, the tick value
				maintained by the tick is stepped forward by one less than the
				time spent waiting. */
				ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
				ulEPITimerInterruptFired = false;
			}
			else
			{
				/* Something other than the tick interrupt ended the sleep.
				Work out how long the sleep lasted rounded to complete tick
				periods (not the ulReload value which accounted for part
				ticks). */
				ulCompleteTickPeriods =
					(ulReloadValue - EPIT_GetCurrentTimerCount(pxEpitBase)) / ulEPITimerCountsForOneTick;
			}

			/* Stop EPIT when CPU waked up then set portGIC_SYSTICK_LOAD_REG back to its standard
			value.  The critical section is used to ensure the tick interrupt
			can only execute once in the case that the reload register is near
			zero. */
			EPIT_StopTimer(pxEpitBase);
			portENTER_CRITICAL();
			{
				portGIC_SYSTICK_CTRL_REG |= portGIC_SYSTICK_ENABLE_BIT;
				vTaskStepTick(ulCompleteTickPeriods);
				portGIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;
			}
			portEXIT_CRITICAL();
		}
	}
#endif /* #if configUSE_TICKLESS_IDLE */
