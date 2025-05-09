/**
  ******************************************************************************
  * @file    stm32n6xx_hal_sram.h
  * @author  MCD Application Team
  * @brief   Header file of SRAM HAL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_HAL_SRAM_H
#define STM32N6xx_HAL_SRAM_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_fmc.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
/** @addtogroup SRAM
  * @{
  */

/* Exported typedef ----------------------------------------------------------*/

/** @defgroup SRAM_Exported_Types SRAM Exported Types
  * @{
  */
/**
  * @brief  HAL SRAM State structures definition
  */
typedef enum
{
  HAL_SRAM_STATE_RESET     = 0x00U,  /*!< SRAM not yet initialized or disabled           */
  HAL_SRAM_STATE_READY     = 0x01U,  /*!< SRAM initialized and ready for use             */
  HAL_SRAM_STATE_BUSY      = 0x02U,  /*!< SRAM internal process is ongoing               */
  HAL_SRAM_STATE_ERROR     = 0x03U,  /*!< SRAM error state                               */
  HAL_SRAM_STATE_PROTECTED = 0x04U   /*!< SRAM peripheral NORSRAM device write protected */

} HAL_SRAM_StateTypeDef;

/**
  * @brief  SRAM handle Structure definition
  */
#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
typedef struct __SRAM_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS  */
{
  FMC_NORSRAM_TypeDef           *Instance;  /*!< Register base address                        */

  FMC_NORSRAM_EXTENDED_TypeDef  *Extended;  /*!< Extended mode register base address          */

  FMC_NORSRAM_InitTypeDef       Init;       /*!< SRAM device control configuration parameters */

  HAL_LockTypeDef               Lock;       /*!< SRAM locking object                          */

  __IO HAL_SRAM_StateTypeDef    State;      /*!< SRAM device access state                     */

  DMA_HandleTypeDef             *hdma;      /*!< Pointer DMA handler                          */

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
  void (* MspInitCallback)(struct __SRAM_HandleTypeDef *hsram);               /*!< SRAM Msp Init callback              */
  void (* MspDeInitCallback)(struct __SRAM_HandleTypeDef *hsram);             /*!< SRAM Msp DeInit callback            */
  void (* DmaXferCpltCallback)(DMA_HandleTypeDef *hdma);                      /*!< SRAM DMA Xfer Complete callback     */
  void (* DmaXferErrorCallback)(DMA_HandleTypeDef *hdma);                     /*!< SRAM DMA Xfer Error callback        */
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS  */
} SRAM_HandleTypeDef;

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL SRAM Callback ID enumeration definition
  */
typedef enum
{
  HAL_SRAM_MSP_INIT_CB_ID       = 0x00U,  /*!< SRAM MspInit Callback ID           */
  HAL_SRAM_MSP_DEINIT_CB_ID     = 0x01U,  /*!< SRAM MspDeInit Callback ID         */
  HAL_SRAM_DMA_XFER_CPLT_CB_ID  = 0x02U,  /*!< SRAM DMA Xfer Complete Callback ID */
  HAL_SRAM_DMA_XFER_ERR_CB_ID   = 0x03U   /*!< SRAM DMA Xfer Complete Callback ID */
} HAL_SRAM_CallbackIDTypeDef;

/**
  * @brief  HAL SRAM Callback pointer definition
  */
typedef void (*pSRAM_CallbackTypeDef)(SRAM_HandleTypeDef *hsram);
typedef void (*pSRAM_DmaCallbackTypeDef)(DMA_HandleTypeDef *hdma);
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS  */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/** @defgroup SRAM_Exported_Macros SRAM Exported Macros
  * @{
  */

/** @brief Reset SRAM handle state
  * @param  __HANDLE__ SRAM handle
  * @retval None
  */
#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
#define __HAL_SRAM_RESET_HANDLE_STATE(__HANDLE__)         do {                                             \
                                                               (__HANDLE__)->State = HAL_SRAM_STATE_RESET; \
                                                               (__HANDLE__)->MspInitCallback = NULL;       \
                                                               (__HANDLE__)->MspDeInitCallback = NULL;     \
                                                             } while(0)
#else
#define __HAL_SRAM_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_SRAM_STATE_RESET)
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SRAM_Exported_Functions SRAM Exported Functions
  * @{
  */

/** @addtogroup SRAM_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */

/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef HAL_SRAM_Init(SRAM_HandleTypeDef *hsram, FMC_NORSRAM_TimingTypeDef *Timing,
                                FMC_NORSRAM_TimingTypeDef *ExtTiming);
HAL_StatusTypeDef HAL_SRAM_DeInit(SRAM_HandleTypeDef *hsram);
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram);
void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef *hsram);

/**
  * @}
  */

/** @addtogroup SRAM_Exported_Functions_Group2 Input Output and memory control functions
  * @{
  */

/* I/O operation functions  ***************************************************/
HAL_StatusTypeDef HAL_SRAM_Read_8b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pDstBuffer,
                                   uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Write_8b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pSrcBuffer,
                                    uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Read_16b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint16_t *pDstBuffer,
                                    uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Write_16b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint16_t *pSrcBuffer,
                                     uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Read_32b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pDstBuffer,
                                    uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Write_32b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pSrcBuffer,
                                     uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Read_DMA(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pDstBuffer,
                                    uint32_t BufferSize);
HAL_StatusTypeDef HAL_SRAM_Write_DMA(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pSrcBuffer,
                                     uint32_t BufferSize);

void HAL_SRAM_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma);
void HAL_SRAM_DMA_XferErrorCallback(DMA_HandleTypeDef *hdma);

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
/* SRAM callback registering/unregistering */
HAL_StatusTypeDef HAL_SRAM_RegisterCallback(SRAM_HandleTypeDef *hsram, HAL_SRAM_CallbackIDTypeDef CallbackId,
                                            pSRAM_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_SRAM_UnRegisterCallback(SRAM_HandleTypeDef *hsram, HAL_SRAM_CallbackIDTypeDef CallbackId);
HAL_StatusTypeDef HAL_SRAM_RegisterDmaCallback(SRAM_HandleTypeDef *hsram, HAL_SRAM_CallbackIDTypeDef CallbackId,
                                               pSRAM_DmaCallbackTypeDef pCallback);
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS  */

/**
  * @}
  */

/** @addtogroup SRAM_Exported_Functions_Group3 Control functions
  * @{
  */

/* SRAM Control functions  ****************************************************/
HAL_StatusTypeDef HAL_SRAM_WriteOperation_Enable(SRAM_HandleTypeDef *hsram);
HAL_StatusTypeDef HAL_SRAM_WriteOperation_Disable(SRAM_HandleTypeDef *hsram);

/**
  * @}
  */

/** @addtogroup SRAM_Exported_Functions_Group4 Peripheral State functions
  * @{
  */

/* SRAM  State functions ******************************************************/
HAL_SRAM_StateTypeDef HAL_SRAM_GetState(const SRAM_HandleTypeDef *hsram);

/**
  * @}
  */

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

#endif /* STM32N6xx_HAL_SRAM_H */
