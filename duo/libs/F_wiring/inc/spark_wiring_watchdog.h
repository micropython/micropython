/**
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

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

#include "spark_wiring_thread.h"
#include "delay_hal.h"
#include "timer_hal.h"

#if PLATFORM_THREADING


class ApplicationWatchdog
{
	volatile system_tick_t timeout;
	static volatile system_tick_t last_checkin;

	std::function<void(void)> timeout_fn;

	Thread thread;

	static void start(void* pointer);

	void loop();

public:

	ApplicationWatchdog(unsigned timeout_ms, std::function<void(void)> fn, unsigned stack_size=512) : timeout(timeout_ms), timeout_fn(fn),
		thread("appwdt", start, this, OS_THREAD_PRIORITY_CRITICAL, stack_size)
	{
		checkin();
	}

	bool isComplete()
	{
		return !timeout_fn;
	}

	static inline system_tick_t current_time()
	{
		return HAL_Timer_Get_Milli_Seconds();
	}

	bool has_timedout()
	{
		return (current_time()-last_checkin)>=timeout;
	}

	/**
	 * Dispose of this thread next time it wakes up.
	 */
	void dispose()
	{
		timeout = 0;
	}

	/**
	 * Lifesign that the application is still working normally.
	 */
	static void checkin()
	{
		last_checkin = current_time();
	}

};

inline void application_checkin() { ApplicationWatchdog::checkin(); }

#else

inline void application_checkin() {  }

#endif

