/**
  ******************************************************************************
  * @file    stm32f7xx_hal_flash_ex.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   Header file of FLASH HAL Extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_HAL_FLASH_EX_H
#define __STM32F7xx_HAL_FLASH_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup FLASHEx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup FLASHEx_Exported_Types FLASH Exported Types
  * @{
  */

/**
  * @brief  FLASH Erase structure definition
  */
typedef struct
{
  uint32_t TypeErase;   /*!< Mass erase or sector Erase.
                             This parameter can be a value of @ref FLASHEx_Type_Erase */

  uint32_t Sector;      /*!< Initial FLASH sector to erase when Mass erase is disabled
                             This parameter must be a value of @ref FLASHEx_Sectors */

  uint32_t NbSectors;   /*!< Number of sectors to be erased.
                             This parameter must be a value between 1 and (max number of sectors - value of Initial sector)*/

  uint32_t VoltageRange;/*!< The device voltage range which defines the erase parallelism
                             This parameter must be a value of @ref FLASHEx_Voltage_Range */

} FLASH_EraseInitTypeDef;

/**
  * @brief  FLASH Option Bytes Program structure definition
  */
typedef struct
{
  uint32_t OptionType;   /*!< Option byte to be configured.
                              This parameter can be a value of @ref FLASHEx_Option_Type */

  uint32_t WRPState;     /*!< Write protection activation or deactivation.
                              This parameter can be a value of @ref FLASHEx_WRP_State */

  uint32_t WRPSector;         /*!< Specifies the sector(s) to be write protected.
                              The value of this parameter depend on device used within the same series */

  uint32_t RDPLevel;     /*!< Set the read protection level.
                              This parameter can be a value of @ref FLASHEx_Option_Bytes_Read_Protection */

  uint32_t BORLevel;     /*!< Set the BOR Level.
                              This parameter can be a value of @ref FLASHEx_BOR_Reset_Level */

  uint32_t USERConfig;   /*!< Program the FLASH User Option Byte: WWDG_SW / IWDG_SW / RST_STOP / RST_STDBY /
                              IWDG_FREEZE_STOP / IWDG_FREEZE_SANDBY. */

  uint32_t BootAddr0;    /*!< Boot base address when Boot pin = 0.
                              This parameter can be a value of @ref FLASHEx_Boot_Address */

  uint32_t BootAddr1;    /*!< Boot base address when Boot pin = 1.
                              This parameter can be a value of @ref FLASHEx_Boot_Address */

} FLASH_OBProgramInitTypeDef;

/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/

/** @defgroup FLASHEx_Exported_Constants FLASH Exported Constants
  * @{
  */

/** @defgroup FLASHEx_Type_Erase FLASH Type Erase
  * @{
  */
#define FLASH_TYPEERASE_SECTORS         ((uint32_t)0x00)  /*!< Sectors erase only          */
#define FLASH_TYPEERASE_MASSERASE       ((uint32_t)0x01)  /*!< Flash Mass erase activation */
/**
  * @}
  */

/** @defgroup FLASHEx_Voltage_Range FLASH Voltage Range
  * @{
  */
#define FLASH_VOLTAGE_RANGE_1        ((uint32_t)0x00)  /*!< Device operating range: 1.8V to 2.1V                */
#define FLASH_VOLTAGE_RANGE_2        ((uint32_t)0x01)  /*!< Device operating range: 2.1V to 2.7V                */
#define FLASH_VOLTAGE_RANGE_3        ((uint32_t)0x02)  /*!< Device operating range: 2.7V to 3.6V                */
#define FLASH_VOLTAGE_RANGE_4        ((uint32_t)0x03)  /*!< Device operating range: 2.7V to 3.6V + External Vpp */
/**
  * @}
  */

/** @defgroup FLASHEx_WRP_State FLASH WRP State
  * @{
  */
#define OB_WRPSTATE_DISABLE       ((uint32_t)0x00)  /*!< Disable the write protection of the desired bank 1 sectors */
#define OB_WRPSTATE_ENABLE        ((uint32_t)0x01)  /*!< Enable the write protection of the desired bank 1 sectors  */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Type FLASH Option Type
  * @{
  */
#define OPTIONBYTE_WRP         ((uint32_t)0x01)  /*!< WRP option byte configuration  */
#define OPTIONBYTE_RDP         ((uint32_t)0x02)  /*!< RDP option byte configuration  */
#define OPTIONBYTE_USER        ((uint32_t)0x04)  /*!< USER option byte configuration */
#define OPTIONBYTE_BOR         ((uint32_t)0x08)  /*!< BOR option byte configuration  */
#define OPTIONBYTE_BOOTADDR_0  ((uint32_t)0x10)  /*!< Boot 0 Address configuration   */
#define OPTIONBYTE_BOOTADDR_1  ((uint32_t)0x20)  /*!< Boot 1 Address configuration   */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_Read_Protection FLASH Option Bytes Read Protection
  * @{
  */
#define OB_RDP_LEVEL_0       ((uint8_t)0xAA)
#define OB_RDP_LEVEL_1       ((uint8_t)0x55)
#define OB_RDP_LEVEL_2       ((uint8_t)0xCC)   /*!< Warning: When enabling read protection level 2
                                                  it s no more possible to go back to level 1 or 0 */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_WWatchdog FLASH Option Bytes WWatchdog
  * @{
  */
#define OB_WWDG_SW           ((uint32_t)0x10)  /*!< Software WWDG selected */
#define OB_WWDG_HW           ((uint32_t)0x00)  /*!< Hardware WWDG selected */
/**
  * @}
  */


/** @defgroup FLASHEx_Option_Bytes_IWatchdog FLASH Option Bytes IWatchdog
  * @{
  */
#define OB_IWDG_SW           ((uint32_t)0x20)  /*!< Software IWDG selected */
#define OB_IWDG_HW           ((uint32_t)0x00)  /*!< Hardware IWDG selected */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_nRST_STOP FLASH Option Bytes nRST_STOP
  * @{
  */
#define OB_STOP_NO_RST       ((uint32_t)0x40) /*!< No reset generated when entering in STOP */
#define OB_STOP_RST          ((uint32_t)0x00) /*!< Reset generated when entering in STOP    */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_nRST_STDBY FLASH Option Bytes nRST_STDBY
  * @{
  */
#define OB_STDBY_NO_RST      ((uint32_t)0x80) /*!< No reset generated when entering in STANDBY */
#define OB_STDBY_RST         ((uint32_t)0x00) /*!< Reset generated when entering in STANDBY    */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_IWDG_FREEZE_STOP FLASH IWDG Counter Freeze in STOP
  * @{
  */
#define OB_IWDG_STOP_FREEZE      ((uint32_t)0x00000000) /*!< Freeze IWDG counter in STOP mode */
#define OB_IWDG_STOP_ACTIVE      ((uint32_t)0x40000000) /*!< IWDG counter active in STOP mode */
/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_IWDG_FREEZE_SANDBY FLASH IWDG Counter Freeze in STANDBY
  * @{
  */
#define OB_IWDG_STDBY_FREEZE      ((uint32_t)0x00000000) /*!< Freeze IWDG counter in STANDBY mode */
#define OB_IWDG_STDBY_ACTIVE      ((uint32_t)0x40000000) /*!< IWDG counter active in STANDBY mode */
/**
  * @}
  */

/** @defgroup FLASHEx_BOR_Reset_Level FLASH BOR Reset Level
  * @{
  */
#define OB_BOR_LEVEL3          ((uint32_t)0x00)  /*!< Supply voltage ranges from 2.70 to 3.60 V */
#define OB_BOR_LEVEL2          ((uint32_t)0x04)  /*!< Supply voltage ranges from 2.40 to 2.70 V */
#define OB_BOR_LEVEL1          ((uint32_t)0x08)  /*!< Supply voltage ranges from 2.10 to 2.40 V */
#define OB_BOR_OFF             ((uint32_t)0x0C)  /*!< Supply voltage ranges from 1.62 to 2.10 V */

/**
  * @}
  */

/** @defgroup FLASHEx_Boot_Address FLASH Boot Address
  * @{
  */
#define OB_BOOTADDR_ITCM_RAM         ((uint32_t)0x0000)  /*!< Boot from ITCM RAM (0x00000000)                 */
#define OB_BOOTADDR_SYSTEM           ((uint32_t)0x0040)  /*!< Boot from System memory bootloader (0x00100000) */
#define OB_BOOTADDR_ITCM_FLASH       ((uint32_t)0x0080)  /*!< Boot from Flash on ITCM interface (0x00200000)  */
#define OB_BOOTADDR_AXIM_FLASH       ((uint32_t)0x2000)  /*!< Boot from Flash on AXIM interface (0x08000000)  */
#define OB_BOOTADDR_DTCM_RAM         ((uint32_t)0x8000)  /*!< Boot from DTCM RAM (0x20000000)                 */
#define OB_BOOTADDR_SRAM1            ((uint32_t)0x8004)  /*!< Boot from SRAM1 (0x20010000)                    */
#define OB_BOOTADDR_SRAM2            ((uint32_t)0x8013)  /*!< Boot from SRAM2 (0x2004C000)                    */

/**
  * @}
  */

/** @defgroup FLASH_Latency FLASH Latency
  * @{
  */
#define FLASH_LATENCY_0                FLASH_ACR_LATENCY_0WS   /*!< FLASH Zero Latency cycle      */
#define FLASH_LATENCY_1                FLASH_ACR_LATENCY_1WS   /*!< FLASH One Latency cycle       */
#define FLASH_LATENCY_2                FLASH_ACR_LATENCY_2WS   /*!< FLASH Two Latency cycles      */
#define FLASH_LATENCY_3                FLASH_ACR_LATENCY_3WS   /*!< FLASH Three Latency cycles    */
#define FLASH_LATENCY_4                FLASH_ACR_LATENCY_4WS   /*!< FLASH Four Latency cycles     */
#define FLASH_LATENCY_5                FLASH_ACR_LATENCY_5WS   /*!< FLASH Five Latency cycles     */
#define FLASH_LATENCY_6                FLASH_ACR_LATENCY_6WS   /*!< FLASH Six Latency cycles      */
#define FLASH_LATENCY_7                FLASH_ACR_LATENCY_7WS   /*!< FLASH Seven Latency cycles    */
#define FLASH_LATENCY_8                FLASH_ACR_LATENCY_8WS   /*!< FLASH Eight Latency cycles    */
#define FLASH_LATENCY_9                FLASH_ACR_LATENCY_9WS   /*!< FLASH Nine Latency cycles     */
#define FLASH_LATENCY_10               FLASH_ACR_LATENCY_10WS  /*!< FLASH Ten Latency cycles      */
#define FLASH_LATENCY_11               FLASH_ACR_LATENCY_11WS  /*!< FLASH Eleven Latency cycles   */
#define FLASH_LATENCY_12               FLASH_ACR_LATENCY_12WS  /*!< FLASH Twelve Latency cycles   */
#define FLASH_LATENCY_13               FLASH_ACR_LATENCY_13WS  /*!< FLASH Thirteen Latency cycles */
#define FLASH_LATENCY_14               FLASH_ACR_LATENCY_14WS  /*!< FLASH Fourteen Latency cycles */
#define FLASH_LATENCY_15               FLASH_ACR_LATENCY_15WS  /*!< FLASH Fifteen Latency cycles  */
/**
  * @}
  */

/** @defgroup FLASHEx_MassErase_bit FLASH Mass Erase bit
  * @{
  */
#define FLASH_MER_BIT     (FLASH_CR_MER) /*!< MER bit to clear */
/**
  * @}
  */

/** @defgroup FLASHEx_Sectors FLASH Sectors
  * @{
  */
#define FLASH_SECTOR_0     ((uint32_t)0) /*!< Sector Number 0   */
#define FLASH_SECTOR_1     ((uint32_t)1) /*!< Sector Number 1   */
#define FLASH_SECTOR_2     ((uint32_t)2) /*!< Sector Number 2   */
#define FLASH_SECTOR_3     ((uint32_t)3) /*!< Sector Number 3   */
#define FLASH_SECTOR_4     ((uint32_t)4) /*!< Sector Number 4   */
#define FLASH_SECTOR_5     ((uint32_t)5) /*!< Sector Number 5   */
#define FLASH_SECTOR_6     ((uint32_t)6) /*!< Sector Number 6   */
#define FLASH_SECTOR_7     ((uint32_t)7) /*!< Sector Number 7   */

/**
  * @}
  */

/** @defgroup FLASHEx_Option_Bytes_Write_Protection FLASH Option Bytes Write Protection
  * @{
  */
#define OB_WRP_SECTOR_0       ((uint32_t)0x00010000) /*!< Write protection of Sector0     */
#define OB_WRP_SECTOR_1       ((uint32_t)0x00020000) /*!< Write protection of Sector1     */
#define OB_WRP_SECTOR_2       ((uint32_t)0x00040000) /*!< Write protection of Sector2     */
#define OB_WRP_SECTOR_3       ((uint32_t)0x00080000) /*!< Write protection of Sector3     */
#define OB_WRP_SECTOR_4       ((uint32_t)0x00100000) /*!< Write protection of Sector4     */
#define OB_WRP_SECTOR_5       ((uint32_t)0x00200000) /*!< Write protection of Sector5     */
#define OB_WRP_SECTOR_6       ((uint32_t)0x00400000) /*!< Write protection of Sector6     */
#define OB_WRP_SECTOR_7       ((uint32_t)0x00800000) /*!< Write protection of Sector7     */
#define OB_WRP_SECTOR_All     ((uint32_t)0x00FF0000) /*!< Write protection of all Sectors */



/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @addtogroup FLASHEx_Exported_Functions
  * @{
  */

/** @addtogroup FLASHEx_Exported_Functions_Group1
  * @{
  */
/* Extension Program operation functions  *************************************/
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError);
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit);
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit);
void              HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit);

/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup FLASHEx_Private_Constants FLASH Private Constants
  * @{
  */
#define FLASH_SECTOR_TOTAL  8
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup FLASHEx_Private_Macros FLASH Private Macros
  * @{
  */

/** @defgroup FLASHEx_IS_FLASH_Definitions FLASH Private macros to check input parameters
  * @{
  */

#define IS_FLASH_TYPEERASE(VALUE)(((VALUE) == FLASH_TYPEERASE_SECTORS) || \
                                  ((VALUE) == FLASH_TYPEERASE_MASSERASE))

#define IS_VOLTAGERANGE(RANGE)(((RANGE) == FLASH_VOLTAGE_RANGE_1) || \
                               ((RANGE) == FLASH_VOLTAGE_RANGE_2) || \
                               ((RANGE) == FLASH_VOLTAGE_RANGE_3) || \
                               ((RANGE) == FLASH_VOLTAGE_RANGE_4))

#define IS_WRPSTATE(VALUE)(((VALUE) == OB_WRPSTATE_DISABLE) || \
                           ((VALUE) == OB_WRPSTATE_ENABLE))

#define IS_OPTIONBYTE(VALUE)(((VALUE) <= (OPTIONBYTE_WRP | OPTIONBYTE_RDP        | OPTIONBYTE_USER |\
                                          OPTIONBYTE_BOR | OPTIONBYTE_BOOTADDR_0 | OPTIONBYTE_BOOTADDR_1)))

#define IS_OB_BOOT_ADDRESS(ADDRESS) (((ADDRESS) == OB_BOOTADDR_ITCM_RAM)   || \
                                     ((ADDRESS) == OB_BOOTADDR_SYSTEM)     || \
                                     ((ADDRESS) == OB_BOOTADDR_ITCM_FLASH) || \
                                     ((ADDRESS) == OB_BOOTADDR_AXIM_FLASH) || \
                                     ((ADDRESS) == OB_BOOTADDR_DTCM_RAM)   || \
                                     ((ADDRESS) == OB_BOOTADDR_SRAM1)      || \
                                     ((ADDRESS) == OB_BOOTADDR_SRAM2))

#define IS_OB_RDP_LEVEL(LEVEL) (((LEVEL) == OB_RDP_LEVEL_0)   ||\
                                ((LEVEL) == OB_RDP_LEVEL_1)   ||\
                                ((LEVEL) == OB_RDP_LEVEL_2))

#define IS_OB_WWDG_SOURCE(SOURCE) (((SOURCE) == OB_WWDG_SW) || ((SOURCE) == OB_WWDG_HW))

#define IS_OB_IWDG_SOURCE(SOURCE) (((SOURCE) == OB_IWDG_SW) || ((SOURCE) == OB_IWDG_HW))

#define IS_OB_STOP_SOURCE(SOURCE) (((SOURCE) == OB_STOP_NO_RST) || ((SOURCE) == OB_STOP_RST))

#define IS_OB_STDBY_SOURCE(SOURCE) (((SOURCE) == OB_STDBY_NO_RST) || ((SOURCE) == OB_STDBY_RST))

#define IS_OB_IWDG_STOP_FREEZE(FREEZE) (((FREEZE) == OB_IWDG_STOP_FREEZE) || ((FREEZE) == OB_IWDG_STOP_ACTIVE))

#define IS_OB_IWDG_STDBY_FREEZE(FREEZE) (((FREEZE) == OB_IWDG_STDBY_FREEZE) || ((FREEZE) == OB_IWDG_STDBY_ACTIVE))

#define IS_OB_BOR_LEVEL(LEVEL) (((LEVEL) == OB_BOR_LEVEL1) || ((LEVEL) == OB_BOR_LEVEL2) ||\
                                ((LEVEL) == OB_BOR_LEVEL3) || ((LEVEL) == OB_BOR_OFF))

#define IS_FLASH_LATENCY(LATENCY) (((LATENCY) == FLASH_LATENCY_0)  || \
                                   ((LATENCY) == FLASH_LATENCY_1)  || \
                                   ((LATENCY) == FLASH_LATENCY_2)  || \
                                   ((LATENCY) == FLASH_LATENCY_3)  || \
                                   ((LATENCY) == FLASH_LATENCY_4)  || \
                                   ((LATENCY) == FLASH_LATENCY_5)  || \
                                   ((LATENCY) == FLASH_LATENCY_6)  || \
                                   ((LATENCY) == FLASH_LATENCY_7)  || \
                                   ((LATENCY) == FLASH_LATENCY_8)  || \
                                   ((LATENCY) == FLASH_LATENCY_9)  || \
                                   ((LATENCY) == FLASH_LATENCY_10) || \
                                   ((LATENCY) == FLASH_LATENCY_11) || \
                                   ((LATENCY) == FLASH_LATENCY_12) || \
                                   ((LATENCY) == FLASH_LATENCY_13) || \
                                   ((LATENCY) == FLASH_LATENCY_14) || \
                                   ((LATENCY) == FLASH_LATENCY_15))

#define IS_FLASH_SECTOR(SECTOR) (((SECTOR) == FLASH_SECTOR_0)   || ((SECTOR) == FLASH_SECTOR_1)   ||\
                                 ((SECTOR) == FLASH_SECTOR_2)   || ((SECTOR) == FLASH_SECTOR_3)   ||\
                                 ((SECTOR) == FLASH_SECTOR_4)   || ((SECTOR) == FLASH_SECTOR_5)   ||\
                                 ((SECTOR) == FLASH_SECTOR_6)   || ((SECTOR) == FLASH_SECTOR_7))


#define IS_FLASH_ADDRESS(ADDRESS) (((ADDRESS) >= FLASH_BASE) && ((ADDRESS) <= FLASH_END))

#define IS_FLASH_NBSECTORS(NBSECTORS) (((NBSECTORS) != 0) && ((NBSECTORS) <= FLASH_SECTOR_TOTAL))

#define IS_OB_WRP_SECTOR(SECTOR)  ((((SECTOR) & (uint32_t)0xFF00FFFF) == 0x00000000) && ((SECTOR) != 0x00000000))
/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup FLASHEx_Private_Functions FLASH Private Functions
  * @{
  */
void FLASH_Erase_Sector(uint32_t Sector, uint8_t VoltageRange);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7xx_HAL_FLASH_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
