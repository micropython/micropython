/**
 * \file
 *
 * \brief Component description for GCLK
 *
 * Copyright (c) 2016 Atmel Corporation,
 *                    a wholly owned subsidiary of Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21_GCLK_COMPONENT_
#define _SAMD21_GCLK_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR GCLK */
/* ========================================================================== */
/** \addtogroup SAMD21_GCLK Generic Clock Generator */
/*@{*/

#define GCLK_U2102
#define REV_GCLK                    0x210

/* -------- GCLK_CTRL : (GCLK Offset: 0x0) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} GCLK_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_CTRL_OFFSET            0x0          /**< \brief (GCLK_CTRL offset) Control */
#define GCLK_CTRL_RESETVALUE        _U(0x00)     /**< \brief (GCLK_CTRL reset_value) Control */

#define GCLK_CTRL_SWRST_Pos         0            /**< \brief (GCLK_CTRL) Software Reset */
#define GCLK_CTRL_SWRST             (_U(0x1) << GCLK_CTRL_SWRST_Pos)
#define GCLK_CTRL_MASK              _U(0x01)     /**< \brief (GCLK_CTRL) MASK Register */

/* -------- GCLK_STATUS : (GCLK Offset: 0x1) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :7;               /*!< bit:  0.. 6  Reserved                           */
    uint8_t  SYNCBUSY:1;       /*!< bit:      7  Synchronization Busy Status        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} GCLK_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_STATUS_OFFSET          0x1          /**< \brief (GCLK_STATUS offset) Status */
#define GCLK_STATUS_RESETVALUE      _U(0x00)     /**< \brief (GCLK_STATUS reset_value) Status */

#define GCLK_STATUS_SYNCBUSY_Pos    7            /**< \brief (GCLK_STATUS) Synchronization Busy Status */
#define GCLK_STATUS_SYNCBUSY        (_U(0x1) << GCLK_STATUS_SYNCBUSY_Pos)
#define GCLK_STATUS_MASK            _U(0x80)     /**< \brief (GCLK_STATUS) MASK Register */

/* -------- GCLK_CLKCTRL : (GCLK Offset: 0x2) (R/W 16) Generic Clock Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t ID:6;             /*!< bit:  0.. 5  Generic Clock Selection ID         */
    uint16_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint16_t GEN:4;            /*!< bit:  8..11  Generic Clock Generator            */
    uint16_t :2;               /*!< bit: 12..13  Reserved                           */
    uint16_t CLKEN:1;          /*!< bit:     14  Clock Enable                       */
    uint16_t WRTLOCK:1;        /*!< bit:     15  Write Lock                         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} GCLK_CLKCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_CLKCTRL_OFFSET         0x2          /**< \brief (GCLK_CLKCTRL offset) Generic Clock Control */
#define GCLK_CLKCTRL_RESETVALUE     _U(0x0000)   /**< \brief (GCLK_CLKCTRL reset_value) Generic Clock Control */

#define GCLK_CLKCTRL_ID_Pos         0            /**< \brief (GCLK_CLKCTRL) Generic Clock Selection ID */
#define GCLK_CLKCTRL_ID_Msk         (_U(0x3F) << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID(value)      (GCLK_CLKCTRL_ID_Msk & ((value) << GCLK_CLKCTRL_ID_Pos))
#define   GCLK_CLKCTRL_ID_DFLL48_Val      _U(0x0)   /**< \brief (GCLK_CLKCTRL) DFLL48 */
#define   GCLK_CLKCTRL_ID_FDPLL_Val       _U(0x1)   /**< \brief (GCLK_CLKCTRL) FDPLL */
#define   GCLK_CLKCTRL_ID_FDPLL32K_Val    _U(0x2)   /**< \brief (GCLK_CLKCTRL) FDPLL32K */
#define   GCLK_CLKCTRL_ID_WDT_Val         _U(0x3)   /**< \brief (GCLK_CLKCTRL) WDT */
#define   GCLK_CLKCTRL_ID_RTC_Val         _U(0x4)   /**< \brief (GCLK_CLKCTRL) RTC */
#define   GCLK_CLKCTRL_ID_EIC_Val         _U(0x5)   /**< \brief (GCLK_CLKCTRL) EIC */
#define   GCLK_CLKCTRL_ID_USB_Val         _U(0x6)   /**< \brief (GCLK_CLKCTRL) USB */
#define   GCLK_CLKCTRL_ID_EVSYS_0_Val     _U(0x7)   /**< \brief (GCLK_CLKCTRL) EVSYS_0 */
#define   GCLK_CLKCTRL_ID_EVSYS_1_Val     _U(0x8)   /**< \brief (GCLK_CLKCTRL) EVSYS_1 */
#define   GCLK_CLKCTRL_ID_EVSYS_2_Val     _U(0x9)   /**< \brief (GCLK_CLKCTRL) EVSYS_2 */
#define   GCLK_CLKCTRL_ID_EVSYS_3_Val     _U(0xA)   /**< \brief (GCLK_CLKCTRL) EVSYS_3 */
#define   GCLK_CLKCTRL_ID_EVSYS_4_Val     _U(0xB)   /**< \brief (GCLK_CLKCTRL) EVSYS_4 */
#define   GCLK_CLKCTRL_ID_EVSYS_5_Val     _U(0xC)   /**< \brief (GCLK_CLKCTRL) EVSYS_5 */
#define   GCLK_CLKCTRL_ID_EVSYS_6_Val     _U(0xD)   /**< \brief (GCLK_CLKCTRL) EVSYS_6 */
#define   GCLK_CLKCTRL_ID_EVSYS_7_Val     _U(0xE)   /**< \brief (GCLK_CLKCTRL) EVSYS_7 */
#define   GCLK_CLKCTRL_ID_EVSYS_8_Val     _U(0xF)   /**< \brief (GCLK_CLKCTRL) EVSYS_8 */
#define   GCLK_CLKCTRL_ID_EVSYS_9_Val     _U(0x10)   /**< \brief (GCLK_CLKCTRL) EVSYS_9 */
#define   GCLK_CLKCTRL_ID_EVSYS_10_Val    _U(0x11)   /**< \brief (GCLK_CLKCTRL) EVSYS_10 */
#define   GCLK_CLKCTRL_ID_EVSYS_11_Val    _U(0x12)   /**< \brief (GCLK_CLKCTRL) EVSYS_11 */
#define   GCLK_CLKCTRL_ID_SERCOMX_SLOW_Val _U(0x13)   /**< \brief (GCLK_CLKCTRL) SERCOMX_SLOW */
#define   GCLK_CLKCTRL_ID_SERCOM0_CORE_Val _U(0x14)   /**< \brief (GCLK_CLKCTRL) SERCOM0_CORE */
#define   GCLK_CLKCTRL_ID_SERCOM1_CORE_Val _U(0x15)   /**< \brief (GCLK_CLKCTRL) SERCOM1_CORE */
#define   GCLK_CLKCTRL_ID_SERCOM2_CORE_Val _U(0x16)   /**< \brief (GCLK_CLKCTRL) SERCOM2_CORE */
#define   GCLK_CLKCTRL_ID_SERCOM3_CORE_Val _U(0x17)   /**< \brief (GCLK_CLKCTRL) SERCOM3_CORE */
#define   GCLK_CLKCTRL_ID_SERCOM4_CORE_Val _U(0x18)   /**< \brief (GCLK_CLKCTRL) SERCOM4_CORE */
#define   GCLK_CLKCTRL_ID_SERCOM5_CORE_Val _U(0x19)   /**< \brief (GCLK_CLKCTRL) SERCOM5_CORE */
#define   GCLK_CLKCTRL_ID_TCC0_TCC1_Val   _U(0x1A)   /**< \brief (GCLK_CLKCTRL) TCC0_TCC1 */
#define   GCLK_CLKCTRL_ID_TCC2_TC3_Val    _U(0x1B)   /**< \brief (GCLK_CLKCTRL) TCC2_TC3 */
#define   GCLK_CLKCTRL_ID_TC4_TC5_Val     _U(0x1C)   /**< \brief (GCLK_CLKCTRL) TC4_TC5 */
#define   GCLK_CLKCTRL_ID_TC6_TC7_Val     _U(0x1D)   /**< \brief (GCLK_CLKCTRL) TC6_TC7 */
#define   GCLK_CLKCTRL_ID_ADC_Val         _U(0x1E)   /**< \brief (GCLK_CLKCTRL) ADC */
#define   GCLK_CLKCTRL_ID_AC_DIG_Val      _U(0x1F)   /**< \brief (GCLK_CLKCTRL) AC_DIG */
#define   GCLK_CLKCTRL_ID_AC_ANA_Val      _U(0x20)   /**< \brief (GCLK_CLKCTRL) AC_ANA */
#define   GCLK_CLKCTRL_ID_DAC_Val         _U(0x21)   /**< \brief (GCLK_CLKCTRL) DAC */
#define   GCLK_CLKCTRL_ID_PTC_Val         _U(0x22)   /**< \brief (GCLK_CLKCTRL) PTC */
#define   GCLK_CLKCTRL_ID_I2S_0_Val       _U(0x23)   /**< \brief (GCLK_CLKCTRL) I2S_0 */
#define   GCLK_CLKCTRL_ID_I2S_1_Val       _U(0x24)   /**< \brief (GCLK_CLKCTRL) I2S_1 */
#define GCLK_CLKCTRL_ID_DFLL48      (GCLK_CLKCTRL_ID_DFLL48_Val    << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_FDPLL       (GCLK_CLKCTRL_ID_FDPLL_Val     << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_FDPLL32K    (GCLK_CLKCTRL_ID_FDPLL32K_Val  << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_WDT         (GCLK_CLKCTRL_ID_WDT_Val       << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_RTC         (GCLK_CLKCTRL_ID_RTC_Val       << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EIC         (GCLK_CLKCTRL_ID_EIC_Val       << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_USB         (GCLK_CLKCTRL_ID_USB_Val       << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_0     (GCLK_CLKCTRL_ID_EVSYS_0_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_1     (GCLK_CLKCTRL_ID_EVSYS_1_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_2     (GCLK_CLKCTRL_ID_EVSYS_2_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_3     (GCLK_CLKCTRL_ID_EVSYS_3_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_4     (GCLK_CLKCTRL_ID_EVSYS_4_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_5     (GCLK_CLKCTRL_ID_EVSYS_5_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_6     (GCLK_CLKCTRL_ID_EVSYS_6_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_7     (GCLK_CLKCTRL_ID_EVSYS_7_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_8     (GCLK_CLKCTRL_ID_EVSYS_8_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_9     (GCLK_CLKCTRL_ID_EVSYS_9_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_10    (GCLK_CLKCTRL_ID_EVSYS_10_Val  << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_EVSYS_11    (GCLK_CLKCTRL_ID_EVSYS_11_Val  << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_SERCOMX_SLOW (GCLK_CLKCTRL_ID_SERCOMX_SLOW_Val << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_SERCOM0_CORE (GCLK_CLKCTRL_ID_SERCOM0_CORE_Val << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_SERCOM1_CORE (GCLK_CLKCTRL_ID_SERCOM1_CORE_Val << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_SERCOM2_CORE (GCLK_CLKCTRL_ID_SERCOM2_CORE_Val << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_SERCOM3_CORE (GCLK_CLKCTRL_ID_SERCOM3_CORE_Val << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_SERCOM4_CORE (GCLK_CLKCTRL_ID_SERCOM4_CORE_Val << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_SERCOM5_CORE (GCLK_CLKCTRL_ID_SERCOM5_CORE_Val << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_TCC0_TCC1   (GCLK_CLKCTRL_ID_TCC0_TCC1_Val << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_TCC2_TC3    (GCLK_CLKCTRL_ID_TCC2_TC3_Val  << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_TC4_TC5     (GCLK_CLKCTRL_ID_TC4_TC5_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_TC6_TC7     (GCLK_CLKCTRL_ID_TC6_TC7_Val   << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_ADC         (GCLK_CLKCTRL_ID_ADC_Val       << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_AC_DIG      (GCLK_CLKCTRL_ID_AC_DIG_Val    << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_AC_ANA      (GCLK_CLKCTRL_ID_AC_ANA_Val    << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_DAC         (GCLK_CLKCTRL_ID_DAC_Val       << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_PTC         (GCLK_CLKCTRL_ID_PTC_Val       << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_I2S_0       (GCLK_CLKCTRL_ID_I2S_0_Val     << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_ID_I2S_1       (GCLK_CLKCTRL_ID_I2S_1_Val     << GCLK_CLKCTRL_ID_Pos)
#define GCLK_CLKCTRL_GEN_Pos        8            /**< \brief (GCLK_CLKCTRL) Generic Clock Generator */
#define GCLK_CLKCTRL_GEN_Msk        (_U(0xF) << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN(value)     (GCLK_CLKCTRL_GEN_Msk & ((value) << GCLK_CLKCTRL_GEN_Pos))
#define   GCLK_CLKCTRL_GEN_GCLK0_Val      _U(0x0)   /**< \brief (GCLK_CLKCTRL) Generic clock generator 0 */
#define   GCLK_CLKCTRL_GEN_GCLK1_Val      _U(0x1)   /**< \brief (GCLK_CLKCTRL) Generic clock generator 1 */
#define   GCLK_CLKCTRL_GEN_GCLK2_Val      _U(0x2)   /**< \brief (GCLK_CLKCTRL) Generic clock generator 2 */
#define   GCLK_CLKCTRL_GEN_GCLK3_Val      _U(0x3)   /**< \brief (GCLK_CLKCTRL) Generic clock generator 3 */
#define   GCLK_CLKCTRL_GEN_GCLK4_Val      _U(0x4)   /**< \brief (GCLK_CLKCTRL) Generic clock generator 4 */
#define   GCLK_CLKCTRL_GEN_GCLK5_Val      _U(0x5)   /**< \brief (GCLK_CLKCTRL) Generic clock generator 5 */
#define   GCLK_CLKCTRL_GEN_GCLK6_Val      _U(0x6)   /**< \brief (GCLK_CLKCTRL) Generic clock generator 6 */
#define   GCLK_CLKCTRL_GEN_GCLK7_Val      _U(0x7)   /**< \brief (GCLK_CLKCTRL) Generic clock generator 7 */
#define GCLK_CLKCTRL_GEN_GCLK0      (GCLK_CLKCTRL_GEN_GCLK0_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK1      (GCLK_CLKCTRL_GEN_GCLK1_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK2      (GCLK_CLKCTRL_GEN_GCLK2_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK3      (GCLK_CLKCTRL_GEN_GCLK3_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK4      (GCLK_CLKCTRL_GEN_GCLK4_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK5      (GCLK_CLKCTRL_GEN_GCLK5_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK6      (GCLK_CLKCTRL_GEN_GCLK6_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_GEN_GCLK7      (GCLK_CLKCTRL_GEN_GCLK7_Val    << GCLK_CLKCTRL_GEN_Pos)
#define GCLK_CLKCTRL_CLKEN_Pos      14           /**< \brief (GCLK_CLKCTRL) Clock Enable */
#define GCLK_CLKCTRL_CLKEN          (_U(0x1) << GCLK_CLKCTRL_CLKEN_Pos)
#define GCLK_CLKCTRL_WRTLOCK_Pos    15           /**< \brief (GCLK_CLKCTRL) Write Lock */
#define GCLK_CLKCTRL_WRTLOCK        (_U(0x1) << GCLK_CLKCTRL_WRTLOCK_Pos)
#define GCLK_CLKCTRL_MASK           _U(0xCF3F)   /**< \brief (GCLK_CLKCTRL) MASK Register */

/* -------- GCLK_GENCTRL : (GCLK Offset: 0x4) (R/W 32) Generic Clock Generator Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ID:4;             /*!< bit:  0.. 3  Generic Clock Generator Selection  */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t SRC:5;            /*!< bit:  8..12  Source Select                      */
    uint32_t :3;               /*!< bit: 13..15  Reserved                           */
    uint32_t GENEN:1;          /*!< bit:     16  Generic Clock Generator Enable     */
    uint32_t IDC:1;            /*!< bit:     17  Improve Duty Cycle                 */
    uint32_t OOV:1;            /*!< bit:     18  Output Off Value                   */
    uint32_t OE:1;             /*!< bit:     19  Output Enable                      */
    uint32_t DIVSEL:1;         /*!< bit:     20  Divide Selection                   */
    uint32_t RUNSTDBY:1;       /*!< bit:     21  Run in Standby                     */
    uint32_t :10;              /*!< bit: 22..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_GENCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_GENCTRL_OFFSET         0x4          /**< \brief (GCLK_GENCTRL offset) Generic Clock Generator Control */
#define GCLK_GENCTRL_RESETVALUE     _U(0x00000000) /**< \brief (GCLK_GENCTRL reset_value) Generic Clock Generator Control */

#define GCLK_GENCTRL_ID_Pos         0            /**< \brief (GCLK_GENCTRL) Generic Clock Generator Selection */
#define GCLK_GENCTRL_ID_Msk         (_U(0xF) << GCLK_GENCTRL_ID_Pos)
#define GCLK_GENCTRL_ID(value)      (GCLK_GENCTRL_ID_Msk & ((value) << GCLK_GENCTRL_ID_Pos))
#define GCLK_GENCTRL_SRC_Pos        8            /**< \brief (GCLK_GENCTRL) Source Select */
#define GCLK_GENCTRL_SRC_Msk        (_U(0x1F) << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC(value)     (GCLK_GENCTRL_SRC_Msk & ((value) << GCLK_GENCTRL_SRC_Pos))
#define   GCLK_GENCTRL_SRC_XOSC_Val       _U(0x0)   /**< \brief (GCLK_GENCTRL) XOSC oscillator output */
#define   GCLK_GENCTRL_SRC_GCLKIN_Val     _U(0x1)   /**< \brief (GCLK_GENCTRL) Generator input pad */
#define   GCLK_GENCTRL_SRC_GCLKGEN1_Val   _U(0x2)   /**< \brief (GCLK_GENCTRL) Generic clock generator 1 output */
#define   GCLK_GENCTRL_SRC_OSCULP32K_Val  _U(0x3)   /**< \brief (GCLK_GENCTRL) OSCULP32K oscillator output */
#define   GCLK_GENCTRL_SRC_OSC32K_Val     _U(0x4)   /**< \brief (GCLK_GENCTRL) OSC32K oscillator output */
#define   GCLK_GENCTRL_SRC_XOSC32K_Val    _U(0x5)   /**< \brief (GCLK_GENCTRL) XOSC32K oscillator output */
#define   GCLK_GENCTRL_SRC_OSC8M_Val      _U(0x6)   /**< \brief (GCLK_GENCTRL) OSC8M oscillator output */
#define   GCLK_GENCTRL_SRC_DFLL48M_Val    _U(0x7)   /**< \brief (GCLK_GENCTRL) DFLL48M output */
#define   GCLK_GENCTRL_SRC_FDPLL_Val      _U(0x8)   /**< \brief (GCLK_GENCTRL) FDPLL output */
#define GCLK_GENCTRL_SRC_XOSC       (GCLK_GENCTRL_SRC_XOSC_Val     << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_GCLKIN     (GCLK_GENCTRL_SRC_GCLKIN_Val   << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_GCLKGEN1   (GCLK_GENCTRL_SRC_GCLKGEN1_Val << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSCULP32K  (GCLK_GENCTRL_SRC_OSCULP32K_Val << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSC32K     (GCLK_GENCTRL_SRC_OSC32K_Val   << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_XOSC32K    (GCLK_GENCTRL_SRC_XOSC32K_Val  << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_OSC8M      (GCLK_GENCTRL_SRC_OSC8M_Val    << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_DFLL48M    (GCLK_GENCTRL_SRC_DFLL48M_Val  << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_SRC_FDPLL      (GCLK_GENCTRL_SRC_FDPLL_Val    << GCLK_GENCTRL_SRC_Pos)
#define GCLK_GENCTRL_GENEN_Pos      16           /**< \brief (GCLK_GENCTRL) Generic Clock Generator Enable */
#define GCLK_GENCTRL_GENEN          (_U(0x1) << GCLK_GENCTRL_GENEN_Pos)
#define GCLK_GENCTRL_IDC_Pos        17           /**< \brief (GCLK_GENCTRL) Improve Duty Cycle */
#define GCLK_GENCTRL_IDC            (_U(0x1) << GCLK_GENCTRL_IDC_Pos)
#define GCLK_GENCTRL_OOV_Pos        18           /**< \brief (GCLK_GENCTRL) Output Off Value */
#define GCLK_GENCTRL_OOV            (_U(0x1) << GCLK_GENCTRL_OOV_Pos)
#define GCLK_GENCTRL_OE_Pos         19           /**< \brief (GCLK_GENCTRL) Output Enable */
#define GCLK_GENCTRL_OE             (_U(0x1) << GCLK_GENCTRL_OE_Pos)
#define GCLK_GENCTRL_DIVSEL_Pos     20           /**< \brief (GCLK_GENCTRL) Divide Selection */
#define GCLK_GENCTRL_DIVSEL         (_U(0x1) << GCLK_GENCTRL_DIVSEL_Pos)
#define GCLK_GENCTRL_RUNSTDBY_Pos   21           /**< \brief (GCLK_GENCTRL) Run in Standby */
#define GCLK_GENCTRL_RUNSTDBY       (_U(0x1) << GCLK_GENCTRL_RUNSTDBY_Pos)
#define GCLK_GENCTRL_MASK           _U(0x003F1F0F) /**< \brief (GCLK_GENCTRL) MASK Register */

/* -------- GCLK_GENDIV : (GCLK Offset: 0x8) (R/W 32) Generic Clock Generator Division -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t ID:4;             /*!< bit:  0.. 3  Generic Clock Generator Selection  */
    uint32_t :4;               /*!< bit:  4.. 7  Reserved                           */
    uint32_t DIV:16;           /*!< bit:  8..23  Division Factor                    */
    uint32_t :8;               /*!< bit: 24..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} GCLK_GENDIV_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define GCLK_GENDIV_OFFSET          0x8          /**< \brief (GCLK_GENDIV offset) Generic Clock Generator Division */
#define GCLK_GENDIV_RESETVALUE      _U(0x00000000) /**< \brief (GCLK_GENDIV reset_value) Generic Clock Generator Division */

#define GCLK_GENDIV_ID_Pos          0            /**< \brief (GCLK_GENDIV) Generic Clock Generator Selection */
#define GCLK_GENDIV_ID_Msk          (_U(0xF) << GCLK_GENDIV_ID_Pos)
#define GCLK_GENDIV_ID(value)       (GCLK_GENDIV_ID_Msk & ((value) << GCLK_GENDIV_ID_Pos))
#define GCLK_GENDIV_DIV_Pos         8            /**< \brief (GCLK_GENDIV) Division Factor */
#define GCLK_GENDIV_DIV_Msk         (_U(0xFFFF) << GCLK_GENDIV_DIV_Pos)
#define GCLK_GENDIV_DIV(value)      (GCLK_GENDIV_DIV_Msk & ((value) << GCLK_GENDIV_DIV_Pos))
#define GCLK_GENDIV_MASK            _U(0x00FFFF0F) /**< \brief (GCLK_GENDIV) MASK Register */

/** \brief GCLK hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO GCLK_CTRL_Type            CTRL;        /**< \brief Offset: 0x0 (R/W  8) Control */
  __I  GCLK_STATUS_Type          STATUS;      /**< \brief Offset: 0x1 (R/   8) Status */
  __IO GCLK_CLKCTRL_Type         CLKCTRL;     /**< \brief Offset: 0x2 (R/W 16) Generic Clock Control */
  __IO GCLK_GENCTRL_Type         GENCTRL;     /**< \brief Offset: 0x4 (R/W 32) Generic Clock Generator Control */
  __IO GCLK_GENDIV_Type          GENDIV;      /**< \brief Offset: 0x8 (R/W 32) Generic Clock Generator Division */
} Gclk;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD21_GCLK_COMPONENT_ */
