/**
 ******************************************************************************
 * @file    platform_flash_modules.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    27-Jan-2015
 * @brief   Platform specific flash module units in application dct
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_FLASH_MODULES_H
#define __PLATFORM_FLASH_MODULES_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct platform_flash_modules {
    uint16_t magicNumber;
    uint8_t sourceDeviceID;
    uint8_t destinationDeviceID;
    uint32_t sourceAddress;
    uint32_t destinationAddress;
    uint32_t length;
    uint8_t flags;
    uint8_t module_function;
    uint8_t reserved[2];
} platform_flash_modules_t; //1 module instance => 20 bytes


#ifdef	__cplusplus
}
#endif

#endif	/* __PLATFORM_FLASH_MODULES_H */

