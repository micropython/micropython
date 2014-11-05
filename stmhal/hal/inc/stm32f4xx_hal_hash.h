/**
  ******************************************************************************
  * @file    stm32f4xx_hal_hash.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of HASH HAL module.
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
#ifndef __STM32F4xx_HAL_HASH_H
#define __STM32F4xx_HAL_HASH_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F415xx) || defined(STM32F417xx) || defined(STM32F437xx) || defined(STM32F439xx)

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup HASH
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  HASH Configuration Structure definition
  */
typedef struct
{
  uint32_t DataType;  /*!< 32-bit data, 16-bit data, 8-bit data or 1-bit string.
                           This parameter can be a value of @ref HASH_Data_Type */

  uint32_t KeySize;   /*!< The key size is used only in HMAC operation          */

  uint8_t* pKey;      /*!< The key is used only in HMAC operation               */
}HASH_InitTypeDef;

/** 
  * @brief HAL State structures definition
  */ 
typedef enum
{
  HAL_HASH_STATE_RESET     = 0x00,  /*!< HASH not yet initialized or disabled */
  HAL_HASH_STATE_READY     = 0x01,  /*!< HASH initialized and ready for use   */
  HAL_HASH_STATE_BUSY      = 0x02,  /*!< HASH internal process is ongoing     */
  HAL_HASH_STATE_TIMEOUT   = 0x03,  /*!< HASH timeout state                   */
  HAL_HASH_STATE_ERROR     = 0x04   /*!< HASH error state                     */
}HAL_HASH_STATETypeDef;

/** 
  * @brief HAL phase structures definition
  */
typedef enum
{
  HAL_HASH_PHASE_READY     = 0x01,  /*!< HASH peripheral is ready for initialization */
  HAL_HASH_PHASE_PROCESS   = 0x02,  /*!< HASH peripheral is in processing phase      */
}HAL_HASHPhaseTypeDef;

/** 
  * @brief  HASH Handle Structure definition
  */
typedef struct
{
      HASH_InitTypeDef           Init;              /*!< HASH required parameters       */

      uint8_t                    *pHashInBuffPtr;   /*!< Pointer to input buffer        */

      uint8_t                    *pHashOutBuffPtr;  /*!< Pointer to input buffer        */

     __IO uint32_t               HashBuffSize;      /*!< Size of buffer to be processed */

     __IO uint32_t               HashInCount;       /*!< Counter of inputed data        */

     __IO uint32_t               HashITCounter;     /*!< Counter of issued interrupts   */

      HAL_StatusTypeDef          Status;            /*!< HASH peripheral status         */

      HAL_HASHPhaseTypeDef       Phase;             /*!< HASH peripheral phase          */

      DMA_HandleTypeDef          *hdmain;           /*!< HASH In DMA handle parameters  */

      HAL_LockTypeDef            Lock;              /*!< HASH locking object            */

     __IO HAL_HASH_STATETypeDef  State;             /*!< HASH peripheral state          */
} HASH_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup HASH_Exported_Constants
  * @{
  */

/** @defgroup HASH_Algo_Selection
  * @{
  */
#define HASH_AlgoSelection_SHA1      ((uint32_t)0x0000)  /*!< HASH function is SHA1   */
#define HASH_AlgoSelection_SHA224    HASH_CR_ALGO_1      /*!< HASH function is SHA224 */
#define HASH_AlgoSelection_SHA256    HASH_CR_ALGO        /*!< HASH function is SHA256 */
#define HASH_AlgoSelection_MD5       HASH_CR_ALGO_0      /*!< HASH function is MD5    */

#define IS_HASH_ALGOSELECTION(ALGOSELECTION) (((ALGOSELECTION) == HASH_AlgoSelection_SHA1)   || \
                                              ((ALGOSELECTION) == HASH_AlgoSelection_SHA224) || \
                                              ((ALGOSELECTION) == HASH_AlgoSelection_SHA256) || \
                                              ((ALGOSELECTION) == HASH_AlgoSelection_MD5))
/**
  * @}
  */

/** @defgroup HASH_Algorithm_Mode
  * @{
  */ 
#define HASH_AlgoMode_HASH         ((uint32_t)0x00000000)  /*!< Algorithm is HASH */ 
#define HASH_AlgoMode_HMAC         HASH_CR_MODE            /*!< Algorithm is HMAC */

#define IS_HASH_ALGOMODE(ALGOMODE) (((ALGOMODE) == HASH_AlgoMode_HASH) || \
                                    ((ALGOMODE) == HASH_AlgoMode_HMAC))
/**
  * @}
  */

/** @defgroup HASH_Data_Type
  * @{
  */
#define HASH_DATATYPE_32B          ((uint32_t)0x0000) /*!< 32-bit data. No swapping                     */
#define HASH_DATATYPE_16B          HASH_CR_DATATYPE_0 /*!< 16-bit data. Each half word is swapped       */
#define HASH_DATATYPE_8B           HASH_CR_DATATYPE_1 /*!< 8-bit data. All bytes are swapped            */
#define HASH_DATATYPE_1B           HASH_CR_DATATYPE   /*!< 1-bit data. In the word all bits are swapped */

#define IS_HASH_DATATYPE(DATATYPE) (((DATATYPE) == HASH_DATATYPE_32B)|| \
                                    ((DATATYPE) == HASH_DATATYPE_16B)|| \
                                    ((DATATYPE) == HASH_DATATYPE_8B) || \
                                    ((DATATYPE) == HASH_DATATYPE_1B))
/**
  * @}
  */

/** @defgroup HASH_HMAC_Long_key_only_for_HMAC_mode
  * @{
  */ 
#define HASH_HMACKeyType_ShortKey      ((uint32_t)0x00000000)  /*!< HMAC Key is <= 64 bytes */
#define HASH_HMACKeyType_LongKey       HASH_CR_LKEY            /*!< HMAC Key is > 64 bytes  */

#define IS_HASH_HMAC_KEYTYPE(KEYTYPE) (((KEYTYPE) == HASH_HMACKeyType_ShortKey) || \
                                       ((KEYTYPE) == HASH_HMACKeyType_LongKey))
/**
  * @}
  */

/** @defgroup HASH_flags_definition
  * @{
  */
#define HASH_FLAG_DINIS            HASH_SR_DINIS  /*!< 16 locations are free in the DIN : A new block can be entered into the input buffer */
#define HASH_FLAG_DCIS             HASH_SR_DCIS   /*!< Digest calculation complete                                                         */
#define HASH_FLAG_DMAS             HASH_SR_DMAS   /*!< DMA interface is enabled (DMAE=1) or a transfer is ongoing                          */
#define HASH_FLAG_BUSY             HASH_SR_BUSY   /*!< The hash core is Busy : processing a block of data                                  */
#define HASH_FLAG_DINNE            HASH_CR_DINNE  /*!< DIN not empty : The input buffer contains at least one word of data                 */
/**
  * @}
  */

/** @defgroup HASH_interrupts_definition
  * @{
  */  
#define HASH_IT_DINI               HASH_IMR_DINIM  /*!< A new block can be entered into the input buffer (DIN) */
#define HASH_IT_DCI                HASH_IMR_DCIM   /*!< Digest calculation complete                            */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @brief Reset HASH handle state
  * @param  __HANDLE__: specifies the HASH handle.
  * @retval None
  */
#define __HAL_HASH_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_HASH_STATE_RESET)

/** @brief  Check whether the specified HASH flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg HASH_FLAG_DINIS: A new block can be entered into the input buffer. 
  *            @arg HASH_FLAG_DCIS: Digest calculation complete
  *            @arg HASH_FLAG_DMAS: DMA interface is enabled (DMAE=1) or a transfer is ongoing
  *            @arg HASH_FLAG_BUSY: The hash core is Busy : processing a block of data
  *            @arg HASH_FLAG_DINNE: DIN not empty : The input buffer contains at least one word of data
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_HASH_GET_FLAG(__FLAG__) ((HASH->SR & (__FLAG__)) == (__FLAG__))

/**
  * @brief  Macros for HMAC finish.
  * @param  None
  * @retval None
  */
#define HAL_HMAC_MD5_Finish          HAL_HASH_MD5_Finish
#define HAL_HMAC_SHA1_Finish         HAL_HASH_SHA1_Finish
#define HAL_HMAC_SHA224_Finish       HAL_HASH_SHA224_Finish
#define HAL_HMAC_SHA256_Finish       HAL_HASH_SHA256_Finish

/**
  * @brief  Enable the multiple DMA mode. 
  *         This feature is available only in STM32F429x and STM32F439x devices.
  * @param  None
  * @retval None
  */
#define __HAL_HASH_SET_MDMAT()          HASH->CR |= HASH_CR_MDMAT

/**
  * @brief  Disable the multiple DMA mode.
  * @param  None
  * @retval None
  */
#define __HAL_HASH_RESET_MDMAT()        HASH->CR &= (uint32_t)(~HASH_CR_MDMAT)

/**
  * @brief  Start the digest computation
  * @param  None
  * @retval None
  */
#define __HAL_HASH_START_DIGEST()       HASH->STR |= HASH_STR_DCAL

/**
  * @brief Set the number of valid bits in last word written in Data register
  * @param  SIZE: size in byte of last data written in Data register.
  * @retval None
*/
#define __HAL_HASH_SET_NBVALIDBITS(SIZE) do{HASH->STR &= ~(HASH_STR_NBW);\
                                            HASH->STR |= 8 * ((SIZE) % 4);\
                                           }while(0)

/* Include HASH HAL Extension module */
#include "stm32f4xx_hal_hash_ex.h"

/* Exported functions --------------------------------------------------------*/

/* Initialization and de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_HASH_Init(HASH_HandleTypeDef *hhash);
HAL_StatusTypeDef HAL_HASH_DeInit(HASH_HandleTypeDef *hhash);

/* HASH processing using polling  *********************************************/
HAL_StatusTypeDef HAL_HASH_SHA1_Start(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer, uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_MD5_Start(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer, uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_MD5_Accumulate(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size);
HAL_StatusTypeDef HAL_HASH_SHA1_Accumulate(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size);

/* HASH-MAC processing using polling  *****************************************/
HAL_StatusTypeDef HAL_HMAC_SHA1_Start(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer, uint32_t Timeout);
HAL_StatusTypeDef HAL_HMAC_MD5_Start(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer, uint32_t Timeout);

/* HASH processing using interrupt  *******************************************/
HAL_StatusTypeDef HAL_HASH_SHA1_Start_IT(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer);
HAL_StatusTypeDef HAL_HASH_MD5_Start_IT(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer);

/* HASH processing using DMA  *************************************************/
HAL_StatusTypeDef HAL_HASH_SHA1_Start_DMA(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size);
HAL_StatusTypeDef HAL_HASH_SHA1_Finish(HASH_HandleTypeDef *hhash, uint8_t* pOutBuffer, uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_MD5_Start_DMA(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size);
HAL_StatusTypeDef HAL_HASH_MD5_Finish(HASH_HandleTypeDef *hhash, uint8_t* pOutBuffer, uint32_t Timeout);

/* HASH-HMAC processing using DMA  ********************************************/
HAL_StatusTypeDef HAL_HMAC_SHA1_Start_DMA(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size);
HAL_StatusTypeDef HAL_HMAC_MD5_Start_DMA(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size);

/* Processing functions  ******************************************************/
void HAL_HASH_IRQHandler(HASH_HandleTypeDef *hhash);

/* Peripheral State functions  ************************************************/
HAL_HASH_STATETypeDef HAL_HASH_GetState(HASH_HandleTypeDef *hhash);
void HAL_HASH_MspInit(HASH_HandleTypeDef *hhash);
void HAL_HASH_MspDeInit(HASH_HandleTypeDef *hhash);
void HAL_HASH_InCpltCallback(HASH_HandleTypeDef *hhash);
void HAL_HASH_DgstCpltCallback(HASH_HandleTypeDef *hhash);
void HAL_HASH_ErrorCallback(HASH_HandleTypeDef *hhash);

#endif /* STM32F415xx || STM32F417xx || STM32F437xx || STM32F439xx */
/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif


#endif /* __STM32F4xx_HAL_HASH_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
