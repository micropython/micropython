/**
  ******************************************************************************
  * @file    stm32f4xx_hal_rtc_ex.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Header file of RTC HAL Extension module.
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
#ifndef __STM32F4xx_HAL_RTC_EX_H
#define __STM32F4xx_HAL_RTC_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup RTCEx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** 
  * @brief  RTC Tamper structure definition  
  */
typedef struct 
{
  uint32_t Tamper;                      /*!< Specifies the Tamper Pin.
                                             This parameter can be a value of @ref  RTCEx_Tamper_Pins_Definitions */
  
  uint32_t PinSelection;                /*!< Specifies the Tamper Pin.
                                             This parameter can be a value of @ref  RTCEx_Tamper_Pins_Selection */                                        
                                             
  uint32_t Trigger;                     /*!< Specifies the Tamper Trigger.
                                             This parameter can be a value of @ref  RTCEx_Tamper_Trigger_Definitions */

  uint32_t Filter;                      /*!< Specifies the RTC Filter Tamper.
                                             This parameter can be a value of @ref RTCEx_Tamper_Filter_Definitions */
  
  uint32_t SamplingFrequency;           /*!< Specifies the sampling frequency.
                                             This parameter can be a value of @ref RTCEx_Tamper_Sampling_Frequencies_Definitions */
                                      
  uint32_t PrechargeDuration;           /*!< Specifies the Precharge Duration .
                                             This parameter can be a value of @ref RTCEx_Tamper_Pin_Precharge_Duration_Definitions */ 
 
  uint32_t TamperPullUp;                /*!< Specifies the Tamper PullUp .
                                             This parameter can be a value of @ref RTCEx_Tamper_Pull_UP_Definitions */           
 
  uint32_t TimeStampOnTamperDetection;  /*!< Specifies the TimeStampOnTamperDetection.
                                             This parameter can be a value of @ref RTCEx_Tamper_TimeStampOnTamperDetection_Definitions */                      
}RTC_TamperTypeDef;

/* Exported constants --------------------------------------------------------*/
/** @defgroup RTCEx_Exported_Constants
  * @{
  */ 

/** @defgroup RTCEx_Backup_Registers_Definitions 
  * @{
  */
#define RTC_BKP_DR0                       ((uint32_t)0x00000000)
#define RTC_BKP_DR1                       ((uint32_t)0x00000001)
#define RTC_BKP_DR2                       ((uint32_t)0x00000002)
#define RTC_BKP_DR3                       ((uint32_t)0x00000003)
#define RTC_BKP_DR4                       ((uint32_t)0x00000004)
#define RTC_BKP_DR5                       ((uint32_t)0x00000005)
#define RTC_BKP_DR6                       ((uint32_t)0x00000006)
#define RTC_BKP_DR7                       ((uint32_t)0x00000007)
#define RTC_BKP_DR8                       ((uint32_t)0x00000008)
#define RTC_BKP_DR9                       ((uint32_t)0x00000009)
#define RTC_BKP_DR10                      ((uint32_t)0x0000000A)
#define RTC_BKP_DR11                      ((uint32_t)0x0000000B)
#define RTC_BKP_DR12                      ((uint32_t)0x0000000C)
#define RTC_BKP_DR13                      ((uint32_t)0x0000000D)
#define RTC_BKP_DR14                      ((uint32_t)0x0000000E)
#define RTC_BKP_DR15                      ((uint32_t)0x0000000F)
#define RTC_BKP_DR16                      ((uint32_t)0x00000010)
#define RTC_BKP_DR17                      ((uint32_t)0x00000011)
#define RTC_BKP_DR18                      ((uint32_t)0x00000012)
#define RTC_BKP_DR19                      ((uint32_t)0x00000013)

#define IS_RTC_BKP(BKP)                   (((BKP) == RTC_BKP_DR0)  || \
                                           ((BKP) == RTC_BKP_DR1)  || \
                                           ((BKP) == RTC_BKP_DR2)  || \
                                           ((BKP) == RTC_BKP_DR3)  || \
                                           ((BKP) == RTC_BKP_DR4)  || \
                                           ((BKP) == RTC_BKP_DR5)  || \
                                           ((BKP) == RTC_BKP_DR6)  || \
                                           ((BKP) == RTC_BKP_DR7)  || \
                                           ((BKP) == RTC_BKP_DR8)  || \
                                           ((BKP) == RTC_BKP_DR9)  || \
                                           ((BKP) == RTC_BKP_DR10) || \
                                           ((BKP) == RTC_BKP_DR11) || \
                                           ((BKP) == RTC_BKP_DR12) || \
                                           ((BKP) == RTC_BKP_DR13) || \
                                           ((BKP) == RTC_BKP_DR14) || \
                                           ((BKP) == RTC_BKP_DR15) || \
                                           ((BKP) == RTC_BKP_DR16) || \
                                           ((BKP) == RTC_BKP_DR17) || \
                                           ((BKP) == RTC_BKP_DR18) || \
                                           ((BKP) == RTC_BKP_DR19))
/**
  * @}
  */ 

/** @defgroup RTCEx_Time_Stamp_Edges_definitions 
  * @{
  */ 
#define RTC_TIMESTAMPEDGE_RISING          ((uint32_t)0x00000000)
#define RTC_TIMESTAMPEDGE_FALLING         ((uint32_t)0x00000008)

#define IS_TIMESTAMP_EDGE(EDGE) (((EDGE) == RTC_TIMESTAMPEDGE_RISING) || \
                                 ((EDGE) == RTC_TIMESTAMPEDGE_FALLING))
/**
  * @}
  */
  
/** @defgroup RTCEx_Tamper_Pins_Definitions 
  * @{
  */ 
#define RTC_TAMPER_1                    RTC_TAFCR_TAMP1E
#define RTC_TAMPER_2                    RTC_TAFCR_TAMP2E

#define IS_TAMPER(TAMPER) ((((TAMPER) & (uint32_t)0xFFFFFFF6) == 0x00) && ((TAMPER) != (uint32_t)RESET))
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Pins_Selection 
  * @{
  */ 
#define RTC_TAMPERPIN_PC13                 ((uint32_t)0x00000000)
#define RTC_TAMPERPIN_PI8                  ((uint32_t)0x00010000)

#define IS_RTC_TAMPER_PIN(PIN) (((PIN) == RTC_TAMPERPIN_PC13) || \
                                ((PIN) == RTC_TAMPERPIN_PI8))
/**
  * @}
  */ 

/** @defgroup RTCEx_TimeStamp_Pin_Selection 
  * @{
  */ 
#define RTC_TIMESTAMPPIN_PC13              ((uint32_t)0x00000000)
#define RTC_TIMESTAMPPIN_PI8               ((uint32_t)0x00020000)

#define IS_RTC_TIMESTAMP_PIN(PIN) (((PIN) == RTC_TIMESTAMPPIN_PC13) || \
                                   ((PIN) == RTC_TIMESTAMPPIN_PI8))
/**
  * @}
  */ 

/** @defgroup RTCEx_Tamper_Trigger_Definitions 
  * @{
  */ 
#define RTC_TAMPERTRIGGER_RISINGEDGE       ((uint32_t)0x00000000)
#define RTC_TAMPERTRIGGER_FALLINGEDGE      ((uint32_t)0x00000002)
#define RTC_TAMPERTRIGGER_LOWLEVEL         RTC_TAMPERTRIGGER_RISINGEDGE
#define RTC_TAMPERTRIGGER_HIGHLEVEL        RTC_TAMPERTRIGGER_FALLINGEDGE

#define IS_TAMPER_TRIGGER(TRIGGER) (((TRIGGER) == RTC_TAMPERTRIGGER_RISINGEDGE) || \
                                        ((TRIGGER) == RTC_TAMPERTRIGGER_FALLINGEDGE) || \
                                        ((TRIGGER) == RTC_TAMPERTRIGGER_LOWLEVEL) || \
                                        ((TRIGGER) == RTC_TAMPERTRIGGER_HIGHLEVEL)) 

/**
  * @}
  */  

/** @defgroup RTCEx_Tamper_Filter_Definitions 
  * @{
  */ 
#define RTC_TAMPERFILTER_DISABLE   ((uint32_t)0x00000000)  /*!< Tamper filter is disabled */

#define RTC_TAMPERFILTER_2SAMPLE   ((uint32_t)0x00000800)  /*!< Tamper is activated after 2 
                                                                consecutive samples at the active level */
#define RTC_TAMPERFILTER_4SAMPLE   ((uint32_t)0x00001000)  /*!< Tamper is activated after 4 
                                                                consecutive samples at the active level */
#define RTC_TAMPERFILTER_8SAMPLE   ((uint32_t)0x00001800)  /*!< Tamper is activated after 8 
                                                                consecutive samples at the active leve. */

#define IS_TAMPER_FILTER(FILTER)  (((FILTER) == RTC_TAMPERFILTER_DISABLE) || \
                                   ((FILTER) == RTC_TAMPERFILTER_2SAMPLE) || \
                                   ((FILTER) == RTC_TAMPERFILTER_4SAMPLE) || \
                                   ((FILTER) == RTC_TAMPERFILTER_8SAMPLE))
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Sampling_Frequencies_Definitions 
  * @{
  */ 
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768  ((uint32_t)0x00000000)  /*!< Each of the tamper inputs are sampled
                                                                             with a frequency =  RTCCLK / 32768 */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV16384  ((uint32_t)0x00000100)  /*!< Each of the tamper inputs are sampled
                                                                             with a frequency =  RTCCLK / 16384 */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV8192   ((uint32_t)0x00000200)  /*!< Each of the tamper inputs are sampled
                                                                             with a frequency =  RTCCLK / 8192  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV4096   ((uint32_t)0x00000300)  /*!< Each of the tamper inputs are sampled
                                                                             with a frequency =  RTCCLK / 4096  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV2048   ((uint32_t)0x00000400)  /*!< Each of the tamper inputs are sampled
                                                                             with a frequency =  RTCCLK / 2048  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV1024   ((uint32_t)0x00000500)  /*!< Each of the tamper inputs are sampled
                                                                             with a frequency =  RTCCLK / 1024  */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV512    ((uint32_t)0x00000600)  /*!< Each of the tamper inputs are sampled
                                                                             with a frequency =  RTCCLK / 512   */
#define RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV256    ((uint32_t)0x00000700)  /*!< Each of the tamper inputs are sampled
                                                                             with a frequency =  RTCCLK / 256   */

#define IS_TAMPER_SAMPLING_FREQ(FREQ) (((FREQ) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768)|| \
                                       ((FREQ) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV16384)|| \
                                       ((FREQ) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV8192) || \
                                       ((FREQ) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV4096) || \
                                       ((FREQ) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV2048) || \
                                       ((FREQ) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV1024) || \
                                       ((FREQ) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV512)  || \
                                       ((FREQ) == RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV256))
/**
  * @}
  */

/** @defgroup RTCEx_Tamper_Pin_Precharge_Duration_Definitions 
  * @{
  */ 
#define RTC_TAMPERPRECHARGEDURATION_1RTCCLK ((uint32_t)0x00000000)  /*!< Tamper pins are pre-charged before 
                                                                         sampling during 1 RTCCLK cycle */
#define RTC_TAMPERPRECHARGEDURATION_2RTCCLK ((uint32_t)0x00002000)  /*!< Tamper pins are pre-charged before 
                                                                         sampling during 2 RTCCLK cycles */
#define RTC_TAMPERPRECHARGEDURATION_4RTCCLK ((uint32_t)0x00004000)  /*!< Tamper pins are pre-charged before 
                                                                         sampling during 4 RTCCLK cycles */
#define RTC_TAMPERPRECHARGEDURATION_8RTCCLK ((uint32_t)0x00006000)  /*!< Tamper pins are pre-charged before 
                                                                         sampling during 8 RTCCLK cycles */

#define IS_TAMPER_PRECHARGE_DURATION(DURATION) (((DURATION) == RTC_TAMPERPRECHARGEDURATION_1RTCCLK) || \
                                                ((DURATION) == RTC_TAMPERPRECHARGEDURATION_2RTCCLK) || \
                                                ((DURATION) == RTC_TAMPERPRECHARGEDURATION_4RTCCLK) || \
                                                ((DURATION) == RTC_TAMPERPRECHARGEDURATION_8RTCCLK))
/**
  * @}
  */
  
/** @defgroup RTCEx_Tamper_TimeStampOnTamperDetection_Definitions 
  * @{
  */ 
#define RTC_TIMESTAMPONTAMPERDETECTION_ENABLE  ((uint32_t)RTC_TAFCR_TAMPTS)  /*!< TimeStamp on Tamper Detection event saved        */
#define RTC_TIMESTAMPONTAMPERDETECTION_DISABLE ((uint32_t)0x00000000)        /*!< TimeStamp on Tamper Detection event is not saved */
                                                                         
#define IS_TAMPER_TIMESTAMPONTAMPER_DETECTION(DETECTION) (((DETECTION) == RTC_TIMESTAMPONTAMPERDETECTION_ENABLE) || \
                                                          ((DETECTION) == RTC_TIMESTAMPONTAMPERDETECTION_DISABLE))
/**
  * @}
  */
  
/** @defgroup  RTCEx_Tamper_Pull_UP_Definitions
  * @{
  */ 
#define RTC_TAMPER_PULLUP_ENABLE  ((uint32_t)0x00000000)            /*!< TimeStamp on Tamper Detection event saved        */
#define RTC_TAMPER_PULLUP_DISABLE ((uint32_t)RTC_TAFCR_TAMPPUDIS)   /*!< TimeStamp on Tamper Detection event is not saved */
                                                                         
#define IS_TAMPER_PULLUP_STATE(STATE) (((STATE) == RTC_TAMPER_PULLUP_ENABLE) || \
                                       ((STATE) == RTC_TAMPER_PULLUP_DISABLE))
/**
  * @}
  */

/** @defgroup RTCEx_Wakeup_Timer_Definitions 
  * @{
  */ 
#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)

#define IS_WAKEUP_CLOCK(CLOCK) (((CLOCK) == RTC_WAKEUPCLOCK_RTCCLK_DIV16)       || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_RTCCLK_DIV8)    || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_RTCCLK_DIV4)    || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_RTCCLK_DIV2)    || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_CK_SPRE_16BITS) || \
                                    ((CLOCK) == RTC_WAKEUPCLOCK_CK_SPRE_17BITS))

#define IS_WAKEUP_COUNTER(COUNTER)  ((COUNTER) <= 0xFFFF)
/**
  * @}
  */ 

/** @defgroup RTCEx_Digital_Calibration_Definitions 
  * @{
  */ 
#define RTC_CALIBSIGN_POSITIVE            ((uint32_t)0x00000000) 
#define RTC_CALIBSIGN_NEGATIVE            ((uint32_t)0x00000080)

#define IS_RTC_CALIB_SIGN(SIGN) (((SIGN) == RTC_CALIBSIGN_POSITIVE) || \
                                 ((SIGN) == RTC_CALIBSIGN_NEGATIVE))

#define IS_RTC_CALIB_VALUE(VALUE) ((VALUE) < 0x20)
/**
  * @}
  */

/** @defgroup RTC_Smooth_calib_period_Definitions 
  * @{
  */ 
#define RTC_SMOOTHCALIB_PERIOD_32SEC   ((uint32_t)0x00000000)  /*!< If RTCCLK = 32768 Hz, Smooth calibation
                                                                    period is 32s,  else 2exp20 RTCCLK seconds */
#define RTC_SMOOTHCALIB_PERIOD_16SEC   ((uint32_t)0x00002000)  /*!< If RTCCLK = 32768 Hz, Smooth calibation 
                                                                    period is 16s, else 2exp19 RTCCLK seconds */
#define RTC_SMOOTHCALIB_PERIOD_8SEC    ((uint32_t)0x00004000)  /*!< If RTCCLK = 32768 Hz, Smooth calibation 
                                                                    period is 8s, else 2exp18 RTCCLK seconds */

#define IS_RTC_SMOOTH_CALIB_PERIOD(PERIOD) (((PERIOD) == RTC_SMOOTHCALIB_PERIOD_32SEC) || \
                                            ((PERIOD) == RTC_SMOOTHCALIB_PERIOD_16SEC) || \
                                            ((PERIOD) == RTC_SMOOTHCALIB_PERIOD_8SEC))                                         
/**
  * @}
  */ 

/** @defgroup RTCEx_Smooth_calib_Plus_pulses_Definitions 
  * @{
  */ 
#define RTC_SMOOTHCALIB_PLUSPULSES_SET    ((uint32_t)0x00008000)  /*!< The number of RTCCLK pulses added  
                                                                       during a X -second window = Y - CALM[8:0] 
                                                                       with Y = 512, 256, 128 when X = 32, 16, 8 */
#define RTC_SMOOTHCALIB_PLUSPULSES_RESET  ((uint32_t)0x00000000)  /*!< The number of RTCCLK pulses subbstited
                                                                       during a 32-second window = CALM[8:0] */

#define IS_RTC_SMOOTH_CALIB_PLUS(PLUS) (((PLUS) == RTC_SMOOTHCALIB_PLUSPULSES_SET) || \
                                        ((PLUS) == RTC_SMOOTHCALIB_PLUSPULSES_RESET))
/**
  * @}
  */ 

/** @defgroup RTCEx_Smooth_calib_Minus_pulses_Definitions 
  * @{
  */ 
#define  IS_RTC_SMOOTH_CALIB_MINUS(VALUE) ((VALUE) <= 0x000001FF)
/**
  * @}
  */

/** @defgroup RTCEx_Add_1_Second_Parameter_Definitions
  * @{
  */ 
#define RTC_SHIFTADD1S_RESET      ((uint32_t)0x00000000)
#define RTC_SHIFTADD1S_SET        ((uint32_t)0x80000000)

#define IS_RTC_SHIFT_ADD1S(SEL) (((SEL) == RTC_SHIFTADD1S_RESET) || \
                                 ((SEL) == RTC_SHIFTADD1S_SET))
/**
  * @}
  */ 

/** @defgroup RTCEx_Substract_Fraction_Of_Second_Value
  * @{
  */ 
#define IS_RTC_SHIFT_SUBFS(FS) ((FS) <= 0x00007FFF)
/**
  * @}
  */

 /** @defgroup RTCEx_Calib_Output_selection_Definitions 
  * @{
  */ 
#define RTC_CALIBOUTPUT_512HZ            ((uint32_t)0x00000000) 
#define RTC_CALIBOUTPUT_1HZ              ((uint32_t)0x00080000)

#define IS_RTC_CALIB_OUTPUT(OUTPUT)  (((OUTPUT) == RTC_CALIBOUTPUT_512HZ) || \
                                      ((OUTPUT) == RTC_CALIBOUTPUT_1HZ))
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/

/**
  * @brief  Enable the RTC WakeUp Timer peripheral.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_ENABLE(__HANDLE__)                      ((__HANDLE__)->Instance->CR |= (RTC_CR_WUTE))

/**
  * @brief  Enable the RTC TimeStamp peripheral.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_ENABLE(__HANDLE__)                        ((__HANDLE__)->Instance->CR |= (RTC_CR_TSE))

/**
  * @brief  Disable the RTC WakeUp Timer peripheral.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_DISABLE(__HANDLE__)                     ((__HANDLE__)->Instance->CR &= ~(RTC_CR_WUTE))

/**
  * @brief  Disable the RTC TimeStamp peripheral.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_DISABLE(__HANDLE__)                       ((__HANDLE__)->Instance->CR &= ~(RTC_CR_TSE))

/**
  * @brief  Enable the Coarse calibration process.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_COARSE_CALIB_ENABLE(__HANDLE__)                       ((__HANDLE__)->Instance->CR |= (RTC_CR_DCE))

/**
  * @brief  Disable the Coarse calibration process.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_COARSE_CALIB_DISABLE(__HANDLE__)                      ((__HANDLE__)->Instance->CR &= ~(RTC_CR_DCE))

/**
  * @brief  Enable the RTC calibration output.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_CALIBRATION_OUTPUT_ENABLE(__HANDLE__)                 ((__HANDLE__)->Instance->CR |= (RTC_CR_COE))

/**
  * @brief  Disable the calibration output.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_CALIBRATION_OUTPUT_DISABLE(__HANDLE__)                ((__HANDLE__)->Instance->CR &= ~(RTC_CR_COE))

/**
  * @brief  Enable the clock reference detection.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_CLOCKREF_DETECTION_ENABLE(__HANDLE__)                 ((__HANDLE__)->Instance->CR |= (RTC_CR_REFCKON))

/**
  * @brief  Disable the clock reference detection.
  * @param  __HANDLE__: specifies the RTC handle.
  * @retval None
  */
#define __HAL_RTC_CLOCKREF_DETECTION_DISABLE(__HANDLE__)                ((__HANDLE__)->Instance->CR &= ~(RTC_CR_REFCKON))

/**
  * @brief  Enable the RTC TimeStamp interrupt.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __INTERRUPT__: specifies the RTC TimeStamp interrupt sources to be enabled or disabled. 
  *         This parameter can be:
  *            @arg RTC_IT_TS: TimeStamp interrupt
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_ENABLE_IT(__HANDLE__, __INTERRUPT__)      ((__HANDLE__)->Instance->CR |= (__INTERRUPT__))

/**
  * @brief  Enable the RTC WakeUpTimer interrupt.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __INTERRUPT__: specifies the RTC WakeUpTimer interrupt sources to be enabled or disabled. 
  *         This parameter can be:
  *            @arg RTC_IT_WUT: WakeUpTimer A interrupt
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_ENABLE_IT(__HANDLE__, __INTERRUPT__)    ((__HANDLE__)->Instance->CR |= (__INTERRUPT__))

/**
  * @brief  Disable the RTC TimeStamp interrupt.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __INTERRUPT__: specifies the RTC TimeStamp interrupt sources to be enabled or disabled. 
  *         This parameter can be:
  *            @arg RTC_IT_TS: TimeStamp interrupt
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_DISABLE_IT(__HANDLE__, __INTERRUPT__)     ((__HANDLE__)->Instance->CR &= ~(__INTERRUPT__))

/**
  * @brief  Disable the RTC WakeUpTimer interrupt.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __INTERRUPT__: specifies the RTC WakeUpTimer interrupt sources to be enabled or disabled. 
  *         This parameter can be:
  *            @arg RTC_IT_WUT: WakeUpTimer A interrupt
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_DISABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->CR &= ~(__INTERRUPT__))

/**
  * @brief  Check whether the specified RTC Tamper interrupt has occurred or not.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC Tamper interrupt sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg  RTC_IT_TAMP1  
  * @retval None
  */
#define __HAL_RTC_TAMPER_GET_IT(__HANDLE__, __FLAG__)                 (((((__HANDLE__)->Instance->ISR) & ((__FLAG__)>> 4)) != RESET)? SET : RESET)

/**
  * @brief  Check whether the specified RTC WakeUpTimer interrupt has occurred or not.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC WakeUpTimer interrupt sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_IT_WUT:  WakeUpTimer A interrupt
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_GET_IT(__HANDLE__, __FLAG__)            (((((__HANDLE__)->Instance->ISR) & ((__FLAG__)>> 4)) != RESET)? SET : RESET)

/**
  * @brief  Check whether the specified RTC TimeStamp interrupt has occurred or not.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC TimeStamp interrupt sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_IT_TS: TimeStamp interrupt
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_GET_IT(__HANDLE__, __FLAG__)              (((((__HANDLE__)->Instance->ISR) & ((__FLAG__)>> 4)) != RESET)? SET : RESET)

/**
  * @brief  Get the selected RTC TimeStamp's flag status.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC TimeStamp Flag sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_FLAG_TSF   
  *            @arg RTC_FLAG_TSOVF     
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_GET_FLAG(__HANDLE__, __FLAG__)            (((((__HANDLE__)->Instance->ISR) & (__FLAG__)) != RESET)? SET : RESET)

/**
  * @brief  Get the selected RTC WakeUpTimer's flag status.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC WakeUpTimer Flag sources to be enabled or disabled.
  *          This parameter can be:
  *             @arg RTC_FLAG_WUTF   
  *             @arg RTC_FLAG_WUTWF     
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_GET_FLAG(__HANDLE__, __FLAG__)          (((((__HANDLE__)->Instance->ISR) & (__FLAG__)) != RESET)? SET : RESET)

/**
  * @brief  Get the selected RTC Tamper's flag status.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC Tamper Flag sources to be enabled or disabled.
  *          This parameter can be:
  *             @arg RTC_FLAG_TAMP1F      
  * @retval None
  */
#define __HAL_RTC_TAMPER_GET_FLAG(__HANDLE__, __FLAG__)               (((((__HANDLE__)->Instance->ISR) & (__FLAG__)) != RESET)? SET : RESET)

/**
  * @brief  Get the selected RTC shift operation's flag status.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC shift operation Flag is pending or not.
  *          This parameter can be:
  *             @arg RTC_FLAG_SHPF   
  * @retval None
  */
#define __HAL_RTC_SHIFT_GET_FLAG(__HANDLE__, __FLAG__)                (((((__HANDLE__)->Instance->ISR) & (__FLAG__)) != RESET)? SET : RESET)

/**
  * @brief  Clear the RTC Time Stamp's pending flags.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC Alarm Flag sources to be enabled or disabled.
  *          This parameter can be:
  *             @arg RTC_FLAG_TSF  
  * @retval None
  */
#define __HAL_RTC_TIMESTAMP_CLEAR_FLAG(__HANDLE__, __FLAG__)              ((__HANDLE__)->Instance->ISR) = (~(((__FLAG__) | RTC_ISR_INIT)& 0x0000FFFF)|((__HANDLE__)->Instance->ISR & RTC_ISR_INIT))

/**
  * @brief  Clear the RTC Tamper's pending flags.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC Tamper Flag sources to be enabled or disabled.
  *          This parameter can be:
  *             @arg RTC_FLAG_TAMP1F  
  * @retval None
  */
#define __HAL_RTC_TAMPER_CLEAR_FLAG(__HANDLE__, __FLAG__)                 ((__HANDLE__)->Instance->ISR) = (~(((__FLAG__) | RTC_ISR_INIT)& 0x0000FFFF)|((__HANDLE__)->Instance->ISR & RTC_ISR_INIT))

/**
  * @brief  Clear the RTC Wake Up timer's pending flags.
  * @param  __HANDLE__: specifies the RTC handle.
  * @param  __FLAG__: specifies the RTC Tamper Flag sources to be enabled or disabled.
  *         This parameter can be:
  *            @arg RTC_FLAG_WUTF   
  * @retval None
  */
#define __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(__HANDLE__, __FLAG__)            ((__HANDLE__)->Instance->ISR) = (~(((__FLAG__) | RTC_ISR_INIT)& 0x0000FFFF)|((__HANDLE__)->Instance->ISR & RTC_ISR_INIT)) 

/* Exported functions --------------------------------------------------------*/

/* RTC TimeStamp and Tamper functions *****************************************/
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp_IT(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTimeStamp(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_GetTimeStamp(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTimeStamp, RTC_DateTypeDef *sTimeStampDate, uint32_t Format);

HAL_StatusTypeDef HAL_RTCEx_SetTamper(RTC_HandleTypeDef *hrtc, RTC_TamperTypeDef* sTamper);
HAL_StatusTypeDef HAL_RTCEx_SetTamper_IT(RTC_HandleTypeDef *hrtc, RTC_TamperTypeDef* sTamper);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTamper(RTC_HandleTypeDef *hrtc, uint32_t Tamper);
void              HAL_RTCEx_TamperTimeStampIRQHandler(RTC_HandleTypeDef *hrtc);

void       HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef *hrtc);
void       HAL_RTCEx_Tamper2EventCallback(RTC_HandleTypeDef *hrtc);
void       HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForTimeStampEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
HAL_StatusTypeDef HAL_RTCEx_PollForTamper1Event(RTC_HandleTypeDef *hrtc, uint32_t Timeout);
HAL_StatusTypeDef HAL_RTCEx_PollForTamper2Event(RTC_HandleTypeDef *hrtc, uint32_t Timeout);

/* RTC Wake-up functions ******************************************************/
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock);
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer_IT(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock);
uint32_t          HAL_RTCEx_DeactivateWakeUpTimer(RTC_HandleTypeDef *hrtc);
uint32_t          HAL_RTCEx_GetWakeUpTimer(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_WakeUpTimerIRQHandler(RTC_HandleTypeDef *hrtc);
void       HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForWakeUpTimerEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);

/* Extension Control functions ************************************************/
void              HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister, uint32_t Data);
uint32_t          HAL_RTCEx_BKUPRead(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister);

HAL_StatusTypeDef HAL_RTCEx_SetCoarseCalib(RTC_HandleTypeDef *hrtc, uint32_t CalibSign, uint32_t Value);
HAL_StatusTypeDef HAL_RTCEx_DeactivateCoarseCalib(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetSmoothCalib(RTC_HandleTypeDef *hrtc, uint32_t SmoothCalibPeriod, uint32_t SmoothCalibPlusPulses, uint32_t SmouthCalibMinusPulsesValue);
HAL_StatusTypeDef HAL_RTCEx_SetSynchroShift(RTC_HandleTypeDef *hrtc, uint32_t ShiftAdd1S, uint32_t ShiftSubFS);
HAL_StatusTypeDef HAL_RTCEx_SetCalibrationOutPut(RTC_HandleTypeDef *hrtc, uint32_t CalibOutput);
HAL_StatusTypeDef HAL_RTCEx_DeactivateCalibrationOutPut(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DeactivateRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_EnableBypassShadow(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DisableBypassShadow(RTC_HandleTypeDef *hrtc);

/* Extension RTC features functions *******************************************/
void       HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc); 
HAL_StatusTypeDef HAL_RTCEx_PollForAlarmBEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout);


/**
  * @}
  */ 

/**
  * @}
  */ 
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_RTC_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
