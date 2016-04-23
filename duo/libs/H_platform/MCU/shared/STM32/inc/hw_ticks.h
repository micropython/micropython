/**
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  Copyright 2012 STMicroelectronics
  http://www.st.com/software_license_agreement_liberty_v2

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#pragma once

#ifdef	__cplusplus
extern "C" {
#endif


#include "platform_config.h"
#include "system_tick_hal.h"

#define SYSTEM_US_TICKS		(SystemCoreClock / 1000000)//cycles per microsecond
#define SYSTEM_TICK_COUNTER     (DWT->CYCCNT)

/**
 * Increment the millisecond tick counter.
 */
void System1MsTick(void);

/**
 * Increment the microsecond tick counter.
 */
void System1UsTick(void);

/**
 * Fetch the current milliseconds count.
 * @return the number of milliseconds since the device was powered on or woken up from
 * sleep. Automatically wraps around when above UINT_MAX;
 */
system_tick_t GetSystem1MsTick();

/**
 * Fetch the current microseconds count.
 * @return the number of microseconds since the device was powered on or woken up from
 * sleep. Automatically wraps around when above UINT_MAX;
 */
system_tick_t GetSystem1UsTick();


/**
 * Testing method that simulates advancing the time forward.
 */
void __advance_system1MsTick(system_tick_t millis, system_tick_t micros_from_rollover);

void SysTick_Disable();



#ifdef	__cplusplus
}
#endif



