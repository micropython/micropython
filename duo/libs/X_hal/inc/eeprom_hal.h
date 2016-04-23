/**
 ******************************************************************************
 * @file    eeprom_hal.h
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
#ifndef __EEPROM_HAL_H
#define __EEPROM_HAL_H

#include <stdint.h>
#include <stddef.h>

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/* Internal Flash Page size = 1KByte */
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif


void HAL_EEPROM_Init(void);
uint8_t HAL_EEPROM_Read(uint32_t address);
void HAL_EEPROM_Write(uint32_t address, uint8_t data);
size_t HAL_EEPROM_Length();

#ifdef __cplusplus
}
#endif

#endif  /* __EEPROM_HAL_H */
