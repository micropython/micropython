/**
  ******************************************************************************
  * @file    stm32f2xx_hal_flash_ex.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-March-2014
  * @brief   Header file of FLASH HAL Extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#ifndef __STM32F2xx_HAL_FLASH_EX_H
#define __STM32F2xx_HAL_FLASH_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal_def.h"

/** @addtogroup STM32F2xx_HAL_Driver
  * @{
  */

/** @addtogroup FLASHEx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/**
  * @brief  FLASH Erase structure definition
  */
typedef struct
{
  uint32_t TypeErase;   /*!< TypeErase: Mass erase or sector Erase.
                             This parameter can be a value of @ref FLASHEx_Type_Erase */

  uint32_t Sector;      /*!< Sector: Initial FLASH sector to erase when Mass erase is disabled
                             This parameter must be a value of @ref FLASHEx_Sectors */        
  
  uint32_t NbSectors;   /*!< NbSectors: Number of sectors to be erased.
                             This parameter must be a value between 1 and (max number of sectors - value of Initial sector)*/           
                                                          
  uint32_t VoltageRange;/*!< VoltageRange: The device voltage range which defines the erase parallelism
                             This parameter must be a value of @ref FLASHEx_Voltage_Range */        
  
} FLASH_EraseInitTypeDef;

/**
  * @brief  FLASH Option Bytes Program structure definition
  */
typedef struct
{
  uint32_t OptionType;   /*!< OptionType: Option byte to be configured.
                              This parameter can be a value of @ref FLASHEx_Option_Type */

  uint32_t WRPState;     /*!< WRPState: Write protection activation or deactivation.
                              This parameter can be a value of @ref FLASHEx_WRP_State */

  uint32_t WRPSector;         /*!< WRPSector: specifies the sector(s) to be write protected
                              The value of this parameter depend on device used within the same series */

  uint32_t RDPLevel;     /*!< RDPLevel: Set the read protection level..
                              This parameter can be a value of @ref FLASHEx_Option_Bytes_Read_Protection */

  uint32_t BORLevel;     /*!< BORLevel: Set the BOR Level.
                              This parameter can be a value of @ref FLASHEx_BOR_Reset_Level */

  uint8_t  USERConfig;   /*!< USERConfig: Program the FLASH User Option Byte: IWDG_SW / RST_STOP / RST_STDBY.
                              This parameter can be a combination of @ref FLASHEx_Option_Bytes_IWatchdog, @ref FLASHEx_Option_Bytes_nRST_STOP and @ref FLASHEx_Option_Bytes_nRST_STDBY*/

} FLASH_OBProgramInitTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup FLASH_Exported_Constants FLASH Exported Constants
  * @{
  */

/** @defgroup FLASHEx_Type_Erase FLASH Type Erase
  * @{
  */ 
#define TYPEERASE_SECTORS         ((uint32_t)0x00)  /*!< Sectors erase only          */
#define TYPEERASE_MASSERASE       ((uint32_t)0x01)  /*!< Flash Mass erase activation */

#define IS_TYPEERASE(VALUE)(((VALUE) == TYPEERASE_SECTORS) || \
                               ((VALUE) == TYPEERASE_MASSERASE))  

/**
  * @}
  */
  
/** @defgroup FLASHEx_Voltage_Range FLASH Voltage Range
  * @{
  */ 
#define VOLTAGE_RANGE_1        ((uint32_t)0x00)  /*!< Device operating range: 1.8V to 2.1V                */
#define VOLTAGE_RANGE_2        ((uint32_t)0x01)  /*!< Device operating range: 2.1V to 2.7V                */
#define VOLTAGE_RANGE_3        ((uint32_t)0x02)  /*!< Device operating range: 2.7V to 3.6V                */
#define VOLTAGE_RANGE_4        ((uint32_t)0x03)  /*!< Device operating range: 2.7V to 3.6V + External Vpp */

#define IS_VOLTAGERANGE(RANGE)(((RANGE) == VOLTAGE_RANGE_1) || \
                               ((RANGE) == VOLTAGE_RANGE_2) || \
                               ((RANGE) == VOLTAGE_RANGE_3) || \
                               ((RANGE) == VOLTAGE_RANGE_4))  
                              
/**
  * @}
  */
  
/** @defgroup FLASHEx_WRP_State FLASH WRP State
  * @{
  */ 
#define WRPSTATE_DISABLE       ((uint32_t)0x00)  /*!< Disable the write protection of the desired bank 1 sectors */
#define WRPSTATE_ENABLE        ((uint32_t)0x01)  /*!< Enable the write protection of the desired bank 1 sectors  */

#define IS_WRPSTATE(VALUE)(((VALUE) == WRPSTATE_DISABLE) || \
                           ((VALUE) == WRPSTATE_ENABLE))  

/**
  * @}
  */
  
/** @defgroup FLASHEx_Option_Type FLASH Option Type
  * @{
  */ 
#define OPTIONBYTE_WRP        ((uint32_t)0x01)  /*!< WRP option byte configuration  */
#define OPTIONBYTE_RDP        ((uint32_t)0x02)  /*!< RDP option byte configuration  */
#define OPTIONBYTE_USER       ((uint32_t)0x04)  /*!< USER option byte configuration */
#define OPTIONBYTE_BOR        ((uint32_t)0x08)  /*!< BOR option byte configuration  */

#define IS_OPTIONBYTE(VALUE)(((VALUE) < (OPTIONBYTE_WRP|OPTIONBYTE_RDP|OPTIONBYTE_USER|OPTIONBYTE_BOR)))

/**
  * @}
  */
  
/** @defgroup FLASHEx_Option_Bytes_Read_Protection FLASH Option Bytes Read Protection
  * @{
  */
#define OB_RDP_LEVEL_0   ((uint8_t)0xAA)
#define OB_RDP_LEVEL_1   ((uint8_t)0x55)
/*#define OB_RDP_LEVEL_2   ((uint8_t)0xCC)*/ /*!< Warning: When enabling read protection level 2 
                                                  it s no more possible to go back to level 1 or 0 */
#define IS_OB_RDP_LEVEL(LEVEL) (((LEVEL) == OB_RDP_LEVEL_0)   ||\
                                ((LEVEL) == OB_RDP_LEVEL_1))/*||\
                                ((LEVEL) == OB_RDP_LEVEL_2))*/
/**
  * @}
  */ 
  
/** @defgroup FLASHEx_Option_Bytes_IWatchdog FLASH Option Bytes IWatchdog
  * @{
  */ 
#define OB_IWDG_SW                     ((uint8_t)0x20)  /*!< Software IWDG selected */
#define OB_IWDG_HW                     ((uint8_t)0x00)  /*!< Hardware IWDG selected */
#define IS_OB_IWDG_SOURCE(SOURCE) (((SOURCE) == OB_IWDG_SW) || ((SOURCE) == OB_IWDG_HW))
/**
  * @}
  */ 
  
/** @defgroup FLASHEx_Option_Bytes_nRST_STOP FLASH Option Bytes nRST_STOP
  * @{
  */ 
#define OB_STOP_NO_RST                 ((uint8_t)0x40) /*!< No reset generated when entering in STOP */
#define OB_STOP_RST                    ((uint8_t)0x00) /*!< Reset generated when entering in STOP    */
#define IS_OB_STOP_SOURCE(SOURCE) (((SOURCE) == OB_STOP_NO_RST) || ((SOURCE) == OB_STOP_RST))
/**
  * @}
  */ 


/** @defgroup FLASHEx_Option_Bytes_nRST_STDBY FLASH Option Bytes nRST_STDBY
  * @{
  */ 
#define OB_STDBY_NO_RST                ((uint8_t)0x80) /*!< No reset generated when entering in STANDBY */
#define OB_STDBY_RST                   ((uint8_t)0x00) /*!< Reset generated when entering in STANDBY    */
#define IS_OB_STDBY_SOURCE(SOURCE) (((SOURCE) == OB_STDBY_NO_RST) || ((SOURCE) == OB_STDBY_RST))
/**
  * @}
  */    

/** @defgroup FLASHEx_BOR_Reset_Level FLASH BOR Reset Level
  * @{
  */  
#define OB_BOR_LEVEL3          ((uint8_t)0x00)  /*!< Supply voltage ranges from 2.70 to 3.60 V */
#define OB_BOR_LEVEL2          ((uint8_t)0x04)  /*!< Supply voltage ranges from 2.40 to 2.70 V */
#define OB_BOR_LEVEL1          ((uint8_t)0x08)  /*!< Supply voltage ranges from 2.10 to 2.40 V */
#define OB_BOR_OFF             ((uint8_t)0x0C)  /*!< Supply voltage ranges from 1.62 to 2.10 V */
#define IS_OB_BOR_LEVEL(LEVEL) (((LEVEL) == OB_BOR_LEVEL1) || ((LEVEL) == OB_BOR_LEVEL2) ||\
                                ((LEVEL) == OB_BOR_LEVEL3) || ((LEVEL) == OB_BOR_OFF))
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

#define IS_FLASH_LATENCY(LATENCY) (((LATENCY) == FLASH_LATENCY_0)  || \
                                   ((LATENCY) == FLASH_LATENCY_1)  || \
                                   ((LATENCY) == FLASH_LATENCY_2)  || \
                                   ((LATENCY) == FLASH_LATENCY_3)  || \
                                   ((LATENCY) == FLASH_LATENCY_4)  || \
                                   ((LATENCY) == FLASH_LATENCY_5)  || \
                                   ((LATENCY) == FLASH_LATENCY_6)  || \
                                   ((LATENCY) == FLASH_LATENCY_7))
/**
  * @}
  */ 
  
/** @defgroup FLASHEx_Sectors FLASH Sectors
  * @{
  */
#define FLASH_SECTOR_0     ((uint32_t)0)  /*!< Sector Number 0   */
#define FLASH_SECTOR_1     ((uint32_t)1)  /*!< Sector Number 1   */
#define FLASH_SECTOR_2     ((uint32_t)2)  /*!< Sector Number 2   */
#define FLASH_SECTOR_3     ((uint32_t)3)  /*!< Sector Number 3   */
#define FLASH_SECTOR_4     ((uint32_t)4)  /*!< Sector Number 4   */
#define FLASH_SECTOR_5     ((uint32_t)5)  /*!< Sector Number 5   */
#define FLASH_SECTOR_6     ((uint32_t)6)  /*!< Sector Number 6   */
#define FLASH_SECTOR_7     ((uint32_t)7)  /*!< Sector Number 7   */
#define FLASH_SECTOR_8     ((uint32_t)8)  /*!< Sector Number 8   */
#define FLASH_SECTOR_9     ((uint32_t)9)  /*!< Sector Number 9   */
#define FLASH_SECTOR_10    ((uint32_t)10) /*!< Sector Number 10  */
#define FLASH_SECTOR_11    ((uint32_t)11) /*!< Sector Number 11  */

#define FLASH_SECTOR_TOTAL 12 

#define IS_FLASH_SECTOR(SECTOR) (((SECTOR) == FLASH_SECTOR_0)   || ((SECTOR) == FLASH_SECTOR_1) ||\
                                 ((SECTOR) == FLASH_SECTOR_2)   || ((SECTOR) == FLASH_SECTOR_3) ||\
                                 ((SECTOR) == FLASH_SECTOR_4)   || ((SECTOR) == FLASH_SECTOR_5) ||\
                                 ((SECTOR) == FLASH_SECTOR_6)   || ((SECTOR) == FLASH_SECTOR_7) ||\
                                 ((SECTOR) == FLASH_SECTOR_8)   || ((SECTOR) == FLASH_SECTOR_9) ||\
                                 ((SECTOR) == FLASH_SECTOR_10)  || ((SECTOR) == FLASH_SECTOR_11))

#define IS_FLASH_ADDRESS(ADDRESS) ((((ADDRESS) >= 0x08000000) && ((ADDRESS) < 0x080FFFFF)) ||\
                                   (((ADDRESS) >= 0x1FFF7800) && ((ADDRESS) < 0x1FFF7A0F)))

#define IS_NBSECTORS(NBSECTORS) (((NBSECTORS) != 0) && ((NBSECTORS) <= FLASH_SECTOR_TOTAL))

/**
  * @}
  */ 

/** @defgroup FLASHEx_Option_Bytes_Write_Protection FLASH Option Bytes Write Protection
  * @{
  */
#define OB_WRP_SECTOR_0       ((uint32_t)0x00000001) /*!< Write protection of Sector0 */
#define OB_WRP_SECTOR_1       ((uint32_t)0x00000002) /*!< Write protection of Sector1 */
#define OB_WRP_SECTOR_2       ((uint32_t)0x00000004) /*!< Write protection of Sector2 */
#define OB_WRP_SECTOR_3       ((uint32_t)0x00000008) /*!< Write protection of Sector3 */
#define OB_WRP_SECTOR_4       ((uint32_t)0x00000010) /*!< Write protection of Sector4 */
#define OB_WRP_SECTOR_5       ((uint32_t)0x00000020) /*!< Write protection of Sector5 */
#define OB_WRP_SECTOR_6       ((uint32_t)0x00000040) /*!< Write protection of Sector6 */
#define OB_WRP_SECTOR_7       ((uint32_t)0x00000080) /*!< Write protection of Sector7 */
#define OB_WRP_SECTOR_8       ((uint32_t)0x00000100) /*!< Write protection of Sector8 */
#define OB_WRP_SECTOR_9       ((uint32_t)0x00000200) /*!< Write protection of Sector9 */
#define OB_WRP_SECTOR_10      ((uint32_t)0x00000400) /*!< Write protection of Sector10 */
#define OB_WRP_SECTOR_11      ((uint32_t)0x00000800) /*!< Write protection of Sector11 */
#define OB_WRP_SECTOR_All     ((uint32_t)0x00000FFF) /*!< Write protection of all Sectors */

#define IS_OB_WRP_SECTOR(SECTOR)((((SECTOR) & (uint32_t)0xFFFFF000) == 0x00000000) && ((SECTOR) != 0x00000000))
/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* Extension Program operation functions  *************************************/
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError);
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit);
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit);
void              HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit);

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

#endif /* __STM32F2xx_HAL_FLASH_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
