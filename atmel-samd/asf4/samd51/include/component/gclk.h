/**
 * \file
 *
 * \brief Component description for GCLK
 *
 * Copyright (c) 2017 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD51_GCLK_COMPONENT_
#define _SAMD51_GCLK_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR GCLK */
/* ========================================================================== */
/** \addtogroup SAMD51_GCLK Generic Clock Generator */
/*@{*/

#define GCLK_U2122
#define REV_GCLK                    0x120

/* -------- GCLK_CTRLA : (GCLK Offset: 0x00) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} GCLK_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_CTRLA_OFFSET           0x00         /**< \brief (GCLK_CTRLA offset) Control */
#define GCLK_CTRLA_RESETVALUE       _U_(0x00)    /**< \brief (GCLK_CTRLA reset_value) Control */

#define GCLK_CTRLA_SWRST_Pos        0            /**< \brief (GCLK_CTRLA) Software Reset */
#define GCLK_CTRLA_SWRST            (_U_(0x1) << GCLK_CTRLA_SWRST_Pos)
#define GCLK_CTRLA_MASK             _U_(0x01)    /**< \brief (GCLK_CTRLA) MASK Register */

/* -------- GCLK_SYNCBUSY : (GCLK Offset: 0x04) (R/  32) Synchronization Busy -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SWRST:1;          /*!< bit:      0  Software Reset Synchroniation Busy bit */
    uint32_t :1;               /*!< bit:      1  Reserved                           */
    uint32_t GENCTRL0:1;       /*!< bit:      2  Generic Clock Generator Control 0 Synchronization Busy bits */
    uint32_t GENCTRL1:1;       /*!< bit:      3  Generic Clock Generator Control 1 Synchronization Busy bits */
    uint32_t GENCTRL2:1;       /*!< bit:      4  Generic Clock Generator Control 2 Synchronization Busy bits */
    uint32_t GENCTRL3:1;       /*!< bit:      5  Generic Clock Generator Control 3 Synchronization Busy bits */
    uint32_t GENCTRL4:1;       /*!< bit:      6  Generic Clock Generator Control 4 Synchronization Busy bits */
    uint32_t GENCTRL5:1;       /*!< bit:      7  Generic Clock Generator Control 5 Synchronization Busy bits */
    uint32_t GENCTRL6:1;       /*!< bit:      8  Generic Clock Generator Control 6 Synchronization Busy bits */
    uint32_t GENCTRL7:1;       /*!< bit:      9  Generic Clock Generator Control 7 Synchronization Busy bits */
    uint32_t GENCTRL8:1;       /*!< bit:     10  Generic Clock Generator Control 8 Synchronization Busy bits */
    uint32_t GENCTRL9:1;       /*!< bit:     11  Generic Clock Generator Control 9 Synchronization Busy bits */
    uint32_t GENCTRL10:1;      /*!< bit:     12  Generic Clock Generator Control 10 Synchronization Busy bits */
    uint32_t GENCTRL11:1;      /*!< bit:     13  Generic Clock Generator Control 11 Synchronization Busy bits */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint32_t GENCTRL:12;       /*!< bit:  2..13  Generic Clock Generator Control x Synchronization Busy bits */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_SYNCBUSY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_SYNCBUSY_OFFSET        0x04         /**< \brief (GCLK_SYNCBUSY offset) Synchronization Busy */
#define GCLK_SYNCBUSY_RESETVALUE    _U_(0x00000000) /**< \brief (GCLK_SYNCBUSY reset_value) Synchronization Busy */

#define GCLK_SYNCBUSY_SWRST_Pos     0            /**< \brief (GCLK_SYNCBUSY) Software Reset Synchroniation Busy bit */
#define GCLK_SYNCBUSY_SWRST         (_U_(0x1) << GCLK_SYNCBUSY_SWRST_Pos)
#define GCLK_SYNCBUSY_GENCTRL0_Pos  2            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 0 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL0      (_U_(1) << GCLK_SYNCBUSY_GENCTRL0_Pos)
#define GCLK_SYNCBUSY_GENCTRL1_Pos  3            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 1 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL1      (_U_(1) << GCLK_SYNCBUSY_GENCTRL1_Pos)
#define GCLK_SYNCBUSY_GENCTRL2_Pos  4            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 2 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL2      (_U_(1) << GCLK_SYNCBUSY_GENCTRL2_Pos)
#define GCLK_SYNCBUSY_GENCTRL3_Pos  5            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 3 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL3      (_U_(1) << GCLK_SYNCBUSY_GENCTRL3_Pos)
#define GCLK_SYNCBUSY_GENCTRL4_Pos  6            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 4 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL4      (_U_(1) << GCLK_SYNCBUSY_GENCTRL4_Pos)
#define GCLK_SYNCBUSY_GENCTRL5_Pos  7            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 5 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL5      (_U_(1) << GCLK_SYNCBUSY_GENCTRL5_Pos)
#define GCLK_SYNCBUSY_GENCTRL6_Pos  8            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 6 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL6      (_U_(1) << GCLK_SYNCBUSY_GENCTRL6_Pos)
#define GCLK_SYNCBUSY_GENCTRL7_Pos  9            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 7 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL7      (_U_(1) << GCLK_SYNCBUSY_GENCTRL7_Pos)
#define GCLK_SYNCBUSY_GENCTRL8_Pos  10           /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 8 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL8      (_U_(1) << GCLK_SYNCBUSY_GENCTRL8_Pos)
#define GCLK_SYNCBUSY_GENCTRL9_Pos  11           /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 9 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL9      (_U_(1) << GCLK_SYNCBUSY_GENCTRL9_Pos)
#define GCLK_SYNCBUSY_GENCTRL10_Pos 12           /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 10 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL10     (_U_(1) << GCLK_SYNCBUSY_GENCTRL10_Pos)
#define GCLK_SYNCBUSY_GENCTRL11_Pos 13           /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control 11 Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL11     (_U_(1) << GCLK_SYNCBUSY_GENCTRL11_Pos)
#define GCLK_SYNCBUSY_GENCTRL_Pos   2            /**< \brief (GCLK_SYNCBUSY) Generic Clock Generator Control x Synchronization Busy bits */
#define GCLK_SYNCBUSY_GENCTRL_Msk   (_U_(0xFFF) << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL(value) (GCLK_SYNCBUSY_GENCTRL_Msk & ((value) << GCLK_SYNCBUSY_GENCTRL_Pos))
#define   GCLK_SYNCBUSY_GENCTRL_GCLK0_Val _U_(0x1)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 0 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK1_Val _U_(0x2)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 1 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK2_Val _U_(0x4)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 2 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK3_Val _U_(0x8)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 3 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK4_Val _U_(0x10)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 4 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK5_Val _U_(0x20)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 5 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK6_Val _U_(0x40)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 6 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK7_Val _U_(0x80)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 7 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK8_Val _U_(0x100)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 8 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK9_Val _U_(0x200)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 9 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK10_Val _U_(0x400)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 10 */
#define   GCLK_SYNCBUSY_GENCTRL_GCLK11_Val _U_(0x800)   /**< \brief (GCLK_SYNCBUSY) Generic clock generator 11 */
#define GCLK_SYNCBUSY_GENCTRL_GCLK0 (GCLK_SYNCBUSY_GENCTRL_GCLK0_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK1 (GCLK_SYNCBUSY_GENCTRL_GCLK1_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK2 (GCLK_SYNCBUSY_GENCTRL_GCLK2_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK3 (GCLK_SYNCBUSY_GENCTRL_GCLK3_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK4 (GCLK_SYNCBUSY_GENCTRL_GCLK4_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK5 (GCLK_SYNCBUSY_GENCTRL_GCLK5_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK6 (GCLK_SYNCBUSY_GENCTRL_GCLK6_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK7 (GCLK_SYNCBUSY_GENCTRL_GCLK7_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK8 (GCLK_SYNCBUSY_GENCTRL_GCLK8_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK9 (GCLK_SYNCBUSY_GENCTRL_GCLK9_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK10 (GCLK_SYNCBUSY_GENCTRL_GCLK10_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_GENCTRL_GCLK11 (GCLK_SYNCBUSY_GENCTRL_GCLK11_Val << GCLK_SYNCBUSY_GENCTRL_Pos)
#define GCLK_SYNCBUSY_MASK          _U_(0x00003FFD) /**< \brief (GCLK_SYNCBUSY) MASK Register */

/* -------- GCLK_GENCTRL : (GCLK Offset: 0x20) (R/W 32) Generic Clock Generator Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SRC:4;            /*!< bit:  0.. 3  Source Select                      */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t GENEN:1;          /*!< bit:      8  Generic Clock Generator Enable     */
    uint32_t IDC:1;            /*!< bit:      9  Improve Duty Cycle                 */
    uint32_t OOV:1;            /*!< bit:     10  Output Off Value                   */
    uint32_t OE:1;             /*!< bit:     11  Output Enable                      */
    uint32_t DIVSEL:1;         /*!< bit:     12  Divide Selection                   */
    uint32_t RUNSTDBY:1;       /*!< bit:     13  Run in Standby                     */
    uint32_t :2;               /*!< bit: 14..15  Reserved                           */
    uint32_t DIV:16;           /*!< bit: 16..31  Division Factor                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_GENCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_GENCTRL_OFFSET         0x20         /**< \brief (GCLK_GENCTRL offset) Generic Clock Generator Control */
#define GCLK_GENCTRL_RESETVALUE     _U_(0x00000000) /**< \brief (GCLK_GENCTRL reset_value) Generic Clock Generator Control */

#define GCLK_GENCTRL_SRC_Pos        0            /**< \brief (GCLK_GENCTRL) Source Select */
#define GCLK_GENCTRL_SRC_Msk        (_U_(0xF) << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC(value)     (GCLK_GENCTRL_SRC_Msk & ((value) << GCLK_GENCTRL_SRC_Pos))
#define   GCLK_GENCTRL_SRC_XOSC0_Val      _U_(0x0)   /**< \brief (GCLK_GENCTRL) XOSC0 oscillator output */
#define   GCLK_GENCTRL_SRC_XOSC1_Val      _U_(0x1)   /**< \brief (GCLK_GENCTRL) XOSC1 oscillator output */
#define   GCLK_GENCTRL_SRC_GCLKIN_Val     _U_(0x2)   /**< \brief (GCLK_GENCTRL) Generator input pad */
#define   GCLK_GENCTRL_SRC_GCLKGEN1_Val   _U_(0x3)   /**< \brief (GCLK_GENCTRL) Generic clock generator 1 output */
#define   GCLK_GENCTRL_SRC_OSCULP32K_Val  _U_(0x4)   /**< \brief (GCLK_GENCTRL) OSCULP32K oscillator output */
#define   GCLK_GENCTRL_SRC_XOSC32K_Val    _U_(0x5)   /**< \brief (GCLK_GENCTRL) XOSC32K oscillator output */
#define   GCLK_GENCTRL_SRC_DFLL_Val       _U_(0x6)   /**< \brief (GCLK_GENCTRL) DFLL output */
#define   GCLK_GENCTRL_SRC_DPLL0_Val      _U_(0x7)   /**< \brief (GCLK_GENCTRL) DPLL0 output */
#define   GCLK_GENCTRL_SRC_DPLL1_Val      _U_(0x8)   /**< \brief (GCLK_GENCTRL) DPLL1 output */
#define GCLK_GENCTRL_SRC_XOSC0      (GCLK_GENCTRL_SRC_XOSC0_Val    << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_XOSC1      (GCLK_GENCTRL_SRC_XOSC1_Val    << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_GCLKIN     (GCLK_GENCTRL_SRC_GCLKIN_Val   << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_GCLKGEN1   (GCLK_GENCTRL_SRC_GCLKGEN1_Val << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSCULP32K  (GCLK_GENCTRL_SRC_OSCULP32K_Val << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_XOSC32K    (GCLK_GENCTRL_SRC_XOSC32K_Val  << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_DFLL       (GCLK_GENCTRL_SRC_DFLL_Val     << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_DPLL0      (GCLK_GENCTRL_SRC_DPLL0_Val    << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_DPLL1      (GCLK_GENCTRL_SRC_DPLL1_Val    << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_GENEN_Pos      8            /**< \brief (GCLK_GENCTRL) Generic Clock Generator Enable */
#define GCLK_GENCTRL_GENEN          (_U_(0x1) << GCLK_GENCTRL_GENEN_Pos)
#define GCLK_GENCTRL_IDC_Pos        9            /**< \brief (GCLK_GENCTRL) Improve Duty Cycle */
#define GCLK_GENCTRL_IDC            (_U_(0x1) << GCLK_GENCTRL_IDC_Pos)
#define GCLK_GENCTRL_OOV_Pos        10           /**< \brief (GCLK_GENCTRL) Output Off Value */
#define GCLK_GENCTRL_OOV            (_U_(0x1) << GCLK_GENCTRL_OOV_Pos)
#define GCLK_GENCTRL_OE_Pos         11           /**< \brief (GCLK_GENCTRL) Output Enable */
#define GCLK_GENCTRL_OE             (_U_(0x1) << GCLK_GENCTRL_OE_Pos)
#define GCLK_GENCTRL_DIVSEL_Pos     12           /**< \brief (GCLK_GENCTRL) Divide Selection */
#define GCLK_GENCTRL_DIVSEL         (_U_(0x1) << GCLK_GENCTRL_DIVSEL_Pos)
#define GCLK_GENCTRL_RUNSTDBY_Pos   13           /**< \brief (GCLK_GENCTRL) Run in Standby */
#define GCLK_GENCTRL_RUNSTDBY       (_U_(0x1) << GCLK_GENCTRL_RUNSTDBY_Pos)
#define GCLK_GENCTRL_DIV_Pos        16           /**< \brief (GCLK_GENCTRL) Division Factor */
#define GCLK_GENCTRL_DIV_Msk        (_U_(0xFFFF) << GCLK_GENCTRL_DIV_Pos)
#define GCLK_GENCTRL_DIV(value)     (GCLK_GENCTRL_DIV_Msk & ((value) << GCLK_GENCTRL_DIV_Pos))
#define GCLK_GENCTRL_MASK           _U_(0xFFFF3F0F) /**< \brief (GCLK_GENCTRL) MASK Register */

/* -------- GCLK_PCHCTRL : (GCLK Offset: 0x80) (R/W 32) Peripheral Clock Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t GEN:4;            /*!< bit:  0.. 3  Generic Clock Generator            */
    uint32_t :2;               /*!< bit:  4.. 5  Reserved                           */
    uint32_t CHEN:1;           /*!< bit:      6  Channel Enable                     */
    uint32_t WRTLOCK:1;        /*!< bit:      7  Write Lock                         */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_PCHCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_PCHCTRL_OFFSET         0x80         /**< \brief (GCLK_PCHCTRL offset) Peripheral Clock Control */
#define GCLK_PCHCTRL_RESETVALUE     _U_(0x00000000) /**< \brief (GCLK_PCHCTRL reset_value) Peripheral Clock Control */

#define GCLK_PCHCTRL_GEN_Pos        0            /**< \brief (GCLK_PCHCTRL) Generic Clock Generator */
#define GCLK_PCHCTRL_GEN_Msk        (_U_(0xF) << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN(value)     (GCLK_PCHCTRL_GEN_Msk & ((value) << GCLK_PCHCTRL_GEN_Pos))
#define   GCLK_PCHCTRL_GEN_GCLK0_Val      _U_(0x0)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 0 */
#define   GCLK_PCHCTRL_GEN_GCLK1_Val      _U_(0x1)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 1 */
#define   GCLK_PCHCTRL_GEN_GCLK2_Val      _U_(0x2)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 2 */
#define   GCLK_PCHCTRL_GEN_GCLK3_Val      _U_(0x3)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 3 */
#define   GCLK_PCHCTRL_GEN_GCLK4_Val      _U_(0x4)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 4 */
#define   GCLK_PCHCTRL_GEN_GCLK5_Val      _U_(0x5)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 5 */
#define   GCLK_PCHCTRL_GEN_GCLK6_Val      _U_(0x6)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 6 */
#define   GCLK_PCHCTRL_GEN_GCLK7_Val      _U_(0x7)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 7 */
#define   GCLK_PCHCTRL_GEN_GCLK8_Val      _U_(0x8)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 8 */
#define   GCLK_PCHCTRL_GEN_GCLK9_Val      _U_(0x9)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 9 */
#define   GCLK_PCHCTRL_GEN_GCLK10_Val     _U_(0xA)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 10 */
#define   GCLK_PCHCTRL_GEN_GCLK11_Val     _U_(0xB)   /**< \brief (GCLK_PCHCTRL) Generic clock generator 11 */
#define GCLK_PCHCTRL_GEN_GCLK0      (GCLK_PCHCTRL_GEN_GCLK0_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK1      (GCLK_PCHCTRL_GEN_GCLK1_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK2      (GCLK_PCHCTRL_GEN_GCLK2_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK3      (GCLK_PCHCTRL_GEN_GCLK3_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK4      (GCLK_PCHCTRL_GEN_GCLK4_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK5      (GCLK_PCHCTRL_GEN_GCLK5_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK6      (GCLK_PCHCTRL_GEN_GCLK6_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK7      (GCLK_PCHCTRL_GEN_GCLK7_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK8      (GCLK_PCHCTRL_GEN_GCLK8_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK9      (GCLK_PCHCTRL_GEN_GCLK9_Val    << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK10     (GCLK_PCHCTRL_GEN_GCLK10_Val   << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_GEN_GCLK11     (GCLK_PCHCTRL_GEN_GCLK11_Val   << GCLK_PCHCTRL_GEN_Pos)
#define GCLK_PCHCTRL_CHEN_Pos       6            /**< \brief (GCLK_PCHCTRL) Channel Enable */
#define GCLK_PCHCTRL_CHEN           (_U_(0x1) << GCLK_PCHCTRL_CHEN_Pos)
#define GCLK_PCHCTRL_WRTLOCK_Pos    7            /**< \brief (GCLK_PCHCTRL) Write Lock */
#define GCLK_PCHCTRL_WRTLOCK        (_U_(0x1) << GCLK_PCHCTRL_WRTLOCK_Pos)
#define GCLK_PCHCTRL_MASK           _U_(0x000000CF) /**< \brief (GCLK_PCHCTRL) MASK Register */

/** \brief GCLK hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO GCLK_CTRLA_Type           CTRLA;       /**< \brief Offset: 0x00 (R/W  8) Control */
       RoReg8                    Reserved1[0x3];
  __I  GCLK_SYNCBUSY_Type        SYNCBUSY;    /**< \brief Offset: 0x04 (R/  32) Synchronization Busy */
       RoReg8                    Reserved2[0x18];
  __IO GCLK_GENCTRL_Type         GENCTRL[12]; /**< \brief Offset: 0x20 (R/W 32) Generic Clock Generator Control */
       RoReg8                    Reserved3[0x30];
  __IO GCLK_PCHCTRL_Type         PCHCTRL[48]; /**< \brief Offset: 0x80 (R/W 32) Peripheral Clock Control */
} Gclk;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD51_GCLK_COMPONENT_ */
