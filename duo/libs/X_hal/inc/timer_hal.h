/**
 ******************************************************************************
 * @file    timer_hal.h
 * @authors Satish Nair, Brett Walach
 * @version V1.0.0
 * @date    12-Sept-2014
 * @brief
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMER_HAL_H
#define __TIMER_HAL_H

/* Includes ------------------------------------------------------------------*/
#include "system_tick_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif


system_tick_t HAL_Timer_Get_Micro_Seconds(void);
system_tick_t HAL_Timer_Get_Milli_Seconds(void);

#define HAL_Timer_Microseconds HAL_Timer_Get_Micro_Seconds
#define HAL_Timer_Milliseconds HAL_Timer_Get_Milli_Seconds

#ifdef __cplusplus
}
#endif

#endif  /* __TIMER_HAL_H */
