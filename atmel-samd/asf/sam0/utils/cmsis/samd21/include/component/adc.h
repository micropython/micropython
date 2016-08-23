/**
 * \file
 *
 * \brief Component description for ADC
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_ADC_COMPONENT_
#define _SAMD21_ADC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR ADC */
/* ========================================================================== */
/** \addtogroup SAMD21_ADC Analog Digital Converter */
/*@{*/

#define ADC_U2204
#define REV_ADC                     0x120

/* -------- ADC_CTRLA : (ADC Offset: 0x00) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  RUNSTDBY:1;       /*!< bit:      2  Run in Standby                     */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_CTRLA_OFFSET            0x00         /**< \brief (ADC_CTRLA offset) Control A */
#define ADC_CTRLA_RESETVALUE        0x00ul       /**< \brief (ADC_CTRLA reset_value) Control A */

#define ADC_CTRLA_SWRST_Pos         0            /**< \brief (ADC_CTRLA) Software Reset */
#define ADC_CTRLA_SWRST             (0x1ul << ADC_CTRLA_SWRST_Pos)
#define ADC_CTRLA_ENABLE_Pos        1            /**< \brief (ADC_CTRLA) Enable */
#define ADC_CTRLA_ENABLE            (0x1ul << ADC_CTRLA_ENABLE_Pos)
#define ADC_CTRLA_RUNSTDBY_Pos      2            /**< \brief (ADC_CTRLA) Run in Standby */
#define ADC_CTRLA_RUNSTDBY          (0x1ul << ADC_CTRLA_RUNSTDBY_Pos)
#define ADC_CTRLA_MASK              0x07ul       /**< \brief (ADC_CTRLA) MASK Register */

/* -------- ADC_REFCTRL : (ADC Offset: 0x01) (R/W  8) Reference Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  REFSEL:4;         /*!< bit:  0.. 3  Reference Selection                */
    uint8_t  :3;               /*!< bit:  4.. 6  Reserved                           */
    uint8_t  REFCOMP:1;        /*!< bit:      7  Reference Buffer Offset Compensation Enable */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_REFCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_REFCTRL_OFFSET          0x01         /**< \brief (ADC_REFCTRL offset) Reference Control */
#define ADC_REFCTRL_RESETVALUE      0x00ul       /**< \brief (ADC_REFCTRL reset_value) Reference Control */

#define ADC_REFCTRL_REFSEL_Pos      0            /**< \brief (ADC_REFCTRL) Reference Selection */
#define ADC_REFCTRL_REFSEL_Msk      (0xFul << ADC_REFCTRL_REFSEL_Pos)
#define ADC_REFCTRL_REFSEL(value)   (ADC_REFCTRL_REFSEL_Msk & ((value) << ADC_REFCTRL_REFSEL_Pos))
#define   ADC_REFCTRL_REFSEL_INT1V_Val    0x0ul  /**< \brief (ADC_REFCTRL) 1.0V voltage reference */
#define   ADC_REFCTRL_REFSEL_INTVCC0_Val  0x1ul  /**< \brief (ADC_REFCTRL) 1/1.48 VDDANA */
#define   ADC_REFCTRL_REFSEL_INTVCC1_Val  0x2ul  /**< \brief (ADC_REFCTRL) 1/2 VDDANA (only for VDDANA > 2.0V) */
#define   ADC_REFCTRL_REFSEL_AREFA_Val    0x3ul  /**< \brief (ADC_REFCTRL) External reference */
#define   ADC_REFCTRL_REFSEL_AREFB_Val    0x4ul  /**< \brief (ADC_REFCTRL) External reference */
#define ADC_REFCTRL_REFSEL_INT1V    (ADC_REFCTRL_REFSEL_INT1V_Val  << ADC_REFCTRL_REFSEL_Pos)
#define ADC_REFCTRL_REFSEL_INTVCC0  (ADC_REFCTRL_REFSEL_INTVCC0_Val << ADC_REFCTRL_REFSEL_Pos)
#define ADC_REFCTRL_REFSEL_INTVCC1  (ADC_REFCTRL_REFSEL_INTVCC1_Val << ADC_REFCTRL_REFSEL_Pos)
#define ADC_REFCTRL_REFSEL_AREFA    (ADC_REFCTRL_REFSEL_AREFA_Val  << ADC_REFCTRL_REFSEL_Pos)
#define ADC_REFCTRL_REFSEL_AREFB    (ADC_REFCTRL_REFSEL_AREFB_Val  << ADC_REFCTRL_REFSEL_Pos)
#define ADC_REFCTRL_REFCOMP_Pos     7            /**< \brief (ADC_REFCTRL) Reference Buffer Offset Compensation Enable */
#define ADC_REFCTRL_REFCOMP         (0x1ul << ADC_REFCTRL_REFCOMP_Pos)
#define ADC_REFCTRL_MASK            0x8Ful       /**< \brief (ADC_REFCTRL) MASK Register */

/* -------- ADC_AVGCTRL : (ADC Offset: 0x02) (R/W  8) Average Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SAMPLENUM:4;      /*!< bit:  0.. 3  Number of Samples to be Collected  */
    uint8_t  ADJRES:3;         /*!< bit:  4.. 6  Adjusting Result / Division Coefficient */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_AVGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_AVGCTRL_OFFSET          0x02         /**< \brief (ADC_AVGCTRL offset) Average Control */
#define ADC_AVGCTRL_RESETVALUE      0x00ul       /**< \brief (ADC_AVGCTRL reset_value) Average Control */

#define ADC_AVGCTRL_SAMPLENUM_Pos   0            /**< \brief (ADC_AVGCTRL) Number of Samples to be Collected */
#define ADC_AVGCTRL_SAMPLENUM_Msk   (0xFul << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM(value) (ADC_AVGCTRL_SAMPLENUM_Msk & ((value) << ADC_AVGCTRL_SAMPLENUM_Pos))
#define   ADC_AVGCTRL_SAMPLENUM_1_Val     0x0ul  /**< \brief (ADC_AVGCTRL) 1 sample */
#define   ADC_AVGCTRL_SAMPLENUM_2_Val     0x1ul  /**< \brief (ADC_AVGCTRL) 2 samples */
#define   ADC_AVGCTRL_SAMPLENUM_4_Val     0x2ul  /**< \brief (ADC_AVGCTRL) 4 samples */
#define   ADC_AVGCTRL_SAMPLENUM_8_Val     0x3ul  /**< \brief (ADC_AVGCTRL) 8 samples */
#define   ADC_AVGCTRL_SAMPLENUM_16_Val    0x4ul  /**< \brief (ADC_AVGCTRL) 16 samples */
#define   ADC_AVGCTRL_SAMPLENUM_32_Val    0x5ul  /**< \brief (ADC_AVGCTRL) 32 samples */
#define   ADC_AVGCTRL_SAMPLENUM_64_Val    0x6ul  /**< \brief (ADC_AVGCTRL) 64 samples */
#define   ADC_AVGCTRL_SAMPLENUM_128_Val   0x7ul  /**< \brief (ADC_AVGCTRL) 128 samples */
#define   ADC_AVGCTRL_SAMPLENUM_256_Val   0x8ul  /**< \brief (ADC_AVGCTRL) 256 samples */
#define   ADC_AVGCTRL_SAMPLENUM_512_Val   0x9ul  /**< \brief (ADC_AVGCTRL) 512 samples */
#define   ADC_AVGCTRL_SAMPLENUM_1024_Val  0xAul  /**< \brief (ADC_AVGCTRL) 1024 samples */
#define ADC_AVGCTRL_SAMPLENUM_1     (ADC_AVGCTRL_SAMPLENUM_1_Val   << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_2     (ADC_AVGCTRL_SAMPLENUM_2_Val   << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_4     (ADC_AVGCTRL_SAMPLENUM_4_Val   << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_8     (ADC_AVGCTRL_SAMPLENUM_8_Val   << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_16    (ADC_AVGCTRL_SAMPLENUM_16_Val  << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_32    (ADC_AVGCTRL_SAMPLENUM_32_Val  << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_64    (ADC_AVGCTRL_SAMPLENUM_64_Val  << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_128   (ADC_AVGCTRL_SAMPLENUM_128_Val << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_256   (ADC_AVGCTRL_SAMPLENUM_256_Val << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_512   (ADC_AVGCTRL_SAMPLENUM_512_Val << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_SAMPLENUM_1024  (ADC_AVGCTRL_SAMPLENUM_1024_Val << ADC_AVGCTRL_SAMPLENUM_Pos)
#define ADC_AVGCTRL_ADJRES_Pos      4            /**< \brief (ADC_AVGCTRL) Adjusting Result / Division Coefficient */
#define ADC_AVGCTRL_ADJRES_Msk      (0x7ul << ADC_AVGCTRL_ADJRES_Pos)
#define ADC_AVGCTRL_ADJRES(value)   (ADC_AVGCTRL_ADJRES_Msk & ((value) << ADC_AVGCTRL_ADJRES_Pos))
#define ADC_AVGCTRL_MASK            0x7Ful       /**< \brief (ADC_AVGCTRL) MASK Register */

/* -------- ADC_SAMPCTRL : (ADC Offset: 0x03) (R/W  8) Sampling Time Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SAMPLEN:6;        /*!< bit:  0.. 5  Sampling Time Length               */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_SAMPCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_SAMPCTRL_OFFSET         0x03         /**< \brief (ADC_SAMPCTRL offset) Sampling Time Control */
#define ADC_SAMPCTRL_RESETVALUE     0x00ul       /**< \brief (ADC_SAMPCTRL reset_value) Sampling Time Control */

#define ADC_SAMPCTRL_SAMPLEN_Pos    0            /**< \brief (ADC_SAMPCTRL) Sampling Time Length */
#define ADC_SAMPCTRL_SAMPLEN_Msk    (0x3Ful << ADC_SAMPCTRL_SAMPLEN_Pos)
#define ADC_SAMPCTRL_SAMPLEN(value) (ADC_SAMPCTRL_SAMPLEN_Msk & ((value) << ADC_SAMPCTRL_SAMPLEN_Pos))
#define ADC_SAMPCTRL_MASK           0x3Ful       /**< \brief (ADC_SAMPCTRL) MASK Register */

/* -------- ADC_CTRLB : (ADC Offset: 0x04) (R/W 16) Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DIFFMODE:1;       /*!< bit:      0  Differential Mode                  */
    uint16_t LEFTADJ:1;        /*!< bit:      1  Left-Adjusted Result               */
    uint16_t FREERUN:1;        /*!< bit:      2  Free Running Mode                  */
    uint16_t CORREN:1;         /*!< bit:      3  Digital Correction Logic Enabled   */
    uint16_t RESSEL:2;         /*!< bit:  4.. 5  Conversion Result Resolution       */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t PRESCALER:3;      /*!< bit:  8..10  Prescaler Configuration            */
    uint16_t :5;               /*!< bit: 11..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} ADC_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_CTRLB_OFFSET            0x04         /**< \brief (ADC_CTRLB offset) Control B */
#define ADC_CTRLB_RESETVALUE        0x0000ul     /**< \brief (ADC_CTRLB reset_value) Control B */

#define ADC_CTRLB_DIFFMODE_Pos      0            /**< \brief (ADC_CTRLB) Differential Mode */
#define ADC_CTRLB_DIFFMODE          (0x1ul << ADC_CTRLB_DIFFMODE_Pos)
#define ADC_CTRLB_LEFTADJ_Pos       1            /**< \brief (ADC_CTRLB) Left-Adjusted Result */
#define ADC_CTRLB_LEFTADJ           (0x1ul << ADC_CTRLB_LEFTADJ_Pos)
#define ADC_CTRLB_FREERUN_Pos       2            /**< \brief (ADC_CTRLB) Free Running Mode */
#define ADC_CTRLB_FREERUN           (0x1ul << ADC_CTRLB_FREERUN_Pos)
#define ADC_CTRLB_CORREN_Pos        3            /**< \brief (ADC_CTRLB) Digital Correction Logic Enabled */
#define ADC_CTRLB_CORREN            (0x1ul << ADC_CTRLB_CORREN_Pos)
#define ADC_CTRLB_RESSEL_Pos        4            /**< \brief (ADC_CTRLB) Conversion Result Resolution */
#define ADC_CTRLB_RESSEL_Msk        (0x3ul << ADC_CTRLB_RESSEL_Pos)
#define ADC_CTRLB_RESSEL(value)     (ADC_CTRLB_RESSEL_Msk & ((value) << ADC_CTRLB_RESSEL_Pos))
#define   ADC_CTRLB_RESSEL_12BIT_Val      0x0ul  /**< \brief (ADC_CTRLB) 12-bit result */
#define   ADC_CTRLB_RESSEL_16BIT_Val      0x1ul  /**< \brief (ADC_CTRLB) For averaging mode output */
#define   ADC_CTRLB_RESSEL_10BIT_Val      0x2ul  /**< \brief (ADC_CTRLB) 10-bit result */
#define   ADC_CTRLB_RESSEL_8BIT_Val       0x3ul  /**< \brief (ADC_CTRLB) 8-bit result */
#define ADC_CTRLB_RESSEL_12BIT      (ADC_CTRLB_RESSEL_12BIT_Val    << ADC_CTRLB_RESSEL_Pos)
#define ADC_CTRLB_RESSEL_16BIT      (ADC_CTRLB_RESSEL_16BIT_Val    << ADC_CTRLB_RESSEL_Pos)
#define ADC_CTRLB_RESSEL_10BIT      (ADC_CTRLB_RESSEL_10BIT_Val    << ADC_CTRLB_RESSEL_Pos)
#define ADC_CTRLB_RESSEL_8BIT       (ADC_CTRLB_RESSEL_8BIT_Val     << ADC_CTRLB_RESSEL_Pos)
#define ADC_CTRLB_PRESCALER_Pos     8            /**< \brief (ADC_CTRLB) Prescaler Configuration */
#define ADC_CTRLB_PRESCALER_Msk     (0x7ul << ADC_CTRLB_PRESCALER_Pos)
#define ADC_CTRLB_PRESCALER(value)  (ADC_CTRLB_PRESCALER_Msk & ((value) << ADC_CTRLB_PRESCALER_Pos))
#define   ADC_CTRLB_PRESCALER_DIV4_Val    0x0ul  /**< \brief (ADC_CTRLB) Peripheral clock divided by 4 */
#define   ADC_CTRLB_PRESCALER_DIV8_Val    0x1ul  /**< \brief (ADC_CTRLB) Peripheral clock divided by 8 */
#define   ADC_CTRLB_PRESCALER_DIV16_Val   0x2ul  /**< \brief (ADC_CTRLB) Peripheral clock divided by 16 */
#define   ADC_CTRLB_PRESCALER_DIV32_Val   0x3ul  /**< \brief (ADC_CTRLB) Peripheral clock divided by 32 */
#define   ADC_CTRLB_PRESCALER_DIV64_Val   0x4ul  /**< \brief (ADC_CTRLB) Peripheral clock divided by 64 */
#define   ADC_CTRLB_PRESCALER_DIV128_Val  0x5ul  /**< \brief (ADC_CTRLB) Peripheral clock divided by 128 */
#define   ADC_CTRLB_PRESCALER_DIV256_Val  0x6ul  /**< \brief (ADC_CTRLB) Peripheral clock divided by 256 */
#define   ADC_CTRLB_PRESCALER_DIV512_Val  0x7ul  /**< \brief (ADC_CTRLB) Peripheral clock divided by 512 */
#define ADC_CTRLB_PRESCALER_DIV4    (ADC_CTRLB_PRESCALER_DIV4_Val  << ADC_CTRLB_PRESCALER_Pos)
#define ADC_CTRLB_PRESCALER_DIV8    (ADC_CTRLB_PRESCALER_DIV8_Val  << ADC_CTRLB_PRESCALER_Pos)
#define ADC_CTRLB_PRESCALER_DIV16   (ADC_CTRLB_PRESCALER_DIV16_Val << ADC_CTRLB_PRESCALER_Pos)
#define ADC_CTRLB_PRESCALER_DIV32   (ADC_CTRLB_PRESCALER_DIV32_Val << ADC_CTRLB_PRESCALER_Pos)
#define ADC_CTRLB_PRESCALER_DIV64   (ADC_CTRLB_PRESCALER_DIV64_Val << ADC_CTRLB_PRESCALER_Pos)
#define ADC_CTRLB_PRESCALER_DIV128  (ADC_CTRLB_PRESCALER_DIV128_Val << ADC_CTRLB_PRESCALER_Pos)
#define ADC_CTRLB_PRESCALER_DIV256  (ADC_CTRLB_PRESCALER_DIV256_Val << ADC_CTRLB_PRESCALER_Pos)
#define ADC_CTRLB_PRESCALER_DIV512  (ADC_CTRLB_PRESCALER_DIV512_Val << ADC_CTRLB_PRESCALER_Pos)
#define ADC_CTRLB_MASK              0x073Ful     /**< \brief (ADC_CTRLB) MASK Register */

/* -------- ADC_WINCTRL : (ADC Offset: 0x08) (R/W  8) Window Monitor Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  WINMODE:3;        /*!< bit:  0.. 2  Window Monitor Mode                */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_WINCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_WINCTRL_OFFSET          0x08         /**< \brief (ADC_WINCTRL offset) Window Monitor Control */
#define ADC_WINCTRL_RESETVALUE      0x00ul       /**< \brief (ADC_WINCTRL reset_value) Window Monitor Control */

#define ADC_WINCTRL_WINMODE_Pos     0            /**< \brief (ADC_WINCTRL) Window Monitor Mode */
#define ADC_WINCTRL_WINMODE_Msk     (0x7ul << ADC_WINCTRL_WINMODE_Pos)
#define ADC_WINCTRL_WINMODE(value)  (ADC_WINCTRL_WINMODE_Msk & ((value) << ADC_WINCTRL_WINMODE_Pos))
#define   ADC_WINCTRL_WINMODE_DISABLE_Val 0x0ul  /**< \brief (ADC_WINCTRL) No window mode (default) */
#define   ADC_WINCTRL_WINMODE_MODE1_Val   0x1ul  /**< \brief (ADC_WINCTRL) Mode 1: RESULT > WINLT */
#define   ADC_WINCTRL_WINMODE_MODE2_Val   0x2ul  /**< \brief (ADC_WINCTRL) Mode 2: RESULT < WINUT */
#define   ADC_WINCTRL_WINMODE_MODE3_Val   0x3ul  /**< \brief (ADC_WINCTRL) Mode 3: WINLT < RESULT < WINUT */
#define   ADC_WINCTRL_WINMODE_MODE4_Val   0x4ul  /**< \brief (ADC_WINCTRL) Mode 4: !(WINLT < RESULT < WINUT) */
#define ADC_WINCTRL_WINMODE_DISABLE (ADC_WINCTRL_WINMODE_DISABLE_Val << ADC_WINCTRL_WINMODE_Pos)
#define ADC_WINCTRL_WINMODE_MODE1   (ADC_WINCTRL_WINMODE_MODE1_Val << ADC_WINCTRL_WINMODE_Pos)
#define ADC_WINCTRL_WINMODE_MODE2   (ADC_WINCTRL_WINMODE_MODE2_Val << ADC_WINCTRL_WINMODE_Pos)
#define ADC_WINCTRL_WINMODE_MODE3   (ADC_WINCTRL_WINMODE_MODE3_Val << ADC_WINCTRL_WINMODE_Pos)
#define ADC_WINCTRL_WINMODE_MODE4   (ADC_WINCTRL_WINMODE_MODE4_Val << ADC_WINCTRL_WINMODE_Pos)
#define ADC_WINCTRL_MASK            0x07ul       /**< \brief (ADC_WINCTRL) MASK Register */

/* -------- ADC_SWTRIG : (ADC Offset: 0x0C) (R/W  8) Software Trigger -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  FLUSH:1;          /*!< bit:      0  ADC Conversion Flush               */
    uint8_t  START:1;          /*!< bit:      1  ADC Start Conversion               */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_SWTRIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_SWTRIG_OFFSET           0x0C         /**< \brief (ADC_SWTRIG offset) Software Trigger */
#define ADC_SWTRIG_RESETVALUE       0x00ul       /**< \brief (ADC_SWTRIG reset_value) Software Trigger */

#define ADC_SWTRIG_FLUSH_Pos        0            /**< \brief (ADC_SWTRIG) ADC Conversion Flush */
#define ADC_SWTRIG_FLUSH            (0x1ul << ADC_SWTRIG_FLUSH_Pos)
#define ADC_SWTRIG_START_Pos        1            /**< \brief (ADC_SWTRIG) ADC Start Conversion */
#define ADC_SWTRIG_START            (0x1ul << ADC_SWTRIG_START_Pos)
#define ADC_SWTRIG_MASK             0x03ul       /**< \brief (ADC_SWTRIG) MASK Register */

/* -------- ADC_INPUTCTRL : (ADC Offset: 0x10) (R/W 32) Input Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t MUXPOS:5;         /*!< bit:  0.. 4  Positive Mux Input Selection       */
    uint32_t :3;               /*!< bit:  5.. 7  Reserved                           */
    uint32_t MUXNEG:5;         /*!< bit:  8..12  Negative Mux Input Selection       */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t INPUTSCAN:4;      /*!< bit: 16..19  Number of Input Channels Included in Scan */
    uint32_t INPUTOFFSET:4;    /*!< bit: 20..23  Positive Mux Setting Offset        */
    uint32_t GAIN:4;           /*!< bit: 24..27  Gain Factor Selection              */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} ADC_INPUTCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_INPUTCTRL_OFFSET        0x10         /**< \brief (ADC_INPUTCTRL offset) Input Control */
#define ADC_INPUTCTRL_RESETVALUE    0x00000000ul /**< \brief (ADC_INPUTCTRL reset_value) Input Control */

#define ADC_INPUTCTRL_MUXPOS_Pos    0            /**< \brief (ADC_INPUTCTRL) Positive Mux Input Selection */
#define ADC_INPUTCTRL_MUXPOS_Msk    (0x1Ful << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS(value) (ADC_INPUTCTRL_MUXPOS_Msk & ((value) << ADC_INPUTCTRL_MUXPOS_Pos))
#define   ADC_INPUTCTRL_MUXPOS_PIN0_Val   0x0ul  /**< \brief (ADC_INPUTCTRL) ADC AIN0 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN1_Val   0x1ul  /**< \brief (ADC_INPUTCTRL) ADC AIN1 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN2_Val   0x2ul  /**< \brief (ADC_INPUTCTRL) ADC AIN2 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN3_Val   0x3ul  /**< \brief (ADC_INPUTCTRL) ADC AIN3 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN4_Val   0x4ul  /**< \brief (ADC_INPUTCTRL) ADC AIN4 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN5_Val   0x5ul  /**< \brief (ADC_INPUTCTRL) ADC AIN5 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN6_Val   0x6ul  /**< \brief (ADC_INPUTCTRL) ADC AIN6 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN7_Val   0x7ul  /**< \brief (ADC_INPUTCTRL) ADC AIN7 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN8_Val   0x8ul  /**< \brief (ADC_INPUTCTRL) ADC AIN8 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN9_Val   0x9ul  /**< \brief (ADC_INPUTCTRL) ADC AIN9 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN10_Val  0xAul  /**< \brief (ADC_INPUTCTRL) ADC AIN10 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN11_Val  0xBul  /**< \brief (ADC_INPUTCTRL) ADC AIN11 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN12_Val  0xCul  /**< \brief (ADC_INPUTCTRL) ADC AIN12 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN13_Val  0xDul  /**< \brief (ADC_INPUTCTRL) ADC AIN13 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN14_Val  0xEul  /**< \brief (ADC_INPUTCTRL) ADC AIN14 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN15_Val  0xFul  /**< \brief (ADC_INPUTCTRL) ADC AIN15 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN16_Val  0x10ul  /**< \brief (ADC_INPUTCTRL) ADC AIN16 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN17_Val  0x11ul  /**< \brief (ADC_INPUTCTRL) ADC AIN17 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN18_Val  0x12ul  /**< \brief (ADC_INPUTCTRL) ADC AIN18 Pin */
#define   ADC_INPUTCTRL_MUXPOS_PIN19_Val  0x13ul  /**< \brief (ADC_INPUTCTRL) ADC AIN19 Pin */
#define   ADC_INPUTCTRL_MUXPOS_TEMP_Val   0x18ul  /**< \brief (ADC_INPUTCTRL) Temperature Reference */
#define   ADC_INPUTCTRL_MUXPOS_BANDGAP_Val 0x19ul  /**< \brief (ADC_INPUTCTRL) Bandgap Voltage */
#define   ADC_INPUTCTRL_MUXPOS_SCALEDCOREVCC_Val 0x1Aul  /**< \brief (ADC_INPUTCTRL) 1/4  Scaled Core Supply */
#define   ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC_Val 0x1Bul  /**< \brief (ADC_INPUTCTRL) 1/4  Scaled I/O Supply */
#define   ADC_INPUTCTRL_MUXPOS_DAC_Val    0x1Cul  /**< \brief (ADC_INPUTCTRL) DAC Output */
#define ADC_INPUTCTRL_MUXPOS_PIN0   (ADC_INPUTCTRL_MUXPOS_PIN0_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN1   (ADC_INPUTCTRL_MUXPOS_PIN1_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN2   (ADC_INPUTCTRL_MUXPOS_PIN2_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN3   (ADC_INPUTCTRL_MUXPOS_PIN3_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN4   (ADC_INPUTCTRL_MUXPOS_PIN4_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN5   (ADC_INPUTCTRL_MUXPOS_PIN5_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN6   (ADC_INPUTCTRL_MUXPOS_PIN6_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN7   (ADC_INPUTCTRL_MUXPOS_PIN7_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN8   (ADC_INPUTCTRL_MUXPOS_PIN8_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN9   (ADC_INPUTCTRL_MUXPOS_PIN9_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN10  (ADC_INPUTCTRL_MUXPOS_PIN10_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN11  (ADC_INPUTCTRL_MUXPOS_PIN11_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN12  (ADC_INPUTCTRL_MUXPOS_PIN12_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN13  (ADC_INPUTCTRL_MUXPOS_PIN13_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN14  (ADC_INPUTCTRL_MUXPOS_PIN14_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN15  (ADC_INPUTCTRL_MUXPOS_PIN15_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN16  (ADC_INPUTCTRL_MUXPOS_PIN16_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN17  (ADC_INPUTCTRL_MUXPOS_PIN17_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN18  (ADC_INPUTCTRL_MUXPOS_PIN18_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_PIN19  (ADC_INPUTCTRL_MUXPOS_PIN19_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_TEMP   (ADC_INPUTCTRL_MUXPOS_TEMP_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_BANDGAP (ADC_INPUTCTRL_MUXPOS_BANDGAP_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_SCALEDCOREVCC (ADC_INPUTCTRL_MUXPOS_SCALEDCOREVCC_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC (ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC_Val << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXPOS_DAC    (ADC_INPUTCTRL_MUXPOS_DAC_Val  << ADC_INPUTCTRL_MUXPOS_Pos)
#define ADC_INPUTCTRL_MUXNEG_Pos    8            /**< \brief (ADC_INPUTCTRL) Negative Mux Input Selection */
#define ADC_INPUTCTRL_MUXNEG_Msk    (0x1Ful << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG(value) (ADC_INPUTCTRL_MUXNEG_Msk & ((value) << ADC_INPUTCTRL_MUXNEG_Pos))
#define   ADC_INPUTCTRL_MUXNEG_PIN0_Val   0x0ul  /**< \brief (ADC_INPUTCTRL) ADC AIN0 Pin */
#define   ADC_INPUTCTRL_MUXNEG_PIN1_Val   0x1ul  /**< \brief (ADC_INPUTCTRL) ADC AIN1 Pin */
#define   ADC_INPUTCTRL_MUXNEG_PIN2_Val   0x2ul  /**< \brief (ADC_INPUTCTRL) ADC AIN2 Pin */
#define   ADC_INPUTCTRL_MUXNEG_PIN3_Val   0x3ul  /**< \brief (ADC_INPUTCTRL) ADC AIN3 Pin */
#define   ADC_INPUTCTRL_MUXNEG_PIN4_Val   0x4ul  /**< \brief (ADC_INPUTCTRL) ADC AIN4 Pin */
#define   ADC_INPUTCTRL_MUXNEG_PIN5_Val   0x5ul  /**< \brief (ADC_INPUTCTRL) ADC AIN5 Pin */
#define   ADC_INPUTCTRL_MUXNEG_PIN6_Val   0x6ul  /**< \brief (ADC_INPUTCTRL) ADC AIN6 Pin */
#define   ADC_INPUTCTRL_MUXNEG_PIN7_Val   0x7ul  /**< \brief (ADC_INPUTCTRL) ADC AIN7 Pin */
#define   ADC_INPUTCTRL_MUXNEG_GND_Val    0x18ul  /**< \brief (ADC_INPUTCTRL) Internal Ground */
#define   ADC_INPUTCTRL_MUXNEG_IOGND_Val  0x19ul  /**< \brief (ADC_INPUTCTRL) I/O Ground */
#define ADC_INPUTCTRL_MUXNEG_PIN0   (ADC_INPUTCTRL_MUXNEG_PIN0_Val << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG_PIN1   (ADC_INPUTCTRL_MUXNEG_PIN1_Val << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG_PIN2   (ADC_INPUTCTRL_MUXNEG_PIN2_Val << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG_PIN3   (ADC_INPUTCTRL_MUXNEG_PIN3_Val << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG_PIN4   (ADC_INPUTCTRL_MUXNEG_PIN4_Val << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG_PIN5   (ADC_INPUTCTRL_MUXNEG_PIN5_Val << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG_PIN6   (ADC_INPUTCTRL_MUXNEG_PIN6_Val << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG_PIN7   (ADC_INPUTCTRL_MUXNEG_PIN7_Val << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG_GND    (ADC_INPUTCTRL_MUXNEG_GND_Val  << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_MUXNEG_IOGND  (ADC_INPUTCTRL_MUXNEG_IOGND_Val << ADC_INPUTCTRL_MUXNEG_Pos)
#define ADC_INPUTCTRL_INPUTSCAN_Pos 16           /**< \brief (ADC_INPUTCTRL) Number of Input Channels Included in Scan */
#define ADC_INPUTCTRL_INPUTSCAN_Msk (0xFul << ADC_INPUTCTRL_INPUTSCAN_Pos)
#define ADC_INPUTCTRL_INPUTSCAN(value) (ADC_INPUTCTRL_INPUTSCAN_Msk & ((value) << ADC_INPUTCTRL_INPUTSCAN_Pos))
#define ADC_INPUTCTRL_INPUTOFFSET_Pos 20           /**< \brief (ADC_INPUTCTRL) Positive Mux Setting Offset */
#define ADC_INPUTCTRL_INPUTOFFSET_Msk (0xFul << ADC_INPUTCTRL_INPUTOFFSET_Pos)
#define ADC_INPUTCTRL_INPUTOFFSET(value) (ADC_INPUTCTRL_INPUTOFFSET_Msk & ((value) << ADC_INPUTCTRL_INPUTOFFSET_Pos))
#define ADC_INPUTCTRL_GAIN_Pos      24           /**< \brief (ADC_INPUTCTRL) Gain Factor Selection */
#define ADC_INPUTCTRL_GAIN_Msk      (0xFul << ADC_INPUTCTRL_GAIN_Pos)
#define ADC_INPUTCTRL_GAIN(value)   (ADC_INPUTCTRL_GAIN_Msk & ((value) << ADC_INPUTCTRL_GAIN_Pos))
#define   ADC_INPUTCTRL_GAIN_1X_Val       0x0ul  /**< \brief (ADC_INPUTCTRL) 1x */
#define   ADC_INPUTCTRL_GAIN_2X_Val       0x1ul  /**< \brief (ADC_INPUTCTRL) 2x */
#define   ADC_INPUTCTRL_GAIN_4X_Val       0x2ul  /**< \brief (ADC_INPUTCTRL) 4x */
#define   ADC_INPUTCTRL_GAIN_8X_Val       0x3ul  /**< \brief (ADC_INPUTCTRL) 8x */
#define   ADC_INPUTCTRL_GAIN_16X_Val      0x4ul  /**< \brief (ADC_INPUTCTRL) 16x */
#define   ADC_INPUTCTRL_GAIN_DIV2_Val     0xFul  /**< \brief (ADC_INPUTCTRL) 1/2x */
#define ADC_INPUTCTRL_GAIN_1X       (ADC_INPUTCTRL_GAIN_1X_Val     << ADC_INPUTCTRL_GAIN_Pos)
#define ADC_INPUTCTRL_GAIN_2X       (ADC_INPUTCTRL_GAIN_2X_Val     << ADC_INPUTCTRL_GAIN_Pos)
#define ADC_INPUTCTRL_GAIN_4X       (ADC_INPUTCTRL_GAIN_4X_Val     << ADC_INPUTCTRL_GAIN_Pos)
#define ADC_INPUTCTRL_GAIN_8X       (ADC_INPUTCTRL_GAIN_8X_Val     << ADC_INPUTCTRL_GAIN_Pos)
#define ADC_INPUTCTRL_GAIN_16X      (ADC_INPUTCTRL_GAIN_16X_Val    << ADC_INPUTCTRL_GAIN_Pos)
#define ADC_INPUTCTRL_GAIN_DIV2     (ADC_INPUTCTRL_GAIN_DIV2_Val   << ADC_INPUTCTRL_GAIN_Pos)
#define ADC_INPUTCTRL_MASK          0x0FFF1F1Ful /**< \brief (ADC_INPUTCTRL) MASK Register */

/* -------- ADC_EVCTRL : (ADC Offset: 0x14) (R/W  8) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  STARTEI:1;        /*!< bit:      0  Start Conversion Event In          */
    uint8_t  SYNCEI:1;         /*!< bit:      1  Synchronization Event In           */
    uint8_t  :2;               /*!< bit:  2.. 3  Reserved                           */
    uint8_t  RESRDYEO:1;       /*!< bit:      4  Result Ready Event Out             */
    uint8_t  WINMONEO:1;       /*!< bit:      5  Window Monitor Event Out           */
    uint8_t  :2;               /*!< bit:  6.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_EVCTRL_OFFSET           0x14         /**< \brief (ADC_EVCTRL offset) Event Control */
#define ADC_EVCTRL_RESETVALUE       0x00ul       /**< \brief (ADC_EVCTRL reset_value) Event Control */

#define ADC_EVCTRL_STARTEI_Pos      0            /**< \brief (ADC_EVCTRL) Start Conversion Event In */
#define ADC_EVCTRL_STARTEI          (0x1ul << ADC_EVCTRL_STARTEI_Pos)
#define ADC_EVCTRL_SYNCEI_Pos       1            /**< \brief (ADC_EVCTRL) Synchronization Event In */
#define ADC_EVCTRL_SYNCEI           (0x1ul << ADC_EVCTRL_SYNCEI_Pos)
#define ADC_EVCTRL_RESRDYEO_Pos     4            /**< \brief (ADC_EVCTRL) Result Ready Event Out */
#define ADC_EVCTRL_RESRDYEO         (0x1ul << ADC_EVCTRL_RESRDYEO_Pos)
#define ADC_EVCTRL_WINMONEO_Pos     5            /**< \brief (ADC_EVCTRL) Window Monitor Event Out */
#define ADC_EVCTRL_WINMONEO         (0x1ul << ADC_EVCTRL_WINMONEO_Pos)
#define ADC_EVCTRL_MASK             0x33ul       /**< \brief (ADC_EVCTRL) MASK Register */

/* -------- ADC_INTENCLR : (ADC Offset: 0x16) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  RESRDY:1;         /*!< bit:      0  Result Ready Interrupt Enable      */
    uint8_t  OVERRUN:1;        /*!< bit:      1  Overrun Interrupt Enable           */
    uint8_t  WINMON:1;         /*!< bit:      2  Window Monitor Interrupt Enable    */
    uint8_t  SYNCRDY:1;        /*!< bit:      3  Synchronization Ready Interrupt Enable */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_INTENCLR_OFFSET         0x16         /**< \brief (ADC_INTENCLR offset) Interrupt Enable Clear */
#define ADC_INTENCLR_RESETVALUE     0x00ul       /**< \brief (ADC_INTENCLR reset_value) Interrupt Enable Clear */

#define ADC_INTENCLR_RESRDY_Pos     0            /**< \brief (ADC_INTENCLR) Result Ready Interrupt Enable */
#define ADC_INTENCLR_RESRDY         (0x1ul << ADC_INTENCLR_RESRDY_Pos)
#define ADC_INTENCLR_OVERRUN_Pos    1            /**< \brief (ADC_INTENCLR) Overrun Interrupt Enable */
#define ADC_INTENCLR_OVERRUN        (0x1ul << ADC_INTENCLR_OVERRUN_Pos)
#define ADC_INTENCLR_WINMON_Pos     2            /**< \brief (ADC_INTENCLR) Window Monitor Interrupt Enable */
#define ADC_INTENCLR_WINMON         (0x1ul << ADC_INTENCLR_WINMON_Pos)
#define ADC_INTENCLR_SYNCRDY_Pos    3            /**< \brief (ADC_INTENCLR) Synchronization Ready Interrupt Enable */
#define ADC_INTENCLR_SYNCRDY        (0x1ul << ADC_INTENCLR_SYNCRDY_Pos)
#define ADC_INTENCLR_MASK           0x0Ful       /**< \brief (ADC_INTENCLR) MASK Register */

/* -------- ADC_INTENSET : (ADC Offset: 0x17) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  RESRDY:1;         /*!< bit:      0  Result Ready Interrupt Enable      */
    uint8_t  OVERRUN:1;        /*!< bit:      1  Overrun Interrupt Enable           */
    uint8_t  WINMON:1;         /*!< bit:      2  Window Monitor Interrupt Enable    */
    uint8_t  SYNCRDY:1;        /*!< bit:      3  Synchronization Ready Interrupt Enable */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_INTENSET_OFFSET         0x17         /**< \brief (ADC_INTENSET offset) Interrupt Enable Set */
#define ADC_INTENSET_RESETVALUE     0x00ul       /**< \brief (ADC_INTENSET reset_value) Interrupt Enable Set */

#define ADC_INTENSET_RESRDY_Pos     0            /**< \brief (ADC_INTENSET) Result Ready Interrupt Enable */
#define ADC_INTENSET_RESRDY         (0x1ul << ADC_INTENSET_RESRDY_Pos)
#define ADC_INTENSET_OVERRUN_Pos    1            /**< \brief (ADC_INTENSET) Overrun Interrupt Enable */
#define ADC_INTENSET_OVERRUN        (0x1ul << ADC_INTENSET_OVERRUN_Pos)
#define ADC_INTENSET_WINMON_Pos     2            /**< \brief (ADC_INTENSET) Window Monitor Interrupt Enable */
#define ADC_INTENSET_WINMON         (0x1ul << ADC_INTENSET_WINMON_Pos)
#define ADC_INTENSET_SYNCRDY_Pos    3            /**< \brief (ADC_INTENSET) Synchronization Ready Interrupt Enable */
#define ADC_INTENSET_SYNCRDY        (0x1ul << ADC_INTENSET_SYNCRDY_Pos)
#define ADC_INTENSET_MASK           0x0Ful       /**< \brief (ADC_INTENSET) MASK Register */

/* -------- ADC_INTFLAG : (ADC Offset: 0x18) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  RESRDY:1;         /*!< bit:      0  Result Ready                       */
    __I uint8_t  OVERRUN:1;        /*!< bit:      1  Overrun                            */
    __I uint8_t  WINMON:1;         /*!< bit:      2  Window Monitor                     */
    __I uint8_t  SYNCRDY:1;        /*!< bit:      3  Synchronization Ready              */
    __I uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_INTFLAG_OFFSET          0x18         /**< \brief (ADC_INTFLAG offset) Interrupt Flag Status and Clear */
#define ADC_INTFLAG_RESETVALUE      0x00ul       /**< \brief (ADC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define ADC_INTFLAG_RESRDY_Pos      0            /**< \brief (ADC_INTFLAG) Result Ready */
#define ADC_INTFLAG_RESRDY          (0x1ul << ADC_INTFLAG_RESRDY_Pos)
#define ADC_INTFLAG_OVERRUN_Pos     1            /**< \brief (ADC_INTFLAG) Overrun */
#define ADC_INTFLAG_OVERRUN         (0x1ul << ADC_INTFLAG_OVERRUN_Pos)
#define ADC_INTFLAG_WINMON_Pos      2            /**< \brief (ADC_INTFLAG) Window Monitor */
#define ADC_INTFLAG_WINMON          (0x1ul << ADC_INTFLAG_WINMON_Pos)
#define ADC_INTFLAG_SYNCRDY_Pos     3            /**< \brief (ADC_INTFLAG) Synchronization Ready */
#define ADC_INTFLAG_SYNCRDY         (0x1ul << ADC_INTFLAG_SYNCRDY_Pos)
#define ADC_INTFLAG_MASK            0x0Ful       /**< \brief (ADC_INTFLAG) MASK Register */

/* -------- ADC_STATUS : (ADC Offset: 0x19) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :7;               /*!< bit:  0.. 6  Reserved                           */
    uint8_t  SYNCBUSY:1;       /*!< bit:      7  Synchronization Busy               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_STATUS_OFFSET           0x19         /**< \brief (ADC_STATUS offset) Status */
#define ADC_STATUS_RESETVALUE       0x00ul       /**< \brief (ADC_STATUS reset_value) Status */

#define ADC_STATUS_SYNCBUSY_Pos     7            /**< \brief (ADC_STATUS) Synchronization Busy */
#define ADC_STATUS_SYNCBUSY         (0x1ul << ADC_STATUS_SYNCBUSY_Pos)
#define ADC_STATUS_MASK             0x80ul       /**< \brief (ADC_STATUS) MASK Register */

/* -------- ADC_RESULT : (ADC Offset: 0x1A) (R/  16) Result -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t RESULT:16;        /*!< bit:  0..15  Result Conversion Value            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} ADC_RESULT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_RESULT_OFFSET           0x1A         /**< \brief (ADC_RESULT offset) Result */
#define ADC_RESULT_RESETVALUE       0x0000ul     /**< \brief (ADC_RESULT reset_value) Result */

#define ADC_RESULT_RESULT_Pos       0            /**< \brief (ADC_RESULT) Result Conversion Value */
#define ADC_RESULT_RESULT_Msk       (0xFFFFul << ADC_RESULT_RESULT_Pos)
#define ADC_RESULT_RESULT(value)    (ADC_RESULT_RESULT_Msk & ((value) << ADC_RESULT_RESULT_Pos))
#define ADC_RESULT_MASK             0xFFFFul     /**< \brief (ADC_RESULT) MASK Register */

/* -------- ADC_WINLT : (ADC Offset: 0x1C) (R/W 16) Window Monitor Lower Threshold -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t WINLT:16;         /*!< bit:  0..15  Window Lower Threshold             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} ADC_WINLT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_WINLT_OFFSET            0x1C         /**< \brief (ADC_WINLT offset) Window Monitor Lower Threshold */
#define ADC_WINLT_RESETVALUE        0x0000ul     /**< \brief (ADC_WINLT reset_value) Window Monitor Lower Threshold */

#define ADC_WINLT_WINLT_Pos         0            /**< \brief (ADC_WINLT) Window Lower Threshold */
#define ADC_WINLT_WINLT_Msk         (0xFFFFul << ADC_WINLT_WINLT_Pos)
#define ADC_WINLT_WINLT(value)      (ADC_WINLT_WINLT_Msk & ((value) << ADC_WINLT_WINLT_Pos))
#define ADC_WINLT_MASK              0xFFFFul     /**< \brief (ADC_WINLT) MASK Register */

/* -------- ADC_WINUT : (ADC Offset: 0x20) (R/W 16) Window Monitor Upper Threshold -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t WINUT:16;         /*!< bit:  0..15  Window Upper Threshold             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} ADC_WINUT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_WINUT_OFFSET            0x20         /**< \brief (ADC_WINUT offset) Window Monitor Upper Threshold */
#define ADC_WINUT_RESETVALUE        0x0000ul     /**< \brief (ADC_WINUT reset_value) Window Monitor Upper Threshold */

#define ADC_WINUT_WINUT_Pos         0            /**< \brief (ADC_WINUT) Window Upper Threshold */
#define ADC_WINUT_WINUT_Msk         (0xFFFFul << ADC_WINUT_WINUT_Pos)
#define ADC_WINUT_WINUT(value)      (ADC_WINUT_WINUT_Msk & ((value) << ADC_WINUT_WINUT_Pos))
#define ADC_WINUT_MASK              0xFFFFul     /**< \brief (ADC_WINUT) MASK Register */

/* -------- ADC_GAINCORR : (ADC Offset: 0x24) (R/W 16) Gain Correction -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t GAINCORR:12;      /*!< bit:  0..11  Gain Correction Value              */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} ADC_GAINCORR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_GAINCORR_OFFSET         0x24         /**< \brief (ADC_GAINCORR offset) Gain Correction */
#define ADC_GAINCORR_RESETVALUE     0x0000ul     /**< \brief (ADC_GAINCORR reset_value) Gain Correction */

#define ADC_GAINCORR_GAINCORR_Pos   0            /**< \brief (ADC_GAINCORR) Gain Correction Value */
#define ADC_GAINCORR_GAINCORR_Msk   (0xFFFul << ADC_GAINCORR_GAINCORR_Pos)
#define ADC_GAINCORR_GAINCORR(value) (ADC_GAINCORR_GAINCORR_Msk & ((value) << ADC_GAINCORR_GAINCORR_Pos))
#define ADC_GAINCORR_MASK           0x0FFFul     /**< \brief (ADC_GAINCORR) MASK Register */

/* -------- ADC_OFFSETCORR : (ADC Offset: 0x26) (R/W 16) Offset Correction -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t OFFSETCORR:12;    /*!< bit:  0..11  Offset Correction Value            */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} ADC_OFFSETCORR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_OFFSETCORR_OFFSET       0x26         /**< \brief (ADC_OFFSETCORR offset) Offset Correction */
#define ADC_OFFSETCORR_RESETVALUE   0x0000ul     /**< \brief (ADC_OFFSETCORR reset_value) Offset Correction */

#define ADC_OFFSETCORR_OFFSETCORR_Pos 0            /**< \brief (ADC_OFFSETCORR) Offset Correction Value */
#define ADC_OFFSETCORR_OFFSETCORR_Msk (0xFFFul << ADC_OFFSETCORR_OFFSETCORR_Pos)
#define ADC_OFFSETCORR_OFFSETCORR(value) (ADC_OFFSETCORR_OFFSETCORR_Msk & ((value) << ADC_OFFSETCORR_OFFSETCORR_Pos))
#define ADC_OFFSETCORR_MASK         0x0FFFul     /**< \brief (ADC_OFFSETCORR) MASK Register */

/* -------- ADC_CALIB : (ADC Offset: 0x28) (R/W 16) Calibration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t LINEARITY_CAL:8;  /*!< bit:  0.. 7  Linearity Calibration Value        */
    uint16_t BIAS_CAL:3;       /*!< bit:  8..10  Bias Calibration Value             */
    uint16_t :5;               /*!< bit: 11..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} ADC_CALIB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_CALIB_OFFSET            0x28         /**< \brief (ADC_CALIB offset) Calibration */
#define ADC_CALIB_RESETVALUE        0x0000ul     /**< \brief (ADC_CALIB reset_value) Calibration */

#define ADC_CALIB_LINEARITY_CAL_Pos 0            /**< \brief (ADC_CALIB) Linearity Calibration Value */
#define ADC_CALIB_LINEARITY_CAL_Msk (0xFFul << ADC_CALIB_LINEARITY_CAL_Pos)
#define ADC_CALIB_LINEARITY_CAL(value) (ADC_CALIB_LINEARITY_CAL_Msk & ((value) << ADC_CALIB_LINEARITY_CAL_Pos))
#define ADC_CALIB_BIAS_CAL_Pos      8            /**< \brief (ADC_CALIB) Bias Calibration Value */
#define ADC_CALIB_BIAS_CAL_Msk      (0x7ul << ADC_CALIB_BIAS_CAL_Pos)
#define ADC_CALIB_BIAS_CAL(value)   (ADC_CALIB_BIAS_CAL_Msk & ((value) << ADC_CALIB_BIAS_CAL_Pos))
#define ADC_CALIB_MASK              0x07FFul     /**< \brief (ADC_CALIB) MASK Register */

/* -------- ADC_DBGCTRL : (ADC Offset: 0x2A) (R/W  8) Debug Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DBGRUN:1;         /*!< bit:      0  Debug Run                          */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} ADC_DBGCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define ADC_DBGCTRL_OFFSET          0x2A         /**< \brief (ADC_DBGCTRL offset) Debug Control */
#define ADC_DBGCTRL_RESETVALUE      0x00ul       /**< \brief (ADC_DBGCTRL reset_value) Debug Control */

#define ADC_DBGCTRL_DBGRUN_Pos      0            /**< \brief (ADC_DBGCTRL) Debug Run */
#define ADC_DBGCTRL_DBGRUN          (0x1ul << ADC_DBGCTRL_DBGRUN_Pos)
#define ADC_DBGCTRL_MASK            0x01ul       /**< \brief (ADC_DBGCTRL) MASK Register */

/** \brief ADC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO ADC_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control A */
  __IO ADC_REFCTRL_Type          REFCTRL;     /**< \brief Offset: 0x01 (R/W  8) Reference Control */
  __IO ADC_AVGCTRL_Type          AVGCTRL;     /**< \brief Offset: 0x02 (R/W  8) Average Control */
  __IO ADC_SAMPCTRL_Type         SAMPCTRL;    /**< \brief Offset: 0x03 (R/W  8) Sampling Time Control */
  __IO ADC_CTRLB_Type            CTRLB;       /**< \brief Offset: 0x04 (R/W 16) Control B */
       RoReg8                    Reserved1[0x2];
  __IO ADC_WINCTRL_Type          WINCTRL;     /**< \brief Offset: 0x08 (R/W  8) Window Monitor Control */
       RoReg8                    Reserved2[0x3];
  __IO ADC_SWTRIG_Type           SWTRIG;      /**< \brief Offset: 0x0C (R/W  8) Software Trigger */
       RoReg8                    Reserved3[0x3];
  __IO ADC_INPUTCTRL_Type        INPUTCTRL;   /**< \brief Offset: 0x10 (R/W 32) Input Control */
  __IO ADC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x14 (R/W  8) Event Control */
       RoReg8                    Reserved4[0x1];
  __IO ADC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x16 (R/W  8) Interrupt Enable Clear */
  __IO ADC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x17 (R/W  8) Interrupt Enable Set */
  __IO ADC_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x18 (R/W  8) Interrupt Flag Status and Clear */
  __I  ADC_STATUS_Type           STATUS;      /**< \brief Offset: 0x19 (R/   8) Status */
  __I  ADC_RESULT_Type           RESULT;      /**< \brief Offset: 0x1A (R/  16) Result */
  __IO ADC_WINLT_Type            WINLT;       /**< \brief Offset: 0x1C (R/W 16) Window Monitor Lower Threshold */
       RoReg8                    Reserved5[0x2];
  __IO ADC_WINUT_Type            WINUT;       /**< \brief Offset: 0x20 (R/W 16) Window Monitor Upper Threshold */
       RoReg8                    Reserved6[0x2];
  __IO ADC_GAINCORR_Type         GAINCORR;    /**< \brief Offset: 0x24 (R/W 16) Gain Correction */
  __IO ADC_OFFSETCORR_Type       OFFSETCORR;  /**< \brief Offset: 0x26 (R/W 16) Offset Correction */
  __IO ADC_CALIB_Type            CALIB;       /**< \brief Offset: 0x28 (R/W 16) Calibration */
  __IO ADC_DBGCTRL_Type          DBGCTRL;     /**< \brief Offset: 0x2A (R/W  8) Debug Control */
} Adc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_ADC_COMPONENT_ */
