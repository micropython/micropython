/**
  **********************************************************************************************************************
  * @file    stm32n6xx_hal_dts.h
  * @author  MCD Application Team
  * @brief   Header file of DTS HAL module.
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef STM32N6xx_HAL_DTS_H
#define STM32N6xx_HAL_DTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup DTS
  * @{
  */

/* Exported types ----------------------------------------------------------------------------------------------------*/

/** @defgroup DTS_Exported_Types  DTS Exported Types
  * @{
  */
/**
  * @brief  HAL DTS states definition
  */
typedef enum
{
  HAL_DTS_STATE_RESET        = 0x00U, /*!< DTS not initialized */
  HAL_DTS_STATE_READY        = 0x01U, /*!< DTS initialized and ready for use */
  HAL_DTS_STATE_RUNNING_0    = 0x02U, /*!< DTS temperature measure in progress on sensor 0 */
  HAL_DTS_STATE_RUNNING_1    = 0x03U, /*!< DTS temperature measure in progress on sensor 1 */
  HAL_DTS_STATE_RUNNING_BOTH = 0x04U  /*!< DTS temperature measure in progress on both sensors */
} HAL_DTS_StateTypeDef;

/**
  * @brief  DTS sensors definition
  */
typedef enum
{
  DTS_SENSOR_0 = 0x00U, /*!< DTS sensor 0 */
  DTS_SENSOR_1 = 0x01U, /*!< DTS sensor 1 */
} HAL_DTS_Sensor;

/**
  * @brief  DTS handle structure definition
  */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
typedef struct __DTS_HandleTypeDef
#else /* USE_HAL_DTS_REGISTER_CALLBACKS */
typedef struct
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
{
  DTS_TypeDef               *Instance;        /*!< DTS instance */
  __IO HAL_DTS_StateTypeDef  State;           /*!< DTS state */
  __IO uint32_t              ErrorCode;       /*!< DTS error code */
  uint32_t                   SensorMode[2U];  /*!< DTS sensor modes */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
  void (*MspInitCallback)(struct __DTS_HandleTypeDef *hdts);     /*!< DTS MSP init callback */
  void (*MspDeInitCallback)(struct __DTS_HandleTypeDef *hdts);   /*!< DTS MSP de-init callback */
  void (*TemperatureCallback)(struct __DTS_HandleTypeDef *hdts,
                              HAL_DTS_Sensor Sensor);            /*!< DTS temperature callback */
  void (*AlarmACallback)(struct __DTS_HandleTypeDef *hdts,
                         HAL_DTS_Sensor Sensor);                 /*!< DTS alarm A callback */
  void (*AlarmBCallback)(struct __DTS_HandleTypeDef *hdts,
                         HAL_DTS_Sensor Sensor);                 /*!< DTS alarm B callback */
  void (*ErrorCallback)(struct __DTS_HandleTypeDef *hdts,
                        HAL_DTS_Sensor Sensor);                  /*!< DTS error callback */
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
} DTS_HandleTypeDef;

#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
/**
  * @brief  DTS callback ID enumeration definition
  */
typedef enum
{
  HAL_DTS_MSPINIT_CB_ID   = 0x00U, /*!< DTS MSP init callback ID */
  HAL_DTS_MSPDEINIT_CB_ID = 0x01U  /*!< DTS MSP de-init callback ID */
} HAL_DTS_CallbackIDTypeDef;

/**
  * @brief  DTS sensor callback ID enumeration definition
  */
typedef enum
{
  HAL_DTS_SENSOR_TEMPERATURE_CB_ID = 0x00U, /*!< DTS sensor temperature callback ID */
  HAL_DTS_SENSOR_ALARMA_CB_ID      = 0x01U, /*!< DTS sensor alarm A callback ID */
  HAL_DTS_SENSOR_ALARMB_CB_ID      = 0x02U, /*!< DTS sensor alarm B callback ID */
  HAL_DTS_SENSOR_ERROR_CB_ID       = 0x03U, /*!< DTS sensor error callback ID */
} HAL_DTS_SensorCallbackIDTypeDef;

/**
  * @brief  DTS callback pointers definition
  */
typedef void (*pDTS_CallbackTypeDef)(DTS_HandleTypeDef *hdts);
typedef void (*pDTS_SensorCallbackTypeDef)(DTS_HandleTypeDef *hdts, HAL_DTS_Sensor Sensor);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */

/**
  * @brief  DTS sensor configuration structure definition
  */
typedef struct
{
  uint32_t Mode;       /*!< Sensor mode.
                            This parameter can be a value of @ref DTS_SensorMode */
  uint32_t Resolution; /*!< Sensor resolution.
                            This parameter can be a value of @ref DTS_SensorResolution */
  uint32_t Trigger;    /*!< Sensor trigger.
                            This parameter can be a value of @ref DTS_SensorTrigger.
                            @note This parameter is used only if Mode is set to DTS_SENSOR_MODE_TRIGGER */
} DTS_SensorConfigTypeDef;

/**
  * @brief  DTS alarm configuration structure definition
  */
typedef struct
{
  FunctionalState Enable;     /*!< Alarm enable/disable */
  float_t         Threshold;  /*!< Alarm threshold in celsius degree.
                                   This parameter must be a number between Min_Data = -40 and Max_Data = 125 */
  float_t         Hysteresis; /*!< Alarm hysteresis in celsius degree.
                                   This parameter must be a number between Min_Data = -40 and Max_Data = 125 */
} DTS_AlarmConfigTypeDef;
/**
  * @}
  */

/* Exported constants ------------------------------------------------------------------------------------------------*/

/** @defgroup DTS_Exported_Constants  DTS Exported Constants
  * @{
  */

/** @defgroup DTS_ErrorCode DTS error code
  * @{
  */
#define HAL_DTS_ERROR_NONE                 0x00000000U  /*!< No error */
#define HAL_DTS_ERROR_INVALID_PARAM        0x00000001U  /*!< Invalid parameter */
#define HAL_DTS_ERROR_INVALID_STATE        0x00000002U  /*!< Invalid state */
#define HAL_DTS_ERROR_INVALID_SENSOR_MODE  0x00000004U  /*!< Invalid sensor mode */
#define HAL_DTS_ERROR_INVALID_SAMPLE       0x00000008U  /*!< Invalid sample */
#define HAL_DTS_ERROR_FAULT                0x00000010U  /*!< Fault */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
#define HAL_DTS_ERROR_INVALID_CALLBACK     0x00000020U  /*!< Invalid callback error */
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup DTS_SensorMode DTS sensor mode
  * @{
  */
#define DTS_SENSOR_MODE_DISABLE     0x00000000U /*!< No acquisition */
#define DTS_SENSOR_MODE_SINGLE      0x00000106U /*!< Single acquisition */
#define DTS_SENSOR_MODE_CONTINUOUS  0x0000010AU /*!< Continuous acquisitions */
#define DTS_SENSOR_MODE_TRIGGER     0x00000102U /*!< Acquisition with trigger */
/**
  * @}
  */

/** @defgroup DTS_SensorResolution DTS sensor resolution
  * @{
  */
#define DTS_SENSOR_RESOLUTION_12BITS  0x00000001U /*!< Sensor resolution of 12 bits */
#define DTS_SENSOR_RESOLUTION_10BITS  0x00000021U /*!< Sensor resolution of 10 bits */
#define DTS_SENSOR_RESOLUTION_8BITS   0x00000041U /*!< Sensor resolution of 8 bits */
/**
  * @}
  */

/** @defgroup DTS_SensorTrigger DTS sensor trigger
  * @{
  */
#define DTS_SENSOR_TRIGGER_LPTIM4_OUT  0x00010000U /*!< LPTIM4 output signal */
#define DTS_SENSOR_TRIGGER_LPTIM2_CH1  0x00020000U /*!< LPTIM2 channel 1 signal */
#define DTS_SENSOR_TRIGGER_LPTIM3_CH1  0x00030000U /*!< LPTIM3 channel 1 signal */
#define DTS_SENSOR_TRIGGER_EXTI13      0x00040000U /*!< EXTI13 signal */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros ---------------------------------------------------------------------------------------------------*/

/** @defgroup DTS_Exported_Macros  DTS Exported Macros
  * @{
  */
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
#define __HAL_DTS_RESET_HANDLE_STATE(__HANDLE__) do{                                             \
                                                      (__HANDLE__)->State = HAL_DTS_STATE_RESET; \
                                                      (__HANDLE__)->MspInitCallback = NULL;      \
                                                      (__HANDLE__)->MspDeInitCallback = NULL;    \
                                                    } while(0)
#else /* USE_HAL_DTS_REGISTER_CALLBACKS */
#define __HAL_DTS_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_DTS_STATE_RESET)
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */

#define IS_DTS_SENSOR_MODE(PARAM) (((PARAM) == DTS_SENSOR_MODE_DISABLE)    || \
                                   ((PARAM) == DTS_SENSOR_MODE_SINGLE)     || \
                                   ((PARAM) == DTS_SENSOR_MODE_CONTINUOUS) || \
                                   ((PARAM) == DTS_SENSOR_MODE_TRIGGER))

#define IS_DTS_SENSOR_RESOLUTION(PARAM) (((PARAM) == DTS_SENSOR_RESOLUTION_12BITS) || \
                                         ((PARAM) == DTS_SENSOR_RESOLUTION_10BITS) || \
                                         ((PARAM) == DTS_SENSOR_RESOLUTION_8BITS))

#define IS_DTS_SENSOR_TRIGGER(PARAM) (((PARAM) == DTS_SENSOR_TRIGGER_LPTIM4_OUT) || \
                                      ((PARAM) == DTS_SENSOR_TRIGGER_LPTIM2_CH1) || \
                                      ((PARAM) == DTS_SENSOR_TRIGGER_LPTIM3_CH1) || \
                                      ((PARAM) == DTS_SENSOR_TRIGGER_EXTI13))

#define IS_DTS_ALARM_PARAM(PARAM) ((-40.0f <= (PARAM)) && ((PARAM) <= 125.0f))
/**
  * @}
  */

/* Exported functions ------------------------------------------------------------------------------------------------*/

/** @addtogroup DTS_Exported_Functions
  * @{
  */

/* Initialization and de-initialization functions */
/** @addtogroup DTS_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef HAL_DTS_Init(DTS_HandleTypeDef *hdts);
HAL_StatusTypeDef HAL_DTS_DeInit(DTS_HandleTypeDef *hdts);
void              HAL_DTS_MspInit(DTS_HandleTypeDef *hdts);
void              HAL_DTS_MspDeInit(DTS_HandleTypeDef *hdts);
#if (USE_HAL_DTS_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_DTS_RegisterCallback(DTS_HandleTypeDef        *hdts,
                                           HAL_DTS_CallbackIDTypeDef CallbackID,
                                           pDTS_CallbackTypeDef      pCallback);
HAL_StatusTypeDef HAL_DTS_UnRegisterCallback(DTS_HandleTypeDef        *hdts,
                                             HAL_DTS_CallbackIDTypeDef CallbackID);
HAL_StatusTypeDef HAL_DTS_RegisterSensorCallback(DTS_HandleTypeDef              *hdts,
                                                 HAL_DTS_SensorCallbackIDTypeDef CallbackID,
                                                 pDTS_SensorCallbackTypeDef      pCallback);
HAL_StatusTypeDef HAL_DTS_UnRegisterSensorCallback(DTS_HandleTypeDef              *hdts,
                                                   HAL_DTS_SensorCallbackIDTypeDef CallbackID);
#endif /* USE_HAL_DTS_REGISTER_CALLBACKS */
/**
  * @}
  */

/* Temperature measurement functions */
/** @addtogroup DTS_Exported_Functions_Group2
  * @{
  */
HAL_StatusTypeDef HAL_DTS_ConfigSensor(DTS_HandleTypeDef       *hdts,
                                       HAL_DTS_Sensor           Sensor,
                                       DTS_SensorConfigTypeDef *pSensorParams);
HAL_StatusTypeDef HAL_DTS_Start(DTS_HandleTypeDef *hdts,
                                HAL_DTS_Sensor     Sensor);
HAL_StatusTypeDef HAL_DTS_PollForTemperature(DTS_HandleTypeDef *hdts,
                                             HAL_DTS_Sensor     Sensor,
                                             uint32_t              Timeout);
HAL_StatusTypeDef HAL_DTS_GetTemperature(DTS_HandleTypeDef *hdts,
                                         HAL_DTS_Sensor     Sensor,
                                         float_t           *pTemperature);
HAL_StatusTypeDef HAL_DTS_Stop(DTS_HandleTypeDef *hdts,
                               HAL_DTS_Sensor     Sensor);
HAL_StatusTypeDef HAL_DTS_Start_IT(DTS_HandleTypeDef *hdts,
                                   HAL_DTS_Sensor     Sensor);
HAL_StatusTypeDef HAL_DTS_Stop_IT(DTS_HandleTypeDef *hdts,
                                  HAL_DTS_Sensor     Sensor);
void              HAL_DTS_TemperatureCallback(DTS_HandleTypeDef *hdts,
                                              HAL_DTS_Sensor     Sensor);
HAL_StatusTypeDef HAL_DTS_GetExtremeTemperatures(DTS_HandleTypeDef *hdts,
                                                 HAL_DTS_Sensor     Sensor,
                                                 float_t           *pMin,
                                                 float_t           *pMax);
HAL_StatusTypeDef HAL_DTS_ConfigSampleCounter(DTS_HandleTypeDef *hdts,
                                              FunctionalState    Enable,
                                              FunctionalState    Clear,
                                              FunctionalState    Hold);
HAL_StatusTypeDef HAL_DTS_GetSampleCounterValue(DTS_HandleTypeDef *hdts,
                                                uint32_t          *pNumber);
HAL_StatusTypeDef HAL_DTS_ConfigSampleDiscard(DTS_HandleTypeDef *hdts,
                                              FunctionalState    Status);
/**
  * @}
  */

/* Alarms functions */
/** @addtogroup DTS_Exported_Functions_Group3
  * @{
  */
HAL_StatusTypeDef HAL_DTS_ConfigAlarmA(DTS_HandleTypeDef            *hdts,
                                       HAL_DTS_Sensor                Sensor,
                                       const DTS_AlarmConfigTypeDef *pAlarmParams);
HAL_StatusTypeDef HAL_DTS_ConfigAlarmB(DTS_HandleTypeDef            *hdts,
                                       HAL_DTS_Sensor                Sensor,
                                       const DTS_AlarmConfigTypeDef *pAlarmParams);
void              HAL_DTS_AlarmACallback(DTS_HandleTypeDef *hdts,
                                         HAL_DTS_Sensor     Sensor);
void              HAL_DTS_AlarmBCallback(DTS_HandleTypeDef *hdts,
                                         HAL_DTS_Sensor     Sensor);
/**
  * @}
  */

/* Generic functions */
/** @addtogroup DTS_Exported_Functions_Group4
  * @{
  */
void                 HAL_DTS_IRQHandler(DTS_HandleTypeDef *hdts);
void                 HAL_DTS_ErrorCallback(DTS_HandleTypeDef *hdts,
                                           HAL_DTS_Sensor     Sensor);
HAL_DTS_StateTypeDef HAL_DTS_GetState(const DTS_HandleTypeDef *hdts);
uint32_t             HAL_DTS_GetError(const DTS_HandleTypeDef *hdts);
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

#endif /* STM32N6xx_HAL_DTS_H */
