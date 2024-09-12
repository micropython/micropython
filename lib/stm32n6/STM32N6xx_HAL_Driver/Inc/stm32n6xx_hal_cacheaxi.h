/**
  ******************************************************************************
  * @file    stm32n6xx_hal_cacheaxi.h
  * @author  MCD Application Team
  * @brief   Header file of CACHEAXI HAL module.
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

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef STM32N6xx_HAL_CACHEAXI_H
#define STM32N6xx_HAL_CACHEAXI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#if defined (CACHEAXI)
/** @addtogroup CACHEAXI
  * @{
  */
/* Exported types ------------------------------------------------------------*/

/** @defgroup CACHEAXI_Exported_Types CACHEAXI Exported Types
  * @{
  */

/**
  * @brief  HAL State structures definition
  */
typedef enum
{
  HAL_CACHEAXI_STATE_RESET   = 0x00U, /*!< CACHEAXI not yet initialized or disabled   */
  HAL_CACHEAXI_STATE_READY   = 0x01U, /*!< Peripheral initialized and ready for use   */
  HAL_CACHEAXI_STATE_BUSY    = 0x02U, /*!< An internal process is ongoing             */
  HAL_CACHEAXI_STATE_TIMEOUT = 0x05U, /*!< Timeout state                              */
  HAL_CACHEAXI_STATE_ERROR   = 0x06U, /*!< CACHEAXI state error                       */
} HAL_CACHEAXI_StateTypeDef;

/** @defgroup CACHEAXI_Configuration_Structure_definition CACHEAXI Configuration Structure definition
  * @brief  CACHEAXI Configuration Structure definition
  * @{
  */
#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
typedef struct __CACHEAXI_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */
{
  CACHEAXI_TypeDef               *Instance;    /*!< CACHEAXI register base address.    */
  __IO HAL_CACHEAXI_StateTypeDef State;
  __IO uint32_t                  ErrorCode;

#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
  void (* ErrorCallback)(struct __CACHEAXI_HandleTypeDef *hcacheaxi);
  void (* CleanByAddrCallback)(struct __CACHEAXI_HandleTypeDef *hcacheaxi);
  void (* InvalidateCompleteCallback)(struct __CACHEAXI_HandleTypeDef *hcacheaxi);
  void (* CleanAndInvalidateByAddrCallback)(struct __CACHEAXI_HandleTypeDef *hcacheaxi);

  void (* MspInitCallback)(struct __CACHEAXI_HandleTypeDef *hcacheaxi);
  void (* MspDeInitCallback)(struct __CACHEAXI_HandleTypeDef *hcacheaxi);
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */
} CACHEAXI_HandleTypeDef;

#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL CACHEAXI Callback ID enumeration definition
  */
typedef enum
{
  HAL_CACHEAXI_CLEAN_BY_ADDRESS_CB_ID                 = 0x00U, /*!< CACHEAXI Clean By Address callback ID                */
  HAL_CACHEAXI_CLEAN_AND_INVALIDATE_BY_ADDRESS_CB_ID  = 0x01U, /*!< CACHEAXI Clean And Invalidate By Address callback ID */
  HAL_CACHEAXI_INVALIDATE_COMPLETE_CB_ID              = 0x02U, /*!< CACHEAXI Invalidate Complete ID                      */
  HAL_CACHEAXI_ERROR_CB_ID                            = 0x03U, /*!< CACHEAXI Error callback ID                           */

  HAL_CACHEAXI_MSPINIT_CB_ID                          = 0x04U, /*!< CACHEAXI Msp Init callback ID                        */
  HAL_CACHEAXI_MSPDEINIT_CB_ID                        = 0x05U  /*!< CACHEAXI Msp DeInit callback ID                      */
} HAL_CACHEAXI_CallbackIDTypeDef;

/**
  * @brief  HAL CACHEAXI Callback pointer definition
  */
/*!< Pointer to a CACHEAXI common callback function */
typedef  void (*pCACHEAXI_CallbackTypeDef)(CACHEAXI_HandleTypeDef *hcacheaxi);

#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */

/**
  * @}
  */

/**
  * @}
  */

/* Exported constants -------------------------------------------------------*/
/** @defgroup CACHEAXI_Exported_Constants CACHEAXI Exported Constants
  * @{
  */

/** @defgroup CACHEAXI_Error_Code CACHEAXI Error Code
  * @{
  */
#define HAL_CACHEAXI_ERROR_NONE              0x00000000U /*!< No error                */
#define HAL_CACHEAXI_ERROR_TIMEOUT           0x00000010U /*!< Timeout error           */
#define HAL_CACHEAXI_ERROR_EVICTION_CLEAN    0x00000020U /*!< Eviction or clean operation write-back error */
#define HAL_CACHEAXI_ERROR_INVALID_OPERATION 0x00000040U /*!< Invalid operation       */
#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
#define HAL_CACHEAXI_ERROR_INVALID_CALLBACK  0x00000080U /*!< Invalid callback error  */
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup CACHEAXI_Monitor_Type Monitor type
  * @{
  */
#define CACHEAXI_MONITOR_READ_HIT         CACHEAXI_CR1_RHITMEN   /*!< Read Hit monitoring */
#define CACHEAXI_MONITOR_READ_MISS        CACHEAXI_CR1_RMISSMEN  /*!< Read Miss monitoring */
#define CACHEAXI_MONITOR_WRITE_HIT        CACHEAXI_CR1_WHITMEN   /*!< Write Hit monitoring */
#define CACHEAXI_MONITOR_WRITE_MISS       CACHEAXI_CR1_WMISSMEN  /*!< Write Miss monitoring */
#define CACHEAXI_MONITOR_READALLOC_MISS   CACHEAXI_CR1_RAMMEN    /*!< Read-allocate Miss monitoring */
#define CACHEAXI_MONITOR_WRITEALLOC_MISS  CACHEAXI_CR1_WAMMEN    /*!< Write-allocate Miss monitoring */
#define CACHEAXI_MONITOR_WRITETHROUGH     CACHEAXI_CR1_WTMEN     /*!< Write-through monitoring */
#define CACHEAXI_MONITOR_EVICTION         CACHEAXI_CR1_EVIMEN    /*!< Eviction monitoring */
#define CACHEAXI_MONITOR_ALL              (CACHEAXI_CR1_RHITMEN | CACHEAXI_CR1_RMISSMEN | \
                                           CACHEAXI_CR1_WHITMEN | CACHEAXI_CR1_WMISSMEN | \
                                           CACHEAXI_CR1_RAMMEN  | CACHEAXI_CR1_WAMMEN   | \
                                           CACHEAXI_CR1_WTMEN   | CACHEAXI_CR1_EVIMEN   )
/**
  * @}
  */

/** @defgroup CACHEAXI_Interrupts Interrupts
  * @{
  */
#define CACHEAXI_IT_BUSYEND              CACHEAXI_IER_BSYENDIE /*!< Busy end interrupt */
#define CACHEAXI_IT_ERROR                CACHEAXI_IER_ERRIE    /*!< Cache error interrupt */
#define CACHEAXI_IT_CMDEND               CACHEAXI_IER_CMDENDIE /*!< Command end interrupt */
/**
  * @}
  */

/** @defgroup CACHEAXI_Flags Flags
  * @{
  */
#define CACHEAXI_FLAG_BUSY               CACHEAXI_SR_BUSYF     /*!< Busy flag */
#define CACHEAXI_FLAG_BUSYEND            CACHEAXI_SR_BSYENDF   /*!< Busy end flag */
#define CACHEAXI_FLAG_ERROR              CACHEAXI_SR_ERRF      /*!< Cache error flag */
#define CACHEAXI_FLAG_BUSYCMD            CACHEAXI_SR_BUSYCMDF  /*!< Busy command flag */
#define CACHEAXI_FLAG_CMDEND             CACHEAXI_SR_CMDENDF   /*!< Command end flag */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros ----------------------------------------------------------*/
/** @defgroup CACHEAXI_Exported_Macros CACHEAXI Exported Macros
  * @{
  */

/** @brief  Enable CACHEAXI interrupts.
  * @param  __HANDLE__ specifies the CACHEAXI handle.
  * @param  __INTERRUPT__ specifies the CACHEAXI interrupt sources to be enabled.
  *         This parameter can be any combination of the following values:
  *            @arg @ref CACHEAXI_IT_BUSYEND  Busy end interrupt
  *            @arg @ref CACHEAXI_IT_ERROR    Cache error interrupt
  *            @arg @ref CACHEAXI_IT_CMDEND   Cache Command end interrupt
  * @retval None
  */
#define __HAL_CACHEAXI_ENABLE_IT(__HANDLE__, __INTERRUPT__) SET_BIT((__HANDLE__)->Instance->IER, (__INTERRUPT__))

/** @brief  Disable CACHEAXI interrupts.
  * @param  __HANDLE__ specifies the CACHEAXI handle.
  * @param  __INTERRUPT__ specifies the CACHEAXI interrupt sources to be disabled.
  *         This parameter can be any combination of the following values:
  *            @arg @ref CACHEAXI_IT_BUSYEND  Busy end interrupt
  *            @arg @ref CACHEAXI_IT_ERROR    Cache error interrupt
  *            @arg @ref CACHEAXI_IT_CMDEND   Cache Command end interrupt
  * @retval None
  */
#define __HAL_CACHEAXI_DISABLE_IT(__HANDLE__, __INTERRUPT__) CLEAR_BIT((__HANDLE__)->Instance->IER, (__INTERRUPT__))

/** @brief  Check whether the specified CACHEAXI interrupt source is enabled or not.
  * @param  __HANDLE__ specifies the CACHEAXI handle.
  * @param  __INTERRUPT__ specifies the CACHEAXI interrupt source to check.
  *         This parameter can be any combination of the following values:
  *            @arg @ref CACHEAXI_IT_BUSYEND  Busy end interrupt
  *            @arg @ref CACHEAXI_IT_ERROR    Cache error interrupt
  *            @arg @ref CACHEAXI_IT_CMDEND   Cache Command end interrupt
  *
  * @retval The state of __INTERRUPT__ (SET or RESET).
  */
#define __HAL_CACHEAXI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) \
  ((READ_BIT((__HANDLE__)->Instance->IER, (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Check whether the selected CACHEAXI flag is set or not.
  * @param  __HANDLE__ specifies the CACHEAXI handle.
  * @param  __FLAG__   specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg @ref CACHEAXI_FLAG_BUSY     Busy flag
  *            @arg @ref CACHEAXI_FLAG_BUSYEND  Busy end flag
  *            @arg @ref CACHEAXI_FLAG_ERROR    Cache error flag
  *            @arg @ref CACHEAXI_FLAG_BUSYCMD  Cache Busy command flag
  *            @arg @ref CACHEAXI_FLAG_CMDEND   Cache command end flag
  * @retval The state of __FLAG__ (0 or 1).
  */
#define __HAL_CACHEAXI_GET_FLAG(__HANDLE__, __FLAG__) \
  ((READ_BIT((__HANDLE__)->Instance->SR, (__FLAG__)) != 0U) ? 1U : 0U)

/** @brief  Clear the selected CACHEAXI flags.
  * @param  __HANDLE__ specifies the CACHEAXI handle.
  * @param  __FLAG__   specifies the CACHEAXI flags to clear.
  *         This parameter can be any combination of the following values:
  *            @arg @ref CACHEAXI_FLAG_BUSYEND  Busy end flag
  *            @arg @ref CACHEAXI_FLAG_ERROR    Cache error flag
  *            @arg @ref CACHEAXI_FLAG_CMDEND   Cache command end flag
  */
#define __HAL_CACHEAXI_CLEAR_FLAG(__HANDLE__, __FLAG__) WRITE_REG((__HANDLE__)->Instance->FCR, (__FLAG__))

/**
  * @}
  */

/* Exported functions -------------------------------------------------------*/
/** @defgroup CACHEAXI_Exported_Functions CACHEAXI Exported Functions
  *  @brief    CACHEAXI Exported functions
  * @{
  */

/** @defgroup CACHEAXI_Exported_Functions_Group1 Initialization and De-Initialization Functions
  * @brief    Initialization and De-Initialization Functions
  * @{
  */
HAL_StatusTypeDef HAL_CACHEAXI_Init(CACHEAXI_HandleTypeDef *hcacheaxi);
HAL_StatusTypeDef HAL_CACHEAXI_DeInit(CACHEAXI_HandleTypeDef *hcacheaxi);
void              HAL_CACHEAXI_MspInit(CACHEAXI_HandleTypeDef *hcacheaxi);
void              HAL_CACHEAXI_MspDeInit(CACHEAXI_HandleTypeDef *hcacheaxi);
/**
  * @}
  */

/** @defgroup CACHEAXI_Exported_Functions_Group2 I/O Operation Functions
  * @brief    I/O Operation Functions
  * @{
  */
/* Peripheral Control functions ***/
HAL_StatusTypeDef HAL_CACHEAXI_Enable(CACHEAXI_HandleTypeDef *hcacheaxi);
HAL_StatusTypeDef HAL_CACHEAXI_Disable(CACHEAXI_HandleTypeDef *hcacheaxi);
uint32_t          HAL_CACHEAXI_IsEnabled(const CACHEAXI_HandleTypeDef *hcacheaxi);

/*** Cache maintenance in blocking mode (Polling) ***/
HAL_StatusTypeDef HAL_CACHEAXI_Invalidate(CACHEAXI_HandleTypeDef *hcacheaxi);
HAL_StatusTypeDef HAL_CACHEAXI_CleanByAddr(CACHEAXI_HandleTypeDef *hcacheaxi, const uint32_t *const pAddr,
                                           uint32_t dSize);
HAL_StatusTypeDef HAL_CACHEAXI_CleanInvalidByAddr(CACHEAXI_HandleTypeDef *hcacheaxi, const uint32_t *const pAddr,
                                                  uint32_t dSize);

/*** Cache maintenance in non-blocking mode (Interrupt) ***/
HAL_StatusTypeDef HAL_CACHEAXI_Invalidate_IT(CACHEAXI_HandleTypeDef *hcacheaxi);
HAL_StatusTypeDef HAL_CACHEAXI_CleanByAddr_IT(CACHEAXI_HandleTypeDef *hcacheaxi, const uint32_t *const pAddr,
                                              uint32_t dSize);
HAL_StatusTypeDef HAL_CACHEAXI_CleanInvalidByAddr_IT(CACHEAXI_HandleTypeDef *hcacheaxi, const uint32_t *const pAddr,
                                                     uint32_t dSize);

/*** IRQHandler and Callbacks ***/
void HAL_CACHEAXI_IRQHandler(CACHEAXI_HandleTypeDef *hcacheaxi);
void HAL_CACHEAXI_ErrorCallback(CACHEAXI_HandleTypeDef *hcacheaxi);
void HAL_CACHEAXI_CleanByAddrCallback(CACHEAXI_HandleTypeDef *hcacheaxi);
void HAL_CACHEAXI_InvalidateCompleteCallback(CACHEAXI_HandleTypeDef *hcacheaxi);
void HAL_CACHEAXI_CleanAndInvalidateByAddrCallback(CACHEAXI_HandleTypeDef *hcacheaxi);

#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
/* Callbacks Register/UnRegister functions ***/
HAL_StatusTypeDef HAL_CACHEAXI_RegisterCallback(CACHEAXI_HandleTypeDef *hcacheaxi,
                                                HAL_CACHEAXI_CallbackIDTypeDef CallbackID,
                                                pCACHEAXI_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_CACHEAXI_UnRegisterCallback(CACHEAXI_HandleTypeDef *hcacheaxi,
                                                  HAL_CACHEAXI_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */

/*** Performance instruction cache monitoring functions ***/
uint32_t HAL_CACHEAXI_Monitor_GetReadHitValue(const CACHEAXI_HandleTypeDef *hcacheaxi);
uint32_t HAL_CACHEAXI_Monitor_GetReadMissValue(const CACHEAXI_HandleTypeDef *hcacheaxi);
uint32_t HAL_CACHEAXI_Monitor_GetWriteHitValue(const CACHEAXI_HandleTypeDef *hcacheaxi);
uint32_t HAL_CACHEAXI_Monitor_GetWriteMissValue(const CACHEAXI_HandleTypeDef *hcacheaxi);
uint32_t HAL_CACHEAXI_Monitor_GetReadAllocMissValue(const CACHEAXI_HandleTypeDef *hcacheaxi);
uint32_t HAL_CACHEAXI_Monitor_GetWriteAllocMissValue(const CACHEAXI_HandleTypeDef *hcacheaxi);
uint32_t HAL_CACHEAXI_Monitor_GetWriteThroughValue(const CACHEAXI_HandleTypeDef *hcacheaxi);
uint32_t HAL_CACHEAXI_Monitor_GetEvictionValue(const CACHEAXI_HandleTypeDef *hcacheaxi);
HAL_StatusTypeDef HAL_CACHEAXI_Monitor_Reset(CACHEAXI_HandleTypeDef *hcacheaxi, uint32_t MonitorType);
HAL_StatusTypeDef HAL_CACHEAXI_Monitor_Start(CACHEAXI_HandleTypeDef *hcacheaxi, uint32_t MonitorType);
HAL_StatusTypeDef HAL_CACHEAXI_Monitor_Stop(CACHEAXI_HandleTypeDef *hcacheaxi, uint32_t MonitorType);
/**
  * @}
  */

/** @defgroup CACHEAXI_Exported_Functions_Group3 State and Error Functions
  * @brief    State and Error Functions
  * @{
  */
HAL_CACHEAXI_StateTypeDef HAL_CACHEAXI_GetState(const CACHEAXI_HandleTypeDef *hcacheaxi);
uint32_t HAL_CACHEAXI_GetError(const CACHEAXI_HandleTypeDef *hcacheaxi);
/**
  * @}
  */

/**
  * @}
  */


/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @}
  */

#endif /* CACHEAXI */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_CACHEAXI_H */
