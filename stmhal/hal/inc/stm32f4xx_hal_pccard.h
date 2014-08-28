/**
  ******************************************************************************
  * @file    stm32f4xx_hal_pccard.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of PCCARD HAL module.
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
#ifndef __STM32F4xx_HAL_PCCARD_H
#define __STM32F4xx_HAL_PCCARD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx)|| defined(STM32F417xx)
  #include "stm32f4xx_ll_fsmc.h"
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx */

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx)
  #include "stm32f4xx_ll_fmc.h"
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup PCCARD
  * @{
  */ 

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Exported typedef ----------------------------------------------------------*/

/** 
  * @brief  HAL SRAM State structures definition  
  */ 
typedef enum
{
  HAL_PCCARD_STATE_RESET     = 0x00,    /*!< PCCARD peripheral not yet initialized or disabled */
  HAL_PCCARD_STATE_READY     = 0x01,    /*!< PCCARD peripheral ready                           */
  HAL_PCCARD_STATE_BUSY      = 0x02,    /*!< PCCARD peripheral busy                            */   
  HAL_PCCARD_STATE_ERROR     = 0x04     /*!< PCCARD peripheral error                           */
}HAL_PCCARD_StateTypeDef;
 
typedef enum
{
  CF_SUCCESS = 0,
  CF_ONGOING,
  CF_ERROR,
  CF_TIMEOUT
}CF_StatusTypedef;

/** 
  * @brief  FMC_PCCARD handle Structure definition  
  */   
typedef struct
{
  FMC_PCCARD_TypeDef           *Instance;              /*!< Register base address for PCCARD device          */
  
  FMC_PCCARD_InitTypeDef       Init;                   /*!< PCCARD device control configuration parameters   */

  __IO HAL_PCCARD_StateTypeDef State;                  /*!< PCCARD device access state                       */
   
  HAL_LockTypeDef              Lock;                   /*!< PCCARD Lock                                      */ 
 
}PCCARD_HandleTypeDef;


/* Exported constants --------------------------------------------------------*/
/** @defgroup PCCARD_Exported_Constants
  * @{
  */
  
#define CF_DEVICE_ADDRESS             ((uint32_t)0x90000000)
#define CF_ATTRIBUTE_SPACE_ADDRESS    ((uint32_t)0x98000000)   /* Attribute space size to @0x9BFF FFFF */
#define CF_COMMON_SPACE_ADDRESS       CF_DEVICE_ADDRESS        /* Common space size to @0x93FF FFFF    */
#define CF_IO_SPACE_ADDRESS           ((uint32_t)0x9C000000)   /* IO space size to @0x9FFF FFFF        */
#define CF_IO_SPACE_PRIMARY_ADDR      ((uint32_t)0x9C0001F0)   /* IO space size to @0x9FFF FFFF        */

/* Compact Flash-ATA registers description */
#define CF_DATA                       ((uint8_t)0x00)    /* Data register */
#define CF_SECTOR_COUNT               ((uint8_t)0x02)    /* Sector Count register */
#define CF_SECTOR_NUMBER              ((uint8_t)0x03)    /* Sector Number register */
#define CF_CYLINDER_LOW               ((uint8_t)0x04)    /* Cylinder low register */
#define CF_CYLINDER_HIGH              ((uint8_t)0x05)    /* Cylinder high register */
#define CF_CARD_HEAD                  ((uint8_t)0x06)    /* Card/Head register */
#define CF_STATUS_CMD                 ((uint8_t)0x07)    /* Status(read)/Command(write) register */
#define CF_STATUS_CMD_ALTERNATE       ((uint8_t)0x0E)    /* Alternate Status(read)/Command(write) register */
#define CF_COMMON_DATA_AREA           ((uint16_t)0x0400) /* Start of data area (for Common access only!) */

/* Compact Flash-ATA commands */
#define CF_READ_SECTOR_CMD            ((uint8_t)0x20)
#define CF_WRITE_SECTOR_CMD           ((uint8_t)0x30)
#define CF_ERASE_SECTOR_CMD           ((uint8_t)0xC0)
#define CF_IDENTIFY_CMD               ((uint8_t)0xEC)

/* Compact Flash status */
#define CF_TIMEOUT_ERROR              ((uint8_t)0x60)
#define CF_BUSY                       ((uint8_t)0x80)
#define CF_PROGR                      ((uint8_t)0x01)
#define CF_READY                      ((uint8_t)0x40)

#define CF_SECTOR_SIZE                ((uint32_t)255)    /* In half words */ 
 
/**
  * @}
  */ 
/* Exported macro ------------------------------------------------------------*/

/** @brief Reset PCCARD handle state
  * @param  __HANDLE__: specifies the PCCARD handle.
  * @retval None
  */
#define __HAL_PCCARD_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_PCCARD_STATE_RESET)

/* Exported functions --------------------------------------------------------*/
/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef  HAL_PCCARD_Init(PCCARD_HandleTypeDef *hpccard, FMC_NAND_PCC_TimingTypeDef *ComSpaceTiming, FMC_NAND_PCC_TimingTypeDef *AttSpaceTiming, FMC_NAND_PCC_TimingTypeDef *IOSpaceTiming);
HAL_StatusTypeDef  HAL_PCCARD_DeInit(PCCARD_HandleTypeDef *hpccard);   
void HAL_PCCARD_MspInit(PCCARD_HandleTypeDef *hpccard);
void HAL_PCCARD_MspDeInit(PCCARD_HandleTypeDef *hpccard);

/* IO operation functions  *****************************************************/
HAL_StatusTypeDef  HAL_CF_Read_ID(PCCARD_HandleTypeDef *hpccard, uint8_t CompactFlash_ID[], uint8_t *pStatus);
HAL_StatusTypeDef  HAL_CF_Write_Sector(PCCARD_HandleTypeDef *hpccard, uint16_t *pBuffer, uint16_t SectorAddress,  uint8_t *pStatus);
HAL_StatusTypeDef  HAL_CF_Read_Sector(PCCARD_HandleTypeDef *hpccard, uint16_t *pBuffer, uint16_t SectorAddress, uint8_t *pStatus);
HAL_StatusTypeDef  HAL_CF_Erase_Sector(PCCARD_HandleTypeDef *hpccard, uint16_t SectorAddress, uint8_t *pStatus);
HAL_StatusTypeDef  HAL_CF_Reset(PCCARD_HandleTypeDef *hpccard);
void               HAL_PCCARD_IRQHandler(PCCARD_HandleTypeDef *hpccard);
void        HAL_PCCARD_ITCallback(PCCARD_HandleTypeDef *hpccard);

/* PCCARD State functions *******************************************************/
HAL_PCCARD_StateTypeDef HAL_PCCARD_GetState(PCCARD_HandleTypeDef *hpccard);
CF_StatusTypedef HAL_CF_GetStatus(PCCARD_HandleTypeDef *hpccard);
CF_StatusTypedef HAL_CF_ReadStatus(PCCARD_HandleTypeDef *hpccard);

#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_PCCARD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
