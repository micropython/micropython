/**
  ******************************************************************************
  * @file    stm32n6xx_hal_gpu2d.h
  * @author  MCD Application Team
  * @brief   Header file of GPU2D HAL module.
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
#ifndef STM32N6xx_HAL_GPU2D_H
#define STM32N6xx_HAL_GPU2D_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

#if defined (GPU2D)

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup GPU2D GPU2D
  * @brief GPU2D HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup GPU2D_Exported_Types GPU2D Exported Types
  * @{
  */
/**
  * @brief  HAL GPU2D State enumeration definition
  */
typedef enum
{
  HAL_GPU2D_STATE_RESET             = 0x00U,    /*!< GPU2D not yet initialized or disabled      */
  HAL_GPU2D_STATE_READY             = 0x01U,    /*!< Peripheral Initialized and ready for use   */
  HAL_GPU2D_STATE_BUSY              = 0x02U,    /*!< An internal process is ongoing             */
  HAL_GPU2D_STATE_TIMEOUT           = 0x03U,    /*!< Timeout state                              */
  HAL_GPU2D_STATE_ERROR             = 0x04U
} HAL_GPU2D_StateTypeDef;

/**
  * @brief  GPU2D_TypeDef definition
  */
typedef uint32_t GPU2D_TypeDef;

/**
  * @brief  GPU2D handle Structure definition
  */
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
typedef struct __GPU2D_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */
{
  GPU2D_TypeDef                Instance;                                                      /*!< GPU2D register base address.               */

  HAL_LockTypeDef              Lock;                                                          /*!< GPU2D lock.                                */

  __IO HAL_GPU2D_StateTypeDef  State;                                                         /*!< GPU2D transfer state.                      */

  __IO uint32_t                ErrorCode;                                                     /*!< GPU2D error code.                          */

#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
  void (* CommandListCpltCallback)(struct __GPU2D_HandleTypeDef *hgpu2d, uint32_t CmdListID); /*!< GPU2D Command Complete Callback            */

  void (* MspInitCallback)(struct __GPU2D_HandleTypeDef *hgpu2d);                             /*!< GPU2D Msp Init callback                    */

  void (* MspDeInitCallback)(struct __GPU2D_HandleTypeDef *hgpu2d);                           /*!< GPU2D Msp DeInit callback                  */
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */
} GPU2D_HandleTypeDef;

#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL GPU2D Callback ID enumeration definition
  */
typedef enum
{
  HAL_GPU2D_MSPINIT_CB_ID             = 0x00U,    /*!< GPU2D MspInit callback ID                 */
  HAL_GPU2D_MSPDEINIT_CB_ID           = 0x01U,    /*!< GPU2D MspDeInit callback ID               */
} HAL_GPU2D_CallbackIDTypeDef;

/** @defgroup HAL_GPU2D_Callback_pointer_definition HAL GPU2D Callback pointer definition
  * @brief  HAL GPU2D Callback pointer definition
  * @{
  */
typedef  void (*pGPU2D_CallbackTypeDef)(GPU2D_HandleTypeDef *hgpu2d);                                /*!< pointer to an GPU2D Callback function                       */
typedef  void (*pGPU2D_CommandListCpltCallbackTypeDef)(GPU2D_HandleTypeDef *hgpu2d, uint32_t CmdID); /*!< pointer to an GPU2D Command List Complete Callback function */
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup GPU2D_Exported_Constants GPU2D Exported Constants
  * @{
  */

/** @defgroup GPU2D_Error_Code_definition GPU2D Error Code definition
  * @brief  GPU2D Error Code definition
  * @{
  */
#define HAL_GPU2D_ERROR_NONE             (0x00000000U)  /*!< No error                */
#define HAL_GPU2D_ERROR_TIMEOUT          (0x00000001U)  /*!< Timeout error           */
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
#define HAL_GPU2D_ERROR_INVALID_CALLBACK (0x00000002U)  /*!< Invalid callback error  */
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */
/**
  * @}
  */

/** @defgroup GPU2D_Interrupt_configuration_definition GPU2D Interrupt configuration definition
  * @brief GPU2D Interrupt definition
  * @{
  */
#define GPU2D_IT_CLC                     0x00000001U              /*!< Command List Complete Interrupt */
/**
  * @}
  */

/** @defgroup GPU2D_Flag_definition GPU2D Flag definition
  * @brief GPU2D Flags definition
  * @{
  */
#define GPU2D_FLAG_CLC                   0x00000001U              /*!< Command List Complete Interrupt Flag  */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros ------------------------------------------------------------*/
/** @defgroup GPU2D_Exported_Macros GPU2D Exported Macros
  * @{
  */

/** @brief Reset GPU2D handle state
  * @param  __HANDLE__: specifies the GPU2D handle.
  * @retval None
  */
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
#define __HAL_GPU2D_RESET_HANDLE_STATE(__HANDLE__) do {                                               \
                                                        (__HANDLE__)->State = HAL_GPU2D_STATE_RESET;  \
                                                        (__HANDLE__)->MspInitCallback = NULL;         \
                                                        (__HANDLE__)->MspDeInitCallback = NULL;       \
                                                        (__HANDLE__)->CommandListCpltCallback = NULL; \
                                                      } while(0)
#else /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 0 */
#define __HAL_GPU2D_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_GPU2D_STATE_RESET)
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */

/* Interrupt & Flag management */
/**
  * @brief  Get the GPU2D pending flags.
  * @param  __HANDLE__: GPU2D handle
  * @param  __FLAG__: flag to check.
  *          This parameter can be one of the following values:
  *            @arg GPU2D_FLAG_CLC:   Command List Complete interrupt mask
  * @retval The state of FLAG.
  */
#define __HAL_GPU2D_GET_FLAG(__HANDLE__, __FLAG__) (READ_REG(*(__IO uint32_t *)((uint32_t)(__HANDLE__)->Instance\
                                                                                + GPU2D_ITCTRL)) & (__FLAG__))

/**
  * @brief  Clear the GPU2D pending flags.
  * @param  __HANDLE__: GPU2D handle
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be one of the following values:
  *            @arg GPU2D_FLAG_CLC:   Command List Complete interrupt mask
  * @retval None
  */
#define __HAL_GPU2D_CLEAR_FLAG(__HANDLE__, __FLAG__) do { \
                                                          __IO uint32_t *tmpreg = \
                                                            (__IO uint32_t *)((uint32_t)(__HANDLE__)->Instance\
                                                                              + GPU2D_ITCTRL); \
                                                          CLEAR_BIT(*tmpreg, __FLAG__); \
                                                        } while(0U)

/**
  * @brief  Check whether the specified GPU2D interrupt source is enabled or not.
  * @param  __HANDLE__: GPU2D handle
  * @param  __INTERRUPT__: specifies the GPU2D interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg GPU2D_IT_CLC:   Command List Complete interrupt mask
  * @retval The state of INTERRUPT source.
  */
#define __HAL_GPU2D_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) (READ_REG(*(__IO uint32_t *)\
                                                                       ((uint32_t)(__HANDLE__)->Instance\
                                                                        + GPU2D_ITCTRL)) & (__INTERRUPT__))

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup GPU2D_Exported_Functions GPU2D Exported Functions
  * @{
  */

/** @addtogroup GPU2D_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */

/* Initialization and de-initialization functions *******************************/
HAL_StatusTypeDef HAL_GPU2D_Init(GPU2D_HandleTypeDef *hgpu2d);
HAL_StatusTypeDef HAL_GPU2D_DeInit(GPU2D_HandleTypeDef *hgpu2d);
void              HAL_GPU2D_MspInit(GPU2D_HandleTypeDef *hgpu2d);
void              HAL_GPU2D_MspDeInit(GPU2D_HandleTypeDef *hgpu2d);
/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_GPU2D_RegisterCallback(GPU2D_HandleTypeDef *hgpu2d, HAL_GPU2D_CallbackIDTypeDef CallbackID,
                                             pGPU2D_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_GPU2D_UnRegisterCallback(GPU2D_HandleTypeDef *hgpu2d, HAL_GPU2D_CallbackIDTypeDef CallbackID);
HAL_StatusTypeDef HAL_GPU2D_RegisterCommandListCpltCallback(GPU2D_HandleTypeDef *hgpu2d,
                                                            pGPU2D_CommandListCpltCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_GPU2D_UnRegisterCommandListCpltCallback(GPU2D_HandleTypeDef *hgpu2d);
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */

/**
  * @}
  */


/** @addtogroup GPU2D_Exported_Functions_Group2 IO operation functions
  * @{
  */

/* IO operation functions *******************************************************/
uint32_t          HAL_GPU2D_ReadRegister(const GPU2D_HandleTypeDef *hgpu2d, uint32_t offset);
HAL_StatusTypeDef HAL_GPU2D_WriteRegister(GPU2D_HandleTypeDef *hgpu2d, uint32_t offset, uint32_t value);
void              HAL_GPU2D_IRQHandler(GPU2D_HandleTypeDef *hgpu2d);
void              HAL_GPU2D_ER_IRQHandler(GPU2D_HandleTypeDef *hgpu2d);
void              HAL_GPU2D_CommandListCpltCallback(GPU2D_HandleTypeDef *hgpu2d, uint32_t CmdListID);
void              HAL_GPU2D_ErrorCallback(GPU2D_HandleTypeDef *hgpu2d);

/**
  * @}
  */

/** @addtogroup GPU2D_Exported_Functions_Group3 Peripheral State and Error functions
  * @{
  */

/* Peripheral State functions ***************************************************/
HAL_GPU2D_StateTypeDef HAL_GPU2D_GetState(GPU2D_HandleTypeDef const *const hgpu2d);
uint32_t               HAL_GPU2D_GetError(GPU2D_HandleTypeDef const *const hgpu2d);

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/

/** @addtogroup GPU2D_Private_Constants GPU2D Private Constants
  * @{
  */

#define GPU2D_ITCTRL                    (0x0F8U)   /*!< GPU2D Interrupt Control Register Offset            */
#define GPU2D_CLID                      (0x148U)   /*!< GPU2D Last Command List Identifier Register Offset */
#define GPU2D_BREAKPOINT                (0x080U)   /*!< GPU2D Breakpoint Register Offset                   */
#define GPU2D_SYS_INTERRUPT             (0xff8U)   /*!< GPU2D System Interrupt Register Offset             */

/** @defgroup GPU2D_Offset GPU2D Last Register Offset
  * @{
  */
#define GPU2D_OFFSET                     0x1000U                  /*!< Last GPU2D Register Offset */
/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup GPU2D_Private_Macros GPU2D Private Macros
  * @{
  */
#define IS_GPU2D_OFFSET(OFFSET)          ((OFFSET) < GPU2D_OFFSET)

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

#endif /* defined (GPU2D) */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_GPU2D_H */

