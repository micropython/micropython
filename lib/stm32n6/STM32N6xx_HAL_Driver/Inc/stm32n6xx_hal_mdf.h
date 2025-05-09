/**
  ******************************************************************************
  * @file    stm32n6xx_hal_mdf.h
  * @author  MCD Application Team
  * @brief   Header file of MDF HAL module.
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
#ifndef STM32N6xx_HAL_MDF_H
#define STM32N6xx_HAL_MDF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup MDF
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup MDF_Exported_Types  MDF Exported Types
  * @{
  */

/**
  * @brief  HAL MDF states definition
  */
typedef enum
{
  HAL_MDF_STATE_RESET       = 0x00U, /*!< MDF not initialized */
  HAL_MDF_STATE_READY       = 0x01U, /*!< MDF initialized and ready for use */
  HAL_MDF_STATE_ACQUISITION = 0x02U, /*!< MDF acquisition in progress */
  HAL_MDF_STATE_ERROR       = 0xFFU  /*!< MDF state error */
} HAL_MDF_StateTypeDef;

/**
  * @brief  MDF clock trigger structure definition
  */
typedef struct
{
  FunctionalState Activation;  /*!< Output clock trigger enable/disable */
  uint32_t        Source;      /*!< Output clock trigger source.
                                    This parameter can be a value of @ref MDF_ClockTriggerSource */
  uint32_t        Edge;        /*!< Output clock trigger edge.
                                    This parameter can be a value of @ref MDF_ClockTriggerEdge */
} MDF_ClockTriggerTypeDef;

/**
  * @brief  MDF output clock structure definition
  */
typedef struct
{
  FunctionalState         Activation; /*!< Output clock enable/disable */
  uint32_t                Pins;       /*!< Output clock pins.
                                           This parameter can be a value of @ref MDF_OuputClockPins */
  uint32_t                Divider;    /*!< Output clock divider.
                                           This parameter must be a number between Min_Data = 1 and Max_Data = 16 */
  MDF_ClockTriggerTypeDef Trigger;    /*!< Output clock trigger parameters */
} MDF_OutputClockTypeDef;

/**
  * @brief  MDF common parameters structure definition
  */
typedef struct
{
  uint32_t               InterleavedFilters; /*!< Number of filters in interleaved mode with filter 0.
                                                  This parameter must be a number between Min_Data = 0
                                                  and Max_Data = 5.
                                                  @note This parameter is not used for ADF instance */
  uint32_t               ProcClockDivider;   /*!< Processing clock divider.
                                                  This parameter must be a number between Min_Data = 1
                                                  and Max_Data = 128 */
  MDF_OutputClockTypeDef OutputClock;        /*!< Output clock parameters */
} MDF_CommonParamTypeDef;

/**
  * @brief  MDF serial interface structure definition
  */
typedef struct
{
  FunctionalState  Activation;  /*!< Serial interface enable/disable */
  uint32_t         Mode;        /*!< Serial interface mode.
                                     This parameter can be a value of @ref MDF_SitfMode */
  uint32_t         ClockSource; /*!< Serial interface clock source.
                                     This parameter can be a value of @ref MDF_SitfClockSource */
  uint32_t         Threshold;   /*!< SPI threshold for clock absence detection or Manchester symbol threshold.
                                     This parameter must be a number between Min_Data = 4 and Max_Data = 31 */
} MDF_SerialInterfaceTypeDef;

/**
  * @brief  MDF init structure definition
  */
typedef struct
{
  MDF_CommonParamTypeDef      CommonParam;      /*!< MDF common parameters */
  MDF_SerialInterfaceTypeDef  SerialInterface;  /*!< MDF serial interface parameters */
  uint32_t                    FilterBistream;   /*!< MDF filter bitstream selection.
                                                     This parameter can be a value of @ref MDF_FilterBitstream */
} MDF_InitTypeDef;

/**
  * @brief  MDF handle structure definition
  */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
typedef struct __MDF_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
{
  MDF_Filter_TypeDef        *Instance;  /*!< MDF instance */
  MDF_InitTypeDef            Init;      /*!< MDF init parameters */
  DMA_HandleTypeDef         *hdma;      /*!< Pointer on DMA handler for acquisitions */
  __IO HAL_MDF_StateTypeDef  State;     /*!< MDF state */
  __IO uint32_t              ErrorCode; /*!< MDF error code */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
  void (*OldCallback)(struct __MDF_HandleTypeDef *hmdf,
                      uint32_t Threshold);                        /*!< MDF out-off limit detector callback.
                                                                       @note Not used for ADF instance */
  void (*AcqCpltCallback)(struct __MDF_HandleTypeDef *hmdf);      /*!< MDF acquisition complete callback */
  void (*AcqHalfCpltCallback)(struct __MDF_HandleTypeDef *hmdf);  /*!< MDF acquisition half complete callback */
  void (*SndLvCallback)(struct __MDF_HandleTypeDef *hmdf,
                        uint32_t SoundLevel,
                        uint32_t AmbientNoise);                   /*!< MDF sound level callback.
                                                                       @note Not used for MDF instance */
  void (*SadCallback)(struct __MDF_HandleTypeDef *hmdf);          /*!< MDF sound activity detector callback.
                                                                       @note Not used for MDF instance */
  void (*ErrorCallback)(struct __MDF_HandleTypeDef *hmdf);        /*!< MDF error callback */
  void (*MspInitCallback)(struct __MDF_HandleTypeDef *hmdf);      /*!< MDF MSP init callback */
  void (*MspDeInitCallback)(struct __MDF_HandleTypeDef *hmdf);    /*!< MDF MSP de-init callback */
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
} MDF_HandleTypeDef;

#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
/**
  * @brief  MDF callback ID enumeration definition
  */
typedef enum
{
  HAL_MDF_OLD_CB_ID              = 0x00U, /*!< MDF out-off limit detector callback ID.
                                               @note Not used for ADF instance */
  HAL_MDF_ACQ_COMPLETE_CB_ID     = 0x01U, /*!< MDF acquisition complete callback ID */
  HAL_MDF_ACQ_HALFCOMPLETE_CB_ID = 0x02U, /*!< MDF acquisition half complete callback ID */
  HAL_MDF_SNDLVL_CB_ID           = 0x03U, /*!< MDF sound level callback ID.
                                               @note Not used for MDF instance */
  HAL_MDF_SAD_CB_ID              = 0x04U, /*!< MDF sound activity detector callback ID.
                                               @note Not used for MDF instance */
  HAL_MDF_ERROR_CB_ID            = 0x05U, /*!< MDF error callback ID */
  HAL_MDF_MSPINIT_CB_ID          = 0x06U, /*!< MDF MSP init callback ID */
  HAL_MDF_MSPDEINIT_CB_ID        = 0x07U  /*!< MDF MSP de-init callback ID */
} HAL_MDF_CallbackIDTypeDef;

/**
  * @brief  MDF callback pointers definition
  */
typedef void (*pMDF_CallbackTypeDef)(MDF_HandleTypeDef *hmdf);
typedef void (*pMDF_OldCallbackTypeDef)(MDF_HandleTypeDef *hmdf, uint32_t Threshold);
typedef void (*pMDF_SndLvlCallbackTypeDef)(MDF_HandleTypeDef *hmdf, uint32_t SoundLevel, uint32_t AmbientNoise);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */

/**
  * @brief  MDF reshape filter structure definition
  */
typedef struct
{
  FunctionalState  Activation;       /*!< Reshape filter enable/disable */
  uint32_t         DecimationRatio;  /*!< Reshape filter decimation ratio.
                                          This parameter can be a value of @ref MDF_ReshapeDecimationRatio */
} MDF_ReshapeFilterTypeDef;

/**
  * @brief  MDF high pass filter structure definition
  */
typedef struct
{
  FunctionalState  Activation;       /*!< High pass filter enable/disable */
  uint32_t         CutOffFrequency;  /*!< High pass filter cut-off frequency.
                                          This parameter can be a value of @ref MDF_HighPassCutOffFreq */
} MDF_HighPassFilterTypeDef;

/**
  * @brief  MDF integrator structure definition
  */
typedef struct
{
  FunctionalState  Activation;      /*!< Integrator enable/disable */
  uint32_t         Value;           /*!< Integrator value.
                                         This parameter must be a number between Min_Data = 2 and Max_Data = 128 */
  uint32_t         OutputDivision;  /*!< Integrator output division.
                                         This parameter can be a value of @ref MDF_IntegratorOutputDiv */
} MDF_IntegratorTypeDef;

/**
  * @brief  MDF sound activity structure definition
  */
typedef struct
{
  FunctionalState  Activation;            /*!< Sound activity detector enable/disable */
  uint32_t         Mode;                  /*!< Sound activity detector mode.
                                               This parameter can be a value of @ref MDF_SadMode */
  uint32_t         FrameSize;             /*!< Size of one frame to compute short-term signal level.
                                               This parameter can be a value of @ref MDF_SadFrameSize */
  FunctionalState  Hysteresis;            /*!< Hysteresis enable/disable.
                                               @note This parameter is not used if Mode is set
                                                     to MDF_SAD_AMBIENT_NOISE_ESTIMATOR */
  uint32_t         SoundTriggerEvent;     /*!< Sound trigger event configuration.
                                               This parameter can be a value of @ref MDF_SadSoundTriggerEvent */
  uint32_t         DataMemoryTransfer;    /*!< Data memory transfer mode.
                                               This parameter can be a value of @ref MDF_SadDataMemoryTransfer */
  uint32_t         MinNoiseLevel;         /*!< Minimum noise level.
                                               This parameter must be a number between Min_Data = 0
                                               and Max_Data = 8191 */
  uint32_t         HangoverWindow;        /*!< Hangover time window in frames.
                                               This parameter can be a value of @ref MDF_SadHangoverWindow */
  uint32_t         LearningFrames;        /*!< Number of learning frames for the first estimation of noise level.
                                               This parameter can be a value of @ref MDF_SadLearningFrames */
  uint32_t         AmbientNoiseSlope;     /*!< Ambient noise slope control.
                                               This parameter must be a number between Min_Data = 0 and Max_Data = 7.
                                               @note This parameter is not used if Mode is set
                                                     to MDF_SAD_SOUND_DETECTOR */
  uint32_t         SignalNoiseThreshold;  /*!< Signal to noise threshold.
                                               This parameter can be a value of @ref MDF_SadSignalNoiseThreshold */
  FunctionalState  SoundLevelInterrupt;    /*!< Sound level interrupt enable/disable.
                                               @note This interrupt is mainly used for debug purpose*/

} MDF_SoundActivityTypeDef;

/**
  * @brief  MDF filter trigger structure definition
  */
typedef struct
{
  uint32_t  Source;  /*!< Filter trigger source.
                          This parameter can be a value of @ref MDF_FilterTriggerSource */
  uint32_t  Edge;    /*!< Filter trigger edge.
                          This parameter can be a value of @ref MDF_FilterTriggerEdge */
} MDF_FilterTriggerTypeDef;

/**
  * @brief  MDF filter configuration structure definition
  */
typedef struct
{
  uint32_t                  DataSource;      /*!< Filter data source.
                                                  This parameter can be a value of @ref MDF_DataSource */
  uint32_t                  Delay;           /*!< Delay to apply on data source in number of samples.
                                                  This parameter must be a number between Min_Data = 0
                                                  and Max_Data = 127 */
  uint32_t                  CicMode;         /*!< CIC filter mode.
                                                  This parameter can be a value of @ref MDF_CicMode */
  uint32_t                  DecimationRatio; /*!< Filter decimation ratio.
                                                  This parameter must be a number between Min_Data = 2
                                                  and Max_Data = 512 */
  int32_t                   Offset;          /*!< Filter offset error compensation.
                                                  This parameter must be a number between Min_Data = -33554432
                                                  and Max_Data = 33554431.
                                                  @note This parameter is not used for ADF instance */
  int32_t                   Gain;            /*!< Filter gain in step of around 3db (from -48db to 72dB).
                                                  This parameter must be a number between Min_Data = -16
                                                  and Max_Data = 24 */
  MDF_ReshapeFilterTypeDef  ReshapeFilter;   /*!< Reshape filter configuration */
  MDF_HighPassFilterTypeDef HighPassFilter;  /*!< High pass filter configuration */
  MDF_IntegratorTypeDef     Integrator;      /*!< Integrator configuration.
                                                  @note This parameter is not used for ADF instance */
  MDF_SoundActivityTypeDef  SoundActivity;   /*!< Sound activity detector configuration.
                                                  @note This parameter is not used for MDF instance */
  uint32_t                  AcquisitionMode; /*!< Filter acquisition mode.
                                                  This parameter can be a value of @ref MDF_AcquisitionMode */
  uint32_t                  FifoThreshold;   /*!< Filter RXFIFO threshold.
                                                  This parameter can be a value of @ref MDF_FifoThreshold */
  uint32_t                  DiscardSamples;  /*!< Number of samples to discard after filter enable.
                                                  This parameter must be a number between Min_Data = 0
                                                  and Max_Data = 255 */
  MDF_FilterTriggerTypeDef  Trigger;         /*!< Filter trigger configuration.
                                                  @note This parameter is not used if AcquisitionMode is set
                                                        to MDF_MODE_ASYNC_CONT or MDF_MODE_ASYNC_SINGLE */
  uint32_t                  SnapshotFormat;  /*!< Snapshot format.
                                                  This parameter can be a value of @ref MDF_SnapshotFormat.
                                                  @note This parameter is used only if AcquisitionMode is set
                                                        to MDF_MODE_SYNC_SNAPSHOT and for MDF instance */
} MDF_FilterConfigTypeDef;

/**
  * @brief  MDF snapshot parameters structure definition
  */
typedef struct
{
  int32_t   Value;              /*!< Snapshot acquisition value on 16 or 23 MSB depending on snapshot format */
  uint32_t  DecimationCounter;  /*!< Decimation counter value when snapshot trigger occurs */
  uint32_t  IntegratorCounter;  /*!< Integrator counter value when snapshot trigger occurs.
                                     This value is available only if snapshot format is 16 bits resolution */
} MDF_SnapshotParamTypeDef;

/**
  * @brief  MDF DMA configuration structure definition
  */
typedef struct
{
  uint32_t         Address;     /*!< DMA destination address */
  uint32_t         DataLength;  /*!< Length of data to transfer in bytes */
  FunctionalState  MsbOnly;     /*!< Transfer only the 16MSB of the acquistion data */
} MDF_DmaConfigTypeDef;

/**
  * @brief  MDF short-circuit detector configuration structure definition
  */
typedef struct
{
  uint32_t  Threshold;    /*!< Short-circuit detector threshold.
                               This parameter must be a number between Min_Data = 2 and Max_Data = 256 */
  uint32_t  BreakSignal;  /*!< Break signal assignment for short-circuit detector.
                               This parameter can be a values combination of @ref MDF_BreakSignals */
} MDF_ScdConfigTypeDef;

/**
  * @brief  MDF out-off limit detector configuration structure definition
  */
typedef struct
{
  uint32_t  OldCicMode;          /*!< Out-off limit detector CIC filter mode.
                                      This parameter can be a value of @ref MDF_OldCicMode */
  uint32_t  OldDecimationRatio;  /*!< Out-off limit detector decimation ratio.
                                      This parameter must be a number between Min_Data = 1 and Max_Data = 32 */
  int32_t   HighThreshold;       /*!< Out-off limit detector high threshold.
                                      This parameter must be a number between Min_Data = -33554432
                                      and Max_Data = 33554431 */
  int32_t   LowThreshold;        /*!< Out-off limit detector low threshold.
                                      This parameter must be a number between Min_Data = -33554432
                                      and Max_Data = 33554431 */
  uint32_t  OldEventConfig;      /*!< Out-off limit event configuration.
                                      This parameter can be a value of @ref MDF_OldEventConfig */
  uint32_t  BreakSignal;         /*!< Break signal assignment for out-off limit detector.
                                      This parameter can be a values combination of @ref MDF_BreakSignals */
} MDF_OldConfigTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup MDF_Exported_Constants  MDF Exported Constants
  * @{
  */

/** @defgroup MDF_ErrorCode MDF error code
  * @{
  */
#define MDF_ERROR_NONE                  0x00000000U /*!< No error */
#define MDF_ERROR_ACQUISITION_OVERFLOW  0x00000001U /*!< Overflow occurs during acquisition */
#define MDF_ERROR_RSF_OVERRUN           0x00000002U /*!< Overrun occurs on reshape filter */
#define MDF_ERROR_CLOCK_ABSENCE         0x00000004U /*!< Clock absence detection occurs */
#define MDF_ERROR_SHORT_CIRCUIT         0x00000008U /*!< Short circuit detection occurs.
                                                         @note Not used for ADF instance */
#define MDF_ERROR_SATURATION            0x00000010U /*!< Saturation detection occurs */
#define MDF_ERROR_OUT_OFF_LIMIT         0x00000020U /*!< Out-off limit detection occurs.
                                                         @note Not used for ADF instance */
#define MDF_ERROR_DMA                   0x00000040U /*!< DMA error occurs */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
#define MDF_ERROR_INVALID_CALLBACK      0x00000080U /*!< Invalid callback error occurs */
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup MDF_ClockTriggerSource MDF output clock trigger source
  * @{
  */
#define MDF_CLOCK_TRIG_TRGO        0x00000000U
#define MDF_CLOCK_TRIG_EXTI15      MDF_CKGCR_TRGSRC_1
#define MDF_CLOCK_TRIG_EXTI11     (MDF_CKGCR_TRGSRC_0 | \
                                   MDF_CKGCR_TRGSRC_1)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_TIM1_TRGO   MDF_CKGCR_TRGSRC_2      /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_TIM1_TRGO2 (MDF_CKGCR_TRGSRC_0 | \
                                   MDF_CKGCR_TRGSRC_2)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_TIM3_TRGO  (MDF_CKGCR_TRGSRC_1 | \
                                   MDF_CKGCR_TRGSRC_2)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_TIM4_TRGO  (MDF_CKGCR_TRGSRC_0 | \
                                   MDF_CKGCR_TRGSRC_1 | \
                                   MDF_CKGCR_TRGSRC_2)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_TIM6_TRGO   MDF_CKGCR_TRGSRC_3      /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_TIM7_TRGO  (MDF_CKGCR_TRGSRC_0 | \
                                   MDF_CKGCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_TIM8_TRGO  (MDF_CKGCR_TRGSRC_1 | \
                                   MDF_CKGCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_TIM8_TRGO2 (MDF_CKGCR_TRGSRC_0 | \
                                   MDF_CKGCR_TRGSRC_1 | \
                                   MDF_CKGCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_TIM15_TRGO (MDF_CKGCR_TRGSRC_2 | \
                                   MDF_CKGCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_LPTIM1_OUT (MDF_CKGCR_TRGSRC_0 | \
                                   MDF_CKGCR_TRGSRC_2 | \
                                   MDF_CKGCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_SAD_DET    (MDF_CKGCR_TRGSRC_1 | \
                                   MDF_CKGCR_TRGSRC_2 | \
                                   MDF_CKGCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_CLOCK_TRIG_SAD_TRGO    MDF_CKGCR_TRGSRC        /*!< @note Not available for ADF instance */
/**
  * @}
  */

/** @defgroup MDF_ClockTriggerEdge MDF output clock trigger edge
  * @{
  */
#define MDF_CLOCK_TRIG_RISING_EDGE   0x00000000U        /*!< Rising edge */
#define MDF_CLOCK_TRIG_FALLING_EDGE  MDF_CKGCR_TRGSENS  /*!< Falling edge */
/**
  * @}
  */

/** @defgroup MDF_OuputClockPins MDF output clock pins
  * @{
  */
#define MDF_OUTPUT_CLOCK_0    MDF_CKGCR_CCK0DIR      /*!< MDF_CCK0 is used as output clock */
#define MDF_OUTPUT_CLOCK_1    MDF_CKGCR_CCK1DIR      /*!< MDF_CCK1 is used as output clock */
#define MDF_OUTPUT_CLOCK_ALL (MDF_CKGCR_CCK0DIR | \
                              MDF_CKGCR_CCK1DIR)     /*!< MDF_CCK0 and MDF_CCK1 are used as output clock */
/**
  * @}
  */

/** @defgroup MDF_SitfMode MDF serial interface mode
  * @{
  */
#define MDF_SITF_LF_MASTER_SPI_MODE       0x00000000U           /*!< Low frequency master SPI mode */
#define MDF_SITF_NORMAL_SPI_MODE          MDF_SITFCR_SITFMOD_0  /*!< Normal SPI mode */
#define MDF_SITF_MANCHESTER_FALLING_MODE  MDF_SITFCR_SITFMOD_1  /*!< Manchester mode rising edge logic 0
                                                                     and falling edge logic 1 */
#define MDF_SITF_MANCHESTER_RISING_MODE   MDF_SITFCR_SITFMOD    /*!< Manchester mode rising edge logic 1
                                                                     and falling edge logic 0 */
/**
  * @}
  */

/** @defgroup MDF_SitfClockSource MDF serial interface clock source
  * @{
  */
#define MDF_SITF_CCK0_SOURCE  0x00000000U          /*!< Common clock 0 source */
#define MDF_SITF_CCK1_SOURCE  MDF_SITFCR_SCKSRC_0  /*!< Common clock 1 source */
#define MDF_SITF_CKI_SOURCE   MDF_SITFCR_SCKSRC_1  /*!< Dedicated input clock source.
                                                        @note Not available for ADF instance */
/**
  * @}
  */

/** @defgroup MDF_FilterBitstream MDF filter bitstream
  * @{
  */
#define MDF_BITSTREAM0_RISING   0x00000000U
#define MDF_BITSTREAM0_FALLING  MDF_BSMXCR_BSSEL_0
#define MDF_BITSTREAM1_RISING   MDF_BSMXCR_BSSEL_1      /*!< @note Not available for ADF instance */
#define MDF_BITSTREAM1_FALLING (MDF_BSMXCR_BSSEL_0 | \
                                MDF_BSMXCR_BSSEL_1)     /*!< @note Not available for ADF instance */
#define MDF_BITSTREAM2_RISING   MDF_BSMXCR_BSSEL_2      /*!< @note Not available for ADF instance */
#define MDF_BITSTREAM2_FALLING (MDF_BSMXCR_BSSEL_0 | \
                                MDF_BSMXCR_BSSEL_2)     /*!< @note Not available for ADF instance */
#define MDF_BITSTREAM3_RISING  (MDF_BSMXCR_BSSEL_1 | \
                                MDF_BSMXCR_BSSEL_2)     /*!< @note Not available for ADF instance */
#define MDF_BITSTREAM3_FALLING (MDF_BSMXCR_BSSEL_0 | \
                                MDF_BSMXCR_BSSEL_1 | \
                                MDF_BSMXCR_BSSEL_2)     /*!< @note Not available for ADF instance */
#define MDF_BITSTREAM4_RISING   MDF_BSMXCR_BSSEL_3      /*!< @note Not available for ADF instance */
#define MDF_BITSTREAM4_FALLING (MDF_BSMXCR_BSSEL_0 | \
                                MDF_BSMXCR_BSSEL_3)     /*!< @note Not available for ADF instance */
#define MDF_BITSTREAM5_RISING  (MDF_BSMXCR_BSSEL_1 | \
                                MDF_BSMXCR_BSSEL_3)     /*!< @note Not available for ADF instance */
#define MDF_BITSTREAM5_FALLING (MDF_BSMXCR_BSSEL_0 | \
                                MDF_BSMXCR_BSSEL_1 | \
                                MDF_BSMXCR_BSSEL_3)     /*!< @note Not available for ADF instance */
/**
  * @}
  */

/** @defgroup MDF_ReshapeDecimationRatio MDF reshape filter decimation ratio
  * @{
  */
#define MDF_RSF_DECIMATION_RATIO_4  0x00000000U          /*!< Reshape filter decimation ratio is 4 */
#define MDF_RSF_DECIMATION_RATIO_1  MDF_DFLTRSFR_RSFLTD  /*!< Reshape filter decimation ratio is 1 */
/**
  * @}
  */

/** @defgroup MDF_HighPassCutOffFreq MDF high pass filter cut-off frequency
  * @{
  */
#define MDF_HPF_CUTOFF_0_000625FPCM  0x00000000U          /*!< Cut-off frequency of 0.000625xFpcm */
#define MDF_HPF_CUTOFF_0_00125FPCM   MDF_DFLTRSFR_HPFC_0  /*!< Cut-off frequency of 0.00125xFpcm */
#define MDF_HPF_CUTOFF_0_0025FPCM    MDF_DFLTRSFR_HPFC_1  /*!< Cut-off frequency of 0.0025xFpcm */
#define MDF_HPF_CUTOFF_0_0095FPCM    MDF_DFLTRSFR_HPFC    /*!< Cut-off frequency of 0.0095xFpcm */
/**
  * @}
  */

/** @defgroup MDF_IntegratorOutputDiv MDF integrator output division
  * @{
  */
#define MDF_INTEGRATOR_OUTPUT_DIV_128  0x00000000U            /*!< Integrator data outputs divided by 128 */
#define MDF_INTEGRATOR_OUTPUT_DIV_32   MDF_DFLTINTR_INTDIV_0  /*!< Integrator data outputs divided by 32 */
#define MDF_INTEGRATOR_OUTPUT_DIV_4    MDF_DFLTINTR_INTDIV_1  /*!< Integrator data outputs divided by 4 */
#define MDF_INTEGRATOR_OUTPUT_NO_DIV   MDF_DFLTINTR_INTDIV    /*!< Integrator data outputs not divided */
/**
  * @}
  */

/** @defgroup MDF_SadMode MDF sound activity detector mode
  * @{
  */
#define MDF_SAD_VOICE_ACTIVITY_DETECTOR  0x00000000U         /*!< Voice activity detector */
#define MDF_SAD_SOUND_DETECTOR           MDF_SADCR_SADMOD_0  /*!< Sound detector */
#define MDF_SAD_AMBIENT_NOISE_DETECTOR   MDF_SADCR_SADMOD    /*!< Ambient noise detector */
/**
  * @}
  */

/** @defgroup MDF_SadFrameSize MDF sound activity detector frame size
  * @{
  */
#define MDF_SAD_8_PCM_SAMPLES    0x00000000U                               /*!< Frame size of 8 PCM samples */
#define MDF_SAD_16_PCM_SAMPLES   MDF_SADCR_FRSIZE_0                        /*!< Frame size of 16 PCM samples */
#define MDF_SAD_32_PCM_SAMPLES   MDF_SADCR_FRSIZE_1                        /*!< Frame size of 32 PCM samples */
#define MDF_SAD_64_PCM_SAMPLES  (MDF_SADCR_FRSIZE_0 | MDF_SADCR_FRSIZE_1)  /*!< Frame size of 64 PCM samples */
#define MDF_SAD_128_PCM_SAMPLES  MDF_SADCR_FRSIZE_2                        /*!< Frame size of 128 PCM samples */
#define MDF_SAD_256_PCM_SAMPLES (MDF_SADCR_FRSIZE_0 | MDF_SADCR_FRSIZE_2)  /*!< Frame size of 256 PCM samples */
#define MDF_SAD_512_PCM_SAMPLES  MDF_SADCR_FRSIZE                          /*!< Frame size of 512 PCM samples */
/**
  * @}
  */

/** @defgroup MDF_SadSoundTriggerEvent MDF sound activity detector trigger event
  * @{
  */
#define MDF_SAD_ENTER_DETECT       0x00000000U       /*!< Event when SAD enters in detect state */
#define MDF_SAD_ENTER_EXIT_DETECT  MDF_SADCR_DETCFG  /*!< Event when SAD enters or exits from detect state */
/**
  * @}
  */

/** @defgroup MDF_SadDataMemoryTransfer MDF sound activity detector data memory transfer mode
  * @{
  */
#define MDF_SAD_NO_MEMORY_TRANSFER         0x00000000U         /*!< No memory transfer */
#define MDF_SAD_MEMORY_TRANSFER_IN_DETECT  MDF_SADCR_DATCAP_0  /*!< Memory transfer only in detect state */
#define MDF_SAD_MEMORY_TRANSFER_ALWAYS     MDF_SADCR_DATCAP    /*!< Memory transfer always */
/**
  * @}
  */

/** @defgroup MDF_SadHangoverWindow MDF sound activity detector data hangover time window
  * @{
  */
#define MDF_SAD_HANGOVER_4_FRAMES    0x00000000U              /*!< Hangover window of 4 frames */
#define MDF_SAD_HANGOVER_8_FRAMES    MDF_SADCFGR_HGOVR_0      /*!< Hangover window of 8 frames */
#define MDF_SAD_HANGOVER_16_FRAMES   MDF_SADCFGR_HGOVR_1      /*!< Hangover window of 16 frames */
#define MDF_SAD_HANGOVER_32_FRAMES  (MDF_SADCFGR_HGOVR_0 | \
                                     MDF_SADCFGR_HGOVR_1)     /*!< Hangover window of 32 frames */
#define MDF_SAD_HANGOVER_64_FRAMES   MDF_SADCFGR_HGOVR_2      /*!< Hangover window of 64 frames */
#define MDF_SAD_HANGOVER_128_FRAMES (MDF_SADCFGR_HGOVR_0 | \
                                     MDF_SADCFGR_HGOVR_2)     /*!< Hangover window of 128 frames */
#define MDF_SAD_HANGOVER_256_FRAMES (MDF_SADCFGR_HGOVR_1 | \
                                     MDF_SADCFGR_HGOVR_2)     /*!< Hangover window of 256 frames */
#define MDF_SAD_HANGOVER_512_FRAMES (MDF_SADCFGR_HGOVR_0 | \
                                     MDF_SADCFGR_HGOVR_1 | \
                                     MDF_SADCFGR_HGOVR_2)     /*!< Hangover window of 512 frames */
/**
  * @}
  */

/** @defgroup MDF_SadLearningFrames MDF sound activity detector data learning frames
  * @{
  */
#define MDF_SAD_LEARNING_2_FRAMES   0x00000000U                                 /*!< 2 learning frames */
#define MDF_SAD_LEARNING_4_FRAMES   MDF_SADCFGR_LFRNB_0                         /*!< 4 learning frames */
#define MDF_SAD_LEARNING_8_FRAMES   MDF_SADCFGR_LFRNB_1                         /*!< 8 learning frames */
#define MDF_SAD_LEARNING_16_FRAMES (MDF_SADCFGR_LFRNB_0 | MDF_SADCFGR_LFRNB_1)  /*!< 16 learning frames */
#define MDF_SAD_LEARNING_32_FRAMES  MDF_SADCFGR_LFRNB                           /*!< 32 learning frames */
/**
  * @}
  */

/** @defgroup MDF_SadSignalNoiseThreshold MDF sound activity detector data signal to noise threshold
  * @{
  */
#define MDF_SAD_SIGNAL_NOISE_3_5DB   0x00000000U              /*!< Signal to noise threshold is 3.5dB */
#define MDF_SAD_SIGNAL_NOISE_6DB     MDF_SADCFGR_SNTHR_0      /*!< Signal to noise threshold is 6dB */
#define MDF_SAD_SIGNAL_NOISE_9_5DB   MDF_SADCFGR_SNTHR_1      /*!< Signal to noise threshold is 9.5dB */
#define MDF_SAD_SIGNAL_NOISE_12DB   (MDF_SADCFGR_SNTHR_0 | \
                                     MDF_SADCFGR_SNTHR_1)     /*!< Signal to noise threshold is 12dB */
#define MDF_SAD_SIGNAL_NOISE_15_6DB  MDF_SADCFGR_SNTHR_2      /*!< Signal to noise threshold is 15.6dB */
#define MDF_SAD_SIGNAL_NOISE_18DB   (MDF_SADCFGR_SNTHR_0 | \
                                     MDF_SADCFGR_SNTHR_2)     /*!< Signal to noise threshold is 18dB */
#define MDF_SAD_SIGNAL_NOISE_21_6DB (MDF_SADCFGR_SNTHR_1 | \
                                     MDF_SADCFGR_SNTHR_2)     /*!< Signal to noise threshold is 21.6dB */
#define MDF_SAD_SIGNAL_NOISE_24_1DB (MDF_SADCFGR_SNTHR_0 | \
                                     MDF_SADCFGR_SNTHR_1 | \
                                     MDF_SADCFGR_SNTHR_2)     /*!< Signal to noise threshold is 24.1dB */
#define MDF_SAD_SIGNAL_NOISE_27_6DB  MDF_SADCFGR_SNTHR_3      /*!< Signal to noise threshold is 27.6dB */
#define MDF_SAD_SIGNAL_NOISE_30_1DB (MDF_SADCFGR_SNTHR_0 | \
                                     MDF_SADCFGR_SNTHR_3)     /*!< Signal to noise threshold is 30.1dB */
/**
  * @}
  */

/** @defgroup MDF_FilterTriggerSource MDF filter trigger source
  * @{
  */
#define MDF_FILTER_TRIG_TRGO         0x00000000U
#define MDF_FILTER_TRIG_OLD_EVENT    MDF_DFLTCR_TRGSRC_0      /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_EXTI15       MDF_DFLTCR_TRGSRC_1
#define MDF_FILTER_TRIG_EXTI11      (MDF_DFLTCR_TRGSRC_0 | \
                                     MDF_DFLTCR_TRGSRC_1)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_TIM1_TRGO    MDF_DFLTCR_TRGSRC_2      /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_TIM1_TRGO2  (MDF_DFLTCR_TRGSRC_0 | \
                                     MDF_DFLTCR_TRGSRC_2)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_TIM3_TRGO   (MDF_DFLTCR_TRGSRC_1 | \
                                     MDF_DFLTCR_TRGSRC_2)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_TIM4_TRGO   (MDF_DFLTCR_TRGSRC_0 | \
                                     MDF_DFLTCR_TRGSRC_1 | \
                                     MDF_DFLTCR_TRGSRC_2)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_TIM6_TRGO    MDF_DFLTCR_TRGSRC_3      /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_TIM7_TRGO   (MDF_DFLTCR_TRGSRC_0 | \
                                     MDF_DFLTCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_TIM8_TRGO   (MDF_DFLTCR_TRGSRC_1 | \
                                     MDF_DFLTCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_TIM8_TRGO2  (MDF_DFLTCR_TRGSRC_0 | \
                                     MDF_DFLTCR_TRGSRC_1 | \
                                     MDF_DFLTCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_TIM15_TRGO  (MDF_DFLTCR_TRGSRC_2 | \
                                     MDF_DFLTCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_LPTIM1_OUT  (MDF_DFLTCR_TRGSRC_0 | \
                                     MDF_DFLTCR_TRGSRC_2 | \
                                     MDF_DFLTCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_ADF_SAD_DET (MDF_DFLTCR_TRGSRC_1 | \
                                     MDF_DFLTCR_TRGSRC_2 | \
                                     MDF_DFLTCR_TRGSRC_3)     /*!< @note Not available for ADF instance */
#define MDF_FILTER_TRIG_ADF_TRGO     MDF_DFLTCR_TRGSRC        /*!< @note Not available for ADF instance */
/**
  * @}
  */

/** @defgroup MDF_FilterTriggerEdge MDF filter trigger edge
  * @{
  */
#define MDF_FILTER_TRIG_RISING_EDGE   0x00000000U         /*!< Rising edge */
#define MDF_FILTER_TRIG_FALLING_EDGE  MDF_DFLTCR_TRGSENS  /*!< Falling edge */
/**
  * @}
  */

/** @defgroup MDF_DataSource MDF data source
  * @{
  */
#define MDF_DATA_SOURCE_BSMX     0x00000000U            /*!< Data from bitstream matrix */
#define MDF_DATA_SOURCE_ADCITF1  MDF_DFLTCICR_DATSRC_1  /*!< Data from ADC interface 1.
                                                             @note Not available for ADF instance */
#define MDF_DATA_SOURCE_ADCITF2  MDF_DFLTCICR_DATSRC    /*!< Data from ADC interface 2.
                                                             @note Not available for ADF instance */
/**
  * @}
  */

/** @defgroup MDF_CicMode MDF CIC mode
  * @{
  */
#define MDF_TWO_FILTERS_MCIC_FASTSINC  0x00000000U                /*!< Two filters, main filter in FastSinc order.
                                                                       @note Not available for ADF instance */
#define MDF_TWO_FILTERS_MCIC_SINC1     MDF_DFLTCICR_CICMOD_0      /*!< Two filters, main filter in Sinc1 order.
                                                                       @note Not available for ADF instance */
#define MDF_TWO_FILTERS_MCIC_SINC2     MDF_DFLTCICR_CICMOD_1      /*!< Two filters, main filter in Sinc2 order.
                                                                       @note Not available for ADF instance */
#define MDF_TWO_FILTERS_MCIC_SINC3    (MDF_DFLTCICR_CICMOD_0 | \
                                       MDF_DFLTCICR_CICMOD_1)     /*!< Two filters, main filter in Sinc3 order.
                                                                       @note Not available for ADF instance */
#define MDF_ONE_FILTER_SINC4           MDF_DFLTCICR_CICMOD_2      /*!< One filter in Sinc4 order */
#define MDF_ONE_FILTER_SINC5          (MDF_DFLTCICR_CICMOD_0 | \
                                       MDF_DFLTCICR_CICMOD_2)     /*!< One filter in Sinc5 order */
/**
  * @}
  */

/** @defgroup MDF_AcquisitionMode MDF acquisition mode
  * @{
  */
#define MDF_MODE_ASYNC_CONT     0x00000000U              /*!< Asynchronous, continuous acquisition mode */
#define MDF_MODE_ASYNC_SINGLE   MDF_DFLTCR_ACQMOD_0      /*!< Asynchronous, single-shot acquisition mode.
                                                              @note Not available for ADF instance with SAD usage */
#define MDF_MODE_SYNC_CONT      MDF_DFLTCR_ACQMOD_1      /*!< Synchronous, continuous acquisition mode */
#define MDF_MODE_SYNC_SINGLE   (MDF_DFLTCR_ACQMOD_0 | \
                                MDF_DFLTCR_ACQMOD_1)     /*!< Synchronous, single-shot acquisition mode.
                                                              @note Not available for ADF instance with SAD usage*/
#define MDF_MODE_WINDOW_CONT    MDF_DFLTCR_ACQMOD_2      /*!< Window, continuous acquisition mode.
                                                              @note Not available for ADF instance with SAD usage*/
#define MDF_MODE_SYNC_SNAPSHOT (MDF_DFLTCR_ACQMOD_0 | \
                                MDF_DFLTCR_ACQMOD_2)     /*!< Synchronous, snapshot acquisition mode.
                                                              @note Not available for ADF instance */
/**
  * @}
  */

/** @defgroup MDF_FifoThreshold MDF RXFIFO threshold
  * @{
  */
#define MDF_FIFO_THRESHOLD_NOT_EMPTY  0x00000000U     /*!< Event generated when RXFIFO is not empty */
#define MDF_FIFO_THRESHOLD_HALF_FULL  MDF_DFLTCR_FTH  /*!< Event generated when RXFIFO is half_full */
/**
  * @}
  */

/** @defgroup MDF_SnapshotFormat MDF snapshot format
  * @{
  */
#define MDF_SNAPSHOT_23BITS  0x00000000U         /*!< Snapshot data resolution of 23 bits */
#define MDF_SNAPSHOT_16BITS  MDF_DFLTCR_SNPSFMT  /*!< Snapshot data resolution of 16 bits */
/**
  * @}
  */

/** @defgroup MDF_BreakSignals MDF break signals
  * @{
  */
#define MDF_NO_BREAK_SIGNAL     0x00000000U /*!< No break signal */
#define MDF_TIM1_BREAK_SIGNAL   0x00000001U /*!< TIM1_BREAK signal */
#define MDF_TIM1_BREAK2_SIGNAL  0x00000002U /*!< TIM1_BREAK2 signal */
#define MDF_TIM8_BREAK_SIGNAL   0x00000004U /*!< TIM8_BREAK signal */
#define MDF_TIM8_BREAK2_SIGNAL  0x00000008U /*!< TIM8_BREAK2 signal */
/**
  * @}
  */

/** @defgroup MDF_OldCicMode MDF out-off limit detector CIC mode
  * @{
  */
#define MDF_OLD_FILTER_FASTSINC  0x00000000U        /*!< Out-off limit detector filter in FastSinc order */
#define MDF_OLD_FILTER_SINC1     MDF_OLDCR_ACICN_0  /*!< Out-off limit detector filter in Sinc1 order */
#define MDF_OLD_FILTER_SINC2     MDF_OLDCR_ACICN_1  /*!< Out-off limit detector filter in Sinc2 order */
#define MDF_OLD_FILTER_SINC3     MDF_OLDCR_ACICN    /*!< Out-off limit detector filter in Sinc3 order */
/**
  * @}
  */

/** @defgroup MDF_OldEventConfig MDF out-off limit detector event configuration
  * @{
  */
#define MDF_OLD_SIGNAL_OUTSIDE_THRESHOLDS  0x00000000U      /*!< Out-off limit detector event is generated
                                                                 if signal is outside thresholds */
#define MDF_OLD_SIGNAL_IN_THRESHOLDS       MDF_OLDCR_THINB  /*!< Out-off limit detector event is generated
                                                                 if signal is in thresholds */
/**
  * @}
  */

/** @defgroup MDF_OldThresholdInfo MDF out-off limit detector threshold information
  * @{
  */
#define MDF_OLD_IN_THRESHOLDS   0x00000000U  /*!< The signal was in thresholds when out-off limit detection occurs */
#define MDF_OLD_HIGH_THRESHOLD  0x00000001U  /*!< The signal was higher than high threshold
                                                  when out-off limit detection occurs */
#define MDF_OLD_LOW_THRESHOLD   0x00000002U  /*!< The signal was lower than low threshold
                                                  when out-off limit detection occurs */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup MDF_Exported_Macros  MDF Exported Macros
  * @{
  */

/** @brief  Reset MDF handle state.
  * @param  __HANDLE__ MDF handle.
  * @retval None
  */
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
#define __HAL_MDF_RESET_HANDLE_STATE(__HANDLE__) do{                                             \
                                                      (__HANDLE__)->State = HAL_MDF_STATE_RESET; \
                                                      (__HANDLE__)->MspInitCallback = NULL;      \
                                                      (__HANDLE__)->MspDeInitCallback = NULL;    \
                                                    } while(0)
#else /* USE_HAL_MDF_REGISTER_CALLBACKS */
#define __HAL_MDF_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_MDF_STATE_RESET)
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup MDF_Exported_Functions
  * @{
  */

/* Initialization and de-initialization functions  ****************************/
/** @addtogroup MDF_Exported_Functions_Group1
  * @{
  */
HAL_StatusTypeDef HAL_MDF_Init(MDF_HandleTypeDef *hmdf);
HAL_StatusTypeDef HAL_MDF_DeInit(MDF_HandleTypeDef *hmdf);
void              HAL_MDF_MspInit(MDF_HandleTypeDef *hmdf);
void              HAL_MDF_MspDeInit(MDF_HandleTypeDef *hmdf);
#if (USE_HAL_MDF_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef HAL_MDF_RegisterCallback(MDF_HandleTypeDef        *hmdf,
                                           HAL_MDF_CallbackIDTypeDef CallbackID,
                                           pMDF_CallbackTypeDef      pCallback);
HAL_StatusTypeDef HAL_MDF_UnRegisterCallback(MDF_HandleTypeDef        *hmdf,
                                             HAL_MDF_CallbackIDTypeDef CallbackID);
HAL_StatusTypeDef HAL_MDF_RegisterOldCallback(MDF_HandleTypeDef      *hmdf,
                                              pMDF_OldCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_MDF_UnRegisterOldCallback(MDF_HandleTypeDef *hmdf);
HAL_StatusTypeDef HAL_MDF_RegisterSndLvlCallback(MDF_HandleTypeDef         *hmdf,
                                                 pMDF_SndLvlCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_MDF_UnRegisterSndLvlCallback(MDF_HandleTypeDef *hmdf);
#endif /* USE_HAL_MDF_REGISTER_CALLBACKS */
/**
  * @}
  */

/* Acquisition functions  *****************************************************/
/** @addtogroup MDF_Exported_Functions_Group2
  * @{
  */
HAL_StatusTypeDef HAL_MDF_AcqStart(MDF_HandleTypeDef *hmdf, const MDF_FilterConfigTypeDef *pFilterConfig);
HAL_StatusTypeDef HAL_MDF_PollForAcq(MDF_HandleTypeDef *hmdf, uint32_t Timeout);
HAL_StatusTypeDef HAL_MDF_PollForSnapshotAcq(MDF_HandleTypeDef *hmdf, uint32_t Timeout);
HAL_StatusTypeDef HAL_MDF_GetAcqValue(const MDF_HandleTypeDef *hmdf, int32_t *pValue);
HAL_StatusTypeDef HAL_MDF_GetSnapshotAcqValue(MDF_HandleTypeDef *hmdf, MDF_SnapshotParamTypeDef *pSnapshotParam);
HAL_StatusTypeDef HAL_MDF_AcqStop(MDF_HandleTypeDef *hmdf);
HAL_StatusTypeDef HAL_MDF_AcqStart_IT(MDF_HandleTypeDef *hmdf, const MDF_FilterConfigTypeDef *pFilterConfig);
HAL_StatusTypeDef HAL_MDF_AcqStop_IT(MDF_HandleTypeDef *hmdf);
HAL_StatusTypeDef HAL_MDF_AcqStart_DMA(MDF_HandleTypeDef *hmdf, const MDF_FilterConfigTypeDef *pFilterConfig,
                                       const MDF_DmaConfigTypeDef *pDmaConfig);
HAL_StatusTypeDef HAL_MDF_AcqStop_DMA(MDF_HandleTypeDef *hmdf);
HAL_StatusTypeDef HAL_MDF_GenerateTrgo(const MDF_HandleTypeDef *hmdf);
HAL_StatusTypeDef HAL_MDF_SetDelay(MDF_HandleTypeDef *hmdf, uint32_t Delay);
HAL_StatusTypeDef HAL_MDF_GetDelay(const MDF_HandleTypeDef *hmdf, uint32_t *pDelay);
HAL_StatusTypeDef HAL_MDF_SetGain(MDF_HandleTypeDef *hmdf, int32_t Gain);
HAL_StatusTypeDef HAL_MDF_GetGain(const MDF_HandleTypeDef *hmdf, int32_t *pGain);
HAL_StatusTypeDef HAL_MDF_SetOffset(MDF_HandleTypeDef *hmdf, int32_t Offset);
HAL_StatusTypeDef HAL_MDF_GetOffset(const MDF_HandleTypeDef *hmdf, int32_t *pOffset);
HAL_StatusTypeDef HAL_MDF_PollForSndLvl(MDF_HandleTypeDef *hmdf, uint32_t Timeout, uint32_t *pSoundLevel,
                                        uint32_t *pAmbientNoise);
HAL_StatusTypeDef HAL_MDF_PollForSad(MDF_HandleTypeDef *hmdf, uint32_t Timeout);
void              HAL_MDF_AcqCpltCallback(MDF_HandleTypeDef *hmdf);
void              HAL_MDF_AcqHalfCpltCallback(MDF_HandleTypeDef *hmdf);
void              HAL_MDF_SndLvlCallback(MDF_HandleTypeDef *hmdf, uint32_t SoundLevel, uint32_t AmbientNoise);
void              HAL_MDF_SadCallback(MDF_HandleTypeDef *hmdf);
/**
  * @}
  */

/* Clock absence detection functions  *****************************************/
/** @addtogroup MDF_Exported_Functions_Group3
  * @{
  */
HAL_StatusTypeDef HAL_MDF_PollForCkab(MDF_HandleTypeDef *hmdf, uint32_t Timeout);
HAL_StatusTypeDef HAL_MDF_CkabStart_IT(MDF_HandleTypeDef *hmdf);
HAL_StatusTypeDef HAL_MDF_CkabStop_IT(MDF_HandleTypeDef *hmdf);
/**
  * @}
  */

/* Short circuit detection functions  *****************************************/
/** @addtogroup MDF_Exported_Functions_Group4
  * @{
  */
HAL_StatusTypeDef HAL_MDF_ScdStart(MDF_HandleTypeDef *hmdf, const MDF_ScdConfigTypeDef *pScdConfig);
HAL_StatusTypeDef HAL_MDF_PollForScd(MDF_HandleTypeDef *hmdf, uint32_t Timeout);
HAL_StatusTypeDef HAL_MDF_ScdStop(MDF_HandleTypeDef *hmdf);
HAL_StatusTypeDef HAL_MDF_ScdStart_IT(MDF_HandleTypeDef *hmdf, const MDF_ScdConfigTypeDef *pScdConfig);
HAL_StatusTypeDef HAL_MDF_ScdStop_IT(MDF_HandleTypeDef *hmdf);
/**
  * @}
  */

/* Out-off limit detection functions  *****************************************/
/** @addtogroup MDF_Exported_Functions_Group5
  * @{
  */
HAL_StatusTypeDef HAL_MDF_OldStart(MDF_HandleTypeDef *hmdf, const MDF_OldConfigTypeDef *pOldConfig);
HAL_StatusTypeDef HAL_MDF_PollForOld(MDF_HandleTypeDef *hmdf, uint32_t Timeout, uint32_t *pThresholdInfo);
HAL_StatusTypeDef HAL_MDF_OldStop(MDF_HandleTypeDef *hmdf);
HAL_StatusTypeDef HAL_MDF_OldStart_IT(MDF_HandleTypeDef *hmdf, const MDF_OldConfigTypeDef *pOldConfig);
HAL_StatusTypeDef HAL_MDF_OldStop_IT(MDF_HandleTypeDef *hmdf);
void              HAL_MDF_OldCallback(MDF_HandleTypeDef *hmdf, uint32_t ThresholdInfo);
/**
  * @}
  */

/* Generic functions  *********************************************************/
/** @addtogroup MDF_Exported_Functions_Group6
  * @{
  */
void                 HAL_MDF_IRQHandler(MDF_HandleTypeDef *hmdf);
void                 HAL_MDF_ErrorCallback(MDF_HandleTypeDef *hmdf);
HAL_MDF_StateTypeDef HAL_MDF_GetState(const MDF_HandleTypeDef *hmdf);
uint32_t             HAL_MDF_GetError(const MDF_HandleTypeDef *hmdf);
/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup MDF_Private_Macros  MDF Private Macros
  * @{
  */
#define IS_MDF_INSTANCE(PARAM) (((PARAM) == MDF1_Filter0) || \
                                ((PARAM) == MDF1_Filter1) || \
                                ((PARAM) == MDF1_Filter2) || \
                                ((PARAM) == MDF1_Filter3) || \
                                ((PARAM) == MDF1_Filter4) || \
                                ((PARAM) == MDF1_Filter5))

#define IS_ADF_INSTANCE(PARAM) ((PARAM) == ADF1_Filter0)

#define IS_MDF_FILTER_BITSTREAM(PARAM) (((PARAM) == MDF_BITSTREAM0_RISING)  || \
                                        ((PARAM) == MDF_BITSTREAM0_FALLING) || \
                                        ((PARAM) == MDF_BITSTREAM1_RISING)  || \
                                        ((PARAM) == MDF_BITSTREAM1_FALLING) || \
                                        ((PARAM) == MDF_BITSTREAM2_RISING)  || \
                                        ((PARAM) == MDF_BITSTREAM2_FALLING) || \
                                        ((PARAM) == MDF_BITSTREAM3_RISING)  || \
                                        ((PARAM) == MDF_BITSTREAM3_FALLING) || \
                                        ((PARAM) == MDF_BITSTREAM4_RISING)  || \
                                        ((PARAM) == MDF_BITSTREAM4_FALLING) || \
                                        ((PARAM) == MDF_BITSTREAM5_RISING)  || \
                                        ((PARAM) == MDF_BITSTREAM5_FALLING))

#define IS_MDF_INTERLEAVED_FILTERS(PARAM) ((PARAM) <= 5U)

#define IS_MDF_PROC_CLOCK_DIVIDER(PARAM) ((1U <= (PARAM)) && ((PARAM) <= 128U))

#define IS_MDF_OUTPUT_CLOCK_PINS(PARAM) (((PARAM) == MDF_OUTPUT_CLOCK_0) || \
                                         ((PARAM) == MDF_OUTPUT_CLOCK_1) || \
                                         ((PARAM) == MDF_OUTPUT_CLOCK_ALL))

#define IS_MDF_OUTPUT_CLOCK_DIVIDER(PARAM) ((1U <= (PARAM)) && ((PARAM) <= 16U))

#define IS_MDF_OUTPUT_CLOCK_TRIGGER_SOURCE(PARAM) (((PARAM) == MDF_CLOCK_TRIG_TRGO)       || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_TIM1_TRGO)  || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_TIM1_TRGO2) || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_TIM8_TRGO)  || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_TIM8_TRGO2) || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_TIM3_TRGO)  || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_TIM4_TRGO)  || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_TIM6_TRGO)  || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_TIM7_TRGO)  || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_TIM15_TRGO) || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_SAD_DET)    || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_EXTI11)     || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_EXTI15)     || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_LPTIM1_OUT) || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_SAD_TRGO))

#define IS_ADF_OUTPUT_CLOCK_TRIGGER_SOURCE(PARAM) (((PARAM) == MDF_CLOCK_TRIG_TRGO)       || \
                                                   ((PARAM) == MDF_CLOCK_TRIG_EXTI15))

#define IS_MDF_OUTPUT_CLOCK_TRIGGER_EDGE(PARAM) (((PARAM) == MDF_CLOCK_TRIG_RISING_EDGE) || \
                                                 ((PARAM) == MDF_CLOCK_TRIG_FALLING_EDGE))

#define IS_MDF_SITF_MODE(PARAM) (((PARAM) == MDF_SITF_LF_MASTER_SPI_MODE)      || \
                                 ((PARAM) == MDF_SITF_NORMAL_SPI_MODE)         || \
                                 ((PARAM) == MDF_SITF_MANCHESTER_FALLING_MODE) || \
                                 ((PARAM) == MDF_SITF_MANCHESTER_RISING_MODE))

#define IS_MDF_SITF_CLOCK_SOURCE(PARAM) (((PARAM) == MDF_SITF_CCK0_SOURCE) || \
                                         ((PARAM) == MDF_SITF_CCK1_SOURCE) || \
                                         ((PARAM) == MDF_SITF_CKI_SOURCE))

#define IS_MDF_SITF_THRESHOLD(PARAM) ((4U <= (PARAM)) && ((PARAM) <= 31U))

#define IS_MDF_CIC_MODE(PARAM) (((PARAM) == MDF_TWO_FILTERS_MCIC_FASTSINC) || \
                                ((PARAM) == MDF_TWO_FILTERS_MCIC_SINC1)    || \
                                ((PARAM) == MDF_TWO_FILTERS_MCIC_SINC2)    || \
                                ((PARAM) == MDF_TWO_FILTERS_MCIC_SINC3)    || \
                                ((PARAM) == MDF_ONE_FILTER_SINC4)          || \
                                ((PARAM) == MDF_ONE_FILTER_SINC5))

#define IS_ADF_CIC_MODE(PARAM) (((PARAM) == MDF_ONE_FILTER_SINC4) || \
                                ((PARAM) == MDF_ONE_FILTER_SINC5))

#define IS_MDF_ACQUISITION_MODE(PARAM) (((PARAM) == MDF_MODE_ASYNC_CONT)   || \
                                        ((PARAM) == MDF_MODE_ASYNC_SINGLE) || \
                                        ((PARAM) == MDF_MODE_SYNC_CONT)    || \
                                        ((PARAM) == MDF_MODE_SYNC_SINGLE)  || \
                                        ((PARAM) == MDF_MODE_WINDOW_CONT)  || \
                                        ((PARAM) == MDF_MODE_SYNC_SNAPSHOT))

#define IS_ADF_ACQUISITION_MODE(PARAM) (((PARAM) == MDF_MODE_ASYNC_CONT)   || \
                                        ((PARAM) == MDF_MODE_ASYNC_SINGLE) || \
                                        ((PARAM) == MDF_MODE_SYNC_CONT)    || \
                                        ((PARAM) == MDF_MODE_SYNC_SINGLE)  || \
                                        ((PARAM) == MDF_MODE_WINDOW_CONT))

#define IS_MDF_DISCARD_SAMPLES(PARAM) ((PARAM) <= 255U)

#define IS_MDF_FIFO_THRESHOLD(PARAM) (((PARAM) == MDF_FIFO_THRESHOLD_NOT_EMPTY) || \
                                      ((PARAM) == MDF_FIFO_THRESHOLD_HALF_FULL))

#define IS_MDF_TRIGGER_SOURCE(PARAM) (((PARAM) == MDF_FILTER_TRIG_TRGO)        || \
                                      ((PARAM) == MDF_FILTER_TRIG_OLD_EVENT)   || \
                                      ((PARAM) == MDF_FILTER_TRIG_TIM1_TRGO)   || \
                                      ((PARAM) == MDF_FILTER_TRIG_TIM1_TRGO2)  || \
                                      ((PARAM) == MDF_FILTER_TRIG_TIM8_TRGO)   || \
                                      ((PARAM) == MDF_FILTER_TRIG_TIM8_TRGO2)  || \
                                      ((PARAM) == MDF_FILTER_TRIG_TIM3_TRGO)   || \
                                      ((PARAM) == MDF_FILTER_TRIG_TIM4_TRGO)   || \
                                      ((PARAM) == MDF_FILTER_TRIG_TIM6_TRGO)   || \
                                      ((PARAM) == MDF_FILTER_TRIG_TIM7_TRGO)   || \
                                      ((PARAM) == MDF_FILTER_TRIG_TIM15_TRGO)  || \
                                      ((PARAM) == MDF_FILTER_TRIG_ADF_SAD_DET) || \
                                      ((PARAM) == MDF_FILTER_TRIG_EXTI11)      || \
                                      ((PARAM) == MDF_FILTER_TRIG_EXTI15)      || \
                                      ((PARAM) == MDF_FILTER_TRIG_LPTIM1_OUT)  || \
                                      ((PARAM) == MDF_FILTER_TRIG_ADF_TRGO))

#define IS_ADF_TRIGGER_SOURCE(PARAM) (((PARAM) == MDF_FILTER_TRIG_TRGO) || \
                                      ((PARAM) == MDF_FILTER_TRIG_EXTI15))

#define IS_MDF_TRIGGER_EDGE(PARAM) (((PARAM) == MDF_FILTER_TRIG_RISING_EDGE) || \
                                    ((PARAM) == MDF_FILTER_TRIG_FALLING_EDGE))

#define IS_MDF_SNAPSHOT_FORMAT(PARAM) (((PARAM) == MDF_SNAPSHOT_23BITS) || \
                                       ((PARAM) == MDF_SNAPSHOT_16BITS))

#define IS_MDF_DATA_SOURCE(PARAM) (((PARAM) == MDF_DATA_SOURCE_BSMX)    || \
                                   ((PARAM) == MDF_DATA_SOURCE_ADCITF1) || \
                                   ((PARAM) == MDF_DATA_SOURCE_ADCITF2))

#define IS_ADF_DATA_SOURCE(PARAM) ((PARAM) == MDF_DATA_SOURCE_BSMX)

#define IS_MDF_DECIMATION_RATIO(PARAM) ((2U <= (PARAM)) && ((PARAM) <= 512U))

#define IS_MDF_GAIN(PARAM) ((-16 <= (PARAM)) && ((PARAM) <= 24))

#define IS_MDF_DELAY(PARAM) ((PARAM) <= 127U)

#define IS_MDF_OFFSET(PARAM) ((-33554432 <= (PARAM)) && ((PARAM) <= 33554431))

#define IS_MDF_RSF_DECIMATION_RATIO(PARAM) (((PARAM) == MDF_RSF_DECIMATION_RATIO_4) || \
                                            ((PARAM) == MDF_RSF_DECIMATION_RATIO_1))

#define IS_MDF_HPF_CUTOFF_FREQ(PARAM) (((PARAM) == MDF_HPF_CUTOFF_0_000625FPCM) || \
                                       ((PARAM) == MDF_HPF_CUTOFF_0_00125FPCM)  || \
                                       ((PARAM) == MDF_HPF_CUTOFF_0_0025FPCM)   || \
                                       ((PARAM) == MDF_HPF_CUTOFF_0_0095FPCM))

#define IS_MDF_INTEGRATOR_VALUE(PARAM) ((2U <= (PARAM)) && ((PARAM) <= 128U))

#define IS_MDF_INTEGRATOR_OUTPUT_DIV(PARAM) (((PARAM) == MDF_INTEGRATOR_OUTPUT_DIV_128) || \
                                             ((PARAM) == MDF_INTEGRATOR_OUTPUT_DIV_32)  || \
                                             ((PARAM) == MDF_INTEGRATOR_OUTPUT_DIV_4)   || \
                                             ((PARAM) == MDF_INTEGRATOR_OUTPUT_NO_DIV))

#define IS_MDF_SAD_MODE(PARAM) (((PARAM) == MDF_SAD_VOICE_ACTIVITY_DETECTOR) || \
                                ((PARAM) == MDF_SAD_SOUND_DETECTOR)          || \
                                ((PARAM) == MDF_SAD_AMBIENT_NOISE_DETECTOR))

#define IS_MDF_SAD_FRAME_SIZE(PARAM) (((PARAM) == MDF_SAD_8_PCM_SAMPLES)   || \
                                      ((PARAM) == MDF_SAD_16_PCM_SAMPLES)  || \
                                      ((PARAM) == MDF_SAD_32_PCM_SAMPLES)  || \
                                      ((PARAM) == MDF_SAD_64_PCM_SAMPLES)  || \
                                      ((PARAM) == MDF_SAD_128_PCM_SAMPLES) || \
                                      ((PARAM) == MDF_SAD_256_PCM_SAMPLES) || \
                                      ((PARAM) == MDF_SAD_512_PCM_SAMPLES))

#define IS_MDF_SAD_SOUND_TRIGGER(PARAM) (((PARAM) == MDF_SAD_ENTER_DETECT) || \
                                         ((PARAM) == MDF_SAD_ENTER_EXIT_DETECT))

#define IS_MDF_SAD_DATA_MEMORY_TRANSFER(PARAM) (((PARAM) == MDF_SAD_NO_MEMORY_TRANSFER)        || \
                                                ((PARAM) == MDF_SAD_MEMORY_TRANSFER_IN_DETECT) || \
                                                ((PARAM) == MDF_SAD_MEMORY_TRANSFER_ALWAYS))

#define IS_MDF_SAD_MIN_NOISE_LEVEL(PARAM) ((PARAM) <= 8191U)

#define IS_MDF_SAD_HANGOVER_WINDOW(PARAM) (((PARAM) == MDF_SAD_HANGOVER_4_FRAMES)   || \
                                           ((PARAM) == MDF_SAD_HANGOVER_8_FRAMES)   || \
                                           ((PARAM) == MDF_SAD_HANGOVER_16_FRAMES)  || \
                                           ((PARAM) == MDF_SAD_HANGOVER_32_FRAMES)  || \
                                           ((PARAM) == MDF_SAD_HANGOVER_64_FRAMES)  || \
                                           ((PARAM) == MDF_SAD_HANGOVER_128_FRAMES) || \
                                           ((PARAM) == MDF_SAD_HANGOVER_256_FRAMES) || \
                                           ((PARAM) == MDF_SAD_HANGOVER_512_FRAMES))

#define IS_MDF_SAD_LEARNING_FRAMES(PARAM) (((PARAM) == MDF_SAD_LEARNING_2_FRAMES)  || \
                                           ((PARAM) == MDF_SAD_LEARNING_4_FRAMES)  || \
                                           ((PARAM) == MDF_SAD_LEARNING_8_FRAMES)  || \
                                           ((PARAM) == MDF_SAD_LEARNING_16_FRAMES) || \
                                           ((PARAM) == MDF_SAD_LEARNING_32_FRAMES))

#define IS_MDF_SAD_AMBIENT_NOISE_SLOPE(PARAM) ((PARAM) <= 7U)

#define IS_MDF_SAD_SIGNAL_NOISE_THRESHOLD(PARAM) (((PARAM) == MDF_SAD_SIGNAL_NOISE_3_5DB)  || \
                                                  ((PARAM) == MDF_SAD_SIGNAL_NOISE_6DB)    || \
                                                  ((PARAM) == MDF_SAD_SIGNAL_NOISE_9_5DB)  || \
                                                  ((PARAM) == MDF_SAD_SIGNAL_NOISE_12DB)   || \
                                                  ((PARAM) == MDF_SAD_SIGNAL_NOISE_15_6DB) || \
                                                  ((PARAM) == MDF_SAD_SIGNAL_NOISE_18DB)   || \
                                                  ((PARAM) == MDF_SAD_SIGNAL_NOISE_21_6DB) || \
                                                  ((PARAM) == MDF_SAD_SIGNAL_NOISE_24_1DB) || \
                                                  ((PARAM) == MDF_SAD_SIGNAL_NOISE_27_6DB) || \
                                                  ((PARAM) == MDF_SAD_SIGNAL_NOISE_30_1DB))

#define IS_MDF_SCD_THRESHOLD(PARAM) ((2U <= (PARAM)) && ((PARAM) <= 256U))

#define IS_MDF_BREAK_SIGNAL(PARAM) ((PARAM) <= 15U)

#define IS_MDF_OLD_CIC_MODE(PARAM) (((PARAM) == MDF_OLD_FILTER_FASTSINC) || \
                                    ((PARAM) == MDF_OLD_FILTER_SINC1)    || \
                                    ((PARAM) == MDF_OLD_FILTER_SINC2)    || \
                                    ((PARAM) == MDF_OLD_FILTER_SINC3))

#define IS_MDF_OLD_DECIMATION_RATIO(PARAM) ((1U <= (PARAM)) && ((PARAM) <= 32U))

#define IS_MDF_OLD_THRESHOLD(PARAM) ((-33554432 <= (PARAM)) && ((PARAM) <= 33554431))

#define IS_MDF_OLD_EVENT_CONFIG(PARAM) (((PARAM) == MDF_OLD_SIGNAL_OUTSIDE_THRESHOLDS) || \
                                        ((PARAM) == MDF_OLD_SIGNAL_IN_THRESHOLDS))
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

#endif /* STM32N6xx_HAL_MDF_H */
