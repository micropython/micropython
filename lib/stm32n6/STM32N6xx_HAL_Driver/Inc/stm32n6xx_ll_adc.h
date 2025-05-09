/**
  ******************************************************************************
  * @file    stm32n6xx_ll_adc.h
  * @author  MCD Application Team
  * @brief   Header file of ADC LL module.
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
#ifndef STM32N6xx_LL_ADC_H
#define STM32N6xx_LL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (ADC1) || defined (ADC2)

/** @defgroup ADC_LL ADC
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup ADC_LL_Private_Constants ADC Private Constants
  * @{
  */

/* Internal mask for ADC group regular sequencer:                             */
/* To select into literal LL_ADC_REG_RANK_x the relevant bits for:            */
/* - sequencer register offset                                                */
/* - sequencer rank bits position into the selected register                  */

/* Internal register offset for ADC group regular sequencer configuration */
/* (offset placed into a spare area of literal definition) */
#define ADC_SQR1_REGOFFSET                 (0x00000000UL)
#define ADC_SQR2_REGOFFSET                 (0x00000100UL)
#define ADC_SQR3_REGOFFSET                 (0x00000200UL)
#define ADC_SQR4_REGOFFSET                 (0x00000300UL)

#define ADC_REG_SQRX_REGOFFSET_MASK        (ADC_SQR1_REGOFFSET | ADC_SQR2_REGOFFSET \
                                            | ADC_SQR3_REGOFFSET | ADC_SQR4_REGOFFSET)
#define ADC_SQRX_REGOFFSET_POS             (8UL) /* Position of bits ADC_SQRx_REGOFFSET in ADC_REG_SQRX_REGOFFSET_MASK*/
#define ADC_REG_RANK_ID_SQRX_MASK          (ADC_CHANNEL_NUMBER_MASK_POSBIT0)

/* Definition of ADC group regular sequencer bits information to be inserted  */
/* into ADC group regular sequencer ranks literals definition.                */
#define ADC_REG_RANK_1_SQRX_BITOFFSET_POS  (ADC_SQR1_SQ1_Pos)
#define ADC_REG_RANK_2_SQRX_BITOFFSET_POS  (ADC_SQR1_SQ2_Pos)
#define ADC_REG_RANK_3_SQRX_BITOFFSET_POS  (ADC_SQR1_SQ3_Pos)
#define ADC_REG_RANK_4_SQRX_BITOFFSET_POS  (ADC_SQR1_SQ4_Pos)
#define ADC_REG_RANK_5_SQRX_BITOFFSET_POS  (ADC_SQR2_SQ5_Pos)
#define ADC_REG_RANK_6_SQRX_BITOFFSET_POS  (ADC_SQR2_SQ6_Pos)
#define ADC_REG_RANK_7_SQRX_BITOFFSET_POS  (ADC_SQR2_SQ7_Pos)
#define ADC_REG_RANK_8_SQRX_BITOFFSET_POS  (ADC_SQR2_SQ8_Pos)
#define ADC_REG_RANK_9_SQRX_BITOFFSET_POS  (ADC_SQR2_SQ9_Pos)
#define ADC_REG_RANK_10_SQRX_BITOFFSET_POS (ADC_SQR3_SQ10_Pos)
#define ADC_REG_RANK_11_SQRX_BITOFFSET_POS (ADC_SQR3_SQ11_Pos)
#define ADC_REG_RANK_12_SQRX_BITOFFSET_POS (ADC_SQR3_SQ12_Pos)
#define ADC_REG_RANK_13_SQRX_BITOFFSET_POS (ADC_SQR3_SQ13_Pos)
#define ADC_REG_RANK_14_SQRX_BITOFFSET_POS (ADC_SQR3_SQ14_Pos)
#define ADC_REG_RANK_15_SQRX_BITOFFSET_POS (ADC_SQR4_SQ15_Pos)
#define ADC_REG_RANK_16_SQRX_BITOFFSET_POS (ADC_SQR4_SQ16_Pos)


/* Internal mask for ADC group injected sequencer:                            */
/* To select into literal LL_ADC_INJ_RANK_x the relevant bits for:            */
/* - data register offset                                                     */
/* - sequencer rank bits position into the selected register                  */

/* Internal register offset for ADC group injected data register */
/* (offset placed into a spare area of literal definition) */
#define ADC_JDR1_REGOFFSET                 (0x00000000UL)
#define ADC_JDR2_REGOFFSET                 (0x00000100UL)
#define ADC_JDR3_REGOFFSET                 (0x00000200UL)
#define ADC_JDR4_REGOFFSET                 (0x00000300UL)

#define ADC_INJ_JDRX_REGOFFSET_MASK        (ADC_JDR1_REGOFFSET | ADC_JDR2_REGOFFSET \
                                            | ADC_JDR3_REGOFFSET | ADC_JDR4_REGOFFSET)
#define ADC_INJ_RANK_ID_JSQR_MASK          (ADC_CHANNEL_NUMBER_MASK_POSBIT0)
#define ADC_JDRX_REGOFFSET_POS             (8UL) /* Position of bits ADC_JDRx_REGOFFSET in ADC_INJ_JDRX_REGOFFSET_MASK*/

/* Definition of ADC group injected sequencer bits information to be inserted */
/* into ADC group injected sequencer ranks literals definition.               */
#define ADC_INJ_RANK_1_JSQR_BITOFFSET_POS  (ADC_JSQR_JSQ1_Pos)
#define ADC_INJ_RANK_2_JSQR_BITOFFSET_POS  (ADC_JSQR_JSQ2_Pos)
#define ADC_INJ_RANK_3_JSQR_BITOFFSET_POS  (ADC_JSQR_JSQ3_Pos)
#define ADC_INJ_RANK_4_JSQR_BITOFFSET_POS  (ADC_JSQR_JSQ4_Pos)

/* Internal mask for ADC group regular trigger:                               */
/* To select into literal LL_ADC_REG_TRIG_x the relevant bits for:            */
/* - regular trigger source                                                   */
/* - regular trigger edge                                                     */
#define ADC_REG_TRIG_EXT_EDGE_DEFAULT   (ADC_CFGR1_EXTEN_0) /* Trigger edge set to rising edge (default setting
                                                               for compatibility with some ADC on other STM32
                                                               series having this setting set by HW default value) */

/* Mask containing trigger source masks for each of possible                  */
/* trigger edge selection duplicated with shifts [0; 4; 8; 12]                */
/* corresponding to {SW start; ext trigger; ext trigger; ext trigger}.        */
#define ADC_REG_TRIG_SOURCE_MASK            (((LL_ADC_REG_TRIG_SOFTWARE & ADC_CFGR1_EXTSEL) << (4U * 0UL)) | \
                                             ((ADC_CFGR1_EXTSEL)                            << (4U * 1UL)) | \
                                             ((ADC_CFGR1_EXTSEL)                            << (4U * 2UL)) | \
                                             ((ADC_CFGR1_EXTSEL)                            << (4U * 3UL))  )

/* Mask containing trigger edge masks for each of possible                    */
/* trigger edge selection duplicated with shifts [0; 4; 8; 12]                */
/* corresponding to {SW start; ext trigger; ext trigger; ext trigger}.        */
#define ADC_REG_TRIG_EDGE_MASK              (((LL_ADC_REG_TRIG_SOFTWARE & ADC_CFGR1_EXTEN) << (4U * 0UL)) | \
                                             ((ADC_REG_TRIG_EXT_EDGE_DEFAULT)              << (4U * 1UL)) | \
                                             ((ADC_REG_TRIG_EXT_EDGE_DEFAULT)              << (4U * 2UL)) | \
                                             ((ADC_REG_TRIG_EXT_EDGE_DEFAULT)              << (4U * 3UL))  )

/* Definition of ADC group regular trigger bits information.                  */
#define ADC_REG_TRIG_EXTSEL_BITOFFSET_POS  (ADC_CFGR1_EXTSEL_Pos)
#define ADC_REG_TRIG_EXTEN_BITOFFSET_POS   (ADC_CFGR1_EXTEN_Pos)


/* Internal mask for ADC group injected trigger:                              */
/* To select into literal LL_ADC_INJ_TRIG_x the relevant bits for:            */
/* - injected trigger source                                                  */
/* - injected trigger edge                                                    */
#define ADC_INJ_TRIG_EXT_EDGE_DEFAULT      (ADC_JSQR_JEXTEN_0) /* Trigger edge set to rising edge (default setting for
                                                                  compatibility with some ADC on other STM32 series
                                                                  having this setting set by HW default value) */

/* Mask containing trigger source masks for each of possible                  */
/* trigger edge selection duplicated with shifts [0; 4; 8; 12]                */
/* corresponding to {SW start; ext trigger; ext trigger; ext trigger}.        */
#define ADC_INJ_TRIG_SOURCE_MASK            (((LL_ADC_INJ_TRIG_SOFTWARE & ADC_JSQR_JEXTSEL)  << (4U * 0UL)) | \
                                             ((ADC_JSQR_JEXTSEL)                             << (4U * 1UL)) | \
                                             ((ADC_JSQR_JEXTSEL)                             << (4U * 2UL)) | \
                                             ((ADC_JSQR_JEXTSEL)                             << (4U * 3UL))  )

/* Mask containing trigger edge masks for each of possible                    */
/* trigger edge selection duplicated with shifts [0; 4; 8; 12]                */
/* corresponding to {SW start; ext trigger; ext trigger; ext trigger}.        */
#define ADC_INJ_TRIG_EDGE_MASK              (((LL_ADC_INJ_TRIG_SOFTWARE & ADC_JSQR_JEXTEN) << (4U * 0UL)) | \
                                             ((ADC_INJ_TRIG_EXT_EDGE_DEFAULT)              << (4U * 1UL)) | \
                                             ((ADC_INJ_TRIG_EXT_EDGE_DEFAULT)              << (4U * 2UL)) | \
                                             ((ADC_INJ_TRIG_EXT_EDGE_DEFAULT)              << (4U * 3UL))  )

/* Definition of ADC group injected trigger bits information.                 */
#define ADC_INJ_TRIG_EXTSEL_BITOFFSET_POS  (ADC_JSQR_JEXTSEL_Pos)
#define ADC_INJ_TRIG_EXTEN_BITOFFSET_POS   (ADC_JSQR_JEXTEN_Pos)


/* Internal mask for ADC channel:                                             */
/* To select into literal ADC_CHANNEL_LUT[] the relevant bits for:         */
/* - channel identifier defined by bitfield                                   */
/* - channel sampling time defined by SMPRx register offset                   */
/*   and SMPx bits positions into SMPRx register                              */

#define ADC_CHANNEL_ID_BITFIELD_MASK       (ADC_AWD2CR_AWD2CH)
/* Equivalent mask of ADC_CHANNEL_NUMBER_MASK aligned on register LSB */
#define ADC_CHANNEL_NUMBER_MASK_POSBIT0    (ADC_SQR2_SQ5) /* Equivalent to shift: (ADC_CHANNEL_NUMBER_MASK
                                                             >> [Position of bitfield "ADC_CHANNEL_NUMBER_MASK" in
                                                             register]) */

/* Channel differentiation between external channels (connected to GPIO pins)  */
/* and internal channels (connected to internal paths)                         */
#define ADC_CHANNEL_EXTERNAL               (0x00000000UL) /* Marker of external channel */
#define ADC_CHANNEL_INTERNAL_ADC1          (0x00000400UL) /* Marker of internal channel of ADC1 */
#define ADC_CHANNEL_INTERNAL_ADC2          (0x00000800UL) /* Marker of internal channel of ADC2 */

#define ADC_CHANNEL_NONE                   (0xFFU)        /* Channel literal used for non connected channels */
#define ADC_CHANNEL_NUMBER_MASK            (0x0000001FUL) /* Mask of channel number region in LL_ADC_CHANNEL_X
                                                             bitfield (values as decimal number in range [0; 255]) */
#define ADC_CHANNEL_INTERNAL_MASK          (0x0000FF00UL) /* Mask of internal channel region in LL_ADC_CHANNEL_X
                                                             bitfield (values as bitfield for each config) */

/* Internal register offset for ADC channel sampling time configuration */
/* (offset placed into a spare area of literal definition) */
#define ADC_SMPR1_REGOFFSET                (0x00000000UL)
#define ADC_SMPR2_REGOFFSET                (0x02000000UL)
#define ADC_CHANNEL_SMPRX_REGOFFSET_MASK   (ADC_SMPR1_REGOFFSET | ADC_SMPR2_REGOFFSET)
#define ADC_SMPRX_REGOFFSET_POS            (25UL)           /* Position of bits ADC_SMPRx_REGOFFSET
                                                               in ADC_CHANNEL_SMPRX_REGOFFSET_MASK */

#define ADC_CHANNEL_SMPx_BITOFFSET_MASK    (0x01F00000UL)
#define ADC_CHANNEL_SMPx_BITOFFSET_POS     (20UL)           /* Equivalent to bitfield "ADC_CHANNEL_SMPx_BITOFFSET_MASK"
                                                               position in register */

/* Definition of channels ID bitfield information to be inserted into         */
/* channels literals definition.                                              */
#define ADC_CHANNEL_0_BITFIELD             (ADC_AWD2CR_AWD2CH_0)
#define ADC_CHANNEL_1_BITFIELD             (ADC_AWD2CR_AWD2CH_1)
#define ADC_CHANNEL_2_BITFIELD             (ADC_AWD2CR_AWD2CH_2)
#define ADC_CHANNEL_3_BITFIELD             (ADC_AWD2CR_AWD2CH_3)
#define ADC_CHANNEL_4_BITFIELD             (ADC_AWD2CR_AWD2CH_4)
#define ADC_CHANNEL_5_BITFIELD             (ADC_AWD2CR_AWD2CH_5)
#define ADC_CHANNEL_6_BITFIELD             (ADC_AWD2CR_AWD2CH_6)
#define ADC_CHANNEL_7_BITFIELD             (ADC_AWD2CR_AWD2CH_7)
#define ADC_CHANNEL_8_BITFIELD             (ADC_AWD2CR_AWD2CH_8)
#define ADC_CHANNEL_9_BITFIELD             (ADC_AWD2CR_AWD2CH_9)
#define ADC_CHANNEL_10_BITFIELD            (ADC_AWD2CR_AWD2CH_10)
#define ADC_CHANNEL_11_BITFIELD            (ADC_AWD2CR_AWD2CH_11)
#define ADC_CHANNEL_12_BITFIELD            (ADC_AWD2CR_AWD2CH_12)
#define ADC_CHANNEL_13_BITFIELD            (ADC_AWD2CR_AWD2CH_13)
#define ADC_CHANNEL_14_BITFIELD            (ADC_AWD2CR_AWD2CH_14)
#define ADC_CHANNEL_15_BITFIELD            (ADC_AWD2CR_AWD2CH_15)
#define ADC_CHANNEL_16_BITFIELD            (ADC_AWD2CR_AWD2CH_16)
#define ADC_CHANNEL_17_BITFIELD            (ADC_AWD2CR_AWD2CH_17)
#define ADC_CHANNEL_18_BITFIELD            (ADC_AWD2CR_AWD2CH_18)
#define ADC_CHANNEL_19_BITFIELD            (ADC_AWD2CR_AWD2CH_19)

/* Definition of channels sampling time information to be inserted into       */
/* channels literals definition.                                              */
/* Value shifted are equivalent to bitfield "ADC_SMPRx_SMPy" position         */
/* in register.                                                               */
#define ADC_CHANNEL_0_SMP                  (ADC_SMPR1_REGOFFSET | (( 0UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_1_SMP                  (ADC_SMPR1_REGOFFSET | (( 3UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_2_SMP                  (ADC_SMPR1_REGOFFSET | (( 6UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_3_SMP                  (ADC_SMPR1_REGOFFSET | (( 9UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_4_SMP                  (ADC_SMPR1_REGOFFSET | ((12UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_5_SMP                  (ADC_SMPR1_REGOFFSET | ((15UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_6_SMP                  (ADC_SMPR1_REGOFFSET | ((18UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_7_SMP                  (ADC_SMPR1_REGOFFSET | ((21UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_8_SMP                  (ADC_SMPR1_REGOFFSET | ((24UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_9_SMP                  (ADC_SMPR1_REGOFFSET | ((27UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_10_SMP                 (ADC_SMPR2_REGOFFSET | (( 0UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_11_SMP                 (ADC_SMPR2_REGOFFSET | (( 3UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_12_SMP                 (ADC_SMPR2_REGOFFSET | (( 6UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_13_SMP                 (ADC_SMPR2_REGOFFSET | (( 9UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_14_SMP                 (ADC_SMPR2_REGOFFSET | ((12UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_15_SMP                 (ADC_SMPR2_REGOFFSET | ((15UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_16_SMP                 (ADC_SMPR2_REGOFFSET | ((18UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_17_SMP                 (ADC_SMPR2_REGOFFSET | ((21UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_18_SMP                 (ADC_SMPR2_REGOFFSET | ((24UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))
#define ADC_CHANNEL_19_SMP                 (ADC_SMPR2_REGOFFSET | ((27UL) << ADC_CHANNEL_SMPx_BITOFFSET_POS))

/* Definition of ADC channel look up table containing channels information */
static const uint32_t ADC_CHANNEL_LUT[] =
{
  (ADC_CHANNEL_0_SMP   | ADC_CHANNEL_0_BITFIELD),    /*!< ADC channel ADCx_IN0  */
  (ADC_CHANNEL_1_SMP   | ADC_CHANNEL_1_BITFIELD),    /*!< ADC channel ADCx_IN1  */
  (ADC_CHANNEL_2_SMP   | ADC_CHANNEL_2_BITFIELD),    /*!< ADC channel ADCx_IN2  */
  (ADC_CHANNEL_3_SMP   | ADC_CHANNEL_3_BITFIELD),    /*!< ADC channel ADCx_IN3  */
  (ADC_CHANNEL_4_SMP   | ADC_CHANNEL_4_BITFIELD),    /*!< ADC channel ADCx_IN4  */
  (ADC_CHANNEL_5_SMP   | ADC_CHANNEL_5_BITFIELD),    /*!< ADC channel ADCx_IN5  */
  (ADC_CHANNEL_6_SMP   | ADC_CHANNEL_6_BITFIELD),    /*!< ADC channel ADCx_IN6  */
  (ADC_CHANNEL_7_SMP   | ADC_CHANNEL_7_BITFIELD),    /*!< ADC channel ADCx_IN7  */
  (ADC_CHANNEL_8_SMP   | ADC_CHANNEL_8_BITFIELD),    /*!< ADC channel ADCx_IN9  */
  (ADC_CHANNEL_9_SMP   | ADC_CHANNEL_9_BITFIELD),    /*!< ADC channel ADCx_IN8  */
  (ADC_CHANNEL_10_SMP  | ADC_CHANNEL_10_BITFIELD),   /*!< ADC channel ADCx_IN10 */
  (ADC_CHANNEL_11_SMP  | ADC_CHANNEL_11_BITFIELD),   /*!< ADC channel ADCx_IN11 */
  (ADC_CHANNEL_12_SMP  | ADC_CHANNEL_12_BITFIELD),   /*!< ADC channel ADCx_IN12 */
  (ADC_CHANNEL_13_SMP  | ADC_CHANNEL_13_BITFIELD),   /*!< ADC channel ADCx_IN13 */
  (ADC_CHANNEL_14_SMP  | ADC_CHANNEL_14_BITFIELD),   /*!< ADC channel ADCx_IN14 */
  (ADC_CHANNEL_15_SMP  | ADC_CHANNEL_15_BITFIELD),   /*!< ADC channel ADCx_IN15 */
  (ADC_CHANNEL_16_SMP  | ADC_CHANNEL_16_BITFIELD),   /*!< ADC channel ADCx_IN16 */
  (ADC_CHANNEL_17_SMP  | ADC_CHANNEL_17_BITFIELD),   /*!< ADC channel ADCx_IN17 */
  (ADC_CHANNEL_18_SMP  | ADC_CHANNEL_18_BITFIELD),   /*!< ADC channel ADCx_IN18 */
  (ADC_CHANNEL_19_SMP  | ADC_CHANNEL_19_BITFIELD),   /*!< ADC channel ADCx_IN19 */
};


/* Internal mask for ADC mode single or differential ended:                   */
/* To select into literals LL_ADC_SINGLE_ENDED or LL_ADC_SINGLE_DIFFERENTIAL  */
/* the relevant bits for:                                                     */
/* (concatenation of multiple bits used in different registers)               */
/* - ADC calibration: calibration start, calibration factor get or set        */
/* - ADC channels: set each ADC channel ending mode                           */
#define ADC_SINGLEDIFF_CALIB_START_MASK    (ADC_CR_ADCALDIF)
#define ADC_SINGLEDIFF_CALIB_FACTOR_MASK   (ADC_CALFACT_CALFACT_D | ADC_CALFACT_CALFACT_S)
#define ADC_SINGLEDIFF_CHANNEL_MASK        (ADC_CHANNEL_ID_BITFIELD_MASK) /* Equivalent to ADC_DIFSEL_DIFSEL */
#define ADC_SINGLEDIFF_CHANNEL_SHIFT_MASK  (ADC_CALFACT_CALFACT_S_4 | ADC_CALFACT_CALFACT_S_3) /* Bits chosen
                                            to perform of shift when single mode is selected, shift value out of
                                            channels bits range. */
#define ADC_SINGLEDIFF_CALIB_F_BIT_D_MASK   (0x00010000UL) /* Selection of 1 bit to discriminate differential mode:
                                            mask of bit */
#define ADC_SINGLEDIFF_CALIB_F_BIT_D_POS    (16UL)         /* Selection of 1 bit to discriminate differential mode:
                                            position of bit */
#define ADC_SINGLEDIFF_CALIB_F_BIT_D_SHIFT4 (ADC_SINGLEDIFF_CALIB_F_BIT_D_POS - 4UL) /* Shift of bit
                                            ADC_SINGLEDIFF_CALIB_F_BIT_D to perform a shift of 4 ranks */

/* Internal mask for ADC analog watchdog:                                     */
/* To select into literals LL_ADC_AWD_CHANNELx_xxx the relevant bits for:     */
/* (concatenation of multiple bits used in different analog watchdogs,        */
/* (feature of several watchdogs not available on all STM32 series)).       */
/* - analog watchdog 1: monitored channel defined by number,                  */
/*   selection of ADC group (ADC groups regular and-or injected).             */
/* - analog watchdog 2 and 3: monitored channel defined by bitfield, no       */
/*   selection on groups.                                                     */

/* Definition of channels ID number information to be inserted into           */
/* analog watchdog channels literals definition.                              */
#define ADC_CHANNEL_0_NUMBER               (0x00000000UL)
#define ADC_CHANNEL_1_NUMBER               (ADC_CFGR1_AWD1CH_0)
#define ADC_CHANNEL_2_NUMBER               (ADC_CFGR1_AWD1CH_1)
#define ADC_CHANNEL_3_NUMBER               (ADC_CFGR1_AWD1CH_1 | ADC_CFGR1_AWD1CH_0)
#define ADC_CHANNEL_4_NUMBER               (ADC_CFGR1_AWD1CH_2)
#define ADC_CHANNEL_5_NUMBER               (ADC_CFGR1_AWD1CH_2 | ADC_CFGR1_AWD1CH_0)
#define ADC_CHANNEL_6_NUMBER               (ADC_CFGR1_AWD1CH_2 | ADC_CFGR1_AWD1CH_1)
#define ADC_CHANNEL_7_NUMBER               (ADC_CFGR1_AWD1CH_2 | ADC_CFGR1_AWD1CH_1 | ADC_CFGR1_AWD1CH_0)
#define ADC_CHANNEL_8_NUMBER               (ADC_CFGR1_AWD1CH_3)
#define ADC_CHANNEL_9_NUMBER               (ADC_CFGR1_AWD1CH_3 | ADC_CFGR1_AWD1CH_0)
#define ADC_CHANNEL_10_NUMBER              (ADC_CFGR1_AWD1CH_3 | ADC_CFGR1_AWD1CH_1)
#define ADC_CHANNEL_11_NUMBER              (ADC_CFGR1_AWD1CH_3 | ADC_CFGR1_AWD1CH_1 | ADC_CFGR1_AWD1CH_0)
#define ADC_CHANNEL_12_NUMBER              (ADC_CFGR1_AWD1CH_3 | ADC_CFGR1_AWD1CH_2)
#define ADC_CHANNEL_13_NUMBER              (ADC_CFGR1_AWD1CH_3 | ADC_CFGR1_AWD1CH_2 | ADC_CFGR1_AWD1CH_0)
#define ADC_CHANNEL_14_NUMBER              (ADC_CFGR1_AWD1CH_3 | ADC_CFGR1_AWD1CH_2 | ADC_CFGR1_AWD1CH_1)
#define ADC_CHANNEL_15_NUMBER              (ADC_CFGR1_AWD1CH_3 | ADC_CFGR1_AWD1CH_2 | \
                                            ADC_CFGR1_AWD1CH_1 | ADC_CFGR1_AWD1CH_0)
#define ADC_CHANNEL_16_NUMBER              (ADC_CFGR1_AWD1CH_4)
#define ADC_CHANNEL_17_NUMBER              (ADC_CFGR1_AWD1CH_4 | ADC_CFGR1_AWD1CH_0)
#define ADC_CHANNEL_18_NUMBER              (ADC_CFGR1_AWD1CH_4 | ADC_CFGR1_AWD1CH_1)
#define ADC_CHANNEL_19_NUMBER              (ADC_CFGR1_AWD1CH_4 | ADC_CFGR1_AWD1CH_1 | ADC_CFGR1_AWD1CH_0)

#define ADC_AWD_CHANNEL_NUMBER_MASK           (ADC_CFGR1_AWD1CH)
#define ADC_AWD_CHANNEL_NUMBER_BITOFFSET_POS  (ADC_CFGR1_AWD1CH_Pos)

/* Internal register offset for ADC analog watchdog channel configuration */
#define ADC_AWD_CR1_REGOFFSET              (0x00000000UL)
#define ADC_AWD_CR2_REGOFFSET              (0x00100000UL)
#define ADC_AWD_CR3_REGOFFSET              (0x00200000UL)

/* Register offset gap between AWD1 and AWD2-AWD3 configuration registers */
/* (Set separately as ADC_AWD_CRX_REGOFFSET to spare 32 bits space */
#define ADC_AWD_CR12_REGOFFSETGAP_MASK     (ADC_AWD2CR_AWD2CH_0)
#define ADC_AWD_CR12_REGOFFSETGAP_VAL      (0x00000024UL)

#define ADC_AWD_CRX_REGOFFSET_MASK         (ADC_AWD_CR1_REGOFFSET | ADC_AWD_CR2_REGOFFSET | ADC_AWD_CR3_REGOFFSET)

#define ADC_AWD_CR1_CHANNEL_MASK           (ADC_CFGR1_AWD1CH | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)
#define ADC_AWD_CR23_CHANNEL_MASK          (ADC_AWD2CR_AWD2CH)
#define ADC_AWD_CR_ALL_CHANNEL_MASK        (ADC_AWD_CR1_CHANNEL_MASK | ADC_AWD_CR23_CHANNEL_MASK)

#define ADC_AWD_CRX_REGOFFSET_POS          (20UL)                      /* Position of bits ADC_AWD_CRx_REGOFFSET
                                                                          in ADC_AWD_CRX_REGOFFSET_MASK */

/* Internal register offset for ADC analog watchdog threshold configuration */
#define ADC_AWD_TR1_REGOFFSET              (ADC_AWD_CR1_REGOFFSET)
#define ADC_AWD_TR2_REGOFFSET              (ADC_AWD_CR2_REGOFFSET)
#define ADC_AWD_TR3_REGOFFSET              (ADC_AWD_CR3_REGOFFSET)
#define ADC_AWD_TRX_REGOFFSET_MASK         (ADC_AWD_TR1_REGOFFSET | ADC_AWD_TR2_REGOFFSET | ADC_AWD_TR3_REGOFFSET)
#define ADC_AWD_TRX_REGOFFSET_POS          (ADC_AWD_CRX_REGOFFSET_POS)      /* Position of bits ADC_SQRx_REGOFFSET
                                                                               in ADC_AWD_TRX_REGOFFSET_MASK */
#define ADC_AWD_TRX_BIT_HIGH_MASK          (0x00010000UL)                   /* Selection of 1 bit to discriminate
                                                                               threshold high: mask of bit */
#define ADC_AWD_TRX_BIT_HIGH_POS           (16UL)                           /* Selection of 1 bit to discriminate
                                                                               threshold high: position of bit */
#define ADC_AWD_TRX_BIT_HIGH_SHIFT4        (ADC_AWD_TRX_BIT_HIGH_POS - 4UL) /* Shift of bit ADC_AWD_TRX_BIT_HIGH to
                                                                               position to perform a shift of 4 ranks */

/* Internal mask for ADC offset:                                              */
/* Internal register offset for ADC offset instance configuration */
#define ADC_OFR1_REGOFFSET                 (0x00000000UL)
#define ADC_OFR2_REGOFFSET                 (0x00000001UL)
#define ADC_OFR3_REGOFFSET                 (0x00000002UL)
#define ADC_OFR4_REGOFFSET                 (0x00000003UL)
#define ADC_OFRx_REGOFFSET_MASK            (ADC_OFR1_REGOFFSET | ADC_OFR2_REGOFFSET \
                                            | ADC_OFR3_REGOFFSET | ADC_OFR4_REGOFFSET)

/* ADC registers bits groups */
#define ADC_CR_BITS_PROPERTY_RS            (ADC_CR_ADEN | ADC_CR_ADDIS        \
                                            | ADC_CR_JADSTART | ADC_CR_JADSTP \
                                            | ADC_CR_ADSTART | ADC_CR_ADSTP)            /* ADC register CR bits with
                                           HW property "rs": Software can read as well as set this bit.
                                           Writing '0' has no effect on the bit value. */

/* Internal mask for ADC channel internal path*/
#define ADC_COMMON_PATH_INTERNAL_MASK      (0x7UL << ADC_CCR_VREFEN_Pos)        /*!< ADC measurement path to internal
                                            channel mask in LL_ADC_CHANNEL bitfield */
#define ADC_PATH_INTERNAL_POS              (16UL)                               /*!< ADC measurement path to internal
                                            channel position in LL_ADC_CHANNEL bitfield */
#define ADC_PATH_INTERNAL_MASK             (0x3UL << ADC_PATH_INTERNAL_POS)  /*!< ADC measurement path to internal
                                            channel mask in LL_ADC_CHANNEL bitfield */

/* ADC internal channels related definitions */
/* Internal voltage reference VrefInt */
#define VREFINT_CAL_ADDR                   ((uint16_t*) (0x460091B8UL)) /* Internal voltage reference, address of
                                           parameter VREFINT_CAL: VrefInt ADC raw data acquired at temperature 30 DegC
                                           (tolerance: +-5 DegC), Vref+ = 3.3 V (tolerance: +-10 mV).
                                           On this STM32 series, it is required to load the OTP110 word before reading
                                           this address.
                                           In case of usage with HAL driver, refer to HAL_BSEC_OTP_Reload() */
#define VREFINT_CAL_VREF                   (1800UL)                     /* Analog voltage reference (Vref+) value
                                           with which VrefInt has been calibrated in production
                                           (tolerance: +-10 mV) (unit: mV). */

/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/
/** @defgroup ADC_LL_Private_Macros ADC Private Macros
  * @{
  */
/* Macro reserved for internal LL driver usage, not intended to be used in    */
/* code of final user.                                                        */

/**
  * @brief  Driver macro reserved for internal use: set a pointer to
  *         a register from a register basis from which an offset
  *         is applied.
  * @param  __REG__ Register basis from which the offset is applied.
  * @param  __REG_OFFFSET__ Offset to be applied (unit: number of registers).
  * @retval Pointer to register address
  */
#define __ADC_PTR_REG_OFFSET(__REG__, __REG_OFFFSET__)                         \
  ((__IO uint32_t *)((uint32_t) ((uint32_t)(&(__REG__)) + ((__REG_OFFFSET__) << 2UL))))

/**
  * @brief Getter to access the ADC instance corresponding index in
  *        differential channel look up table
  * @param __INSTANCE__ ADC instance
  * @retval Look-up table index related to __INSTANCE__
  */
#define __ADC_INSTANCE_INDEX(__INSTANCE__)          \
  ( ((__INSTANCE__) == ADC1)                        \
    ?                                               \
    (0UL)                                           \
    :                                               \
    (1UL)                                           \
  )

#define __ADC_CHANNEL_INDEX(__CHANNEL__)                                                \
  ((__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_0 )) ?  0UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_1 )) ?  1UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_2 )) ?  2UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_3 )) ?  3UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_4 )) ?  4UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_5 )) ?  5UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_6 )) ?  6UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_7 )) ?  7UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_8 )) ?  8UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_9 )) ?  9UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_10)) ? 10UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_11)) ? 11UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_12)) ? 12UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_13)) ? 13UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_14)) ? 14UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_15)) ? 15UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_16)) ? 16UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_17)) ? 17UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_18)) ? 18UL : \
   (__LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) == (LL_ADC_CHANNEL_19)) ? 19UL : 0UL)

/**
  * @}
  */


/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)
/** @defgroup ADC_LL_ES_INIT ADC Exported Init structure
  * @{
  */

/**
  * @brief  Structure definition of some features of ADC common parameters
  *         and multimode
  *         (all ADC instances belonging to the same ADC common instance).
  * @note   The setting of these parameters by function @ref LL_ADC_CommonInit()
  *         is conditioned to ADC instances state (all ADC instances
  *         sharing the same ADC common instance):
  *         All ADC instances sharing the same ADC common instance must be
  *         disabled.
  */
typedef struct
{
#if defined(ADC_MULTIMODE_SUPPORT)
  uint32_t Multimode;                   /*!< Set ADC multimode configuration to operate in independent mode or multimode
                                             (for devices with several ADC instances).
                                             This parameter can be a value of @ref ADC_LL_EC_MULTI_MODE
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_SetMultimode(). */

  uint32_t MultiDataFormat;             /*!< Set ADC multimode conversion data format of ADC group regular: conversion
                                             data in data register of each ADC instance or ADC common instance.
                                             This parameter can be a value of @ref ADC_LL_EC_MULTI_DATA_FORMAT
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_SetMultiDataFormat(). */

  uint32_t MultiTwoSamplingDelay;       /*!< Set ADC multimode delay between 2 sampling phases.
                                             Relevant only for multimode modes: interleaved based modes.
                                             This parameter can be a value of @ref ADC_LL_EC_MULTI_TWOSMP_DELAY
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_SetMultiTwoSamplingDelay(). */
#endif /* ADC_MULTIMODE_SUPPORT */

} LL_ADC_CommonInitTypeDef;

/**
  * @brief  Structure definition of some features of ADC instance.
  * @note   These parameters have an impact on ADC scope: ADC instance.
  *         Affects both group regular and group injected (availability
  *         of ADC group injected depends on STM32 series).
  *         Refer to corresponding unitary functions into
  *         @ref ADC_LL_EF_Configuration_ADC_Instance .
  * @note   The setting of these parameters by function @ref LL_ADC_Init()
  *         is conditioned to ADC state:
  *         ADC instance must be disabled.
  *         This condition is applied to all ADC features, for efficiency
  *         and compatibility over all STM32 series. However, the different
  *         features can be set under different ADC state conditions
  *         (setting possible with ADC enabled without conversion on going,
  *         ADC enabled with conversion on going, ...)
  *         Each feature can be updated afterwards with a unitary function
  *         and potentially with ADC in a different state than disabled,
  *         refer to description of each function for setting
  *         conditioned to ADC state.
  */
typedef struct
{
  uint32_t Resolution;                  /*!< Set ADC resolution.
                                             This parameter can be a value of @ref ADC_LL_EC_RESOLUTION
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_SetResolution(). */

  uint32_t LeftBitShift;                /*!< Configures the left shifting applied to the final result with or without
                                             oversampling.
                                             This parameter can be a value of @ref ADC_LL_EC_LEFT_BIT_SHIFT. */

  uint32_t LowPowerMode;                /*!< Set ADC low power mode.
                                             This parameter can be a value of @ref ADC_LL_EC_LP_MODE
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_SetLPModeAutoWait(). */

} LL_ADC_InitTypeDef;

/**
  * @brief  Structure definition of some features of ADC group regular.
  * @note   These parameters have an impact on ADC scope: ADC group regular.
  *         Refer to corresponding unitary functions into
  *         @ref ADC_LL_EF_Configuration_ADC_Group_Regular
  *         (functions with prefix "REG").
  * @note   The setting of these parameters by function @ref LL_ADC_REG_Init()
  *         is conditioned to ADC state:
  *         ADC instance must be disabled.
  *         This condition is applied to all ADC features, for efficiency
  *         and compatibility over all STM32 series. However, the different
  *         features can be set under different ADC state conditions
  *         (setting possible with ADC enabled without conversion on going,
  *         ADC enabled with conversion on going, ...)
  *         Each feature can be updated afterwards with a unitary function
  *         and potentially with ADC in a different state than disabled,
  *         refer to description of each function for setting
  *         conditioned to ADC state.
  */
typedef struct
{
  uint32_t TriggerSource;               /*!< Set ADC group regular conversion trigger source: internal (SW start) or
                                             from external peripheral (timer event, external interrupt line).
                                             This parameter can be a value of @ref ADC_LL_EC_REG_TRIGGER_SOURCE
                                             @note On this STM32 series, setting trigger source to external trigger also
                                                   set trigger polarity to rising edge(default setting for compatibility
                                                   with some ADC on other STM32 series having this setting set by HW
                                                   default value).
                                                   In case of need to modify trigger edge, use function
                                                   @ref LL_ADC_REG_SetTriggerEdge().
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_REG_SetTriggerSource(). */

  uint32_t SequencerLength;             /*!< Set ADC group regular sequencer length.
                                             This parameter can be a value of @ref ADC_LL_EC_REG_SEQ_SCAN_LENGTH
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_REG_SetSequencerLength(). */

  uint32_t SequencerDiscont;            /*!< Set ADC group regular sequencer discontinuous mode: sequence subdivided
                                             and scan conversions interrupted every selected number of ranks.
                                             This parameter can be a value of @ref ADC_LL_EC_REG_SEQ_DISCONT_MODE
                                             @note This parameter has an effect only if group regular sequencer is
                                                   enabled (scan length of 2 ranks or more).
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_REG_SetSequencerDiscont(). */

  uint32_t ContinuousMode;              /*!< Set ADC continuous conversion mode on ADC group regular, whether ADC
                                             conversions are performed in single mode (one conversion per trigger) or in
                                             continuous mode (after the first trigger, following conversions launched
                                             successively automatically).
                                             This parameter can be a value of @ref ADC_LL_EC_REG_CONTINUOUS_MODE
                                             Note: It is not possible to enable both ADC group regular continuous mode
                                                   and discontinuous mode.
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_REG_SetContinuousMode(). */

  uint32_t DataTransferMode;            /*!< Set ADC group regular conversion data transfer: no transfer, transfer
                                             by DMA or other peripherals.
                                             This parameter can be a value of @ref ADC_LL_EC_REG_DATA_TRANSFER
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_REG_SetDataTransferMode(). */

  uint32_t Overrun;                     /*!< Set ADC group regular behavior in case of overrun:
                                             data preserved or overwritten.
                                             This parameter can be a value of @ref ADC_LL_EC_REG_OVR_DATA_BEHAVIOR
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_REG_SetOverrun(). */

} LL_ADC_REG_InitTypeDef;

/**
  * @brief  Structure definition of some features of ADC group injected.
  * @note   These parameters have an impact on ADC scope: ADC group injected.
  *         Refer to corresponding unitary functions into
  *         @ref ADC_LL_EF_Configuration_ADC_Group_Regular
  *         (functions with prefix "INJ").
  * @note   The setting of these parameters by function @ref LL_ADC_INJ_Init()
  *         is conditioned to ADC state:
  *         ADC instance must be disabled.
  *         This condition is applied to all ADC features, for efficiency
  *         and compatibility over all STM32 series. However, the different
  *         features can be set under different ADC state conditions
  *         (setting possible with ADC enabled without conversion on going,
  *         ADC enabled with conversion on going, ...)
  *         Each feature can be updated afterwards with a unitary function
  *         and potentially with ADC in a different state than disabled,
  *         refer to description of each function for setting
  *         conditioned to ADC state.
  */
typedef struct
{
  uint32_t TriggerSource;               /*!< Set ADC group injected conversion trigger source: internal (SW start)
                                             or from external peripheral (timer event, external interrupt line).
                                             This parameter can be a value of @ref ADC_LL_EC_INJ_TRIGGER_SOURCE
                                             @note On this STM32 series, setting trigger source to external trigger also
                                                   set trigger polarity to rising edge (default setting for
                                                   compatibility with some ADC on other STM32 series having this
                                                   setting set by HW default value).
                                                   In case of need to modify trigger edge, use function
                                                   @ref LL_ADC_INJ_SetTriggerEdge().
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_INJ_SetTriggerSource(). */

  uint32_t SequencerLength;             /*!< Set ADC group injected sequencer length.
                                             This parameter can be a value of @ref ADC_LL_EC_INJ_SEQ_SCAN_LENGTH
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_INJ_SetSequencerLength(). */

  uint32_t SequencerDiscont;            /*!< Set ADC group injected sequencer discontinuous mode: sequence subdivided
                                             and scan conversions interrupted every selected number of ranks.
                                             This parameter can be a value of @ref ADC_LL_EC_INJ_SEQ_DISCONT_MODE
                                             @note This parameter has an effect only if group injected sequencer is
                                                   enabled (scan length of 2 ranks or more).
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_INJ_SetSequencerDiscont(). */

  uint32_t TrigAuto;                    /*!< Set ADC group injected conversion trigger: independent or from ADC group
                                             regular.
                                             This parameter can be a value of @ref ADC_LL_EC_INJ_TRIG_AUTO
                                             Note: This parameter must be set to set to independent trigger if injected
                                                   trigger source is set to an external trigger.
                                             This feature can be modified afterwards using unitary function
                                             @ref LL_ADC_INJ_SetTrigAuto(). */

} LL_ADC_INJ_InitTypeDef;

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/
/** @defgroup ADC_LL_Exported_Constants ADC Exported Constants
  * @{
  */

/** @defgroup ADC_LL_EC_FLAG ADC flags
  * @brief    Flags defines which can be used with LL_ADC_ReadReg function
  * @{
  */
#define LL_ADC_FLAG_ADRDY                  ADC_ISR_ADRDY      /*!< ADC flag ADC instance ready */
#define LL_ADC_FLAG_EOC                    ADC_ISR_EOC        /*!< ADC flag ADC group regular end of unitary
                                           conversion */
#define LL_ADC_FLAG_EOS                    ADC_ISR_EOS        /*!< ADC flag ADC group regular end of sequence
                                           conversions */
#define LL_ADC_FLAG_OVR                    ADC_ISR_OVR        /*!< ADC flag ADC group regular overrun */
#define LL_ADC_FLAG_EOSMP                  ADC_ISR_EOSMP      /*!< ADC flag ADC group regular end of sampling phase */
#define LL_ADC_FLAG_JEOC                   ADC_ISR_JEOC       /*!< ADC flag ADC group injected end of unitary
                                           conversion */
#define LL_ADC_FLAG_JEOS                   ADC_ISR_JEOS       /*!< ADC flag ADC group injected end of sequence
                                           conversions */
#define LL_ADC_FLAG_AWD1                   ADC_ISR_AWD1       /*!< ADC flag ADC analog watchdog 1 */
#define LL_ADC_FLAG_AWD2                   ADC_ISR_AWD2       /*!< ADC flag ADC analog watchdog 2 */
#define LL_ADC_FLAG_AWD3                   ADC_ISR_AWD3       /*!< ADC flag ADC analog watchdog 3 */
#if defined(ADC_MULTIMODE_SUPPORT)
#define LL_ADC_FLAG_ADRDY_MST              ADC_CSR_ADRDY_MST  /*!< ADC flag ADC multimode master instance ready */
#define LL_ADC_FLAG_ADRDY_SLV              ADC_CSR_ADRDY_SLV  /*!< ADC flag ADC multimode slave instance ready */
#define LL_ADC_FLAG_EOC_MST                ADC_CSR_EOC_MST    /*!< ADC flag ADC multimode master group regular end of
                                           unitary conversion */
#define LL_ADC_FLAG_EOC_SLV                ADC_CSR_EOC_SLV    /*!< ADC flag ADC multimode slave group regular end of
                                           unitary conversion */
#define LL_ADC_FLAG_EOS_MST                ADC_CSR_EOS_MST    /*!< ADC flag ADC multimode master group regular end of
                                           sequence conversions */
#define LL_ADC_FLAG_EOS_SLV                ADC_CSR_EOS_SLV    /*!< ADC flag ADC multimode slave group regular end of
                                           sequence conversions */
#define LL_ADC_FLAG_OVR_MST                ADC_CSR_OVR_MST    /*!< ADC flag ADC multimode master group regular
                                           overrun */
#define LL_ADC_FLAG_OVR_SLV                ADC_CSR_OVR_SLV    /*!< ADC flag ADC multimode slave group regular
                                           overrun */
#define LL_ADC_FLAG_EOSMP_MST              ADC_CSR_EOSMP_MST  /*!< ADC flag ADC multimode master group regular end of
                                           sampling phase */
#define LL_ADC_FLAG_EOSMP_SLV              ADC_CSR_EOSMP_SLV  /*!< ADC flag ADC multimode slave group regular end of
                                           sampling phase */
#define LL_ADC_FLAG_JEOC_MST               ADC_CSR_JEOC_MST   /*!< ADC flag ADC multimode master group injected end of
                                           unitary conversion */
#define LL_ADC_FLAG_JEOC_SLV               ADC_CSR_JEOC_SLV   /*!< ADC flag ADC multimode slave group injected end of
                                           unitary conversion */
#define LL_ADC_FLAG_JEOS_MST               ADC_CSR_JEOS_MST   /*!< ADC flag ADC multimode master group injected end of
                                           sequence conversions */
#define LL_ADC_FLAG_JEOS_SLV               ADC_CSR_JEOS_SLV   /*!< ADC flag ADC multimode slave group injected end of
                                           sequence conversions */
#define LL_ADC_FLAG_AWD1_MST               ADC_CSR_AWD1_MST   /*!< ADC flag ADC multimode master analog watchdog 1
                                           of the ADC master */
#define LL_ADC_FLAG_AWD1_SLV               ADC_CSR_AWD1_SLV   /*!< ADC flag ADC multimode slave analog watchdog 1
                                           of the ADC slave */
#define LL_ADC_FLAG_AWD2_MST               ADC_CSR_AWD2_MST   /*!< ADC flag ADC multimode master analog watchdog 2
                                           of the ADC master */
#define LL_ADC_FLAG_AWD2_SLV               ADC_CSR_AWD2_SLV   /*!< ADC flag ADC multimode slave analog watchdog 2
                                           of the ADC slave */
#define LL_ADC_FLAG_AWD3_MST               ADC_CSR_AWD3_MST   /*!< ADC flag ADC multimode master analog watchdog 3
                                           of the ADC master */
#define LL_ADC_FLAG_AWD3_SLV               ADC_CSR_AWD3_SLV   /*!< ADC flag ADC multimode slave analog watchdog 3
                                           of the ADC slave */
#endif /* ADC_MULTIMODE_SUPPORT */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_IT ADC interruptions for configuration (interruption enable or disable)
  * @brief    IT defines which can be used with LL_ADC_ReadReg and  LL_ADC_WriteReg functions
  * @{
  */
#define LL_ADC_IT_ADRDY                    ADC_IER_ADRDYIE    /*!< ADC interruption ADC instance ready */
#define LL_ADC_IT_EOC                      ADC_IER_EOCIE      /*!< ADC interruption ADC group regular end of unitary
                                           conversion */
#define LL_ADC_IT_EOS                      ADC_IER_EOSIE      /*!< ADC interruption ADC group regular end of sequence
                                           conversions */
#define LL_ADC_IT_OVR                      ADC_IER_OVRIE      /*!< ADC interruption ADC group regular overrun */
#define LL_ADC_IT_EOSMP                    ADC_IER_EOSMPIE    /*!< ADC interruption ADC group regular end of sampling
                                           phase */
#define LL_ADC_IT_JEOC                     ADC_IER_JEOCIE     /*!< ADC interruption ADC group injected end of unitary
                                           conversion */
#define LL_ADC_IT_JEOS                     ADC_IER_JEOSIE     /*!< ADC interruption ADC group injected end of sequence
                                           conversions */
#define LL_ADC_IT_AWD1                     ADC_IER_AWD1IE     /*!< ADC interruption ADC analog watchdog 1 */
#define LL_ADC_IT_AWD2                     ADC_IER_AWD2IE     /*!< ADC interruption ADC analog watchdog 2 */
#define LL_ADC_IT_AWD3                     ADC_IER_AWD3IE     /*!< ADC interruption ADC analog watchdog 3 */

/**
  * @}
  */

/** @defgroup ADC_LL_EC_REGISTERS  ADC registers compliant with specific purpose
  * @{
  */
/* List of ADC registers intended to be used (most commonly) with             */
/* DMA transfer.                                                              */
/* Refer to function @ref LL_ADC_DMA_GetRegAddr().                            */
#define LL_ADC_DMA_REG_REGULAR_DATA        (0x00000000UL) /*!< ADC group regular conversion data register
                                           (corresponding to register DR) to be used with ADC configured in independent
                                           mode. Without DMA transfer, register accessed by LL function
                                           @ref LL_ADC_REG_ReadConversionData32() */
#if defined(ADC_MULTIMODE_SUPPORT)
#define LL_ADC_DMA_REG_REGULAR_MULTI_NO_PACKING (0x00000001UL) /*!< ADC group regular conversion data register
                                           to be used with ADC configured in multimode (available on STM32 devices
                                           with several ADC instances), without data packing.
                                           Register used is CDR2, compliant with all ADC multimode data format
                                           and data width, refer to description of literals in
                                           @ref ADC_LL_EC_MULTI_DATA_FORMAT.
                                           Without DMA transfer, register accessed by LL function
                                           @ref LL_ADC_REG_ReadMultiConvNoPacking() */
#define LL_ADC_DMA_REG_REGULAR_MULTI_PACKING    (0x00000002UL) /*!< ADC group regular conversion data register
                                           to be used with ADC configured in multimode (available on STM32 devices
                                           with several ADC instances), with data packing.
                                           Register used is CDR, usable under conditions of ADC multimode data format
                                           selected and data width, refer to description of literals in
                                           @ref ADC_LL_EC_MULTI_DATA_FORMAT.
                                           Without DMA transfer, register accessed by LL function
                                           @ref LL_ADC_REG_ReadMultiConvPacking() */
#endif /* ADC_MULTIMODE_SUPPORT */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_COMMON_PATH_INTERNAL  ADC common - Measurement path to internal channels
  * @{
  */
/* Note: Other measurement paths to internal channels may be available        */
/*       (connections to other peripherals).                                  */
/*       If they are not listed below, they do not require any specific       */
/*       path enable. In this case, Access to measurement path is done        */
/*       only by selecting the corresponding ADC internal channel.            */
#define LL_ADC_PATH_INTERNAL_NONE          (0x00000000UL)       /*!< ADC measurement paths all disabled */
#define LL_ADC_PATH_INTERNAL_VREFINT       (ADC_CCR_VREFEN)     /*!< ADC measurement path to internal channel VrefInt */
#define LL_ADC_PATH_INTERNAL_VBAT          (ADC_CCR_VBATEN)     /*!< ADC measurement path to internal channel Vbat    */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_PATH_INTERNAL  ADC instance - Measurement path to internal channels
  * @{
  */
/* Note: Other measurement paths to internal channels may be available        */
/*       (connections to other peripherals).                                  */
/*       If they are not listed below, they do not require any specific       */
/*       path enable. In this case, Access to measurement path is done        */
/*       only by selecting the corresponding ADC internal channel.            */
#define LL_ADC_PATH_INTERNAL_VDDCORE       (ADC_OR_OP2 << ADC_PATH_INTERNAL_POS)            /*!< ADC measurement path
                                            to internal channel VddCore */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_RESOLUTION  ADC instance - Resolution
  * @{
  */
#define LL_ADC_RESOLUTION_12B              (0x00000000UL)                      /*!< ADC resolution 12 bits */
#define LL_ADC_RESOLUTION_10B              (                 ADC_CFGR1_RES_0)  /*!< ADC resolution 10 bits */
#define LL_ADC_RESOLUTION_8B               (ADC_CFGR1_RES_1                 )  /*!< ADC resolution  8 bits */
#define LL_ADC_RESOLUTION_6B               (ADC_CFGR1_RES_1 | ADC_CFGR1_RES_0) /*!< ADC resolution  6 bits */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_LEFT_BIT_SHIFT   ADC left Shift
  * @{
  */
#define LL_ADC_LEFT_BIT_SHIFT_NONE  (0x00000000UL)                                                                       /*!< ADC no bit shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_1     (ADC_CFGR2_LSHIFT_0)                                                                 /*!< ADC 1 bit shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_2     (ADC_CFGR2_LSHIFT_1)                                                                 /*!< ADC 2 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_3     (ADC_CFGR2_LSHIFT_1 | ADC_CFGR2_LSHIFT_0)                                            /*!< ADC 3 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_4     (ADC_CFGR2_LSHIFT_2)                                                                 /*!< ADC 4 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_5     (ADC_CFGR2_LSHIFT_2 | ADC_CFGR2_LSHIFT_0)                                            /*!< ADC 5 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_6     (ADC_CFGR2_LSHIFT_2 | ADC_CFGR2_LSHIFT_1)                                            /*!< ADC 6 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_7     (ADC_CFGR2_LSHIFT_2 | ADC_CFGR2_LSHIFT_1 | ADC_CFGR2_LSHIFT_0)                       /*!< ADC 7 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_8     (ADC_CFGR2_LSHIFT_3)                                                                 /*!< ADC 8 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_9     (ADC_CFGR2_LSHIFT_3 | ADC_CFGR2_LSHIFT_0)                                            /*!< ADC 9 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_10    (ADC_CFGR2_LSHIFT_3 | ADC_CFGR2_LSHIFT_1)                                            /*!< ADC 10 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_11    (ADC_CFGR2_LSHIFT_3 | ADC_CFGR2_LSHIFT_1 | ADC_CFGR2_LSHIFT_0)                       /*!< ADC 11 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_12    (ADC_CFGR2_LSHIFT_3 | ADC_CFGR2_LSHIFT_2)                                            /*!< ADC 12 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_13    (ADC_CFGR2_LSHIFT_3 | ADC_CFGR2_LSHIFT_2 | ADC_CFGR2_LSHIFT_0)                       /*!< ADC 13 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_14    (ADC_CFGR2_LSHIFT_3 | ADC_CFGR2_LSHIFT_2 | ADC_CFGR2_LSHIFT_1)                       /*!< ADC 14 bits shift left applied on the final ADC conversion data */
#define LL_ADC_LEFT_BIT_SHIFT_15    (ADC_CFGR2_LSHIFT_3 | ADC_CFGR2_LSHIFT_2 | ADC_CFGR2_LSHIFT_1 | ADC_CFGR2_LSHIFT_0)  /*!< ADC 15 bits shift left applied on the final ADC conversion data */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_LP_MODE  ADC instance - Low power mode
  * @{
  */
#define LL_ADC_LP_AUTOWAIT_DISABLE         (0x00000000UL)     /*!< ADC low power mode auto delay disabled. */
#define LL_ADC_LP_AUTOWAIT_ENABLE          (ADC_CFGR1_AUTDLY) /*!< ADC low power mode auto delay enabled: dynamic
                                           low power mode, ADC conversions are performed only when necessary
                                           (when previous ADC conversion data is read).
                                           See description with function @ref LL_ADC_SetLPModeAutoWait(). */
/**
  * @}
  */
/* Definitions for backward compatibility with legacy STM32 series */
#define LL_ADC_LP_MODE_NONE LL_ADC_LP_AUTOWAIT_DISABLE
#define LL_ADC_LP_AUTOWAIT  LL_ADC_LP_AUTOWAIT_ENABLE

/** @defgroup ADC_LL_EC_OFFSET_NB  ADC instance - Offset instance
  * @{
  */
#define LL_ADC_OFFSET_1                    ADC_OFR1_REGOFFSET /*!< ADC offset instance 1: ADC channel and offset level
to which the offset programmed will be applied (independently of channel mapped on ADC group regular or injected) */
#define LL_ADC_OFFSET_2                    ADC_OFR2_REGOFFSET /*!< ADC offset instance 2: ADC channel and offset level
to which the offset programmed will be applied (independently of channel mapped on ADC group regular or injected) */
#define LL_ADC_OFFSET_3                    ADC_OFR3_REGOFFSET /*!< ADC offset instance 3: ADC channel and offset level
to which the offset programmed will be applied (independently of channel mapped on ADC group regular or injected) */
#define LL_ADC_OFFSET_4                    ADC_OFR4_REGOFFSET /*!< ADC offset instance 4: ADC channel and offset level
to which the offset programmed will be applied (independently of channel mapped on ADC group regular or injected) */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_OFFSET_SIGNED_SATURATION ADC instance - Offset signed saturation mode
  * @{
  */
#define LL_ADC_OFFSET_SIGNED_SAT_DISABLE   (0x00000000UL)     /*!< ADC offset signed saturation is disabled */
#define LL_ADC_OFFSET_SIGNED_SAT_ENABLE    (ADC_OFCFGR1_SSAT) /*!< ADC offset signed saturation is enabled */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_OFFSET_UNSIGNED_SATURATION ADC instance - Offset unsigned saturation mode
  * @{
  */
#define LL_ADC_OFFSET_UNSIGNED_SAT_DISABLE   (0x00000000UL)     /*!< ADC offset unsigned saturation is disabled */
#define LL_ADC_OFFSET_UNSIGNED_SAT_ENABLE    (ADC_OFCFGR1_USAT) /*!< ADC offset unsigned saturation is enabled */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_OFFSET_SIGN ADC instance - Offset sign
  * @{
  */
#define LL_ADC_OFFSET_SIGN_NEGATIVE        (0x00000000UL)       /*!< ADC offset is negative */
#define LL_ADC_OFFSET_SIGN_POSITIVE        (ADC_OFCFGR1_POSOFF) /*!< ADC offset is positive */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_GROUPS  ADC instance - Groups
  * @{
  */
#define LL_ADC_GROUP_REGULAR               (0x00000001UL)     /*!< ADC group regular (available on all STM32 devices) */
#define LL_ADC_GROUP_INJECTED              (0x00000002UL)     /*!< ADC group injected (not available on all STM32
                                                                   devices)*/
#define LL_ADC_GROUP_REGULAR_INJECTED      (0x00000003UL)     /*!< ADC both groups regular and injected */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_CHANNEL  ADC instance - Channel number
  * @{
  */
#define LL_ADC_CHANNEL_0            ( 0UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN0  */
#define LL_ADC_CHANNEL_1            ( 1UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN1  */
#define LL_ADC_CHANNEL_2            ( 2UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN2  */
#define LL_ADC_CHANNEL_3            ( 3UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN3  */
#define LL_ADC_CHANNEL_4            ( 4UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN4  */
#define LL_ADC_CHANNEL_5            ( 5UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN5  */
#define LL_ADC_CHANNEL_6            ( 6UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN6  */
#define LL_ADC_CHANNEL_7            ( 7UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN7  */
#define LL_ADC_CHANNEL_8            ( 8UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN8  */
#define LL_ADC_CHANNEL_9            ( 9UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN9  */
#define LL_ADC_CHANNEL_10           (10UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN10 */
#define LL_ADC_CHANNEL_11           (11UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN11 */
#define LL_ADC_CHANNEL_12           (12UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN12 */
#define LL_ADC_CHANNEL_13           (13UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN13 */
#define LL_ADC_CHANNEL_14           (14UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN14 */
#define LL_ADC_CHANNEL_15           (15UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN15 */
#define LL_ADC_CHANNEL_16           (16UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN16 */
#define LL_ADC_CHANNEL_17           (17UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN17 */
#define LL_ADC_CHANNEL_18           (18UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN18 */
#define LL_ADC_CHANNEL_19           (19UL | ADC_CHANNEL_EXTERNAL | LL_ADC_PATH_INTERNAL_NONE)  /*!< Channel ADCx_IN19 */
#define LL_ADC_CHANNEL_VREFINT      (17UL \
                                     | ADC_CHANNEL_INTERNAL_ADC1 \
                                     | LL_ADC_PATH_INTERNAL_VREFINT)                           /*!< ADC internal channel
                                     connected to VrefInt: Internal voltage reference.
                                     On this STM32 series, ADC channel available only on ADC instance: ADC1. */
#define LL_ADC_CHANNEL_VBAT         (16UL \
                                     | ADC_CHANNEL_INTERNAL_ADC2 \
                                     | LL_ADC_PATH_INTERNAL_VBAT)                              /*!< ADC internal channel
                                     connected to Vbat/4: Vbat voltage through a divider ladder of factor 1/4 to have
                                     channel voltage always below Vdda.
                                     On this STM32 series, ADC channel available only on ADC instance: ADC2. */
#define LL_ADC_CHANNEL_VDDCORE      (17UL \
                                     | ADC_CHANNEL_INTERNAL_ADC2 \
                                     | LL_ADC_PATH_INTERNAL_VDDCORE)                           /*!< ADC internal channel
                                     connected to VddCore.
                                     On this STM32 series, ADC channel available only on ADC instance: ADC2. */
static const uint8_t ADC_CHANNEL_DIFF_LUT[2][20] =
{
  {
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN0  -> VREF-       */
    (uint8_t)(LL_ADC_CHANNEL_0),        /*!< Channel ADCx_INN1  -> ADCx_INP0   */
    (uint8_t)(LL_ADC_CHANNEL_6),        /*!< Channel ADCx_INN2  -> ADCx_INP6   */
    (uint8_t)(LL_ADC_CHANNEL_7),        /*!< Channel ADCx_INN3  -> ADCx_INP7   */
    (uint8_t)(LL_ADC_CHANNEL_8),        /*!< Channel ADCx_INN4  -> ADCx_INP8   */
    (uint8_t)(LL_ADC_CHANNEL_9),        /*!< Channel ADCx_INN5  -> ADCx_INP9   */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN6  -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN7  -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN8  -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN9  -> VREF-       */
    (uint8_t)(LL_ADC_CHANNEL_11),       /*!< Channel ADCx_INN10 -> ADCx_INP11  */
    (uint8_t)(LL_ADC_CHANNEL_12),       /*!< Channel ADCx_INN11 -> ADCx_INP12  */
    (uint8_t)(LL_ADC_CHANNEL_13),       /*!< Channel ADCx_INN12 -> ADCx_INP13  */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN13 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN14 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN15 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN16 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN17 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN18 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE)         /*!< Channel ADCx_INN19 -> VREF-       */
  },
  {
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN0  -> VREF-       */
    (uint8_t)(LL_ADC_CHANNEL_0),        /*!< Channel ADCx_INN1  -> ADCx_INP0   */
    (uint8_t)(LL_ADC_CHANNEL_6),        /*!< Channel ADCx_INN2  -> ADCx_INP6   */
    (uint8_t)(LL_ADC_CHANNEL_7),        /*!< Channel ADCx_INN3  -> ADCx_INP7   */
    (uint8_t)(LL_ADC_CHANNEL_8),        /*!< Channel ADCx_INN4  -> ADCx_INP8   */
    (uint8_t)(LL_ADC_CHANNEL_9),        /*!< Channel ADCx_INN5  -> ADCx_INP9   */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN6  -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN7  -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN8  -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN9  -> VREF-       */
    (uint8_t)(LL_ADC_CHANNEL_11),       /*!< Channel ADCx_INN10 -> ADCx_INP11  */
    (uint8_t)(LL_ADC_CHANNEL_12),       /*!< Channel ADCx_INN11 -> ADCx_INP12  */
    (uint8_t)(LL_ADC_CHANNEL_13),       /*!< Channel ADCx_INN12 -> ADCx_INP13  */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN13 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN14 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN15 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN16 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN17 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE),        /*!< Channel ADCx_INN18 -> VREF-       */
    (uint8_t)(ADC_CHANNEL_NONE)         /*!< Channel ADCx_INN19 -> VREF-       */
  }
};
/**
  * @}
  */

/** @defgroup ADC_LL_EC_REG_TRIGGER_SOURCE  ADC group regular - Trigger source
  * @{
  */
#define LL_ADC_REG_TRIG_SOFTWARE           (0x00000000UL)                                         /*!< ADC group regular
                                           conversion trigger internal: SW start. */
#define LL_ADC_REG_TRIG_EXT_EXTI_LINE11    (ADC_REG_TRIG_EXT_EDGE_DEFAULT)                        /*!< ADC group regular
                                           conversion trigger from external peripheral: external interrupt line 11.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM1_CH1       (ADC_CFGR1_EXTSEL_0 | ADC_REG_TRIG_EXT_EDGE_DEFAULT)   /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM1 channel 1 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM1_CH2       (ADC_CFGR1_EXTSEL_1 | ADC_REG_TRIG_EXT_EDGE_DEFAULT)   /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM1 channel 2 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM1_CH3       (ADC_CFGR1_EXTSEL_1 | ADC_CFGR1_EXTSEL_0 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM1 channel 3 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM1_TRGO      (ADC_CFGR1_EXTSEL_2 | ADC_REG_TRIG_EXT_EDGE_DEFAULT)   /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM1 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM1_TRGO2     (ADC_CFGR1_EXTSEL_2 | ADC_CFGR1_EXTSEL_0 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM1 TRGO2 event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM2_CH2       (ADC_CFGR1_EXTSEL_2 | ADC_CFGR1_EXTSEL_1 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM2 channel 2 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM2_TRGO      (ADC_CFGR1_EXTSEL_2 | ADC_CFGR1_EXTSEL_1 \
                                            | ADC_CFGR1_EXTSEL_0 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM2 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM3_CH4       (ADC_CFGR1_EXTSEL_3 | ADC_CFGR1_EXTSEL_1 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM3 channel 4 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM3_TRGO      (ADC_CFGR1_EXTSEL_3 | ADC_CFGR1_EXTSEL_1 \
                                            | ADC_CFGR1_EXTSEL_0 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM3 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM4_CH4       (ADC_CFGR1_EXTSEL_3 | ADC_CFGR1_EXTSEL_2 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM4 channel 4 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM4_TRGO      (ADC_CFGR1_EXTSEL_3 | ADC_CFGR1_EXTSEL_2 \
                                            | ADC_CFGR1_EXTSEL_0 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM4 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM5_TRGO      (ADC_CFGR1_EXTSEL_3 | ADC_CFGR1_EXTSEL_2 \
                                            | ADC_CFGR1_EXTSEL_1 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM5 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM6_TRGO      (ADC_CFGR1_EXTSEL_3 | ADC_CFGR1_EXTSEL_2 \
                                            | ADC_CFGR1_EXTSEL_1 | ADC_CFGR1_EXTSEL_0 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM6 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM7_TRGO      (ADC_CFGR1_EXTSEL_4 | ADC_REG_TRIG_EXT_EDGE_DEFAULT)  /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM7 TRGO event. Trigger edge
                                           set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM8_TRGO      (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_0 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM8 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM8_TRGO2     (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_1 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM8 TRGO2 event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM9_CH1       (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_1 \
                                            | ADC_CFGR1_EXTSEL_0 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM9 channel 1 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM9_TRGO      (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_2 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM9 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM12_TRGO     (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_2 \
                                            | ADC_CFGR1_EXTSEL_0 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM12 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM15_TRGO     (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_2 \
                                            | ADC_CFGR1_EXTSEL_1 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM15 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_TIM18_TRGO     (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_2 \
                                            | ADC_CFGR1_EXTSEL_1 | ADC_CFGR1_EXTSEL_0 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: TIM18 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_LPTIM1_CH1     (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_3 \
                                            | ADC_REG_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group regular
                                           conversion trigger from external peripheral: LPTIM1 channel 1 event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_LPTIM2_CH1     (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_3 \
                                            | ADC_CFGR1_EXTSEL_0 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group regular
                                           conversion trigger from external peripheral: LPTIM2 channel 2 event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_REG_TRIG_EXT_LPTIM3_CH1     (ADC_CFGR1_EXTSEL_4 | ADC_CFGR1_EXTSEL_3 \
                                            | ADC_CFGR1_EXTSEL_1 | ADC_REG_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group regular
                                           conversion trigger from external peripheral: LPTIM3 channel 3 event.
                                           Trigger edge set to rising edge (default setting). */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_REG_TRIGGER_EDGE  ADC group regular - Trigger edge
  * @{
  */
#define LL_ADC_REG_TRIG_EXT_RISING         (ADC_CFGR1_EXTEN_0)                     /*!< ADC group regular conversion
                                           trigger polarity set to rising edge */
#define LL_ADC_REG_TRIG_EXT_FALLING        (ADC_CFGR1_EXTEN_1)                     /*!< ADC group regular conversion
                                           trigger polarity set to falling edge */
#define LL_ADC_REG_TRIG_EXT_RISINGFALLING  (ADC_CFGR1_EXTEN_1 | ADC_CFGR1_EXTEN_0) /*!< ADC group regular conversion
                                           trigger polarity set to both rising and falling edges */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_REG_SAMPLING_MODE  ADC group regular - Sampling mode
  * @{
  */
#define LL_ADC_REG_SAMPLING_MODE_NORMAL    (0x00000000UL)     /*!< ADC conversions sampling phase duration
                                           is defined using  @ref ADC_LL_EC_CHANNEL_SAMPLINGTIME. */
#define LL_ADC_REG_SAMPLING_MODE_BULB      (ADC_CFGR2_BULB)   /*!< ADC conversions sampling phase starts immediately
                                           after end of conversion, and stops upon trigger event.
                                           Note: First conversion is using minimal sampling time
                                           (see @ref ADC_LL_EC_CHANNEL_SAMPLINGTIME). */
#define LL_ADC_REG_SAMPLING_MODE_TRIGGER_CONTROLED  (ADC_CFGR2_SMPTRIG)/*!< ADC conversions sampling phase is controlled
                                           by trigger events: Trigger rising edge  = start sampling,
                                           Trigger falling edge = stop sampling and start conversion. */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_REG_CONTINUOUS_MODE  ADC group regular - Continuous mode
  * @{
  */
#define LL_ADC_REG_CONV_SINGLE             (0x00000000UL)     /*!< ADC conversions performed in single mode:
                                           one conversion per trigger */
#define LL_ADC_REG_CONV_CONTINUOUS         (ADC_CFGR1_CONT)   /*!< ADC conversions performed in continuous mode:
                                           after the first trigger, following conversions launched successively
                                           automatically */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_REG_DATA_TRANSFER  ADC group regular - Data transfer mode of ADC conversion data
  * @{
  */
#define LL_ADC_REG_DR_TRANSFER             (0x00000000UL)       /*!< ADC conversions data are available
                                           in ADC data register only */
#define LL_ADC_REG_DMA_TRANSFER_LIMITED    (ADC_CFGR1_DMNGT_0)  /*!< ADC conversion data are transferred by DMA,
                                           in limited mode (one shot mode): DMA transfer requests are stopped when
                                           number of DMA data transfers (number of ADC conversions) is reached.
                                           This ADC mode is intended to be used with DMA mode non-circular. */
#define LL_ADC_REG_DMA_TRANSFER_UNLIMITED  (ADC_CFGR1_DMNGT_1 \
                                            | ADC_CFGR1_DMNGT_0) /*!< ADC conversion data are transferred by DMA,
                                           in unlimited mode: DMA transfer requests are unlimited, whatever
                                           number of DMA data transferred (number of ADC conversions).
                                           This ADC mode is intended to be used with DMA mode circular. */
#define LL_ADC_REG_MDF_TRANSFER            (ADC_CFGR1_DMNGT_1)  /*!< ADC conversion data are transferred to MDF */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_REG_OVR_DATA_BEHAVIOR  ADC group regular - Overrun behavior on conversion data
  * @{
  */
#define LL_ADC_REG_OVR_DATA_PRESERVED      (0x00000000UL)     /*!< ADC group regular behavior in case of overrun:
                                                                   data preserved */
#define LL_ADC_REG_OVR_DATA_OVERWRITTEN    (ADC_CFGR1_OVRMOD) /*!< ADC group regular behavior in case of overrun:
                                                                   data overwritten */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_REG_SEQ_SCAN_LENGTH  ADC group regular - Sequencer scan length
  * @{
  */
#define LL_ADC_REG_SEQ_SCAN_DISABLE        (0x00000000UL)                 /*!< ADC group regular sequencer disable
                                           (equivalent to sequencer of 1 rank: ADC conversion on only 1 channel) */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS  (ADC_SQR1_L_0)                 /*!< ADC group regular sequencer enable
                                           with 2 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS  (ADC_SQR1_L_1)                 /*!< ADC group regular sequencer enable
                                           with 3 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS  (ADC_SQR1_L_1 | ADC_SQR1_L_0)  /*!< ADC group regular sequencer enable
                                           with 4 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS  (ADC_SQR1_L_2)                 /*!< ADC group regular sequencer enable
                                           with 5 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS  (ADC_SQR1_L_2 | ADC_SQR1_L_0)  /*!< ADC group regular sequencer enable
                                           with 6 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS  (ADC_SQR1_L_2 | ADC_SQR1_L_1) /*!< ADC group regular sequencer enable
                                           with 7 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS  (ADC_SQR1_L_2 | ADC_SQR1_L_1 \
                                            | ADC_SQR1_L_0)                /*!< ADC group regular sequencer enable
                                           with 8 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS  (ADC_SQR1_L_3)                 /*!< ADC group regular sequencer enable
                                           with 9 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS (ADC_SQR1_L_3 | ADC_SQR1_L_0)  /*!< ADC group regular sequencer enable
                                           with 10 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS (ADC_SQR1_L_3 | ADC_SQR1_L_1)  /*!< ADC group regular sequencer enable
                                           with 11 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS (ADC_SQR1_L_3 | ADC_SQR1_L_1 \
                                            | ADC_SQR1_L_0)                /*!< ADC group regular sequencer enable
                                           with 12 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS (ADC_SQR1_L_3 | ADC_SQR1_L_2)  /*!< ADC group regular sequencer enable
                                           with 13 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS (ADC_SQR1_L_3 | ADC_SQR1_L_2 \
                                            | ADC_SQR1_L_0)                /*!< ADC group regular sequencer enable
                                           with 14 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS (ADC_SQR1_L_3 | ADC_SQR1_L_2 \
                                            | ADC_SQR1_L_1)                /*!< ADC group regular sequencerenable
                                           with 15 ranks in the sequence */
#define LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS (ADC_SQR1_L_3 | ADC_SQR1_L_2 \
                                            | ADC_SQR1_L_1 | ADC_SQR1_L_0) /*!< ADC group regular sequencer enable
                                           with 16 ranks in the sequence */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_REG_SEQ_DISCONT_MODE  ADC group regular - Sequencer discontinuous mode
  * @{
  */
#define LL_ADC_REG_SEQ_DISCONT_DISABLE     (0x00000000UL)                            /*!< ADC group regular sequencer
                                           discontinuous mode disable */
#define LL_ADC_REG_SEQ_DISCONT_1RANK       (ADC_CFGR1_DISCEN)                        /*!< ADC group regular sequencer
                                           discontinuous mode enable with sequence interruption every rank */
#define LL_ADC_REG_SEQ_DISCONT_2RANKS      (ADC_CFGR1_DISCNUM_0 | ADC_CFGR1_DISCEN)  /*!< ADC group regular sequencer
                                           discontinuous mode enabled with sequence interruption every 2 ranks */
#define LL_ADC_REG_SEQ_DISCONT_3RANKS      (ADC_CFGR1_DISCNUM_1 | ADC_CFGR1_DISCEN)  /*!< ADC group regular sequencer
                                           discontinuous mode enable with sequence interruption every 3 ranks */
#define LL_ADC_REG_SEQ_DISCONT_4RANKS      (ADC_CFGR1_DISCNUM_1 | ADC_CFGR1_DISCNUM_0 \
                                            | ADC_CFGR1_DISCEN)                        /*!< ADC group regular sequencer
                                           discontinuous mode enable with sequence interruption every 4 ranks */
#define LL_ADC_REG_SEQ_DISCONT_5RANKS      (ADC_CFGR1_DISCNUM_2 | ADC_CFGR1_DISCEN)   /*!< ADC group regular sequencer
                                           discontinuous mode enable with sequence interruption every 5 ranks */
#define LL_ADC_REG_SEQ_DISCONT_6RANKS      (ADC_CFGR1_DISCNUM_2 | ADC_CFGR1_DISCNUM_0 \
                                            | ADC_CFGR1_DISCEN)                        /*!< ADC group regular sequencer
                                           discontinuous mode enable with sequence interruption every 6 ranks */
#define LL_ADC_REG_SEQ_DISCONT_7RANKS      (ADC_CFGR1_DISCNUM_2 | ADC_CFGR1_DISCNUM_1 \
                                            | ADC_CFGR1_DISCEN)                        /*!< ADC group regular sequencer
                                           discontinuous mode enable with sequence interruption every 7 ranks */
#define LL_ADC_REG_SEQ_DISCONT_8RANKS      (ADC_CFGR1_DISCNUM_2 | ADC_CFGR1_DISCNUM_1 \
                                            | ADC_CFGR1_DISCNUM_0 | ADC_CFGR1_DISCEN)  /*!< ADC group regular sequencer
                                           discontinuous mode enable with sequence interruption every 8 ranks */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_REG_SEQ_RANKS  ADC group regular - Sequencer ranks
  * @{
  */
#define LL_ADC_REG_RANK_1                  (ADC_SQR1_REGOFFSET | ADC_REG_RANK_1_SQRX_BITOFFSET_POS)  /*!< ADC group
                                           regular sequencer rank 1 */
#define LL_ADC_REG_RANK_2                  (ADC_SQR1_REGOFFSET | ADC_REG_RANK_2_SQRX_BITOFFSET_POS)  /*!< ADC group
                                           regular sequencer rank 2 */
#define LL_ADC_REG_RANK_3                  (ADC_SQR1_REGOFFSET | ADC_REG_RANK_3_SQRX_BITOFFSET_POS)  /*!< ADC group
                                           regular sequencer rank 3 */
#define LL_ADC_REG_RANK_4                  (ADC_SQR1_REGOFFSET | ADC_REG_RANK_4_SQRX_BITOFFSET_POS)  /*!< ADC group
                                           regular sequencer rank 4 */
#define LL_ADC_REG_RANK_5                  (ADC_SQR2_REGOFFSET | ADC_REG_RANK_5_SQRX_BITOFFSET_POS)  /*!< ADC group
                                           regular sequencer rank 5 */
#define LL_ADC_REG_RANK_6                  (ADC_SQR2_REGOFFSET | ADC_REG_RANK_6_SQRX_BITOFFSET_POS)  /*!< ADC group
                                           regular sequencer rank 6 */
#define LL_ADC_REG_RANK_7                  (ADC_SQR2_REGOFFSET | ADC_REG_RANK_7_SQRX_BITOFFSET_POS)  /*!< ADC group
                                           regular sequencer rank 7 */
#define LL_ADC_REG_RANK_8                  (ADC_SQR2_REGOFFSET | ADC_REG_RANK_8_SQRX_BITOFFSET_POS)  /*!< ADC group
                                           regular sequencer rank 8 */
#define LL_ADC_REG_RANK_9                  (ADC_SQR2_REGOFFSET | ADC_REG_RANK_9_SQRX_BITOFFSET_POS)  /*!< ADC group
                                           regular sequencer rank 9 */
#define LL_ADC_REG_RANK_10                 (ADC_SQR3_REGOFFSET | ADC_REG_RANK_10_SQRX_BITOFFSET_POS) /*!< ADC group
                                           regular sequencer rank 10 */
#define LL_ADC_REG_RANK_11                 (ADC_SQR3_REGOFFSET | ADC_REG_RANK_11_SQRX_BITOFFSET_POS) /*!< ADC group
                                           regular sequencer rank 11 */
#define LL_ADC_REG_RANK_12                 (ADC_SQR3_REGOFFSET | ADC_REG_RANK_12_SQRX_BITOFFSET_POS) /*!< ADC group
                                           regular sequencer rank 12 */
#define LL_ADC_REG_RANK_13                 (ADC_SQR3_REGOFFSET | ADC_REG_RANK_13_SQRX_BITOFFSET_POS) /*!< ADC group
                                           regular sequencer rank 13 */
#define LL_ADC_REG_RANK_14                 (ADC_SQR3_REGOFFSET | ADC_REG_RANK_14_SQRX_BITOFFSET_POS) /*!< ADC group
                                           regular sequencer rank 14 */
#define LL_ADC_REG_RANK_15                 (ADC_SQR4_REGOFFSET | ADC_REG_RANK_15_SQRX_BITOFFSET_POS) /*!< ADC group
                                           regular sequencer rank 15 */
#define LL_ADC_REG_RANK_16                 (ADC_SQR4_REGOFFSET | ADC_REG_RANK_16_SQRX_BITOFFSET_POS) /*!< ADC group
                                           regular sequencer rank 16 */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_INJ_TRIGGER_SOURCE  ADC group injected - Trigger source
  * @{
  */
#define LL_ADC_INJ_TRIG_SOFTWARE           (0x00000000UL)                                        /*!< ADC group inject.
                                           conversion trigger internal: SW start. */
#define LL_ADC_INJ_TRIG_EXT_EXTI_LINE15    (ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                       /*!< ADC group inject.
                                           conversion trigger from external peripheral: external interrupt line 15.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM1_CH4       (ADC_JSQR_JEXTSEL_0 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)  /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM1 channel 4 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM1_TRGO      (ADC_JSQR_JEXTSEL_2 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)  /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM1 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM1_TRGO2     (ADC_JSQR_JEXTSEL_2 | ADC_JSQR_JEXTSEL_0 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM1 TRGO2 event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM2_CH1       (ADC_JSQR_JEXTSEL_2 | ADC_JSQR_JEXTSEL_1 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM2 channel 1 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM2_TRGO      (ADC_JSQR_JEXTSEL_2 | ADC_JSQR_JEXTSEL_1 \
                                            | ADC_JSQR_JEXTSEL_0 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM2 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM3_CH1       (ADC_JSQR_JEXTSEL_3 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)  /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM3 channel 1 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM3_CH3       (ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_0 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM3 channel 3 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM3_CH4       (ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_1 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM3 channel 4 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM3_TRGO      (ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_1 \
                                            | ADC_JSQR_JEXTSEL_0 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM3 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM4_TRGO      (ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_2 \
                                            | ADC_JSQR_JEXTSEL_0 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM4 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM5_TRGO      (ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_2 \
                                            | ADC_JSQR_JEXTSEL_1 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM5 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM6_TRGO      (ADC_JSQR_JEXTSEL_3 | ADC_JSQR_JEXTSEL_2 \
                                            | ADC_JSQR_JEXTSEL_1 | ADC_JSQR_JEXTSEL_0 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM6 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM7_TRGO      (ADC_JSQR_JEXTSEL_4 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)  /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM7 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM8_TRGO      (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_0 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM8 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM8_TRGO2     (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_1 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM8 TRGO2 event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM9_CH2       (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_1 \
                                            | ADC_JSQR_JEXTSEL_0 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM9 channel 2 event
                                           (capture compare: input capture or output capture).
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM9_TRGO      (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_2 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM9 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM12_TRGO     (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_2 \
                                            | ADC_JSQR_JEXTSEL_0 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM12 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM15_TRGO     (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_2 \
                                            | ADC_JSQR_JEXTSEL_1 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM12 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_TIM18_TRGO     (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_2 \
                                            | ADC_JSQR_JEXTSEL_1 | ADC_JSQR_JEXTSEL_0 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: TIM12 TRGO event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_LPTIM1_CH2     (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_3 \
                                            | ADC_INJ_TRIG_EXT_EDGE_DEFAULT)                      /*!< ADC group inject.
                                           conversion trigger from external peripheral: LPTIM1 channel 2 event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_LPTIM2_CH2     (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_3 \
                                            | ADC_JSQR_JEXTSEL_0 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group inject.
                                           conversion trigger from external peripheral: LPTIM2 channel 2 event.
                                           Trigger edge set to rising edge (default setting). */
#define LL_ADC_INJ_TRIG_EXT_LPTIM3_CH2     (ADC_JSQR_JEXTSEL_4 | ADC_JSQR_JEXTSEL_3 \
                                            | ADC_JSQR_JEXTSEL_1 | ADC_INJ_TRIG_EXT_EDGE_DEFAULT) /*!< ADC group inject.
                                           conversion trigger from external peripheral: LPTIM3 channel 2 event.
                                           Trigger edge set to rising edge (default setting). */

/**
  * @}
  */

/** @defgroup ADC_LL_EC_INJ_TRIGGER_EDGE  ADC group injected - Trigger edge
  * @{
  */
#define LL_ADC_INJ_TRIG_EXT_RISING         (ADC_JSQR_JEXTEN_0) /*!< ADC group injected conversion trigger polarity
                                           set to rising edge */
#define LL_ADC_INJ_TRIG_EXT_FALLING        (ADC_JSQR_JEXTEN_1) /*!< ADC group injected conversion trigger polarity
                                           set to falling edge */
#define LL_ADC_INJ_TRIG_EXT_RISINGFALLING  (ADC_JSQR_JEXTEN_1 \
                                            | ADC_JSQR_JEXTEN_0) /*!< ADC group injected conversion trigger polarity
                                           set to both rising and falling edges */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_INJ_TRIG_AUTO  ADC group injected - Automatic trigger mode
  * @{
  */
#define LL_ADC_INJ_TRIG_INDEPENDENT        (0x00000000UL)     /*!< ADC group injected conversion trigger independent.
                                           Setting mandatory if ADC group injected injected trigger source is set to
                                           an external trigger. */
#define LL_ADC_INJ_TRIG_FROM_GRP_REGULAR   (ADC_CFGR1_JAUTO)  /*!< ADC group injected conversion trigger from ADC group
                                           regular. Setting compliant only with group injected trigger source set to
                                           SW start, without any further action on  ADC group injected conversion start
                                           or stop: in this case, ADC group injected is controlled only from ADC group
                                           regular. */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_INJ_SEQ_SCAN_LENGTH  ADC group injected - Sequencer scan length
  * @{
  */
#define LL_ADC_INJ_SEQ_SCAN_DISABLE        (0x00000000UL)  /*!< ADC group injected sequencer disable
                                           (equivalent to sequencer of 1 rank: ADC conversion on only 1 channel) */
#define LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS  (ADC_JSQR_JL_0) /*!< ADC group injected sequencer enable with 2 ranks
                                           in the sequence */
#define LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS  (ADC_JSQR_JL_1) /*!< ADC group injected sequencer enable with 3 ranks
                                           in the sequence */
#define LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS  (ADC_JSQR_JL_1 \
                                            | ADC_JSQR_JL_0) /*!< ADC group injected sequencer enable with 4 ranks
                                           in the sequence */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_INJ_SEQ_DISCONT_MODE  ADC group injected - Sequencer discontinuous mode
  * @{
  */
#define LL_ADC_INJ_SEQ_DISCONT_DISABLE     (0x00000000UL)      /*!< ADC group injected sequencer discontinuous mode
                                           disable */
#define LL_ADC_INJ_SEQ_DISCONT_1RANK       (ADC_CFGR1_JDISCEN) /*!< ADC group injected sequencer discontinuous mode
                                           enable with sequence interruption every rank */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_INJ_SEQ_RANKS  ADC group injected - Sequencer ranks
  * @{
  */
#define LL_ADC_INJ_RANK_1                  (ADC_JDR1_REGOFFSET \
                                            | ADC_INJ_RANK_1_JSQR_BITOFFSET_POS) /*!< ADC group injected sequencer
                                           rank 1 */
#define LL_ADC_INJ_RANK_2                  (ADC_JDR2_REGOFFSET \
                                            | ADC_INJ_RANK_2_JSQR_BITOFFSET_POS) /*!< ADC group injected sequencer
                                           rank 2 */
#define LL_ADC_INJ_RANK_3                  (ADC_JDR3_REGOFFSET \
                                            | ADC_INJ_RANK_3_JSQR_BITOFFSET_POS) /*!< ADC group injected sequencer
                                           rank 3 */
#define LL_ADC_INJ_RANK_4                  (ADC_JDR4_REGOFFSET \
                                            | ADC_INJ_RANK_4_JSQR_BITOFFSET_POS) /*!< ADC group injected sequencer
                                           rank 4 */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_CHANNEL_SAMPLINGTIME  Channel - Sampling time
  * @{
  */
#define LL_ADC_SAMPLINGTIME_1CYCLE_5       (0x00000000UL)           /*!< Sampling time 1.5 ADC clock cycle */
#define LL_ADC_SAMPLINGTIME_2CYCLES_5      (ADC_SMPR2_SMP10_0)      /*!< Sampling time 2.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_6CYCLES_5      (ADC_SMPR2_SMP10_1)      /*!< Sampling time 6.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_11CYCLES_5     (ADC_SMPR2_SMP10_1   \
                                            | ADC_SMPR2_SMP10_0)    /*!< Sampling time 11.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_23CYCLES_5     (ADC_SMPR2_SMP10_2)      /*!< Sampling time 23.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_46CYCLES_5     (ADC_SMPR2_SMP10_2   \
                                            | ADC_SMPR2_SMP10_0)    /*!< Sampling time 46.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_246CYCLES_5    (ADC_SMPR2_SMP10_2   \
                                            | ADC_SMPR2_SMP10_1)    /*!< Sampling time 246.5 ADC clock cycles */
#define LL_ADC_SAMPLINGTIME_1499CYCLES_5   (ADC_SMPR2_SMP10_2   \
                                            | ADC_SMPR2_SMP10_1 \
                                            | ADC_SMPR2_SMP10_0)    /*!< Sampling time 1499.5 ADC clock cycles */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_CHANNEL_SINGLE_DIFF_ENDING  Channel - Single or differential ending
  * @{
  */

#define LL_ADC_SINGLE_ENDED                (ADC_CALFACT_CALFACT_S)                   /*!< ADC channel ending set to
                                           single ended (literal also used to set calibration mode) */
#define LL_ADC_DIFFERENTIAL_ENDED          (ADC_CR_ADCALDIF | ADC_CALFACT_CALFACT_D) /*!< ADC channel ending set to
                                           differential (literal also used to set calibration mode) */
#define LL_ADC_BOTH_SINGLE_DIFF_ENDED      (LL_ADC_SINGLE_ENDED \
                                            | LL_ADC_DIFFERENTIAL_ENDED) /*!< ADC channel ending set to both single
                                           ended and differential (literal used only to set calibration factors) */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_AWD_NUMBER Analog watchdog - Analog watchdog number
  * @{
  */
#define LL_ADC_AWD1                        (ADC_AWD_CR1_CHANNEL_MASK \
                                            | ADC_AWD_CR1_REGOFFSET) /*!< ADC analog watchdog number 1 */
#define LL_ADC_AWD2                        (ADC_AWD_CR23_CHANNEL_MASK \
                                            | ADC_AWD_CR2_REGOFFSET) /*!< ADC analog watchdog number 2 */
#define LL_ADC_AWD3                        (ADC_AWD_CR23_CHANNEL_MASK \
                                            | ADC_AWD_CR3_REGOFFSET) /*!< ADC analog watchdog number 3 */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_AWD_CHANNELS  Analog watchdog - Monitored channels
  * @{
  */
#define LL_ADC_AWD_DISABLE                 (0x00000000UL)                            /*!< ADC analog watchdog monitoring
                                           disabled */
#define LL_ADC_AWD_ALL_CHANNELS_REG        (ADC_AWD_CR23_CHANNEL_MASK \
                                            | ADC_CFGR1_AWD1EN)                      /*!< ADC analog watchdog monitoring
                                           of all channels, converted by group regular only */
#define LL_ADC_AWD_ALL_CHANNELS_INJ        (ADC_AWD_CR23_CHANNEL_MASK \
                                            | ADC_CFGR1_JAWD1EN)                     /*!< ADC analog watchdog monitoring
                                           of all channels, converted by group injected only */
#define LL_ADC_AWD_ALL_CHANNELS_REG_INJ    (ADC_AWD_CR23_CHANNEL_MASK \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN)  /*!< ADC analog watchdog monitoring
                                           of all channels, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_0_REG           (ADC_CHANNEL_0_NUMBER | ADC_CHANNEL_0_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN0, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_0_INJ           (ADC_CHANNEL_0_NUMBER | ADC_CHANNEL_0_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN0, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_0_REG_INJ       (ADC_CHANNEL_0_NUMBER | ADC_CHANNEL_0_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN0, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_1_REG           (ADC_CHANNEL_1_NUMBER | ADC_CHANNEL_1_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN1, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_1_INJ           (ADC_CHANNEL_1_NUMBER | ADC_CHANNEL_1_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN1, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_1_REG_INJ       (ADC_CHANNEL_1_NUMBER | ADC_CHANNEL_1_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN1, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_2_REG           (ADC_CHANNEL_2_NUMBER | ADC_CHANNEL_2_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN2, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_2_INJ           (ADC_CHANNEL_2_NUMBER | ADC_CHANNEL_2_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN2, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_2_REG_INJ       (ADC_CHANNEL_2_NUMBER | ADC_CHANNEL_2_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN2, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_3_REG           (ADC_CHANNEL_3_NUMBER | ADC_CHANNEL_3_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN3, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_3_INJ           (ADC_CHANNEL_3_NUMBER | ADC_CHANNEL_3_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN3, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_3_REG_INJ       (ADC_CHANNEL_3_NUMBER | ADC_CHANNEL_3_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN3, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_4_REG           (ADC_CHANNEL_4_NUMBER | ADC_CHANNEL_4_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN4, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_4_INJ           (ADC_CHANNEL_4_NUMBER | ADC_CHANNEL_4_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN4, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_4_REG_INJ       (ADC_CHANNEL_4_NUMBER | ADC_CHANNEL_4_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN4, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_5_REG           (ADC_CHANNEL_5_NUMBER | ADC_CHANNEL_5_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN5, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_5_INJ           (ADC_CHANNEL_5_NUMBER | ADC_CHANNEL_5_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN5, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_5_REG_INJ       (ADC_CHANNEL_5_NUMBER | ADC_CHANNEL_5_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN5, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_6_REG           (ADC_CHANNEL_6_NUMBER | ADC_CHANNEL_6_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN6, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_6_INJ           (ADC_CHANNEL_6_NUMBER | ADC_CHANNEL_6_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN6, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_6_REG_INJ       (ADC_CHANNEL_6_NUMBER | ADC_CHANNEL_6_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN6, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_7_REG           (ADC_CHANNEL_7_NUMBER | ADC_CHANNEL_7_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN7, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_7_INJ           (ADC_CHANNEL_7_NUMBER | ADC_CHANNEL_7_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN7, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_7_REG_INJ       (ADC_CHANNEL_7_NUMBER | ADC_CHANNEL_7_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN7, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_8_REG           (ADC_CHANNEL_8_NUMBER | ADC_CHANNEL_8_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN8, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_8_INJ           (ADC_CHANNEL_8_NUMBER | ADC_CHANNEL_8_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN8, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_8_REG_INJ       (ADC_CHANNEL_8_NUMBER | ADC_CHANNEL_8_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN8, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_9_REG           (ADC_CHANNEL_9_NUMBER | ADC_CHANNEL_9_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN9, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_9_INJ           (ADC_CHANNEL_9_NUMBER | ADC_CHANNEL_9_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN9, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_9_REG_INJ       (ADC_CHANNEL_9_NUMBER | ADC_CHANNEL_9_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN9, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_10_REG          (ADC_CHANNEL_10_NUMBER | ADC_CHANNEL_10_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN10, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_10_INJ          (ADC_CHANNEL_10_NUMBER | ADC_CHANNEL_10_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN10, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_10_REG_INJ      (ADC_CHANNEL_10_NUMBER | ADC_CHANNEL_10_BITFIELD\
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN10, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_11_REG          (ADC_CHANNEL_11_NUMBER | ADC_CHANNEL_11_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN11, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_11_INJ          (ADC_CHANNEL_11_NUMBER | ADC_CHANNEL_11_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN11, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_11_REG_INJ      (ADC_CHANNEL_11_NUMBER | ADC_CHANNEL_11_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN11, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_12_REG          (ADC_CHANNEL_12_NUMBER | ADC_CHANNEL_12_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN12, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_12_INJ          (ADC_CHANNEL_12_NUMBER | ADC_CHANNEL_12_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN12, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_12_REG_INJ      (ADC_CHANNEL_12_NUMBER | ADC_CHANNEL_12_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN12, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_13_REG          (ADC_CHANNEL_13_NUMBER | ADC_CHANNEL_13_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN13, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_13_INJ          (ADC_CHANNEL_13_NUMBER | ADC_CHANNEL_13_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN13, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_13_REG_INJ      (ADC_CHANNEL_13_NUMBER | ADC_CHANNEL_13_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN13, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_14_REG          (ADC_CHANNEL_14_NUMBER | ADC_CHANNEL_14_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN14, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_14_INJ          (ADC_CHANNEL_14_NUMBER | ADC_CHANNEL_14_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN14, converted by group only */
#define LL_ADC_AWD_CHANNEL_14_REG_INJ      (ADC_CHANNEL_14_NUMBER | ADC_CHANNEL_14_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN14, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_15_REG          (ADC_CHANNEL_15_NUMBER | ADC_CHANNEL_15_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           monitoring of ADC channel ADCx_IN15, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_15_INJ          (ADC_CHANNEL_15_NUMBER | ADC_CHANNEL_15_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN15, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_15_REG_INJ      (ADC_CHANNEL_15_NUMBER | ADC_CHANNEL_15_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN15, converted by either group
                                           regular or injected */
#define LL_ADC_AWD_CHANNEL_16_REG          (ADC_CHANNEL_16_NUMBER | ADC_CHANNEL_16_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN16, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_16_INJ          (ADC_CHANNEL_16_NUMBER | ADC_CHANNEL_16_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN16, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_16_REG_INJ      (ADC_CHANNEL_16_NUMBER | ADC_CHANNEL_16_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN16, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_17_REG          (ADC_CHANNEL_17_NUMBER | ADC_CHANNEL_17_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN17, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_17_INJ          (ADC_CHANNEL_17_NUMBER | ADC_CHANNEL_17_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN17, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_17_REG_INJ      (ADC_CHANNEL_17_NUMBER | ADC_CHANNEL_17_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN17, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_18_REG          (ADC_CHANNEL_18_NUMBER | ADC_CHANNEL_18_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN18, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_18_INJ          (ADC_CHANNEL_18_NUMBER | ADC_CHANNEL_18_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN18, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_18_REG_INJ      (ADC_CHANNEL_18_NUMBER | ADC_CHANNEL_18_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN18, converted by either group regular or injected */
#define LL_ADC_AWD_CHANNEL_19_REG          (ADC_CHANNEL_19_NUMBER | ADC_CHANNEL_19_BITFIELD \
                                            | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)  /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN19, converted by group regular only */
#define LL_ADC_AWD_CHANNEL_19_INJ          (ADC_CHANNEL_19_NUMBER | ADC_CHANNEL_19_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL) /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN19, converted by group injected only */
#define LL_ADC_AWD_CHANNEL_19_REG_INJ      (ADC_CHANNEL_19_NUMBER | ADC_CHANNEL_19_BITFIELD \
                                            | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN \
                                            | ADC_CFGR1_AWD1SGL)                     /*!< ADC analog watchdog monitoring
                                           of ADC channel ADCx_IN19, converted by either group regular or injected */
#define LL_ADC_AWD_CH_VREFINT_REG          LL_ADC_AWD_CHANNEL_17_REG                 /*!< ADC analog watchdog monitoring
                                           of ADC internal channel connected to VrefInt: Internal voltage reference,
                                           converted by group regular only */
#define LL_ADC_AWD_CH_VREFINT_INJ          LL_ADC_AWD_CHANNEL_17_INJ                 /*!< ADC analog watchdog monitoring
                                           of ADC internal channel connected to VrefInt: Internal voltage reference,
                                           converted by group injected only */
#define LL_ADC_AWD_CH_VREFINT_REG_INJ      LL_ADC_AWD_CHANNEL_17_REG_INJ             /*!< ADC analog watchdog monitoring
                                           of ADC internal channel connected to VrefInt: Internal voltage reference,
                                           converted by either group regular or injected */
#define LL_ADC_AWD_CH_VBAT_REG             LL_ADC_AWD_CHANNEL_16_REG                 /*!< ADC analog watchdog monitoring
                                           of ADC internal channel connected to Vbat/4: Vbat voltage through
                                           a divider ladder of factor 1/4 to have Vbat always below Vdda,
                                           converted by group regular only */
#define LL_ADC_AWD_CH_VBAT_INJ             LL_ADC_AWD_CHANNEL_16_INJ                 /*!< ADC analog watchdog monitoring
                                           of ADC internal channel connected to Vbat/4: Vbat voltage through
                                           a divider ladder of factor 1/4 to have Vbat always below Vdda,
                                           converted by group injected only */
#define LL_ADC_AWD_CH_VBAT_REG_INJ         LL_ADC_AWD_CHANNEL_16_REG_INJ             /*!< ADC analog watchdog monitoring
                                           of ADC internal channel connected to Vbat/4: Vbat voltage through
                                           a divider ladder of factor 1/4 to have Vbat always below Vdda */
#define LL_ADC_AWD_CH_VDDCORE_REG          LL_ADC_AWD_CHANNEL_17_REG                 /*!< ADC analog watchdog monitoring
                                           of ADC internal channel connected to VddCore,
                                           converted by group regular only */
#define LL_ADC_AWD_CH_VDDCORE_INJ          LL_ADC_AWD_CHANNEL_17_INJ                 /*!< ADC analog watchdog monitoring
                                           of ADC internal channel connected to VddCore,
                                           converted by group injected only */
#define LL_ADC_AWD_CH_VDDCORE_REG_INJ      LL_ADC_AWD_CHANNEL_17_REG_INJ             /*!< ADC analog watchdog monitoring
                                           of ADC internal channel connected to VddCore,
                                           converted by either group regular or injected */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_AWD_THRESHOLDS  Analog watchdog - Thresholds
  * @{
  */
#define LL_ADC_AWD_THRESHOLD_HIGH          (0x1UL)                     /*!< ADC analog watchdog threshold high */
#define LL_ADC_AWD_THRESHOLD_LOW           (0x0UL)                     /*!< ADC analog watchdog threshold low */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_AWD_FILTERING_CONFIG  Analog watchdog - filtering config
  * @{
  */
#define LL_ADC_AWD_FILTERING_NONE          (0x00000000UL)             /*!< ADC analog watchdog filtering disabled:
                                           one out-of-window sample is needed to raise flag or interrupt */
#define LL_ADC_AWD_FILTERING_2SAMPLES      (ADC_AWD1HTR_AWDFILT_0)    /*!< ADC analog watchdog filtering enabled: 2
                                           out-of-window samples are needed to raise flag or interrupt */
#define LL_ADC_AWD_FILTERING_3SAMPLES      (ADC_AWD1HTR_AWDFILT_1)    /*!< ADC analog watchdog filtering enabled: 3
                                           out-of-window samples are needed to raise flag or interrupt */
#define LL_ADC_AWD_FILTERING_4SAMPLES      (ADC_AWD1HTR_AWDFILT_1 \
                                            | ADC_AWD1HTR_AWDFILT_0)   /*!< ADC analog watchdog filtering enabled: 4
                                           out-of-window samples are needed to raise flag or interrupt */
#define LL_ADC_AWD_FILTERING_5SAMPLES      (ADC_AWD1HTR_AWDFILT_2)    /*!< ADC analog watchdog filtering enabled: 5
                                           out-of-window samples are needed to raise flag or interrupt */
#define LL_ADC_AWD_FILTERING_6SAMPLES      (ADC_AWD1HTR_AWDFILT_2 \
                                            | ADC_AWD1HTR_AWDFILT_0)   /*!< ADC analog watchdog filtering enabled: 6
                                           out-of-window samples are needed to raise flag or interrupt */
#define LL_ADC_AWD_FILTERING_7SAMPLES      (ADC_AWD1HTR_AWDFILT_2 \
                                            | ADC_AWD1HTR_AWDFILT_1)   /*!< ADC analog watchdog filtering enabled: 7
                                           out-of-window samples are needed to raise flag or interrupt */
#define LL_ADC_AWD_FILTERING_8SAMPLES      (ADC_AWD1HTR_AWDFILT_2 \
                                            | ADC_AWD1HTR_AWDFILT_1 \
                                            | ADC_AWD1HTR_AWDFILT_0)   /*!< ADC analog watchdog filtering enabled: 8
                                           out-of-window samples are needed to raise flag or interrupt */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_OVS_SCOPE  Oversampling - Oversampling scope
  * @{
  */
#define LL_ADC_OVS_DISABLE                 (0x00000000UL)                      /*!< ADC oversampling disabled. */
#define LL_ADC_OVS_GRP_REGULAR_CONTINUED   (ADC_CFGR2_ROVSE)                   /*!< ADC oversampling on conversions of
                                           ADC group regular. If group injected interrupts group regular:
                                           when ADC group injected is triggered, the oversampling on ADC group regular
                                           is temporary stopped and continued afterwards. */
#define LL_ADC_OVS_GRP_REGULAR_RESUMED     (ADC_CFGR2_ROVSM | ADC_CFGR2_ROVSE) /*!< ADC oversampling on conversions of
                                           ADC group regular. If group injected interrupts group regular:
                                           when ADC group injected is triggered, the oversampling on ADC group regular
                                           is resumed from start (oversampler buffer reset). */
#define LL_ADC_OVS_GRP_INJECTED            (ADC_CFGR2_JOVSE)                   /*!< ADC oversampling on conversions of
                                           ADC group injected. */
#define LL_ADC_OVS_GRP_INJ_REG_RESUMED     (ADC_CFGR2_JOVSE | ADC_CFGR2_ROVSE) /*!< ADC oversampling on conversions of
                                           both ADC groups regular and injected. If group injected interrupting group
                                           regular: when ADC group injected is triggered, the oversampling on ADC group
                                           regular is resumed from start (oversampler buffer reset). */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_OVS_DISCONT_MODE  Oversampling - Discontinuous mode
  * @{
  */
#define LL_ADC_OVS_REG_CONT                (0x00000000UL)     /*!< ADC oversampling discontinuous mode: continuous mode
                                           (all conversions of oversampling ratio are done from 1 trigger) */
#define LL_ADC_OVS_REG_DISCONT             (ADC_CFGR2_TROVS)  /*!< ADC oversampling discontinuous mode: discontinuous
                                           mode (each conversion of oversampling ratio needs a trigger) */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_OVS_SHIFT  Oversampling - Data right shift
  * @{
  */
#define LL_ADC_OVS_SHIFT_NONE              (0x00000000UL)                        /*!< ADC oversampling no shift
                                           (sum of the ADC conversions data is not divided to result as oversampling
                                           conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_1           (ADC_CFGR2_OVSS_0)                    /*!< ADC oversampling right shift of 1
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 2
                                           to result as oversampling conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_2           (ADC_CFGR2_OVSS_1)                    /*!< ADC oversampling right shift of 2
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 4
                                           to result as oversampling conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_3           (ADC_CFGR2_OVSS_1 | ADC_CFGR2_OVSS_0) /*!< ADC oversampling right shift of 3
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 8
                                           to result as oversampling conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_4           (ADC_CFGR2_OVSS_2)                    /*!< ADC oversampling right shift of 4
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 16
                                           to result as oversampling conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_5           (ADC_CFGR2_OVSS_2 | ADC_CFGR2_OVSS_0) /*!< ADC oversampling right shift of 5
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 32
                                           to result as oversampling conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_6           (ADC_CFGR2_OVSS_2 | ADC_CFGR2_OVSS_1) /*!< ADC oversampling right shift of 6
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 64
                                           to result as oversampling conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_7           (ADC_CFGR2_OVSS_2 | ADC_CFGR2_OVSS_1 \
                                            | ADC_CFGR2_OVSS_0)                   /*!< ADC oversampling right shift of 7
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 128
                                           to result as oversampling conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_8           (ADC_CFGR2_OVSS_3)                    /*!< ADC oversampling right shift of 8
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 256
                                           to result as oversampling conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_9           (ADC_CFGR2_OVSS_3 | ADC_CFGR2_OVSS_0) /*!< ADC oversampling right shift of 8
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 512
                                           to result as oversampling conversion data) */
#define LL_ADC_OVS_SHIFT_RIGHT_10          (ADC_CFGR2_OVSS_3 | ADC_CFGR2_OVSS_1) /*!< ADC oversampling right shift of 8
                                           (sum of the ADC conversions data (after OVS ratio) is divided by 1024
                                           to result as oversampling conversion data) */
/**
  * @}
  */

#if defined(ADC_MULTIMODE_SUPPORT)
/** @defgroup ADC_LL_EC_MULTI_MODE  Multimode - Mode
  * @{
  */
#define LL_ADC_MULTI_INDEPENDENT           (0x00000000UL)                    /*!< ADC dual mode disabled (ADC
                                           independent mode) */
#define LL_ADC_MULTI_DUAL_REG_SIMULT       (ADC_CCR_DUAL_2 | ADC_CCR_DUAL_1) /*!< ADC dual mode enabled: group regular
                                           simultaneous */
#define LL_ADC_MULTI_DUAL_REG_INTERL       (ADC_CCR_DUAL_2 | ADC_CCR_DUAL_1 \
                                            | ADC_CCR_DUAL_0)                  /*!< ADC dual mode enabled: Combined group
                                           regular interleaved */
#define LL_ADC_MULTI_DUAL_INJ_SIMULT       (ADC_CCR_DUAL_2 | ADC_CCR_DUAL_0) /*!< ADC dual mode enabled: group injected
                                           simultaneous */
#define LL_ADC_MULTI_DUAL_INJ_ALTERN       (ADC_CCR_DUAL_3 | ADC_CCR_DUAL_0) /*!< ADC dual mode enabled: group injected
                                           alternate trigger. Works only with external triggers (not SW start) */
#define LL_ADC_MULTI_DUAL_REG_SIM_INJ_SIM  (ADC_CCR_DUAL_0)                  /*!< ADC dual mode enabled: Combined group
                                           regular simultaneous + group injected simultaneous */
#define LL_ADC_MULTI_DUAL_REG_SIM_INJ_ALT  (ADC_CCR_DUAL_1)                  /*!< ADC dual mode enabled: Combined group
                                           regular simultaneous + group injected alternate trigger */
#define LL_ADC_MULTI_DUAL_REG_INT_INJ_SIM  (ADC_CCR_DUAL_1 | ADC_CCR_DUAL_0) /*!< ADC dual mode enabled: Combined group
                                           regular interleaved + group injected simultaneous */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_MULTI_DATA_FORMAT  Multimode - Data format
  * @{
  */
#define LL_ADC_MULTI_REG_DATA_EACH_ADC     (0x00000000UL)                      /*!< ADC multimode group regular
                                           data format: conversion data in data register of each ADC instance.
                                           If ADC data transfer by DMA is used: each ADC uses its own DMA channel,
                                           with its individual DMA transfer settings. */
#define LL_ADC_MULTI_REG_DATA_COMMON_32B   (ADC_CCR_DAMDF_1)                   /*!< ADC multimode group regular
                                           data format: conversion data in two ADC common instance data registers
                                           (CDR, CDR2) with packing option on 32 bit. In register CDR,
                                           data packing on 32 bit: ADC master and slave data are concatenated
                                           (data master in [15; 0], data slave in [31; 16]), therefore data width
                                           must be lower than 16 bit (even with ADC resolution 12 bit,
                                           higher width reachable by post processing: oversampling, offset, ...).
                                           In register CDR2, data of master and slave are alternatively set in full
                                           register width 32 bit, therefore no constraint on data width.
                                           In case of usage with DMA, CDR generate ones transfer request
                                           and CDR2 two transfer requests per conversion. */
#define LL_ADC_MULTI_REG_DATA_COMMON_16B   (ADC_CCR_DAMDF_1 | ADC_CCR_DAMDF_0) /*!< ADC multimode group regular
                                           data format: conversion data in two ADC common instance data registers
                                           (CDR, CDR2) with packing option on 16 bit. In register CDR,
                                           data packing on 16 bit: ADC master and slave data are concatenated
                                           (data master in [7; 0], data slave in [15; 8]), therefore data width
                                           must be lower than 8 bit (even with ADC resolution 8 bit,
                                           higher width reachable by post processing: oversampling, offset, ...).
                                           In register CDR2, data of master and slave are alternatively set in full
                                           register width 32 bit, therefore no constraint on data width.
                                           In case of usage with DMA, CDR generate ones transfer request
                                           and CDR2 two transfer requests per conversion. */

/* Legacy literals */
#define LL_ADC_MULTI_REG_DMA_EACH_ADC LL_ADC_MULTI_REG_DATA_EACH_ADC
#define LL_ADC_MULTI_REG_DMA_RES_32B  LL_ADC_MULTI_REG_DATA_COMMON_32B
#define LL_ADC_MULTI_REG_DMA_RES_16B  LL_ADC_MULTI_REG_DATA_COMMON_16B
/**
  * @}
  */

/** @defgroup ADC_LL_EC_MULTI_TWOSMP_DELAY  Multimode - Delay between two sampling phases
  * @{
  */
#define LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE   (0x00000000UL)                      /*!< ADC multimode interleaved delay
                                           between two sampling phases: 1 ADC clock cycle */
#define LL_ADC_MULTI_TWOSMP_DELAY_2CYCLES  (ADC_CCR_DELAY_0)                   /*!< ADC multimode interleaved delay
                                           between two sampling phases: 2 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_3CYCLES  (ADC_CCR_DELAY_1)                   /*!< ADC multimode interleaved delay
                                           between two sampling phases: 3 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_4CYCLES  (ADC_CCR_DELAY_1 | ADC_CCR_DELAY_0) /*!< ADC multimode interleaved delay
                                           between two sampling phases: 4 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_5CYCLES  (ADC_CCR_DELAY_2)                   /*!< ADC multimode interleaved delay
                                           between two sampling phases: 5 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_6CYCLES  (ADC_CCR_DELAY_2 | ADC_CCR_DELAY_0) /*!< ADC multimode interleaved delay
                                           between two sampling phases: 6 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_7CYCLES  (ADC_CCR_DELAY_2 | ADC_CCR_DELAY_1) /*!< ADC multimode interleaved delay
                                           between two sampling phases: 7 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_8CYCLES  (ADC_CCR_DELAY_2 | ADC_CCR_DELAY_1 \
                                            | ADC_CCR_DELAY_0)                  /*!< ADC multimode interleaved delay
                                           between two sampling phases: 8 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_9CYCLES  (ADC_CCR_DELAY_3)                   /*!< ADC multimode interleaved delay
                                           between two sampling phases: 9 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_10CYCLES (ADC_CCR_DELAY_3 | ADC_CCR_DELAY_0) /*!< ADC multimode interleaved delay
                                           between two sampling phases: 10 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_11CYCLES (ADC_CCR_DELAY_3 | ADC_CCR_DELAY_1) /*!< ADC multimode interleaved delay
                                           between two sampling phases: 11 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_12CYCLES (ADC_CCR_DELAY_3 | ADC_CCR_DELAY_1 \
                                            | ADC_CCR_DELAY_0)                  /*!< ADC multimode interleaved delay
                                           between two sampling phases: 12 ADC clock cycles */
#define LL_ADC_MULTI_TWOSMP_DELAY_13CYCLES (ADC_CCR_DELAY_3 | ADC_CCR_DELAY_2) /*!< ADC multimode interleaved delay
                                           between two sampling phases: 13 ADC clock cycles */
/**
  * @}
  */

/** @defgroup ADC_LL_EC_MULTI_MASTER_SLAVE  Multimode - ADC master or slave
  * @{
  */
#define LL_ADC_MULTI_MASTER                (ADC_CDR_RDATA_MST) /*!< In multimode, selection among several ADC
                                           instances: ADC master */
#define LL_ADC_MULTI_SLAVE                 (ADC_CDR_RDATA_SLV) /*!< In multimode, selection among several ADC
                                           instances: ADC slave */
#define LL_ADC_MULTI_MASTER_SLAVE          (ADC_CDR_RDATA_SLV \
                                            | ADC_CDR_RDATA_MST) /*!< In multimode, selection among several ADC
                                           instances: both ADC master and ADC slave */
/**
  * @}
  */

#endif /* ADC_MULTIMODE_SUPPORT */

/** @defgroup ADC_LL_EC_HW_DELAYS  Definitions of ADC hardware constraints delays
  * @note   Only ADC peripheral HW delays are defined in ADC LL driver driver,
  *         not timeout values.
  *         For details on delays values, refer to descriptions in source code
  *         above each literal definition.
  * @{
  */

/* Note: Only ADC peripheral HW delays are defined in ADC LL driver driver,   */
/*       not timeout values.                                                  */
/*       Timeout values for ADC operations are dependent to device clock      */
/*       configuration (system clock versus ADC clock),                       */
/*       and therefore must be defined in user application.                   */
/*       Indications for estimation of ADC timeout delays, for this           */
/*       STM32 series:                                                        */
/*       - ADC calibration time: maximum delay is 16384/fADC.                 */
/*         (refer to device datasheet, parameter "tCAL")                      */
/*       - ADC enable time: maximum delay is 1 conversion cycle.              */
/*         (refer to device datasheet, parameter "tSTAB")                     */
/*       - ADC disable time: maximum delay should be a few ADC clock cycles   */
/*       - ADC stop conversion time: maximum delay should be a few ADC clock  */
/*         cycles                                                             */
/*       - ADC conversion time: duration depending on ADC clock and ADC       */
/*         configuration.                                                     */
/*         (refer to device reference manual, section "Timing")               */

/* Delay for ADC stabilization time (ADC voltage regulator start-up time)     */
/* Delay set to maximum value (refer to device datasheet,                     */
/* parameter "tADCVREG_STUP").                                                */
/* Unit: us                                                                   */
#define LL_ADC_DELAY_INTERNAL_REGUL_STAB_US ( 10UL)           /*!< Delay for ADC stabilization time (ADC voltage
                                                              regulator start-up time) */

/* Delay for internal voltage reference stabilization time.                   */
/* Delay set to maximum value (refer to device datasheet,                     */
/* parameter "tstart_vrefint").                                               */
/* Unit: us                                                                   */
#define LL_ADC_DELAY_VREFINT_STAB_US           ( 12UL)        /*!< Delay for internal voltage reference stabilization
                                                                   time */

/* Delay required between ADC end of calibration and ADC enable.              */
/* Note: On this STM32 series, a minimum number of ADC clock cycles           */
/*       are required between ADC end of calibration and ADC enable.          */
/*       Wait time can be computed in user application by waiting for the     */
/*       equivalent number of CPU cycles, by taking into account              */
/*       ratio of CPU clock versus ADC clock prescalers.                      */
/* Unit: ADC clock cycles.                                                    */
#define LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES   (  4UL)        /*!< Delay required between ADC end of calibration
                                                                   and ADC enable */

/**
  * @}
  */

/**
  * @}
  */


/* Exported macro ------------------------------------------------------------*/
/** @defgroup ADC_LL_Exported_Macros ADC Exported Macros
  * @{
  */

/** @defgroup ADC_LL_EM_WRITE_READ Common write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in ADC register
  * @param  __INSTANCE__ ADC Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_ADC_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in ADC register
  * @param  __INSTANCE__ ADC Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_ADC_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/** @defgroup ADC_LL_EM_HELPER_MACRO ADC helper macro
  * @{
  */

/**
  * @brief  Helper macro to get ADC channel number in decimal format
  *         from literals LL_ADC_CHANNEL_x.
  * @note   Example:
  *           __LL_ADC_CHANNEL_TO_DECIMAL_NB(LL_ADC_CHANNEL_4)
  *           will return decimal number "4".
  * @note   The input can be a value from functions where a channel
  *         number is returned, either defined with number
  *         or with bitfield (only one bit must be set).
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval Value between Min_Data=0 and Max_Data=18
  */
#define __LL_ADC_CHANNEL_TO_DECIMAL_NB(__CHANNEL__)  ((__CHANNEL__) & ADC_CHANNEL_NUMBER_MASK)


/**
  * @brief  Helper macro to get ADC channel in literal format LL_ADC_CHANNEL_x
  *         from number in decimal format.
  * @note   Example:
  *           __LL_ADC_DECIMAL_NB_TO_CHANNEL(4)
  *           will return a data equivalent to "LL_ADC_CHANNEL_4".
  * @param  __DECIMAL_NB__ Value between Min_Data=0 and Max_Data=18
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)(3)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)(3)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)(3)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  *         (3) For ADC channel read back from ADC register,
  *             comparison with internal channel parameter to be done
  *             using helper macro @ref __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL().
  */
#define __LL_ADC_DECIMAL_NB_TO_CHANNEL(__DECIMAL_NB__) (__DECIMAL_NB__)

/**
  * @brief  Helper macro to determine whether the selected channel
  *         corresponds to literal definitions of driver.
  * @note   The different literal definitions of ADC channels are:
  *         - ADC internal channel:
  *           LL_ADC_CHANNEL_VREFINT, ...
  *         - ADC external channel (channel connected to a GPIO pin):
  *           LL_ADC_CHANNEL_1, LL_ADC_CHANNEL_2, ...
  * @note   The channel parameter must be a value defined from literal
  *         definition of a ADC internal channel (LL_ADC_CHANNEL_VREFINT, ...),
  *         ADC external channel (LL_ADC_CHANNEL_1, LL_ADC_CHANNEL_2, ...),
  *         must not be a value from functions where a channel number is
  *         returned from ADC registers,
  *         because internal and external channels share the same channel
  *         number in ADC registers. The differentiation is made only with
  *         parameters definitions of driver.
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval Value "0" if the channel corresponds to a parameter definition of a ADC external channel
  *         (channel connected to a GPIO pin).
  *         Value "1" if the channel corresponds to a parameter definition of a ADC internal channel.
  */
#define __LL_ADC_IS_CHANNEL_INTERNAL(__CHANNEL__) (((__CHANNEL__) & ADC_CHANNEL_INTERNAL_MASK) != 0UL)
/**
  * @brief  Helper macro to convert a channel defined from parameter
  *         definition of a ADC internal channel (LL_ADC_CHANNEL_VREFINT, ...),
  *         to its equivalent parameter definition of a ADC external channel
  *         (LL_ADC_CHANNEL_1, LL_ADC_CHANNEL_2, ...).
  * @note   The channel parameter can be, additionally to a value
  *         defined from parameter definition of a ADC internal channel
  *         (LL_ADC_CHANNEL_VREFINT, ...),
  *         a value defined from parameter definition of
  *         ADC external channel (LL_ADC_CHANNEL_1, LL_ADC_CHANNEL_2, ...)
  *         or a value from functions where a channel number is returned
  *         from ADC registers.
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  */
#define __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL(__CHANNEL__) ((__CHANNEL__) & ADC_CHANNEL_NUMBER_MASK)

/**
  * @brief  Helper macro to determine whether the internal channel
  *         selected is available on the ADC instance selected.
  * @note   The channel parameter must be a value defined from parameter
  *         definition of a ADC internal channel (LL_ADC_CHANNEL_VREFINT, ...),
  *         must not be a value defined from parameter definition of
  *         ADC external channel (LL_ADC_CHANNEL_1, LL_ADC_CHANNEL_2, ...)
  *         or a value from functions where a channel number is
  *         returned from ADC registers,
  *         because internal and external channels share the same channel
  *         number in ADC registers. The differentiation is made only with
  *         parameters definitions of driver.
  * @param  __ADC_INSTANCE__ ADC instance
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.
  * @retval Value "0" if the internal channel selected is not available on the ADC instance selected.
  *         Value "1" if the internal channel selected is available on the ADC instance selected.
  */
#define __LL_ADC_IS_CHANNEL_INTERNAL_AVAILABLE(__ADC_INSTANCE__, __CHANNEL__)                                      \
  ((((__ADC_INSTANCE__) == ADC1)                                                                                   \
    &&(((__CHANNEL__ & ADC_CHANNEL_INTERNAL_ADC1) == ADC_CHANNEL_INTERNAL_ADC1))                                   \
   )                                                                                                               \
   ||                                                                                                              \
   (((__ADC_INSTANCE__) == ADC2)                                                                                   \
    &&(((__CHANNEL__ & ADC_CHANNEL_INTERNAL_ADC2) == ADC_CHANNEL_INTERNAL_ADC2))                                   \
   )                                                                                                               \
  )

/**
  * @brief  Helper macro to determine the selected channel corresponding
  *         negative input on the ADC instance selected.
  * @param  __ADC_INSTANCE__ ADC instance
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)(3)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)(3)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)(3)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  *         (3) For ADC channel read back from ADC register,
  *             comparison with internal channel parameter to be done
  *             using helper macro @ref __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL().
  */
#define __LL_ADC_CHANNEL_DIFF_NEG_INPUT(__ADC_INSTANCE__, __CHANNEL__)                                                 \
  __LL_ADC_DECIMAL_NB_TO_CHANNEL(ADC_CHANNEL_DIFF_LUT[__ADC_INSTANCE_INDEX(__ADC_INSTANCE__)][(uint8_t)(__CHANNEL__)]) \

/**
  * @brief  Helper macro to define ADC analog watchdog parameter:
  *         define a single channel to monitor with analog watchdog
  *         from sequencer channel and groups definition.
  * @note   To be used with function @ref LL_ADC_SetAnalogWDMonitChannels().
  *         Example:
  *           LL_ADC_SetAnalogWDMonitChannels(
  *             ADC1, LL_ADC_AWD1,
  *             __LL_ADC_ANALOGWD_CHANNEL_GROUP(LL_ADC_CHANNEL4, LL_ADC_GROUP_REGULAR))
  * @param  __CHANNEL__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)(3)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)(3)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)(3)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  *         (3) For ADC channel read back from ADC register,
  *             comparison with internal channel parameter to be done
  *             using helper macro @ref __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL().
  * @param  __GROUP__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_GROUP_REGULAR
  *         @arg @ref LL_ADC_GROUP_INJECTED
  *         @arg @ref LL_ADC_GROUP_REGULAR_INJECTED
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_AWD_DISABLE
  *         @arg @ref LL_ADC_AWD_ALL_CHANNELS_REG        (0)
  *         @arg @ref LL_ADC_AWD_ALL_CHANNELS_INJ        (0)
  *         @arg @ref LL_ADC_AWD_ALL_CHANNELS_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_0_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_0_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_0_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_1_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_1_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_1_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_2_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_2_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_2_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_3_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_3_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_3_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_4_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_4_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_4_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_5_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_5_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_5_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_6_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_6_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_6_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_7_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_7_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_7_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_8_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_8_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_8_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_9_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_9_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_9_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_10_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_10_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_10_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_11_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_11_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_11_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_12_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_12_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_12_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_13_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_13_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_13_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_14_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_14_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_14_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_15_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_15_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_15_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_16_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_16_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_16_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_17_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_17_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_17_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_18_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_18_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_18_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_19_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_19_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_19_REG_INJ
  *         @arg @ref LL_ADC_AWD_CH_VREFINT_REG          (0)
  *         @arg @ref LL_ADC_AWD_CH_VREFINT_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CH_VREFINT_REG_INJ
  *         @arg @ref LL_ADC_AWD_CH_VBAT_REG             (0)(1)
  *         @arg @ref LL_ADC_AWD_CH_VBAT_INJ             (0)(1)
  *         @arg @ref LL_ADC_AWD_CH_VBAT_REG_INJ            (1)
  *         @arg @ref LL_ADC_AWD_CH_VDDCORE_REG          (0)(1)
  *         @arg @ref LL_ADC_AWD_CH_VDDCORE_INJ          (0)(1)
  *         @arg @ref LL_ADC_AWD_CH_VDDCORE_REG_INJ      (1)
  *
  *         (0) On this STM32 series, parameter available only on analog watchdog instance: AWD1.\n
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC2, ADC3.
  */
#define __LL_ADC_ANALOGWD_CHANNEL_GROUP(__CHANNEL__, __GROUP__)                                        \
  (((__GROUP__) == LL_ADC_GROUP_REGULAR)                                                               \
   ? ((((__CHANNEL__) & ADC_CHANNEL_NUMBER_MASK) << ADC_AWD_CHANNEL_NUMBER_BITOFFSET_POS)              \
      | (ADC_CHANNEL_LUT[(uint8_t)(__CHANNEL__)] & ADC_CHANNEL_ID_BITFIELD_MASK)                       \
      | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)                                                          \
   :                                                                                                   \
   ((__GROUP__) == LL_ADC_GROUP_INJECTED)                                                              \
   ? ((((__CHANNEL__) & ADC_CHANNEL_NUMBER_MASK) << ADC_AWD_CHANNEL_NUMBER_BITOFFSET_POS)              \
      | (ADC_CHANNEL_LUT[(uint8_t)(__CHANNEL__)] & ADC_CHANNEL_ID_BITFIELD_MASK)                       \
      | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1SGL)                                                         \
   :                                                                                                   \
   ((((__CHANNEL__) & ADC_CHANNEL_NUMBER_MASK) << ADC_AWD_CHANNEL_NUMBER_BITOFFSET_POS)                \
    | (ADC_CHANNEL_LUT[(uint8_t)(__CHANNEL__)] & ADC_CHANNEL_ID_BITFIELD_MASK)                         \
    | ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)                                        \
  )

/**
  * @brief  Helper macro to set the value of ADC analog watchdog threshold high
  *         or low in function of ADC resolution when ADC resolution is different of 12 bit.
  * @note   In case of ADC resolution different of 12 bits, this macro performs the required data formatting:
  *         - analog watchdog thresholds data aligned to left side (bit 11).
  *         - bits out of resolution range (LSB) set to value "0".
  * @note   To be used with function @ref LL_ADC_SetAnalogWDThresholds().
  *         Example, with a ADC resolution of 8 bits, to set the value of
  *         analog watchdog threshold high (on 8 bits):
  *           LL_ADC_SetAnalogWDThresholds
  *            (< ADCx param >,
  *             __LL_ADC_ANALOGWD_SET_THRESHOLD_RESOLUTION(LL_ADC_RESOLUTION_8B, <threshold_data_8_bits>)
  *            );
  * @param  __ADC_RESOLUTION__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_RESOLUTION_12B
  *         @arg @ref LL_ADC_RESOLUTION_10B
  *         @arg @ref LL_ADC_RESOLUTION_8B
  *         @arg @ref LL_ADC_RESOLUTION_6B
  * @param  __AWD_THRESHOLD_DATA__ Value between Min_Data=0x000 and Max_Data=0xFFF
  *        (or tigher range for data corresponding to lower ADC resolution)
  * @retval Value between Min_Data=0x000 and Max_Data=0xFFF
  */
#define __LL_ADC_ANALOGWD_SET_THRESHOLD_RESOLUTION(__ADC_RESOLUTION__, __AWD_THRESHOLD_DATA__) \
  ((__AWD_THRESHOLD_DATA__) << ((__ADC_RESOLUTION__) >> (ADC_CFGR1_RES_Pos - 1U )))

/**
  * @brief  Helper macro to get the value of ADC analog watchdog threshold high
  *         or low in function of ADC resolution, when ADC resolution is
  *         different of 12 bits.
  * @note   To be used with function @ref LL_ADC_GetAnalogWDThresholds().
  *         Example, with a ADC resolution of 8 bits, to get the value of
  *         analog watchdog threshold high (on 8 bits):
  *           < threshold_value_6_bits > = __LL_ADC_ANALOGWD_GET_THRESHOLD_RESOLUTION
  *            (LL_ADC_RESOLUTION_8B,
  *             LL_ADC_GetAnalogWDThresholds(<ADCx param>, LL_ADC_AWD_THRESHOLD_HIGH)
  *            );
  * @param  __ADC_RESOLUTION__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_RESOLUTION_12B
  *         @arg @ref LL_ADC_RESOLUTION_10B
  *         @arg @ref LL_ADC_RESOLUTION_8B
  *         @arg @ref LL_ADC_RESOLUTION_6B
  * @param  __AWD_THRESHOLD_12_BITS__ Value between Min_Data=0x000 and Max_Data=0xFFF
  * @retval Value between Min_Data=0x000 and Max_Data=0xFFF
  */
#define __LL_ADC_ANALOGWD_GET_THRESHOLD_RESOLUTION(__ADC_RESOLUTION__, __AWD_THRESHOLD_12_BITS__) \
  ((__AWD_THRESHOLD_12_BITS__) >> ((__ADC_RESOLUTION__) >> (ADC_CFGR1_RES_Pos - 1U )))

/**
  * @brief  Helper macro to set the ADC calibration value with both single ended
  *         and differential modes calibration factors concatenated.
  * @note   To be used with function @ref LL_ADC_SetCalibrationFactor().
  *         Example, to set calibration factors single ended to 0x55
  *         and differential ended to 0x2A:
  *           LL_ADC_SetCalibrationFactor(
  *             ADC1,
  *             __LL_ADC_CALIB_FACTOR_SINGLE_DIFF(0x55, 0x2A))
  * @param  __CALIB_FACTOR_SINGLE_ENDED__ Value between Min_Data=0x00 and Max_Data=0x1FF
  * @param  __CALIB_FACTOR_DIFFERENTIAL__ Value between Min_Data=0x00 and Max_Data=0x1FF
  * @retval Value between Min_Data=0x00000000 and Max_Data=0xFFFFFFFF
  */
#define __LL_ADC_CALIB_FACTOR_SINGLE_DIFF(__CALIB_FACTOR_SINGLE_ENDED__, __CALIB_FACTOR_DIFFERENTIAL__)        \
  (((__CALIB_FACTOR_DIFFERENTIAL__) << ADC_CALFACT_CALFACT_D_Pos) | (__CALIB_FACTOR_SINGLE_ENDED__))

#if defined(ADC_MULTIMODE_SUPPORT)
/**
  * @brief  Helper macro to get the ADC multimode conversion data of ADC master
  *         or ADC slave from raw value with both ADC conversion data concatenated.
  * @note   This macro is intended to be used when multimode transfer by DMA
  *         is enabled: refer to function @ref LL_ADC_SetMultiDataFormat().
  *         In this case the transferred data need to processed with this macro
  *         to separate the conversion data of ADC master and ADC slave.
  * @param  __ADC_MULTI_MASTER_SLAVE__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_MULTI_MASTER
  *         @arg @ref LL_ADC_MULTI_SLAVE
  * @param  __ADC_MULTI_CONV_DATA__ Value between Min_Data=0x000 and Max_Data=0xFFF
  * @retval Value between Min_Data=0x000 and Max_Data=0xFFF
  */
#define __LL_ADC_MULTI_CONV_DATA_MASTER_SLAVE(__ADC_MULTI_MASTER_SLAVE__, __ADC_MULTI_CONV_DATA__) \
  (((__ADC_MULTI_CONV_DATA__) >> ((ADC_CDR_RDATA_SLV_Pos) & ~(__ADC_MULTI_MASTER_SLAVE__))) & ADC_CDR_RDATA_MST)
#endif /* ADC_MULTIMODE_SUPPORT */

#if defined(ADC_MULTIMODE_SUPPORT)
/**
  * @brief  Helper macro to select, from a ADC instance, to which ADC instance
  *         it has a dependence in multimode (ADC master of the corresponding
  *         ADC common instance).
  * @note   In case of device with multimode available and a mix of
  *         ADC instances compliant and not compliant with multimode feature,
  *         ADC instances not compliant with multimode feature are
  *         considered as master instances (do not depend to
  *         any other ADC instance).
  * @param  __ADCx__ ADC instance
  * @retval __ADCx__ ADC instance master of the corresponding ADC common instance
  */
#define __LL_ADC_MULTI_INSTANCE_MASTER(__ADCx__) \
  ( ( ((__ADCx__) == ADC2)                                                     \
    )?                                                                         \
    (ADC1)                                                                     \
    :                                                                          \
    (__ADCx__)                                                                 \
  )
#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @brief  Helper macro to select the ADC common instance
  *         to which is belonging the selected ADC instance.
  * @note   ADC common register instance can be used for:
  *         - Set parameters common to several ADC instances
  *         - Multimode (for devices with several ADC instances)
  *         Refer to functions having argument "ADCxy_COMMON" as parameter.
  * @param  __ADCx__ ADC instance
  * @retval ADC common register instance
  */
#define __LL_ADC_COMMON_INSTANCE(__ADCx__)    (ADC12_COMMON)

/**
  * @brief  Helper macro to check if all ADC instances sharing the same
  *         ADC common instance are disabled.
  * @note   This check is required by functions with setting conditioned to
  *         ADC state:
  *         All ADC instances of the ADC common group must be disabled.
  *         Refer to functions having argument "ADCxy_COMMON" as parameter.
  * @note   On devices with only 1 ADC common instance, parameter of this macro
  *         is useless and can be ignored (parameter kept for compatibility
  *         with devices featuring several ADC common instances).
  * @param  __ADCXY_COMMON__ ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval Value "0" if all ADC instances sharing the same ADC common instance
  *         are disabled.
  *         Value "1" if at least one ADC instance sharing the same ADC common instance
  *         is enabled.
  */
#define __LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(__ADCXY_COMMON__)              \
  (LL_ADC_IsEnabled(ADC1) | LL_ADC_IsEnabled(ADC2))
/**
  * @brief  Helper macro to define the ADC conversion data full-scale digital
  *         value corresponding to the selected ADC resolution.
  * @note   ADC conversion data full-scale corresponds to voltage range
  *         determined by analog voltage references Vref+ and Vref-
  *         (refer to reference manual).
  * @param  __ADC_RESOLUTION__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_RESOLUTION_12B
  *         @arg @ref LL_ADC_RESOLUTION_10B
  *         @arg @ref LL_ADC_RESOLUTION_8B
  *         @arg @ref LL_ADC_RESOLUTION_6B
  * @retval ADC conversion data full-scale digital value (unit: digital value of ADC conversion data)
  */
#define __LL_ADC_DIGITAL_SCALE(__ADC_RESOLUTION__)                             \
  (0xFFFUL >> ((__ADC_RESOLUTION__) >> (ADC_CFGR1_RES_Pos - 1UL)))

/**
  * @brief  Helper macro to convert the ADC conversion data from
  *         a resolution to another resolution.
  * @param  __DATA__ ADC conversion data to be converted
  * @param  __ADC_RESOLUTION_CURRENT__ Resolution of the data to be converted
  *         This parameter can be one of the following values:
  *         @arg @ref LL_ADC_RESOLUTION_12B
  *         @arg @ref LL_ADC_RESOLUTION_10B
  *         @arg @ref LL_ADC_RESOLUTION_8B
  *         @arg @ref LL_ADC_RESOLUTION_6B
  * @param  __ADC_RESOLUTION_TARGET__ Resolution of the data after conversion
  *         This parameter can be one of the following values:
  *         @arg @ref LL_ADC_RESOLUTION_12B
  *         @arg @ref LL_ADC_RESOLUTION_10B
  *         @arg @ref LL_ADC_RESOLUTION_8B
  *         @arg @ref LL_ADC_RESOLUTION_6B
  * @retval ADC conversion data to the requested resolution
  */
#define __LL_ADC_CONVERT_DATA_RESOLUTION(__DATA__,\
                                         __ADC_RESOLUTION_CURRENT__,\
                                         __ADC_RESOLUTION_TARGET__)          \
(((__DATA__)                                                                 \
  << ((__ADC_RESOLUTION_CURRENT__) >> (ADC_CFGR1_RES_Pos - 1UL)))   \
 >> ((__ADC_RESOLUTION_TARGET__) >> (ADC_CFGR1_RES_Pos - 1UL))      \
)

/**
  * @brief  Helper macro to calculate the voltage (unit: mVolt)
  *         corresponding to a ADC conversion data (unit: digital value).
  * @note   Analog reference voltage (Vref+) must be either known from
  *         user board environment.
  * @param  __VREFANALOG_VOLTAGE__ Analog reference voltage (unit: mV)
  * @param  __ADC_DATA__ ADC conversion data (resolution 12 bits)
  *                       (unit: digital value).
  * @param  __ADC_RESOLUTION__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_RESOLUTION_12B
  *         @arg @ref LL_ADC_RESOLUTION_10B
  *         @arg @ref LL_ADC_RESOLUTION_8B
  *         @arg @ref LL_ADC_RESOLUTION_6B
  * @retval ADC conversion data equivalent voltage value (unit: mVolt)
  */
#define __LL_ADC_CALC_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,\
                                      __ADC_DATA__,\
                                      __ADC_RESOLUTION__)                    \
((__ADC_DATA__) * (int32_t)(__VREFANALOG_VOLTAGE__)                          \
 / (int32_t)(__LL_ADC_DIGITAL_SCALE(__ADC_RESOLUTION__))                     \
)

/**
  * @brief  Helper macro to calculate the voltage (unit: mVolt)
  *         corresponding to a ADC conversion data (unit: digital value) in
  *         differential ended mode.
  * @note   ADC data from ADC data register is unsigned and centered around
  *         middle code in. Converted voltage can be positive or negative
  *         depending on differential input voltages.
  * @note   Analog reference voltage (Vref+) must be either known from
  *         user board environment.
  * @param  __VREFANALOG_VOLTAGE__ Analog reference voltage (unit: mV)
  * @param  __ADC_DATA__ ADC conversion data (unit: digital value).
  * @param  __ADC_RESOLUTION__ This parameter can be one of the following values:
  *         @arg @ref LL_ADC_RESOLUTION_12B
  *         @arg @ref LL_ADC_RESOLUTION_10B
  *         @arg @ref LL_ADC_RESOLUTION_8B
  *         @arg @ref LL_ADC_RESOLUTION_6B
  * @retval ADC conversion data equivalent voltage value (unit: mVolt)
  */
#define __LL_ADC_CALC_DIFF_DATA_TO_VOLTAGE(__VREFANALOG_VOLTAGE__,\
                                           __ADC_DATA__,\
                                           __ADC_RESOLUTION__)\
((int32_t)((__ADC_DATA__) << 1U) * (int32_t)(__VREFANALOG_VOLTAGE__)\
 / (int32_t)(__LL_ADC_DIGITAL_SCALE(__ADC_RESOLUTION__))\
 - (int32_t)(__VREFANALOG_VOLTAGE__))


/**
  * @}
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup ADC_LL_Exported_Functions ADC Exported Functions
  * @{
  */

/** @defgroup ADC_LL_EF_DMA_Management ADC DMA management
  * @{
  */
/* Note: LL ADC functions to set DMA transfer are located into sections of    */
/*       configuration of ADC instance, groups and multimode (if available):  */
/*       @ref LL_ADC_REG_SetDMATransfer(), ...                                */

/**
  * @brief  Function to help to configure DMA transfer from ADC: retrieve the
  *         ADC register address from ADC instance and a list of ADC registers
  *         intended to be used (most commonly) with DMA transfer.
  * @note   These ADC registers are data registers:
  *         when ADC conversion data is available in ADC data registers,
  *         ADC generates a DMA transfer request.
  * @note   This macro is intended to be used with LL DMA driver, refer to
  *         function "LL_DMA_ConfigAddresses()".
  *         Example:
  *           LL_DMA_ConfigAddresses(DMA1,
  *                                  LL_DMA_CHANNEL_1,
  *                                  LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
  *                                  (uint32_t)&< array or variable >,
  *                                  LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  * @note   For devices with several ADC: in multimode, some devices
  *         use a different data register outside of ADC instance scope
  *         (common data register). This macro manages this register difference,
  *         only ADC instance has to be set as parameter.
  * @rmtoll DR       RDATA          LL_ADC_DMA_GetRegAddr\n
  *         CDR      RDATA_MST      LL_ADC_DMA_GetRegAddr\n
  *         CDR      RDATA_SLV      LL_ADC_DMA_GetRegAddr
  * @param  ADCx ADC instance
  * @param  Register This parameter can be one of the following values:
  *         @arg @ref LL_ADC_DMA_REG_REGULAR_DATA
  *         @arg @ref LL_ADC_DMA_REG_REGULAR_MULTI_NO_PACKING (1)
  *         @arg @ref LL_ADC_DMA_REG_REGULAR_MULTI_PACKING (1)
  *
  *         (1) Available on devices with several ADC instances.
  * @retval ADC register address
  */
#if defined(ADC_MULTIMODE_SUPPORT)
__STATIC_INLINE uint32_t LL_ADC_DMA_GetRegAddr(const ADC_TypeDef *ADCx, uint32_t Register)
{
  uint32_t data_reg_addr;

  if (Register == LL_ADC_DMA_REG_REGULAR_DATA)
  {
    /* Retrieve address of register DR */
    data_reg_addr = (uint32_t)(&(ADCx->DR));
  }
  else /* (Register == LL_ADC_DMA_REG_REGULAR_MULTI_NO_PACKING || LL_ADC_DMA_REG_REGULAR_MULTI_PACKING) */
  {
    /* Retrieve address of register CDR */
    data_reg_addr = (uint32_t)(&((__LL_ADC_COMMON_INSTANCE(ADCx))->CDR));
  }

  return data_reg_addr;
}
#else
__STATIC_INLINE uint32_t LL_ADC_DMA_GetRegAddr(const ADC_TypeDef *ADCx, uint32_t Register)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(Register);

  /* Retrieve address of register DR */
  return (uint32_t)(&(ADCx->DR));
}
#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @}
  */

/** @defgroup ADC_LL_EF_Configuration_ADC_Common Configuration of ADC hierarchical scope: common to several
  *           ADC instances
  * @{
  */

/**
  * @brief  Set parameter common to several ADC: measurement path to
  *         internal channels (VrefInt, temperature sensor, ...).
  *         Configure all paths (overwrite current configuration).
  * @note   One or several values can be selected.
  *         Example: (LL_ADC_PATH_INTERNAL_VREFINT |
  *                   LL_ADC_PATH_INTERNAL_TEMPSENSOR)
  *         The values not selected are removed from configuration.
  * @note   Stabilization time of measurement path to internal channel:
  *         After enabling internal paths, before starting ADC conversion,
  *         a delay is required for internal voltage reference and
  *         temperature sensor stabilization time.
  *         Refer to device datasheet.
  *         Refer to literal @ref LL_ADC_DELAY_VREFINT_STAB_US.
  * @note   ADC internal channel sampling time constraint:
  *         For ADC conversion of internal channels,
  *         a sampling time minimum value is required.
  *         Refer to device datasheet.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         All ADC instances of the ADC common group must be disabled.
  *         This check can be done with function @ref LL_ADC_IsEnabled() for each
  *         ADC instance or by using helper macro helper macro
  *         @ref __LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE().
  * @rmtoll CCR      VREFEN         LL_ADC_SetCommonPathInternalCh\n
  *         CCR      VBATEN         LL_ADC_SetCommonPathInternalCh
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @param  PathInternal This parameter can be a combination of the following values:
  *         @arg @ref LL_ADC_PATH_INTERNAL_NONE
  *         @arg @ref LL_ADC_PATH_INTERNAL_VREFINT
  *         @arg @ref LL_ADC_PATH_INTERNAL_VBAT
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
{
  MODIFY_REG(ADCxy_COMMON->CCR, ADC_CCR_VREFEN | ADC_CCR_VBATEN, PathInternal);
}

/**
  * @brief  Set parameter common to several ADC: measurement path to
  *         internal channels (VrefInt, temperature sensor, ...).
  *         Add paths to the current configuration.
  * @note   One or several values can be selected.
  *         Example: (LL_ADC_PATH_INTERNAL_VREFINT |
  *                   LL_ADC_PATH_INTERNAL_TEMPSENSOR)
  * @note   Stabilization time of measurement path to internal channel:
  *         After enabling internal paths, before starting ADC conversion,
  *         a delay is required for internal voltage reference and
  *         temperature sensor stabilization time.
  *         Refer to device datasheet.
  *         Refer to literal @ref LL_ADC_DELAY_VREFINT_STAB_US.
  * @note   ADC internal channel sampling time constraint:
  *         For ADC conversion of internal channels,
  *         a sampling time minimum value is required.
  *         Refer to device datasheet.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         All ADC instances of the ADC common group must be disabled.
  *         This check can be done with function @ref LL_ADC_IsEnabled() for each
  *         ADC instance or by using helper macro helper macro
  *         @ref __LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE().
  * @rmtoll CCR      VREFEN         LL_ADC_SetCommonPathInternalChAdd\n
  *         CCR      VBATEN         LL_ADC_SetCommonPathInternalChAdd
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @param  PathInternal This parameter can be a combination of the following values:
  *         @arg @ref LL_ADC_PATH_INTERNAL_NONE
  *         @arg @ref LL_ADC_PATH_INTERNAL_VREFINT
  *         @arg @ref LL_ADC_PATH_INTERNAL_VBAT
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetCommonPathInternalChAdd(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
{
  SET_BIT(ADCxy_COMMON->CCR, PathInternal);
}

/**
  * @brief  Set parameter common to several ADC: measurement path to
  *         internal channels (VrefInt, temperature sensor, ...).
  *         Remove paths to the current configuration.
  * @note   One or several values can be selected.
  *         Example: (LL_ADC_PATH_INTERNAL_VREFINT |
  *                   LL_ADC_PATH_INTERNAL_TEMPSENSOR)
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         All ADC instances of the ADC common group must be disabled.
  *         This check can be done with function @ref LL_ADC_IsEnabled() for each
  *         ADC instance or by using helper macro helper macro
  *         @ref __LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE().
  * @rmtoll CCR      VREFEN         LL_ADC_SetCommonPathInternalChRem\n
  *         CCR      VBATEN         LL_ADC_SetCommonPathInternalChRem
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @param  PathInternal This parameter can be a combination of the following values:
  *         @arg @ref LL_ADC_PATH_INTERNAL_NONE
  *         @arg @ref LL_ADC_PATH_INTERNAL_VREFINT
  *         @arg @ref LL_ADC_PATH_INTERNAL_VBAT
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetCommonPathInternalChRem(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
{
  CLEAR_BIT(ADCxy_COMMON->CCR, PathInternal);
}

/**
  * @brief  Get parameter common to several ADC: measurement path to internal
  *         channels (VrefInt, temperature sensor, ...).
  * @note   One or several values can be selected.
  *         Example: (LL_ADC_PATH_INTERNAL_VREFINT |
  *                   LL_ADC_PATH_INTERNAL_TEMPSENSOR)
  * @rmtoll CCR      VREFEN         LL_ADC_GetCommonPathInternalCh\n
  *         CCR      VBATEN         LL_ADC_GetCommonPathInternalCh
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval Returned value can be a combination of the following values:
  *         @arg @ref LL_ADC_PATH_INTERNAL_NONE
  *         @arg @ref LL_ADC_PATH_INTERNAL_VREFINT
  *         @arg @ref LL_ADC_PATH_INTERNAL_VBAT
  */
__STATIC_INLINE uint32_t LL_ADC_GetCommonPathInternalCh(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(READ_BIT(ADCxy_COMMON->CCR, ADC_CCR_VREFEN | ADC_CCR_VBATEN));
}

/**
  * @}
  */

/** @defgroup ADC_LL_EF_Configuration_ADC_Instance Configuration of ADC hierarchical scope: ADC instance
  * @{
  */

/**
  * @brief  Set parameter on ADC instance scope: measurement path to
  *         internal channels (Vcore, Vcpu...).
  *         Configure all paths (overwrite current configuration).
  * @note   One or several values can be selected.
  *         Example: (LL_ADC_PATH_INTERNAL_VDDCORE |
  *                   LL_ADC_PATH_INTERNAL_...)
  *         The values not selected are removed from configuration.
  * @note   Stabilization time of measurement path to internal channel:
  *         After enabling internal paths, before starting ADC conversion,
  *         a delay may be required required for analog stabilization.
  *         Refer to device datasheet.
  * @note   ADC internal channel sampling time constraint:
  *         For ADC conversion of internal channels,
  *         a sampling time minimum value is required.
  *         Refer to device datasheet.
  * @rmtoll OR       OP2            LL_ADC_SetPathInternalCh
  * @param  ADCx ADC instance
  * @param  PathInternal This parameter can be a combination of the following values:
  *         @arg @ref LL_ADC_PATH_INTERNAL_NONE
  *         @arg @ref LL_ADC_PATH_INTERNAL_VDDCORE
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetPathInternalCh(ADC_TypeDef *ADCx, uint32_t PathInternal)
{
  MODIFY_REG(ADCx->OR, ADC_OR_OP2, (PathInternal >> ADC_PATH_INTERNAL_POS));
}

/**
  * @brief  Set parameter on ADC instance scope: measurement path to
  *         internal channels (Vcore, Vcpu...).
  *         Add paths to the current configuration.
  * @note   One or several values can be selected.
  *         Example: (LL_ADC_PATH_INTERNAL_VDDCORE |
  *                   LL_ADC_PATH_INTERNAL_...)
  *         The values not selected are removed from configuration.
  * @note   Stabilization time of measurement path to internal channel:
  *         After enabling internal paths, before starting ADC conversion,
  *         a delay may be required required for analog stabilization.
  *         Refer to device datasheet.
  * @note   ADC internal channel sampling time constraint:
  *         For ADC conversion of internal channels,
  *         a sampling time minimum value is required.
  *         Refer to device datasheet.
  * @rmtoll OR       OP2            LL_ADC_SetPathInternalChAdd
  * @param  ADCx ADC instance
  * @param  PathInternal This parameter can be a combination of the following values:
  *         @arg @ref LL_ADC_PATH_INTERNAL_NONE
  *         @arg @ref LL_ADC_PATH_INTERNAL_VDDCORE
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetPathInternalChAdd(ADC_TypeDef *ADCx, uint32_t PathInternal)
{
  SET_BIT(ADCx->OR, (PathInternal >> ADC_PATH_INTERNAL_POS));
}

/**
  * @brief  Set parameter on ADC instance scope: measurement path to
  *         internal channels (Vcore, Vcpu...).
  *         Remove paths to the current configuration.
  * @note   One or several values can be selected.
  *         Example: (LL_ADC_PATH_INTERNAL_VDDCORE |
  *                   LL_ADC_PATH_INTERNAL_...)
  *         The values not selected are removed from configuration.
  * @note   Stabilization time of measurement path to internal channel:
  *         After enabling internal paths, before starting ADC conversion,
  *         a delay may be required required for analog stabilization.
  *         Refer to device datasheet.
  * @note   ADC internal channel sampling time constraint:
  *         For ADC conversion of internal channels,
  *         a sampling time minimum value is required.
  *         Refer to device datasheet.
  * @rmtoll OR       OP2            LL_ADC_SetPathInternalChRem
  * @param  ADCx ADC instance
  * @param  PathInternal This parameter can be a combination of the following values:
  *         @arg @ref LL_ADC_PATH_INTERNAL_NONE
  *         @arg @ref LL_ADC_PATH_INTERNAL_VDDCORE
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetPathInternalChRem(ADC_TypeDef *ADCx, uint32_t PathInternal)
{
  CLEAR_BIT(ADCx->OR, (PathInternal >> ADC_PATH_INTERNAL_POS));
}

/**
  * @brief  Get parameter on ADC instance scope: measurement path to
  *         internal channels (Vcore, Vcpu...).
  * @note   One or several values can be selected.
  *         Example: (LL_ADC_PATH_INTERNAL_VDDCORE |
  *                   LL_ADC_PATH_INTERNAL_...)
  * @rmtoll OR       OP2            LL_ADC_GetPathInternalCh
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_PATH_INTERNAL_NONE
  *         @arg @ref LL_ADC_PATH_INTERNAL_VDDCORE
  */
__STATIC_INLINE uint32_t LL_ADC_GetPathInternalCh(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->OR, ADC_OR_OP2)) << ADC_PATH_INTERNAL_POS;
}

/**
  * @brief  Set ADC calibration factor in the mode single-ended
  *         or differential (for devices with differential mode available).
  * @note   This function is intended to set calibration parameters
  *         without having to perform a new calibration using
  *         @ref LL_ADC_StartCalibration().
  * @note   In case of setting calibration factors of both modes single ended
  *         and differential (parameter LL_ADC_BOTH_SINGLE_DIFF_ENDED):
  *         both calibration factors must be concatenated.
  *         To perform this processing, use helper macro
  *         @ref __LL_ADC_CALIB_FACTOR_SINGLE_DIFF().
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be enabled, without calibration on going, without conversion
  *         on going on group regular and group injected.
  * @rmtoll CALFACT  CALFACT_S      LL_ADC_SetCalibrationFactor\n
  *         CALFACT  CALFACT_D      LL_ADC_SetCalibrationFactor
  * @param  ADCx ADC instance
  * @param  SingleDiff This parameter can be one of the following values:
  *         @arg @ref LL_ADC_SINGLE_ENDED
  *         @arg @ref LL_ADC_DIFFERENTIAL_ENDED
  *         @arg @ref LL_ADC_BOTH_SINGLE_DIFF_ENDED
  * @param  CalibrationFactor Value between Min_Data=0x00 and Max_Data=0x7F
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetCalibrationFactor(ADC_TypeDef *ADCx, uint32_t SingleDiff, uint32_t CalibrationFactor)
{
  MODIFY_REG(ADCx->CALFACT,
             SingleDiff & ADC_SINGLEDIFF_CALIB_FACTOR_MASK,
             (CalibrationFactor << (ADC_SINGLEDIFF_CALIB_F_BIT_D_POS & ~(SingleDiff & ADC_CALFACT_CALFACT_S)))
             & SingleDiff & ADC_SINGLEDIFF_CALIB_FACTOR_MASK);
}
/**
  * @brief  Get ADC calibration factor in the mode single-ended
  *         or differential (for devices with differential mode available).
  * @note   Calibration factors are set by hardware after performing
  *         a calibration run using function @ref LL_ADC_StartCalibration().
  * @note   For devices with differential mode available:
  *         Calibration of offset is specific to each of
  *         single-ended and differential modes
  * @rmtoll CALFACT  CALFACT_S      LL_ADC_GetCalibrationFactor\n
  *         CALFACT  CALFACT_D      LL_ADC_GetCalibrationFactor
  * @param  ADCx ADC instance
  * @param  SingleDiff This parameter can be one of the following values:
  *         @arg @ref LL_ADC_SINGLE_ENDED
  *         @arg @ref LL_ADC_DIFFERENTIAL_ENDED
  * @retval Value between Min_Data=0x00 and Max_Data=0x1FF
  */
__STATIC_INLINE uint32_t LL_ADC_GetCalibrationFactor(const ADC_TypeDef *ADCx, uint32_t SingleDiff)
{
  /* Retrieve bits with position in register depending on parameter           */
  /* "SingleDiff".                                                            */
  /* Parameter used with mask "ADC_SINGLEDIFF_CALIB_FACTOR_MASK" because      */
  /* containing other bits reserved for other purpose.                        */
  return (uint32_t)(READ_BIT(ADCx->CALFACT,
                             (SingleDiff & ADC_SINGLEDIFF_CALIB_FACTOR_MASK))
                    >> ((SingleDiff & ADC_SINGLEDIFF_CALIB_F_BIT_D_MASK) >>
                        ADC_SINGLEDIFF_CALIB_F_BIT_D_SHIFT4));
}
/**
  * @brief  Set ADC resolution.
  *         Refer to reference manual for alignments formats
  *         dependencies to ADC resolutions.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll CFGR1    RES            LL_ADC_SetResolution
  * @param  ADCx ADC instance
  * @param  Resolution This parameter can be one of the following values:
  *         @arg @ref LL_ADC_RESOLUTION_12B
  *         @arg @ref LL_ADC_RESOLUTION_10B
  *         @arg @ref LL_ADC_RESOLUTION_8B
  *         @arg @ref LL_ADC_RESOLUTION_6B
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetResolution(ADC_TypeDef *ADCx, uint32_t Resolution)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_RES, Resolution);
}

/**
  * @brief  Get ADC resolution.
  *         Refer to reference manual for alignments formats
  *         dependencies to ADC resolutions.
  * @rmtoll CFGR1    RES            LL_ADC_GetResolution
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_RESOLUTION_12B
  *         @arg @ref LL_ADC_RESOLUTION_10B
  *         @arg @ref LL_ADC_RESOLUTION_8B
  *         @arg @ref LL_ADC_RESOLUTION_6B
  */
__STATIC_INLINE uint32_t LL_ADC_GetResolution(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_RES));
}

/**
  * @brief  Set ADC low power mode.
  * @note   Description of ADC low power modes:
  *         - ADC low power mode "auto wait": Dynamic low power mode,
  *           ADC conversions occurrences are limited to the minimum necessary
  *           in order to reduce power consumption.
  *           New ADC conversion starts only when the previous
  *           unitary conversion data (for ADC group regular)
  *           or previous sequence conversions data (for ADC group injected)
  *           has been retrieved by user software.
  *           In the meantime, ADC remains idle: does not performs any
  *           other conversion.
  *           This mode allows to automatically adapt the ADC conversions
  *           triggers to the speed of the software that reads the data.
  *           Moreover, this avoids risk of overrun for low frequency
  *           applications.
  *           How to use this low power mode:
  *           - It is not recommended to use with interruption or DMA
  *             since these modes have to clear immediately the EOC flag
  *             (by CPU to free the IRQ pending event or by DMA).
  *             Auto wait will work but fort a very short time, discarding
  *             its intended benefit (except specific case of high load of CPU
  *             or DMA transfers which can justify usage of auto wait).
  *           - Do use with polling: 1. Start conversion,
  *             2. Later on, when conversion data is needed: poll for end of
  *             conversion  to ensure that conversion is completed and
  *             retrieve ADC conversion data. This will trig another
  *             ADC conversion start.
  *         - ADC low power mode "auto power-off" (feature available on
  *           this device if parameter LL_ADC_LP_AUTOPOWEROFF is available):
  *           the ADC automatically powers-off after a conversion and
  *           automatically wakes up when a new conversion is triggered
  *           (with startup time between trigger and start of sampling).
  *           This feature can be combined with low power mode "auto wait".
  * @note   With ADC low power mode "auto wait", the ADC conversion data read
  *         is corresponding to previous ADC conversion start, independently
  *         of delay during which ADC was idle.
  *         Therefore, the ADC conversion data may be outdated: does not
  *         correspond to the current voltage level on the selected
  *         ADC channel.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll CFGR1    AUTDLY         LL_ADC_SetLPModeAutoWait
  * @param  ADCx ADC instance
  * @param  LowPowerMode This parameter can be one of the following values:
  *         @arg @ref LL_ADC_LP_AUTOWAIT_DISABLE
  *         @arg @ref LL_ADC_LP_AUTOWAIT_ENABLE
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetLPModeAutoWait(ADC_TypeDef *ADCx, uint32_t LowPowerMode)
{
  MODIFY_REG(ADCx->CFGR1, (ADC_CFGR1_AUTDLY), LowPowerMode);
}

/**
  * @brief  Get ADC low power mode:
  * @note   Description of ADC low power modes:
  *         - ADC low power mode "auto wait": Dynamic low power mode,
  *           ADC conversions occurrences are limited to the minimum necessary
  *           in order to reduce power consumption.
  *           New ADC conversion starts only when the previous
  *           unitary conversion data (for ADC group regular)
  *           or previous sequence conversions data (for ADC group injected)
  *           has been retrieved by user software.
  *           In the meantime, ADC remains idle: does not performs any
  *           other conversion.
  *           This mode allows to automatically adapt the ADC conversions
  *           triggers to the speed of the software that reads the data.
  *           Moreover, this avoids risk of overrun for low frequency
  *           applications.
  *           How to use this low power mode:
  *           - It is not recommended to use with interruption or DMA
  *             since these modes have to clear immediately the EOC flag
  *             (by CPU to free the IRQ pending event or by DMA).
  *             Auto wait will work but fort a very short time, discarding
  *             its intended benefit (except specific case of high load of CPU
  *             or DMA transfers which can justify usage of auto wait).
  *           - Do use with polling: 1. Start conversion,
  *             2. Later on, when conversion data is needed: poll for end of
  *             conversion  to ensure that conversion is completed and
  *             retrieve ADC conversion data. This will trig another
  *             ADC conversion start.
  *         - ADC low power mode "auto power-off" (feature available on
  *           this device if parameter LL_ADC_LP_AUTOPOWEROFF is available):
  *           the ADC automatically powers-off after a conversion and
  *           automatically wakes up when a new conversion is triggered
  *           (with startup time between trigger and start of sampling).
  *           This feature can be combined with low power mode "auto wait".
  * @note   With ADC low power mode "auto wait", the ADC conversion data read
  *         is corresponding to previous ADC conversion start, independently
  *         of delay during which ADC was idle.
  *         Therefore, the ADC conversion data may be outdated: does not
  *         correspond to the current voltage level on the selected
  *         ADC channel.
  * @rmtoll CFGR1    AUTDLY         LL_ADC_GetLPModeAutoWait
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_LP_AUTOWAIT_DISABLE
  *         @arg @ref LL_ADC_LP_AUTOWAIT_ENABLE
  */
__STATIC_INLINE uint32_t LL_ADC_GetLPModeAutoWait(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_AUTDLY));
}

/**
  * @brief  Set for the ADC selected offset number 1, 2, 3 or 4:
  *         Channel to which the offset programmed will be applied
  *         (independently of channel mapped on ADC group regular
  *         or group injected)
  * @note   If a channel is mapped on several offsets numbers, only the offset
  *         with the lowest value is considered for the subtraction.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll OFR1     OFFSET_CH      LL_ADC_SetOffsetChannel\n
  *         OFR2     OFFSET_CH      LL_ADC_SetOffsetChannel\n
  *         OFR3     OFFSET_CH      LL_ADC_SetOffsetChannel\n
  *         OFR4     OFFSET_CH      LL_ADC_SetOffsetChannel
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetOffsetChannel(ADC_TypeDef *ADCx, uint32_t Offsety, uint32_t Channel)
{
  __IO uint32_t *preg_offset_cfg = __ADC_PTR_REG_OFFSET(ADCx->OFCFGR1, Offsety);

  MODIFY_REG(*preg_offset_cfg,
             ADC_OFCFGR1_OFFSET_CH,
             (Channel & ADC_CHANNEL_NUMBER_MASK) << ADC_OFCFGR1_OFFSET_CH_Pos);
}

/**
  * @brief  Get for the ADC selected offset instance 1, 2, 3 or 4:
  *         Channel to which the offset programmed will be applied
  *         (independently of channel mapped on ADC group regular
  *         or group injected)
  * @note   Usage of the returned channel number:
  *         - To reinject this channel into another function LL_ADC_xxx:
  *           the returned channel number is only partly formatted on definition
  *           of literals LL_ADC_CHANNEL_x. Therefore, it has to be compared
  *           with parts of literals LL_ADC_CHANNEL_x or using
  *           helper macro @ref __LL_ADC_CHANNEL_TO_DECIMAL_NB().
  *           Then the selected literal LL_ADC_CHANNEL_x can be used
  *           as parameter for another function.
  *         - To get the channel number in decimal format:
  *           process the returned value with the helper macro
  *           @ref __LL_ADC_CHANNEL_TO_DECIMAL_NB().
  * @rmtoll OFR1     OFFSET_CH      LL_ADC_SetOffsetChannel\n
  *         OFR2     OFFSET_CH      LL_ADC_SetOffsetChannel\n
  *         OFR3     OFFSET_CH      LL_ADC_SetOffsetChannel\n
  *         OFR4     OFFSET_CH      LL_ADC_SetOffsetChannel
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)(3)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)(3)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)(3)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  *         (3) For ADC channel read back from ADC register,
  *             comparison with internal channel parameter to be done
  *             using helper macro @ref __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL().
  */
__STATIC_INLINE uint32_t LL_ADC_GetOffsetChannel(const ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->OFCFGR1, Offsety);

  return (uint32_t) __LL_ADC_DECIMAL_NB_TO_CHANNEL(READ_BIT(*preg, ADC_OFCFGR1_OFFSET_CH) >> ADC_OFCFGR1_OFFSET_CH_Pos);
}

/**
  * @brief  Set for the ADC selected offset number 1, 2, 3 or 4:
  *         Offset level (offset to be subtracted from the raw
  *         converted data).
  * @note   Caution: Offset format is dependent to ADC resolution:
  *         offset has to be left-aligned on bit 11, the LSB (right bits)
  *         are set to 0.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll OFR1     OFFSET1        LL_ADC_SetOffsetLevel\n
  *         OFR2     OFFSET2        LL_ADC_SetOffsetLevel\n
  *         OFR3     OFFSET3        LL_ADC_SetOffsetLevel\n
  *         OFR4     OFFSET4        LL_ADC_SetOffsetLevel
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @param  OffsetLevel Value between Min_Data=0x000 and Max_Data=0x00FFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetOffsetLevel(ADC_TypeDef *ADCx, uint32_t Offsety, uint32_t OffsetLevel)
{
  __IO uint32_t *preg_offset_val = __ADC_PTR_REG_OFFSET(ADCx->OFR1, Offsety);

  MODIFY_REG(*preg_offset_val,
             ADC_OFR1_OFFSET,
             OffsetLevel);
}

/**
  * @brief  Get for the ADC selected offset instance 1, 2, 3 or 4:
  *         Offset level (offset to be subtracted from the raw
  *         converted data).
  * @note   Caution: Offset format is dependent to ADC resolution:
  *         offset has to be left-aligned on bit 11, the LSB (right bits)
  *         are set to 0.
  * @rmtoll OFR1     OFFSET1        LL_ADC_GetOffsetLevel\n
  *         OFR2     OFFSET2        LL_ADC_GetOffsetLevel\n
  *         OFR3     OFFSET3        LL_ADC_GetOffsetLevel\n
  *         OFR4     OFFSET4        LL_ADC_GetOffsetLevel
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @retval Value between Min_Data=0x000 and Max_Data=0x00FFFFFF
  */
__STATIC_INLINE uint32_t LL_ADC_GetOffsetLevel(const ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->OFR1, Offsety);

  return (uint32_t) READ_BIT(*preg, ADC_OFR1_OFFSET);
}

/**
  * @brief  Set for the ADC selected offset instance 1, 2, 3 or 4:
  *         force offset state disable or enable
  *         without modifying offset channel or offset value.
  * @note   This function should be needed only in case of offset to be
  *         enabled-disabled dynamically, and should not be needed in other cases:
  *         function LL_ADC_SetOffset() automatically enables the offset.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll OFCFGR1  OFFSETPOS      LL_ADC_SetOffsetSign\n
  *         OFCFGR2  OFFSETPOS      LL_ADC_SetOffsetSign\n
  *         OFCFGR3  OFFSETPOS      LL_ADC_SetOffsetSign\n
  *         OFCFGR4  OFFSETPOS      LL_ADC_SetOffsetSign
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @param  OffsetSign This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_SIGN_NEGATIVE
  *         @arg @ref LL_ADC_OFFSET_SIGN_POSITIVE
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetOffsetSign(ADC_TypeDef *ADCx, uint32_t Offsety, uint32_t OffsetSign)
{
  __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->OFCFGR1, Offsety);

  MODIFY_REG(*preg,
             ADC_OFCFGR1_POSOFF,
             OffsetSign);
}

/**
  * @brief  Get for the ADC selected offset instance 1, 2, 3 or 4:
  *         offset sign if positive or negative.
  * @rmtoll OFCFGR1  OFFSETPOS      LL_ADC_GetOffsetSign\n
  *         OFCFGR2  OFFSETPOS      LL_ADC_GetOffsetSign\n
  *         OFCFGR3  OFFSETPOS      LL_ADC_GetOffsetSign\n
  *         OFCFGR4  OFFSETPOS      LL_ADC_GetOffsetSign
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_SIGN_NEGATIVE
  *         @arg @ref LL_ADC_OFFSET_SIGN_POSITIVE
  */
__STATIC_INLINE uint32_t LL_ADC_GetOffsetSign(const ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->OFCFGR1, Offsety);

  return (uint32_t) READ_BIT(*preg, ADC_OFCFGR1_POSOFF);
}

/**
  * @brief  Set Signed saturation for the ADC selected offset instance 1, 2, 3 or 4:
  *         signed offset saturation if enabled or disabled.
  * @rmtoll OFCFGR1  SSAT          LL_ADC_SetOffsetSignedSaturation\n
  *         OFCFGR2  SSAT          LL_ADC_SetOffsetSignedSaturation\n
  *         OFCFGR3  SSAT          LL_ADC_SetOffsetSignedSaturation\n
  *         OFCFGR4  SSAT          LL_ADC_SetOffsetSignedSaturation
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @param  OffsetSignedSaturation This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_SIGNED_SAT_ENABLE
  *         @arg @ref LL_ADC_OFFSET_SIGNED_SAT_DISABLE
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetOffsetSignedSaturation(ADC_TypeDef *ADCx, uint32_t Offsety,
                                                      uint32_t OffsetSignedSaturation)
{
  __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->OFCFGR1, Offsety);
  MODIFY_REG(*preg, ADC_OFCFGR1_SSAT, OffsetSignedSaturation);
}

/**
  * @brief  Get Signed saturation for the ADC selected offset number 1, 2, 3 or 4:
  *         signed offset saturation if enabled or disabled.
  * @rmtoll OFCFGR1  SSAT          LL_ADC_GetOffsetSignedSaturation\n
  *         OFCFGR2  SSAT          LL_ADC_GetOffsetSignedSaturation\n
  *         OFCFGR3  SSAT          LL_ADC_GetOffsetSignedSaturation\n
  *         OFCFGR4  SSAT          LL_ADC_GetOffsetSignedSaturation
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_SIGNED_SAT_ENABLE
  *         @arg @ref LL_ADC_OFFSET_SIGNED_SAT_DISABLE
  */
__STATIC_INLINE uint32_t LL_ADC_GetOffsetSignedSaturation(const ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->OFCFGR1, Offsety);
  return (uint32_t) READ_BIT(*preg, ADC_OFCFGR1_SSAT);
}

/**
  * @brief  Set Unsigned saturation for the ADC selected offset instance 1, 2, 3 or 4:
  *         signed offset saturation if enabled or disabled.
  * @rmtoll OFCFGR1  USAT          LL_ADC_SetOffsetUnsignedSaturation\n
  *         OFCFGR2  USAT          LL_ADC_SetOffsetUnsignedSaturation\n
  *         OFCFGR3  USAT          LL_ADC_SetOffsetUnsignedSaturation\n
  *         OFCFGR4  USAT          LL_ADC_SetOffsetUnsignedSaturation
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @param  OffsetUnsignedSaturation This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_UNSIGNED_SAT_ENABLE
  *         @arg @ref LL_ADC_OFFSET_UNSIGNED_SAT_DISABLE
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetOffsetUnsignedSaturation(ADC_TypeDef *ADCx, uint32_t Offsety,
                                                        uint32_t OffsetUnsignedSaturation)
{
  __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->OFCFGR1, Offsety);
  MODIFY_REG(*preg, ADC_OFCFGR1_USAT, OffsetUnsignedSaturation);
}

/**
  * @brief  Get Unsigned saturation for the ADC selected offset instance 1, 2, 3 or 4:
  *         signed offset saturation if enabled or disabled.
  * @rmtoll OFCFGR1  USAT          LL_ADC_GetOffsetUnsignedSaturation\n
  *         OFCFGR2  USAT          LL_ADC_GetOffsetUnsignedSaturation\n
  *         OFCFGR3  USAT          LL_ADC_GetOffsetUnsignedSaturation\n
  *         OFCFGR4  USAT          LL_ADC_GetOffsetUnsignedSaturation
  * @param  ADCx ADC instance
  * @param  Offsety This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_1
  *         @arg @ref LL_ADC_OFFSET_2
  *         @arg @ref LL_ADC_OFFSET_3
  *         @arg @ref LL_ADC_OFFSET_4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_OFFSET_UNSIGNED_SAT_ENABLE
  *         @arg @ref LL_ADC_OFFSET_UNSIGNED_SAT_DISABLE
  */
__STATIC_INLINE uint32_t LL_ADC_GetOffsetUnsignedSaturation(const ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->OFCFGR1, Offsety);
  return (uint32_t) READ_BIT(*preg, ADC_OFCFGR1_USAT);
}

/**
  * @brief  Set ADC gain compensation.
  * @note   This function set the gain compensation coefficient
  *         that is applied to raw converted data using the formula:
  *           DATA = DATA(raw) * (gain compensation coef) / 4096
  * @note   This function enables the gain compensation if given
  *         coefficient is above 0, otherwise it disables it.
  * @note   Gain compensation when enabled is applied to all channels.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll GCOMP    GCOMPCOEFF     LL_ADC_SetGainCompensation\n
  *         CFGR2    GCOMP          LL_ADC_SetGainCompensation
  * @param  ADCx ADC instance
  * @param  GainCompensation This parameter can be:
  *         0           Gain compensation will be disabled and value set to 0
  *         1 -> 16393  Gain compensation will be enabled with specified value
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetGainCompensation(ADC_TypeDef *ADCx, uint32_t GainCompensation)
{
  MODIFY_REG(ADCx->GCOMP, ADC_GCOMP_GCOMPCOEFF, GainCompensation);
  MODIFY_REG(ADCx->GCOMP, ADC_GCOMP_GCOMP, ((GainCompensation == 0UL) ? 0UL : 1UL) << ADC_GCOMP_GCOMP_Pos);
}

/**
  * @brief  Get the ADC gain compensation value
  * @rmtoll GCOMP    GCOMPCOEFF     LL_ADC_GetGainCompensation\n
  *         CFGR2    GCOMP          LL_ADC_GetGainCompensation
  * @param  ADCx ADC instance
  * @retval Returned value can be:
  *         0           Gain compensation is disabled
  *         1 -> 16393  Gain compensation is enabled with returned value
  */
__STATIC_INLINE uint32_t LL_ADC_GetGainCompensation(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->GCOMP, ADC_GCOMP_GCOMP) == ADC_GCOMP_GCOMP)   \
          ? READ_BIT(ADCx->GCOMP, ADC_GCOMP_GCOMPCOEFF) : 0UL);
}

/**
  * @}
  */

/** @defgroup ADC_LL_EF_Configuration_ADC_Group_Regular Configuration of ADC hierarchical scope: group regular
  * @{
  */

/**
  * @brief  Set ADC group regular conversion trigger source:
  *         internal (SW start) or from external peripheral (timer event,
  *         external interrupt line).
  * @note   On this STM32 series, setting trigger source to external trigger
  *         also set trigger polarity to rising edge
  *         (default setting for compatibility with some ADC on other
  *         STM32 series having this setting set by HW default value).
  *         In case of need to modify trigger edge, use
  *         function @ref LL_ADC_REG_SetTriggerEdge().
  * @note   Availability of parameters of trigger sources from timer
  *         depends on timers availability on the selected device.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on group regular.
  * @rmtoll CFGR1    EXTSEL         LL_ADC_REG_SetTriggerSource\n
  *         CFGR1    EXTEN          LL_ADC_REG_SetTriggerSource
  * @param  ADCx ADC instance
  * @param  TriggerSource This parameter can be one of the following values:
  *         @arg @ref LL_ADC_REG_TRIG_SOFTWARE
  *         @arg @ref LL_ADC_REG_TRIG_EXT_EXTI_LINE11
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_CH1
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_CH2
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_CH3
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_TRGO2
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM2_CH2
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM2_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM3_CH4
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM3_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM4_CH4
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM4_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM5_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM6_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM7_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM8_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM8_TRGO2
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM9_CH1
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM9_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM12_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM15_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM18_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_LPTIM1_CH1
  *         @arg @ref LL_ADC_REG_TRIG_EXT_LPTIM2_CH1
  *         @arg @ref LL_ADC_REG_TRIG_EXT_LPTIM3_CH1
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_SetTriggerSource(ADC_TypeDef *ADCx, uint32_t TriggerSource)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_EXTEN | ADC_CFGR1_EXTSEL, TriggerSource);
}

/**
  * @brief  Get ADC group regular conversion trigger source:
  *         internal (SW start) or from external peripheral (timer event,
  *         external interrupt line).
  * @note   To determine whether group regular trigger source is
  *         internal (SW start) or external, without detail
  *         of which peripheral is selected as external trigger,
  *         (equivalent to
  *         "if(LL_ADC_REG_GetTriggerSource(ADC1) == LL_ADC_REG_TRIG_SOFTWARE)")
  *         use function @ref LL_ADC_REG_IsTriggerSourceSWStart.
  * @note   Availability of parameters of trigger sources from timer
  *         depends on timers availability on the selected device.
  * @rmtoll CFGR1    EXTSEL         LL_ADC_REG_GetTriggerSource\n
  *         CFGR1    EXTEN          LL_ADC_REG_GetTriggerSource
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_REG_TRIG_SOFTWARE
  *         @arg @ref LL_ADC_REG_TRIG_EXT_EXTI_LINE11
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_CH1
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_CH2
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_CH3
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM1_TRGO2
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM2_CH2
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM2_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM3_CH4
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM3_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM4_CH4
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM4_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM5_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM6_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM7_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM8_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM8_TRGO2
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM9_CH1
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM9_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM12_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM15_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_TIM18_TRGO
  *         @arg @ref LL_ADC_REG_TRIG_EXT_LPTIM1_CH1
  *         @arg @ref LL_ADC_REG_TRIG_EXT_LPTIM2_CH1
  *         @arg @ref LL_ADC_REG_TRIG_EXT_LPTIM3_CH1
  */
__STATIC_INLINE uint32_t LL_ADC_REG_GetTriggerSource(const ADC_TypeDef *ADCx)
{
  __IO uint32_t trigger_source = READ_BIT(ADCx->CFGR1, ADC_CFGR1_EXTSEL | ADC_CFGR1_EXTEN);

  /* Value for shift of {0; 4; 8; 12} depending on value of bitfield          */
  /* corresponding to ADC_CFGR1_EXTEN {0; 1; 2; 3}.                            */
  uint32_t shift_exten = ((trigger_source & ADC_CFGR1_EXTEN) >> (ADC_REG_TRIG_EXTEN_BITOFFSET_POS - 2UL));

  /* Set bitfield corresponding to ADC_CFGR1_EXTEN and ADC_CFGR1_EXTSEL       */
  /* to match with triggers literals definition.                              */
  return ((trigger_source
           & (ADC_REG_TRIG_SOURCE_MASK >> shift_exten) & ADC_CFGR1_EXTSEL)
          | ((ADC_REG_TRIG_EDGE_MASK >> shift_exten) & ADC_CFGR1_EXTEN)
         );
}

/**
  * @brief  Get ADC group regular conversion trigger source internal (SW start)
  *         or external.
  * @note   In case of group regular trigger source set to external trigger,
  *         to determine which peripheral is selected as external trigger,
  *         use function @ref LL_ADC_REG_GetTriggerSource().
  * @rmtoll CFGR1    EXTEN          LL_ADC_REG_IsTriggerSourceSWStart
  * @param  ADCx ADC instance
  * @retval Value "0" if trigger source external trigger
  *         Value "1" if trigger source SW start.
  */
__STATIC_INLINE uint32_t LL_ADC_REG_IsTriggerSourceSWStart(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CFGR1, ADC_CFGR1_EXTEN) == (LL_ADC_REG_TRIG_SOFTWARE & ADC_CFGR1_EXTEN)) ? 1UL : 0UL);
}

/**
  * @brief  Set ADC group regular conversion trigger polarity.
  * @note   Applicable only for trigger source set to external trigger.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on group regular.
  * @rmtoll CFGR1    EXTEN          LL_ADC_REG_SetTriggerEdge
  * @param  ADCx ADC instance
  * @param  ExternalTriggerEdge This parameter can be one of the following values:
  *         @arg @ref LL_ADC_REG_TRIG_EXT_RISING
  *         @arg @ref LL_ADC_REG_TRIG_EXT_FALLING
  *         @arg @ref LL_ADC_REG_TRIG_EXT_RISINGFALLING
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_SetTriggerEdge(ADC_TypeDef *ADCx, uint32_t ExternalTriggerEdge)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_EXTEN, ExternalTriggerEdge);
}

/**
  * @brief  Get ADC group regular conversion trigger polarity.
  * @note   Applicable only for trigger source set to external trigger.
  * @rmtoll CFGR1    EXTEN          LL_ADC_REG_GetTriggerEdge
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_REG_TRIG_EXT_RISING
  *         @arg @ref LL_ADC_REG_TRIG_EXT_FALLING
  *         @arg @ref LL_ADC_REG_TRIG_EXT_RISINGFALLING
  */
__STATIC_INLINE uint32_t LL_ADC_REG_GetTriggerEdge(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_EXTEN));
}

/**
  * @brief  Set ADC sampling mode.
  * @note   This function set the ADC conversion sampling mode
  * @note   This mode applies to regular group only.
  * @note   Set sampling mode is applied to all conversion of regular group.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on group regular.
  * @rmtoll CFGR2    BULB           LL_ADC_REG_SetSamplingMode\n
  *         CFGR2    SMPTRIG        LL_ADC_REG_SetSamplingMode
  * @param  ADCx ADC instance
  * @param  SamplingMode This parameter can be one of the following values:
  *         @arg @ref LL_ADC_REG_SAMPLING_MODE_NORMAL
  *         @arg @ref LL_ADC_REG_SAMPLING_MODE_BULB
  *         @arg @ref LL_ADC_REG_SAMPLING_MODE_TRIGGER_CONTROLED
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_SetSamplingMode(ADC_TypeDef *ADCx, uint32_t SamplingMode)
{
  MODIFY_REG(ADCx->CFGR2, ADC_CFGR2_BULB | ADC_CFGR2_SMPTRIG, SamplingMode);
}

/**
  * @brief  Get the ADC sampling mode
  * @rmtoll CFGR2    BULB           LL_ADC_REG_GetSamplingMode\n
  *         CFGR2    SMPTRIG        LL_ADC_REG_GetSamplingMode
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_REG_SAMPLING_MODE_NORMAL
  *         @arg @ref LL_ADC_REG_SAMPLING_MODE_BULB
  *         @arg @ref LL_ADC_REG_SAMPLING_MODE_TRIGGER_CONTROLED
  */
__STATIC_INLINE uint32_t LL_ADC_REG_GetSamplingMode(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR2, ADC_CFGR2_BULB | ADC_CFGR2_SMPTRIG));
}

/**
  * @brief  Start ADC sampling phase for sampling time trigger mode
  * @note   This function is relevant only when
  *         - @ref LL_ADC_REG_SAMPLING_MODE_TRIGGER_CONTROLED has been set
  *           using @ref LL_ADC_REG_SetSamplingMode
  *         - @ref LL_ADC_REG_TRIG_SOFTWARE is used as trigger source
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be enabled without conversion on going on group regular,
  *         without conversion stop command on going on group regular,
  *         without ADC disable command on going.
  * @rmtoll CFGR2    SWTRIG         LL_ADC_REG_StartSamplingPhase
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_StartSamplingPhase(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->CFGR2, ADC_CFGR2_SWTRIG);
}

/**
  * @brief  Stop ADC sampling phase for sampling time trigger mode and start conversion
  * @note   This function is relevant only when
  *         - @ref LL_ADC_REG_SAMPLING_MODE_TRIGGER_CONTROLED has been set
  *           using @ref LL_ADC_REG_SetSamplingMode
  *         - @ref LL_ADC_REG_TRIG_SOFTWARE is used as trigger source
  *         - @ref LL_ADC_REG_StartSamplingPhase has been called to start
  *           the sampling phase
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be enabled without conversion on going on group regular,
  *         without conversion stop command on going on group regular,
  *         without ADC disable command on going.
  * @rmtoll CFGR2    SWTRIG         LL_ADC_REG_StopSamplingPhase
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_StopSamplingPhase(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->CFGR2, ADC_CFGR2_SWTRIG);
}

/**
  * @brief  Set ADC group regular sequencer length and scan direction.
  * @note   Description of ADC group regular sequencer features:
  *         - For devices with sequencer fully configurable
  *           (function "LL_ADC_REG_SetSequencerRanks()" available):
  *           sequencer length and each rank affectation to a channel
  *           are configurable.
  *           This function performs configuration of:
  *           - Sequence length: Number of ranks in the scan sequence.
  *           - Sequence direction: Unless specified in parameters, sequencer
  *             scan direction is forward (from rank 1 to rank n).
  *           Sequencer ranks are selected using
  *           function "LL_ADC_REG_SetSequencerRanks()".
  *         - For devices with sequencer not fully configurable
  *           (function "LL_ADC_REG_SetSequencerChannels()" available):
  *           sequencer length and each rank affectation to a channel
  *           are defined by channel number.
  *           This function performs configuration of:
  *           - Sequence length: Number of ranks in the scan sequence is
  *             defined by number of channels set in the sequence,
  *             rank of each channel is fixed by channel HW number.
  *             (channel 0 fixed on rank 0, channel 1 fixed on rank1, ...).
  *           - Sequence direction: Unless specified in parameters, sequencer
  *             scan direction is forward (from lowest channel number to
  *             highest channel number).
  *           Sequencer ranks are selected using
  *           function "LL_ADC_REG_SetSequencerChannels()".
  * @note   Sequencer disabled is equivalent to sequencer of 1 rank:
  *         ADC conversion on only 1 channel.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on group regular.
  * @rmtoll SQR1     L              LL_ADC_REG_SetSequencerLength
  * @param  ADCx ADC instance
  * @param  SequencerNbRanks This parameter can be one of the following values:
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_DISABLE
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_SetSequencerLength(ADC_TypeDef *ADCx, uint32_t SequencerNbRanks)
{
  MODIFY_REG(ADCx->SQR1, ADC_SQR1_L, SequencerNbRanks);
}

/**
  * @brief  Get ADC group regular sequencer length and scan direction.
  * @note   Description of ADC group regular sequencer features:
  *         - For devices with sequencer fully configurable
  *           (function "LL_ADC_REG_SetSequencerRanks()" available):
  *           sequencer length and each rank affectation to a channel
  *           are configurable.
  *           This function retrieves:
  *           - Sequence length: Number of ranks in the scan sequence.
  *           - Sequence direction: Unless specified in parameters, sequencer
  *             scan direction is forward (from rank 1 to rank n).
  *           Sequencer ranks are selected using
  *           function "LL_ADC_REG_SetSequencerRanks()".
  *         - For devices with sequencer not fully configurable
  *           (function "LL_ADC_REG_SetSequencerChannels()" available):
  *           sequencer length and each rank affectation to a channel
  *           are defined by channel number.
  *           This function retrieves:
  *           - Sequence length: Number of ranks in the scan sequence is
  *             defined by number of channels set in the sequence,
  *             rank of each channel is fixed by channel HW number.
  *             (channel 0 fixed on rank 0, channel 1 fixed on rank1, ...).
  *           - Sequence direction: Unless specified in parameters, sequencer
  *             scan direction is forward (from lowest channel number to
  *             highest channel number).
  *           Sequencer ranks are selected using
  *           function "LL_ADC_REG_SetSequencerChannels()".
  * @note   Sequencer disabled is equivalent to sequencer of 1 rank:
  *         ADC conversion on only 1 channel.
  * @rmtoll SQR1     L              LL_ADC_REG_GetSequencerLength
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_DISABLE
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS
  *         @arg @ref LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS
  */
__STATIC_INLINE uint32_t LL_ADC_REG_GetSequencerLength(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->SQR1, ADC_SQR1_L));
}

/**
  * @brief  Set ADC group regular sequencer discontinuous mode:
  *         sequence subdivided and scan conversions interrupted every selected
  *         number of ranks.
  * @note   It is not possible to enable both ADC group regular
  *         continuous mode and sequencer discontinuous mode.
  * @note   It is not possible to enable both ADC auto-injected mode
  *         and ADC group regular sequencer discontinuous mode.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on group regular.
  * @rmtoll CFGR1    DISCEN         LL_ADC_REG_SetSequencerDiscont\n
  *         CFGR1    DISCNUM        LL_ADC_REG_SetSequencerDiscont
  * @param  ADCx ADC instance
  * @param  SeqDiscont This parameter can be one of the following values:
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_DISABLE
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_1RANK
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_2RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_3RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_4RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_5RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_6RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_7RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_8RANKS
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_SetSequencerDiscont(ADC_TypeDef *ADCx, uint32_t SeqDiscont)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_DISCEN | ADC_CFGR1_DISCNUM, SeqDiscont);
}

/**
  * @brief  Get ADC group regular sequencer discontinuous mode:
  *         sequence subdivided and scan conversions interrupted every selected
  *         number of ranks.
  * @rmtoll CFGR1    DISCEN         LL_ADC_REG_GetSequencerDiscont\n
  *         CFGR1    DISCNUM        LL_ADC_REG_GetSequencerDiscont
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_DISABLE
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_1RANK
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_2RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_3RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_4RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_5RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_6RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_7RANKS
  *         @arg @ref LL_ADC_REG_SEQ_DISCONT_8RANKS
  */
__STATIC_INLINE uint32_t LL_ADC_REG_GetSequencerDiscont(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_DISCEN | ADC_CFGR1_DISCNUM));
}

/**
  * @brief  Set ADC group regular sequence: channel on the selected
  *         scan sequence rank.
  * @note   This function performs configuration of:
  *         - Channels ordering into each rank of scan sequence:
  *           whatever channel can be placed into whatever rank.
  * @note   On this STM32 series, ADC group regular sequencer is
  *         fully configurable: sequencer length and each rank
  *         affectation to a channel are configurable.
  *         Refer to description of function @ref LL_ADC_REG_SetSequencerLength().
  * @note   Depending on devices and packages, some channels may not be available.
  *         Refer to device datasheet for channels availability.
  * @note   On this STM32 series, to measure internal channels (VrefInt,
  *         TempSensor, ...), measurement paths to internal channels must be
  *         enabled separately.
  *         This can be done using function @ref LL_ADC_SetCommonPathInternalCh().
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on group regular.
  * @rmtoll SQR1     SQ1            LL_ADC_REG_SetSequencerRanks\n
  *         SQR1     SQ2            LL_ADC_REG_SetSequencerRanks\n
  *         SQR1     SQ3            LL_ADC_REG_SetSequencerRanks\n
  *         SQR1     SQ4            LL_ADC_REG_SetSequencerRanks\n
  *         SQR2     SQ5            LL_ADC_REG_SetSequencerRanks\n
  *         SQR2     SQ6            LL_ADC_REG_SetSequencerRanks\n
  *         SQR2     SQ7            LL_ADC_REG_SetSequencerRanks\n
  *         SQR2     SQ8            LL_ADC_REG_SetSequencerRanks\n
  *         SQR2     SQ9            LL_ADC_REG_SetSequencerRanks\n
  *         SQR3     SQ10           LL_ADC_REG_SetSequencerRanks\n
  *         SQR3     SQ11           LL_ADC_REG_SetSequencerRanks\n
  *         SQR3     SQ12           LL_ADC_REG_SetSequencerRanks\n
  *         SQR3     SQ13           LL_ADC_REG_SetSequencerRanks\n
  *         SQR3     SQ14           LL_ADC_REG_SetSequencerRanks\n
  *         SQR4     SQ15           LL_ADC_REG_SetSequencerRanks\n
  *         SQR4     SQ16           LL_ADC_REG_SetSequencerRanks
  * @param  ADCx ADC instance
  * @param  Rank This parameter can be one of the following values:
  *         @arg @ref LL_ADC_REG_RANK_1
  *         @arg @ref LL_ADC_REG_RANK_2
  *         @arg @ref LL_ADC_REG_RANK_3
  *         @arg @ref LL_ADC_REG_RANK_4
  *         @arg @ref LL_ADC_REG_RANK_5
  *         @arg @ref LL_ADC_REG_RANK_6
  *         @arg @ref LL_ADC_REG_RANK_7
  *         @arg @ref LL_ADC_REG_RANK_8
  *         @arg @ref LL_ADC_REG_RANK_9
  *         @arg @ref LL_ADC_REG_RANK_10
  *         @arg @ref LL_ADC_REG_RANK_11
  *         @arg @ref LL_ADC_REG_RANK_12
  *         @arg @ref LL_ADC_REG_RANK_13
  *         @arg @ref LL_ADC_REG_RANK_14
  *         @arg @ref LL_ADC_REG_RANK_15
  *         @arg @ref LL_ADC_REG_RANK_16
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_SetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t Channel)
{
  /* Set bits with content of parameter "Channel" with bits position          */
  /* in register and register position depending on parameter "Rank".         */
  /* Parameters "Rank" and "Channel" are used with masks because containing   */
  /* other bits reserved for other purpose.                                   */
  __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->SQR1,
                                             ((Rank & ADC_REG_SQRX_REGOFFSET_MASK) >> ADC_SQRX_REGOFFSET_POS));

  MODIFY_REG(*preg,
             ADC_CHANNEL_NUMBER_MASK_POSBIT0 << (Rank & ADC_REG_RANK_ID_SQRX_MASK),
             (Channel & ADC_CHANNEL_NUMBER_MASK) << (Rank & ADC_REG_RANK_ID_SQRX_MASK));
}

/**
  * @brief  Get ADC group regular sequence: channel on the selected
  *         scan sequence rank.
  * @note   On this STM32 series, ADC group regular sequencer is
  *         fully configurable: sequencer length and each rank
  *         affectation to a channel are configurable.
  *         Refer to description of function @ref LL_ADC_REG_SetSequencerLength().
  * @note   Depending on devices and packages, some channels may not be available.
  *         Refer to device datasheet for channels availability.
  * @note   Usage of the returned channel number:
  *         - To reinject this channel into another function LL_ADC_xxx:
  *           the returned channel number is only partly formatted on definition
  *           of literals LL_ADC_CHANNEL_x. Therefore, it has to be compared
  *           with parts of literals LL_ADC_CHANNEL_x or using
  *           helper macro @ref __LL_ADC_CHANNEL_TO_DECIMAL_NB().
  *           Then the selected literal LL_ADC_CHANNEL_x can be used
  *           as parameter for another function.
  *         - To get the channel number in decimal format:
  *           process the returned value with the helper macro
  *           @ref __LL_ADC_CHANNEL_TO_DECIMAL_NB().
  * @rmtoll SQR1     SQ1            LL_ADC_REG_GetSequencerRanks\n
  *         SQR1     SQ2            LL_ADC_REG_GetSequencerRanks\n
  *         SQR1     SQ3            LL_ADC_REG_GetSequencerRanks\n
  *         SQR1     SQ4            LL_ADC_REG_GetSequencerRanks\n
  *         SQR2     SQ5            LL_ADC_REG_GetSequencerRanks\n
  *         SQR2     SQ6            LL_ADC_REG_GetSequencerRanks\n
  *         SQR2     SQ7            LL_ADC_REG_GetSequencerRanks\n
  *         SQR2     SQ8            LL_ADC_REG_GetSequencerRanks\n
  *         SQR2     SQ9            LL_ADC_REG_GetSequencerRanks\n
  *         SQR3     SQ10           LL_ADC_REG_GetSequencerRanks\n
  *         SQR3     SQ11           LL_ADC_REG_GetSequencerRanks\n
  *         SQR3     SQ12           LL_ADC_REG_GetSequencerRanks\n
  *         SQR3     SQ13           LL_ADC_REG_GetSequencerRanks\n
  *         SQR3     SQ14           LL_ADC_REG_GetSequencerRanks\n
  *         SQR4     SQ15           LL_ADC_REG_GetSequencerRanks\n
  *         SQR4     SQ16           LL_ADC_REG_GetSequencerRanks
  * @param  ADCx ADC instance
  * @param  Rank This parameter can be one of the following values:
  *         @arg @ref LL_ADC_REG_RANK_1
  *         @arg @ref LL_ADC_REG_RANK_2
  *         @arg @ref LL_ADC_REG_RANK_3
  *         @arg @ref LL_ADC_REG_RANK_4
  *         @arg @ref LL_ADC_REG_RANK_5
  *         @arg @ref LL_ADC_REG_RANK_6
  *         @arg @ref LL_ADC_REG_RANK_7
  *         @arg @ref LL_ADC_REG_RANK_8
  *         @arg @ref LL_ADC_REG_RANK_9
  *         @arg @ref LL_ADC_REG_RANK_10
  *         @arg @ref LL_ADC_REG_RANK_11
  *         @arg @ref LL_ADC_REG_RANK_12
  *         @arg @ref LL_ADC_REG_RANK_13
  *         @arg @ref LL_ADC_REG_RANK_14
  *         @arg @ref LL_ADC_REG_RANK_15
  *         @arg @ref LL_ADC_REG_RANK_16
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)(3)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)(3)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)(3)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  *         (3) For ADC channel read back from ADC register,
  *             comparison with internal channel parameter to be done
  *             using helper macro @ref __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL().
  */
__STATIC_INLINE uint32_t LL_ADC_REG_GetSequencerRanks(const ADC_TypeDef *ADCx, uint32_t Rank)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->SQR1,
                                                   ((Rank & ADC_REG_SQRX_REGOFFSET_MASK) >> ADC_SQRX_REGOFFSET_POS));

  return (uint32_t)((READ_BIT(*preg,
                              ADC_CHANNEL_NUMBER_MASK_POSBIT0 << (Rank & ADC_REG_RANK_ID_SQRX_MASK))
                     >> (Rank & ADC_REG_RANK_ID_SQRX_MASK))
                   );
}

/**
  * @brief  Set ADC Channel Preselection to LL_ADC_CHANNEL_x, x = 0 to 19.
  * @note   This function set the the value for the channel preselection register
  *         corresponding to ADC channel to be selected.
  * @rmtoll PCSEL   PCSEL          LL_ADC_SetChannelPreselection
  * @param  ADCx ADC instance.
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetChannelPreselection(ADC_TypeDef *ADCx, uint32_t Channel)
{
  __IO uint32_t channel_preselectione = READ_REG(ADCx->PCSEL);
  WRITE_REG(ADCx->PCSEL,
            channel_preselectione | (1UL << (__LL_ADC_CHANNEL_TO_DECIMAL_NB((uint32_t)Channel) & 0x1FUL)));
}

/**
  * @brief  Get ADC Channel Preselection register value.
  * @note   This function set the the value for the channel preselection register
  *         corresponding to ADC channel to be selected.
  * @rmtoll PCSEL   PCSEL          LL_ADC_GetChannelPreselection
  * @param  ADCx ADC instance.
  *
  * @retval Returned decimal value that can correspend to one or multiple channels:
  */
__STATIC_INLINE uint32_t LL_ADC_GetChannelPreselection(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->PCSEL, ADC_PCSEL_PCSEL));
}

/**
  * @brief  Set ADC continuous conversion mode on ADC group regular.
  * @note   Description of ADC continuous conversion mode:
  *         - single mode: one conversion per trigger
  *         - continuous mode: after the first trigger, following
  *           conversions launched successively automatically.
  * @note   It is not possible to enable both ADC group regular
  *         continuous mode and sequencer discontinuous mode.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on group regular.
  * @rmtoll CFGR1    CONT           LL_ADC_REG_SetContinuousMode
  * @param  ADCx ADC instance
  * @param  Continuous This parameter can be one of the following values:
  *         @arg @ref LL_ADC_REG_CONV_SINGLE
  *         @arg @ref LL_ADC_REG_CONV_CONTINUOUS
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_SetContinuousMode(ADC_TypeDef *ADCx, uint32_t Continuous)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_CONT, Continuous);
}

/**
  * @brief  Get ADC continuous conversion mode on ADC group regular.
  * @note   Description of ADC continuous conversion mode:
  *         - single mode: one conversion per trigger
  *         - continuous mode: after the first trigger, following
  *           conversions launched successively automatically.
  * @rmtoll CFGR1    CONT           LL_ADC_REG_GetContinuousMode
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_REG_CONV_SINGLE
  *         @arg @ref LL_ADC_REG_CONV_CONTINUOUS
  */
__STATIC_INLINE uint32_t LL_ADC_REG_GetContinuousMode(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_CONT));
}

/**
  * @brief  Set ADC data transfer mode
  * @note   Conversion data can be either:
  *            - Available in Data Register
  *            - Transferred by DMA in one shot mode
  *            - Transferred by DMA in circular mode
  *            - Transferred to MDF data register
  * @rmtoll CFGR1    DMNGT           LL_ADC_REG_SetDataTransferMode
  * @param  ADCx ADC instance
  * @param  DataTransferMode Select Data Management configuration
  *         @arg @ref LL_ADC_REG_DR_TRANSFER
  *         @arg @ref LL_ADC_REG_DMA_TRANSFER_LIMITED
  *         @arg @ref LL_ADC_REG_DMA_TRANSFER_UNLIMITED
  *         @arg @ref LL_ADC_REG_MDF_TRANSFER
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_SetDataTransferMode(ADC_TypeDef *ADCx, uint32_t DataTransferMode)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_DMNGT, DataTransferMode);
}

/**
  * @brief  Get ADC data transfer mode
  * @note   Conversion data can be either:
  *            - Available in Data Register
  *            - Transferred by DMA in one shot mode
  *            - Transferred by DMA in circular mode
  *            - Transferred to DFSDM data register
  * @rmtoll CFGR1    DMNGT           LL_ADC_REG_GetDataTransferMode
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_REG_DR_TRANSFER
  *         @arg @ref LL_ADC_REG_DMA_TRANSFER_LIMITED
  *         @arg @ref LL_ADC_REG_DMA_TRANSFER_UNLIMITED
  *         @arg @ref LL_ADC_REG_MDF_TRANSFER
  */
__STATIC_INLINE uint32_t LL_ADC_REG_GetDataTransferMode(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_DMNGT));
}

/**
  * @brief  Set ADC group regular behavior in case of overrun:
  *         data preserved or overwritten.
  * @note   Compatibility with devices without feature overrun:
  *         other devices without this feature have a behavior
  *         equivalent to data overwritten.
  *         The default setting of overrun is data preserved.
  *         Therefore, for compatibility with all devices, parameter
  *         overrun should be set to data overwritten.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on group regular.
  * @rmtoll CFGR1    OVRMOD         LL_ADC_REG_SetOverrun
  * @param  ADCx ADC instance
  * @param  Overrun This parameter can be one of the following values:
  *         @arg @ref LL_ADC_REG_OVR_DATA_PRESERVED
  *         @arg @ref LL_ADC_REG_OVR_DATA_OVERWRITTEN
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_SetOverrun(ADC_TypeDef *ADCx, uint32_t Overrun)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_OVRMOD, Overrun);
}

/**
  * @brief  Get ADC group regular behavior in case of overrun:
  *         data preserved or overwritten.
  * @rmtoll CFGR1    OVRMOD         LL_ADC_REG_GetOverrun
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_REG_OVR_DATA_PRESERVED
  *         @arg @ref LL_ADC_REG_OVR_DATA_OVERWRITTEN
  */
__STATIC_INLINE uint32_t LL_ADC_REG_GetOverrun(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_OVRMOD));
}

/**
  * @}
  */

/** @defgroup ADC_LL_EF_Configuration_ADC_Group_Injected Configuration of ADC hierarchical scope: group injected
  * @{
  */

/**
  * @brief  Set ADC group injected conversion trigger source:
  *         internal (SW start) or from external peripheral (timer event,
  *         external interrupt line).
  * @note   On this STM32 series, setting trigger source to external trigger
  *         also set trigger polarity to rising edge
  *         (default setting for compatibility with some ADC on other
  *         STM32 series having this setting set by HW default value).
  *         In case of need to modify trigger edge, use
  *         function @ref LL_ADC_INJ_SetTriggerEdge().
  * @note   Availability of parameters of trigger sources from timer
  *         depends on timers availability on the selected device.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must not be disabled. Can be enabled with or without conversion
  *         on going on either groups regular or injected.
  * @rmtoll JSQR     JEXTSEL        LL_ADC_INJ_SetTriggerSource\n
  *         JSQR     JEXTEN         LL_ADC_INJ_SetTriggerSource
  * @param  ADCx ADC instance
  * @param  TriggerSource This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_TRIG_SOFTWARE
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_EXTI_LINE15
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM1_CH4
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM1_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM1_TRGO2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM2_CH1
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM2_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM3_CH1
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM3_CH3
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM3_CH4
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM3_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM4_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM5_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM6_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM7_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM8_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM8_TRGO2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM9_CH2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM9_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM12_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM15_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM18_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_LPTIM1_CH2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_LPTIM2_CH2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_LPTIM3_CH2
  * @retval None
  */
__STATIC_INLINE void LL_ADC_INJ_SetTriggerSource(ADC_TypeDef *ADCx, uint32_t TriggerSource)
{
  MODIFY_REG(ADCx->JSQR, ADC_JSQR_JEXTSEL | ADC_JSQR_JEXTEN, TriggerSource);
}

/**
  * @brief  Get ADC group injected conversion trigger source:
  *         internal (SW start) or from external peripheral (timer event,
  *         external interrupt line).
  * @note   To determine whether group injected trigger source is
  *         internal (SW start) or external, without detail
  *         of which peripheral is selected as external trigger,
  *         (equivalent to
  *         "if(LL_ADC_INJ_GetTriggerSource(ADC1) == LL_ADC_INJ_TRIG_SOFTWARE)")
  *         use function @ref LL_ADC_INJ_IsTriggerSourceSWStart.
  * @note   Availability of parameters of trigger sources from timer
  *         depends on timers availability on the selected device.
  * @rmtoll JSQR     JEXTSEL        LL_ADC_INJ_GetTriggerSource\n
  *         JSQR     JEXTEN         LL_ADC_INJ_GetTriggerSource
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_INJ_TRIG_SOFTWARE
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_EXTI_LINE15
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM1_CH4
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM1_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM1_TRGO2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM2_CH1
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM2_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM3_CH1
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM3_CH3
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM3_CH4
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM3_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM4_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM5_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM6_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM7_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM8_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM8_TRGO2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM9_CH2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM9_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM12_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM15_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_TIM18_TRGO
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_LPTIM1_CH2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_LPTIM2_CH2
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_LPTIM3_CH2
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_GetTriggerSource(const ADC_TypeDef *ADCx)
{
  __IO uint32_t trigger_source = READ_BIT(ADCx->JSQR, ADC_JSQR_JEXTSEL | ADC_JSQR_JEXTEN);

  /* Value for shift of {0; 4; 8; 12} depending on value of bitfield          */
  /* corresponding to ADC_JSQR_JEXTEN {0; 1; 2; 3}.                           */
  uint32_t shift_jexten = ((trigger_source & ADC_JSQR_JEXTEN) >> (ADC_INJ_TRIG_EXTEN_BITOFFSET_POS - 2UL));

  /* Set bitfield corresponding to ADC_JSQR_JEXTEN and ADC_JSQR_JEXTSEL       */
  /* to match with triggers literals definition.                              */
  return ((trigger_source
           & (ADC_INJ_TRIG_SOURCE_MASK >> shift_jexten) & ADC_JSQR_JEXTSEL)
          | ((ADC_INJ_TRIG_EDGE_MASK >> shift_jexten) & ADC_JSQR_JEXTEN)
         );
}

/**
  * @brief  Get ADC group injected conversion trigger source internal (SW start)
            or external
  * @note   In case of group injected trigger source set to external trigger,
  *         to determine which peripheral is selected as external trigger,
  *         use function @ref LL_ADC_INJ_GetTriggerSource.
  * @rmtoll JSQR     JEXTEN         LL_ADC_INJ_IsTriggerSourceSWStart
  * @param  ADCx ADC instance
  * @retval Value "0" if trigger source external trigger
  *         Value "1" if trigger source SW start.
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_IsTriggerSourceSWStart(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->JSQR, ADC_JSQR_JEXTEN) == (LL_ADC_INJ_TRIG_SOFTWARE & ADC_JSQR_JEXTEN)) ? 1UL : 0UL);
}

/**
  * @brief  Set ADC group injected conversion trigger polarity.
  *         Applicable only for trigger source set to external trigger.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must not be disabled. Can be enabled with or without conversion
  *         on going on either groups regular or injected.
  * @rmtoll JSQR     JEXTEN         LL_ADC_INJ_SetTriggerEdge
  * @param  ADCx ADC instance
  * @param  ExternalTriggerEdge This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_RISING
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_FALLING
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_RISINGFALLING
  * @retval None
  */
__STATIC_INLINE void LL_ADC_INJ_SetTriggerEdge(ADC_TypeDef *ADCx, uint32_t ExternalTriggerEdge)
{
  MODIFY_REG(ADCx->JSQR, ADC_JSQR_JEXTEN, ExternalTriggerEdge);
}

/**
  * @brief  Get ADC group injected conversion trigger polarity.
  *         Applicable only for trigger source set to external trigger.
  * @rmtoll JSQR     JEXTEN         LL_ADC_INJ_GetTriggerEdge
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_RISING
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_FALLING
  *         @arg @ref LL_ADC_INJ_TRIG_EXT_RISINGFALLING
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_GetTriggerEdge(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->JSQR, ADC_JSQR_JEXTEN));
}

/**
  * @brief  Set ADC group injected sequencer length and scan direction.
  * @note   This function performs configuration of:
  *         - Sequence length: Number of ranks in the scan sequence.
  *         - Sequence direction: Unless specified in parameters, sequencer
  *           scan direction is forward (from rank 1 to rank n).
  * @note   Sequencer disabled is equivalent to sequencer of 1 rank:
  *         ADC conversion on only 1 channel.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must not be disabled. Can be enabled with or without conversion
  *         on going on either groups regular or injected.
  * @rmtoll JSQR     JL             LL_ADC_INJ_SetSequencerLength
  * @param  ADCx ADC instance
  * @param  SequencerNbRanks This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_SEQ_SCAN_DISABLE
  *         @arg @ref LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS
  *         @arg @ref LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS
  *         @arg @ref LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS
  * @retval None
  */
__STATIC_INLINE void LL_ADC_INJ_SetSequencerLength(ADC_TypeDef *ADCx, uint32_t SequencerNbRanks)
{
  MODIFY_REG(ADCx->JSQR, ADC_JSQR_JL, SequencerNbRanks);
}

/**
  * @brief  Get ADC group injected sequencer length and scan direction.
  * @note   This function retrieves:
  *         - Sequence length: Number of ranks in the scan sequence.
  *         - Sequence direction: Unless specified in parameters, sequencer
  *           scan direction is forward (from rank 1 to rank n).
  * @note   Sequencer disabled is equivalent to sequencer of 1 rank:
  *         ADC conversion on only 1 channel.
  * @rmtoll JSQR     JL             LL_ADC_INJ_GetSequencerLength
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_INJ_SEQ_SCAN_DISABLE
  *         @arg @ref LL_ADC_INJ_SEQ_SCAN_ENABLE_2RANKS
  *         @arg @ref LL_ADC_INJ_SEQ_SCAN_ENABLE_3RANKS
  *         @arg @ref LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_GetSequencerLength(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->JSQR, ADC_JSQR_JL));
}

/**
  * @brief  Set ADC group injected sequencer discontinuous mode:
  *         sequence subdivided and scan conversions interrupted every selected
  *         number of ranks.
  * @note   It is not possible to enable both ADC group injected
  *         auto-injected mode and sequencer discontinuous mode.
  * @rmtoll CFGR1    JDISCEN        LL_ADC_INJ_SetSequencerDiscont
  * @param  ADCx ADC instance
  * @param  SeqDiscont This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_SEQ_DISCONT_DISABLE
  *         @arg @ref LL_ADC_INJ_SEQ_DISCONT_1RANK
  * @retval None
  */
__STATIC_INLINE void LL_ADC_INJ_SetSequencerDiscont(ADC_TypeDef *ADCx, uint32_t SeqDiscont)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_JDISCEN, SeqDiscont);
}

/**
  * @brief  Get ADC group injected sequencer discontinuous mode:
  *         sequence subdivided and scan conversions interrupted every selected
  *         number of ranks.
  * @rmtoll CFGR1    JDISCEN        LL_ADC_INJ_GetSequencerDiscont
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_INJ_SEQ_DISCONT_DISABLE
  *         @arg @ref LL_ADC_INJ_SEQ_DISCONT_1RANK
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_GetSequencerDiscont(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_JDISCEN));
}

/**
  * @brief  Set ADC group injected sequence: channel on the selected
  *         sequence rank.
  * @note   Depending on devices and packages, some channels may not be available.
  *         Refer to device datasheet for channels availability.
  * @note   On this STM32 series, to measure internal channels (VrefInt,
  *         TempSensor, ...), measurement paths to internal channels must be
  *         enabled separately.
  *         This can be done using function @ref LL_ADC_SetCommonPathInternalCh().
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must not be disabled. Can be enabled with or without conversion
  *         on going on either groups regular or injected.
  * @rmtoll JSQR     JSQ1           LL_ADC_INJ_SetSequencerRanks\n
  *         JSQR     JSQ2           LL_ADC_INJ_SetSequencerRanks\n
  *         JSQR     JSQ3           LL_ADC_INJ_SetSequencerRanks\n
  *         JSQR     JSQ4           LL_ADC_INJ_SetSequencerRanks
  * @param  ADCx ADC instance
  * @param  Rank This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_RANK_1
  *         @arg @ref LL_ADC_INJ_RANK_2
  *         @arg @ref LL_ADC_INJ_RANK_3
  *         @arg @ref LL_ADC_INJ_RANK_4
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @retval None
  */
__STATIC_INLINE void LL_ADC_INJ_SetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t Channel)
{
  /* Set bits with content of parameter "Channel" with bits position          */
  /* in register depending on parameter "Rank".                               */
  /* Parameters "Rank" and "Channel" are used with masks because containing   */
  /* other bits reserved for other purpose.                                   */
  MODIFY_REG(ADCx->JSQR,
             (ADC_CHANNEL_NUMBER_MASK >> ADC_AWD_CHANNEL_NUMBER_BITOFFSET_POS)
             << (Rank & ADC_INJ_RANK_ID_JSQR_MASK),
             (Channel & ADC_CHANNEL_NUMBER_MASK) << (Rank & ADC_INJ_RANK_ID_JSQR_MASK));
}

/**
  * @brief  Get ADC group injected sequence: channel on the selected
  *         sequence rank.
  * @note   Depending on devices and packages, some channels may not be available.
  *         Refer to device datasheet for channels availability.
  * @note   Usage of the returned channel number:
  *         - To reinject this channel into another function LL_ADC_xxx:
  *           the returned channel number is only partly formatted on definition
  *           of literals LL_ADC_CHANNEL_x. Therefore, it has to be compared
  *           with parts of literals LL_ADC_CHANNEL_x or using
  *           helper macro @ref __LL_ADC_CHANNEL_TO_DECIMAL_NB().
  *           Then the selected literal LL_ADC_CHANNEL_x can be used
  *           as parameter for another function.
  *         - To get the channel number in decimal format:
  *           process the returned value with the helper macro
  *           @ref __LL_ADC_CHANNEL_TO_DECIMAL_NB().
  * @rmtoll JSQR     JSQ1           LL_ADC_INJ_GetSequencerRanks\n
  *         JSQR     JSQ2           LL_ADC_INJ_GetSequencerRanks\n
  *         JSQR     JSQ3           LL_ADC_INJ_GetSequencerRanks\n
  *         JSQR     JSQ4           LL_ADC_INJ_GetSequencerRanks
  * @param  ADCx ADC instance
  * @param  Rank This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_RANK_1
  *         @arg @ref LL_ADC_INJ_RANK_2
  *         @arg @ref LL_ADC_INJ_RANK_3
  *         @arg @ref LL_ADC_INJ_RANK_4
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)(3)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)(3)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)(3)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  *         (3) For ADC channel read back from ADC register,
  *             comparison with internal channel parameter to be done
  *             using helper macro @ref __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL().
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_GetSequencerRanks(const ADC_TypeDef *ADCx, uint32_t Rank)
{
  return (uint32_t)((READ_BIT(ADCx->JSQR,
                              ADC_CHANNEL_NUMBER_MASK << (Rank & ADC_INJ_RANK_ID_JSQR_MASK))
                     >> (Rank & ADC_INJ_RANK_ID_JSQR_MASK))
                   );
}

/**
  * @brief  Set ADC group injected conversion trigger:
  *         independent or from ADC group regular.
  * @note   This mode can be used to extend number of data registers
  *         updated after one ADC conversion trigger and with data
  *         permanently kept (not erased by successive conversions of scan of
  *         ADC sequencer ranks), up to 5 data registers:
  *         1 data register on ADC group regular, 4 data registers
  *         on ADC group injected.
  * @note   If ADC group injected injected trigger source is set to an
  *         external trigger, this feature must be must be set to
  *         independent trigger.
  *         ADC group injected automatic trigger is compliant only with
  *         group injected trigger source set to SW start, without any
  *         further action on  ADC group injected conversion start or stop:
  *         in this case, ADC group injected is controlled only
  *         from ADC group regular.
  * @note   It is not possible to enable both ADC group injected
  *         auto-injected mode and sequencer discontinuous mode.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll CFGR1    JAUTO          LL_ADC_INJ_SetTrigAuto
  * @param  ADCx ADC instance
  * @param  TrigAuto This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_TRIG_INDEPENDENT
  *         @arg @ref LL_ADC_INJ_TRIG_FROM_GRP_REGULAR
  * @retval None
  */
__STATIC_INLINE void LL_ADC_INJ_SetTrigAuto(ADC_TypeDef *ADCx, uint32_t TrigAuto)
{
  MODIFY_REG(ADCx->CFGR1, ADC_CFGR1_JAUTO, TrigAuto);
}

/**
  * @brief  Get ADC group injected conversion trigger:
  *         independent or from ADC group regular.
  * @rmtoll CFGR1    JAUTO          LL_ADC_INJ_GetTrigAuto
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_INJ_TRIG_INDEPENDENT
  *         @arg @ref LL_ADC_INJ_TRIG_FROM_GRP_REGULAR
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_GetTrigAuto(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR1, ADC_CFGR1_JAUTO));
}

/**
  * @}
  */

/** @defgroup ADC_LL_EF_Configuration_Channels Configuration of ADC hierarchical scope: channels
  * @{
  */

/**
  * @brief  Set sampling time of the selected ADC channel
  *         Unit: ADC clock cycles.
  * @note   On this device, sampling time is on channel scope: independently
  *         of channel mapped on ADC group regular or injected.
  * @note   In case of internal channel (VrefInt, TempSensor, ...) to be
  *         converted:
  *         sampling time constraints must be respected (sampling time can be
  *         adjusted in function of ADC clock frequency and sampling time
  *         setting).
  *         Refer to device datasheet for timings values (parameters TS_vrefint,
  *         TS_temp, ...).
  * @note   Conversion time is the addition of sampling time and processing time.
  *         On this STM32 series, ADC processing time is:
  *         - 13.5 ADC clock cycles at ADC resolution 12 bits
  *         - 11.5 ADC clock cycles at ADC resolution 10 bits
  *         - 8.5 ADC clock cycles at ADC resolution 8 bits
  *         - 6.5 ADC clock cycles at ADC resolution 6 bits
  * @note   In case of ADC conversion of internal channel (VrefInt,
  *         temperature sensor, ...), a sampling time minimum value
  *         is required.
  *         Refer to device datasheet.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll SMPR1    SMP0           LL_ADC_SetChannelSamplingTime\n
  *         SMPR1    SMP1           LL_ADC_SetChannelSamplingTime\n
  *         SMPR1    SMP2           LL_ADC_SetChannelSamplingTime\n
  *         SMPR1    SMP3           LL_ADC_SetChannelSamplingTime\n
  *         SMPR1    SMP4           LL_ADC_SetChannelSamplingTime\n
  *         SMPR1    SMP5           LL_ADC_SetChannelSamplingTime\n
  *         SMPR1    SMP6           LL_ADC_SetChannelSamplingTime\n
  *         SMPR1    SMP7           LL_ADC_SetChannelSamplingTime\n
  *         SMPR1    SMP8           LL_ADC_SetChannelSamplingTime\n
  *         SMPR1    SMP9           LL_ADC_SetChannelSamplingTime\n
  *         SMPR2    SMP10          LL_ADC_SetChannelSamplingTime\n
  *         SMPR2    SMP11          LL_ADC_SetChannelSamplingTime\n
  *         SMPR2    SMP12          LL_ADC_SetChannelSamplingTime\n
  *         SMPR2    SMP13          LL_ADC_SetChannelSamplingTime\n
  *         SMPR2    SMP14          LL_ADC_SetChannelSamplingTime\n
  *         SMPR2    SMP15          LL_ADC_SetChannelSamplingTime\n
  *         SMPR2    SMP16          LL_ADC_SetChannelSamplingTime\n
  *         SMPR2    SMP17          LL_ADC_SetChannelSamplingTime\n
  *         SMPR2    SMP18          LL_ADC_SetChannelSamplingTime
  * @param  ADCx ADC instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  * @param  SamplingTime This parameter can be one of the following values:
  *         @arg @ref LL_ADC_SAMPLINGTIME_1CYCLE_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_2CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_6CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_11CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_23CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_46CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_246CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_1499CYCLES_5
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetChannelSamplingTime(ADC_TypeDef *ADCx, uint32_t Channel, uint32_t SamplingTime)
{
  /* Set bits with content of parameter "SamplingTime" with bits position     */
  /* in register and register position depending on parameter "Channel".      */
  /* Parameter "Channel" is used with masks because containing                */
  /* other bits reserved for other purpose.                                   */
  const uint32_t iChannel = __ADC_CHANNEL_INDEX(Channel);
  __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->SMPR1,
                                             ((ADC_CHANNEL_LUT[iChannel]
                                               & ADC_CHANNEL_SMPRX_REGOFFSET_MASK) >> ADC_SMPRX_REGOFFSET_POS));
  MODIFY_REG(*preg,
             ADC_SMPR1_SMP0 << ((ADC_CHANNEL_LUT[iChannel] & ADC_CHANNEL_SMPx_BITOFFSET_MASK)
                                >> ADC_CHANNEL_SMPx_BITOFFSET_POS),
             SamplingTime   << ((ADC_CHANNEL_LUT[iChannel] & ADC_CHANNEL_SMPx_BITOFFSET_MASK)
                                >> ADC_CHANNEL_SMPx_BITOFFSET_POS));
}

/**
  * @brief  Get sampling time of the selected ADC channel
  *         Unit: ADC clock cycles.
  * @note   On this device, sampling time is on channel scope: independently
  *         of channel mapped on ADC group regular or injected.
  * @note   Conversion time is the addition of sampling time and processing time.
  *         On this STM32 series, ADC processing time is:
  *         - 13.5 ADC clock cycles at ADC resolution 12 bits
  *         - 11.5 ADC clock cycles at ADC resolution 10 bits
  *         - 8.5 ADC clock cycles at ADC resolution 8 bits
  *         - 6.5 ADC clock cycles at ADC resolution 6 bits
  * @rmtoll SMPR1    SMP0           LL_ADC_GetChannelSamplingTime\n
  *         SMPR1    SMP1           LL_ADC_GetChannelSamplingTime\n
  *         SMPR1    SMP2           LL_ADC_GetChannelSamplingTime\n
  *         SMPR1    SMP3           LL_ADC_GetChannelSamplingTime\n
  *         SMPR1    SMP4           LL_ADC_GetChannelSamplingTime\n
  *         SMPR1    SMP5           LL_ADC_GetChannelSamplingTime\n
  *         SMPR1    SMP6           LL_ADC_GetChannelSamplingTime\n
  *         SMPR1    SMP7           LL_ADC_GetChannelSamplingTime\n
  *         SMPR1    SMP8           LL_ADC_GetChannelSamplingTime\n
  *         SMPR1    SMP9           LL_ADC_GetChannelSamplingTime\n
  *         SMPR2    SMP10          LL_ADC_GetChannelSamplingTime\n
  *         SMPR2    SMP11          LL_ADC_GetChannelSamplingTime\n
  *         SMPR2    SMP12          LL_ADC_GetChannelSamplingTime\n
  *         SMPR2    SMP13          LL_ADC_GetChannelSamplingTime\n
  *         SMPR2    SMP14          LL_ADC_GetChannelSamplingTime\n
  *         SMPR2    SMP15          LL_ADC_GetChannelSamplingTime\n
  *         SMPR2    SMP16          LL_ADC_GetChannelSamplingTime\n
  *         SMPR2    SMP17          LL_ADC_GetChannelSamplingTime\n
  *         SMPR2    SMP18          LL_ADC_GetChannelSamplingTime
  * @param  ADCx ADC instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_0
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  *         @arg @ref LL_ADC_CHANNEL_VREFINT     (1)(3)
  *         @arg @ref LL_ADC_CHANNEL_VBAT        (2)(3)
  *         @arg @ref LL_ADC_CHANNEL_VDDCORE     (2)(3)
  *
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC1.\n
  *         (2) On this STM32 series, parameter available only on ADC instance: ADC2.\n
  *         (3) For ADC channel read back from ADC register,
  *             comparison with internal channel parameter to be done
  *             using helper macro @ref __LL_ADC_CHANNEL_INTERNAL_TO_EXTERNAL().
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_SAMPLINGTIME_1CYCLE_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_2CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_6CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_11CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_23CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_46CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_246CYCLES_5
  *         @arg @ref LL_ADC_SAMPLINGTIME_1499CYCLES_5
  */
__STATIC_INLINE uint32_t LL_ADC_GetChannelSamplingTime(const ADC_TypeDef *ADCx, uint32_t Channel)
{
  const uint32_t iChannel = __ADC_CHANNEL_INDEX(Channel);
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->SMPR1,
                                                   ((ADC_CHANNEL_LUT[iChannel]
                                                     & ADC_CHANNEL_SMPRX_REGOFFSET_MASK) >> ADC_SMPRX_REGOFFSET_POS));
  return (uint32_t)(READ_BIT(*preg,
                             ADC_SMPR1_SMP0
                             << ((ADC_CHANNEL_LUT[iChannel]
                                  & ADC_CHANNEL_SMPx_BITOFFSET_MASK) >> ADC_CHANNEL_SMPx_BITOFFSET_POS))
                    >> ((ADC_CHANNEL_LUT[iChannel] & ADC_CHANNEL_SMPx_BITOFFSET_MASK)
                        >> ADC_CHANNEL_SMPx_BITOFFSET_POS)
                   );
}

/**
  * @brief  Set mode single-ended or differential input of the selected
  *         ADC channel.
  * @note   Channel ending is on channel scope: independently of channel mapped
  *         on ADC group regular or injected.
  *         In differential mode: Differential measurement is carried out
  *         between the selected channel (positive input) and another
  *         channel (negative input).
  *         Only selected channel has to be configured, the other channel
  *         is configured automatically.
  * @note   The selected channel and the other other channel have
  *         not necessarily contiguous channel number.
  *         To get the other channel number, refer to reference manual
  *         in section of ADC instance connectivity.
  * @note   Refer to Reference Manual to ensure the selected channel is
  *         available in differential mode.
  *         For example, internal channels (VrefInt, TempSensor, ...) are
  *         not available in differential mode.
  * @note   When configuring a channel 'inp' in differential mode,
  *         the channel 'inn' is not usable separately.
  * @note   Some channels are internally fixed to single-ended inputs
  *         configuration, refer to device datasheet for more details.
  * @note   For ADC channels configured in differential mode, both inputs
  *         should be biased at (Vref+)/2 +/-200mV.
  *         (Vref+ is the analog voltage reference)
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be ADC disabled.
  * @rmtoll DIFSEL   DIFSEL         LL_ADC_SetChannelSingleDiff
  * @param  ADCx ADC instance
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  * @param  SingleDiff This parameter can be a combination of the following values:
  *         @arg @ref LL_ADC_SINGLE_ENDED
  *         @arg @ref LL_ADC_DIFFERENTIAL_ENDED
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetChannelSingleDiff(ADC_TypeDef *ADCx, uint32_t Channel, uint32_t SingleDiff)
{
  /* Bits of channels in single or differential mode are set only for         */
  /* differential mode (for single mode, mask of bits allowed to be set is    */
  /* shifted out of range of bits of channels in single or differential mode. */
  const uint32_t iChannel = __ADC_CHANNEL_INDEX(Channel);
  MODIFY_REG(ADCx->DIFSEL,
             ADC_CHANNEL_LUT[iChannel] & ADC_SINGLEDIFF_CHANNEL_MASK,
             (ADC_CHANNEL_LUT[iChannel] & ADC_SINGLEDIFF_CHANNEL_MASK)
             & (ADC_DIFSEL_DIFSEL >> (SingleDiff & ADC_SINGLEDIFF_CHANNEL_SHIFT_MASK)));
}

/**
  * @brief  Get mode single-ended or differential input of the selected
  *         ADC channel.
  * @note   Channel ending is on channel scope: independently of channel mapped
  *         on ADC group regular or injected.
  *         In differential mode: Differential measurement is carried out
  *         between the selected channel (positive input) and another
  *         channel (negative input).
  *         Only selected channel has to be configured, the other channel
  *         is configured automatically.
  * @note   The selected channel and the other other channel have
  *         not necessarily contiguous channel number.
  *         To get the other channel number, refer to reference manual
  *         in section of ADC instance connectivity.
  * @note   Refer to Reference Manual to ensure the selected channel is
  *         available in differential mode.
  *         For example, internal channels (VrefInt, TempSensor, ...) are
  *         not available in differential mode.
  * @note   When configuring a channel 'i' in differential mode,
  *         the channel 'i+1' is not usable separately.
  * @note   Some channels are internally fixed to single-ended inputs
  *         configuration, refer to device datasheet for more details.
  * @rmtoll DIFSEL   DIFSEL         LL_ADC_GetChannelSingleDiff
  * @param  ADCx ADC instance
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_ADC_CHANNEL_1
  *         @arg @ref LL_ADC_CHANNEL_2
  *         @arg @ref LL_ADC_CHANNEL_3
  *         @arg @ref LL_ADC_CHANNEL_4
  *         @arg @ref LL_ADC_CHANNEL_5
  *         @arg @ref LL_ADC_CHANNEL_6
  *         @arg @ref LL_ADC_CHANNEL_7
  *         @arg @ref LL_ADC_CHANNEL_8
  *         @arg @ref LL_ADC_CHANNEL_9
  *         @arg @ref LL_ADC_CHANNEL_10
  *         @arg @ref LL_ADC_CHANNEL_11
  *         @arg @ref LL_ADC_CHANNEL_12
  *         @arg @ref LL_ADC_CHANNEL_13
  *         @arg @ref LL_ADC_CHANNEL_14
  *         @arg @ref LL_ADC_CHANNEL_15
  *         @arg @ref LL_ADC_CHANNEL_16
  *         @arg @ref LL_ADC_CHANNEL_17
  *         @arg @ref LL_ADC_CHANNEL_18
  *         @arg @ref LL_ADC_CHANNEL_19
  * @retval 0: channel in single-ended mode, else: channel in differential mode
  */
__STATIC_INLINE uint32_t LL_ADC_GetChannelSingleDiff(const ADC_TypeDef *ADCx, uint32_t Channel)
{
  const uint32_t iChannel = __ADC_CHANNEL_INDEX(Channel);
  return (uint32_t)(READ_BIT(ADCx->DIFSEL,
                             (ADC_CHANNEL_LUT[iChannel] & ADC_SINGLEDIFF_CHANNEL_MASK)));
}

/**
  * @}
  */

/** @defgroup ADC_LL_EF_Configuration_ADC_AnalogWatchdog Configuration of ADC transversal scope: analog watchdog
  * @{
  */

/**
  * @brief  Set ADC analog watchdog monitored channels:
  *         a single channel, multiple channels or all channels,
  *         on ADC groups regular and-or injected.
  * @note   Once monitored channels are selected, analog watchdog
  *         is enabled.
  * @note   In case of need to define a single channel to monitor
  *         with analog watchdog from sequencer channel definition,
  *         use helper macro @ref __LL_ADC_ANALOGWD_CHANNEL_GROUP().
  * @note   On this STM32 series, there are 2 kinds of analog watchdog
  *         instance:
  *         - AWD standard (instance AWD1):
  *           - channels monitored: can monitor 1 channel or all channels.
  *           - groups monitored: ADC groups regular and-or injected.
  *         - AWD flexible (instances AWD2, AWD3):
  *           - channels monitored: flexible on channels monitored, selection is
  *             channel wise, from from 1 to all channels.
  *             Specificity of this analog watchdog: Multiple channels can
  *             be selected. For example:
  *             (LL_ADC_AWD_CHANNEL4_REG_INJ | LL_ADC_AWD_CHANNEL5_REG_INJ | ...)
  *           - groups monitored: not selection possible (monitoring on both
  *             groups regular and injected).
  *             Channels selected are monitored on groups regular and injected:
  *             LL_ADC_AWD_CHANNELxx_REG_INJ (do not use parameters
  *             LL_ADC_AWD_CHANNELxx_REG and LL_ADC_AWD_CHANNELxx_INJ)
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll CFGR1    AWD1CH         LL_ADC_SetAnalogWDMonitChannels\n
  *         CFGR1    AWD1SGL        LL_ADC_SetAnalogWDMonitChannels\n
  *         CFGR1    AWD1EN         LL_ADC_SetAnalogWDMonitChannels\n
  *         CFGR1    JAWD1EN        LL_ADC_SetAnalogWDMonitChannels\n
  *         AWD2CR   AWD2CH         LL_ADC_SetAnalogWDMonitChannels\n
  *         AWD3CR   AWD3CH         LL_ADC_SetAnalogWDMonitChannels
  * @param  ADCx ADC instance
  * @param  AWDy This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD1
  *         @arg @ref LL_ADC_AWD2
  *         @arg @ref LL_ADC_AWD3
  * @param  AWDChannelGroup This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD_DISABLE
  *         @arg @ref LL_ADC_AWD_ALL_CHANNELS_REG        (0)
  *         @arg @ref LL_ADC_AWD_ALL_CHANNELS_INJ        (0)
  *         @arg @ref LL_ADC_AWD_ALL_CHANNELS_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_0_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_0_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_0_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_1_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_1_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_1_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_2_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_2_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_2_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_3_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_3_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_3_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_4_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_4_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_4_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_5_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_5_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_5_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_6_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_6_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_6_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_7_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_7_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_7_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_8_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_8_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_8_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_9_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_9_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_9_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_10_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_10_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_10_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_11_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_11_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_11_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_12_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_12_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_12_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_13_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_13_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_13_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_14_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_14_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_14_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_15_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_15_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_15_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_16_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_16_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_16_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_17_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_17_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_17_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_18_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_18_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_18_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_19_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_19_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_19_REG_INJ
  *         @arg @ref LL_ADC_AWD_CH_VREFINT_REG          (0)
  *         @arg @ref LL_ADC_AWD_CH_VREFINT_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CH_VREFINT_REG_INJ
  *         @arg @ref LL_ADC_AWD_CH_VBAT_REG             (0)(1)
  *         @arg @ref LL_ADC_AWD_CH_VBAT_INJ             (0)(1)
  *         @arg @ref LL_ADC_AWD_CH_VBAT_REG_INJ            (1)
  *         @arg @ref LL_ADC_AWD_CH_VDDCORE_REG          (0)(1)
  *         @arg @ref LL_ADC_AWD_CH_VDDCORE_INJ          (0)(1)
  *         @arg @ref LL_ADC_AWD_CH_VDDCORE_REG_INJ      (1)
  *
  *         (0) On this STM32 series, parameter available only on analog watchdog instance: AWD1.\n
  *         (1) On this STM32 series, parameter available only on ADC instance: ADC2, ADC3.
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetAnalogWDMonitChannels(ADC_TypeDef *ADCx, uint32_t AWDy, uint32_t AWDChannelGroup)
{
  /* Set bits with content of parameter "AWDChannelGroup" with bits position  */
  /* in register and register position depending on parameter "AWDy".         */
  /* Parameters "AWDChannelGroup" and "AWDy" are used with masks because      */
  /* containing other bits reserved for other purpose.                        */
  __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->CFGR1,
                                             ((AWDy & ADC_AWD_CRX_REGOFFSET_MASK) >> ADC_AWD_CRX_REGOFFSET_POS)
                                             + ((AWDy & ADC_AWD_CR12_REGOFFSETGAP_MASK)
                                                * ADC_AWD_CR12_REGOFFSETGAP_VAL));

  MODIFY_REG(*preg,
             (AWDy & ADC_AWD_CR_ALL_CHANNEL_MASK),
             AWDChannelGroup & AWDy);
}

/**
  * @brief  Get ADC analog watchdog monitored channel.
  * @note   Usage of the returned channel number:
  *         - To reinject this channel into another function LL_ADC_xxx:
  *           the returned channel number is only partly formatted on definition
  *           of literals LL_ADC_CHANNEL_x. Therefore, it has to be compared
  *           with parts of literals LL_ADC_CHANNEL_x or using
  *           helper macro @ref __LL_ADC_CHANNEL_TO_DECIMAL_NB().
  *           Then the selected literal LL_ADC_CHANNEL_x can be used
  *           as parameter for another function.
  *         - To get the channel number in decimal format:
  *           process the returned value with the helper macro
  *           @ref __LL_ADC_CHANNEL_TO_DECIMAL_NB().
  *           Applicable only when the analog watchdog is set to monitor
  *           one channel.
  * @note   On this STM32 series, there are 2 kinds of analog watchdog
  *         instance:
  *         - AWD standard (instance AWD1):
  *           - channels monitored: can monitor 1 channel or all channels.
  *           - groups monitored: ADC groups regular and-or injected.
  *         - AWD flexible (instances AWD2, AWD3):
  *           - channels monitored: flexible on channels monitored, selection is
  *             channel wise, from from 1 to all channels.
  *             Specificity of this analog watchdog: Multiple channels can
  *             be selected. For example:
  *             (LL_ADC_AWD_CHANNEL4_REG_INJ | LL_ADC_AWD_CHANNEL5_REG_INJ | ...)
  *           - groups monitored: not selection possible (monitoring on both
  *             groups regular and injected).
  *             Channels selected are monitored on groups regular and injected:
  *             LL_ADC_AWD_CHANNELxx_REG_INJ (do not use parameters
  *             LL_ADC_AWD_CHANNELxx_REG and LL_ADC_AWD_CHANNELxx_INJ)
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll CFGR1    AWD1CH         LL_ADC_GetAnalogWDMonitChannels\n
  *         CFGR1    AWD1SGL        LL_ADC_GetAnalogWDMonitChannels\n
  *         CFGR1    AWD1EN         LL_ADC_GetAnalogWDMonitChannels\n
  *         CFGR1    JAWD1EN        LL_ADC_GetAnalogWDMonitChannels\n
  *         AWD2CR   AWD2CH         LL_ADC_GetAnalogWDMonitChannels\n
  *         AWD3CR   AWD3CH         LL_ADC_GetAnalogWDMonitChannels
  * @param  ADCx ADC instance
  * @param  AWDy This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD1
  *         @arg @ref LL_ADC_AWD2 (1)
  *         @arg @ref LL_ADC_AWD3 (1)
  *
  *         (1) On this AWD number, monitored channel can be retrieved
  *             if only 1 channel is programmed (or none or all channels).
  *             This function cannot retrieve monitored channel if
  *             multiple channels are programmed simultaneously
  *             by bitfield.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_AWD_DISABLE
  *         @arg @ref LL_ADC_AWD_ALL_CHANNELS_REG        (0)
  *         @arg @ref LL_ADC_AWD_ALL_CHANNELS_INJ        (0)
  *         @arg @ref LL_ADC_AWD_ALL_CHANNELS_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_0_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_0_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_0_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_1_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_1_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_1_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_2_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_2_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_2_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_3_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_3_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_3_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_4_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_4_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_4_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_5_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_5_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_5_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_6_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_6_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_6_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_7_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_7_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_7_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_8_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_8_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_8_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_9_REG           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_9_INJ           (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_9_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_10_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_10_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_10_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_11_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_11_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_11_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_12_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_12_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_12_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_13_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_13_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_13_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_14_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_14_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_14_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_15_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_15_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_15_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_16_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_16_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_16_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_17_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_17_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_17_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_18_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_18_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_18_REG_INJ
  *         @arg @ref LL_ADC_AWD_CHANNEL_19_REG          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_19_INJ          (0)
  *         @arg @ref LL_ADC_AWD_CHANNEL_19_REG_INJ
  *
  *         (0) On this STM32 series, parameter available only on analog watchdog number: AWD1.
  */
__STATIC_INLINE uint32_t LL_ADC_GetAnalogWDMonitChannels(const ADC_TypeDef *ADCx, uint32_t AWDy)
{
  const __IO uint32_t *preg;

  if (AWDy == LL_ADC_AWD1)
  {
    /* Set pointer to register of selected analog watchdog */
    preg = __ADC_PTR_REG_OFFSET(ADCx->CFGR1, 0UL);
  }
  else
  {
    /* Set pointer to register of selected analog watchdog */
    preg = __ADC_PTR_REG_OFFSET(ADCx->AWD2CR, ((AWDy & ADC_AWD_CRX_REGOFFSET_MASK))                                   \
                                >> (ADC_AWD_CRX_REGOFFSET_POS + 1UL));
  }

  uint32_t analog_wd_monit_channels = (READ_BIT(*preg, AWDy) & AWDy & ADC_AWD_CR_ALL_CHANNEL_MASK);

  /* If "analog_wd_monit_channels" == 0, then the selected AWD is disabled       */
  /* (parameter value LL_ADC_AWD_DISABLE).                                    */
  /* Else, the selected AWD is enabled and is monitoring a group of channels  */
  /* or a single channel.                                                     */
  if (analog_wd_monit_channels != 0UL)
  {
    if (AWDy == LL_ADC_AWD1)
    {
      if ((analog_wd_monit_channels & ADC_CFGR1_AWD1SGL) == 0UL)
      {
        /* AWD monitoring a group of channels */
        analog_wd_monit_channels = ((analog_wd_monit_channels
                                     | (ADC_AWD_CR23_CHANNEL_MASK)
                                    )
                                    & (~(ADC_CFGR1_AWD1CH))
                                   );
      }
      else
      {
        /* AWD monitoring a single channel */
        analog_wd_monit_channels = (analog_wd_monit_channels
                                    | (ADC_AWD2CR_AWD2CH_0 << (analog_wd_monit_channels >> ADC_CFGR1_AWD1CH_Pos))
                                   );
      }
    }
    else
    {
      if ((analog_wd_monit_channels & ADC_AWD_CR23_CHANNEL_MASK) == ADC_AWD_CR23_CHANNEL_MASK)
      {
        /* AWD monitoring a group of channels */
        analog_wd_monit_channels = (ADC_AWD_CR23_CHANNEL_MASK
                                    | ((ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN))
                                   );
      }
      else
      {
        /* AWD monitoring a single channel */
        /* AWD monitoring a group of channels */
        analog_wd_monit_channels = (analog_wd_monit_channels
                                    | (ADC_CFGR1_JAWD1EN | ADC_CFGR1_AWD1EN | ADC_CFGR1_AWD1SGL)
                                    | (((uint32_t)(POSITION_VAL(analog_wd_monit_channels))) << ADC_CFGR1_AWD1CH_Pos)
                                   );
      }
    }
  }

  return analog_wd_monit_channels;
}

/**
  * @brief  Set ADC analog watchdog threshold value of threshold
  *         high or low.
  * @note   In case of ADC resolution different of 12 bits, specific threshold data formatting is required:
  *         - analog watchdog thresholds data must be aligned to left side (bit 11).
  *         - bits out of resolution range (LSB) must be set to value "0".
  *         To perform this formatting automatically, use helper
  *         macro @ref __LL_ADC_ANALOGWD_SET_THRESHOLD_RESOLUTION().
  * @note   On this STM32 series, there are 2 kinds of analog watchdog
  *         instance:
  *         - AWD standard (instance AWD1):
  *           - channels monitored: can monitor 1 channel or all channels.
  *           - groups monitored: ADC groups regular and-or injected.
  *           - resolution: resolution is not limited (corresponds to
  *             ADC resolution configured).
  *         - AWD flexible (instances AWD2, AWD3):
  *           - channels monitored: flexible on channels monitored, selection is
  *             channel wise, from from 1 to all channels.
  *             Specificity of this analog watchdog: Multiple channels can
  *             be selected. For example:
  *             (LL_ADC_AWD_CHANNEL4_REG_INJ | LL_ADC_AWD_CHANNEL5_REG_INJ | ...)
  *           - groups monitored: not selection possible (monitoring on both
  *             groups regular and injected).
  *             Channels selected are monitored on groups regular and injected:
  *             LL_ADC_AWD_CHANNELxx_REG_INJ (do not use parameters
  *             LL_ADC_AWD_CHANNELxx_REG and LL_ADC_AWD_CHANNELxx_INJ)
  * @note   If ADC oversampling is enabled, ADC analog watchdog thresholds are
  *         impacted: the comparison of analog watchdog thresholds is done
  *         on oversampling intermediate computation (after ratio, before shift
  *         application): intermediate register bitfield [32:7]
  *         (26 most significant bits).
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC can be disabled, enabled with or without conversion on going
  *         on either ADC groups regular or injected.
  * @rmtoll AWD1LTR  LTR            LL_ADC_SetAnalogWDThresholds\n
  *         AWD1HTR  HTR            LL_ADC_SetAnalogWDThresholds\n
  *         AWD2LTR  LTR            LL_ADC_SetAnalogWDThresholds\n
  *         AWD2HTR  HTR            LL_ADC_SetAnalogWDThresholds\n
  *         AWD3LTR  LTR            LL_ADC_SetAnalogWDThresholds\n
  *         AWD3HTR  HTR            LL_ADC_SetAnalogWDThresholds
  * @param  ADCx ADC instance
  * @param  AWDy This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD1
  *         @arg @ref LL_ADC_AWD2
  *         @arg @ref LL_ADC_AWD3
  * @param  AWDThresholdsHighLow This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD_THRESHOLD_HIGH
  *         @arg @ref LL_ADC_AWD_THRESHOLD_LOW
  * @param  AWDThresholdValue Value between Min_Data=0x000 and Max_Data=0xFFF
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDy, uint32_t AWDThresholdsHighLow,
                                                  uint32_t AWDThresholdValue)
{
  __IO uint32_t *preg;

  /* Set bits with content of parameter "AWDThresholdValue" with bits         */
  /* position in register and register position depending on parameters       */
  /* "AWDThresholdsHighLow" and "AWDy".                                       */
  /* Parameters "AWDy" and "AWDThresholdValue" are used with masks because    */
  /* containing other bits reserved for other purpose.                        */
  if (AWDy == LL_ADC_AWD1)
  {
    preg = __ADC_PTR_REG_OFFSET(ADCx->AWD1LTR, (AWDThresholdsHighLow));
  }
  else
  {
    preg = __ADC_PTR_REG_OFFSET(ADCx->AWD1LTR, (((AWDy & ADC_AWD_TRX_REGOFFSET_MASK)
                                                 >> ADC_AWD_TRX_REGOFFSET_POS) * 2UL)
                                + (AWDThresholdsHighLow));
  }

  MODIFY_REG(*preg, ADC_AWD1LTR_LTR, AWDThresholdValue);
}

/**
  * @brief  Get ADC analog watchdog threshold value of threshold high,
  *         threshold low or raw data with ADC thresholds high and low
  *         concatenated.
  * @note   In case of ADC resolution different of 12 bits, specific threshold data formatting is required:
  *         - analog watchdog thresholds data must be aligned to left side (bit 11).
  *         - bits out of resolution range (LSB) must be set to value "0".
  *         To perform this formatting automatically, use helper
  *         macro @ref __LL_ADC_ANALOGWD_SET_THRESHOLD_RESOLUTION().
  * @rmtoll AWD1LTR  LTR            LL_ADC_GetAnalogWDThresholds\n
  *         AWD1HTR  HTR            LL_ADC_GetAnalogWDThresholds\n
  *         AWD2LTR  LTR            LL_ADC_GetAnalogWDThresholds\n
  *         AWD2HTR  HTR            LL_ADC_GetAnalogWDThresholds\n
  *         AWD3LTR  LTR            LL_ADC_GetAnalogWDThresholds\n
  *         AWD3HTR  HTR            LL_ADC_GetAnalogWDThresholds
  * @param  ADCx ADC instance
  * @param  AWDy This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD1
  *         @arg @ref LL_ADC_AWD2
  *         @arg @ref LL_ADC_AWD3
  * @param  AWDThresholdsHighLow This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD_THRESHOLD_HIGH
  *         @arg @ref LL_ADC_AWD_THRESHOLD_LOW
  * @retval Value between Min_Data=0x000 and Max_Data=0xFFF
  */
__STATIC_INLINE uint32_t LL_ADC_GetAnalogWDThresholds(const ADC_TypeDef *ADCx,
                                                      uint32_t AWDy, uint32_t AWDThresholdsHighLow)
{
  const __IO uint32_t *preg;

  if (AWDy == LL_ADC_AWD1)
  {
    preg = __ADC_PTR_REG_OFFSET(ADCx->AWD1LTR, (AWDThresholdsHighLow));
  }
  else
  {
    preg = __ADC_PTR_REG_OFFSET(ADCx->AWD1LTR,
                                (((AWDy & ADC_AWD_TRX_REGOFFSET_MASK) >> ADC_AWD_TRX_REGOFFSET_POS) * 2UL)
                                + (AWDThresholdsHighLow));
  }

  return (uint32_t)(READ_BIT(*preg, ADC_AWD1LTR_LTR));
}

/**
  * @brief  Set ADC analog watchdog filtering configuration
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @note   On this STM32 series, this feature is only available on first
  *         analog watchdog (AWD1)
  * @rmtoll AWD1HTR  AWDFILT        LL_ADC_SetAWDFilteringConfiguration
  * @param  ADCx ADC instance
  * @param  AWDy This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD1
  * @param  FilteringConfig This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD_FILTERING_NONE
  *         @arg @ref LL_ADC_AWD_FILTERING_2SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_3SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_4SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_5SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_6SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_7SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_8SAMPLES
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetAWDFilteringConfiguration(ADC_TypeDef *ADCx, uint32_t AWDy, uint32_t FilteringConfig)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(AWDy);
  MODIFY_REG(ADCx->AWD1HTR, ADC_AWD1HTR_AWDFILT, FilteringConfig);
}

/**
  * @brief  Get ADC analog watchdog filtering configuration
  * @note   On this STM32 series, this feature is only available on first
  *         analog watchdog (AWD1)
  * @rmtoll AWD1HTR  AWDFILT        LL_ADC_GetAWDFilteringConfiguration
  * @param  ADCx ADC instance
  * @param  AWDy This parameter can be one of the following values:
  *         @arg @ref LL_ADC_AWD1
  * @retval Returned value can be:
  *         @arg @ref LL_ADC_AWD_FILTERING_NONE
  *         @arg @ref LL_ADC_AWD_FILTERING_2SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_3SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_4SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_5SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_6SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_7SAMPLES
  *         @arg @ref LL_ADC_AWD_FILTERING_8SAMPLES
  */
__STATIC_INLINE uint32_t LL_ADC_GetAWDFilteringConfiguration(const ADC_TypeDef *ADCx, uint32_t AWDy)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(AWDy);
  return (uint32_t)(READ_BIT(ADCx->AWD1HTR, ADC_AWD1HTR_AWDFILT));
}

/**
  * @}
  */

/** @defgroup ADC_LL_EF_Configuration_ADC_oversampling Configuration of ADC transversal scope: oversampling
  * @{
  */

/**
  * @brief  Set ADC oversampling scope: ADC groups regular and-or injected
  *         (availability of ADC group injected depends on STM32 series).
  * @note   If both groups regular and injected are selected,
  *         specify behavior of ADC group injected interrupting
  *         group regular: when ADC group injected is triggered,
  *         the oversampling on ADC group regular is either
  *         temporary stopped and continued, or resumed from start
  *         (oversampler buffer reset).
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll CFGR2    ROVSE          LL_ADC_SetOverSamplingScope\n
  *         CFGR2    JOVSE          LL_ADC_SetOverSamplingScope\n
  *         CFGR2    ROVSM          LL_ADC_SetOverSamplingScope
  * @param  ADCx ADC instance
  * @param  OvsScope This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OVS_DISABLE
  *         @arg @ref LL_ADC_OVS_GRP_REGULAR_CONTINUED
  *         @arg @ref LL_ADC_OVS_GRP_REGULAR_RESUMED
  *         @arg @ref LL_ADC_OVS_GRP_INJECTED
  *         @arg @ref LL_ADC_OVS_GRP_INJ_REG_RESUMED
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetOverSamplingScope(ADC_TypeDef *ADCx, uint32_t OvsScope)
{
  MODIFY_REG(ADCx->CFGR2, ADC_CFGR2_ROVSE | ADC_CFGR2_JOVSE | ADC_CFGR2_ROVSM, OvsScope);
}

/**
  * @brief  Get ADC oversampling scope: ADC groups regular and-or injected
  *         (availability of ADC group injected depends on STM32 series).
  * @note   If both groups regular and injected are selected,
  *         specify behavior of ADC group injected interrupting
  *         group regular: when ADC group injected is triggered,
  *         the oversampling on ADC group regular is either
  *         temporary stopped and continued, or resumed from start
  *         (oversampler buffer reset).
  * @rmtoll CFGR2    ROVSE          LL_ADC_GetOverSamplingScope\n
  *         CFGR2    JOVSE          LL_ADC_GetOverSamplingScope\n
  *         CFGR2    ROVSM          LL_ADC_GetOverSamplingScope
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_OVS_DISABLE
  *         @arg @ref LL_ADC_OVS_GRP_REGULAR_CONTINUED
  *         @arg @ref LL_ADC_OVS_GRP_REGULAR_RESUMED
  *         @arg @ref LL_ADC_OVS_GRP_INJECTED
  *         @arg @ref LL_ADC_OVS_GRP_INJ_REG_RESUMED
  */
__STATIC_INLINE uint32_t LL_ADC_GetOverSamplingScope(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR2, ADC_CFGR2_ROVSE | ADC_CFGR2_JOVSE | ADC_CFGR2_ROVSM));
}

/**
  * @brief  Set ADC oversampling discontinuous mode (triggered mode)
  *         on the selected ADC group.
  * @note   Number of oversampled conversions are done either in:
  *         - continuous mode (all conversions of oversampling ratio
  *           are done from 1 trigger)
  *         - discontinuous mode (each conversion of oversampling ratio
  *           needs a trigger)
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on group regular.
  * @note   On this STM32 series, oversampling discontinuous mode
  *         (triggered mode) can be used only when oversampling is
  *         set on group regular only and in resumed mode.
  * @rmtoll CFGR2    TROVS          LL_ADC_SetOverSamplingDiscont
  * @param  ADCx ADC instance
  * @param  OverSamplingDiscont This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OVS_REG_CONT
  *         @arg @ref LL_ADC_OVS_REG_DISCONT
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetOverSamplingDiscont(ADC_TypeDef *ADCx, uint32_t OverSamplingDiscont)
{
  MODIFY_REG(ADCx->CFGR2, ADC_CFGR2_TROVS, OverSamplingDiscont);
}

/**
  * @brief  Get ADC oversampling discontinuous mode (triggered mode)
  *         on the selected ADC group.
  * @note   Number of oversampled conversions are done either in:
  *         - continuous mode (all conversions of oversampling ratio
  *           are done from 1 trigger)
  *         - discontinuous mode (each conversion of oversampling ratio
  *           needs a trigger)
  * @rmtoll CFGR2    TROVS          LL_ADC_GetOverSamplingDiscont
  * @param  ADCx ADC instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_OVS_REG_CONT
  *         @arg @ref LL_ADC_OVS_REG_DISCONT
  */
__STATIC_INLINE uint32_t LL_ADC_GetOverSamplingDiscont(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR2, ADC_CFGR2_TROVS));
}

/**
  * @brief  Set ADC oversampling
  *         (impacting both ADC groups regular and injected)
  * @note   This function set the 2 items of oversampling configuration:
  *         - ratio
  *         - shift
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be disabled or enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll CFGR2    OVSS           LL_ADC_ConfigOverSamplingRatioShift\n
  *         CFGR2    OVSR           LL_ADC_ConfigOverSamplingRatioShift
  * @param  ADCx ADC instance
  * @param  Ratio This parameter can be in the range from 1 to 1024
  * @param  Shift This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OVS_SHIFT_NONE
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_1
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_2
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_3
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_4
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_5
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_6
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_7
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_8
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_9
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_10
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ConfigOverSamplingRatioShift(ADC_TypeDef *ADCx, uint32_t Ratio, uint32_t Shift)
{
  MODIFY_REG(ADCx->CFGR2, (ADC_CFGR2_OVSS | ADC_CFGR2_OVSR), (Shift | (((Ratio - 1UL) << ADC_CFGR2_OVSR_Pos))));
}

/**
  * @brief  Get ADC oversampling ratio
  *        (impacting both ADC groups regular and injected)
  * @rmtoll CFGR2    OVSR           LL_ADC_GetOverSamplingRatio
  * @param  ADCx ADC instance
  * @retval Ratio This parameter can be in the from 1 to 1024.
  */
__STATIC_INLINE uint32_t LL_ADC_GetOverSamplingRatio(const ADC_TypeDef *ADCx)
{
  return (((uint32_t)(READ_BIT(ADCx->CFGR2, ADC_CFGR2_OVSR)) + (1UL << ADC_CFGR2_OVSR_Pos)) >> ADC_CFGR2_OVSR_Pos);
}

/**
  * @brief  Get ADC oversampling shift
  *        (impacting both ADC groups regular and injected)
  * @rmtoll CFGR2    OVSS           LL_ADC_GetOverSamplingShift
  * @param  ADCx ADC instance
  * @retval Shift This parameter can be one of the following values:
  *         @arg @ref LL_ADC_OVS_SHIFT_NONE
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_1
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_2
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_3
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_4
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_5
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_6
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_7
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_8
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_9
  *         @arg @ref LL_ADC_OVS_SHIFT_RIGHT_10
  */
__STATIC_INLINE uint32_t LL_ADC_GetOverSamplingShift(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->CFGR2, ADC_CFGR2_OVSS));
}

/**
  * @}
  */

/** @defgroup ADC_LL_EF_Configuration_ADC_Multimode Configuration of ADC hierarchical scope: multimode
  * @{
  */

#if defined(ADC_MULTIMODE_SUPPORT)
/**
  * @brief  Set ADC multimode configuration to operate in independent mode
  *         or multimode (for devices with several ADC instances).
  * @note   If multimode configuration: the selected ADC instance is
  *         either master or slave depending on hardware.
  *         Refer to reference manual.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         All ADC instances of the ADC common group must be disabled.
  *         This check can be done with function @ref LL_ADC_IsEnabled() for each
  *         ADC instance or by using helper macro
  *         @ref __LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE().
  * @rmtoll CCR      DUAL           LL_ADC_SetMultimode
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @param  Multimode This parameter can be one of the following values:
  *         @arg @ref LL_ADC_MULTI_INDEPENDENT
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_SIMULT
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_INTERL
  *         @arg @ref LL_ADC_MULTI_DUAL_INJ_SIMULT
  *         @arg @ref LL_ADC_MULTI_DUAL_INJ_ALTERN
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_SIM_INJ_SIM
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_SIM_INJ_ALT
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_INT_INJ_SIM
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetMultimode(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t Multimode)
{
  MODIFY_REG(ADCxy_COMMON->CCR, ADC_CCR_DUAL, Multimode);
}

/**
  * @brief  Get ADC multimode configuration to operate in independent mode
  *         or multimode (for devices with several ADC instances).
  * @note   If multimode configuration: the selected ADC instance is
  *         either master or slave depending on hardware.
  *         Refer to reference manual.
  * @rmtoll CCR      DUAL           LL_ADC_GetMultimode
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_MULTI_INDEPENDENT
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_SIMULT
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_INTERL
  *         @arg @ref LL_ADC_MULTI_DUAL_INJ_SIMULT
  *         @arg @ref LL_ADC_MULTI_DUAL_INJ_ALTERN
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_SIM_INJ_SIM
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_SIM_INJ_ALT
  *         @arg @ref LL_ADC_MULTI_DUAL_REG_INT_INJ_SIM
  */
__STATIC_INLINE uint32_t LL_ADC_GetMultimode(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(READ_BIT(ADCxy_COMMON->CCR, ADC_CCR_DUAL));
}

/**
  * @brief  Set ADC multimode conversion data format: conversion data in data
  *         register of each ADC instance or ADC common instance.
  * @note   If ADC data transfer by DMA is used: data register as DMA source
  *         and number of DMA requests is impacted, refer to description
  *         of parameters of this function.
  * @note   How to retrieve multimode conversion data:
  *         For multimode data transfer setting 16 bit resolution or less,
  *         using functions @ref LL_ADC_REG_ReadMultiConvPacking() or @ref LL_ADC_REG_ReadMultiConvNoPacking().
  *         Conversion data is a raw data with ADC master and slave
  *         concatenated.
  *         A macro is available to get the conversion data of
  *         ADC master or ADC slave: see helper macro
  *         @ref __LL_ADC_MULTI_CONV_DATA_MASTER_SLAVE().
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         All ADC instances of the ADC common group must be disabled
  *         or enabled without conversion on going on group regular.
  * @rmtoll CCR      DAMDF          LL_ADC_SetMultiDataFormat
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @param  MultiDataFormat This parameter can be one of the following values:
  *         @arg @ref LL_ADC_MULTI_REG_DATA_EACH_ADC
  *         @arg @ref LL_ADC_MULTI_REG_DATA_COMMON_32B
  *         @arg @ref LL_ADC_MULTI_REG_DATA_COMMON_16B
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetMultiDataFormat(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiDataFormat)
{
  MODIFY_REG(ADCxy_COMMON->CCR, ADC_CCR_DAMDF, MultiDataFormat);
}

/**
  * @brief  Get ADC multimode conversion data format: conversion data in data
  *         register of each ADC instance or ADC common instance.
  * @note   If ADC data transfer by DMA is used: data register as DMA source
  *         and number of DMA requests is impacted, refer to description
  *         of parameters of this function.
  * @note   How to retrieve multimode conversion data:
  *         For multimode data transfer setting 16 bit resolution or less,
  *         using functions @ref LL_ADC_REG_ReadMultiConvPacking() or @ref LL_ADC_REG_ReadMultiConvNoPacking().
  *         Conversion data is a raw data with ADC master and slave
  *         concatenated.
  *         A macro is available to get the conversion data of
  *         ADC master or ADC slave: see helper macro
  *         @ref __LL_ADC_MULTI_CONV_DATA_MASTER_SLAVE().
  * @rmtoll CCR      DAMDF          LL_ADC_GetMultiDataFormat
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_MULTI_REG_DATA_EACH_ADC
  *         @arg @ref LL_ADC_MULTI_REG_DATA_COMMON_32B
  *         @arg @ref LL_ADC_MULTI_REG_DATA_COMMON_16B
  */
__STATIC_INLINE uint32_t LL_ADC_GetMultiDataFormat(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(READ_BIT(ADCxy_COMMON->CCR, ADC_CCR_DAMDF));
}

/**
  * @brief  Set ADC multimode delay between 2 sampling phases.
  * @note   The sampling delay range depends on ADC resolution:
  *         - ADC resolution 12 bits can have maximum delay of 12 cycles.
  *         - ADC resolution 10 bits can have maximum delay of 10 cycles.
  *         - ADC resolution  8 bits can have maximum delay of  8 cycles.
  *         - ADC resolution  6 bits can have maximum delay of  6 cycles.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         All ADC instances of the ADC common group must be disabled.
  *         This check can be done with function @ref LL_ADC_IsEnabled() for each
  *         ADC instance or by using helper macro helper macro
  *         @ref __LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE().
  * @rmtoll CCR      DELAY          LL_ADC_SetMultiTwoSamplingDelay
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @param  MultiTwoSamplingDelay This parameter can be one of the following values:
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_2CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_3CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_4CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_5CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_6CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_7CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_8CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_9CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_10CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_11CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_12CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_13CYCLES
  * @retval None
  */
__STATIC_INLINE void LL_ADC_SetMultiTwoSamplingDelay(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiTwoSamplingDelay)
{
  MODIFY_REG(ADCxy_COMMON->CCR, ADC_CCR_DELAY, MultiTwoSamplingDelay);
}

/**
  * @brief  Get ADC multimode delay between 2 sampling phases.
  * @rmtoll CCR      DELAY          LL_ADC_GetMultiTwoSamplingDelay
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_2CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_3CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_4CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_5CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_6CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_7CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_8CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_9CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_10CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_11CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_12CYCLES
  *         @arg @ref LL_ADC_MULTI_TWOSMP_DELAY_13CYCLES
  */
__STATIC_INLINE uint32_t LL_ADC_GetMultiTwoSamplingDelay(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(READ_BIT(ADCxy_COMMON->CCR, ADC_CCR_DELAY));
}
#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @}
  */
/** @defgroup ADC_LL_EF_Operation_ADC_Instance Operation on ADC hierarchical scope: ADC instance
  * @{
  */

/**
  * @brief  Put ADC instance in deep power down state.
  * @note   In case of ADC calibration necessary: When ADC is in deep-power-down
  *         state, the internal analog calibration is lost. After exiting from
  *         deep power down, calibration must be relaunched or calibration factor
  *         (preliminarily saved) must be set back into calibration register.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be ADC disabled.
  * @rmtoll CR       DEEPPWD        LL_ADC_EnableDeepPowerDown
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableDeepPowerDown(ADC_TypeDef *ADCx)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  MODIFY_REG(ADCx->CR,
             ADC_CR_BITS_PROPERTY_RS,
             ADC_CR_DEEPPWD);
}

/**
  * @brief  Disable ADC deep power down mode.
  * @note   In case of ADC calibration necessary: When ADC is in deep-power-down
  *         state, the internal analog calibration is lost. After exiting from
  *         deep power down, calibration must be relaunched or calibration factor
  *         (preliminarily saved) must be set back into calibration register.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be ADC disabled.
  * @rmtoll CR       DEEPPWD        LL_ADC_DisableDeepPowerDown
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableDeepPowerDown(ADC_TypeDef *ADCx)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  CLEAR_BIT(ADCx->CR, (ADC_CR_DEEPPWD | ADC_CR_BITS_PROPERTY_RS));
}

/**
  * @brief  Get the selected ADC instance deep power down state.
  * @rmtoll CR       DEEPPWD        LL_ADC_IsDeepPowerDownEnabled
  * @param  ADCx ADC instance
  * @retval 0: deep power down is disabled, 1: deep power down is enabled.
  */
__STATIC_INLINE uint32_t LL_ADC_IsDeepPowerDownEnabled(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CR, ADC_CR_DEEPPWD) == (ADC_CR_DEEPPWD)) ? 1UL : 0UL);
}


/**
  * @brief  Enable the selected ADC instance.
  * @note   On this STM32 series, after ADC enable, a delay for
  *         ADC internal analog stabilization is required before performing a
  *         ADC conversion start.
  *         Refer to device datasheet, parameter tSTAB.
  * @note   On this STM32 series, flag LL_ADC_FLAG_ADRDY is raised when the ADC
  *         is enabled and when conversion clock is active.
  *         (not only core clock: this ADC has a dual clock domain)
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be ADC disabled and ADC internal voltage regulator enabled.
  * @rmtoll CR       ADEN           LL_ADC_Enable
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_Enable(ADC_TypeDef *ADCx)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  MODIFY_REG(ADCx->CR,
             ADC_CR_BITS_PROPERTY_RS,
             ADC_CR_ADEN);
}

/**
  * @brief  Disable the selected ADC instance.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be not disabled. Must be enabled without conversion on going
  *         on either groups regular or injected.
  * @rmtoll CR       ADDIS          LL_ADC_Disable
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_Disable(ADC_TypeDef *ADCx)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  MODIFY_REG(ADCx->CR,
             ADC_CR_BITS_PROPERTY_RS,
             ADC_CR_ADDIS);
}

/**
  * @brief  Get the selected ADC instance enable state.
  * @note   On this STM32 series, flag LL_ADC_FLAG_ADRDY is raised when the ADC
  *         is enabled and when conversion clock is active.
  *         (not only core clock: this ADC has a dual clock domain)
  * @rmtoll CR       ADEN           LL_ADC_IsEnabled
  * @param  ADCx ADC instance
  * @retval 0: ADC is disabled, 1: ADC is enabled.
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabled(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CR, ADC_CR_ADEN) == (ADC_CR_ADEN)) ? 1UL : 0UL);
}

/**
  * @brief  Get the selected ADC instance disable state.
  * @rmtoll CR       ADDIS          LL_ADC_IsDisableOngoing
  * @param  ADCx ADC instance
  * @retval 0: no ADC disable command on going.
  */
__STATIC_INLINE uint32_t LL_ADC_IsDisableOngoing(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CR, ADC_CR_ADDIS) == (ADC_CR_ADDIS)) ? 1UL : 0UL);
}

/**
  * @brief  Start ADC calibration in the mode single-ended
  *         or differential (for devices with differential mode available).
  * @note   On this STM32 series, a minimum number of ADC clock cycles
  *         are required between ADC end of calibration and ADC enable.
  *         Refer to literal @ref LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES.
  * @note   For devices with differential mode available:
  *         Calibration of offset is specific to each of
  *         single-ended and differential modes
  *         (calibration run must be performed for each of these
  *         differential modes, if used afterwards and if the application
  *         requires their calibration).
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be ADC disabled.
  * @rmtoll CR       ADCAL          LL_ADC_StartCalibration\n
  *         CR       ADCALDIF       LL_ADC_StartCalibration
  * @param  ADCx ADC instance
  * @param  SingleDiff This parameter can be one of the following values:
  *         @arg @ref LL_ADC_SINGLE_ENDED
  *         @arg @ref LL_ADC_DIFFERENTIAL_ENDED
  * @retval None
  */
__STATIC_INLINE void LL_ADC_StartCalibration(ADC_TypeDef *ADCx, uint32_t SingleDiff)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  MODIFY_REG(ADCx->CR,
             ADC_CR_ADCALDIF | ADC_CR_BITS_PROPERTY_RS,
             ADC_CR_ADCAL | (SingleDiff & ADC_SINGLEDIFF_CALIB_START_MASK));
}

/**
  * @brief  Get ADC calibration state.
  * @rmtoll CR       ADCAL          LL_ADC_IsCalibrationOnGoing
  * @param  ADCx ADC instance
  * @retval 0: calibration complete, 1: calibration in progress.
  */
__STATIC_INLINE uint32_t LL_ADC_IsCalibrationOnGoing(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CR, ADC_CR_ADCAL) == (ADC_CR_ADCAL)) ? 1UL : 0UL);
}

/**
  * @brief  Stop ADC calibration
  * @note   On this STM32 series, a minimum number of ADC clock cycles
  *         are required between ADC end of calibration and ADC enable.
  *         Refer to literal @ref LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES.
  * @rmtoll CR       ADCAL          LL_ADC_StopCalibration\n
  *         CR       ADCALDIF       LL_ADC_StopCalibration
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_StopCalibration(ADC_TypeDef *ADCx)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  CLEAR_BIT(ADCx->CR, ADC_CR_ADCALDIF | ADC_CR_ADCAL | ADC_CR_BITS_PROPERTY_RS);
}

/**
  * @brief  Enable calibration additional offset
  * @rmtoll CALFACT       CALADDOS       LL_ADC_EnableCalibrationOffset
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableCalibrationOffset(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->CALFACT, ADC_CALFACT_CALADDOS);
}

/**
  * @brief  Disable calibration additional offset
  * @rmtoll CALFACT       CALADDOS       LL_ADC_DisableCalibrationOffset
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableCalibrationOffset(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->CALFACT, ADC_CALFACT_CALADDOS);
}

/**
  * @brief  Get calibration additional offset state.
  * @rmtoll CALFACT       CALADDOS          LL_ADC_IsCalibrationOnGoing
  * @param  ADCx ADC instance
  * @retval 0: calibration offset disabled, 1: calibration offset enabled.
  */
__STATIC_INLINE uint32_t LL_ADC_IsCalibrationOffsetEnabled(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CALFACT, ADC_CALFACT_CALADDOS) == (ADC_CALFACT_CALADDOS)) ? 1UL : 0UL);
}
/**
  * @}
  */

/** @defgroup ADC_LL_EF_Operation_ADC_Group_Regular Operation on ADC hierarchical scope: group regular
  * @{
  */

/**
  * @brief  Start ADC group regular conversion.
  * @note   On this STM32 series, this function is relevant for both
  *         internal trigger (SW start) and external trigger:
  *         - If ADC trigger has been set to software start, ADC conversion
  *           starts immediately.
  *         - If ADC trigger has been set to external trigger, ADC conversion
  *           will start at next trigger event (on the selected trigger edge)
  *           following the ADC start conversion command.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be enabled without conversion on going on group regular,
  *         without conversion stop command on going on group regular,
  *         without ADC disable command on going.
  * @rmtoll CR       ADSTART        LL_ADC_REG_StartConversion
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_StartConversion(ADC_TypeDef *ADCx)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  MODIFY_REG(ADCx->CR,
             ADC_CR_BITS_PROPERTY_RS,
             ADC_CR_ADSTART);
}

/**
  * @brief  Stop ADC group regular conversion.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be enabled with conversion on going on group regular,
  *         without ADC disable command on going.
  * @rmtoll CR       ADSTP          LL_ADC_REG_StopConversion
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_REG_StopConversion(ADC_TypeDef *ADCx)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  MODIFY_REG(ADCx->CR,
             ADC_CR_BITS_PROPERTY_RS,
             ADC_CR_ADSTP);
}

/**
  * @brief  Get ADC group regular conversion state.
  * @rmtoll CR       ADSTART        LL_ADC_REG_IsConversionOngoing
  * @param  ADCx ADC instance
  * @retval 0: no conversion is on going on ADC group regular.
  */
__STATIC_INLINE uint32_t LL_ADC_REG_IsConversionOngoing(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CR, ADC_CR_ADSTART) == (ADC_CR_ADSTART)) ? 1UL : 0UL);
}

/**
  * @brief  Get ADC group regular command of conversion stop state
  * @rmtoll CR       ADSTP          LL_ADC_REG_IsStopConversionOngoing
  * @param  ADCx ADC instance
  * @retval 0: no command of conversion stop is on going on ADC group regular.
  */
__STATIC_INLINE uint32_t LL_ADC_REG_IsStopConversionOngoing(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CR, ADC_CR_ADSTP) == (ADC_CR_ADSTP)) ? 1UL : 0UL);
}

/**
  * @brief  Get ADC group regular conversion data, range fit for
  *         all ADC configurations: all ADC resolutions and
  *         all oversampling increased data width (for devices
  *         with feature oversampling).
  * @rmtoll DR       RDATA          LL_ADC_REG_ReadConversionData32
  * @param  ADCx ADC instance
  * @retval Value between Min_Data=0x00000000 and Max_Data=0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_ADC_REG_ReadConversionData32(const ADC_TypeDef *ADCx)
{
  return (uint32_t)(READ_BIT(ADCx->DR, ADC_DR_RDATA));
}

/**
  * @brief  Get ADC group regular conversion data, range fit for
  *         ADC resolution 12 bits.
  * @note   For devices with feature oversampling: Oversampling
  *         can increase data width, function for extended range
  *         may be needed: @ref LL_ADC_REG_ReadConversionData32.
  * @rmtoll DR       RDATA          LL_ADC_REG_ReadConversionData12
  * @param  ADCx ADC instance
  * @retval Value between Min_Data=0x000 and Max_Data=0xFFF
  */
__STATIC_INLINE uint16_t LL_ADC_REG_ReadConversionData12(const ADC_TypeDef *ADCx)
{
  return (uint16_t)(READ_BIT(ADCx->DR, ADC_DR_RDATA));
}

/**
  * @brief  Get ADC group regular conversion data, range fit for
  *         ADC resolution 10 bits.
  * @note   For devices with feature oversampling: Oversampling
  *         can increase data width, function for extended range
  *         may be needed: @ref LL_ADC_REG_ReadConversionData32.
  * @rmtoll DR       RDATA          LL_ADC_REG_ReadConversionData10
  * @param  ADCx ADC instance
  * @retval Value between Min_Data=0x000 and Max_Data=0x3FF
  */
__STATIC_INLINE uint16_t LL_ADC_REG_ReadConversionData10(const ADC_TypeDef *ADCx)
{
  return (uint16_t)(READ_BIT(ADCx->DR, ADC_DR_RDATA));
}

/**
  * @brief  Get ADC group regular conversion data, range fit for
  *         ADC resolution 8 bits.
  * @note   For devices with feature oversampling: Oversampling
  *         can increase data width, function for extended range
  *         may be needed: @ref LL_ADC_REG_ReadConversionData32.
  * @rmtoll DR       RDATA          LL_ADC_REG_ReadConversionData8
  * @param  ADCx ADC instance
  * @retval Value between Min_Data=0x00 and Max_Data=0xFF
  */
__STATIC_INLINE uint8_t LL_ADC_REG_ReadConversionData8(const ADC_TypeDef *ADCx)
{
  return (uint8_t)(READ_BIT(ADCx->DR, ADC_DR_RDATA));
}

/**
  * @brief  Get ADC group regular conversion data, range fit for
  *         ADC resolution 6 bits.
  * @note   For devices with feature oversampling: Oversampling
  *         can increase data width, function for extended range
  *         may be needed: @ref LL_ADC_REG_ReadConversionData32.
  * @rmtoll DR       RDATA          LL_ADC_REG_ReadConversionData6
  * @param  ADCx ADC instance
  * @retval Value between Min_Data=0x00 and Max_Data=0x3F
  */
__STATIC_INLINE uint8_t LL_ADC_REG_ReadConversionData6(const ADC_TypeDef *ADCx)
{
  return (uint8_t)(READ_BIT(ADCx->DR, ADC_DR_RDATA));
}

#if defined(ADC_MULTIMODE_SUPPORT)
/**
  * @brief  Get ADC multimode conversion data of ADC master and ADC slave
  *         concatenated.
  * @note   Multimode data concatenation is usable under conditions of
  *         ADC multimode data format selected and data width,
  *         refer to description of literals in @ref ADC_LL_EC_MULTI_DATA_FORMAT.
  * @note   From raw data with ADC master and slave concatenated,
  *         a macro is available to get the conversion data of
  *         ADC master or ADC slave: see helper macro
  *         @ref __LL_ADC_MULTI_CONV_DATA_MASTER_SLAVE().
  * @rmtoll CDR      RDATA_MST      LL_ADC_REG_ReadMultiConvPacking\n
  *         CDR      RDATA_SLV      LL_ADC_REG_ReadMultiConvPacking
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval Value between Min_Data=0x00000000 and Max_Data=0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_ADC_REG_ReadMultiConvPacking(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(READ_BIT(ADCxy_COMMON->CDR, (ADC_CDR_RDATA_SLV | ADC_CDR_RDATA_MST)));
}

/**
  * @brief  Get ADC multimode conversion data of ADC master or ADC slave
  *         (data set alternatively in specific data register).
  * @note   Multimode data without concatenation is compliant with all
  *         ADC multimode data format and data width,
  *         refer to description of literals in @ref ADC_LL_EC_MULTI_DATA_FORMAT.
  * @note   Multimode data without packing is using a unique data register
  *         (CDR2) for data of ADC master and slave alternatively.
  *         Therefore, data can be overwritten very quickly, recommended
  *         method to retrieve data without risk to miss some occurrences
  *         is to use DMA transfer.
  * @rmtoll CDR2     RDATA_ALT      LL_ADC_REG_ReadMultiConvNoPacking
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval Value between Min_Data=0x00000000 and Max_Data=0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_ADC_REG_ReadMultiConvNoPacking(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(READ_BIT(ADCxy_COMMON->CDR2, ADC_CDR2_RDATA_ALT));
}
#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @}
  */

/** @defgroup ADC_LL_EF_Operation_ADC_Group_Injected Operation on ADC hierarchical scope: group injected
  * @{
  */

/**
  * @brief  Start ADC group injected conversion.
  * @note   On this STM32 series, this function is relevant for both
  *         internal trigger (SW start) and external trigger:
  *         - If ADC trigger has been set to software start, ADC conversion
  *           starts immediately.
  *         - If ADC trigger has been set to external trigger, ADC conversion
  *           will start at next trigger event (on the selected trigger edge)
  *           following the ADC start conversion command.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be enabled without conversion on going on group injected,
  *         without conversion stop command on going on group injected,
  *         without ADC disable command on going.
  * @rmtoll CR       JADSTART       LL_ADC_INJ_StartConversion
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_INJ_StartConversion(ADC_TypeDef *ADCx)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  MODIFY_REG(ADCx->CR,
             ADC_CR_BITS_PROPERTY_RS,
             ADC_CR_JADSTART);
}

/**
  * @brief  Stop ADC group injected conversion.
  * @note   On this STM32 series, setting of this feature is conditioned to
  *         ADC state:
  *         ADC must be enabled with conversion on going on group injected,
  *         without ADC disable command on going.
  * @rmtoll CR       JADSTP         LL_ADC_INJ_StopConversion
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_INJ_StopConversion(ADC_TypeDef *ADCx)
{
  /* Note: Write register with some additional bits forced to state reset     */
  /*       instead of modifying only the selected bit for this function,      */
  /*       to not interfere with bits with HW property "rs".                  */
  MODIFY_REG(ADCx->CR,
             ADC_CR_BITS_PROPERTY_RS,
             ADC_CR_JADSTP);
}

/**
  * @brief  Get ADC group injected conversion state.
  * @rmtoll CR       JADSTART       LL_ADC_INJ_IsConversionOngoing
  * @param  ADCx ADC instance
  * @retval 0: no conversion is on going on ADC group injected.
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_IsConversionOngoing(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CR, ADC_CR_JADSTART) == (ADC_CR_JADSTART)) ? 1UL : 0UL);
}

/**
  * @brief  Get ADC group injected command of conversion stop state
  * @rmtoll CR       JADSTP         LL_ADC_INJ_IsStopConversionOngoing
  * @param  ADCx ADC instance
  * @retval 0: no command of conversion stop is on going on ADC group injected.
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_IsStopConversionOngoing(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->CR, ADC_CR_JADSTP) == (ADC_CR_JADSTP)) ? 1UL : 0UL);
}

/**
  * @brief  Get ADC group injected conversion data, range fit for
  *         all ADC configurations: all ADC resolutions and
  *         all oversampling increased data width (for devices
  *         with feature oversampling).
  * @rmtoll JDR1     JDATA          LL_ADC_INJ_ReadConversionData32\n
  *         JDR2     JDATA          LL_ADC_INJ_ReadConversionData32\n
  *         JDR3     JDATA          LL_ADC_INJ_ReadConversionData32\n
  *         JDR4     JDATA          LL_ADC_INJ_ReadConversionData32
  * @param  ADCx ADC instance
  * @param  Rank This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_RANK_1
  *         @arg @ref LL_ADC_INJ_RANK_2
  *         @arg @ref LL_ADC_INJ_RANK_3
  *         @arg @ref LL_ADC_INJ_RANK_4
  * @retval Value between Min_Data=0x00000000 and Max_Data=0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_ADC_INJ_ReadConversionData32(const ADC_TypeDef *ADCx, uint32_t Rank)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->JDR1,
                                                   ((Rank & ADC_INJ_JDRX_REGOFFSET_MASK) >> ADC_JDRX_REGOFFSET_POS));

  return (uint32_t)(READ_BIT(*preg,
                             ADC_JDR1_JDATA)
                   );
}

/**
  * @brief  Get ADC group injected conversion data, range fit for
  *         ADC resolution 12 bits.
  * @note   For devices with feature oversampling: Oversampling
  *         can increase data width, function for extended range
  *         may be needed: @ref LL_ADC_INJ_ReadConversionData32.
  * @rmtoll JDR1     JDATA          LL_ADC_INJ_ReadConversionData12\n
  *         JDR2     JDATA          LL_ADC_INJ_ReadConversionData12\n
  *         JDR3     JDATA          LL_ADC_INJ_ReadConversionData12\n
  *         JDR4     JDATA          LL_ADC_INJ_ReadConversionData12
  * @param  ADCx ADC instance
  * @param  Rank This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_RANK_1
  *         @arg @ref LL_ADC_INJ_RANK_2
  *         @arg @ref LL_ADC_INJ_RANK_3
  *         @arg @ref LL_ADC_INJ_RANK_4
  * @retval Value between Min_Data=0x000 and Max_Data=0xFFF
  */
__STATIC_INLINE uint16_t LL_ADC_INJ_ReadConversionData12(const ADC_TypeDef *ADCx, uint32_t Rank)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->JDR1,
                                                   ((Rank & ADC_INJ_JDRX_REGOFFSET_MASK) >> ADC_JDRX_REGOFFSET_POS));

  return (uint16_t)(READ_BIT(*preg,
                             ADC_JDR1_JDATA)
                   );
}

/**
  * @brief  Get ADC group injected conversion data, range fit for
  *         ADC resolution 10 bits.
  * @note   For devices with feature oversampling: Oversampling
  *         can increase data width, function for extended range
  *         may be needed: @ref LL_ADC_INJ_ReadConversionData32.
  * @rmtoll JDR1     JDATA          LL_ADC_INJ_ReadConversionData10\n
  *         JDR2     JDATA          LL_ADC_INJ_ReadConversionData10\n
  *         JDR3     JDATA          LL_ADC_INJ_ReadConversionData10\n
  *         JDR4     JDATA          LL_ADC_INJ_ReadConversionData10
  * @param  ADCx ADC instance
  * @param  Rank This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_RANK_1
  *         @arg @ref LL_ADC_INJ_RANK_2
  *         @arg @ref LL_ADC_INJ_RANK_3
  *         @arg @ref LL_ADC_INJ_RANK_4
  * @retval Value between Min_Data=0x000 and Max_Data=0x3FF
  */
__STATIC_INLINE uint16_t LL_ADC_INJ_ReadConversionData10(const ADC_TypeDef *ADCx, uint32_t Rank)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->JDR1,
                                                   ((Rank & ADC_INJ_JDRX_REGOFFSET_MASK) >> ADC_JDRX_REGOFFSET_POS));

  return (uint16_t)(READ_BIT(*preg,
                             ADC_JDR1_JDATA)
                   );
}

/**
  * @brief  Get ADC group injected conversion data, range fit for
  *         ADC resolution 8 bits.
  * @note   For devices with feature oversampling: Oversampling
  *         can increase data width, function for extended range
  *         may be needed: @ref LL_ADC_INJ_ReadConversionData32.
  * @rmtoll JDR1     JDATA          LL_ADC_INJ_ReadConversionData8\n
  *         JDR2     JDATA          LL_ADC_INJ_ReadConversionData8\n
  *         JDR3     JDATA          LL_ADC_INJ_ReadConversionData8\n
  *         JDR4     JDATA          LL_ADC_INJ_ReadConversionData8
  * @param  ADCx ADC instance
  * @param  Rank This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_RANK_1
  *         @arg @ref LL_ADC_INJ_RANK_2
  *         @arg @ref LL_ADC_INJ_RANK_3
  *         @arg @ref LL_ADC_INJ_RANK_4
  * @retval Value between Min_Data=0x00 and Max_Data=0xFF
  */
__STATIC_INLINE uint8_t LL_ADC_INJ_ReadConversionData8(const ADC_TypeDef *ADCx, uint32_t Rank)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->JDR1,
                                                   ((Rank & ADC_INJ_JDRX_REGOFFSET_MASK) >> ADC_JDRX_REGOFFSET_POS));

  return (uint8_t)(READ_BIT(*preg,
                            ADC_JDR1_JDATA)
                  );
}

/**
  * @brief  Get ADC group injected conversion data, range fit for
  *         ADC resolution 6 bits.
  * @note   For devices with feature oversampling: Oversampling
  *         can increase data width, function for extended range
  *         may be needed: @ref LL_ADC_INJ_ReadConversionData32.
  * @rmtoll JDR1     JDATA          LL_ADC_INJ_ReadConversionData6\n
  *         JDR2     JDATA          LL_ADC_INJ_ReadConversionData6\n
  *         JDR3     JDATA          LL_ADC_INJ_ReadConversionData6\n
  *         JDR4     JDATA          LL_ADC_INJ_ReadConversionData6
  * @param  ADCx ADC instance
  * @param  Rank This parameter can be one of the following values:
  *         @arg @ref LL_ADC_INJ_RANK_1
  *         @arg @ref LL_ADC_INJ_RANK_2
  *         @arg @ref LL_ADC_INJ_RANK_3
  *         @arg @ref LL_ADC_INJ_RANK_4
  * @retval Value between Min_Data=0x00 and Max_Data=0x3F
  */
__STATIC_INLINE uint8_t LL_ADC_INJ_ReadConversionData6(const ADC_TypeDef *ADCx, uint32_t Rank)
{
  const __IO uint32_t *preg = __ADC_PTR_REG_OFFSET(ADCx->JDR1,
                                                   ((Rank & ADC_INJ_JDRX_REGOFFSET_MASK) >> ADC_JDRX_REGOFFSET_POS));

  return (uint8_t)(READ_BIT(*preg,
                            ADC_JDR1_JDATA)
                  );
}

/**
  * @}
  */

/** @defgroup ADC_LL_EF_FLAG_Management ADC flag management
  * @{
  */

/**
  * @brief  Get flag ADC ready.
  * @note   On this STM32 series, flag LL_ADC_FLAG_ADRDY is raised when the ADC
  *         is enabled and when conversion clock is active.
  *         (not only core clock: this ADC has a dual clock domain)
  * @rmtoll ISR      ADRDY          LL_ADC_IsActiveFlag_ADRDY
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_ADRDY(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, LL_ADC_FLAG_ADRDY) == (LL_ADC_FLAG_ADRDY)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag ADC group regular end of unitary conversion.
  * @rmtoll ISR      EOC            LL_ADC_IsActiveFlag_EOC
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_EOC(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, ADC_ISR_EOC) == (ADC_ISR_EOC)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag ADC group regular end of sequence conversions.
  * @rmtoll ISR      EOS            LL_ADC_IsActiveFlag_EOS
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_EOS(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, LL_ADC_FLAG_EOS) == (LL_ADC_FLAG_EOS)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag ADC group regular overrun.
  * @rmtoll ISR      OVR            LL_ADC_IsActiveFlag_OVR
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_OVR(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, LL_ADC_FLAG_OVR) == (LL_ADC_FLAG_OVR)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag ADC group regular end of sampling phase.
  * @rmtoll ISR      EOSMP          LL_ADC_IsActiveFlag_EOSMP
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_EOSMP(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, LL_ADC_FLAG_EOSMP) == (LL_ADC_FLAG_EOSMP)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag ADC group injected end of unitary conversion.
  * @rmtoll ISR      JEOC           LL_ADC_IsActiveFlag_JEOC
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_JEOC(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, LL_ADC_FLAG_JEOC) == (LL_ADC_FLAG_JEOC)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag ADC group injected end of sequence conversions.
  * @rmtoll ISR      JEOS           LL_ADC_IsActiveFlag_JEOS
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_JEOS(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, LL_ADC_FLAG_JEOS) == (LL_ADC_FLAG_JEOS)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag ADC analog watchdog 1 flag
  * @rmtoll ISR      AWD1           LL_ADC_IsActiveFlag_AWD1
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_AWD1(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, LL_ADC_FLAG_AWD1) == (LL_ADC_FLAG_AWD1)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag ADC analog watchdog 2.
  * @rmtoll ISR      AWD2           LL_ADC_IsActiveFlag_AWD2
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_AWD2(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, LL_ADC_FLAG_AWD2) == (LL_ADC_FLAG_AWD2)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag ADC analog watchdog 3.
  * @rmtoll ISR      AWD3           LL_ADC_IsActiveFlag_AWD3
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_AWD3(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->ISR, LL_ADC_FLAG_AWD3) == (LL_ADC_FLAG_AWD3)) ? 1UL : 0UL);
}

/**
  * @brief  Clear flag ADC ready.
  * @note   On this STM32 series, flag LL_ADC_FLAG_ADRDY is raised when the ADC
  *         is enabled and when conversion clock is active.
  *         (not only core clock: this ADC has a dual clock domain)
  * @rmtoll ISR      ADRDY          LL_ADC_ClearFlag_ADRDY
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_ADRDY(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_ADRDY);
}

/**
  * @brief  Clear flag ADC group regular end of unitary conversion.
  * @rmtoll ISR      EOC            LL_ADC_ClearFlag_EOC
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_EOC(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_EOC);
}

/**
  * @brief  Clear flag ADC group regular end of sequence conversions.
  * @rmtoll ISR      EOS            LL_ADC_ClearFlag_EOS
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_EOS(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_EOS);
}

/**
  * @brief  Clear flag ADC group regular overrun.
  * @rmtoll ISR      OVR            LL_ADC_ClearFlag_OVR
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_OVR(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_OVR);
}

/**
  * @brief  Clear flag ADC group regular end of sampling phase.
  * @rmtoll ISR      EOSMP          LL_ADC_ClearFlag_EOSMP
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_EOSMP(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_EOSMP);
}

/**
  * @brief  Clear flag ADC group injected end of unitary conversion.
  * @rmtoll ISR      JEOC           LL_ADC_ClearFlag_JEOC
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_JEOC(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_JEOC);
}

/**
  * @brief  Clear flag ADC group injected end of sequence conversions.
  * @rmtoll ISR      JEOS           LL_ADC_ClearFlag_JEOS
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_JEOS(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_JEOS);
}

/**
  * @brief  Clear flag ADC analog watchdog 1.
  * @rmtoll ISR      AWD1           LL_ADC_ClearFlag_AWD1
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_AWD1(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_AWD1);
}

/**
  * @brief  Clear flag ADC analog watchdog 2.
  * @rmtoll ISR      AWD2           LL_ADC_ClearFlag_AWD2
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_AWD2(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_AWD2);
}

/**
  * @brief  Clear flag ADC analog watchdog 3.
  * @rmtoll ISR      AWD3           LL_ADC_ClearFlag_AWD3
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_ClearFlag_AWD3(ADC_TypeDef *ADCx)
{
  WRITE_REG(ADCx->ISR, LL_ADC_FLAG_AWD3);
}

#if defined(ADC_MULTIMODE_SUPPORT)
/**
  * @brief  Get flag multimode ADC ready of the ADC master.
  * @rmtoll CSR      ADRDY_MST      LL_ADC_IsActiveFlag_MST_ADRDY
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_ADRDY(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_ADRDY_MST) == (LL_ADC_FLAG_ADRDY_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC ready of the ADC slave.
  * @rmtoll CSR      ADRDY_SLV      LL_ADC_IsActiveFlag_SLV_ADRDY
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_ADRDY(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_ADRDY_SLV) == (LL_ADC_FLAG_ADRDY_SLV)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group regular end of unitary conversion of the ADC master.
  * @rmtoll CSR      EOC_MST        LL_ADC_IsActiveFlag_MST_EOC
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_EOC(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_EOC_MST) == (LL_ADC_FLAG_EOC_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group regular end of unitary conversion of the ADC slave.
  * @rmtoll CSR      EOC_SLV        LL_ADC_IsActiveFlag_SLV_EOC
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_EOC(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_EOC_SLV) == (LL_ADC_FLAG_EOC_SLV)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group regular end of sequence conversions of the ADC master.
  * @rmtoll CSR      EOS_MST        LL_ADC_IsActiveFlag_MST_EOS
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_EOS(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_EOS_MST) == (LL_ADC_FLAG_EOS_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group regular end of sequence conversions of the ADC slave.
  * @rmtoll CSR      EOS_SLV        LL_ADC_IsActiveFlag_SLV_EOS
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_EOS(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_EOS_SLV) == (LL_ADC_FLAG_EOS_SLV)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group regular overrun of the ADC master.
  * @rmtoll CSR      OVR_MST        LL_ADC_IsActiveFlag_MST_OVR
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_OVR(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_OVR_MST) == (LL_ADC_FLAG_OVR_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group regular overrun of the ADC slave.
  * @rmtoll CSR      OVR_SLV        LL_ADC_IsActiveFlag_SLV_OVR
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_OVR(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_OVR_SLV) == (LL_ADC_FLAG_OVR_SLV)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group regular end of sampling of the ADC master.
  * @rmtoll CSR      EOSMP_MST      LL_ADC_IsActiveFlag_MST_EOSMP
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_EOSMP(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_EOSMP_MST) == (LL_ADC_FLAG_EOSMP_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group regular end of sampling of the ADC slave.
  * @rmtoll CSR      EOSMP_SLV      LL_ADC_IsActiveFlag_SLV_EOSMP
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_EOSMP(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_EOSMP_SLV) == (LL_ADC_FLAG_EOSMP_SLV)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group injected end of unitary conversion of the ADC master.
  * @rmtoll CSR      JEOC_MST       LL_ADC_IsActiveFlag_MST_JEOC
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_JEOC(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_JEOC_MST) == (LL_ADC_FLAG_JEOC_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group injected end of unitary conversion of the ADC slave.
  * @rmtoll CSR      JEOC_SLV       LL_ADC_IsActiveFlag_SLV_JEOC
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_JEOC(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_JEOC_SLV) == (LL_ADC_FLAG_JEOC_SLV)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group injected end of sequence conversions of the ADC master.
  * @rmtoll CSR      JEOS_MST       LL_ADC_IsActiveFlag_MST_JEOS
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_JEOS(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_JEOS_MST) == (LL_ADC_FLAG_JEOS_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC group injected end of sequence conversions of the ADC slave.
  * @rmtoll CSR      JEOS_SLV       LL_ADC_IsActiveFlag_SLV_JEOS
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_JEOS(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_JEOS_SLV) == (LL_ADC_FLAG_JEOS_SLV)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC analog watchdog 1 of the ADC master.
  * @rmtoll CSR      AWD1_MST       LL_ADC_IsActiveFlag_MST_AWD1
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_AWD1(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_AWD1_MST) == (LL_ADC_FLAG_AWD1_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode analog watchdog 1 of the ADC slave.
  * @rmtoll CSR      AWD1_SLV       LL_ADC_IsActiveFlag_SLV_AWD1
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_AWD1(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_AWD1_SLV) == (LL_ADC_FLAG_AWD1_SLV)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC analog watchdog 2 of the ADC master.
  * @rmtoll CSR      AWD2_MST       LL_ADC_IsActiveFlag_MST_AWD2
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_AWD2(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_AWD2_MST) == (LL_ADC_FLAG_AWD2_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC analog watchdog 2 of the ADC slave.
  * @rmtoll CSR      AWD2_SLV       LL_ADC_IsActiveFlag_SLV_AWD2
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_AWD2(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_AWD2_SLV) == (LL_ADC_FLAG_AWD2_SLV)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC analog watchdog 3 of the ADC master.
  * @rmtoll CSR      AWD3_MST       LL_ADC_IsActiveFlag_MST_AWD3
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_MST_AWD3(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_AWD3_MST) == (LL_ADC_FLAG_AWD3_MST)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag multimode ADC analog watchdog 3 of the ADC slave.
  * @rmtoll CSR      AWD3_SLV       LL_ADC_IsActiveFlag_SLV_AWD3
  * @param  ADCxy_COMMON ADC common instance
  *         (can be set directly from CMSIS definition or by using helper macro @ref __LL_ADC_COMMON_INSTANCE() )
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsActiveFlag_SLV_AWD3(const ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((READ_BIT(ADCxy_COMMON->CSR, LL_ADC_FLAG_AWD3_SLV) == (LL_ADC_FLAG_AWD3_SLV)) ? 1UL : 0UL);
}
#endif /* ADC_MULTIMODE_SUPPORT */

/**
  * @}
  */

/** @defgroup ADC_LL_EF_IT_Management ADC IT management
  * @{
  */

/**
  * @brief  Enable ADC ready.
  * @rmtoll IER      ADRDYIE        LL_ADC_EnableIT_ADRDY
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_ADRDY(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_ADRDY);
}

/**
  * @brief  Enable interruption ADC group regular end of unitary conversion.
  * @rmtoll IER      EOCIE          LL_ADC_EnableIT_EOC
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_EOC(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_EOC);
}

/**
  * @brief  Enable interruption ADC group regular end of sequence conversions.
  * @rmtoll IER      EOSIE          LL_ADC_EnableIT_EOS
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_EOS(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_EOS);
}

/**
  * @brief  Enable ADC group regular interruption overrun.
  * @rmtoll IER      OVRIE          LL_ADC_EnableIT_OVR
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_OVR(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_OVR);
}

/**
  * @brief  Enable interruption ADC group regular end of sampling.
  * @rmtoll IER      EOSMPIE        LL_ADC_EnableIT_EOSMP
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_EOSMP(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_EOSMP);
}

/**
  * @brief  Enable interruption ADC group injected end of unitary conversion.
  * @rmtoll IER      JEOCIE         LL_ADC_EnableIT_JEOC
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_JEOC(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_JEOC);
}

/**
  * @brief  Enable interruption ADC group injected end of sequence conversions.
  * @rmtoll IER      JEOSIE         LL_ADC_EnableIT_JEOS
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_JEOS(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_JEOS);
}

/**
  * @brief  Enable interruption ADC analog watchdog 1.
  * @rmtoll IER      AWD1IE         LL_ADC_EnableIT_AWD1
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_AWD1(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_AWD1);
}

/**
  * @brief  Enable interruption ADC analog watchdog 2.
  * @rmtoll IER      AWD2IE         LL_ADC_EnableIT_AWD2
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_AWD2(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_AWD2);
}

/**
  * @brief  Enable interruption ADC analog watchdog 3.
  * @rmtoll IER      AWD3IE         LL_ADC_EnableIT_AWD3
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_EnableIT_AWD3(ADC_TypeDef *ADCx)
{
  SET_BIT(ADCx->IER, LL_ADC_IT_AWD3);
}

/**
  * @brief  Disable interruption ADC ready.
  * @rmtoll IER      ADRDYIE        LL_ADC_DisableIT_ADRDY
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_ADRDY(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_ADRDY);
}

/**
  * @brief  Disable interruption ADC group regular end of unitary conversion.
  * @rmtoll IER      EOCIE          LL_ADC_DisableIT_EOC
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_EOC(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_EOC);
}

/**
  * @brief  Disable interruption ADC group regular end of sequence conversions.
  * @rmtoll IER      EOSIE          LL_ADC_DisableIT_EOS
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_EOS(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_EOS);
}

/**
  * @brief  Disable interruption ADC group regular overrun.
  * @rmtoll IER      OVRIE          LL_ADC_DisableIT_OVR
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_OVR(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_OVR);
}

/**
  * @brief  Disable interruption ADC group regular end of sampling.
  * @rmtoll IER      EOSMPIE        LL_ADC_DisableIT_EOSMP
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_EOSMP(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_EOSMP);
}

/**
  * @brief  Disable interruption ADC group regular end of unitary conversion.
  * @rmtoll IER      JEOCIE         LL_ADC_DisableIT_JEOC
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_JEOC(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_JEOC);
}

/**
  * @brief  Disable interruption ADC group injected end of sequence conversions.
  * @rmtoll IER      JEOSIE         LL_ADC_DisableIT_JEOS
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_JEOS(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_JEOS);
}

/**
  * @brief  Disable interruption ADC analog watchdog 1.
  * @rmtoll IER      AWD1IE         LL_ADC_DisableIT_AWD1
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_AWD1(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_AWD1);
}

/**
  * @brief  Disable interruption ADC analog watchdog 2.
  * @rmtoll IER      AWD2IE         LL_ADC_DisableIT_AWD2
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_AWD2(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_AWD2);
}

/**
  * @brief  Disable interruption ADC analog watchdog 3.
  * @rmtoll IER      AWD3IE         LL_ADC_DisableIT_AWD3
  * @param  ADCx ADC instance
  * @retval None
  */
__STATIC_INLINE void LL_ADC_DisableIT_AWD3(ADC_TypeDef *ADCx)
{
  CLEAR_BIT(ADCx->IER, LL_ADC_IT_AWD3);
}

/**
  * @brief  Get state of interruption ADC ready
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      ADRDYIE        LL_ADC_IsEnabledIT_ADRDY
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_ADRDY(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_ADRDY) == (LL_ADC_IT_ADRDY)) ? 1UL : 0UL);
}

/**
  * @brief  Get state of interruption ADC group regular end of unitary conversion
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      EOCIE          LL_ADC_IsEnabledIT_EOC
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_EOC(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_EOC) == (LL_ADC_IT_EOC)) ? 1UL : 0UL);
}

/**
  * @brief  Get state of interruption ADC group regular end of sequence conversions
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      EOSIE          LL_ADC_IsEnabledIT_EOS
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_EOS(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_EOS) == (LL_ADC_IT_EOS)) ? 1UL : 0UL);
}

/**
  * @brief  Get state of interruption ADC group regular overrun
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      OVRIE          LL_ADC_IsEnabledIT_OVR
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_OVR(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_OVR) == (LL_ADC_IT_OVR)) ? 1UL : 0UL);
}

/**
  * @brief  Get state of interruption ADC group regular end of sampling
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      EOSMPIE        LL_ADC_IsEnabledIT_EOSMP
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_EOSMP(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_EOSMP) == (LL_ADC_IT_EOSMP)) ? 1UL : 0UL);
}

/**
  * @brief  Get state of interruption ADC group injected end of unitary conversion
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      JEOCIE         LL_ADC_IsEnabledIT_JEOC
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_JEOC(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_JEOC) == (LL_ADC_IT_JEOC)) ? 1UL : 0UL);
}

/**
  * @brief  Get state of interruption ADC group injected end of sequence conversions
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      JEOSIE         LL_ADC_IsEnabledIT_JEOS
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_JEOS(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_JEOS) == (LL_ADC_IT_JEOS)) ? 1UL : 0UL);
}

/**
  * @brief  Get state of interruption ADC analog watchdog 1
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      AWD1IE         LL_ADC_IsEnabledIT_AWD1
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_AWD1(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_AWD1) == (LL_ADC_IT_AWD1)) ? 1UL : 0UL);
}

/**
  * @brief  Get state of interruption Get ADC analog watchdog 2
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      AWD2IE         LL_ADC_IsEnabledIT_AWD2
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_AWD2(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_AWD2) == (LL_ADC_IT_AWD2)) ? 1UL : 0UL);
}

/**
  * @brief  Get state of interruption Get ADC analog watchdog 3
  *         (0: interrupt disabled, 1: interrupt enabled).
  * @rmtoll IER      AWD3IE         LL_ADC_IsEnabledIT_AWD3
  * @param  ADCx ADC instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_ADC_IsEnabledIT_AWD3(const ADC_TypeDef *ADCx)
{
  return ((READ_BIT(ADCx->IER, LL_ADC_IT_AWD3) == (LL_ADC_IT_AWD3)) ? 1UL : 0UL);
}

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup ADC_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

/* Initialization of some features of ADC common parameters and multimode */
ErrorStatus LL_ADC_CommonDeInit(const ADC_Common_TypeDef *ADCxy_COMMON);
ErrorStatus LL_ADC_CommonInit(ADC_Common_TypeDef *ADCxy_COMMON, const LL_ADC_CommonInitTypeDef *pADC_CommonInitStruct);
void        LL_ADC_CommonStructInit(LL_ADC_CommonInitTypeDef *pADC_CommonInitStruct);

/* De-initialization of ADC instance, ADC group regular and ADC group injected */
/* (availability of ADC group injected depends on STM32 series) */
ErrorStatus LL_ADC_DeInit(ADC_TypeDef *ADCx);

/* Initialization of some features of ADC instance */
ErrorStatus LL_ADC_Init(ADC_TypeDef *ADCx, const LL_ADC_InitTypeDef *pADC_InitStruct);
void        LL_ADC_StructInit(LL_ADC_InitTypeDef *pADC_InitStruct);

/* Initialization of some features of ADC instance and ADC group regular */
ErrorStatus LL_ADC_REG_Init(ADC_TypeDef *ADCx, const LL_ADC_REG_InitTypeDef *pADC_RegInitStruct);
void        LL_ADC_REG_StructInit(LL_ADC_REG_InitTypeDef *pADC_RegInitStruct);

/* Initialization of some features of ADC instance and ADC group injected */
ErrorStatus LL_ADC_INJ_Init(ADC_TypeDef *ADCx, const LL_ADC_INJ_InitTypeDef *pADC_InjInitStruct);
void        LL_ADC_INJ_StructInit(LL_ADC_INJ_InitTypeDef *pADC_InjInitStruct);

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/**
  * @}
  */

/**
  * @}
  */

#endif /* ADC1 || ADC2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_LL_ADC_H */
