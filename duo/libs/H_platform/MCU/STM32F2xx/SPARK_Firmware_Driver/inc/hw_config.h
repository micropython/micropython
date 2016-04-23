/**
 ******************************************************************************
 * @file    hw_config.h
 * @author  Satish Nair
 * @version V1.0.0
 * @date    22-Oct-2014
 * @brief   Hardware Configuration & Setup
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
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H
/* Includes ------------------------------------------------------------------*/

#include <limits.h>
#include "platform_config.h"
#include "config.h"
#include "rgbled.h"
#include "flash_mal.h"
#include "system_tick_hal.h"
#include "usb_hal.h"
#include "platform_system_flags.h"
#include "hw_ticks.h"
#if defined(HAS_SERIAL_FLASH)
#include "spi_flash.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
typedef enum
{
	BUTTON1 = 0
} Button_TypeDef;

typedef enum
{
	BUTTON_MODE_GPIO = 0, BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

/* Exported constants --------------------------------------------------------*/

/* Exported macros ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void NVIC_Configuration(void);
void SysTick_Configuration(void);

/**
 * Enables the WDT if the system flags allow it.
 */
void IWDG_Reset_Enable(uint32_t msTimeout);

/**
 * Unconditionally enables the WDT.
 */
void IWDG_Force_Enable(uint32_t msTimeout);


void UI_Timer_Configure(void);

void BUTTON_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
void BUTTON_EXTI_Config(Button_TypeDef Button, FunctionalState NewState);
uint8_t BUTTON_GetState(Button_TypeDef Button);
uint16_t BUTTON_GetDebouncedTime(Button_TypeDef Button);
void BUTTON_ResetDebouncedState(Button_TypeDef Button);

#ifdef HAS_SERIAL_FLASH
/* Serial Flash Hardware related methods */
void sFLASH_SPI_DeInit(void);
void sFLASH_SPI_Init(void);
void sFLASH_CS_LOW(void);
void sFLASH_CS_HIGH(void);
#endif

void USB_Cable_Config(FunctionalState NewState);

#define SYSTEM_FLAG(x) (system_flags.x)
void Load_SystemFlags(void);
void Save_SystemFlags(void);
extern platform_system_flags_t system_flags;

#if PLATFORM_ID == 88
#define EXTRA_SYSTEM_FLAG(x) (extra_system_flags.x)

extern extra_platform_system_flags_t extra_system_flags;

void Load_ExtraSystemFlags(void);
void Save_ExtraSystemFlags(void);
#endif

bool FACTORY_Flash_Reset(void);
void BACKUP_Flash_Reset(void);
void OTA_Flash_Reset(void);

bool OTA_Flashed_GetStatus(void);
void OTA_Flashed_ResetStatus(void);

void Finish_Update(void);

uint16_t Bootloader_Get_Version(void);
void Bootloader_Update_Version(uint16_t bootloaderVersion);

uint32_t Compute_CRC32(const uint8_t *pBuffer, uint32_t bufferSize);

/* External variables --------------------------------------------------------*/
extern uint8_t USE_SYSTEM_FLAGS;

extern volatile uint32_t TimingDelay;

#define KICK_WDT() IWDG_ReloadCounter()
void Save_Reset_Syndrome();


#ifdef __cplusplus
}
#endif

#endif  /*__HW_CONFIG_H*/
