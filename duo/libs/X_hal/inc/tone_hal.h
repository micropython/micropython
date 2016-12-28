/**
 ******************************************************************************
 * @file    tone_hal.h
 * @author  Satish Nair, Brett Walach
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
#ifndef __TONE_HAL_H
#define __TONE_HAL_H

/* Includes ------------------------------------------------------------------*/
#include "pinmap_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

void HAL_Tone_Start(uint8_t pin, uint32_t frequency, uint32_t duration);
void HAL_Tone_Stop(uint8_t pin);
uint32_t HAL_Tone_Get_Frequency(uint8_t pin);
bool HAL_Tone_Is_Stopped(uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif  /* __TONE_HAL_H */
