/**
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

#ifndef HAL_DYNALIB_CONCURRENT_H
#define	HAL_DYNALIB_CONCURRENT_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "concurrent_hal.h"
#endif

DYNALIB_BEGIN(hal_concurrent)
#if PLATFORM_THREADING
DYNALIB_FN(hal_concurrent,__gthread_equal)
DYNALIB_FN(hal_concurrent,os_thread_create)
DYNALIB_FN(hal_concurrent,os_thread_is_current)
DYNALIB_FN(hal_concurrent,os_thread_yield)
DYNALIB_FN(hal_concurrent,os_thread_join)
DYNALIB_FN(hal_concurrent,os_thread_cleanup)
DYNALIB_FN(hal_concurrent,os_thread_delay_until)
DYNALIB_FN(hal_concurrent,os_thread_scheduling)

DYNALIB_FN(hal_concurrent,os_timer_create)
DYNALIB_FN(hal_concurrent,os_timer_destroy)
DYNALIB_FN(hal_concurrent,os_timer_get_id)
DYNALIB_FN(hal_concurrent,os_timer_change)

DYNALIB_FN(hal_concurrent,os_mutex_create)
DYNALIB_FN(hal_concurrent,os_mutex_destroy)
DYNALIB_FN(hal_concurrent,os_mutex_lock)
DYNALIB_FN(hal_concurrent,os_mutex_trylock)
DYNALIB_FN(hal_concurrent,os_mutex_unlock)

DYNALIB_FN(hal_concurrent,os_mutex_recursive_create)
DYNALIB_FN(hal_concurrent,os_mutex_recursive_destroy)
DYNALIB_FN(hal_concurrent,os_mutex_recursive_lock)
DYNALIB_FN(hal_concurrent,os_mutex_recursive_trylock)
DYNALIB_FN(hal_concurrent,os_mutex_recursive_unlock)

#endif
DYNALIB_END(hal_concurrent)



#endif	/* HAL_DYNALIB_CONCURRENT_H */

