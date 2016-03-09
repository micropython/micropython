/**
  ******************************************************************************
  * @file    stm32l4xx_hal_dfsdm.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of DFSDM HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#ifndef __STM32L4xx_HAL_DFSDM_H
#define __STM32L4xx_HAL_DFSDM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup DFSDM
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/** @defgroup DFSDM_Exported_Types DFSDM Exported Types
  * @{
  */

/** 
  * @brief  HAL DFSDM Channel states definition  
  */ 
typedef enum
{
  HAL_DFSDM_CHANNEL_STATE_RESET = 0x00U, /*!< DFSDM channel not initialized */
  HAL_DFSDM_CHANNEL_STATE_READY = 0x01U, /*!< DFSDM channel initialized and ready for use */
  HAL_DFSDM_CHANNEL_STATE_ERROR = 0xFFU  /*!< DFSDM channel state error */
}HAL_DFSDM_Channel_StateTypeDef;

/** 
  * @brief  DFSDM channel output clock structure definition  
  */  
typedef struct
{
  FunctionalState Activation; /*!< Output clock enable/disable */
  uint32_t        Selection;  /*!< Output clock is system clock or audio clock.
                                   This parameter can be a value of @ref DFSDM_Channel_OuputClock */
  uint32_t        Divider;    /*!< Output clock divider.
                                   This parameter must be a number between Min_Data = 2 and Max_Data = 256 */
}DFSDM_Channel_OutputClockTypeDef;

/** 
  * @brief  DFSDM channel input structure definition  
  */  
typedef struct
{
  uint32_t Multiplexer; /*!< Input is external serial inputs or internal register.
                             This parameter can be a value of @ref DFSDM_Channel_InputMultiplexer */
  uint32_t DataPacking; /*!< Standard, interleaved or dual mode for internal register.
                             This parameter can be a value of @ref DFSDM_Channel_DataPacking */
  uint32_t Pins;        /*!< Input pins are taken from same or following channel.
                             This parameter can be a value of @ref DFSDM_Channel_InputPins */
}DFSDM_Channel_InputTypeDef;

/** 
  * @brief  DFSDM channel serial interface structure definition  
  */  
typedef struct
{
  uint32_t Type;     /*!< SPI or Manchester modes.
                          This parameter can be a value of @ref DFSDM_Channel_SerialInterfaceType */
  uint32_t SpiClock; /*!< SPI clock select (external or internal with different sampling point).
                          This parameter can be a value of @ref DFSDM_Channel_SpiClock */
}DFSDM_Channel_SerialInterfaceTypeDef;

/** 
  * @brief  DFSDM channel analog watchdog structure definition  
  */  
typedef struct
{
  uint32_t FilterOrder;  /*!< Analog watchdog Sinc filter order.
                              This parameter can be a value of @ref DFSDM_Channel_AwdFilterOrder */
  uint32_t Oversampling; /*!< Analog watchdog filter oversampling ratio.
                              This parameter must be a number between Min_Data = 1 and Max_Data = 32 */
}DFSDM_Channel_AwdTypeDef;

/** 
  * @brief  DFSDM channel init structure definition  
  */  
typedef struct
{
  DFSDM_Channel_OutputClockTypeDef     OutputClock;     /*!< DFSDM channel output clock parameters */
  DFSDM_Channel_InputTypeDef           Input;           /*!< DFSDM channel input parameters */
  DFSDM_Channel_SerialInterfaceTypeDef SerialInterface; /*!< DFSDM channel serial interface parameters */
  DFSDM_Channel_AwdTypeDef             Awd;             /*!< DFSDM channel analog watchdog parameters */
  int32_t                              Offset;          /*!< DFSDM channel offset.
                                                             This parameter must be a number between Min_Data = -8388608 and Max_Data = 8388607 */
  uint32_t                             RightBitShift;   /*!< DFSDM channel right bit shift.
                                                             This parameter must be a number between Min_Data = 0x00 and Max_Data = 0x1F */
}DFSDM_Channel_InitTypeDef;

/** 
  * @brief  DFSDM channel handle structure definition  
  */  
typedef struct
{
  DFSDM_Channel_TypeDef          *Instance; /*!< DFSDM channel instance */
  DFSDM_Channel_InitTypeDef      Init;      /*!< DFSDM channel init parameters */
  HAL_DFSDM_Channel_StateTypeDef State;     /*!< DFSDM channel state */
}DFSDM_Channel_HandleTypeDef;

/** 
  * @brief  HAL DFSDM Filter states definition  
  */ 
typedef enum
{
  HAL_DFSDM_FILTER_STATE_RESET   = 0x00U, /*!< DFSDM filter not initialized */
  HAL_DFSDM_FILTER_STATE_READY   = 0x01U, /*!< DFSDM filter initialized and ready for use */
  HAL_DFSDM_FILTER_STATE_REG     = 0x02U, /*!< DFSDM filter regular conversion in progress */
  HAL_DFSDM_FILTER_STATE_INJ     = 0x03U, /*!< DFSDM filter injected conversion in progress */
  HAL_DFSDM_FILTER_STATE_REG_INJ = 0x04U, /*!< DFSDM filter regular and injected conversions in progress */
  HAL_DFSDM_FILTER_STATE_ERROR   = 0xFFU  /*!< DFSDM filter state error */
}HAL_DFSDM_Filter_StateTypeDef;

/** 
  * @brief  DFSDM filter regular conversion parameters structure definition  
  */  
typedef struct
{
  uint32_t        Trigger;  /*!< Trigger used to start regular conversion: software or synchronous.
                                 This parameter can be a value of @ref DFSDM_Filter_Trigger */
  FunctionalState FastMode; /*!< Enable/disable fast mode for regular conversion */
  FunctionalState DmaMode;  /*!< Enable/disable DMA for regular conversion */
}DFSDM_Filter_RegularParamTypeDef;

/** 
  * @brief  DFSDM filter injected conversion parameters structure definition  
  */  
typedef struct
{
  uint32_t        Trigger;        /*!< Trigger used to start injected conversion: software, external or synchronous.
                                       This parameter can be a value of @ref DFSDM_Filter_Trigger */
  FunctionalState ScanMode;       /*!< Enable/disable scanning mode for injected conversion */
  FunctionalState DmaMode;        /*!< Enable/disable DMA for injected conversion */
  uint32_t        ExtTrigger;     /*!< External trigger.
                                       This parameter can be a value of @ref DFSDM_Filter_ExtTrigger */
  uint32_t        ExtTriggerEdge; /*!< External trigger edge: rising, falling or both.
                                       This parameter can be a value of @ref DFSDM_Filter_ExtTriggerEdge */
}DFSDM_Filter_InjectedParamTypeDef;

/** 
  * @brief  DFSDM filter parameters structure definition  
  */  
typedef struct
{
  uint32_t SincOrder;       /*!< Sinc filter order.
                                 This parameter can be a value of @ref DFSDM_Filter_SincOrder */
  uint32_t Oversampling;    /*!< Filter oversampling ratio.
                                 This parameter must be a number between Min_Data = 1 and Max_Data = 1024 */
  uint32_t IntOversampling; /*!< Integrator oversampling ratio.
                                 This parameter must be a number between Min_Data = 1 and Max_Data = 256 */
}DFSDM_Filter_FilterParamTypeDef;

/** 
  * @brief  DFSDM filter init structure definition  
  */  
typedef struct
{
  DFSDM_Filter_RegularParamTypeDef  RegularParam;  /*!< DFSDM regular conversion parameters */
  DFSDM_Filter_InjectedParamTypeDef InjectedParam; /*!< DFSDM injected conversion parameters */
  DFSDM_Filter_FilterParamTypeDef   FilterParam;   /*!< DFSDM filter parameters */
}DFSDM_Filter_InitTypeDef;

/** 
  * @brief  DFSDM filter handle structure definition  
  */  
typedef struct
{
  DFSDM_Filter_TypeDef          *Instance;           /*!< DFSDM filter instance */
  DFSDM_Filter_InitTypeDef      Init;                /*!< DFSDM filter init parameters */
  DMA_HandleTypeDef             *hdmaReg;            /*!< Pointer on DMA handler for regular conversions */
  DMA_HandleTypeDef             *hdmaInj;            /*!< Pointer on DMA handler for injected conversions */
  uint32_t                      RegularContMode;     /*!< Regular conversion continuous mode */
  uint32_t                      RegularTrigger;      /*!< Trigger used for regular conversion */
  uint32_t                      InjectedTrigger;     /*!< Trigger used for injected conversion */
  uint32_t                      ExtTriggerEdge;      /*!< Rising, falling or both edges selected */
  FunctionalState               InjectedScanMode;    /*!< Injected scanning mode */
  uint32_t                      InjectedChannelsNbr; /*!< Number of channels in injected sequence */
  uint32_t                      InjConvRemaining;    /*!< Injected conversions remaining */
  HAL_DFSDM_Filter_StateTypeDef State;               /*!< DFSDM filter state */
  uint32_t                      ErrorCode;           /*!< DFSDM filter error code */  
}DFSDM_Filter_HandleTypeDef;

/** 
  * @brief  DFSDM filter analog watchdog parameters structure definition  
  */  
typedef struct
{
  uint32_t DataSource;      /*!< Values from digital filter or from channel watchdog filter.
                                 This parameter can be a value of @ref DFSDM_Filter_AwdDataSource */
  uint32_t Channel;         /*!< Analog watchdog channel selection.
                                 This parameter can be a values combination of @ref DFSDM_Channel_Selection */
  int32_t  HighThreshold;   /*!< High threshold for the analog watchdog.
                                 This parameter must be a number between Min_Data = -8388608 and Max_Data = 8388607 */
  int32_t  LowThreshold;    /*!< Low threshold for the analog watchdog.
                                 This parameter must be a number between Min_Data = -8388608 and Max_Data = 8388607 */
  uint32_t HighBreakSignal; /*!< Break signal assigned to analog watchdog high threshold event.
                                 This parameter can be a values combination of @ref DFSDM_BreakSignals */
  uint32_t LowBreakSignal;  /*!< Break signal assigned to analog watchdog low threshold event.
                                 This parameter can be a values combination of @ref DFSDM_BreakSignals */
}DFSDM_Filter_AwdParamTypeDef;

/**
  * @}
  */ 
/* End of exported types -----------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup DFSDM_Exported_Constants DFSDM Exported Constants
  * @{
  */

/** @defgroup DFSDM_Channel_OuputClock DFSDM channel output clock selection
  * @{
  */
#define DFSDM_CHANNEL_OUTPUT_CLOCK_SYSTEM    ((uint32_t)0x00000000U) /*!< Source for ouput clock is system clock */
#define DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO     DFSDM_CHCFGR1_CKOUTSRC  /*!< Source for ouput clock is audio clock */
/**
  * @}
  */

/** @defgroup DFSDM_Channel_InputMultiplexer DFSDM channel input multiplexer
  * @{
  */
#define DFSDM_CHANNEL_EXTERNAL_INPUTS    ((uint32_t)0x00000000U) /*!< Data are taken from external inputs */
#define DFSDM_CHANNEL_INTERNAL_REGISTER  DFSDM_CHCFGR1_DATMPX_1  /*!< Data are taken from internal register */
/**
  * @}
  */

/** @defgroup DFSDM_Channel_DataPacking DFSDM channel input data packing
  * @{
  */
#define DFSDM_CHANNEL_STANDARD_MODE         ((uint32_t)0x00000000U) /*!< Standard data packing mode */
#define DFSDM_CHANNEL_INTERLEAVED_MODE      DFSDM_CHCFGR1_DATPACK_0 /*!< Interleaved data packing mode */
#define DFSDM_CHANNEL_DUAL_MODE             DFSDM_CHCFGR1_DATPACK_1 /*!< Dual data packing mode */
/**
  * @}
  */

/** @defgroup DFSDM_Channel_InputPins DFSDM channel input pins
  * @{
  */
#define DFSDM_CHANNEL_SAME_CHANNEL_PINS      ((uint32_t)0x00000000U) /*!< Input from pins on same channel */
#define DFSDM_CHANNEL_FOLLOWING_CHANNEL_PINS DFSDM_CHCFGR1_CHINSEL   /*!< Input from pins on following channel */
/**
  * @}
  */

/** @defgroup DFSDM_Channel_SerialInterfaceType DFSDM channel serial interface type
  * @{
  */
#define DFSDM_CHANNEL_SPI_RISING         ((uint32_t)0x00000000U) /*!< SPI with rising edge */
#define DFSDM_CHANNEL_SPI_FALLING        DFSDM_CHCFGR1_SITP_0    /*!< SPI with falling edge */
#define DFSDM_CHANNEL_MANCHESTER_RISING  DFSDM_CHCFGR1_SITP_1    /*!< Manchester with rising edge */
#define DFSDM_CHANNEL_MANCHESTER_FALLING DFSDM_CHCFGR1_SITP      /*!< Manchester with falling edge */
/**
  * @}
  */

/** @defgroup DFSDM_Channel_SpiClock DFSDM channel SPI clock selection
  * @{
  */
#define DFSDM_CHANNEL_SPI_CLOCK_EXTERNAL              ((uint32_t)0x00000000U)  /*!< External SPI clock */
#define DFSDM_CHANNEL_SPI_CLOCK_INTERNAL              DFSDM_CHCFGR1_SPICKSEL_0 /*!< Internal SPI clock */
#define DFSDM_CHANNEL_SPI_CLOCK_INTERNAL_DIV2_FALLING DFSDM_CHCFGR1_SPICKSEL_1 /*!< Internal SPI clock divided by 2, falling edge */
#define DFSDM_CHANNEL_SPI_CLOCK_INTERNAL_DIV2_RISING  DFSDM_CHCFGR1_SPICKSEL   /*!< Internal SPI clock divided by 2, rising edge */
/**
  * @}
  */

/** @defgroup DFSDM_Channel_AwdFilterOrder DFSDM channel analog watchdog filter order
  * @{
  */
#define DFSDM_CHANNEL_FASTSINC_ORDER ((uint32_t)0x00000000U) /*!< FastSinc filter type */
#define DFSDM_CHANNEL_SINC1_ORDER    DFSDM_AWSCDR_AWFORD_0   /*!< Sinc 1 filter type */
#define DFSDM_CHANNEL_SINC2_ORDER    DFSDM_AWSCDR_AWFORD_1   /*!< Sinc 2 filter type */
#define DFSDM_CHANNEL_SINC3_ORDER    DFSDM_AWSCDR_AWFORD     /*!< Sinc 3 filter type */
/**
  * @}
  */

/** @defgroup DFSDM_Filter_Trigger DFSDM filter conversion trigger
  * @{
  */
#define DFSDM_FILTER_SW_TRIGGER   ((uint32_t)0x00000000U) /*!< Software trigger */
#define DFSDM_FILTER_SYNC_TRIGGER ((uint32_t)0x00000001U) /*!< Synchronous with DFSDM0 */
#define DFSDM_FILTER_EXT_TRIGGER  ((uint32_t)0x00000002U) /*!< External trigger (only for injected conversion) */
/**
  * @}
  */

/** @defgroup DFSDM_Filter_ExtTrigger DFSDM filter external trigger
  * @{
  */
#define DFSDM_FILTER_EXT_TRIG_TIM1_TRGO  ((uint32_t)0x00000000U)                     /*!< For DFSDM 0, 1, 2 and 3 */
#define DFSDM_FILTER_EXT_TRIG_TIM1_TRGO2 DFSDM_CR1_JEXTSEL_0                         /*!< For DFSDM 0, 1, 2 and 3 */
#define DFSDM_FILTER_EXT_TRIG_TIM8_TRGO  DFSDM_CR1_JEXTSEL_1                         /*!< For DFSDM 0, 1, 2 and 3 */
#define DFSDM_FILTER_EXT_TRIG_TIM8_TRGO2 (DFSDM_CR1_JEXTSEL_0 | DFSDM_CR1_JEXTSEL_1) /*!< For DFSDM 0, 1 and 2 */
#define DFSDM_FILTER_EXT_TRIG_TIM3_TRGO  (DFSDM_CR1_JEXTSEL_0 | DFSDM_CR1_JEXTSEL_1) /*!< For DFSDM 3 */
#define DFSDM_FILTER_EXT_TRIG_TIM4_TRGO  DFSDM_CR1_JEXTSEL_2                         /*!< For DFSDM 0, 1 and 2 */
#define DFSDM_FILTER_EXT_TRIG_TIM16_OC1  DFSDM_CR1_JEXTSEL_2                         /*!< For DFSDM 3 */
#define DFSDM_FILTER_EXT_TRIG_TIM6_TRGO  (DFSDM_CR1_JEXTSEL_0 | DFSDM_CR1_JEXTSEL_2) /*!< For DFSDM 0 and 1 */
#define DFSDM_FILTER_EXT_TRIG_TIM7_TRGO  (DFSDM_CR1_JEXTSEL_0 | DFSDM_CR1_JEXTSEL_2) /*!< For DFSDM 2 and 3 */
#define DFSDM_FILTER_EXT_TRIG_EXTI11     (DFSDM_CR1_JEXTSEL_1 | DFSDM_CR1_JEXTSEL_2) /*!< For DFSDM 0, 1, 2 and 3 */
#define DFSDM_FILTER_EXT_TRIG_EXTI15     DFSDM_CR1_JEXTSEL                           /*!< For DFSDM 0, 1, 2 and 3 */
/**
  * @}
  */

/** @defgroup DFSDM_Filter_ExtTriggerEdge DFSDM filter external trigger edge
  * @{
  */
#define DFSDM_FILTER_EXT_TRIG_RISING_EDGE  DFSDM_CR1_JEXTEN_0 /*!< External rising edge */
#define DFSDM_FILTER_EXT_TRIG_FALLING_EDGE DFSDM_CR1_JEXTEN_1 /*!< External falling edge */
#define DFSDM_FILTER_EXT_TRIG_BOTH_EDGES   DFSDM_CR1_JEXTEN   /*!< External rising and falling edges */
/**
  * @}
  */

/** @defgroup DFSDM_Filter_SincOrder DFSDM filter sinc order
  * @{
  */
#define DFSDM_FILTER_FASTSINC_ORDER ((uint32_t)0x00000000U)               /*!< FastSinc filter type */
#define DFSDM_FILTER_SINC1_ORDER    DFSDM_FCR_FORD_0                      /*!< Sinc 1 filter type */
#define DFSDM_FILTER_SINC2_ORDER    DFSDM_FCR_FORD_1                      /*!< Sinc 2 filter type */
#define DFSDM_FILTER_SINC3_ORDER    (DFSDM_FCR_FORD_0 | DFSDM_FCR_FORD_1) /*!< Sinc 3 filter type */
#define DFSDM_FILTER_SINC4_ORDER    DFSDM_FCR_FORD_2                      /*!< Sinc 4 filter type */
#define DFSDM_FILTER_SINC5_ORDER    (DFSDM_FCR_FORD_0 | DFSDM_FCR_FORD_2) /*!< Sinc 5 filter type */
/**
  * @}
  */

/** @defgroup DFSDM_Filter_AwdDataSource DFSDM filter analog watchdog data source
  * @{
  */
#define DFSDM_FILTER_AWD_FILTER_DATA  ((uint32_t)0x00000000U) /*!< From digital filter */
#define DFSDM_FILTER_AWD_CHANNEL_DATA DFSDM_CR1_AWFSEL        /*!< From analog watchdog channel */
/**
  * @}
  */

/** @defgroup DFSDM_Filter_ErrorCode DFSDM filter error code
  * @{
  */ 
#define DFSDM_FILTER_ERROR_NONE             ((uint32_t)0x00000000U) /*!< No error */
#define DFSDM_FILTER_ERROR_REGULAR_OVERRUN  ((uint32_t)0x00000001U) /*!< Overrun occurs during regular conversion */
#define DFSDM_FILTER_ERROR_INJECTED_OVERRUN ((uint32_t)0x00000002U) /*!< Overrun occurs during injected conversion */
#define DFSDM_FILTER_ERROR_DMA              ((uint32_t)0x00000003U) /*!< DMA error occurs */
/**
  * @}
  */

/** @defgroup DFSDM_BreakSignals DFSDM break signals
  * @{
  */
#define DFSDM_NO_BREAK_SIGNAL ((uint32_t)0x00000000U) /*!< No break signal */
#define DFSDM_BREAK_SIGNAL_0  ((uint32_t)0x00000001U) /*!< Break signal 0 */
#define DFSDM_BREAK_SIGNAL_1  ((uint32_t)0x00000002U) /*!< Break signal 1 */
#define DFSDM_BREAK_SIGNAL_2  ((uint32_t)0x00000004U) /*!< Break signal 2 */
#define DFSDM_BREAK_SIGNAL_3  ((uint32_t)0x00000008U) /*!< Break signal 3 */
/**
  * @}
  */

/** @defgroup DFSDM_Channel_Selection DFSDM Channel Selection
  * @{
  */
/* DFSDM Channels ------------------------------------------------------------*/
/* The DFSDM channels are defined as follows:
   - in 16-bit LSB the channel mask is set
   - in 16-bit MSB the channel number is set
   e.g. for channel 5 definition:
        - the channel mask is 0x00000020 (bit 5 is set)
        - the channel number 5 is 0x00050000 
        --> Consequently, channel 5 definition is 0x00000020 | 0x00050000 = 0x00050020 */
#define DFSDM_CHANNEL_0                              ((uint32_t)0x00000001U)
#define DFSDM_CHANNEL_1                              ((uint32_t)0x00010002U)
#define DFSDM_CHANNEL_2                              ((uint32_t)0x00020004U)
#define DFSDM_CHANNEL_3                              ((uint32_t)0x00030008U)
#define DFSDM_CHANNEL_4                              ((uint32_t)0x00040010U)
#define DFSDM_CHANNEL_5                              ((uint32_t)0x00050020U)
#define DFSDM_CHANNEL_6                              ((uint32_t)0x00060040U)
#define DFSDM_CHANNEL_7                              ((uint32_t)0x00070080U)
/**
  * @}
  */

/** @defgroup DFSDM_ContinuousMode DFSDM Continuous Mode
  * @{
  */
#define DFSDM_CONTINUOUS_CONV_OFF            ((uint32_t)0x00000000U) /*!< Conversion are not continuous */
#define DFSDM_CONTINUOUS_CONV_ON             ((uint32_t)0x00000001U) /*!< Conversion are continuous */
/**
  * @}
  */

/** @defgroup DFSDM_AwdThreshold DFSDM analog watchdog threshold
  * @{
  */
#define DFSDM_AWD_HIGH_THRESHOLD            ((uint32_t)0x00000000U) /*!< Analog watchdog high threshold */
#define DFSDM_AWD_LOW_THRESHOLD             ((uint32_t)0x00000001U) /*!< Analog watchdog low threshold */
/**
  * @}
  */

/**
  * @}
  */ 
/* End of exported constants -------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/** @defgroup DFSDM_Exported_Macros DFSDM Exported Macros
 * @{
 */

/** @brief  Reset DFSDM channel handle state.
  * @param  __HANDLE__: DFSDM channel handle.
  * @retval None
  */
#define __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_DFSDM_CHANNEL_STATE_RESET)

/** @brief  Reset DFSDM filter handle state.
  * @param  __HANDLE__: DFSDM filter handle.
  * @retval None
  */
#define __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_DFSDM_FILTER_STATE_RESET)

/**
  * @}
  */
/* End of exported macros ----------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup DFSDM_Exported_Functions DFSDM Exported Functions
  * @{
  */

/** @addtogroup DFSDM_Exported_Functions_Group1_Channel Channel initialization and de-initialization functions
  * @{
  */
/* Channel initialization and de-initialization functions *********************/
HAL_StatusTypeDef HAL_DFSDM_ChannelInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
HAL_StatusTypeDef HAL_DFSDM_ChannelDeInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
void HAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
void HAL_DFSDM_ChannelMspDeInit(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
/**
  * @}
  */

/** @addtogroup DFSDM_Exported_Functions_Group2_Channel Channel operation functions
  * @{
  */
/* Channel operation functions ************************************************/
HAL_StatusTypeDef HAL_DFSDM_ChannelCkabStart(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
HAL_StatusTypeDef HAL_DFSDM_ChannelCkabStart_IT(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
HAL_StatusTypeDef HAL_DFSDM_ChannelCkabStop(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
HAL_StatusTypeDef HAL_DFSDM_ChannelCkabStop_IT(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);

HAL_StatusTypeDef HAL_DFSDM_ChannelScdStart(DFSDM_Channel_HandleTypeDef *hdfsdm_channel, uint32_t Threshold, uint32_t BreakSignal);
HAL_StatusTypeDef HAL_DFSDM_ChannelScdStart_IT(DFSDM_Channel_HandleTypeDef *hdfsdm_channel, uint32_t Threshold, uint32_t BreakSignal);
HAL_StatusTypeDef HAL_DFSDM_ChannelScdStop(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
HAL_StatusTypeDef HAL_DFSDM_ChannelScdStop_IT(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);

int16_t           HAL_DFSDM_ChannelGetAwdValue(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
HAL_StatusTypeDef HAL_DFSDM_ChannelModifyOffset(DFSDM_Channel_HandleTypeDef *hdfsdm_channel, int32_t Offset);

HAL_StatusTypeDef HAL_DFSDM_ChannelPollForCkab(DFSDM_Channel_HandleTypeDef *hdfsdm_channel, uint32_t Timeout);
HAL_StatusTypeDef HAL_DFSDM_ChannelPollForScd(DFSDM_Channel_HandleTypeDef *hdfsdm_channel, uint32_t Timeout);

void HAL_DFSDM_ChannelCkabCallback(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
void HAL_DFSDM_ChannelScdCallback(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
/**
  * @}
  */

/** @defgroup DFSDM_Exported_Functions_Group3_Channel Channel state function
  * @{
  */
/* Channel state function *****************************************************/
HAL_DFSDM_Channel_StateTypeDef HAL_DFSDM_ChannelGetState(DFSDM_Channel_HandleTypeDef *hdfsdm_channel);
/**
  * @}
  */

/** @addtogroup DFSDM_Exported_Functions_Group1_Filter Filter initialization and de-initialization functions
  * @{
  */
/* Filter initialization and de-initialization functions *********************/
HAL_StatusTypeDef HAL_DFSDM_FilterInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterDeInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
void HAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
void HAL_DFSDM_FilterMspDeInit(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
/**
  * @}
  */

/** @addtogroup DFSDM_Exported_Functions_Group2_Filter Filter control functions
  * @{
  */
/* Filter control functions *********************/
HAL_StatusTypeDef HAL_DFSDM_FilterConfigRegChannel(DFSDM_Filter_HandleTypeDef *hdfsdm_filter,
                                                   uint32_t                    Channel,
                                                   uint32_t                    ContinuousMode);
HAL_StatusTypeDef HAL_DFSDM_FilterConfigInjChannel(DFSDM_Filter_HandleTypeDef *hdfsdm_filter,
                                                   uint32_t                    Channel);
/**
  * @}
  */

/** @addtogroup DFSDM_Exported_Functions_Group3_Filter Filter operation functions
  * @{
  */
/* Filter operation functions *********************/
HAL_StatusTypeDef HAL_DFSDM_FilterRegularStart(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterRegularStart_IT(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterRegularStart_DMA(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, int32_t *pData, uint32_t Length);
HAL_StatusTypeDef HAL_DFSDM_FilterRegularMsbStart_DMA(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, int16_t *pData, uint32_t Length);
HAL_StatusTypeDef HAL_DFSDM_FilterRegularStop(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterRegularStop_IT(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterRegularStop_DMA(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterInjectedStart(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterInjectedStart_IT(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterInjectedStart_DMA(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, int32_t *pData, uint32_t Length);
HAL_StatusTypeDef HAL_DFSDM_FilterInjectedMsbStart_DMA(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, int16_t *pData, uint32_t Length);
HAL_StatusTypeDef HAL_DFSDM_FilterInjectedStop(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterInjectedStop_IT(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterInjectedStop_DMA(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterAwdStart_IT(DFSDM_Filter_HandleTypeDef *hdfsdm_filter,
                                              DFSDM_Filter_AwdParamTypeDef* awdParam);
HAL_StatusTypeDef HAL_DFSDM_FilterAwdStop_IT(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
HAL_StatusTypeDef HAL_DFSDM_FilterExdStart(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, uint32_t Channel);
HAL_StatusTypeDef HAL_DFSDM_FilterExdStop(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);

int32_t  HAL_DFSDM_FilterGetRegularValue(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, uint32_t* Channel);
int32_t  HAL_DFSDM_FilterGetInjectedValue(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, uint32_t* Channel);
int32_t  HAL_DFSDM_FilterGetExdMaxValue(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, uint32_t* Channel);
int32_t  HAL_DFSDM_FilterGetExdMinValue(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, uint32_t* Channel);
uint32_t HAL_DFSDM_FilterGetConvTimeValue(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);

void HAL_DFSDM_IRQHandler(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);

HAL_StatusTypeDef HAL_DFSDM_FilterPollForRegConversion(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, uint32_t Timeout);
HAL_StatusTypeDef HAL_DFSDM_FilterPollForInjConversion(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, uint32_t Timeout);

void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
void HAL_DFSDM_FilterInjConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
void HAL_DFSDM_FilterInjConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
void HAL_DFSDM_FilterAwdCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter, uint32_t Channel, uint32_t Threshold);
void HAL_DFSDM_FilterErrorCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
/**
  * @}
  */

/** @defgroup DFSDM_Exported_Functions_Group4_Filter Filter state functions
  * @{
  */
/* Filter state functions *****************************************************/
HAL_DFSDM_Filter_StateTypeDef HAL_DFSDM_FilterGetState(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
uint32_t                      HAL_DFSDM_FilterGetError(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);
/**
  * @}
  */

/**
  * @}
  */
/* End of exported functions -------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
/** @defgroup DFSDM_Private_Macros DFSDM Private Macros
* @{
*/
#define IS_DFSDM_CHANNEL_OUTPUT_CLOCK(CLOCK)          (((CLOCK) == DFSDM_CHANNEL_OUTPUT_CLOCK_SYSTEM) || \
                                                       ((CLOCK) == DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO))
#define IS_DFSDM_CHANNEL_OUTPUT_CLOCK_DIVIDER(DIVIDER) ((2 <= (DIVIDER)) && ((DIVIDER) <= 256))
#define IS_DFSDM_CHANNEL_INPUT(INPUT)                 (((INPUT) == DFSDM_CHANNEL_EXTERNAL_INPUTS) || \
                                                       ((INPUT) == DFSDM_CHANNEL_INTERNAL_REGISTER))
#define IS_DFSDM_CHANNEL_DATA_PACKING(MODE)           (((MODE) == DFSDM_CHANNEL_STANDARD_MODE) || \
                                                       ((MODE) == DFSDM_CHANNEL_INTERLEAVED_MODE) || \
                                                       ((MODE) == DFSDM_CHANNEL_DUAL_MODE))
#define IS_DFSDM_CHANNEL_INPUT_PINS(PINS)             (((PINS) == DFSDM_CHANNEL_SAME_CHANNEL_PINS) || \
                                                       ((PINS) == DFSDM_CHANNEL_FOLLOWING_CHANNEL_PINS))
#define IS_DFSDM_CHANNEL_SERIAL_INTERFACE_TYPE(MODE)  (((MODE) == DFSDM_CHANNEL_SPI_RISING) || \
                                                       ((MODE) == DFSDM_CHANNEL_SPI_FALLING) || \
                                                       ((MODE) == DFSDM_CHANNEL_MANCHESTER_RISING) || \
                                                       ((MODE) == DFSDM_CHANNEL_MANCHESTER_FALLING))
#define IS_DFSDM_CHANNEL_SPI_CLOCK(TYPE)              (((TYPE) == DFSDM_CHANNEL_SPI_CLOCK_EXTERNAL) || \
                                                       ((TYPE) == DFSDM_CHANNEL_SPI_CLOCK_INTERNAL) || \
                                                       ((TYPE) == DFSDM_CHANNEL_SPI_CLOCK_INTERNAL_DIV2_FALLING) || \
                                                       ((TYPE) == DFSDM_CHANNEL_SPI_CLOCK_INTERNAL_DIV2_RISING))
#define IS_DFSDM_CHANNEL_FILTER_ORDER(ORDER)          (((ORDER) == DFSDM_CHANNEL_FASTSINC_ORDER) || \
                                                       ((ORDER) == DFSDM_CHANNEL_SINC1_ORDER) || \
                                                       ((ORDER) == DFSDM_CHANNEL_SINC2_ORDER) || \
                                                       ((ORDER) == DFSDM_CHANNEL_SINC3_ORDER))
#define IS_DFSDM_CHANNEL_FILTER_OVS_RATIO(RATIO)       ((1 <= (RATIO)) && ((RATIO) <= 32))
#define IS_DFSDM_CHANNEL_OFFSET(VALUE)                 ((-8388608 <= (VALUE)) && ((VALUE) <= 8388607))
#define IS_DFSDM_CHANNEL_RIGHT_BIT_SHIFT(VALUE)        ((VALUE) <= 0x1F)
#define IS_DFSDM_CHANNEL_SCD_THRESHOLD(VALUE)          ((VALUE) <= 0xFF)
#define IS_DFSDM_FILTER_REG_TRIGGER(TRIG)             (((TRIG) == DFSDM_FILTER_SW_TRIGGER) || \
                                                       ((TRIG) == DFSDM_FILTER_SYNC_TRIGGER))
#define IS_DFSDM_FILTER_INJ_TRIGGER(TRIG)             (((TRIG) == DFSDM_FILTER_SW_TRIGGER) || \
                                                       ((TRIG) == DFSDM_FILTER_SYNC_TRIGGER) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIGGER))
#define IS_DFSDM_FILTER_EXT_TRIG(TRIG)                (((TRIG) == DFSDM_FILTER_EXT_TRIG_TIM1_TRGO) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_TIM1_TRGO2) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_TIM8_TRGO) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_TIM8_TRGO2) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_TIM3_TRGO) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_TIM4_TRGO) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_TIM16_OC1) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_TIM6_TRGO) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_TIM7_TRGO) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_EXTI11) || \
                                                       ((TRIG) == DFSDM_FILTER_EXT_TRIG_EXTI15))
#define IS_DFSDM_FILTER_EXT_TRIG_EDGE(EDGE)           (((EDGE) == DFSDM_FILTER_EXT_TRIG_RISING_EDGE)  || \
                                                       ((EDGE) == DFSDM_FILTER_EXT_TRIG_FALLING_EDGE)  || \
                                                       ((EDGE) == DFSDM_FILTER_EXT_TRIG_BOTH_EDGES))
#define IS_DFSDM_FILTER_SINC_ORDER(ORDER)             (((ORDER) == DFSDM_FILTER_FASTSINC_ORDER) || \
                                                       ((ORDER) == DFSDM_FILTER_SINC1_ORDER) || \
                                                       ((ORDER) == DFSDM_FILTER_SINC2_ORDER) || \
                                                       ((ORDER) == DFSDM_FILTER_SINC3_ORDER) || \
                                                       ((ORDER) == DFSDM_FILTER_SINC4_ORDER) || \
                                                       ((ORDER) == DFSDM_FILTER_SINC5_ORDER))
#define IS_DFSDM_FILTER_OVS_RATIO(RATIO)               ((1 <= (RATIO)) && ((RATIO) <= 1024))
#define IS_DFSDM_FILTER_INTEGRATOR_OVS_RATIO(RATIO)    ((1 <= (RATIO)) && ((RATIO) <= 256))
#define IS_DFSDM_FILTER_AWD_DATA_SOURCE(DATA)         (((DATA) == DFSDM_FILTER_AWD_FILTER_DATA)  || \
                                                       ((DATA) == DFSDM_FILTER_AWD_CHANNEL_DATA))
#define IS_DFSDM_FILTER_AWD_THRESHOLD(VALUE)           ((-8388608 <= (VALUE)) && ((VALUE) <= 8388607))
#define IS_DFSDM_BREAK_SIGNALS(VALUE)                  ((VALUE) <= 0xFU)
#define IS_DFSDM_REGULAR_CHANNEL(CHANNEL)             (((CHANNEL) == DFSDM_CHANNEL_0)  || \
                                                       ((CHANNEL) == DFSDM_CHANNEL_1)  || \
                                                       ((CHANNEL) == DFSDM_CHANNEL_2)  || \
                                                       ((CHANNEL) == DFSDM_CHANNEL_3)  || \
                                                       ((CHANNEL) == DFSDM_CHANNEL_4)  || \
                                                       ((CHANNEL) == DFSDM_CHANNEL_5)  || \
                                                       ((CHANNEL) == DFSDM_CHANNEL_6)  || \
                                                       ((CHANNEL) == DFSDM_CHANNEL_7))
#define IS_DFSDM_INJECTED_CHANNEL(CHANNEL)            (((CHANNEL) != 0) && ((CHANNEL) <= 0x000F00FFU))
#define IS_DFSDM_CONTINUOUS_MODE(MODE)                (((MODE) == DFSDM_CONTINUOUS_CONV_OFF)  || \
                                                       ((MODE) == DFSDM_CONTINUOUS_CONV_ON))
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
#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_HAL_DFSDM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
