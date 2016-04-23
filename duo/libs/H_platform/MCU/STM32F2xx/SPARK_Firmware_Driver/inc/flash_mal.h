/**
 ******************************************************************************
 * @file    flash_mal.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    30-Jan-2015
 * @brief   Header for flash media access layer
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
#ifndef __FLASH_MAL_H
#define __FLASH_MAL_H

#include "hw_config.h"
#include "flash_device_hal.h"
#include "module_info.h"
#include "module_info_hal.h"

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#if (PLATFORM_ID==6 || PLATFORM_ID==8) && !defined(MODULAR_FIRMWARE)
#define COMBINED_FIRMWARE_IMAGE 1
#endif

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/
#ifndef INTERNAL_FLASH_SIZE
#   error "INTERNAL_FLASH_SIZE not defined"
#endif

/* Internal Flash memory address where various firmwares are located */
#ifndef INTERNAL_FLASH_START
#define INTERNAL_FLASH_START        ((uint32_t)0x08000000)
#endif

//Bootloader firmware at the start of internal flash
#define USB_DFU_ADDRESS             INTERNAL_FLASH_START
//Main firmware begin address after 128KB (4 x 16K + 64K) from start of flash
#define CORE_FW_ADDRESS             ((uint32_t)0x08020000)
#define APP_START_MASK              ((uint32_t)0x2FF10000)

/* Internal Flash page size */
#define INTERNAL_FLASH_PAGE_SIZE    ((uint32_t)0x20000) //128K (7 sectors of 128K each used by main firmware)

#ifdef MODULAR_FIRMWARE
    #define FACTORY_RESET_MODULE_FUNCTION MODULE_FUNCTION_USER_PART

#ifndef USER_FIRMWARE_IMAGE_LOCATION
    #error USER_FIRMWARE_IMAGE_LOCATION not defined
#endif

#ifndef USER_FIRMWARE_IMAGE_SIZE
    #error USER_FIRMWARE_IMAGE_SIZE not defined
#endif

#if PLATFORM_ID == PLATFORM_DUO_PRODUCTION
    #define FIRMWARE_IMAGE_SIZE                     0x80000 // 512K
	
    #define EXTERNAL_FLASH_OTA_ADDRESS  			((uint32_t)0xC0000) // 768K reserved for user or WICED application usage
    #define EXTERNAL_FLASH_FAC_ADDRESS  			((uint32_t)(EXTERNAL_FLASH_OTA_ADDRESS+FIRMWARE_IMAGE_SIZE))
    #define EXTERNAL_FLASH_WIFI_FIRMWARE_ADDRESS  	((uint32_t)(EXTERNAL_FLASH_FAC_ADDRESS+USER_FIRMWARE_IMAGE_SIZE))
#else
    #ifdef USER_FIRMWARE_IMAGE_SIZE
        #define FIRMWARE_IMAGE_SIZE  USER_FIRMWARE_IMAGE_SIZE
    #endif

    #define INTERNAL_FLASH_OTA_ADDRESS (USER_FIRMWARE_IMAGE_LOCATION+FIRMWARE_IMAGE_SIZE)
    #define INTERNAL_FLASH_FAC_ADDRESS (USER_FIRMWARE_IMAGE_LOCATION+FIRMWARE_IMAGE_SIZE+FIRMWARE_IMAGE_SIZE)
#endif

#else
#ifdef COMBINED_FIRMWARE_IMAGE
	#define FACTORY_RESET_MODULE_FUNCTION MODULE_FUNCTION_SYSTEM_PART
#else
    #define FACTORY_RESET_MODULE_FUNCTION MODULE_FUNCTION_MONO_FIRMWARE
#endif
    #define USER_FIRMWARE_IMAGE_LOCATION CORE_FW_ADDRESS
    #ifdef USE_SERIAL_FLASH
    #define FIRMWARE_IMAGE_SIZE     0x7E000 //504K
    #else
	#ifdef COMBINED_FIRMWARE_IMAGE
		#define FIRMWARE_IMAGE_SIZE 0x80000	// 512k system-part1+system-part2
		#define TEST_FIRMWARE_IMAGE_SIZE 0x60000	// 384k for test firmware
	#else
	// this was "true" mono firmware. We are now using a hybrid where the firmware payload is 512k
		#define FIRMWARE_IMAGE_SIZE     0x60000 //384K (monolithic firmware size)
	#endif // COMBINED_FIRMWARE_IMAGE
    #endif

#ifdef COMBINED_FIRMWARE_IMAGE
	/* Internal Flash memory address where Factory programmed monolithic core firmware is located */
	#define INTERNAL_FLASH_FAC_ADDRESS  ((uint32_t)(USER_FIRMWARE_IMAGE_LOCATION + TEST_FIRMWARE_IMAGE_SIZE))
	/* Internal Flash memory address where OTA upgraded monolithic core firmware will be saved */
	#define INTERNAL_FLASH_OTA_ADDRESS  ((uint32_t)(USER_FIRMWARE_IMAGE_LOCATION + TEST_FIRMWARE_IMAGE_SIZE))
#else
    /* Internal Flash memory address where Factory programmed monolithic core firmware is located */
    #define INTERNAL_FLASH_FAC_ADDRESS  ((uint32_t)(USER_FIRMWARE_IMAGE_LOCATION + FIRMWARE_IMAGE_SIZE))
    /* Internal Flash memory address where monolithic core firmware will be saved for backup/restore */
    //#define INTERNAL_FLASH_BKP_ADDRESS  ((uint32_t)(USER_FIRMWARE_IMAGE_LOCATION + FIRMWARE_IMAGE_SIZE))
    /* Internal Flash memory address where OTA upgraded monolithic core firmware will be saved */
    #define INTERNAL_FLASH_OTA_ADDRESS  ((uint32_t)(USER_FIRMWARE_IMAGE_LOCATION + FIRMWARE_IMAGE_SIZE))
#endif

    #ifdef USE_SERIAL_FLASH
    /* External Flash memory address where Factory programmed core firmware is located */
    #define EXTERNAL_FLASH_FAC_ADDRESS  ((uint32_t)0x4000)
    /* External Flash memory address where core firmware will be saved for backup/restore */
    #define EXTERNAL_FLASH_BKP_ADDRESS  ((uint32_t)EXTERNAL_FLASH_FAC_ADDRESS)
    /* External Flash memory address where OTA upgraded core firmware will be saved */
    #define EXTERNAL_FLASH_OTA_ADDRESS  ((uint32_t)(EXTERNAL_FLASH_FAC_ADDRESS + FIRMWARE_IMAGE_SIZE))
    #endif
#endif

#if FIRMWARE_IMAGE_SIZE > INTERNAL_FLASH_SIZE
#   error "FIRMWARE_IMAGE_SIZE too large to fit into internal flash"
#endif

/* Bootloader Flash regions that needs to be protected: 0x08000000 - 0x08003FFF */
#define BOOTLOADER_FLASH_PAGES      (OB_WRP_Sector_0)

void FLASH_WriteProtection_Enable(uint32_t FLASH_Sectors);
void FLASH_WriteProtection_Disable(uint32_t FLASH_Sectors);
uint32_t FLASH_PagesMask(uint32_t imageSize, uint32_t pageSize);


#include "flash_access.h"

#ifdef __cplusplus
}
#endif


#endif  /*__FLASH_MAL_H*/
