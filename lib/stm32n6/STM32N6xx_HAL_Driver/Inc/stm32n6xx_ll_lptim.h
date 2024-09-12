/**
  ******************************************************************************
  * @file    stm32n6xx_ll_lptim.h
  * @author  MCD Application Team
  * @brief   Header file of LPTIM LL module.
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
#ifndef STM32N6xx_LL_LPTIM_H
#define STM32N6xx_LL_LPTIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (LPTIM1) || defined (LPTIM2) || defined (LPTIM3) || defined (LPTIM4) || defined (LPTIM5)

/** @defgroup LPTIM_LL LPTIM
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup LPTIM_LL_Private_variables LPTIM Private variables
  * @{
  */

static const uint8_t LL_LPTIM_SHIFT_TAB_CCxP[] =
{
  0U,                              /* CC1P */
  16U                              /* CC2P */
};

static const uint8_t LL_LPTIM_SHIFT_TAB_ICxF[] =
{
  0U,                              /* IC1F */
  16U                              /* IC2F */
};

static const uint8_t LL_LPTIM_SHIFT_TAB_ICxPSC[] =
{
  0U,                              /* IC1PSC */
  16U                              /* IC2PSC */
};

static const uint8_t LL_LPTIM_SHIFT_TAB_CCxSEL[] =
{
  0U,                              /* CC1SEL */
  16U                              /* CC2SEL */
};

static const uint8_t LL_LPTIM_SHIFT_TAB_CCxE[] =
{
  LPTIM_CCMR1_CC1E_Pos,            /* CC1E */
  LPTIM_CCMR1_CC2E_Pos             /* CC2E */
};

static const uint8_t LL_LPTIM_OFFSET_TAB_ICx[8][4] =
{
  {2, 7, 9, 13},
  {3, 5, 6, 8},
  {2, 3, 4, 5},
  {2, 2, 3, 3},
  {2, 2, 2, 2},
  {2, 2, 2, 2},
  {2, 2, 2, 2},
  {2, 2, 2, 2}

};

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup LPTIM_LL_Private_Macros LPTIM Private Macros
  * @{
  */
/**
  * @}
  */
#endif /*USE_FULL_LL_DRIVER*/

/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup LPTIM_LL_ES_INIT LPTIM Exported Init structure
  * @{
  */

/**
  * @brief  LPTIM Init structure definition
  */
typedef struct
{
  uint32_t ClockSource;    /*!< Specifies the source of the clock used by the LPTIM instance.
                                This parameter can be a value of @ref LPTIM_LL_EC_CLK_SOURCE.

                                This feature can be modified afterwards using unitary
                                function @ref LL_LPTIM_SetClockSource().*/

  uint32_t Prescaler;      /*!< Specifies the prescaler division ratio.
                                This parameter can be a value of @ref LPTIM_LL_EC_PRESCALER.

                                This feature can be modified afterwards using using unitary
                                function @ref LL_LPTIM_SetPrescaler().*/

  uint32_t Waveform;       /*!< Specifies the waveform shape.
                                This parameter can be a value of @ref LPTIM_LL_EC_OUTPUT_WAVEFORM.

                                This feature can be modified afterwards using unitary
                                function @ref LL_LPTIM_SetWaveform().*/
} LL_LPTIM_InitTypeDef;

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/
/** @defgroup LPTIM_LL_Exported_Constants LPTIM Exported Constants
  * @{
  */

/** @defgroup LPTIM_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_LPTIM_ReadReg function
  * @{
  */
#define LL_LPTIM_ISR_CMP1OK                   LPTIM_ISR_CMP1OK   /*!< Compare register 1 update OK */
#define LL_LPTIM_ISR_CMP2OK                   LPTIM_ISR_CMP2OK   /*!< Compare register 2 update OK */
#define LL_LPTIM_ISR_CC1IF                    LPTIM_ISR_CC1IF    /*!< Capture/Compare 1 interrupt flag */
#define LL_LPTIM_ISR_CC2IF                    LPTIM_ISR_CC2IF    /*!< Capture/Compare 2 interrupt flag */
#define LL_LPTIM_ISR_CC1OF                    LPTIM_ISR_CC1OF    /*!< Capture/Compare 1 over-capture flag */
#define LL_LPTIM_ISR_CC2OF                    LPTIM_ISR_CC2OF    /*!< Capture/Compare 2 over-capture flag */
#define LL_LPTIM_ISR_DIEROK                   LPTIM_ISR_DIEROK   /*!< Interrupt enable register update OK */
#define LL_LPTIM_ISR_ARRM                     LPTIM_ISR_ARRM     /*!< Autoreload match */
#define LL_LPTIM_ISR_EXTTRIG                  LPTIM_ISR_EXTTRIG  /*!< External trigger edge event */
#define LL_LPTIM_ISR_ARROK                    LPTIM_ISR_ARROK    /*!< Autoreload register update OK */
#define LL_LPTIM_ISR_UP                       LPTIM_ISR_UP       /*!< Counter direction change down to up */
#define LL_LPTIM_ISR_DOWN                     LPTIM_ISR_DOWN     /*!< Counter direction change up to down */
#define LL_LPTIM_ISR_UE                       LPTIM_ISR_UE       /*!< Update event */
#define LL_LPTIM_ISR_REPOK                    LPTIM_ISR_REPOK    /*!< Repetition register update OK */
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_IT IT Defines
  * @brief    IT defines which can be used with LL_LPTIM_ReadReg and  LL_LPTIM_WriteReg functions
  * @{
  */
#define LL_LPTIM_DIER_CMP1OKIE                 LPTIM_DIER_CMP1OKIE   /*!< Compare register 1 update OK */
#define LL_LPTIM_DIER_CMP2OKIE                 LPTIM_DIER_CMP2OKIE   /*!< Compare register 2 update OK */
#define LL_LPTIM_DIER_CC1IFIE                  LPTIM_DIER_CC1IE      /*!< Capture/Compare 1 interrupt flag */
#define LL_LPTIM_DIER_CC2IFIE                  LPTIM_DIER_CC2IE      /*!< Capture/Compare 2 interrupt flag */
#define LL_LPTIM_DIER_CC1OFIE                  LPTIM_DIER_CC1OIE     /*!< Capture/Compare 1 over-capture flag */
#define LL_LPTIM_DIER_CC2OFIE                  LPTIM_DIER_CC2OIE     /*!< Capture/Compare 2 over-capture flag */
#define LL_LPTIM_DIER_ARRMIE                   LPTIM_DIER_ARRMIE     /*!< Autoreload match */
#define LL_LPTIM_DIER_EXTTRIGIE                LPTIM_DIER_EXTTRIGIE  /*!< External trigger edge event */
#define LL_LPTIM_DIER_ARROKIE                  LPTIM_DIER_ARROKIE    /*!< Autoreload register update OK */
#define LL_LPTIM_DIER_UPIE                     LPTIM_DIER_UPIE       /*!< Counter direction change down to up */
#define LL_LPTIM_DIER_DOWNIE                   LPTIM_DIER_DOWNIE     /*!< Counter direction change up to down */
#define LL_LPTIM_DIER_UEIE                     LPTIM_DIER_UEIE       /*!< Update event */
#define LL_LPTIM_DIER_REPOKIE                  LPTIM_DIER_REPOKIE    /*!< Repetition register update OK */
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_OPERATING_MODE Operating Mode
  * @{
  */
#define LL_LPTIM_OPERATING_MODE_CONTINUOUS    LPTIM_CR_CNTSTRT /*!<LP Timer starts in continuous mode*/
#define LL_LPTIM_OPERATING_MODE_ONESHOT       LPTIM_CR_SNGSTRT /*!<LP Tilmer starts in single mode*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_UPDATE_MODE Update Mode
  * @{
  */
#define LL_LPTIM_UPDATE_MODE_IMMEDIATE        0x00000000U        /*!<Preload is disabled: registers are updated after each APB bus write access*/
#define LL_LPTIM_UPDATE_MODE_ENDOFPERIOD      LPTIM_CFGR_PRELOAD /*!<preload is enabled: registers are updated at the end of the current LPTIM period*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_COUNTER_MODE Counter Mode
  * @{
  */
#define LL_LPTIM_COUNTER_MODE_INTERNAL        0x00000000U          /*!<The counter is incremented following each internal clock pulse*/
#define LL_LPTIM_COUNTER_MODE_EXTERNAL        LPTIM_CFGR_COUNTMODE /*!<The counter is incremented following each valid clock pulse on the LPTIM external Input1*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_OUTPUT_WAVEFORM Output Waveform Type
  * @{
  */
#define LL_LPTIM_OUTPUT_WAVEFORM_PWM          0x00000000U     /*!<LPTIM  generates either a PWM waveform or a One pulse waveform depending on chosen operating mode CONTINUOUS or SINGLE*/
#define LL_LPTIM_OUTPUT_WAVEFORM_SETONCE      LPTIM_CFGR_WAVE /*!<LPTIM  generates a Set Once waveform*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_OUTPUT_POLARITY Output Polarity
  * @{
  */
#define LL_LPTIM_OUTPUT_POLARITY_REGULAR      0x00000000U             /*!<The LPTIM output reflects the compare results between LPTIMx_ARR and LPTIMx_CCRx registers*/
#define LL_LPTIM_OUTPUT_POLARITY_INVERSE      LPTIM_CCMR1_CC1P_0      /*!<The LPTIM output reflects the inverse of the compare results between LPTIMx_ARR and LPTIMx_CCx registers*/
/**
  * @}
  */

/** @defgroup TIM_LL_EC_CHANNEL Channel
  * @{
  */
#define LL_LPTIM_CHANNEL_CH1                  0x00000000U     /*!< LPTIM input/output channel 1 */
#define LL_LPTIM_CHANNEL_CH2                  0x00000001U     /*!< LPTIM input/output channel 2 */
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_LPTIM_IC_PRESCALER Input Capture Prescaler
  * @{
  */
#define LL_LPTIM_ICPSC_DIV1                   0x00000000UL                                  /*!< Capture performed each time an edge is detected on the capture input */
#define LL_LPTIM_ICPSC_DIV2                   LPTIM_CCMR1_IC1PSC_0                          /*!< Capture performed once every 2 events                                */
#define LL_LPTIM_ICPSC_DIV4                   LPTIM_CCMR1_IC1PSC_1                          /*!< Capture performed once every 4 events                                */
#define LL_LPTIM_ICPSC_DIV8                   (LPTIM_CCMR1_IC1PSC_0|LPTIM_CCMR1_IC1PSC_1)   /*!< Capture performed once every 8 events                                */
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_LPTIM_IC_FILTER Input Capture Filter
  * @{
  */
#define LL_LPTIM_ICFLT_CLOCK_DIV1             0x00000000UL                            /*!< any external input capture signal level change is considered as a valid transition */
#define LL_LPTIM_ICFLT_CLOCK_DIV2             LPTIM_CCMR1_IC1F_0                      /*!< external input capture signal level change must be stable for at least 2 clock periods before it is considered as valid transition */
#define LL_LPTIM_ICFLT_CLOCK_DIV4             LPTIM_CCMR1_IC1F_1                      /*!< external input capture signal level change must be stable for at least 4 clock periods before it is considered as valid transition */
#define LL_LPTIM_ICFLT_CLOCK_DIV8             (LPTIM_CCMR1_IC1F_0|LPTIM_CCMR1_IC1F_1) /*!< external input capture signal level change must be stable for at least 8 clock periods before it is considered as valid transition */
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_LPTIM_IC_POLARITY  Input Capture Polarity
  * @{
  */
#define LL_LPTIM_ICPOLARITY_RISING            0x00000000UL                              /*!< Capture/Compare input rising polarity */
#define LL_LPTIM_ICPOLARITY_FALLING           LPTIM_CCMR1_CC1P_0                        /*!< Capture/Compare input falling polarity */
#define LL_LPTIM_ICPOLARITY_RISING_FALLING    (LPTIM_CCMR1_CC1P_0|LPTIM_CCMR1_CC1P_1)   /*!< Capture/Compare input rising and falling polarities */
/**
  * @}
  */
/** @defgroup LPTIM_LL_EC_LPTIM_IC_Selection  Input Capture selection
  * @{
  */
#define LL_LPTIM_CCMODE_OUTPUT_PWM            0x00000000UL                              /*!< Select PWM mode */
#define LL_LPTIM_CCMODE_INPUTCAPTURE          LPTIM_CCMR1_CC1SEL                        /*!< Select Input Capture mode*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_PRESCALER Prescaler Value
  * @{
  */
#define LL_LPTIM_PRESCALER_DIV1               0x00000000U                               /*!<Prescaler division factor is set to 1*/
#define LL_LPTIM_PRESCALER_DIV2               LPTIM_CFGR_PRESC_0                        /*!<Prescaler division factor is set to 2*/
#define LL_LPTIM_PRESCALER_DIV4               LPTIM_CFGR_PRESC_1                        /*!<Prescaler division factor is set to 4*/
#define LL_LPTIM_PRESCALER_DIV8               (LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_0) /*!<Prescaler division factor is set to 8*/
#define LL_LPTIM_PRESCALER_DIV16              LPTIM_CFGR_PRESC_2                        /*!<Prescaler division factor is set to 16*/
#define LL_LPTIM_PRESCALER_DIV32              (LPTIM_CFGR_PRESC_2 | LPTIM_CFGR_PRESC_0) /*!<Prescaler division factor is set to 32*/
#define LL_LPTIM_PRESCALER_DIV64              (LPTIM_CFGR_PRESC_2 | LPTIM_CFGR_PRESC_1) /*!<Prescaler division factor is set to 64*/
#define LL_LPTIM_PRESCALER_DIV128             LPTIM_CFGR_PRESC                          /*!<Prescaler division factor is set to 128*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_TRIG_SOURCE Trigger Source
  * @{
  */
#define LL_LPTIM_TRIG_SOURCE_GPIO             0x00000000U                                                          /*!<External input trigger is connected to TIMx_ETR input*/
#define LL_LPTIM_TRIG_SOURCE_RTCALARMA        LPTIM_CFGR_TRIGSEL_0                                                 /*!<External input trigger is connected to RTC Alarm A*/
#define LL_LPTIM_TRIG_SOURCE_RTCALARMB        LPTIM_CFGR_TRIGSEL_1                                                 /*!<External input trigger is connected to RTC Alarm B*/
#define LL_LPTIM_TRIG_SOURCE_RTCTAMP1         (LPTIM_CFGR_TRIGSEL_1 | LPTIM_CFGR_TRIGSEL_0)                        /*!<External input trigger is connected to RTC Tamper 1*/
#define LL_LPTIM_TRIG_SOURCE_RTCTAMP2         LPTIM_CFGR_TRIGSEL_2                                                 /*!<External input trigger is connected to RTC Tamper 2*/
#define LL_LPTIM_TRIG_SOURCE_RTCTAMP3         (LPTIM_CFGR_TRIGSEL_2 | LPTIM_CFGR_TRIGSEL_0)                        /*!<External input trigger is connected to RTC Tamper 3*/
#define LL_LPTIM_TRIG_SOURCE_SAI2_FS_A        (LPTIM_CFGR_TRIGSEL_2 | LPTIM_CFGR_TRIGSEL_0)                        /*!<External input trigger is connected to SAI2 FS A output */
#define LL_LPTIM_TRIG_SOURCE_SAI2_FS_B        (LPTIM_CFGR_TRIGSEL_2 | LPTIM_CFGR_TRIGSEL_1)                        /*!<External input trigger is connected to SAI2 FS B output */
#define LL_LPTIM_TRIG_SOURCE_LPTIM2_CH1       LPTIM_CFGR_TRIGSEL_0                                                 /*!<External input trigger is connected to LPTIM2 Channel 1 */
#define LL_LPTIM_TRIG_SOURCE_LPTIM3_CH1       LPTIM_CFGR_TRIGSEL_1                                                 /*!<External input trigger is connected to LPTIM3 Channel 1 */
#define LL_LPTIM_TRIG_SOURCE_LPTIM4_OUT       (LPTIM_CFGR_TRIGSEL_1 | LPTIM_CFGR_TRIGSEL_0)                        /*!<External input trigger is connected to LPTIM4 Output */
#define LL_LPTIM_TRIG_SOURCE_LPTIM5_OUT       LPTIM_CFGR_TRIGSEL_2                                                 /*!<External input trigger is connected to LPTIM5 Output */
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_TRIG_FILTER Trigger Filter
  * @{
  */
#define LL_LPTIM_TRIG_FILTER_NONE             0x00000000U         /*!<Any trigger active level change is considered as a valid trigger*/
#define LL_LPTIM_TRIG_FILTER_2                LPTIM_CFGR_TRGFLT_0 /*!<Trigger active level change must be stable for at least 2 clock periods before it is considered as valid trigger*/
#define LL_LPTIM_TRIG_FILTER_4                LPTIM_CFGR_TRGFLT_1 /*!<Trigger active level change must be stable for at least 4 clock periods before it is considered as valid trigger*/
#define LL_LPTIM_TRIG_FILTER_8                LPTIM_CFGR_TRGFLT   /*!<Trigger active level change must be stable for at least 8 clock periods before it is considered as valid trigger*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_TRIG_POLARITY Trigger Polarity
  * @{
  */
#define LL_LPTIM_TRIG_POLARITY_RISING         LPTIM_CFGR_TRIGEN_0 /*!<LPTIM counter starts when a rising edge is detected*/
#define LL_LPTIM_TRIG_POLARITY_FALLING        LPTIM_CFGR_TRIGEN_1 /*!<LPTIM counter starts when a falling edge is detected*/
#define LL_LPTIM_TRIG_POLARITY_RISING_FALLING LPTIM_CFGR_TRIGEN   /*!<LPTIM counter starts when a rising or a falling edge is detected*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_CLK_SOURCE Clock Source
  * @{
  */
#define LL_LPTIM_CLK_SOURCE_INTERNAL          0x00000000U      /*!<LPTIM is clocked by internal clock source (APB clock or any of the embedded oscillators)*/
#define LL_LPTIM_CLK_SOURCE_EXTERNAL          LPTIM_CFGR_CKSEL /*!<LPTIM is clocked by an external clock source through the LPTIM external Input1*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_CLK_FILTER Clock Filter
  * @{
  */
#define LL_LPTIM_CLK_FILTER_NONE              0x00000000U        /*!<Any external clock signal level change is considered as a valid transition*/
#define LL_LPTIM_CLK_FILTER_2                 LPTIM_CFGR_CKFLT_0 /*!<External clock signal level change must be stable for at least 2 clock periods before it is considered as valid transition*/
#define LL_LPTIM_CLK_FILTER_4                 LPTIM_CFGR_CKFLT_1 /*!<External clock signal level change must be stable for at least 4 clock periods before it is considered as valid transition*/
#define LL_LPTIM_CLK_FILTER_8                 LPTIM_CFGR_CKFLT   /*!<External clock signal level change must be stable for at least 8 clock periods before it is considered as valid transition*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_CLK_POLARITY Clock Polarity
  * @{
  */
#define LL_LPTIM_CLK_POLARITY_RISING          0x00000000U        /*!< The rising edge is the active edge used for counting*/
#define LL_LPTIM_CLK_POLARITY_FALLING         LPTIM_CFGR_CKPOL_0 /*!< The falling edge is the active edge used for counting*/
#define LL_LPTIM_CLK_POLARITY_RISING_FALLING  LPTIM_CFGR_CKPOL_1 /*!< Both edges are active edges*/
/**
  * @}
  */

/** @defgroup LPTIM_LL_EC_ENCODER_MODE Encoder Mode
  * @{
  */
#define LL_LPTIM_ENCODER_MODE_RISING          0x00000000U        /*!< The rising edge is the active edge used for counting*/
#define LL_LPTIM_ENCODER_MODE_FALLING         LPTIM_CFGR_CKPOL_0 /*!< The falling edge is the active edge used for counting*/
#define LL_LPTIM_ENCODER_MODE_RISING_FALLING  LPTIM_CFGR_CKPOL_1 /*!< Both edges are active edges*/
/**
  * @}
  */

/** @defgroup LPTIM_EC_INPUT1_SRC Input1 Source
  * @{
  */
#define LL_LPTIM_INPUT1_SRC_GPIO         0x00000000UL                  /*!< For LPTIM1, LPTIM2 and LPTIM3 */
/**
  * @}
  */

/** @defgroup LPTIM_EC_INPUT2_SRC Input2 Source
  * @{
  */
#define LL_LPTIM_INPUT2_SRC_GPIO         0x00000000UL                  /*!< For LPTIM1, LPTIM2 and LPTIM3 */
/**
  * @}
  */

/** @defgroup LPTIM_EC_LPTIM1_IC1_RMP LPTIM1 Input Ch1 Remap
  * @{
  */
#define LL_LPTIM_LPTIM1_IC1_RMP_GPIO     0x00000000UL                 /*!< IC1 connected to GPIO */
/**
  * @}
  */

/** @defgroup LPTIM_EC_LPTIM1_IC2_RMP LPTIM1 Input Ch2 Remap
  * @{
  */
#define LL_LPTIM_LPTIM1_IC2_RMP_GPIO     0x00000000UL                 /*!< IC2 connected to GPIO */
#define LL_LPTIM_LPTIM1_IC2_RMP_LSI      LPTIM_CFGR2_IC2SEL_0         /*!< IC2 connected to LSI */
#define LL_LPTIM_LPTIM1_IC2_RMP_LSE      LPTIM_CFGR2_IC2SEL_1         /*!< IC2 connected to LSE */
/**
  * @}
  */

/** @defgroup LPTIM_EC_LPTIM2_IC1_RMP LPTIM2 Input Ch1 Remap
  * @{
  */
#define LL_LPTIM_LPTIM2_IC1_RMP_GPIO           0x00000000UL                 /*!< IC1 connected to GPIO */
#define LL_LPTIM_LPTIM2_IC1_RMP_I3C1_IBIACK    LPTIM_CFGR2_IC1SEL_0         /*!< IC1 connected to I3C1 IBI ACK */
/**
  * @}
  */

/** @defgroup LPTIM_EC_LPTIM2_IC2_RMP LPTIM2 Input Ch2 Remap
  * @{
  */
#define LL_LPTIM_LPTIM2_IC2_RMP_GPIO         0x00000000UL                                  /*!< IC2 connected to GPIO */
#define LL_LPTIM_LPTIM2_IC2_RMP_HSI_1024     LPTIM_CFGR2_IC2SEL_0                          /*!< IC2 connected to HSI/1024 */
#define LL_LPTIM_LPTIM2_IC2_RMP_MSI_128      LPTIM_CFGR2_IC2SEL_1                          /*!< IC2 connected to MSI/128 */
#define LL_LPTIM_LPTIM2_IC2_RMP_I3C2_IBIACK  (LPTIM_CFGR2_IC2SEL_1 | LPTIM_CFGR2_IC2SEL_0) /*!< IC2 connected to I3C2 IBI ACK */
/**
  * @}
  */

/** @defgroup LPTIM_EC_LPTIM3_IC1_RMP LPTIM3 Input Ch1 Remap
  * @{
  */
#define LL_LPTIM_LPTIM3_IC1_RMP_GPIO           0x00000000UL                 /*!< IC1 connected to GPIO */
#define LL_LPTIM_LPTIM3_IC1_RMP_I3C1_IBIACK    LPTIM_CFGR2_IC1SEL_0         /*!< IC1 connected to I3C1 IBI ACK */
/**
  * @}
  */

/** @defgroup LPTIM_EC_LPTIM3_IC2_RMP LPTIM3 Input Ch2 Remap
  * @{
  */

#define LL_LPTIM_LPTIM3_IC2_RMP_GPIO           0x00000000UL                            /*!< IC2 connected to GPIO */
#define LL_LPTIM_LPTIM3_IC2_RMP_I3C2_IBIACK    LPTIM_CFGR2_IC2SEL_0                    /*!< IC2 connected to I3C2 IBI ACK */
/**
  * @}
  */


/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup LPTIM_LL_Exported_Macros LPTIM Exported Macros
  * @{
  */

/** @defgroup LPTIM_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in LPTIM register
  * @param  __INSTANCE__ LPTIM Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_LPTIM_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG((__INSTANCE__)->__REG__, (__VALUE__))

/**
  * @brief  Read a value in LPTIM register
  * @param  __INSTANCE__ LPTIM Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_LPTIM_ReadReg(__INSTANCE__, __REG__) READ_REG((__INSTANCE__)->__REG__)

/**
  * @brief  LPTimer Input Capture Get Offset(in counter step unit)
  * @note   The real capture value corresponding to the input capture trigger can be calculated using
  *         the formula hereafter : Real capture value = captured(LPTIM_CCRx) - offset
  *         The Offset value is depending on the glitch filter value for the channel
  *         and the value of the prescaler for the kernel clock.
  *         Please check Errata Sheet V1_8 for more details under "variable latency
  *         on input capture channel" section.
  * @param  __PSC__ This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_PRESCALER_DIV1
  *         @arg @ref LL_LPTIM_PRESCALER_DIV2
  *         @arg @ref LL_LPTIM_PRESCALER_DIV4
  *         @arg @ref LL_LPTIM_PRESCALER_DIV8
  *         @arg @ref LL_LPTIM_PRESCALER_DIV16
  *         @arg @ref LL_LPTIM_PRESCALER_DIV32
  *         @arg @ref LL_LPTIM_PRESCALER_DIV64
  *         @arg @ref LL_LPTIM_PRESCALER_DIV128
  * @param  __FLT__ This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV1
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV2
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV4
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV8
  * @retval offset value
  */
#define LL_LPTIM_IC_GET_OFFSET(__PSC__, __FLT__) LL_LPTIM_OFFSET_TAB_ICx\
  [((__PSC__) & LPTIM_CFGR_PRESC_Msk) >> LPTIM_CFGR_PRESC_Pos]\
  [((__FLT__) & LPTIM_CCMR1_IC1F_Msk) >> LPTIM_CCMR1_IC1F_Pos]
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup LPTIM_LL_Exported_Functions LPTIM Exported Functions
  * @{
  */

/** Legacy definitions for compatibility purpose
@cond 0
  */
#define LL_LPTIM_ClearFLAG_CMPM  LL_LPTIM_ClearFlag_CMPM
#define LL_LPTIM_ClearFLAG_CC1   LL_LPTIM_ClearFlag_CC1
#define LL_LPTIM_ClearFLAG_CC2   LL_LPTIM_ClearFlag_CC2
#define LL_LPTIM_ClearFLAG_CC1O  LL_LPTIM_ClearFlag_CC1O
#define LL_LPTIM_ClearFLAG_CC2O  LL_LPTIM_ClearFlag_CC2O
#define LL_LPTIM_ClearFLAG_ARRM  LL_LPTIM_ClearFlag_ARRM
/**
@endcond
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup LPTIM_LL_EF_Init Initialisation and deinitialisation functions
  * @{
  */

ErrorStatus LL_LPTIM_DeInit(const LPTIM_TypeDef *LPTIMx);
void LL_LPTIM_StructInit(LL_LPTIM_InitTypeDef *LPTIM_InitStruct);
ErrorStatus LL_LPTIM_Init(LPTIM_TypeDef *LPTIMx, const LL_LPTIM_InitTypeDef *LPTIM_InitStruct);
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/** @defgroup LPTIM_LL_EF_LPTIM_Configuration LPTIM Configuration
  * @{
  */

/**
  * @brief  Enable the LPTIM instance
  * @note After setting the ENABLE bit, a delay of two counter clock is needed
  *       before the LPTIM instance is actually enabled.
  * @rmtoll CR           ENABLE        LL_LPTIM_Enable
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_Enable(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->CR, LPTIM_CR_ENABLE);
}

/**
  * @brief  Disable the LPTIM instance
  * @rmtoll CR           ENABLE        LL_LPTIM_Disable
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_Disable(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->CR, LPTIM_CR_ENABLE);
}

/**
  * @brief  Indicates whether the LPTIM instance is enabled.
  * @rmtoll CR           ENABLE        LL_LPTIM_IsEnabled
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabled(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->CR, LPTIM_CR_ENABLE) == LPTIM_CR_ENABLE) ? 1UL : 0UL));
}

/**
  * @brief  Starts the LPTIM counter in the desired mode.
  * @note LPTIM instance must be enabled before starting the counter.
  * @note It is possible to change on the fly from One Shot mode to
  *       Continuous mode.
  * @rmtoll CR           CNTSTRT       LL_LPTIM_StartCounter\n
  *         CR           SNGSTRT       LL_LPTIM_StartCounter
  * @param  LPTIMx Low-Power Timer instance
  * @param  OperatingMode This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_OPERATING_MODE_CONTINUOUS
  *         @arg @ref LL_LPTIM_OPERATING_MODE_ONESHOT
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_StartCounter(LPTIM_TypeDef *LPTIMx, uint32_t OperatingMode)
{
  MODIFY_REG(LPTIMx->CR, LPTIM_CR_CNTSTRT | LPTIM_CR_SNGSTRT, OperatingMode);
}

/**
  * @brief  Enable reset after read.
  * @note After calling this function any read access to LPTIM_CNT
  *        register will asynchronously reset the LPTIM_CNT register content.
  * @rmtoll CR           RSTARE        LL_LPTIM_EnableResetAfterRead
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableResetAfterRead(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->CR, LPTIM_CR_RSTARE);
}

/**
  * @brief  Disable reset after read.
  * @rmtoll CR           RSTARE        LL_LPTIM_DisableResetAfterRead
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableResetAfterRead(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->CR, LPTIM_CR_RSTARE);
}

/**
  * @brief  Indicate whether the reset after read feature is enabled.
  * @rmtoll CR           RSTARE        LL_LPTIM_IsEnabledResetAfterRead
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledResetAfterRead(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->CR, LPTIM_CR_RSTARE) == LPTIM_CR_RSTARE) ? 1UL : 0UL));
}

/**
  * @brief  Reset of the LPTIM_CNT counter register (synchronous).
  * @note Due to the synchronous nature of this reset, it only takes
  *       place after a synchronization delay of 3 LPTIM core clock cycles
  *      (LPTIM core clock may be different from APB clock).
  * @note COUNTRST is automatically cleared by hardware
  * @rmtoll CR           COUNTRST       LL_LPTIM_ResetCounter\n
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ResetCounter(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->CR, LPTIM_CR_COUNTRST);
}

/**
  * @brief  Set the LPTIM registers update mode (enable/disable register preload)
  * @note This function must be called when the LPTIM instance is disabled.
  * @rmtoll CFGR         PRELOAD       LL_LPTIM_SetUpdateMode
  * @param  LPTIMx Low-Power Timer instance
  * @param  UpdateMode This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_UPDATE_MODE_IMMEDIATE
  *         @arg @ref LL_LPTIM_UPDATE_MODE_ENDOFPERIOD
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetUpdateMode(LPTIM_TypeDef *LPTIMx, uint32_t UpdateMode)
{
  MODIFY_REG(LPTIMx->CFGR, LPTIM_CFGR_PRELOAD, UpdateMode);
}

/**
  * @brief  Get the LPTIM registers update mode
  * @rmtoll CFGR         PRELOAD       LL_LPTIM_GetUpdateMode
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_UPDATE_MODE_IMMEDIATE
  *         @arg @ref LL_LPTIM_UPDATE_MODE_ENDOFPERIOD
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetUpdateMode(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_PRELOAD));
}

/**
  * @brief  Set the auto reload value
  * @note The LPTIMx_ARR register content must only be modified when the LPTIM is enabled
  * @note After a write to the LPTIMx_ARR register a new write operation to the
  *       same register can only be performed when the previous write operation
  *       is completed. Any successive write before  the ARROK flag is set, will
  *       lead to unpredictable results.
  * @note autoreload value be strictly greater than the compare value.
  * @rmtoll ARR          ARR           LL_LPTIM_SetAutoReload
  * @param  LPTIMx Low-Power Timer instance
  * @param  AutoReload Value between Min_Data=0x0001 and Max_Data=0xFFFF
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetAutoReload(LPTIM_TypeDef *LPTIMx, uint32_t AutoReload)
{
  MODIFY_REG(LPTIMx->ARR, LPTIM_ARR_ARR, AutoReload);
}

/**
  * @brief  Get actual auto reload value
  * @rmtoll ARR          ARR           LL_LPTIM_GetAutoReload
  * @param  LPTIMx Low-Power Timer instance
  * @retval AutoReload Value between Min_Data=0x0001 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetAutoReload(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->ARR, LPTIM_ARR_ARR));
}

/**
  * @brief  Set the repetition value
  * @note The LPTIMx_RCR register content must only be modified when the LPTIM is enabled
  * @rmtoll RCR          REP           LL_LPTIM_SetRepetition
  * @param  LPTIMx Low-Power Timer instance
  * @param  Repetition Value between Min_Data=0x00 and Max_Data=0xFF
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetRepetition(LPTIM_TypeDef *LPTIMx, uint32_t Repetition)
{
  MODIFY_REG(LPTIMx->RCR, LPTIM_RCR_REP, Repetition);
}

/**
  * @brief  Get the repetition value
  * @rmtoll RCR          REP           LL_LPTIM_GetRepetition
  * @param  LPTIMx Low-Power Timer instance
  * @retval Repetition Value between Min_Data=0x00 and Max_Data=0xFF
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetRepetition(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->RCR, LPTIM_RCR_REP));
}

/**
  * @brief  Enable capture/compare channel.
  * @rmtoll CCMR1         CC1E          LL_LPTIM_CC_EnableChannel\n
  *         CCMR1         CC2E          LL_LPTIM_CC_EnableChannel
  * @param  LPTIMx LPTimer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_CC_EnableChannel(LPTIM_TypeDef *LPTIMx, uint32_t Channel)
{
  SET_BIT(LPTIMx->CCMR1, 0x1UL << LL_LPTIM_SHIFT_TAB_CCxE[Channel]);
}

/**
  * @brief  Disable capture/compare channel.
  * @rmtoll CCMR1         CC1E          LL_LPTIM_CC_DisableChannel\n
  *         CCMR1         CC2E          LL_LPTIM_CC_DisableChannel
  * @param  LPTIMx LPTimer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_CC_DisableChannel(LPTIM_TypeDef *LPTIMx, uint32_t Channel)
{
  CLEAR_BIT(LPTIMx->CCMR1, 0x1UL << LL_LPTIM_SHIFT_TAB_CCxE[Channel]);
}

/**
  * @brief  Indicate whether channel is enabled.
  * @rmtoll CCMR1         CC1E          LL_LPTIM_CC_IsEnabledChannel\n
  *         CCMR1         CC2E          LL_LPTIM_CC_IsEnabledChannel
  * @param  LPTIMx LPTimer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_CC_IsEnabledChannel(const LPTIM_TypeDef *LPTIMx, uint32_t Channel)
{
  return ((READ_BIT(LPTIMx->CCMR1, 0x1UL << LL_LPTIM_SHIFT_TAB_CCxE[Channel]) ==                                       \
           (0x1UL << LL_LPTIM_SHIFT_TAB_CCxE[Channel])) ? 1UL : 0UL);

}

/**
  * @brief  Set the compare value
  * @note After a write to the LPTIMx_CCR1 register a new write operation to the
  *       same register can only be performed when the previous write operation
  *       is completed. Any successive write before the CMP1OK flag is set, will
  *       lead to unpredictable results.
  * @rmtoll CCR1          CCR1           LL_LPTIM_OC_SetCompareCH1
  * @param  LPTIMx Low-Power Timer instance
  * @param  CompareValue Value between Min_Data=0x00 and Max_Data=0xFFFF
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_OC_SetCompareCH1(LPTIM_TypeDef *LPTIMx, uint32_t CompareValue)
{
  MODIFY_REG(LPTIMx->CCR1, LPTIM_CCR1_CCR1, CompareValue);
}

/**
  * @brief  Get actual compare value
  * @rmtoll CCR1          CCR1           LL_LPTIM_OC_GetCompareCH1
  * @param  LPTIMx Low-Power Timer instance
  * @retval CompareValue Value between Min_Data=0x00 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_LPTIM_OC_GetCompareCH1(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CCR1, LPTIM_CCR1_CCR1));
}

/**
  * @brief  Set the compare value
  * @note After a write to the LPTIMx_CCR2 register a new write operation to the
  *       same register can only be performed when the previous write operation
  *       is completed. Any successive write before the CMP2OK flag is set, will
  *       lead to unpredictable results.
  * @rmtoll CCR2          CCR2           LL_LPTIM_OC_SetCompareCH2
  * @param  LPTIMx Low-Power Timer instance
  * @param  CompareValue Value between Min_Data=0x00 and Max_Data=0xFFFF
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_OC_SetCompareCH2(LPTIM_TypeDef *LPTIMx, uint32_t CompareValue)
{
  MODIFY_REG(LPTIMx->CCR2, LPTIM_CCR2_CCR2, CompareValue);
}

/**
  * @brief  Get actual compare value
  * @rmtoll CCR2          CCR2           LL_LPTIM_OC_GetCompareCH2
  * @param  LPTIMx Low-Power Timer instance
  * @retval CompareValue Value between Min_Data=0x00 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_LPTIM_OC_GetCompareCH2(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CCR2, LPTIM_CCR2_CCR2));
}

/**
  * @brief  Get actual counter value
  * @note When the LPTIM instance is running with an asynchronous clock, reading
  *       the LPTIMx_CNT register may return unreliable values. So in this case
  *       it is necessary to perform two consecutive read accesses and verify
  *       that the two returned values are identical.
  * @rmtoll CNT          CNT           LL_LPTIM_GetCounter
  * @param  LPTIMx Low-Power Timer instance
  * @retval Counter value
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetCounter(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CNT, LPTIM_CNT_CNT));
}

/**
  * @brief  Set the counter mode (selection of the LPTIM counter clock source).
  * @note The counter mode can be set only when the LPTIM instance is disabled.
  * @rmtoll CFGR         COUNTMODE     LL_LPTIM_SetCounterMode
  * @param  LPTIMx Low-Power Timer instance
  * @param  CounterMode This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_COUNTER_MODE_INTERNAL
  *         @arg @ref LL_LPTIM_COUNTER_MODE_EXTERNAL
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetCounterMode(LPTIM_TypeDef *LPTIMx, uint32_t CounterMode)
{
  MODIFY_REG(LPTIMx->CFGR, LPTIM_CFGR_COUNTMODE, CounterMode);
}

/**
  * @brief  Get the counter mode
  * @rmtoll CFGR         COUNTMODE     LL_LPTIM_GetCounterMode
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_COUNTER_MODE_INTERNAL
  *         @arg @ref LL_LPTIM_COUNTER_MODE_EXTERNAL
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetCounterMode(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_COUNTMODE));
}

/**
  * @brief  Set  waveform shape
  * @rmtoll CFGR         WAVE          LL_LPTIM_SetWaveform
  * @param  LPTIMx Low-Power Timer instance
  * @param  Waveform This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_OUTPUT_WAVEFORM_PWM
  *         @arg @ref LL_LPTIM_OUTPUT_WAVEFORM_SETONCE
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetWaveform(LPTIM_TypeDef *LPTIMx, uint32_t Waveform)
{
  MODIFY_REG(LPTIMx->CFGR, LPTIM_CFGR_WAVE, Waveform);
}

/**
  * @brief  Get actual waveform shape
  * @rmtoll CFGR         WAVE          LL_LPTIM_GetWaveform
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_OUTPUT_WAVEFORM_PWM
  *         @arg @ref LL_LPTIM_OUTPUT_WAVEFORM_SETONCE
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetWaveform(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_WAVE));
}

/**
  * @brief  Set the polarity of an output channel.
  * @rmtoll CCMR1         CC1P          LL_LPTIM_OC_SetPolarity\n
  * @rmtoll CCMR1         CC2P          LL_LPTIM_OC_SetPolarity\n
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @param  Polarity This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_OUTPUT_POLARITY_REGULAR
  *         @arg @ref LL_LPTIM_OUTPUT_POLARITY_INVERSE
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_OC_SetPolarity(LPTIM_TypeDef *LPTIMx, uint32_t Channel, uint32_t Polarity)
{
  if ((LPTIMx == LPTIM4) || (LPTIMx == LPTIM5))
  {
    MODIFY_REG(LPTIMx->CFGR, LPTIM_CFGR_WAVPOL, ((Polarity >> LPTIM_CCMR1_CC1P_Pos) << LPTIM_CFGR_WAVPOL_Pos));
  }
  else
  {
    MODIFY_REG(LPTIMx->CCMR1, (LPTIM_CCMR1_CC1P << LL_LPTIM_SHIFT_TAB_CCxP[Channel]),
               (Polarity << LL_LPTIM_SHIFT_TAB_CCxP[Channel]));
  }
}

/**
  * @brief  Get the polarity of an output channel.
  * @rmtoll CCMR1         CC1P          LL_LPTIM_OC_GetPolarity\n
  * @rmtoll CCMR1         CC2P          LL_LPTIM_OC_GetPolarity\n
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_OUTPUT_POLARITY_REGULAR
  *         @arg @ref LL_LPTIM_OUTPUT_POLARITY_INVERSE
  */
__STATIC_INLINE uint32_t LL_LPTIM_OC_GetPolarity(const LPTIM_TypeDef *LPTIMx, uint32_t Channel)
{
  if ((LPTIMx == LPTIM4) || (LPTIMx == LPTIM5))
  {
    return (uint32_t)((READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_WAVPOL) >> LPTIM_CFGR_WAVPOL_Pos) << LPTIM_CCMR1_CC1P_Pos);
  }
  else
  {
    return (uint32_t)(READ_BIT(LPTIMx->CCMR1, LPTIM_CCMR1_CC1P << LL_LPTIM_SHIFT_TAB_CCxP[Channel]) >>                 \
                      LL_LPTIM_SHIFT_TAB_CCxP[Channel]);
  }
}

/**
  * @brief  Set actual prescaler division ratio.
  * @note This function must be called when the LPTIM instance is disabled.
  * @note When the LPTIM is configured to be clocked by an internal clock source
  *       and the LPTIM counter is configured to be updated by active edges
  *       detected on the LPTIM external Input1, the internal clock provided to
  *       the LPTIM must be not be prescaled.
  * @rmtoll CFGR         PRESC         LL_LPTIM_SetPrescaler
  * @param  LPTIMx Low-Power Timer instance
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_PRESCALER_DIV1
  *         @arg @ref LL_LPTIM_PRESCALER_DIV2
  *         @arg @ref LL_LPTIM_PRESCALER_DIV4
  *         @arg @ref LL_LPTIM_PRESCALER_DIV8
  *         @arg @ref LL_LPTIM_PRESCALER_DIV16
  *         @arg @ref LL_LPTIM_PRESCALER_DIV32
  *         @arg @ref LL_LPTIM_PRESCALER_DIV64
  *         @arg @ref LL_LPTIM_PRESCALER_DIV128
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetPrescaler(LPTIM_TypeDef *LPTIMx, uint32_t Prescaler)
{
  MODIFY_REG(LPTIMx->CFGR, LPTIM_CFGR_PRESC, Prescaler);
}

/**
  * @brief  Get actual prescaler division ratio.
  * @rmtoll CFGR         PRESC         LL_LPTIM_GetPrescaler
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_PRESCALER_DIV1
  *         @arg @ref LL_LPTIM_PRESCALER_DIV2
  *         @arg @ref LL_LPTIM_PRESCALER_DIV4
  *         @arg @ref LL_LPTIM_PRESCALER_DIV8
  *         @arg @ref LL_LPTIM_PRESCALER_DIV16
  *         @arg @ref LL_LPTIM_PRESCALER_DIV32
  *         @arg @ref LL_LPTIM_PRESCALER_DIV64
  *         @arg @ref LL_LPTIM_PRESCALER_DIV128
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetPrescaler(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_PRESC));
}

/**
  * @brief  Set LPTIM input 1 source (default GPIO).
  * @rmtoll CFGR2      IN1SEL       LL_LPTIM_SetInput1Src
  * @param  LPTIMx Low-Power Timer instance
  * @param  Src This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_INPUT1_SRC_GPIO
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetInput1Src(LPTIM_TypeDef *LPTIMx, uint32_t Src)
{
  MODIFY_REG(LPTIMx->CFGR2, LPTIM_CFGR2_IN1SEL, Src);
}

/**
  * @brief  Set LPTIM input 2 source (default GPIO).
  * @rmtoll CFGR2      IN2SEL       LL_LPTIM_SetInput2Src
  * @param  LPTIMx Low-Power Timer instance
  * @param  Src This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_INPUT2_SRC_GPIO
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetInput2Src(LPTIM_TypeDef *LPTIMx, uint32_t Src)
{
  MODIFY_REG(LPTIMx->CFGR2, LPTIM_CFGR2_IN2SEL, Src);
}

/**
  * @brief  Set LPTIM input source (default GPIO).
  * @rmtoll CFGR2      IC1SEL        LL_LPTIM_SetRemap
  * @rmtoll CFGR2      IC2SEL        LL_LPTIM_SetRemap
  * @param  LPTIMx Low-Power Timer instance
  * @param  Src This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_LPTIM1_IC1_RMP_GPIO
  *         @arg @ref LL_LPTIM_LPTIM1_IC2_RMP_GPIO
  *         @arg @ref LL_LPTIM_LPTIM1_IC2_RMP_LSI
  *         @arg @ref LL_LPTIM_LPTIM1_IC2_RMP_LSE
  *         @arg @ref LL_LPTIM_LPTIM2_IC1_RMP_GPIO
  *         @arg @ref LL_LPTIM_LPTIM2_IC1_RMP_I3C1_IBIACK
  *         @arg @ref LL_LPTIM_LPTIM2_IC2_RMP_GPIO
  *         @arg @ref LL_LPTIM_LPTIM2_IC2_RMP_HSI_1024
  *         @arg @ref LL_LPTIM_LPTIM2_IC2_RMP_MSI_128
  *         @arg @ref LL_LPTIM_LPTIM2_IC2_RMP_I3C2_IBIACK
  *         @arg @ref LL_LPTIM_LPTIM3_IC1_RMP_GPIO
  *         @arg @ref LL_LPTIM_LPTIM3_IC1_RMP_I3C1_IBIACK
  *         @arg @ref LL_LPTIM_LPTIM3_IC2_RMP_GPIO
  *         @arg @ref LL_LPTIM_LPTIM3_IC2_RMP_I3C2_IBIACK
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetRemap(LPTIM_TypeDef *LPTIMx, uint32_t Src)
{
  MODIFY_REG(LPTIMx->CFGR2, LPTIM_CFGR2_IC1SEL | LPTIM_CFGR2_IC2SEL, Src);
}

/**
  * @brief  Set the polarity of IC channel 1.
  * @rmtoll CCMR1         CC1P          LL_LPTIM_IC_SetPolarity\n
  * @rmtoll CCMR1         CC2P          LL_LPTIM_IC_SetPolarity\n
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @param  Polarity This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_ICPOLARITY_RISING
  *         @arg @ref LL_LPTIM_ICPOLARITY_FALLING
  *         @arg @ref LL_LPTIM_ICPOLARITY_RISING_FALLING
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_IC_SetPolarity(LPTIM_TypeDef *LPTIMx, uint32_t Channel, uint32_t Polarity)
{
  MODIFY_REG(LPTIMx->CCMR1, LPTIM_CCMR1_CC1P << LL_LPTIM_SHIFT_TAB_CCxP[Channel],
             Polarity << LL_LPTIM_SHIFT_TAB_CCxP[Channel]);
}

/**
  * @brief  Get the polarity of IC channels.
  * @rmtoll CCMR1         CC1P          LL_LPTIM_IC_GetPolarity\n
  * @rmtoll CCMR1         CC2P          LL_LPTIM_IC_GetPolarity\n
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_ICPOLARITY_RISING
  *         @arg @ref LL_LPTIM_ICPOLARITY_FALLING
  *         @arg @ref LL_LPTIM_ICPOLARITY_RISING_FALLING
  */
__STATIC_INLINE uint32_t LL_LPTIM_IC_GetPolarity(const LPTIM_TypeDef *LPTIMx, uint32_t Channel)
{
  return (uint32_t)((READ_BIT(LPTIMx->CCMR1, LPTIM_CCMR1_CC1P << LL_LPTIM_SHIFT_TAB_CCxP[Channel])) >>                 \
                    LL_LPTIM_SHIFT_TAB_CCxP[Channel]);

}

/**
  * @brief  Set the filter of IC channels.
  * @rmtoll CCMR1         IC1F          LL_LPTIM_IC_SetFilter\n
  * @rmtoll CCMR1         IC2F          LL_LPTIM_IC_SetFilter\n
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @param  Filter This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV1
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV2
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV4
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV8
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_IC_SetFilter(LPTIM_TypeDef *LPTIMx, uint32_t Channel, uint32_t Filter)
{
  MODIFY_REG(LPTIMx->CCMR1, LPTIM_CCMR1_IC1F << LL_LPTIM_SHIFT_TAB_ICxF[Channel],
             Filter << LL_LPTIM_SHIFT_TAB_ICxF[Channel]);
}

/**
  * @brief  Get the filter of IC channels.
  * @rmtoll CCMR1         IC1F          LL_LPTIM_IC_GetFilter\n
  * @rmtoll CCMR1         IC2F          LL_LPTIM_IC_GetFilter\n
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV1
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV2
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV4
  *         @arg @ref LL_LPTIM_ICFLT_CLOCK_DIV8
  */
__STATIC_INLINE uint32_t LL_LPTIM_IC_GetFilter(const LPTIM_TypeDef *LPTIMx, uint32_t Channel)
{
  return (uint32_t)((READ_BIT(LPTIMx->CCMR1, LPTIM_CCMR1_IC1F << LL_LPTIM_SHIFT_TAB_ICxF[Channel])) >>                 \
                    LL_LPTIM_SHIFT_TAB_ICxF[Channel]);
}

/**
  * @brief  Set the prescaler of IC channels.
  * @rmtoll CCMR1         IC1PSC        LL_LPTIM_IC_SetPrescaler\n
  * @rmtoll CCMR1         IC2PSC        LL_LPTIM_IC_SetPrescaler\n
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_ICPSC_DIV1
  *         @arg @ref LL_LPTIM_ICPSC_DIV2
  *         @arg @ref LL_LPTIM_ICPSC_DIV4
  *         @arg @ref LL_LPTIM_ICPSC_DIV8
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_IC_SetPrescaler(LPTIM_TypeDef *LPTIMx, uint32_t Channel, uint32_t Prescaler)
{
  MODIFY_REG(LPTIMx->CCMR1, LPTIM_CCMR1_IC1PSC << LL_LPTIM_SHIFT_TAB_ICxPSC[Channel],
             Prescaler << LL_LPTIM_SHIFT_TAB_ICxPSC[Channel]);
}

/**
  * @brief  Get the prescaler of IC channels.
  * @rmtoll CCMR1         IC1PSC        LL_LPTIM_IC_GetPrescaler\n
  * @rmtoll CCMR1         IC2PSC        LL_LPTIM_IC_GetPrescaler\n
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_ICPSC_DIV1
  *         @arg @ref LL_LPTIM_ICPSC_DIV2
  *         @arg @ref LL_LPTIM_ICPSC_DIV4
  *         @arg @ref LL_LPTIM_ICPSC_DIV8
  */
__STATIC_INLINE uint32_t LL_LPTIM_IC_GetPrescaler(const LPTIM_TypeDef *LPTIMx, uint32_t Channel)
{
  return (uint32_t)((READ_BIT(LPTIMx->CCMR1, LPTIM_CCMR1_IC1PSC << LL_LPTIM_SHIFT_TAB_ICxPSC[Channel])) >>             \
                    LL_LPTIM_SHIFT_TAB_ICxPSC[Channel]);
}

/**
  * @brief  Set the Channel  Mode.
  * @rmtoll CCMR1         CC1SEL          LL_LPTIM_CC_SetChannelMode\n
  *         CCMR1         CC2SEL          LL_LPTIM_CC_SetChannelMode
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @param  CCMode This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CCMODE_OUTPUT_PWM
  *         @arg @ref LL_LPTIM_CCMODE_INPUTCAPTURE
  * @retval None
  */
__STATIC_INLINE void  LL_LPTIM_CC_SetChannelMode(LPTIM_TypeDef *LPTIMx, uint32_t Channel, uint32_t CCMode)
{
  MODIFY_REG(LPTIMx->CCMR1, LPTIM_CCMR1_CC1SEL << LL_LPTIM_SHIFT_TAB_CCxSEL[Channel],
             CCMode << LL_LPTIM_SHIFT_TAB_CCxSEL[Channel]);
}

/**
  * @brief  Get the Channel  Mode.
  * @rmtoll CCMR1         CC1SEL          LL_LPTIM_CC_GetChannelMode\n
  *         CCMR1         CC2SEL          LL_LPTIM_CC_GetChannelMode
  * @param  LPTIMx Low-Power Timer instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CHANNEL_CH1
  *         @arg @ref LL_LPTIM_CHANNEL_CH2
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_CCMODE_OUTPUT_PWM
  *         @arg @ref LL_LPTIM_CCMODE_INPUTCAPTURE
  */
__STATIC_INLINE uint32_t  LL_LPTIM_CC_GetChannelMode(const LPTIM_TypeDef *LPTIMx, uint32_t Channel)
{
  return (uint32_t)((READ_BIT(LPTIMx->CCMR1, LPTIM_CCMR1_CC1SEL << LL_LPTIM_SHIFT_TAB_CCxSEL[Channel])) >>             \
                    LL_LPTIM_SHIFT_TAB_CCxSEL[Channel]);
}

/**
  * @brief  Get captured value for input channel 1.
  * @rmtoll CCR1         CCR1          LL_LPTIM_IC_GetCaptureCH1
  * @note   The real capture value corresponding to the input capture trigger can be calculated using
  *         the formula hereafter : Real capture value = captured(LPTIM_CCRx) - offset
  *         where offset can be retrieved by calling @ref LL_LPTIM_IC_GET_OFFSET
  * @param  LPTIMx Low-Power Timer instance
  * @retval CapturedValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_LPTIM_IC_GetCaptureCH1(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CCR1, LPTIM_CCR1_CCR1));
}

/**
  * @brief  Get captured value for input channel 2.
  * @rmtoll CCR2         CCR2          LL_LPTIM_IC_GetCaptureCH2
  * @note   The real capture value corresponding to the input capture trigger can be calculated using
  *         the formula hereafter : Real capture value = captured(LPTIM_CCRx) - offset
  *         where offset can be retrieved by calling @ref LL_LPTIM_IC_GET_OFFSET
  * @param  LPTIMx Low-Power Timer instance
  * @retval CapturedValue (between Min_Data=0 and Max_Data=65535)
  */
__STATIC_INLINE uint32_t LL_LPTIM_IC_GetCaptureCH2(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CCR2, LPTIM_CCR2_CCR2));
}

/**
  * @}
  */

/** @defgroup LPTIM_LL_EF_Trigger_Configuration Trigger Configuration
  * @{
  */

/**
  * @brief  Enable the timeout function
  * @note This function must be called when the LPTIM instance is disabled.
  * @note The first trigger event will start the timer, any successive trigger
  *       event will reset the counter and the timer will restart.
  * @note The timeout value corresponds to the compare value; if no trigger
  *       occurs within the expected time frame, the MCU is waked-up by the
  *       compare match event.
  * @rmtoll CFGR         TIMOUT        LL_LPTIM_EnableTimeout
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableTimeout(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->CFGR, LPTIM_CFGR_TIMOUT);
}

/**
  * @brief  Disable the timeout function
  * @note This function must be called when the LPTIM instance is disabled.
  * @note A trigger event arriving when the timer is already started will be
  *       ignored.
  * @rmtoll CFGR         TIMOUT        LL_LPTIM_DisableTimeout
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableTimeout(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->CFGR, LPTIM_CFGR_TIMOUT);
}

/**
  * @brief  Indicate whether the timeout function is enabled.
  * @rmtoll CFGR         TIMOUT        LL_LPTIM_IsEnabledTimeout
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledTimeout(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_TIMOUT) == LPTIM_CFGR_TIMOUT) ? 1UL : 0UL));
}

/**
  * @brief  Start the LPTIM counter
  * @note This function must be called when the LPTIM instance is disabled.
  * @rmtoll CFGR         TRIGEN        LL_LPTIM_TrigSw
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_TrigSw(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->CFGR, LPTIM_CFGR_TRIGEN);
}

/**
  * @brief  Configure the external trigger used as a trigger event for the LPTIM.
  * @note This function must be called when the LPTIM instance is disabled.
  * @note An internal clock source must be present when a digital filter is
  *       required for the trigger.
  * @rmtoll CFGR         TRIGSEL       LL_LPTIM_ConfigTrigger\n
  *         CFGR         TRGFLT        LL_LPTIM_ConfigTrigger\n
  *         CFGR         TRIGEN        LL_LPTIM_ConfigTrigger
  * @param  LPTIMx Low-Power Timer instance
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_GPIO
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCALARMA
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCALARMB
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCTAMP1
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCTAMP2
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCTAMP3
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_SAI2_FS_A
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_SAI2_FS_B
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_LPTIM2_CH1
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_LPTIM3_CH1
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_LPTIM4_OUT
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_LPTIM5_OUT
  * @param  Filter This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_TRIG_FILTER_NONE
  *         @arg @ref LL_LPTIM_TRIG_FILTER_2
  *         @arg @ref LL_LPTIM_TRIG_FILTER_4
  *         @arg @ref LL_LPTIM_TRIG_FILTER_8
  * @param  Polarity This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_TRIG_POLARITY_RISING
  *         @arg @ref LL_LPTIM_TRIG_POLARITY_FALLING
  *         @arg @ref LL_LPTIM_TRIG_POLARITY_RISING_FALLING
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ConfigTrigger(LPTIM_TypeDef *LPTIMx, uint32_t Source, uint32_t Filter, uint32_t Polarity)
{
  MODIFY_REG(LPTIMx->CFGR, LPTIM_CFGR_TRIGSEL | LPTIM_CFGR_TRGFLT | LPTIM_CFGR_TRIGEN, Source | Filter | Polarity);
}

/**
  * @brief  Get actual external trigger source.
  * @rmtoll CFGR         TRIGSEL       LL_LPTIM_GetTriggerSource
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_GPIO
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCALARMA
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCALARMB
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCTAMP1
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCTAMP2
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_RTCTAMP3
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_SAI2_FS_A
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_SAI2_FS_B
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_LPTIM2_CH1
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_LPTIM3_CH1
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_LPTIM4_OUT
  *         @arg @ref LL_LPTIM_TRIG_SOURCE_LPTIM5_OUT
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetTriggerSource(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_TRIGSEL));
}

/**
  * @brief  Get actual external trigger filter.
  * @rmtoll CFGR         TRGFLT        LL_LPTIM_GetTriggerFilter
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_TRIG_FILTER_NONE
  *         @arg @ref LL_LPTIM_TRIG_FILTER_2
  *         @arg @ref LL_LPTIM_TRIG_FILTER_4
  *         @arg @ref LL_LPTIM_TRIG_FILTER_8
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetTriggerFilter(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_TRGFLT));
}

/**
  * @brief  Get actual external trigger polarity.
  * @rmtoll CFGR         TRIGEN        LL_LPTIM_GetTriggerPolarity
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_TRIG_POLARITY_RISING
  *         @arg @ref LL_LPTIM_TRIG_POLARITY_FALLING
  *         @arg @ref LL_LPTIM_TRIG_POLARITY_RISING_FALLING
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetTriggerPolarity(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_TRIGEN));
}

/**
  * @}
  */

/** @defgroup LPTIM_LL_EF_Clock_Configuration Clock Configuration
  * @{
  */

/**
  * @brief  Set the source of the clock used by the LPTIM instance.
  * @note This function must be called when the LPTIM instance is disabled.
  * @rmtoll CFGR         CKSEL         LL_LPTIM_SetClockSource
  * @param  LPTIMx Low-Power Timer instance
  * @param  ClockSource This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CLK_SOURCE_INTERNAL
  *         @arg @ref LL_LPTIM_CLK_SOURCE_EXTERNAL
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetClockSource(LPTIM_TypeDef *LPTIMx, uint32_t ClockSource)
{
  MODIFY_REG(LPTIMx->CFGR, LPTIM_CFGR_CKSEL, ClockSource);
}

/**
  * @brief  Get actual LPTIM instance clock source.
  * @rmtoll CFGR         CKSEL         LL_LPTIM_GetClockSource
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_CLK_SOURCE_INTERNAL
  *         @arg @ref LL_LPTIM_CLK_SOURCE_EXTERNAL
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetClockSource(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_CKSEL));
}

/**
  * @brief  Configure the active edge or edges used by the counter when
            the LPTIM is clocked by an external clock source.
  * @note This function must be called when the LPTIM instance is disabled.
  * @note When both external clock signal edges are considered active ones,
  *       the LPTIM must also be clocked by an internal clock source with a
  *       frequency equal to at least four times the external clock frequency.
  * @note An internal clock source must be present when a digital filter is
  *       required for external clock.
  * @rmtoll CFGR         CKFLT         LL_LPTIM_ConfigClock\n
  *         CFGR         CKPOL         LL_LPTIM_ConfigClock
  * @param  LPTIMx Low-Power Timer instance
  * @param  ClockFilter This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CLK_FILTER_NONE
  *         @arg @ref LL_LPTIM_CLK_FILTER_2
  *         @arg @ref LL_LPTIM_CLK_FILTER_4
  *         @arg @ref LL_LPTIM_CLK_FILTER_8
  * @param  ClockPolarity This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_CLK_POLARITY_RISING
  *         @arg @ref LL_LPTIM_CLK_POLARITY_FALLING
  *         @arg @ref LL_LPTIM_CLK_POLARITY_RISING_FALLING
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ConfigClock(LPTIM_TypeDef *LPTIMx, uint32_t ClockFilter, uint32_t ClockPolarity)
{
  MODIFY_REG(LPTIMx->CFGR, LPTIM_CFGR_CKFLT | LPTIM_CFGR_CKPOL, ClockFilter | ClockPolarity);
}

/**
  * @brief  Get actual clock polarity
  * @rmtoll CFGR         CKPOL         LL_LPTIM_GetClockPolarity
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_CLK_POLARITY_RISING
  *         @arg @ref LL_LPTIM_CLK_POLARITY_FALLING
  *         @arg @ref LL_LPTIM_CLK_POLARITY_RISING_FALLING
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetClockPolarity(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_CKPOL));
}

/**
  * @brief  Get actual clock digital filter
  * @rmtoll CFGR         CKFLT         LL_LPTIM_GetClockFilter
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_CLK_FILTER_NONE
  *         @arg @ref LL_LPTIM_CLK_FILTER_2
  *         @arg @ref LL_LPTIM_CLK_FILTER_4
  *         @arg @ref LL_LPTIM_CLK_FILTER_8
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetClockFilter(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_CKFLT));
}

/**
  * @}
  */

/** @defgroup LPTIM_LL_EF_Encoder_Mode Encoder Mode
  * @{
  */

/**
  * @brief  Configure the encoder mode.
  * @note This function must be called when the LPTIM instance is disabled.
  * @rmtoll CFGR         CKPOL         LL_LPTIM_SetEncoderMode
  * @param  LPTIMx Low-Power Timer instance
  * @param  EncoderMode This parameter can be one of the following values:
  *         @arg @ref LL_LPTIM_ENCODER_MODE_RISING
  *         @arg @ref LL_LPTIM_ENCODER_MODE_FALLING
  *         @arg @ref LL_LPTIM_ENCODER_MODE_RISING_FALLING
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_SetEncoderMode(LPTIM_TypeDef *LPTIMx, uint32_t EncoderMode)
{
  MODIFY_REG(LPTIMx->CFGR, LPTIM_CFGR_CKPOL, EncoderMode);
}

/**
  * @brief  Get actual encoder mode.
  * @rmtoll CFGR         CKPOL         LL_LPTIM_GetEncoderMode
  * @param  LPTIMx Low-Power Timer instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_LPTIM_ENCODER_MODE_RISING
  *         @arg @ref LL_LPTIM_ENCODER_MODE_FALLING
  *         @arg @ref LL_LPTIM_ENCODER_MODE_RISING_FALLING
  */
__STATIC_INLINE uint32_t LL_LPTIM_GetEncoderMode(const LPTIM_TypeDef *LPTIMx)
{
  return (uint32_t)(READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_CKPOL));
}

/**
  * @brief  Enable the encoder mode
  * @note This function must be called when the LPTIM instance is disabled.
  * @note In this mode the LPTIM instance must be clocked by an internal clock
  *       source. Also, the prescaler division ratio must be equal to 1.
  * @note LPTIM instance must be configured in continuous mode prior enabling
  *       the encoder mode.
  * @rmtoll CFGR         ENC           LL_LPTIM_EnableEncoderMode
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableEncoderMode(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->CFGR, LPTIM_CFGR_ENC);
}

/**
  * @brief  Disable the encoder mode
  * @note This function must be called when the LPTIM instance is disabled.
  * @rmtoll CFGR         ENC           LL_LPTIM_DisableEncoderMode
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableEncoderMode(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->CFGR, LPTIM_CFGR_ENC);
}

/**
  * @brief  Indicates whether the LPTIM operates in encoder mode.
  * @rmtoll CFGR         ENC           LL_LPTIM_IsEnabledEncoderMode
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledEncoderMode(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->CFGR, LPTIM_CFGR_ENC) == LPTIM_CFGR_ENC) ? 1UL : 0UL));
}

/**
  * @}
  */

/** @defgroup LPTIM_LL_EF_FLAG_Management FLAG Management
  * @{
  */

/**
  * @brief  Clear the compare match flag for channel 1 (CC1CF)
  * @rmtoll ICR          CC1CF        LL_LPTIM_ClearFlag_CC1
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_CC1(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_CC1CF);
}

/**
  * @brief  Inform application whether a capture/compare interrupt has occurred for channel 1.
  * @rmtoll ISR          CC1IF         LL_LPTIM_IsActiveFlag_CC1
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_CC1(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_CC1IF) == LPTIM_ISR_CC1IF) ? 1UL : 0UL));
}

/**
  * @brief  Clear the compare match flag for channel 2 (CC2CF)
  * @rmtoll ICR          CC2CF        LL_LPTIM_ClearFlag_CC2
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_CC2(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_CC2CF);
}

/**
  * @brief  Inform application whether a capture/compare interrupt has occurred for channel 2.
  * @rmtoll ISR          CC2IF          LL_LPTIM_IsActiveFlag_CC2
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_CC2(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_CC2IF) == LPTIM_ISR_CC2IF) ? 1UL : 0UL));
}

/**
  * @brief  Clear the Capture/Compare 1 over-capture flag for channel 1 (CC1OCF)
  * @rmtoll ICR          CC1OCF       LL_LPTIM_ClearFlag_CC1O
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_CC1O(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_CC1OCF);
}

/**
  * @brief  Inform application whether a Capture/Compare 1 over-capture  has occurred for channel 1.
  * @rmtoll ISR          CC1OF          LL_LPTIM_IsActiveFlag_CC1O
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_CC1O(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_CC1OF) == LPTIM_ISR_CC1OF) ? 1UL : 0UL));
}

/**
  * @brief  Clear the Capture/Compare 2 over-capture flag for channel 2 (CC2OCF)
  * @rmtoll ICR          CC2OCF       LL_LPTIM_ClearFlag_CC2O
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_CC2O(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_CC2OCF);
}

/**
  * @brief  Inform application whether a Capture/Compare 2 over-capture  has occurred for channel 2.
  * @rmtoll ISR          CC2OF          LL_LPTIM_IsActiveFlag_CC2O
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_CC2O(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_CC2OF) == LPTIM_ISR_CC2OF) ? 1UL : 0UL));
}
/**
  * @brief  Clear the autoreload match flag (ARRMCF)
  * @rmtoll ICR          ARRMCF        LL_LPTIM_ClearFlag_ARRM
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_ARRM(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_ARRMCF);
}

/**
  * @brief  Inform application whether a autoreload match interrupt has occurred.
  * @rmtoll ISR          ARRM          LL_LPTIM_IsActiveFlag_ARRM
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_ARRM(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_ARRM) == LPTIM_ISR_ARRM) ? 1UL : 0UL));
}

/**
  * @brief  Clear the external trigger valid edge flag(EXTTRIGCF).
  * @rmtoll ICR          EXTTRIGCF     LL_LPTIM_ClearFlag_EXTTRIG
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_EXTTRIG(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_EXTTRIGCF);
}

/**
  * @brief  Inform application whether a valid edge on the selected external trigger input has occurred.
  * @rmtoll ISR          EXTTRIG       LL_LPTIM_IsActiveFlag_EXTTRIG
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_EXTTRIG(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_EXTTRIG) == LPTIM_ISR_EXTTRIG) ? 1UL : 0UL));
}

/**
  * @brief  Clear the compare register update interrupt flag (CMP1OKCF).
  * @rmtoll ICR          CMP1OKCF       LL_LPTIM_ClearFlag_CMP1OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_CMP1OK(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_CMP1OKCF);
}

/**
  * @brief  Informs application whether the APB bus write operation to the LPTIMx_CCR1 register has been successfully
            completed. If so, a new one can be initiated.
  * @rmtoll ISR          CMP1OK         LL_LPTIM_IsActiveFlag_CMP1OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_CMP1OK(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_CMP1OK) == LPTIM_ISR_CMP1OK) ? 1UL : 0UL));
}

/**
  * @brief  Clear the compare register update interrupt flag (CMP2OKCF).
  * @rmtoll ICR          CMP2OKCF       LL_LPTIM_ClearFlag_CMP2OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_CMP2OK(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_CMP2OKCF);
}

/**
  * @brief  Informs application whether the APB bus write operation to the LPTIMx_CCR2 register has been successfully
            completed. If so, a new one can be initiated.
  * @rmtoll ISR          CMP2OK         LL_LPTIM_IsActiveFlag_CMP2OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_CMP2OK(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_CMP2OK) == LPTIM_ISR_CMP2OK) ? 1UL : 0UL));
}

/**
  * @brief  Clear the interrupt register update interrupt flag (DIEROKCF).
  * @rmtoll ICR       DIEROKCF          LL_LPTIM_ClearFlag_DIEROK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_DIEROK(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_DIEROKCF);
}

/**
  * @brief  Informs application whether the APB bus write operation to the LPTIMx_DIER register has been successfully
            completed. If so, a new one can be initiated.
  * @rmtoll ISR          DIEROK            LL_LPTIM_IsActiveFlag_DIEROK
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_DIEROK(const LPTIM_TypeDef *LPTIMx)
{
  return ((READ_BIT(LPTIMx->ISR, LPTIM_ISR_DIEROK) == (LPTIM_ISR_DIEROK)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the autoreload register update interrupt flag (ARROKCF).
  * @rmtoll ICR          ARROKCF       LL_LPTIM_ClearFlag_ARROK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_ARROK(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_ARROKCF);
}

/**
  * @brief  Informs application whether the APB bus write operation to the LPTIMx_ARR register has been successfully
            completed. If so, a new one can be initiated.
  * @rmtoll ISR          ARROK         LL_LPTIM_IsActiveFlag_ARROK
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_ARROK(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_ARROK) == LPTIM_ISR_ARROK) ? 1UL : 0UL));
}

/**
  * @brief  Clear the counter direction change to up interrupt flag (UPCF).
  * @rmtoll ICR          UPCF          LL_LPTIM_ClearFlag_UP
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_UP(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_UPCF);
}

/**
  * @brief  Informs the application whether the counter direction has changed from down to up (when the LPTIM instance
            operates in encoder mode).
  * @rmtoll ISR          UP            LL_LPTIM_IsActiveFlag_UP
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_UP(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_UP) == LPTIM_ISR_UP) ? 1UL : 0UL));
}

/**
  * @brief  Clear the counter direction change to down interrupt flag (DOWNCF).
  * @rmtoll ICR          DOWNCF        LL_LPTIM_ClearFlag_DOWN
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_DOWN(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_DOWNCF);
}

/**
  * @brief  Informs the application whether the counter direction has changed from up to down (when the LPTIM instance
            operates in encoder mode).
  * @rmtoll ISR          DOWN          LL_LPTIM_IsActiveFlag_DOWN
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_DOWN(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->ISR, LPTIM_ISR_DOWN) == LPTIM_ISR_DOWN) ? 1UL : 0UL));
}

/**
  * @brief  Clear the repetition register update interrupt flag (REPOKCF).
  * @rmtoll ICR          REPOKCF       LL_LPTIM_ClearFlag_REPOK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_REPOK(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_REPOKCF);
}

/**
  * @brief  Informs application whether the APB bus write operation to the LPTIMx_RCR register has been successfully
            completed; If so, a new one can be initiated.
  * @rmtoll ISR          REPOK         LL_LPTIM_IsActiveFlag_REPOK
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_REPOK(const LPTIM_TypeDef *LPTIMx)
{
  return ((READ_BIT(LPTIMx->ISR, LPTIM_ISR_REPOK) == (LPTIM_ISR_REPOK)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the update event flag (UECF).
  * @rmtoll ICR          UECF          LL_LPTIM_ClearFlag_UE
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_ClearFlag_UE(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->ICR, LPTIM_ICR_UECF);
}

/**
  * @brief  Informs application whether the LPTIMx update event has occurred.
  * @rmtoll ISR          UE            LL_LPTIM_IsActiveFlag_UE
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsActiveFlag_UE(const LPTIM_TypeDef *LPTIMx)
{
  return ((READ_BIT(LPTIMx->ISR, LPTIM_ISR_UE) == (LPTIM_ISR_UE)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup LPTIM_LL_EF_IT_Management Interrupt Management
  * @{
  */
/**
  * @brief  Enable capture/compare 1 interrupt (CC1IE).
  * @rmtoll DIER         CC1IE        LL_LPTIM_EnableIT_CC1
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_CC1(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_CC1IE);
}

/**
  * @brief  Disable capture/compare 1 interrupt (CC1IE).
  * @rmtoll DIER          CC1IE        LL_LPTIM_DisableIT_CC1
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_CC1(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_CC1IE);
}

/**
  * @brief  Indicates whether the capture/compare 1 interrupt (CC1IE) is enabled.
  * @rmtoll DIER          CC1IE        LL_LPTIM_IsEnabledIT_CC1
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_CC1(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_CC1IE) == LPTIM_DIER_CC1IE) ? 1UL : 0UL));
}

/**
  * @brief  Enable capture/compare 1 interrupt (CC2IE).
  * @rmtoll DIER         CC2IE        LL_LPTIM_EnableIT_CC2
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_CC2(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_CC2IE);
}

/**
  * @brief  Disable capture/compare 2 interrupt (CC2IE).
  * @rmtoll DIER          CC2IE        LL_LPTIM_DisableIT_CC2
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_CC2(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_CC2IE);
}

/**
  * @brief  Indicates whether the capture/compare 2 interrupt (CC2IE) is enabled.
  * @rmtoll DIER          CC2IE        LL_LPTIM_IsEnabledIT_CC2
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_CC2(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_CC2IE) == LPTIM_DIER_CC2IE) ? 1UL : 0UL));
}

/**
  * @brief  Enable capture/compare 1 over-capture interrupt (CC1OIE).
  * @rmtoll DIER         CC1OIE        LL_LPTIM_EnableIT_CC1O
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_CC1O(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_CC1OIE);
}

/**
  * @brief  Disable capture/compare 1 over-capture interrupt (CC1OIE).
  * @rmtoll DIER          CC1OIE        LL_LPTIM_DisableIT_CC1O
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_CC1O(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_CC1OIE);
}

/**
  * @brief  Indicates whether the capture/compare 1 over-capture interrupt (CC1OIE) is enabled.
  * @rmtoll DIER          CC1OIE        LL_LPTIM_IsEnabledIT_CC1O
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_CC1O(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_CC1OIE) == LPTIM_DIER_CC1OIE) ? 1UL : 0UL));
}

/**
  * @brief  Enable capture/compare 1 over-capture interrupt (CC2OIE).
  * @rmtoll DIER         CC2OIE        LL_LPTIM_EnableIT_CC2O
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_CC2O(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_CC2OIE);
}

/**
  * @brief  Disable capture/compare 1 over-capture interrupt (CC2OIE).
  * @rmtoll DIER          CC2OIE        LL_LPTIM_DisableIT_CC2O
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_CC2O(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_CC2OIE);
}

/**
  * @brief  Indicates whether the capture/compare 2 over-capture interrupt (CC2OIE) is enabled.
  * @rmtoll DIER          CC2OIE        LL_LPTIM_IsEnabledIT_CC2O
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_CC2O(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_CC2OIE) == LPTIM_DIER_CC2OIE) ? 1UL : 0UL));
}

/**
  * @brief  Enable autoreload match interrupt (ARRMIE).
  * @rmtoll DIER          ARRMIE        LL_LPTIM_EnableIT_ARRM
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_ARRM(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_ARRMIE);
}

/**
  * @brief  Disable autoreload match interrupt (ARRMIE).
  * @rmtoll DIER          ARRMIE        LL_LPTIM_DisableIT_ARRM
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_ARRM(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_ARRMIE);
}

/**
  * @brief  Indicates whether the autoreload match interrupt (ARRMIE) is enabled.
  * @rmtoll DIER          ARRMIE        LL_LPTIM_IsEnabledIT_ARRM
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_ARRM(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_ARRMIE) == LPTIM_DIER_ARRMIE) ? 1UL : 0UL));
}

/**
  * @brief  Enable external trigger valid edge interrupt (EXTTRIGIE).
  * @rmtoll DIER          EXTTRIGIE     LL_LPTIM_EnableIT_EXTTRIG
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_EXTTRIG(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_EXTTRIGIE);
}

/**
  * @brief  Disable external trigger valid edge interrupt (EXTTRIGIE).
  * @rmtoll DIER          EXTTRIGIE     LL_LPTIM_DisableIT_EXTTRIG
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_EXTTRIG(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_EXTTRIGIE);
}

/**
  * @brief  Indicates external trigger valid edge interrupt (EXTTRIGIE) is enabled.
  * @rmtoll DIER          EXTTRIGIE     LL_LPTIM_IsEnabledIT_EXTTRIG
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_EXTTRIG(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_EXTTRIGIE) == LPTIM_DIER_EXTTRIGIE) ? 1UL : 0UL));
}

/**
  * @brief  Enable compare register write completed interrupt (CMP1OKIE).
  * @rmtoll IER          CMP1OKIE       LL_LPTIM_EnableIT_CMP1OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_CMP1OK(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_CMP1OKIE);
}

/**
  * @brief  Disable compare register write completed interrupt (CMP1OKIE).
  * @rmtoll IER          CMPO1KIE       LL_LPTIM_DisableIT_CMP1OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_CMP1OK(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_CMP1OKIE);
}

/**
  * @brief  Indicates whether the compare register write completed interrupt (CMP1OKIE) is enabled.
  * @rmtoll IER          CMP1OKIE       LL_LPTIM_IsEnabledIT_CMP1OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_CMP1OK(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_CMP1OKIE) == LPTIM_DIER_CMP1OKIE) ? 1UL : 0UL));
}

/**
  * @brief  Enable compare register write completed interrupt (CMP2OKIE).
  * @rmtoll IER          CMP2OKIE       LL_LPTIM_EnableIT_CMP2OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_CMP2OK(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_CMP2OKIE);
}

/**
  * @brief  Disable compare register write completed interrupt (CMP2OKIE).
  * @rmtoll IER          CMP2OKIE       LL_LPTIM_DisableIT_CMP2OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_CMP2OK(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_CMP2OKIE);
}

/**
  * @brief  Indicates whether the compare register write completed interrupt (CMP2OKIE) is enabled.
  * @rmtoll IER          CMP2OKIE       LL_LPTIM_IsEnabledIT_CMP2OK
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_CMP2OK(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_CMP2OKIE) == LPTIM_DIER_CMP2OKIE) ? 1UL : 0UL));
}

/**
  * @brief  Enable autoreload register write completed interrupt (ARROKIE).
  * @rmtoll DIER         ARROKIE       LL_LPTIM_EnableIT_ARROK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_ARROK(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_ARROKIE);
}

/**
  * @brief  Disable autoreload register write completed interrupt (ARROKIE).
  * @rmtoll DIER         ARROKIE       LL_LPTIM_DisableIT_ARROK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_ARROK(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_ARROKIE);
}

/**
  * @brief  Indicates whether the autoreload register write completed interrupt (ARROKIE) is enabled.
  * @rmtoll DIER         ARROKIE       LL_LPTIM_IsEnabledIT_ARROK
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit(1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_ARROK(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_ARROKIE) == LPTIM_DIER_ARROKIE) ? 1UL : 0UL));
}

/**
  * @brief  Enable direction change to up interrupt (UPIE).
  * @rmtoll DIER         UPIE          LL_LPTIM_EnableIT_UP
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_UP(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_UPIE);
}

/**
  * @brief  Disable direction change to up interrupt (UPIE).
  * @rmtoll DIER         UPIE          LL_LPTIM_DisableIT_UP
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_UP(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_UPIE);
}

/**
  * @brief  Indicates whether the direction change to up interrupt (UPIE) is enabled.
  * @rmtoll DIER         UPIE          LL_LPTIM_IsEnabledIT_UP
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit(1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_UP(const LPTIM_TypeDef *LPTIMx)
{
  return (((READ_BIT(LPTIMx->DIER, LPTIM_DIER_UPIE) == LPTIM_DIER_UPIE) ? 1UL : 0UL));
}

/**
  * @brief  Enable direction change to down interrupt (DOWNIE).
  * @rmtoll DIER         DOWNIE        LL_LPTIM_EnableIT_DOWN
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_DOWN(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_DOWNIE);
}

/**
  * @brief  Disable direction change to down interrupt (DOWNIE).
  * @rmtoll DIER         DOWNIE        LL_LPTIM_DisableIT_DOWN
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_DOWN(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_DOWNIE);
}

/**
  * @brief  Indicates whether the direction change to down interrupt (DOWNIE) is enabled.
  * @rmtoll DIER         DOWNIE        LL_LPTIM_IsEnabledIT_DOWN
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit(1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_DOWN(const LPTIM_TypeDef *LPTIMx)
{
  return ((READ_BIT(LPTIMx->DIER, LPTIM_DIER_DOWNIE) == LPTIM_DIER_DOWNIE) ? 1UL : 0UL);
}

/**
  * @brief  Enable repetition register update successfully completed interrupt (REPOKIE).
  * @rmtoll DIER         REPOKIE       LL_LPTIM_EnableIT_REPOK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_REPOK(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_REPOKIE);
}

/**
  * @brief  Disable repetition register update successfully completed interrupt (REPOKIE).
  * @rmtoll DIER         REPOKIE       LL_LPTIM_DisableIT_REPOK
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_REPOK(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_REPOKIE);
}

/**
  * @brief  Indicates whether the repetition register update successfully completed interrupt (REPOKIE) is enabled.
  * @rmtoll DIER         REPOKIE       LL_LPTIM_IsEnabledIT_REPOK
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit(1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_REPOK(const LPTIM_TypeDef *LPTIMx)
{
  return ((READ_BIT(LPTIMx->DIER, LPTIM_DIER_REPOKIE) == (LPTIM_DIER_REPOKIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable update event interrupt (UEIE).
  * @rmtoll DIER         UEIE          LL_LPTIM_EnableIT_UE
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableIT_UE(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_UEIE);
}

/**
  * @brief  Disable update event interrupt (UEIE).
  * @rmtoll DIER          UEIE         LL_LPTIM_DisableIT_UE
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableIT_UE(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_UEIE);
}

/**
  * @brief  Indicates whether the update event interrupt (UEIE) is enabled.
  * @rmtoll DIER         UEIE          LL_LPTIM_IsEnabledIT_UE
  * @param  LPTIMx Low-Power Timer instance
  *@ retval State of bit(1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledIT_UE(const LPTIM_TypeDef *LPTIMx)
{
  return ((READ_BIT(LPTIMx->DIER, LPTIM_DIER_UEIE) == (LPTIM_DIER_UEIE)) ? 1UL : 0UL);
}
/**
  * @}
  */


/** @defgroup TIM_LL_EF_DMA_Management DMA Management
  * @{
  */
/**
  * @brief  Enable update DMA request.
  * @rmtoll DIER         UEDE          LL_LPTIM_EnableDMAReq_UPDATE
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableDMAReq_UPDATE(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_UEDE);
}

/**
  * @brief  Disable update DMA request.
  * @rmtoll DIER         UEDE          LL_LPTIM_DisableDMAReq_UPDATE
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableDMAReq_UPDATE(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_UEDE);
}

/**
  * @brief  Indicates whether the update DMA request is enabled.
  * @rmtoll DIER         UEDE          LL_LPTIM_IsEnabledDMAReq_UPDATE
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledDMAReq_UPDATE(const LPTIM_TypeDef *LPTIMx)
{
  return ((READ_BIT(LPTIMx->DIER, LPTIM_DIER_UEDE) == (LPTIM_DIER_UEDE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 1 DMA request (CC1DE).
  * @rmtoll DIER         CC1DE         LL_LPTIM_EnableDMAReq_CC1
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableDMAReq_CC1(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_CC1DE);
}

/**
  * @brief  Disable capture/compare 1  DMA request (CC1DE).
  * @rmtoll DIER         CC1DE         LL_LPTIM_DisableDMAReq_CC1
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableDMAReq_CC1(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_CC1DE);
}

/**
  * @brief  Indicates whether the capture/compare 1 DMA request (CC1DE) is enabled.
  * @rmtoll DIER         CC1DE         LL_LPTIM_IsEnabledDMAReq_CC1
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledDMAReq_CC1(const LPTIM_TypeDef *LPTIMx)
{
  return ((READ_BIT(LPTIMx->DIER, LPTIM_DIER_CC1DE) == (LPTIM_DIER_CC1DE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable capture/compare 2 DMA request (CC2DE).
  * @rmtoll DIER         CC2DE         LL_LPTIM_EnableDMAReq_CC2
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_EnableDMAReq_CC2(LPTIM_TypeDef *LPTIMx)
{
  SET_BIT(LPTIMx->DIER, LPTIM_DIER_CC2DE);
}

/**
  * @brief  Disable capture/compare 2  DMA request (CC2DE).
  * @rmtoll DIER         CC2DE         LL_LPTIM_DisableDMAReq_CC2
  * @param  LPTIMx Low-Power Timer instance
  * @retval None
  */
__STATIC_INLINE void LL_LPTIM_DisableDMAReq_CC2(LPTIM_TypeDef *LPTIMx)
{
  CLEAR_BIT(LPTIMx->DIER, LPTIM_DIER_CC2DE);
}

/**
  * @brief  Indicates whether the capture/compare 2 DMA request (CC2DE) is enabled.
  * @rmtoll DIER         CC2DE         LL_LPTIM_IsEnabledDMAReq_CC2
  * @param  LPTIMx Low-Power Timer instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_LPTIM_IsEnabledDMAReq_CC2(const LPTIM_TypeDef *LPTIMx)
{
  return ((READ_BIT(LPTIMx->DIER, LPTIM_DIER_CC2DE) == (LPTIM_DIER_CC2DE)) ? 1UL : 0UL);
}

/**
  * @}
  */
/**
  * @}
  */

/**
  * @}
  */

#endif /* LPTIM1 || LPTIM2 || LPTIM3 || LPTIM4 || LPTIM5 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_LL_LPTIM_H */
