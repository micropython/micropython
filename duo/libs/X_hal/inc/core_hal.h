/**
 ******************************************************************************
 * @file    core_hal.h
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
#ifndef __CORE_HAL_H
#define __CORE_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    BKP_DR_01 = 0x01,
    BKP_DR_02 = 0x02,
    BKP_DR_03 = 0x03,
    BKP_DR_04 = 0x04,
    BKP_DR_05 = 0x05,
    BKP_DR_06 = 0x06,
    BKP_DR_07 = 0x07,
    BKP_DR_08 = 0x08,
    BKP_DR_09 = 0x09,
    BKP_DR_10 = 0x10
} BKP_DR_TypeDef;

typedef enum
{
    PIN_RESET = 0x01,
    SOFTWARE_RESET = 0x02,
    WATCHDOG_RESET = 0x03,
    LOW_POWER_RESET = 0x04
} RESET_TypeDef;

/* Exported constants --------------------------------------------------------*/

//Following is normally defined via "CFLAGS += -DDFU_BUILD_ENABLE" in makefile
#ifndef DFU_BUILD_ENABLE
#define DFU_BUILD_ENABLE
#endif

/*
 * Use the JTAG IOs as standard GPIOs (D3 to D7)
 * Note that once the JTAG IOs are disabled, the connection with the host debugger
 * is lost and cannot be re-established as long as the JTAG IOs remain disabled.
 */

#ifdef USE_SWD_JTAG
#define SWD_JTAG_ENABLE
#else
#ifdef USE_SWD
#define SWD_ENABLE_JTAG_DISABLE
#else
#define SWD_JTAG_DISABLE
#endif
#endif
/*
 * Use Independent Watchdog to force a system reset when a software error occurs
 * During JTAG program/debug, the Watchdog counting is disabled by debug configuration
 */
#define IWDG_RESET_ENABLE
#define TIMING_IWDG_RELOAD      1000 //1sec

/* Exported functions --------------------------------------------------------*/
#include "watchdog_hal.h"
#include "core_subsys_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_Core_Init(void);
void HAL_Core_Config(void);
bool HAL_Core_Validate_User_Module(void);
bool HAL_Core_Mode_Button_Pressed(uint16_t pressedMillisDuration);
void HAL_Core_Mode_Button_Reset(void);
void HAL_Core_System_Reset(void);
void HAL_Core_Factory_Reset(void);

/**
 * Notification from hal to the external system.
 * @param button    The button that was pressed, 0-based
 * @param state     The current state of the button.
 */
void HAL_Notify_Button_State(uint8_t button, uint8_t state);

void HAL_Core_Enter_Safe_Mode(void* reserved);
void HAL_Core_Enter_Bootloader(bool persist);
void HAL_Core_Enter_Stop_Mode(uint16_t wakeUpPin, uint16_t edgeTriggerMode, long seconds);
void HAL_Core_Execute_Stop_Mode(void);
void HAL_Core_Enter_Standby_Mode(void);
void HAL_Core_Execute_Standby_Mode(void);
uint32_t HAL_Core_Compute_CRC32(const uint8_t *pBuffer, uint32_t bufferSize);

typedef enum _BootloaderFlag_t {
    BOOTLOADER_FLAG_VERSION,
    BOOTLOADER_FLAG_STARTUP_MODE
} BootloaderFlag;

enum BootloaderFeaturesEnabled
{
    BL_FEATURE_SAFE_MODE = 1<<0,
    BL_FEATURE_DFU_MODE = 1<<1,
    BL_FEATURE_FACTORY_RESET = 1<<2,
    BL_BUTTON_FEATURES = (BL_FEATURE_SAFE_MODE|BL_FEATURE_DFU_MODE|BL_FEATURE_FACTORY_RESET)
};



uint16_t HAL_Bootloader_Get_Flag(BootloaderFlag flag);

//Following is currently defined in bootloader/src/core-vx/dfu_hal.c
//Move the definitions to core_hal.c and add hal as a dependency for bootloader
int32_t HAL_Core_Backup_Register(uint32_t BKP_DR);
void HAL_Core_Write_Backup_Register(uint32_t BKP_DR, uint32_t Data);
uint32_t HAL_Core_Read_Backup_Register(uint32_t BKP_DR);

void HAL_SysTick_Handler(void);

void HAL_Bootloader_Lock(bool lock);

bool HAL_Core_System_Reset_FlagSet(RESET_TypeDef resetType);


typedef struct runtime_info_t {
    uint16_t size;              /* Size of this struct. */
    uint16_t flags;             /* reserved, set to 0. */
    uint32_t freeheap;          /* Amount of guaranteed heap memory available. */
    uint32_t system_version;
} runtime_info_t;

uint32_t HAL_Core_Runtime_Info(runtime_info_t* info, void* reserved);

extern void app_setup_and_loop();

typedef enum HAL_SystemClock
{
    SYSTEMCLOCK_PRIMARY,
    SYSTEMCLOCK_SPI
} HAL_SystemClock;

/**
 * Retrieves the
 * @param reserved
 * @return
 */
unsigned HAL_Core_System_Clock(HAL_SystemClock clock, void* reserved);


typedef enum hal_system_config_t
{
    SYSTEM_CONFIG_NONE,
    SYSTEM_CONFIG_DEVICE_KEY,
    SYSTEM_CONFIG_SERVER_KEY,

    SYSTEM_CONFIG_SOFTAP_PREFIX,
    SYSTEM_CONFIG_SOFTAP_SUFFIX,
    SYSTEM_CONFIG_SOFTAP_HOSTNAMES

} hal_system_config_t;

/**
 * Sets a system configuration item.
 * @param config_item       The item to set
 * @param data              The data to set to
 * @param length            The length of the data.
 * @return      0 on success.
 */
int HAL_Set_System_Config(hal_system_config_t config_item, const void* data, unsigned length);


typedef enum HAL_Feature {
    FEATURE_RETAINED_MEMORY=1,       // [write only] retained memory on backup power
    FEATURE_WARM_START,              // [read only] set to true if previous retained memory contents are available]
	FEATURE_CLOUD_UDP,				// [read only] true if the UDP implementation should be used.
} HAL_Feature;

int HAL_Feature_Set(HAL_Feature feature, bool enabled);
bool HAL_Feature_Get(HAL_Feature feature);

/**
 * Externally defined function that is called before user constructors.
 */
extern void module_user_init_hook(void);

int HAL_System_Backup_Save(size_t offset, const void* buffer, size_t length, void* reserved);
int HAL_System_Backup_Restore(size_t offset, void* buffer, size_t max_length, size_t* length, void* reserved);


#ifdef __cplusplus
}
#endif

#endif  /* __CORE_HAL_H */
