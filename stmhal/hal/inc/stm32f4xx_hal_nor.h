/**
  ******************************************************************************
  * @file    stm32f4xx_hal_nor.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of NOR HAL module.
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
#ifndef __STM32F4xx_HAL_NOR_H
#define __STM32F4xx_HAL_NOR_H

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

/** @addtogroup NOR
  * @{
  */ 

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Exported typedef ----------------------------------------------------------*/
/** 
  * @brief  HAL SRAM State structures definition  
  */ 
typedef enum
{  
  HAL_NOR_STATE_RESET             = 0x00,  /*!< NOR not yet initialized or disabled  */
  HAL_NOR_STATE_READY             = 0x01,  /*!< NOR initialized and ready for use    */
  HAL_NOR_STATE_BUSY              = 0x02,  /*!< NOR internal processing is ongoing   */
  HAL_NOR_STATE_ERROR             = 0x03,  /*!< NOR error state                      */
  HAL_NOR_STATE_PROTECTED         = 0x04   /*!< NOR NORSRAM device write protected   */
}HAL_NOR_StateTypeDef;

/**
  * @brief  FMC NOR Status typedef
  */
typedef enum
{
  NOR_SUCCESS = 0,
  NOR_ONGOING,
  NOR_ERROR,
  NOR_TIMEOUT
}NOR_StatusTypedef;

/**
  * @brief  FMC NOR ID typedef
  */
typedef struct
{
  uint16_t Manufacturer_Code;  /*!< Defines the device's manufacturer code used to identify the memory       */

  uint16_t Device_Code1;

  uint16_t Device_Code2;

  uint16_t Device_Code3;       /*!< Defines the devices' codes used to identify the memory. 
                                    These codes can be accessed by performing read operations with specific 
                                    control signals and addresses set.They can also be accessed by issuing 
                                    an Auto Select command                                                   */
}NOR_IDTypeDef;

/**
  * @brief  FMC NOR CFI typedef
  */
typedef struct
{
  /*!< Defines the information stored in the memory's Common flash interface
       which contains a description of various electrical and timing parameters, 
       density information and functions supported by the memory                   */

  uint16_t CFI_1;

  uint16_t CFI_2;

  uint16_t CFI_3;

  uint16_t CFI_4;
}NOR_CFITypeDef;

/** 
  * @brief  NOR handle Structure definition
  */ 
typedef struct
{
  FMC_NORSRAM_TypeDef           *Instance;    /*!< Register base address                        */

  FMC_NORSRAM_EXTENDED_TypeDef  *Extended;    /*!< Extended mode register base address          */

  FMC_NORSRAM_InitTypeDef       Init;         /*!< NOR device control configuration parameters  */

  HAL_LockTypeDef               Lock;         /*!< NOR locking object                           */

  __IO HAL_NOR_StateTypeDef     State;        /*!< NOR device access state                      */

}NOR_HandleTypeDef; 

/* Exported constants --------------------------------------------------------*/
/** @defgroup NOR_Exported_Constants
  * @{
  */
/* NOR device IDs addresses */
#define MC_ADDRESS               ((uint16_t)0x0000)
#define DEVICE_CODE1_ADDR        ((uint16_t)0x0001)
#define DEVICE_CODE2_ADDR        ((uint16_t)0x000E)
#define DEVICE_CODE3_ADDR        ((uint16_t)0x000F)

/* NOR CFI IDs addresses */
#define CFI1_ADDRESS             ((uint16_t)0x61)
#define CFI2_ADDRESS             ((uint16_t)0x62)
#define CFI3_ADDRESS             ((uint16_t)0x63)
#define CFI4_ADDRESS             ((uint16_t)0x64)

/* NOR operation wait timeout */
#define NOR_TMEOUT               ((uint16_t)0xFFFF)
   
/* NOR memory data width */
#define NOR_MEMORY_8B            ((uint8_t)0x0)
#define NOR_MEMORY_16B           ((uint8_t)0x1)

/* NOR memory device read/write start address */
#define NOR_MEMORY_ADRESS1       ((uint32_t)0x60000000)
#define NOR_MEMORY_ADRESS2       ((uint32_t)0x64000000)
#define NOR_MEMORY_ADRESS3       ((uint32_t)0x68000000)
#define NOR_MEMORY_ADRESS4       ((uint32_t)0x6C000000)

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @brief Reset NOR handle state
  * @param  __HANDLE__: specifies the NOR handle.
  * @retval None
  */
#define __HAL_NOR_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_NOR_STATE_RESET)

/**
  * @brief  NOR memory address shifting.
  * @param  __ADDRESS__: NOR memory address 
  * @retval NOR shifted address value
  */
#define __NOR_ADDR_SHIFT(__NOR_ADDRESS, __NOR_MEMORY_WIDTH_, __ADDRESS__) (((__NOR_MEMORY_WIDTH_) == NOR_MEMORY_8B)? ((uint32_t)((__NOR_ADDRESS) + (2 * (__ADDRESS__)))):\
                         ((uint32_t)((__NOR_ADDRESS) + (__ADDRESS__))))
 
/**
  * @brief  NOR memory write data to specified address.
  * @param  __ADDRESS__: NOR memory address 
  * @param  __DATA__: Data to write
  * @retval None
  */
#define __NOR_WRITE(__ADDRESS__, __DATA__)  (*(__IO uint16_t *)((uint32_t)(__ADDRESS__)) = (__DATA__))

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef HAL_NOR_Init(NOR_HandleTypeDef *hnor, FMC_NORSRAM_TimingTypeDef *Timing, FMC_NORSRAM_TimingTypeDef *ExtTiming);
HAL_StatusTypeDef HAL_NOR_DeInit(NOR_HandleTypeDef *hnor);
void HAL_NOR_MspInit(NOR_HandleTypeDef *hnor);
void HAL_NOR_MspDeInit(NOR_HandleTypeDef *hnor);
void HAL_NOR_MspWait(NOR_HandleTypeDef *hnor, uint32_t Timeout);

/* I/O operation functions  ***************************************************/
HAL_StatusTypeDef HAL_NOR_Read_ID(NOR_HandleTypeDef *hnor, NOR_IDTypeDef *pNOR_ID);
HAL_StatusTypeDef HAL_NOR_ReturnToReadMode(NOR_HandleTypeDef *hnor);
HAL_StatusTypeDef HAL_NOR_Read(NOR_HandleTypeDef *hnor, uint32_t *pAddress, uint16_t *pData);
HAL_StatusTypeDef HAL_NOR_Program(NOR_HandleTypeDef *hnor, uint32_t *pAddress, uint16_t *pData);

HAL_StatusTypeDef HAL_NOR_ReadBuffer(NOR_HandleTypeDef *hnor, uint32_t uwAddress, uint16_t *pData, uint32_t uwBufferSize);
HAL_StatusTypeDef HAL_NOR_ProgramBuffer(NOR_HandleTypeDef *hnor, uint32_t uwAddress, uint16_t *pData, uint32_t uwBufferSize);

HAL_StatusTypeDef HAL_NOR_Erase_Block(NOR_HandleTypeDef *hnor, uint32_t BlockAddress, uint32_t Address);
HAL_StatusTypeDef HAL_NOR_Erase_Chip(NOR_HandleTypeDef *hnor, uint32_t Address);
HAL_StatusTypeDef HAL_NOR_Read_CFI(NOR_HandleTypeDef *hnor, NOR_CFITypeDef *pNOR_CFI);

/* NOR Control functions  *****************************************************/
HAL_StatusTypeDef HAL_NOR_WriteOperation_Enable(NOR_HandleTypeDef *hnor);
HAL_StatusTypeDef HAL_NOR_WriteOperation_Disable(NOR_HandleTypeDef *hnor);

/* NOR State functions ********************************************************/
HAL_NOR_StateTypeDef HAL_NOR_GetState(NOR_HandleTypeDef *hnor);
NOR_StatusTypedef HAL_NOR_GetStatus(NOR_HandleTypeDef *hnor, uint32_t Address, uint32_t Timeout);

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

#endif /* __STM32F4xx_HAL_NOR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
