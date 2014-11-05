/**
  ******************************************************************************
  * @file    stm32f4xx_hal_nand.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of NAND HAL module.
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
#ifndef __STM32F4xx_HAL_NAND_H
#define __STM32F4xx_HAL_NAND_H

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

/** @addtogroup NAND
  * @{
  */ 

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Exported typedef ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  HAL NAND State structures definition
  */
typedef enum
{
  HAL_NAND_STATE_RESET     = 0x00,  /*!< NAND not yet initialized or disabled */
  HAL_NAND_STATE_READY     = 0x01,  /*!< NAND initialized and ready for use   */
  HAL_NAND_STATE_BUSY      = 0x02,  /*!< NAND internal process is ongoing     */
  HAL_NAND_STATE_ERROR     = 0x03   /*!< NAND error state                     */
}HAL_NAND_StateTypeDef;
   
/** 
  * @brief  NAND Memory electronic signature Structure definition
  */
typedef struct
{
  /*<! NAND memory electronic signature maker and device IDs */

  uint8_t Maker_Id; 

  uint8_t Device_Id;

  uint8_t Third_Id;

  uint8_t Fourth_Id;
}NAND_IDTypeDef;

/** 
  * @brief  NAND Memory address Structure definition
  */
typedef struct 
{
  uint16_t Page;   /*!< NAND memory Page address  */

  uint16_t Zone;   /*!< NAND memory Zone address  */

  uint16_t Block;  /*!< NAND memory Block address */

}NAND_AddressTypedef;

/** 
  * @brief  NAND Memory info Structure definition
  */ 
typedef struct
{
  uint32_t PageSize;       /*!< NAND memory page (without spare area) size measured in K. bytes */

  uint32_t SpareAreaSize;  /*!< NAND memory spare area size measured in K. bytes                */

  uint32_t BlockSize;      /*!< NAND memory block size number of pages                          */

  uint32_t BlockNbr;       /*!< NAND memory number of blocks                                    */

  uint32_t ZoneSize;       /*!< NAND memory zone size measured in number of blocks              */
}NAND_InfoTypeDef;

/** 
  * @brief  NAND handle Structure definition
  */   
typedef struct
{
  FMC_NAND_TypeDef             *Instance;  /*!< Register base address                        */
  
  FMC_NAND_InitTypeDef         Init;       /*!< NAND device control configuration parameters */

  HAL_LockTypeDef              Lock;       /*!< NAND locking object                          */

  __IO HAL_NAND_StateTypeDef   State;      /*!< NAND device access state                     */

  NAND_InfoTypeDef             Info;       /*!< NAND characteristic information structure    */
}NAND_HandleTypeDef;


/* Exported constants --------------------------------------------------------*/
/** @defgroup NAND_Exported_Constants
 * @{
 */ 
#define NAND_DEVICE1               ((uint32_t)0x70000000) 
#define NAND_DEVICE2               ((uint32_t)0x80000000) 
#define NAND_WRITE_TIMEOUT         ((uint32_t)0x01000000)

#define CMD_AREA                   ((uint32_t)(1<<16))  /* A16 = CLE high */
#define ADDR_AREA                  ((uint32_t)(1<<17))  /* A17 = ALE high */

#define	NAND_CMD_AREA_A            ((uint8_t)0x00)
#define	NAND_CMD_AREA_B            ((uint8_t)0x01)
#define NAND_CMD_AREA_C            ((uint8_t)0x50)
#define NAND_CMD_AREA_TRUE1        ((uint8_t)0x30)

#define NAND_CMD_WRITE0            ((uint8_t)0x80)
#define NAND_CMD_WRITE_TRUE1       ((uint8_t)0x10)	
#define NAND_CMD_ERASE0            ((uint8_t)0x60)
#define NAND_CMD_ERASE1            ((uint8_t)0xD0)  
#define NAND_CMD_READID            ((uint8_t)0x90)	
#define NAND_CMD_STATUS            ((uint8_t)0x70)
#define NAND_CMD_LOCK_STATUS       ((uint8_t)0x7A)
#define NAND_CMD_RESET             ((uint8_t)0xFF)

/* NAND memory status */
#define NAND_VALID_ADDRESS         ((uint32_t)0x00000100)
#define NAND_INVALID_ADDRESS       ((uint32_t)0x00000200)
#define NAND_TIMEOUT_ERROR         ((uint32_t)0x00000400)
#define NAND_BUSY                  ((uint32_t)0x00000000)
#define NAND_ERROR                 ((uint32_t)0x00000001)
#define NAND_READY                 ((uint32_t)0x00000040)

/**
  * @}
  */
  
/* Exported macro ------------------------------------------------------------*/

/** @brief Reset NAND handle state
  * @param  __HANDLE__: specifies the NAND handle.
  * @retval None
  */
#define __HAL_NAND_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_NAND_STATE_RESET)

/**
  * @brief  NAND memory address computation.
  * @param  __ADDRESS__: NAND memory address.
  * @param  __HANDLE__ : NAND handle.
  * @retval NAND Raw address value
  */
#define __ARRAY_ADDRESS(__ADDRESS__ , __HANDLE__) ((__ADDRESS__)->Page + \
                         (((__ADDRESS__)->Block + (((__ADDRESS__)->Zone) * ((__HANDLE__)->Info.ZoneSize)))* ((__HANDLE__)->Info.BlockSize)))
        
/**
  * @brief  NAND memory address cycling.
  * @param  __ADDRESS__: NAND memory address.
  * @retval NAND address cycling value.
  */
#define __ADDR_1st_CYCLE(__ADDRESS__)       (uint8_t)(__ADDRESS__)              /* 1st addressing cycle */
#define __ADDR_2nd_CYCLE(__ADDRESS__)       (uint8_t)((__ADDRESS__) >> 8)       /* 2nd addressing cycle */
#define __ADDR_3rd_CYCLE(__ADDRESS__)       (uint8_t)((__ADDRESS__) >> 16)      /* 3rd addressing cycle */
#define __ADDR_4th_CYCLE(__ADDRESS__)       (uint8_t)((__ADDRESS__) >> 24)      /* 4th addressing cycle */

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef  HAL_NAND_Init(NAND_HandleTypeDef *hnand, FMC_NAND_PCC_TimingTypeDef *ComSpace_Timing, FMC_NAND_PCC_TimingTypeDef *AttSpace_Timing);
HAL_StatusTypeDef  HAL_NAND_DeInit(NAND_HandleTypeDef *hnand);
void        HAL_NAND_MspInit(NAND_HandleTypeDef *hnand);
void        HAL_NAND_MspDeInit(NAND_HandleTypeDef *hnand);
void               HAL_NAND_IRQHandler(NAND_HandleTypeDef *hnand);
void        HAL_NAND_ITCallback(NAND_HandleTypeDef *hnand);

/* IO operation functions  ****************************************************/
HAL_StatusTypeDef  HAL_NAND_Read_ID(NAND_HandleTypeDef *hnand, NAND_IDTypeDef *pNAND_ID);
HAL_StatusTypeDef  HAL_NAND_Reset(NAND_HandleTypeDef *hnand);
HAL_StatusTypeDef  HAL_NAND_Read_Page(NAND_HandleTypeDef *hnand, NAND_AddressTypedef *pAddress, uint8_t *pBuffer, uint32_t NumPageToRead);
HAL_StatusTypeDef  HAL_NAND_Write_Page(NAND_HandleTypeDef *hnand, NAND_AddressTypedef *pAddress, uint8_t *pBuffer, uint32_t NumPageToWrite);
HAL_StatusTypeDef  HAL_NAND_Read_SpareArea(NAND_HandleTypeDef *hnand, NAND_AddressTypedef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaToRead);
HAL_StatusTypeDef  HAL_NAND_Write_SpareArea(NAND_HandleTypeDef *hnand, NAND_AddressTypedef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaTowrite);
HAL_StatusTypeDef  HAL_NAND_Erase_Block(NAND_HandleTypeDef *hnand, NAND_AddressTypedef *pAddress);
uint32_t           HAL_NAND_Read_Status(NAND_HandleTypeDef *hnand);
uint32_t           HAL_NAND_Address_Inc(NAND_HandleTypeDef *hnand, NAND_AddressTypedef *pAddress);

/* NAND Control functions  ****************************************************/
HAL_StatusTypeDef  HAL_NAND_ECC_Enable(NAND_HandleTypeDef *hnand);
HAL_StatusTypeDef  HAL_NAND_ECC_Disable(NAND_HandleTypeDef *hnand);
HAL_StatusTypeDef  HAL_NAND_GetECC(NAND_HandleTypeDef *hnand, uint32_t *ECCval, uint32_t Timeout);

/* NAND State functions *******************************************************/
HAL_NAND_StateTypeDef HAL_NAND_GetState(NAND_HandleTypeDef *hnand);
uint32_t HAL_NAND_Read_Status(NAND_HandleTypeDef *hnand);

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

#endif /* __STM32F4xx_HAL_NAND_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
