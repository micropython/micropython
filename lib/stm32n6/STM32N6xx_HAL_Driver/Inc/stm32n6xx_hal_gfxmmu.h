/**
  ******************************************************************************
  * @file    stm32n6xx_hal_gfxmmu.h
  * @author  MCD Application Team
  * @brief   Header file of GFXMMU HAL module.
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
#ifndef STM32N6xx_HAL_GFXMMU_H
#define STM32N6xx_HAL_GFXMMU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

#if defined(GFXMMU)

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup GFXMMU
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup GFXMMU_Exported_Types GFXMMU Exported Types
  * @{
  */

/**
  * @brief  HAL GFXMMU states definition
  */
typedef enum
{
  HAL_GFXMMU_STATE_RESET = 0x00U, /*!< GFXMMU not initialized. */
  HAL_GFXMMU_STATE_READY = 0x01U, /*!< GFXMMU initialized and ready for use. */
} HAL_GFXMMU_StateTypeDef;

/**
  * @brief  GFXMMU buffers structure definition
  */
typedef struct
{
  uint32_t Buf0Address; /*!< Physical address of buffer 0. */
  uint32_t Buf1Address; /*!< Physical address of buffer 1. */
  uint32_t Buf2Address; /*!< Physical address of buffer 2. */
  uint32_t Buf3Address; /*!< Physical address of buffer 3. */
} GFXMMU_BuffersTypeDef;

/**
  * @brief  GFXMMU interrupts structure definition
  */
typedef struct
{
  FunctionalState Activation;     /*!< Interrupts enable/disable. */
  uint32_t        UsedInterrupts; /*!< Interrupts used.
                                       This parameter can be a values combination of @ref GFXMMU_Interrupts.
                                       @note: Useful only when interrupts are enabled. */
} GFXMMU_InterruptsTypeDef;

/**
  * @brief  GFXMMU init structure definition
  */
typedef struct
{
  uint32_t                    BlockSize;          /*!< Size of virtual memory block.
                                                       This parameter can be a value of @ref GFXMMU_BlockSize. */
  uint32_t                    DefaultValue;       /*!< Value returned when virtual memory location not physically mapped. */
                                                  /*   @note: Useful only when address translation is enabled. */
  FunctionalState             AddressTranslation; /*!< Address translation enable/disable. */
  GFXMMU_BuffersTypeDef       Buffers;            /*!< Physical buffers addresses. */
  GFXMMU_InterruptsTypeDef    Interrupts;         /*!< Interrupts parameters. */
} GFXMMU_InitTypeDef;

/**
  * @brief  GFXMMU handle structure definition
  */
#if (USE_HAL_GFXMMU_REGISTER_CALLBACKS == 1)
typedef struct __GFXMMU_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_GFXMMU_REGISTER_CALLBACKS */
{
  GFXMMU_TypeDef          *Instance; /*!< GFXMMU instance. */
  GFXMMU_InitTypeDef      Init;      /*!< GFXMMU init parameters. */
  HAL_GFXMMU_StateTypeDef State;     /*!< GFXMMU state. */
  __IO uint32_t           ErrorCode; /*!< GFXMMU error code. */
#if (USE_HAL_GFXMMU_REGISTER_CALLBACKS == 1)
  void (*ErrorCallback)     (struct __GFXMMU_HandleTypeDef *hgfxmmu); /*!< GFXMMU error callback. */
  void (*MspInitCallback)   (struct __GFXMMU_HandleTypeDef *hgfxmmu); /*!< GFXMMU MSP init callback. */
  void (*MspDeInitCallback) (struct __GFXMMU_HandleTypeDef *hgfxmmu); /*!< GFXMMU MSP de-init callback. */
#endif /* USE_HAL_GFXMMU_REGISTER_CALLBACKS */
} GFXMMU_HandleTypeDef;

/**
  * @brief  GFXMMU LUT line structure definition
  */
typedef struct
{
  uint32_t LineNumber;        /*!< LUT line number.
                                   This parameter must be a number between Min_Data = 0 and Max_Data = 1023. */
  uint32_t LineStatus;        /*!< LUT line enable/disable.
                                   This parameter can be a value of @ref GFXMMU_LutLineStatus. */
  uint32_t FirstVisibleBlock; /*!< First visible block on this line.
                                   This parameter must be a number between Min_Data = 0 and Max_Data = 255. */
  uint32_t LastVisibleBlock;  /*!< Last visible block on this line.
                                   This parameter must be a number between Min_Data = 0 and Max_Data = 255. */
  int32_t  LineOffset;        /*!< Offset of block 0 of the current line in physical buffer.
                                   This parameter must be a number between Min_Data = -255 and Max_Data = 261888.
                                   @note: Line offset has to be computed with the following formula:
                                          LineOffset = [(Blocks already used) - (1st visible block)]. */
} GFXMMU_LutLineTypeDef;

/**
  * @brief  GFXMMU packing structure definition
  */
typedef struct
{
  FunctionalState Buffer0Activation; /*!< Packing on buffer 0 enable/disable. */
  uint32_t        Buffer0Mode;       /*!< Buffer 0 packing mode.
                                          This parameter can be a value of @ref GFXMMU_PackingModes. */
  FunctionalState Buffer1Activation; /*!< Packing on buffer 1 enable/disable. */
  uint32_t        Buffer1Mode;       /*!< Buffer 1 packing mode.
                                          This parameter can be a value of @ref GFXMMU_PackingModes. */
  FunctionalState Buffer2Activation; /*!< Packing on buffer 2 enable/disable. */
  uint32_t        Buffer2Mode;       /*!< Buffer 2 packing mode.
                                          This parameter can be a value of @ref GFXMMU_PackingModes. */
  FunctionalState Buffer3Activation; /*!< Packing on buffer 3 enable/disable. */
  uint32_t        Buffer3Mode;       /*!< Buffer 3 packing mode.
                                          This parameter can be a value of @ref GFXMMU_PackingModes. */
  uint32_t        DefaultAlpha;      /*!< Default alpha value.
                                          This parameter must be a number between Min_Data = 0 and Max_Data = 255. */
} GFXMMU_PackingTypeDef;

#if (USE_HAL_GFXMMU_REGISTER_CALLBACKS == 1)
/**
  * @brief  GFXMMU callback ID enumeration definition
  */
typedef enum
{
  HAL_GFXMMU_ERROR_CB_ID     = 0x00U, /*!< GFXMMU error callback ID. */
  HAL_GFXMMU_MSPINIT_CB_ID   = 0x01U, /*!< GFXMMU MSP init callback ID. */
  HAL_GFXMMU_MSPDEINIT_CB_ID = 0x02U  /*!< GFXMMU MSP de-init callback ID. */
} HAL_GFXMMU_CallbackIDTypeDef;

/**
  * @brief  GFXMMU callback pointer definition
  */
typedef void (*pGFXMMU_CallbackTypeDef)(GFXMMU_HandleTypeDef *hgfxmmu);
#endif /* USE_HAL_GFXMMU_REGISTER_CALLBACKS */

/**
  * @}
  */
/* End of exported types -----------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup GFXMMU_Exported_Constants GFXMMU Exported Constants
  * @{
  */

/** @defgroup GFXMMU_BlockSize GFXMMU block size
  * @{
  */
#define GFXMMU_12BYTE_BLOCKS GFXMMU_CR_BS  /*!< Blocks of 12-byte. */
#define GFXMMU_16BYTE_BLOCKS 0x00000000U   /*!< Blocks of 16-byte. */
/**
  * @}
  */

/** @defgroup GFXMMU_Interrupts GFXMMU interrupts
  * @{
  */
#define GFXMMU_BUS_MASTER_ERROR_IT GFXMMU_CR_AMEIE /*!< Bus master error interrupt. */
#define GFXMMU_BUFFER0_OVERFLOW_IT GFXMMU_CR_B0OIE /*!< Buffer 0 overflow interrupt. */
#define GFXMMU_BUFFER1_OVERFLOW_IT GFXMMU_CR_B1OIE /*!< Buffer 1 overflow interrupt. */
#define GFXMMU_BUFFER2_OVERFLOW_IT GFXMMU_CR_B2OIE /*!< Buffer 2 overflow interrupt. */
#define GFXMMU_BUFFER3_OVERFLOW_IT GFXMMU_CR_B3OIE /*!< Buffer 3 overflow interrupt. */
/**
  * @}
  */

/** @defgroup GFXMMU_Error_Code GFXMMU Error Code
  * @{
  */
#define GFXMMU_ERROR_NONE             0x00000000U    /*!< No error. */
#define GFXMMU_ERROR_BUFFER0_OVERFLOW GFXMMU_SR_B0OF /*!< Buffer 0 overflow. */
#define GFXMMU_ERROR_BUFFER1_OVERFLOW GFXMMU_SR_B1OF /*!< Buffer 1 overflow. */
#define GFXMMU_ERROR_BUFFER2_OVERFLOW GFXMMU_SR_B2OF /*!< Buffer 2 overflow. */
#define GFXMMU_ERROR_BUFFER3_OVERFLOW GFXMMU_SR_B3OF /*!< Buffer 3 overflow. */
#define GFXMMU_ERROR_BUS_MASTER       GFXMMU_SR_AMEF /*!< Bus master error. */
#if (USE_HAL_GFXMMU_REGISTER_CALLBACKS == 1)
#define GFXMMU_ERROR_INVALID_CALLBACK 0x00000100U    /*!< Invalid callback error. */
#endif /* USE_HAL_GFXMMU_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup GFXMMU_LutLineStatus GFXMMU LUT line status
  * @{
  */
#define GFXMMU_LUT_LINE_DISABLE 0x00000000U     /*!< LUT line disabled. */
#define GFXMMU_LUT_LINE_ENABLE  GFXMMU_LUTxL_EN /*!< LUT line enabled. */
/**
  * @}
  */

/** @defgroup GFXMMU_PackingModes GFXMMU packing modes
  * @{
  */
#define GFXMMU_PACKING_MSB_REMOVE 0x00000000U /*!< Remove MSB during packing operation. */
#define GFXMMU_PACKING_LSB_REMOVE 0x00000001U /*!< Remove LSB during packing operation. */
/**
  * @}
  */

/**
  * @}
  */
/* End of exported constants -------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/** @defgroup GFXMMU_Exported_Macros GFXMMU Exported Macros
  * @{
  */

/** @brief  Reset GFXMMU handle state.
  * @param  __HANDLE__ GFXMMU handle.
  * @retval None
  */
#if (USE_HAL_GFXMMU_REGISTER_CALLBACKS == 1)
#define __HAL_GFXMMU_RESET_HANDLE_STATE(__HANDLE__) do{                                               \
                                                        (__HANDLE__)->State = HAL_GFXMMU_STATE_RESET; \
                                                        (__HANDLE__)->MspInitCallback = NULL;         \
                                                        (__HANDLE__)->MspDeInitCallback = NULL;       \
                                                      } while(0)
#else
#define __HAL_GFXMMU_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_GFXMMU_STATE_RESET)
#endif /* USE_HAL_GFXMMU_REGISTER_CALLBACKS */

/**
  * @}
  */
/* End of exported macros ----------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup GFXMMU_Exported_Functions GFXMMU Exported Functions
  * @{
  */

/** @addtogroup GFXMMU_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
/* Initialization and de-initialization functions *****************************/
HAL_StatusTypeDef HAL_GFXMMU_Init(GFXMMU_HandleTypeDef *hgfxmmu);
HAL_StatusTypeDef HAL_GFXMMU_DeInit(GFXMMU_HandleTypeDef *hgfxmmu);
void HAL_GFXMMU_MspInit(GFXMMU_HandleTypeDef *hgfxmmu);
void HAL_GFXMMU_MspDeInit(GFXMMU_HandleTypeDef *hgfxmmu);
#if (USE_HAL_GFXMMU_REGISTER_CALLBACKS == 1)
/* GFXMMU callbacks register/unregister functions *****************************/
HAL_StatusTypeDef HAL_GFXMMU_RegisterCallback(GFXMMU_HandleTypeDef        *hgfxmmu,
                                              HAL_GFXMMU_CallbackIDTypeDef CallbackID,
                                              pGFXMMU_CallbackTypeDef      pCallback);
HAL_StatusTypeDef HAL_GFXMMU_UnRegisterCallback(GFXMMU_HandleTypeDef        *hgfxmmu,
                                                HAL_GFXMMU_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_GFXMMU_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @addtogroup GFXMMU_Exported_Functions_Group2 Operations functions
  * @{
  */
/* Operation functions ********************************************************/
HAL_StatusTypeDef HAL_GFXMMU_ConfigLut(const GFXMMU_HandleTypeDef *hgfxmmu,
                                       uint32_t FirstLine,
                                       uint32_t LinesNumber,
                                       uint32_t Address);

HAL_StatusTypeDef HAL_GFXMMU_DisableLutLines(const GFXMMU_HandleTypeDef *hgfxmmu,
                                             uint32_t FirstLine,
                                             uint32_t LinesNumber);

HAL_StatusTypeDef HAL_GFXMMU_ConfigLutLine(const GFXMMU_HandleTypeDef *hgfxmmu, const GFXMMU_LutLineTypeDef *lutLine);

HAL_StatusTypeDef HAL_GFXMMU_ModifyBuffers(GFXMMU_HandleTypeDef *hgfxmmu, const GFXMMU_BuffersTypeDef *Buffers);

HAL_StatusTypeDef HAL_GFXMMU_ConfigPacking(GFXMMU_HandleTypeDef *hgfxmmu, const GFXMMU_PackingTypeDef *pPacking);

void HAL_GFXMMU_IRQHandler(GFXMMU_HandleTypeDef *hgfxmmu);

void HAL_GFXMMU_ErrorCallback(GFXMMU_HandleTypeDef *hgfxmmu);
/**
  * @}
  */

/** @defgroup GFXMMU_Exported_Functions_Group3 State functions
  * @{
  */
/* State function *************************************************************/
HAL_GFXMMU_StateTypeDef HAL_GFXMMU_GetState(const GFXMMU_HandleTypeDef *hgfxmmu);

uint32_t HAL_GFXMMU_GetError(GFXMMU_HandleTypeDef *hgfxmmu);
/**
  * @}
  */

/**
  * @}
  */
/* End of exported functions -------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
/** @defgroup GFXMMU_Private_Macros GFXMMU Private Macros
  * @{
  */
#define IS_GFXMMU_BLOCK_SIZE(VALUE) (((VALUE) == GFXMMU_12BYTE_BLOCKS) || \
                                     ((VALUE) == GFXMMU_16BYTE_BLOCKS))
#define IS_GFXMMU_LUT_LINE_OFFSET(VALUE) (((VALUE) >= -255) && ((VALUE) <= 261888))

#define IS_GFXMMU_BUFFER_ADDRESS(VALUE) (((VALUE) & 0xFU) == 0U)

#define IS_GFXMMU_INTERRUPTS(VALUE) (((VALUE) & 0x1FU) != 0U)

#define IS_GFXMMU_LUT_LINE(VALUE) ((VALUE) < 1024U)

#define IS_GFXMMU_LUT_LINES_NUMBER(VALUE) (((VALUE) > 0U) && ((VALUE) <= 1024U))

#define IS_GFXMMU_LUT_LINE_STATUS(VALUE) (((VALUE) == GFXMMU_LUT_LINE_DISABLE) || \
                                          ((VALUE) == GFXMMU_LUT_LINE_ENABLE))

#define IS_GFXMMU_LUT_BLOCK(VALUE) ((VALUE) < 256U)

#define IS_GFXMMU_PACKING_MODE(VALUE) (((VALUE) == GFXMMU_PACKING_MSB_REMOVE) || \
                                       ((VALUE) == GFXMMU_PACKING_LSB_REMOVE))

#define IS_GFXMMU_DEFAULT_ALPHA_VALUE(VALUE) ((VALUE) < 256U)
/**
  * @}
  */
/* End of private macros -----------------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */
#endif /* GFXMMU */
#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_GFXMMU_H */
