/**
  ******************************************************************************
  * @file    stm32f4xx_hal_cryp.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of CRYP HAL module.
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
#ifndef __STM32F4xx_HAL_CRYP_H
#define __STM32F4xx_HAL_CRYP_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32F415xx) || defined(STM32F417xx) || defined(STM32F437xx) || defined(STM32F439xx)
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup CRYP
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  CRYP Configuration Structure definition
  */
typedef struct
{
  uint32_t DataType;    /*!< 32-bit data, 16-bit data, 8-bit data or 1-bit string.
                             This parameter can be a value of @ref CRYP_Data_Type */

  uint32_t KeySize;     /*!< Used only in AES mode only : 128, 192 or 256 bit key length. 
                             This parameter can be a value of @ref CRYP_Key_Size */

  uint8_t* pKey;        /*!< The key used for encryption/decryption */

  uint8_t* pInitVect;   /*!< The initialization vector used also as initialization
                             counter in CTR mode */

  uint8_t IVSize;       /*!< The size of initialization vector. 
                             This parameter (called nonce size in CCM) is used only 
                             in AES-128/192/256 encryption/decryption CCM mode */

  uint8_t TagSize;      /*!< The size of returned authentication TAG. 
                             This parameter is used only in AES-128/192/256 
                             encryption/decryption CCM mode */

  uint8_t* Header;      /*!< The header used in GCM and CCM modes */

  uint16_t HeaderSize;  /*!< The size of header buffer in bytes */

  uint8_t* pScratch;    /*!< Scratch buffer used to append the header. It's size must be equal to header size + 21 bytes.
                             This parameter is used only in AES-128/192/256 encryption/decryption CCM mode */
}CRYP_InitTypeDef;

/** 
  * @brief HAL CRYP State structures definition
  */
typedef enum
{
  HAL_CRYP_STATE_RESET             = 0x00,  /*!< CRYP not yet initialized or disabled  */
  HAL_CRYP_STATE_READY             = 0x01,  /*!< CRYP initialized and ready for use    */
  HAL_CRYP_STATE_BUSY              = 0x02,  /*!< CRYP internal processing is ongoing   */
  HAL_CRYP_STATE_TIMEOUT           = 0x03,  /*!< CRYP timeout state                    */
  HAL_CRYP_STATE_ERROR             = 0x04   /*!< CRYP error state                      */
}HAL_CRYP_STATETypeDef;

/** 
  * @brief HAL CRYP phase structures definition
  */
typedef enum
{
  HAL_CRYP_PHASE_READY             = 0x01,    /*!< CRYP peripheral is ready for initialization. */
  HAL_CRYP_PHASE_PROCESS           = 0x02,    /*!< CRYP peripheral is in processing phase */
  HAL_CRYP_PHASE_FINAL             = 0x03     /*!< CRYP peripheral is in final phase
                                                   This is relevant only with CCM and GCM modes */
}HAL_PhaseTypeDef;

/** 
  * @brief  CRYP handle Structure definition
  */ 
typedef struct
{
      CRYP_InitTypeDef         Init;             /*!< CRYP required parameters */

      uint8_t                  *pCrypInBuffPtr;  /*!< Pointer to CRYP processing (encryption, decryption,...) buffer */

      uint8_t                  *pCrypOutBuffPtr; /*!< Pointer to CRYP processing (encryption, decryption,...) buffer */

      __IO uint16_t            CrypInCount;      /*!< Counter of inputed data */

      __IO uint16_t            CrypOutCount;     /*!< Counter of outputed data */

      HAL_StatusTypeDef        Status;           /*!< CRYP peripheral status */

      HAL_PhaseTypeDef         Phase;            /*!< CRYP peripheral phase */

      DMA_HandleTypeDef        *hdmain;          /*!< CRYP In DMA handle parameters */

      DMA_HandleTypeDef        *hdmaout;         /*!< CRYP Out DMA handle parameters */

      HAL_LockTypeDef          Lock;             /*!< CRYP locking object */

   __IO  HAL_CRYP_STATETypeDef State;            /*!< CRYP peripheral state */
}CRYP_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup CRYP_Exported_Constants
  * @{
  */

/** @defgroup CRYP_Key_Size 
  * @{
  */
#define CRYP_KEYSIZE_128B         ((uint32_t)0x00000000)
#define CRYP_KEYSIZE_192B         CRYP_CR_KEYSIZE_0
#define CRYP_KEYSIZE_256B         CRYP_CR_KEYSIZE_1

#define IS_CRYP_KEYSIZE(KEYSIZE)  (((KEYSIZE) == CRYP_KEYSIZE_128B)  || \
                                   ((KEYSIZE) == CRYP_KEYSIZE_192B)  || \
                                   ((KEYSIZE) == CRYP_KEYSIZE_256B))
/**
  * @}
  */

/** @defgroup CRYP_Data_Type 
  * @{
  */
#define CRYP_DATATYPE_32B         ((uint32_t)0x00000000)
#define CRYP_DATATYPE_16B         CRYP_CR_DATATYPE_0
#define CRYP_DATATYPE_8B          CRYP_CR_DATATYPE_1
#define CRYP_DATATYPE_1B          CRYP_CR_DATATYPE

#define IS_CRYP_DATATYPE(DATATYPE) (((DATATYPE) == CRYP_DATATYPE_32B) || \
                                    ((DATATYPE) == CRYP_DATATYPE_16B) || \
                                    ((DATATYPE) == CRYP_DATATYPE_8B)  || \
                                    ((DATATYPE) == CRYP_DATATYPE_1B))  
/**
  * @}
  */

/** @defgroup CRYP_AlgoModeDirection
  * @{
  */ 
#define CRYP_CR_ALGOMODE_DIRECTION         ((uint32_t)0x0008003C)
#define CRYP_CR_ALGOMODE_TDES_ECB_ENCRYPT  ((uint32_t)0x00000000)
#define CRYP_CR_ALGOMODE_TDES_ECB_DECRYPT  ((uint32_t)0x00000004)
#define CRYP_CR_ALGOMODE_TDES_CBC_ENCRYPT  ((uint32_t)0x00000008)
#define CRYP_CR_ALGOMODE_TDES_CBC_DECRYPT  ((uint32_t)0x0000000C)
#define CRYP_CR_ALGOMODE_DES_ECB_ENCRYPT   ((uint32_t)0x00000010)
#define CRYP_CR_ALGOMODE_DES_ECB_DECRYPT   ((uint32_t)0x00000014)
#define CRYP_CR_ALGOMODE_DES_CBC_ENCRYPT   ((uint32_t)0x00000018)
#define CRYP_CR_ALGOMODE_DES_CBC_DECRYPT   ((uint32_t)0x0000001C)
#define CRYP_CR_ALGOMODE_AES_ECB_ENCRYPT   ((uint32_t)0x00000020)
#define CRYP_CR_ALGOMODE_AES_ECB_DECRYPT   ((uint32_t)0x00000024)
#define CRYP_CR_ALGOMODE_AES_CBC_ENCRYPT   ((uint32_t)0x00000028)
#define CRYP_CR_ALGOMODE_AES_CBC_DECRYPT   ((uint32_t)0x0000002C)
#define CRYP_CR_ALGOMODE_AES_CTR_ENCRYPT   ((uint32_t)0x00000030)
#define CRYP_CR_ALGOMODE_AES_CTR_DECRYPT   ((uint32_t)0x00000034)
/**
  * @}
  */
  
/** @defgroup CRYP_Interrupt
  * @{
  */
#define CRYP_IT_INI               ((uint32_t)CRYP_IMSCR_INIM)   /*!< Input FIFO Interrupt */
#define CRYP_IT_OUTI              ((uint32_t)CRYP_IMSCR_OUTIM)  /*!< Output FIFO Interrupt */
/**
  * @}
  */

/** @defgroup CRYP_Flags 
  * @{
  */

#define CRYP_FLAG_BUSY   ((uint32_t)0x00000010)  /*!< The CRYP core is currently 
                                                     processing a block of data 
                                                     or a key preparation (for 
                                                     AES decryption). */
#define CRYP_FLAG_IFEM   ((uint32_t)0x00000001)  /*!< Input FIFO is empty */
#define CRYP_FLAG_IFNF   ((uint32_t)0x00000002)  /*!< Input FIFO is not Full */
#define CRYP_FLAG_OFNE   ((uint32_t)0x00000004)  /*!< Output FIFO is not empty */
#define CRYP_FLAG_OFFU   ((uint32_t)0x00000008)  /*!< Output FIFO is Full */
#define CRYP_FLAG_OUTRIS ((uint32_t)0x01000002)  /*!< Output FIFO service raw 
                                                      interrupt status */
#define CRYP_FLAG_INRIS  ((uint32_t)0x01000001)  /*!< Input FIFO service raw 
                                                      interrupt status */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @brief Reset CRYP handle state
  * @param  __HANDLE__: specifies the CRYP handle.
  * @retval None
  */
#define __HAL_CRYP_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_CRYP_STATE_RESET)

/**
  * @brief  Enable/Disable the CRYP peripheral.
  * @param  None
  * @retval None
  */
#define __HAL_CRYP_ENABLE()  (CRYP->CR |=  CRYP_CR_CRYPEN)
#define __HAL_CRYP_DISABLE() (CRYP->CR &=  ~CRYP_CR_CRYPEN)

/**
  * @brief  Flush the data FIFO.
  * @param  None
  * @retval None
  */
#define __HAL_CRYP_FIFO_FLUSH() (CRYP->CR |=  CRYP_CR_FFLUSH)

/**
  * @brief  Set the algorithm mode: AES-ECB, AES-CBC, AES-CTR, DES-ECB, DES-CBC.
  * @param  MODE: The algorithm mode.
  * @retval None
  */
#define __HAL_CRYP_SET_MODE(MODE)  CRYP->CR |= (uint32_t)(MODE)

/** @brief  Check whether the specified CRYP flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg CRYP_FLAG_BUSY: The CRYP core is currently processing a block of data 
  *                                 or a key preparation (for AES decryption). 
  *            @arg CRYP_FLAG_IFEM: Input FIFO is empty
  *            @arg CRYP_FLAG_IFNF: Input FIFO is not full
  *            @arg CRYP_FLAG_INRIS: Input FIFO service raw interrupt is pending
  *            @arg CRYP_FLAG_OFNE: Output FIFO is not empty
  *            @arg CRYP_FLAG_OFFU: Output FIFO is full
  *            @arg CRYP_FLAG_OUTRIS: Input FIFO service raw interrupt is pending
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define CRYP_FLAG_MASK  ((uint32_t)0x0000001F)
#define __HAL_CRYP_GET_FLAG(__FLAG__) ((((uint8_t)((__FLAG__) >> 24)) == 0x01)?(((CRYP->RISR) & ((__FLAG__) & CRYP_FLAG_MASK)) == ((__FLAG__) & CRYP_FLAG_MASK)): \
                                                 (((CRYP->RISR) & ((__FLAG__) & CRYP_FLAG_MASK)) == ((__FLAG__) & CRYP_FLAG_MASK)))

/** @brief  Check whether the specified CRYP interrupt is set or not.
  * @param  __INTERRUPT__: specifies the interrupt to check.
  *         This parameter can be one of the following values:
  *            @arg CRYP_IT_INRIS: Input FIFO service raw interrupt is pending
  *            @arg CRYP_IT_OUTRIS: Output FIFO service raw interrupt is pending
  * @retval The new state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_CRYP_GET_IT(__INTERRUPT__) ((CRYP->MISR & (__INTERRUPT__)) == (__INTERRUPT__))

/**
  * @brief  Enable the CRYP interrupt.
  * @param  __INTERRUPT__: CRYP Interrupt.
  * @retval None
  */
#define __HAL_CRYP_ENABLE_IT(__INTERRUPT__) ((CRYP->IMSCR) |= (__INTERRUPT__))

/**
  * @brief  Disable the CRYP interrupt.
  * @param  __INTERRUPT__: CRYP interrupt.
  * @retval None
  */
#define __HAL_CRYP_DISABLE_IT(__INTERRUPT__) ((CRYP->IMSCR) &= ~(__INTERRUPT__))

/* Include CRYP HAL Extension module */
#include "stm32f4xx_hal_cryp_ex.h"

/* Exported functions --------------------------------------------------------*/
/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef HAL_CRYP_Init(CRYP_HandleTypeDef *hcryp);
HAL_StatusTypeDef HAL_CRYP_DeInit(CRYP_HandleTypeDef *hcryp);

/* AES encryption/decryption using polling  ***********************************/
HAL_StatusTypeDef HAL_CRYP_AESECB_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_AESECB_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_AESCBC_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_AESCBC_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_AESCTR_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_AESCTR_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout);

/* AES encryption/decryption using interrupt  *********************************/
HAL_StatusTypeDef HAL_CRYP_AESECB_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_AESCBC_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_AESCTR_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_AESECB_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);
HAL_StatusTypeDef HAL_CRYP_AESCTR_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);
HAL_StatusTypeDef HAL_CRYP_AESCBC_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);

/* AES encryption/decryption using DMA  ***************************************/
HAL_StatusTypeDef HAL_CRYP_AESECB_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_AESECB_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);
HAL_StatusTypeDef HAL_CRYP_AESCBC_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_AESCBC_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);
HAL_StatusTypeDef HAL_CRYP_AESCTR_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_AESCTR_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);

/* DES encryption/decryption using polling  ***********************************/
HAL_StatusTypeDef HAL_CRYP_DESECB_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_DESCBC_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_DESECB_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_DESCBC_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout);

/* DES encryption/decryption using interrupt  *********************************/
HAL_StatusTypeDef HAL_CRYP_DESECB_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_DESECB_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);
HAL_StatusTypeDef HAL_CRYP_DESCBC_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_DESCBC_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);

/* DES encryption/decryption using DMA  ***************************************/
HAL_StatusTypeDef HAL_CRYP_DESECB_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_DESECB_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_DESCBC_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_DESCBC_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);

/* TDES encryption/decryption using polling  **********************************/
HAL_StatusTypeDef HAL_CRYP_TDESECB_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_TDESECB_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout);

/* TDES encryption/decryption using interrupt  ********************************/
HAL_StatusTypeDef HAL_CRYP_TDESECB_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_TDESECB_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);

/* TDES encryption/decryption using DMA  **************************************/
HAL_StatusTypeDef HAL_CRYP_TDESECB_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_TDESECB_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData);
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData);

/* Processing functions  ******************************************************/
void HAL_CRYP_IRQHandler(CRYP_HandleTypeDef *hcryp);

/* Peripheral State functions  ************************************************/
HAL_CRYP_STATETypeDef HAL_CRYP_GetState(CRYP_HandleTypeDef *hcryp);

/* MSP functions  *************************************************************/
void HAL_CRYP_MspInit(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef *hcryp);

/* CallBack functions  ********************************************************/
void HAL_CRYP_InCpltCallback(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_OutCpltCallback(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_ErrorCallback(CRYP_HandleTypeDef *hcryp);

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

#endif /* __STM32F4xx_HAL_CRYP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
