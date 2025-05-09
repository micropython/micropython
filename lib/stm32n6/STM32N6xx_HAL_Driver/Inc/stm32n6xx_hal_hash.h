/**
  ******************************************************************************
  * @file    stm32n6xx_hal_hash.h
  * @author  MCD Application Team
  * @brief   Header file of HASH HAL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_HAL_HASH_H
#define STM32N6xx_HAL_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined (HASH)
/** @defgroup HASH HASH
  * @brief HASH HAL module driver.
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup HASH_Exported_Types HASH Exported Types
  * @{
  */

/**
  * @brief  HASH Configuration Structure definition
  */
typedef struct
{
  uint32_t DataType;    /*!< no swap (32-bit data), half word swap (16-bit data), byte swap (8-bit data) or bit swap
                             (1-bit data). This parameter can be a value of @ref HASH_Data_Type.       */

  uint32_t KeySize;     /*!< The key size is used only in HMAC operation.                 */

  uint8_t *pKey;        /*!< The key is used only in HMAC operation.                      */

  uint32_t Algorithm;   /*!<  HASH algorithm MD5, SHA1 or SHA2.
                            This parameter can be a value of @ref HASH_Algorithm_Selection */


} HASH_ConfigTypeDef;

/**
  * @brief HAL State structure definition
  */
typedef enum
{
  HAL_HASH_STATE_RESET             = 0x00U,    /*!< Peripheral is not initialized            */
  HAL_HASH_STATE_READY             = 0x01U,    /*!< Peripheral Initialized and ready for use */
  HAL_HASH_STATE_BUSY              = 0x02U,    /*!< Processing (hashing) is ongoing          */
  HAL_HASH_STATE_SUSPENDED         = 0x03U     /*!< Suspended state                          */
} HAL_HASH_StateTypeDef;

/**
  * @brief HAL phase structure definition
  */
typedef enum
{
  HAL_HASH_PHASE_READY             = 0x01U,    /*!< HASH peripheral is ready to start                  */
  HAL_HASH_PHASE_PROCESS           = 0x02U,    /*!< HASH peripheral is in HASH processing phase        */
  HAL_HASH_PHASE_HMAC_STEP_1       = 0x03U,    /*!< HASH peripheral is in HMAC step 1 processing phase
                                              (step 1 consists in entering the inner hash function key)*/
  HAL_HASH_PHASE_HMAC_STEP_2       = 0x04U,    /*!< HASH peripheral is in HMAC step 2 processing phase
                                              (step 2 consists in entering the message text)           */
  HAL_HASH_PHASE_HMAC_STEP_3       = 0x05U     /*!< HASH peripheral is in HMAC step 3 processing phase
                                              (step 3 consists in entering the outer hash function key)*/

} HAL_HASH_PhaseTypeDef;

#if (USE_HAL_HASH_SUSPEND_RESUME == 1U)
/**
  * @brief HAL HASH mode suspend definitions
  */
typedef enum
{
  HAL_HASH_SUSPEND_NONE            = 0x00U,    /*!< HASH peripheral suspension not requested */
  HAL_HASH_SUSPEND                 = 0x01U     /*!< HASH peripheral suspension is requested  */
} HAL_HASH_SuspendTypeDef;
#endif /* USE_HAL_HASH_SUSPEND_RESUME */


/**
  * @brief  HASH Handle Structure definition
  */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
typedef struct __HASH_HandleTypeDef
#else
typedef struct
#endif /* (USE_HAL_HASH_REGISTER_CALLBACKS) */
{
  HASH_TypeDef               *Instance;        /*!< HASH Register base address              */

  HASH_ConfigTypeDef           Init;           /*!< HASH required parameters                */

  uint8_t const              *pHashInBuffPtr;  /*!< Pointer to input buffer                 */

  uint8_t                    *pHashOutBuffPtr; /*!< Pointer to output buffer (digest)       */

  __IO uint32_t              HashInCount;      /*!< Counter of inputted data                */

  uint32_t                   Size;             /*!< Size of buffer to be processed in bytes */

  uint8_t                   *pHashKeyBuffPtr;  /*!< Pointer to key buffer (HMAC only)       */

  HAL_HASH_PhaseTypeDef      Phase;            /*!< HASH peripheral phase                   */

  DMA_HandleTypeDef          *hdmain;          /*!< HASH In DMA Handle parameters           */

  HAL_LockTypeDef            Lock;             /*!< Locking object                          */

  __IO  uint32_t             ErrorCode;        /*!< HASH Error code                         */

  __IO HAL_HASH_StateTypeDef State;            /*!< HASH peripheral state                   */

  __IO  uint32_t             Accumulation;     /*!< HASH multi buffers accumulation flag    */

#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
  void (* InCpltCallback)(struct __HASH_HandleTypeDef *hhash);         /*!< HASH input completion callback            */

  void (* DgstCpltCallback)(struct __HASH_HandleTypeDef *hhash);       /*!< HASH digest computation complete callback */

  void (* ErrorCallback)(struct __HASH_HandleTypeDef *hhash);          /*!< HASH error callback                       */

  void (* MspInitCallback)(struct __HASH_HandleTypeDef *hhash);        /*!< HASH Msp Init callback                    */

  void (* MspDeInitCallback)(struct __HASH_HandleTypeDef *hhash);      /*!< HASH Msp DeInit callback                  */

#endif /* (USE_HAL_HASH_REGISTER_CALLBACKS) */
#if (USE_HAL_HASH_SUSPEND_RESUME == 1U)
  __IO HAL_HASH_SuspendTypeDef    SuspendRequest;    /*!< HASH peripheral suspension request flag */

  HASH_ConfigTypeDef         Init_saved;             /*!< Saved HASH required parameters          */

  uint8_t const              *pHashInBuffPtr_saved;  /*!< Saved pointer to input buffer           */

  uint8_t                    *pHashOutBuffPtr_saved; /*!< Saved pointer to output buffer (digest) */

  __IO uint32_t              HashInCount_saved;      /*!< Saved counter of inputted data          */

  uint32_t                   Size_saved;             /*!< Saved size of buffer to be processed    */

  uint8_t                    *pHashKeyBuffPtr_saved; /*!< Saved pointer to key buffer (HMAC only) */

  HAL_HASH_PhaseTypeDef      Phase_saved;            /*!< Saved HASH peripheral phase             */
#endif /* USE_HAL_HASH_SUSPEND_RESUME */

} HASH_HandleTypeDef;

#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1U)
/**
  * @brief  HAL HASH common Callback ID enumeration definition
  */
typedef enum
{
  HAL_HASH_MSPINIT_CB_ID           = 0x00U,    /*!< HASH MspInit callback ID                       */
  HAL_HASH_MSPDEINIT_CB_ID         = 0x01U,    /*!< HASH MspDeInit callback ID                     */
  HAL_HASH_INPUTCPLT_CB_ID         = 0x02U,    /*!< HASH input completion callback ID              */
  HAL_HASH_DGSTCPLT_CB_ID          = 0x03U,    /*!< HASH digest computation completion callback ID */
  HAL_HASH_ERROR_CB_ID             = 0x04U,    /*!< HASH error callback ID                         */
} HAL_HASH_CallbackIDTypeDef;

/**
  * @brief  HAL HASH Callback pointer definition
  */
typedef  void (*pHASH_CallbackTypeDef)(HASH_HandleTypeDef *hhash);  /*!< pointer to a HASH common callback functions */

#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */


/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup HASH_Exported_Constants  HASH Exported Constants
  * @{
  */

/** @defgroup HASH_Error_Definition   HASH Error Definition
  * @{
  */
#define  HAL_HASH_ERROR_NONE             0x00000000U   /*!< No error                */
#define  HAL_HASH_ERROR_BUSY             0x00000001U   /*!< Busy flag error  */
#define  HAL_HASH_ERROR_DMA              0x00000002U   /*!< DMA-based process error */
#define  HAL_HASH_ERROR_TIMEOUT          0x00000004U   /*!< Timeout error */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1U)
#define  HAL_HASH_ERROR_INVALID_CALLBACK 0x00000010U   /*!< Invalid Callback error  */
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup HASH_Algorithm_Selection   HASH algorithm selection
  * @{
  */
#define HASH_ALGOSELECTION_SHA1         0x00000000U                         /*!< HASH function is SHA1   */
#define HASH_ALGOSELECTION_SHA224       HASH_CR_ALGO_1                      /*!< HASH function is SHA224 */
#define HASH_ALGOSELECTION_SHA256       (HASH_CR_ALGO_0 | HASH_CR_ALGO_1)   /*!< HASH function is SHA256 */
#define HASH_ALGOSELECTION_SHA384       (HASH_CR_ALGO_2 |  HASH_CR_ALGO_3)  /*!< HASH function is SHA384 */
#define HASH_ALGOSELECTION_SHA512_224   (HASH_CR_ALGO_0 | HASH_CR_ALGO_2 |  HASH_CR_ALGO_3)
/*!< HASH function is SHA512_224 */
#define HASH_ALGOSELECTION_SHA512_256   (HASH_CR_ALGO_1 | HASH_CR_ALGO_2 |  HASH_CR_ALGO_3)
/*!< HASH function is SHA512_256 */
#define HASH_ALGOSELECTION_SHA512        HASH_CR_ALGO                       /*!< HASH function is SHA512 */
/**
  * @}
  */

/** @defgroup HASH_Mode   HASH Mode
  * @{
  */
#define HASH_ALGOMODE_HASH         0x00000000U     /*!< HASH mode */
#define HASH_ALGOMODE_HMAC         HASH_CR_MODE    /*!< HMAC mode */
/**
  * @}
  */

/** @defgroup HASH_Data_Type      HASH Data Type
  * @{
  */
#define HASH_NO_SWAP          0x00000000U        /*!< 32-bit data. No swapping                     */
#define HASH_HALFWORD_SWAP    HASH_CR_DATATYPE_0 /*!< 16-bit data. Each half word is swapped       */
#define HASH_BYTE_SWAP        HASH_CR_DATATYPE_1 /*!< 8-bit data. All bytes are swapped            */
#define HASH_BIT_SWAP         HASH_CR_DATATYPE   /*!< 1-bit data. In the word all bits are swapped */
/**
  * @}
  */

/** @defgroup HASH_HMAC_KEY   key length only for HMAC mode
  * @{
  */
#define HASH_SHORTKEY      0x00000000U   /*!< HMAC Key size is <= block size (64 or 128 bytes) */
#define HASH_LONGKEY       HASH_CR_LKEY  /*!< HMAC Key size is > block size (64 or 128 bytes) */
/**
  * @}
  */

/** @defgroup HASH_flags_definition  HASH flags definitions
  * @{
  */
#define HASH_FLAG_DINIS            HASH_SR_DINIS  /*!< 16 locations are free in the DIN : new block can be entered 
                                                       in the Peripheral */
#define HASH_FLAG_DCIS             HASH_SR_DCIS   /*!< Digest calculation complete                                    */
#define HASH_FLAG_DMAS             HASH_SR_DMAS   /*!< DMA interface is enabled (DMAE=1) or a transfer is ongoing     */
#define HASH_FLAG_BUSY             HASH_SR_BUSY   /*!< The hash core is Busy, processing a block of data              */
#define HASH_FLAG_DINNE            HASH_CR_DINNE  /*!< DIN not empty : input buffer contains at least one word of data*/
/**
  * @}
  */

/** @defgroup HASH_interrupts_definition   HASH interrupts definitions
  * @{
  */
#define HASH_IT_DINI               HASH_IMR_DINIE  /*!< A new block can be entered into the input buffer (DIN) */
#define HASH_IT_DCI                HASH_IMR_DCIE   /*!< Digest calculation complete                            */

/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup HASH_Exported_Macros HASH Exported Macros
  * @{
  */

/** @brief  Check whether or not the specified HASH flag is set.
  * @param  __HANDLE__ specifies the HASH handle.
  * @param  __FLAG__ specifies the flag to check.
  *        This parameter can be one of the following values:
  *            @arg @ref HASH_FLAG_DINIS A new block can be entered into the input buffer.
  *            @arg @ref HASH_FLAG_DCIS Digest calculation complete.
  *            @arg @ref HASH_FLAG_DMAS DMA interface is enabled (DMAE=1) or a transfer is ongoing.
  *            @arg @ref HASH_FLAG_BUSY The hash core is Busy : processing a block of data.
  *            @arg @ref HASH_FLAG_DINNE DIN not empty : the input buffer contains at least one word of data.
  * @retval The new state of __FLAG__ (SET or RESET).
  */
#define __HAL_HASH_GET_FLAG(__HANDLE__, __FLAG__)  (((__FLAG__) > 8U)  ?                           \
                                                    ((((__HANDLE__)->Instance->CR & (__FLAG__)) == \
                                                      (__FLAG__)) ? SET : RESET) :                     \
                                                    ((((__HANDLE__)->Instance->SR & (__FLAG__)) == \
                                                      (__FLAG__)) ? SET : RESET) )

/** @brief  Clear the specified HASH flag.
  * @param  __HANDLE__ specifies the HASH handle.
  * @param  __FLAG__ specifies the flag to clear.
  *        This parameter can be one of the following values:
  *            @arg @ref HASH_FLAG_DINIS A new block can be entered into the input buffer.
  *            @arg @ref HASH_FLAG_DCIS Digest calculation complete
  * @retval None
  */
#define __HAL_HASH_CLEAR_FLAG(__HANDLE__, __FLAG__) CLEAR_BIT((__HANDLE__)->Instance->SR, (__FLAG__))

/** @brief  Check whether the specified HASH interrupt source is enabled or not.
  * @param  __HANDLE__ specifies the HASH handle.
  * @param __INTERRUPT__ HASH interrupt source to check
  *         This parameter can be one of the following values :
  *            @arg @ref HASH_IT_DINI  A new block can be entered into the input buffer (DIN)
  *            @arg @ref HASH_IT_DCI   Digest calculation complete
  * @retval State of interruption (TRUE or FALSE).
  */
#define __HAL_HASH_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) (((__HANDLE__)->Instance->IMR\
                                                              & (__INTERRUPT__)) == (__INTERRUPT__))

/** @brief  Enable the specified HASH interrupt.
  * @param  __HANDLE__ specifies the HASH handle.
  * @param  __INTERRUPT__ specifies the HASH interrupt source to enable.
  *          This parameter can be one of the following values:
  *            @arg @ref HASH_IT_DINI  A new block can be entered into the input buffer (DIN)
  *            @arg @ref HASH_IT_DCI   Digest calculation complete
  * @retval None
  */
#define __HAL_HASH_ENABLE_IT(__HANDLE__, __INTERRUPT__)   SET_BIT((__HANDLE__)->Instance->IMR, (__INTERRUPT__))

/** @brief  Disable the specified HASH interrupt.
  * @param  __HANDLE__ specifies the HASH handle.
  * @param  __INTERRUPT__ specifies the HASH interrupt source to disable.
  *          This parameter can be one of the following values:
  *            @arg @ref HASH_IT_DINI  A new block can be entered into the input buffer (DIN)
  *            @arg @ref HASH_IT_DCI   Digest calculation complete
  * @retval None
  */
#define __HAL_HASH_DISABLE_IT(__HANDLE__, __INTERRUPT__)   CLEAR_BIT((__HANDLE__)->Instance->IMR, (__INTERRUPT__))

/** @brief Reset HASH handle state.
  * @param  __HANDLE__ HASH handle.
  * @retval None
  */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
#define __HAL_HASH_RESET_HANDLE_STATE(__HANDLE__) do{\
                                                      (__HANDLE__)->State = HAL_HASH_STATE_RESET;\
                                                      (__HANDLE__)->MspInitCallback = NULL;      \
                                                      (__HANDLE__)->MspDeInitCallback = NULL;    \
                                                     }while(0)
#else
#define __HAL_HASH_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_HASH_STATE_RESET)
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */

/**
  * @brief  Enable the multi-buffer DMA transfer mode.
  * @note   This bit is set when hashing large files when multiple DMA transfers are needed.
  * @retval None
  */
#define __HAL_HASH_SET_MDMAT()          SET_BIT(HASH->CR, HASH_CR_MDMAT)

/**
  * @brief  Disable the multi-buffer DMA transfer mode.
  * @retval None
  */
#define __HAL_HASH_RESET_MDMAT()        CLEAR_BIT(HASH->CR, HASH_CR_MDMAT)

/**
  * @brief  HAL HASH driver version.
  * @retval None
  */
#define HAL_HASH_VERSION       200  /*!< HAL HASH driver version 2.0.0*/

/**
  * @}
 */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup HASH_Exported_Functions HASH Exported Functions
  * @{
  */

/** @addtogroup HASH_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_HASH_Init(HASH_HandleTypeDef *hhash);
HAL_StatusTypeDef HAL_HASH_DeInit(HASH_HandleTypeDef *hhash);
void HAL_HASH_MspInit(HASH_HandleTypeDef *hhash);
void HAL_HASH_MspDeInit(HASH_HandleTypeDef *hhash);
HAL_StatusTypeDef HAL_HASH_GetConfig(HASH_HandleTypeDef *hhash, HASH_ConfigTypeDef *pConf);
HAL_StatusTypeDef HAL_HASH_SetConfig(HASH_HandleTypeDef *hhash, const HASH_ConfigTypeDef *pConf);

/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_HASH_RegisterCallback(HASH_HandleTypeDef *hhash, HAL_HASH_CallbackIDTypeDef CallbackID,
                                            pHASH_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_HASH_UnRegisterCallback(HASH_HandleTypeDef *hhash, HAL_HASH_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */

HAL_StatusTypeDef HAL_HASH_ProcessSuspend(HASH_HandleTypeDef *hhash);
void HAL_HASH_Resume(HASH_HandleTypeDef *hhash, uint8_t *pMemBuffer);
void HAL_HASH_Suspend(HASH_HandleTypeDef *hhash, uint8_t *pMemBuffer);
/**
  * @}
  */

/** @addtogroup HASH_Exported_Functions_Group2 HASH processing functions
  * @{
  */

HAL_StatusTypeDef HAL_HASH_Start(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                 uint8_t *const pOutBuffer,
                                 uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_Start_IT(HASH_HandleTypeDef *hhash, const  uint8_t *const pInBuffer, uint32_t Size,
                                    uint8_t  *const pOutBuffer);
HAL_StatusTypeDef HAL_HASH_Start_DMA(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                     uint8_t  *const pOutBuffer);

HAL_StatusTypeDef HAL_HASH_Accumulate(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                      uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_AccumulateLast(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                          uint8_t *const pOutBuffer,
                                          uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_AccumulateLast_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                             uint8_t *const pOutBuffer);
HAL_StatusTypeDef HAL_HASH_Accumulate_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size);

/**
  * @}
  */

/** @addtogroup HASH_Exported_Functions_Group3 HMAC processing functions
  * @{
  */
HAL_StatusTypeDef HAL_HASH_HMAC_Start(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                      uint8_t  *const pOutBuffer,
                                      uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_HMAC_Start_DMA(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                          uint8_t *const pOutBuffer);
HAL_StatusTypeDef HAL_HASH_HMAC_Start_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                         uint8_t *const pOutBuffer);

HAL_StatusTypeDef HAL_HASH_HMAC_Accumulate(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                           uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_HMAC_AccumulateLast(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                               uint8_t *const pOutBuffer, uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_HMAC_Accumulate_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size);
HAL_StatusTypeDef HAL_HASH_HMAC_AccumulateLast_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer,
                                                  uint32_t Size, uint8_t *const pOutBuffer);

/**
  * @}
  */

/** @addtogroup HASH_Exported_Functions_Group4 HASH IRQ handler management
  * @{
  */
void HAL_HASH_IRQHandler(HASH_HandleTypeDef *hhash);
void HAL_HASH_InCpltCallback(HASH_HandleTypeDef *hhash);
void HAL_HASH_DgstCpltCallback(HASH_HandleTypeDef *hhash);
void HAL_HASH_ErrorCallback(HASH_HandleTypeDef *hhash);
HAL_HASH_StateTypeDef HAL_HASH_GetState(const HASH_HandleTypeDef *hhash);
uint32_t HAL_HASH_GetError(const HASH_HandleTypeDef *hhash);

/**
  * @}
  */

/**
  * @}
  */

/* Private macros --------------------------------------------------------*/
/** @defgroup HASH_Private_Macros   HASH Private Macros
  * @{
  */

/**
  * @brief  Return digest length in bytes.
  * @retval Digest length
  */
#define HASH_DIGEST_LENGTH(__HANDLE__) (((READ_BIT((__HANDLE__)->Instance->CR, HASH_CR_ALGO) \
                                          == HASH_ALGOSELECTION_SHA1) ?  20U : \
                                         ((READ_BIT((__HANDLE__)->Instance->CR, HASH_CR_ALGO) \
                                           == HASH_ALGOSELECTION_SHA224) ?  28U : \
                                          ((READ_BIT((__HANDLE__)->Instance->CR, HASH_CR_ALGO) \
                                            == HASH_ALGOSELECTION_SHA256) ?  32U : \
                                           ((READ_BIT((__HANDLE__)->Instance->CR, HASH_CR_ALGO) \
                                             == HASH_ALGOSELECTION_SHA384) ?  48U : \
                                            ((READ_BIT((__HANDLE__)->Instance->CR, HASH_CR_ALGO) \
                                              == HASH_ALGOSELECTION_SHA512_224) ?  28U : \
                                             ((READ_BIT((__HANDLE__)->Instance->CR, HASH_CR_ALGO) \
                                               == HASH_ALGOSELECTION_SHA512_256) ?  32U : \
                                              ((READ_BIT((__HANDLE__)->Instance->CR, HASH_CR_ALGO) \
                                                == HASH_ALGOSELECTION_SHA512) ?  64U : 20U ) ) ))))))

/**
  * @brief Ensure that HASH input data type is valid.
  * @param __DATATYPE__ HASH input data type.
  * @retval SET (__DATATYPE__ is valid) or RESET (__DATATYPE__ is invalid)
  */
#define IS_HASH_DATATYPE(__DATATYPE__) (((__DATATYPE__) == HASH_NO_SWAP)|| \
                                        ((__DATATYPE__) == HASH_HALFWORD_SWAP)|| \
                                        ((__DATATYPE__) == HASH_BYTE_SWAP) || \
                                        ((__DATATYPE__) == HASH_BIT_SWAP))

/**
  * @brief Ensure that HASH input algorithm is valid.
  * @param __ALGORITHM__ HASH algorithm.
  * @retval SET (__ALGORITHM__ is valid) or RESET (__ALGORITHM__ is invalid)
  */
#define IS_HASH_ALGORITHM(__ALGORITHM__) (((__ALGORITHM__) == HASH_ALGOSELECTION_SHA1)|| \
                                          ((__ALGORITHM__) == HASH_ALGOSELECTION_SHA224)|| \
                                          ((__ALGORITHM__) == HASH_ALGOSELECTION_SHA256)|| \
                                          ((__ALGORITHM__) == HASH_ALGOSELECTION_SHA384)|| \
                                          ((__ALGORITHM__) == HASH_ALGOSELECTION_SHA512_224)|| \
                                          ((__ALGORITHM__) == HASH_ALGOSELECTION_SHA512_256)|| \
                                          ((__ALGORITHM__) == HASH_ALGOSELECTION_SHA512))

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup HASH_Private_Constants HASH Private Constants
  * @{
  */

/**
  * @}
  */
/* Private defines -----------------------------------------------------------*/
/** @defgroup HASH_Private_Defines HASH Private Defines
  * @{
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup HASH_Private_Variables HASH Private Variables
  * @{
  */

/**
  * @}
  */
/* Private functions -----------------------------------------------------------*/

/** @addtogroup HASH_Private_Functions HASH Private Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /*  HASH*/
/**
  * @}
  */


#ifdef __cplusplus
}
#endif


#endif /* STM32N6xx_HAL_HASH_H */
