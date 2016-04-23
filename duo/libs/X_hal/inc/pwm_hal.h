/**
 ******************************************************************************
 * @file    pwm_hal.h
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
#ifndef __PWM_HAL_H
#define __PWM_HAL_H

/* Includes ------------------------------------------------------------------*/
#include "pinmap_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* This perhaps should be moved in a different place. */
typedef enum
{
	PWM_INIT = 1,
	/* add other flags here*/

}user_property_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

void HAL_PWM_Write(uint16_t pin, uint8_t value);
void HAL_PWM_Write_With_Frequency(uint16_t pin, uint8_t value, uint16_t pwm_frequency);
uint16_t HAL_PWM_Get_Frequency(uint16_t pin);
uint16_t HAL_PWM_Get_AnalogValue(uint16_t pin);
void HAL_PWM_UpdateDutyCycle(uint16_t pin, uint16_t value);

#ifdef __cplusplus
}
#endif

#endif  /* __PWM_HAL_H */
